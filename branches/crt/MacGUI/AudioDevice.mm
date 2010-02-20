/*
 * (C) 2008 Jérôme Lang. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#import "MyDocument.h"

#define BUFFERSIZE 2048

#define CHECK_ERROR(ERRNO, RESULT) \
	if (RESULT != kAudioHardwareNoError) \
	{ \
		lastError = ERRNO; \
		return nil; \
	}

	
#define SET_PROPS() \
	if (AudioDeviceSetProperty (mySoundDeviceID, NULL, 0, 0, \
								kAudioDevicePropertyStreamFormat, \
								myPropertySize, &mySoundBasicDescription)) \
	{ \
		CHECK_ERROR \
		( \
			MPERR_OSX_BAD_PROPERTY, \
			AudioDeviceGetProperty (mySoundDeviceID, 0, 0, \
									kAudioDevicePropertyStreamFormat, \
									&myPropertySize, &mySoundBasicDescription) \
		); \
	}

// my sound IO proc
static OSStatus OSX_AudioIOProc16Bit(AudioDeviceID inDevice,
							   const AudioTimeStamp* inNow,
							   const AudioBufferList* inInputData,
							   const AudioTimeStamp* inInputTime,
							   AudioBufferList* outOutputData, 
							   const AudioTimeStamp* inOutputTime,
							   void *inClientData)
{
    register float*	myOutBuffer = (float*)outOutputData->mBuffers[0].mData;
    register UInt32 size = BUFFERSIZE;

	// cast void pointer to SID*
	SID* sid = reinterpret_cast<SID*>(inClientData);

	// get samples from SID
	sid->mix(myOutBuffer, size);
	
    return noErr;
}


@implementation AudioDevice

- (id)initWithSID:(SID *)sid
{
	self = [super init];
    if (self) 
	{	
		AudioStreamBasicDescription mySoundBasicDescription;
		UInt32						myPropertySize, myBufferByteCount;
		
		if (ioProcIsInstalled == 1)
			return self;
			
		// get the device...
		myPropertySize = sizeof (mySoundDeviceID);
		CHECK_ERROR
			(
			 MPERR_DETECTING_DEVICE,
			 AudioHardwareGetProperty (kAudioHardwarePropertyDefaultOutputDevice,
									   &myPropertySize, &mySoundDeviceID)
			 );
		
		if (mySoundDeviceID == kAudioDeviceUnknown)
		{
			lastError = MPERR_OSX_UNKNOWN_DEVICE;
			return nil;
		}
		
		// get the device format...
		myPropertySize = sizeof (mySoundBasicDescription);
		CHECK_ERROR
			(
			 MPERR_OSX_BAD_PROPERTY,
			 AudioDeviceGetProperty (mySoundDeviceID, 0, 0, kAudioDevicePropertyStreamFormat,
									 &myPropertySize, &mySoundBasicDescription)
			 );
		
		// try the selected mix frequency, if failure, fall back to native frequency and ajust SID's samplerate...
		if (mySoundBasicDescription.mSampleRate != sid->getSamplerate())
		{
			// try adjusting changing samplerate to wanted samplerate
			mySoundBasicDescription.mSampleRate = sid->getSamplerate();
			SET_PROPS ();
			// samplerate couldn't be changed
			if (mySoundBasicDescription.mSampleRate != sid->getSamplerate())
			{
				sid->setSamplerate(mySoundBasicDescription.mSampleRate); // adjust SID's samplerate to native frequency of hardware
				printf("Samplerate of SID was changed to native frequency of used audio hardware.");
			}
		}
		
		// try stereo, if failure try to select mono - if failure again cancel audio support of C64
		if (mySoundBasicDescription.mChannelsPerFrame != 2)
		{
			// change channels to stereo
			mySoundBasicDescription.mChannelsPerFrame = 2;
			SET_PROPS();
			
			// stereo doesn't seem to be supported
			if (mySoundBasicDescription.mChannelsPerFrame != 2) // stereo not supported
			{
				mySoundBasicDescription.mChannelsPerFrame = 1;
				SET_PROPS();
				if (mySoundBasicDescription.mChannelsPerFrame != 1) // mono not supported
				{
					return nil;
				}
				else 
				{
					bufferMono = true;
					sid->setMono();
				}
			}
		}
				
		// linear PCM is required...
		if (mySoundBasicDescription.mFormatID != kAudioFormatLinearPCM)
		{
			lastError = MPERR_OSX_UNSUPPORTED_FORMAT;
			return nil;
		}
		
		// prepare the buffers...
		if (bufferMono)
		{
			inBufferSize = BUFFERSIZE;
		}
		else // interleaved buffer
		{
			inBufferSize = BUFFERSIZE << 1;
		}
		
		myBufferByteCount = inBufferSize * sizeof(float);
		CHECK_ERROR
			(
			 MPERR_OSX_BUFFER_ALLOC,
			 AudioDeviceSetProperty (mySoundDeviceID, NULL, 0, 0, kAudioDevicePropertyBufferSize,
									 sizeof(myBufferByteCount), &myBufferByteCount)
			 );


		// add our audio IO procedure....
		// the optional void pointer is used in order to submit the SID to the callback
		CHECK_ERROR
			(
			 MPERR_OSX_ADD_IO_PROC,
			 AudioDeviceCreateIOProcID(mySoundDeviceID, OSX_AudioIOProc16Bit, sid, &mySoundIOProcID)
			 );
		
		// callback successfully started
		ioProcIsInstalled = 1;
	}
	else 
	{
		return nil;
	}
	return self;
}

-(int)startPlayback
{
	if (AudioDeviceStart (mySoundDeviceID, mySoundIOProcID))
	{
		lastError = MPERR_OSX_DEVICE_START;
		return lastError;
	}
	return 0;
}

-(void)stopPlayback
{
	// stop playing
	AudioDeviceStop (mySoundDeviceID, mySoundIOProcID);
	return;
}

-(void)dealloc
{
	// if callback has been properly started before
	if (ioProcIsInstalled)
	{
		// stop playing first
		AudioDeviceStop (mySoundDeviceID, mySoundIOProcID);
		// remove callback
		AudioDeviceDestroyIOProcID( mySoundDeviceID, mySoundIOProcID );
		ioProcIsInstalled = 0;
	}
	[super dealloc];
}



@end
