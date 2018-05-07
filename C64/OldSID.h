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
#include "FastSid.h"
#include "SIDbase.h"
#include "resid/sid.h"

//! The virtual sound interface device (SID)
/*! SID is the sound chip of the Commodore 64.
	The SID chip occupied the memory mapped I/O space from address 0xD400 to 0xD7FF. 
*/
class OldSID : public SIDbase {

private:
    
    // Fast SID state
    sound_s st;

public:
    
	//! Constructor.
	OldSID();
	
	//! Destructor.
	~OldSID();
	
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
    bool getAudioFilter() { return st.emulatefilter; }
    
    //! Enable or disable reSIDs audio filtering capability
    void setAudioFilter(bool enable) { st.emulatefilter = enable; }
    
    //! Return samplerate.
    uint32_t getSampleRate() { return 0; } // TODO
    
    //! Set sample rate
    void setSampleRate(uint32_t sr) { } // TODO
    
    //! Get sampling method
    reSID::sampling_method getSamplingMethod() { return reSID::SAMPLE_FAST; }
    
    //! Set sampling method
    void setSamplingMethod(reSID::sampling_method value) { }
    
    //! Get chip model
    reSID::chip_model getChipModel() {
        return st.newsid ? reSID::chip_model::MOS8580 : reSID::chip_model::MOS6581; }
    
    //! Set chip model
    void setChipModel(reSID::chip_model value) {
        st.newsid = (value == reSID::chip_model::MOS8580);
    }
    
    //! Get clock frequency
    uint32_t getClockFrequency() { return 0; }
    
    //! Set clock frequency
    void setClockFrequency(uint32_t f) { };
};

#endif
