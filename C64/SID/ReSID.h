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
#include "resid/sid.h"

class ReSID : public VirtualComponent {

public:
    
    // ReSID object
    reSID::SID *sid;
    
public:
    
    void clock() { sid->clock(); }
    
private:
    
    //! ReSID state
    reSID::SID::State st;
    
    //! @brief   The emulated chip model
    SIDChipModel chipModel;
    
    //! @brief   Clock frequency
    /*! @details Either PAL_CYCLES_PER_SECOND or NTSC_CYCLES_PER_SECOND
     */
    uint32_t clockFrequency;
    
    //! @brief   Sample rate (usually set to 44.1 kHz)
    uint32_t sampleRate;
    
    //! @brief   Sampling method
    SamplingMethod samplingMethod;
    
    //! @brief   Switches filter emulation on or off.
    bool emulateFilter;
    
public:
		
    //! Pointer to bridge object
    SIDBridge *bridge;
    
	//! Constructor.
	ReSID();
	
	//! Destructor.
	~ReSID();
	
	//! Method from VirtualComponent
	void reset();
	
    //! Load state
	void loadFromBuffer(uint8_t **buffer);

    //! Save state
    void saveToBuffer(uint8_t **buffer);
	
    //! @brief    Gathers all values that are displayed in the debugger
    SIDInfo getInfo();
    
    //! @brief    Gathers all debug information for a specific voice
    VoiceInfo getVoiceInfo(unsigned voice);
    VoiceInfo getVoiceInfo(unsigned voice, reSID::SID::State *state);

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
    
    //! Returns the chip model
    SIDChipModel getChipModel() {
        assert((SIDChipModel)sid->sid_model == chipModel);
        return chipModel;
    }
    
    //! Sets the chip model
    void setChipModel(SIDChipModel value);
    
    //! Returns the clock frequency
    uint32_t getClockFrequency() {
        assert((uint32_t)sid->clock_frequency == clockFrequency);
        return (uint32_t)sid->clock_frequency;
    }
    
    //! Sets the clock frequency
    void setClockFrequency(uint32_t frequency);
    
    //! Returns the sample rate
    uint32_t getSampleRate() { return sampleRate; }
    
    //! Sets the sample rate
    void setSampleRate(uint32_t rate);
    
    //! Returns true iff audio filters should be emulated.
    bool getAudioFilter() { return emulateFilter; }
    
    //! Enable or disable audio filter emulation
	void setAudioFilter(bool enable);

    //! Get sampling method
    SamplingMethod getSamplingMethod() {
        assert((SamplingMethod)sid->sampling == samplingMethod);
        return samplingMethod;
    }
    
    //! Set sampling method
    void setSamplingMethod(SamplingMethod value);
};

#endif
