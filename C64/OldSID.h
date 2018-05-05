/*
 * (C) 2006 Dirk W. Hoffmann, Jérôme Lang. All rights reserved.
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

#ifndef _OLDSID_INC
#define _OLDSID_INC

#include "VirtualComponent.h"

//! The virtual sound interface device (SID)
/*! SID is the sound chip of the Commodore 64.
	The SID chip occupied the memory mapped I/O space from address 0xD400 to 0xD7FF. 
*/
class OldSID : public VirtualComponent {

public:
    
	//! Constructor.
	OldSID();
	
	//! Destructor.
	~OldSID();
	
	//! Bring the SID chip back to it's initial state.
	void reset();
	
    //! Size of internal state
    size_t stateSize();

	//! Load state
	void loadFromBuffer(uint8_t **buffer);
	
	//! Save state
	void saveToBuffer(uint8_t **buffer);	
	
	//! Dump internal state to console
	void dumpState();
	
	//! Special peek function for the I/O memory range
	uint8_t peek(uint16_t addr);
	
	//! Special poke function for the I/O memory range
	void poke(uint16_t addr, uint8_t value);
	
	//! Execute SID
	void execute(uint64_t cycles);
	
    //! Notifies the SID chip that the emulator has started
    void run();
	
	//! Notifies the SID chip that the emulator has started
	void halt();

    //! @brief  Reads one audio sample from ringbuffer
	float readData();
	
    //! @brief  Reads n mono sample from ringbuffer
    void readMonoSamples(float *target, size_t n);
    
    //! @brief  Reads n mono sample from ringbuffer
    void readStereoSamples(float *target1, float *target2, size_t n);
    
    //! @brief  Reads n mono sample from ringbuffer
    void readStereoSamplesInterleaved(float *target, size_t n);
    
	//! Enable or disable filters of SID.
	/*!
		By default filters of SID are disabled.
		\param enable True to switch filters on, else false.
	*/
    void setAudioFilter(bool enable);

	//! Return volumeControl value
    float getVolumeControl();
	
	//! Set volume control value
    void setVolumeControl(float value);
		
	//! Sets samplerate of SID and it's 3 voices.
    void setSampleRate(uint32_t sr);
	
	//! Returns samplerate of SID.
    uint32_t getSampleRate(); 
	
	//! Set clock frequency
    void setClockFrequency(uint32_t frequency);
};

#endif
