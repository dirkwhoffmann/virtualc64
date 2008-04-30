/*
 * (C) 2006-2008 Jérôme Lang. All rights reserved.
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
 
 
#ifndef _SIDDEVICE_INC
#define _SIDDEVICE_INC

#include <CoreAudio/AudioHardware.h>
#include "SID.h"

class SIDDevice
{
private:

	//! the AudioDevice
	AudioDeviceID	SoundDeviceID;
	
	//! the identifier passed back when creating/registering the IOProc. 
	AudioDeviceIOProcID mySoundProcID;
	
	//! buffersize - size of transmitted audio data 
	UInt32			InBufferSize;
	
	//! holds last error
	int				lastError;
	
	//! true if callback is installed
	bool			IOProcIsInstalled;
	
	//! true if audio hardware only supports mono playback
	bool BufferMono;
	
	
	
public:
	//! constructor
	SIDDevice();
	
	//! destructor
	~SIDDevice();
	
	//! initialized the audiohardware
	/*!
		Supported are stereo und mono audio devices.
		If the selected samplerate of SID isn't supported by hardware 
		the samplerate of SID will be changed to native frequency of audio hardware
		\param sid pointer to SID instance generating the audio data for us
	*/
	int SetupDevice(SID* sid);
	
	//! starts playback (callback mechanism) of generated sound 
	int StartPlaying();
	
	//! stops playback of generated sound
	int StopPlaying();
	
	//! stops playback and closes audio device if it was opened before
	int FreeDevice();
	
	//! the actual callback
	/*!
		calls periodicaly mix() of SID in order to get audio samples
		\param inDevice  ignored
		\param inNow  ignored
		\param inInputData  ignored
		\param inInputTime  ignored
		\param outOutputData  contains pointer to buffer that must be filled with samples
		\param inClientData pointer to SID from which we get audio samples
	*/
	static OSStatus OSX_AudioIOProc16Bit(AudioDeviceID inDevice, 
							   const AudioTimeStamp* inNow,
							   const AudioBufferList* inInputData,
							   const AudioTimeStamp* inInputTime,
							   AudioBufferList* outOutputData, 
							   const AudioTimeStamp* inOutputTime,
							   void *inClientData);
	
};



#endif