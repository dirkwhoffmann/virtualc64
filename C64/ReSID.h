/*!
 * @header      ReSID.h
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   2011 - 2015 Dirk W. Hoffmann
 * @brief       Declares ReSID wrapper class
 */
/* This program is free software; you can redistribute it and/or modify
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
#include "SIDbase.h"
#include "resid/sid.h"

class ReSID : public SIDbase {

private:
    
    reSID::SID *sid;
    
    //! ReSID state
    reSID::SID::State st;
    
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
	
	/*! @brief   Execute SID
     *  @details Runs reSID for the specified amount of CPU cycles and writes
     *           the generated sound samples into the internal ring buffer. 
     */
    void execute(uint64_t cycles);
	

    // Configuring
    
    //! Returns true iff audio filters are enabled.
    bool getAudioFilter() { return audioFilter; }

    //! Returns true iff external audio filters are enabled.
    bool getExternalAudioFilter() { return externalAudioFilter; }

	//! Enable or disable reSIDs audio filtering capability
	void setAudioFilter(bool enable);

    //! Enable or disable reSIDs external audio filtering capability
    void setExternalAudioFilter(bool enable);

    //! Return samplerate.
    uint32_t getSampleRate() { return sampleRate; }
    
	//! Set sample rate 
	void setSampleRate(uint32_t sr);
    
    //! Get sampling method
    reSID::sampling_method getSamplingMethod() { return samplingMethod; }
    
    //! Set sampling method
    void setSamplingMethod(reSID::sampling_method value);
    
    //! Get chip model 
    reSID::chip_model getChipModel() { return chipModel; }
    
    //! Set chip model 
    void setChipModel(reSID::chip_model value);
    
    //! Get clock frequency
    uint32_t getClockFrequency() { return cpuFrequency; }
    
	//! Set clock frequency
    void setClockFrequency(uint32_t f);

};

#endif
