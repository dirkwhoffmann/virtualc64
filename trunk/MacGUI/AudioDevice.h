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

#import <Cocoa/Cocoa.h>
#import <CoreAudio/AudioHardware.h>
#import <AudioToolbox/AudioServices.h>

class SID;

#define MPERR_DETECTING_DEVICE			-1
#define MPERR_OSX_UNKNOWN_DEVICE		-2
#define MPERR_OSX_BAD_PROPERTY			-3
#define MPERR_OSX_UNSUPPORTED_FORMAT	-4
#define MPERR_OSX_BUFFER_ALLOC			-5
#define MPERR_OSX_ADD_IO_PROC			-6
#define MPERR_OSX_DEVICE_START			-7

@interface AudioDevice : NSObject {

@private
	//! the AudioDevice
	AudioDeviceID	mySoundDeviceID;
	
	//! the identifier passed back when creating/registering the IOProc. 
	AudioDeviceIOProcID mySoundIOProcID;
	
	//! buffersize - size of transmitted audio data 
	UInt32			inBufferSize;
	
	//! holds last error
	int				lastError;
	
	//! true if callback is installed
	bool			ioProcIsInstalled;
	
	//! true if audio hardware only supports mono playback
	bool			bufferMono;
}

//! initializes the audio hardware
/*!
	Supported are stereo und mono audio devices.
	If the selected samplerate of SID isn't supported by hardware 
	the samplerate of SID will be changed to native frequency of audio hardware
	\param sid pointer to SID instance generating the audio data for us
*/
-(id)initWithSID:(SID *)sid;

//! starts playback (callback mechanism) of generated sound
/*! 
	\return MPERR_OSX_DEVICE_START if playback couldn't be started, else 0
*/
-(int)startPlayback;
	
//! stops playback of generated sound
-(void)stopPlayback;

//! closes audio device (if opened before)
-(void)dealloc;



@end
