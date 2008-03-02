/*
 * (C) 2006 Jérôme Lang. All rights reserved.
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


#include "SIDDevice.h"

#define BUFFERSIZE 2048


#define CHECK_ERROR(ERRNO, RESULT) \
	if (RESULT != kAudioHardwareNoError) \
	{ \
		lastError = ERRNO; \
		return -1; \
	}
	
#define SET_PROPS() \
	if (AudioDeviceSetProperty (SoundDeviceID, NULL, 0, 0, \
								kAudioDevicePropertyStreamFormat, \
								myPropertySize, &mySoundBasicDescription)) \
	{ \
		CHECK_ERROR \
		( \
			MPERR_OSX_BAD_PROPERTY, \
			AudioDeviceGetProperty (SoundDeviceID, 0, 0, \
									kAudioDevicePropertyStreamFormat, \
									&myPropertySize, &mySoundBasicDescription) \
		); \
	}

#define MPERR_DETECTING_DEVICE			-1
#define MPERR_OSX_UNKNOWN_DEVICE		-2
#define MPERR_OSX_BAD_PROPERTY			-3
#define MPERR_OSX_UNSUPPORTED_FORMAT	-4
#define MPERR_OSX_BUFFER_ALLOC			-5
#define MPERR_OSX_ADD_IO_PROC			-6
#define MPERR_OSX_DEVICE_START			-7



OSStatus SIDDevice::OSX_AudioIOProc16Bit(AudioDeviceID inDevice, 
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


SIDDevice::SIDDevice() : lastError(0), InBufferSize(0), IOProcIsInstalled(0), BufferMono(0)
{
}


int SIDDevice::SetupDevice(SID* sid)
	{
		AudioStreamBasicDescription mySoundBasicDescription;
		UInt32						myPropertySize, myBufferByteCount;
		
		
		// get the device...
		myPropertySize = sizeof (SoundDeviceID);
		CHECK_ERROR
			(
			 MPERR_DETECTING_DEVICE,
			 AudioHardwareGetProperty (kAudioHardwarePropertyDefaultOutputDevice,
									   &myPropertySize, &SoundDeviceID)
			 );
		
		if (SoundDeviceID == kAudioDeviceUnknown)
		{
			lastError = MPERR_OSX_UNKNOWN_DEVICE;
			return lastError;
		}
		
		// get the device format...
		myPropertySize = sizeof (mySoundBasicDescription);
		CHECK_ERROR
			(
			 MPERR_OSX_BAD_PROPERTY,
			 AudioDeviceGetProperty (SoundDeviceID, 0, 0, kAudioDevicePropertyStreamFormat,
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
				debug("Samplerate of SID was changed to native frequency of used audio hardware.\n");
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
					return -2;
				}
				else 
				{
					BufferMono = true;
					sid->setMono();
				}
			}
		}
				
		// linear PCM is required...
		if (mySoundBasicDescription.mFormatID != kAudioFormatLinearPCM)
		{
			lastError = MPERR_OSX_UNSUPPORTED_FORMAT;
			return lastError;
		}
		
		// prepare the buffers...
		if (BufferMono)
		{
			InBufferSize = BUFFERSIZE;
		}
		else // interleaved buffer
		{
			InBufferSize = BUFFERSIZE << 1;
		}
		
		myBufferByteCount = InBufferSize * sizeof(float);
		CHECK_ERROR
			(
			 MPERR_OSX_BUFFER_ALLOC,
			 AudioDeviceSetProperty (SoundDeviceID, NULL, 0, 0, kAudioDevicePropertyBufferSize,
									 sizeof(myBufferByteCount), &myBufferByteCount)
			 );


		// add our audio IO procedure....
		CHECK_ERROR
			(
			 MPERR_OSX_ADD_IO_PROC,
			 AudioDeviceAddIOProc (SoundDeviceID, OSX_AudioIOProc16Bit, sid)// over the optional void pointer we submit the SID to the callback
			 );
		
		// start the audio IO Proc... => start playing
		if (AudioDeviceStart (SoundDeviceID, OSX_AudioIOProc16Bit))
		{
			lastError = MPERR_OSX_DEVICE_START;
			return lastError;
		}
		// callback successfully started
		IOProcIsInstalled = 1;
		
		return 0;
	}
	
	int SIDDevice::FreeDevice(void)
	{
		// if callback has been properly started before
		if (IOProcIsInstalled)
		{
			// stop playing first
			AudioDeviceStop (SoundDeviceID, OSX_AudioIOProc16Bit);
			// remove callback
			AudioDeviceRemoveIOProc (SoundDeviceID, OSX_AudioIOProc16Bit);
		}
		
		return 0;
	}
	
	SIDDevice::~SIDDevice()
	{
		this->FreeDevice();
	}
