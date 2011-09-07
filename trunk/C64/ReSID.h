/*
 * (C) 2011 Dirk W. Hoffmann. All rights reserved.
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

#ifndef _RESID_INC
#define _RESID_INC

#include "VirtualComponent.h"
#include "sid.h"

class ReSID : public VirtualComponent {
		
private:
    SID *sid;
    	
	//! Samplerate of SID.
	uint32_t samplerate;
	
	//! CPU frequency of Emulation (NTSC or PAL).
	uint32_t cpuFrequency;
		
	//! Size of \a ringBuffer.
	uint32_t bufferSize;
	
	//!  Ringbuffer with sound data.
	float* ringBuffer;
	
	//! Pointer to write position in buffer.
	float* writeBuffer;
	
	//! Pointer to read position in buffer.
	float* readBuffer;
	
	//! Pointer to end of buffer.
	float* endBuffer;
			
	//! True if filters are enabled.
	// bool filtersEnabled;

public:
		
	//! Constructor.
	ReSID();
	
	//! Destructor.
	~ReSID();
	
	//! Bring the SID chip back to it's initial state.
	void reset();
	
	//! Load state
	void loadFromBuffer(uint8_t **buffer);
	
	//! Save state
	void saveToBuffer(uint8_t **buffer);	
	
	//! Dump internal state to console
	void dumpState();
	
	//! Special peek function for the I/O memory range.
	uint8_t peek(uint16_t addr);
	
	//! Special poke function for the I/O memory range.
	void poke(uint16_t addr, uint8_t value);
	
	//! Pass control to the SID chip.
	/*! The SID will be executed and generate audio samples for about one video frame.
		Actual number of generated samples depends on executed CPU cycles since last call.
		\param cycles Number of cycles to execute (ignored).
	*/
	bool execute(int cycles);
	
    //! Get next sample from \a ringBuffer.
	float readData();
	
    //! Invoked on buffer underflow or overflow
    void handleBufferException();
    
	//! Write sample to \a ringBuffer.
	void writeData(float data);

	//! Enable or disable filters of SID.
	void enableFilters(bool enable);
		
	//! Set sample rate 
	void setSampleRate(uint32_t sr);
	
	//! Returns samplerate of SID.
	uint32_t getSampleRate() {return this->samplerate;}
	
	//! Set clock frequency
	void setClockFrequency(uint32_t frequency);	
};

#endif
