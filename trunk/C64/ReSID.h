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

// Good candidate for testing sound emulation: INTERNAT.P00

#ifndef _RESID_INC
#define _RESID_INC

#include "VirtualComponent.h"
#include "sid.h"

class ReSID : public VirtualComponent {
		
private:
    SID *sid;
    	
	//! Sample rate.
	uint32_t sampleRate;
	
    //! Audio filters.
    bool audioFilter;
    
    //! Sampling method.
    sampling_method samplingMethod;

    //! Chip model.
    chip_model chipModel;
    
	//! CPU frequency.
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

    // Configuring
    
    //! Returns true iff audio filters are enabled.
	inline bool getAudioFilter() { return audioFilter; }

	//! Enable or disable filters of SID.
	void setAudioFilter(bool enable);

    //! Return samplerate.
	inline uint32_t getSampleRate() { return sampleRate; }

	//! Set sample rate 
	void setSampleRate(uint32_t sr);

    //! Get sampling method
    inline sampling_method getSamplingMethod() { return samplingMethod; }

    //! Set sampling method
    void setSamplingMethod(sampling_method value);

    //! Get chip model 
    inline chip_model getChipModel() { return chipModel; }

    //! Set chip model 
    void setChipModel(chip_model value);

    //! Get clock frequency
	uint32_t getClockFrequency();	

	//! Set clock frequency
	void setClockFrequency(uint32_t frequency);	
};

#endif
