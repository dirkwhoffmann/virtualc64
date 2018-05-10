/*
 * This file belongs to the FastSID implementation of VirtualC64,
 * an adaption of the code used in VICE 3.1, the Versatile Commodore Emulator.
 *
 * Original code written by
 *  Teemu Rantanen <tvr@cs.hut.fi>
 *
 * Adapted for VirtualC64 by
 *  Dirk Hoffmann
 */
/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *  02111-1307  USA.
 *
 */

#ifndef _FASTSID_INC
#define _FASTSID_INC

#include "VirtualComponent.h"
#include "FastSidVoice.h"
#include "SIDbase.h"
#include "../resid/sid.h"

/* needed data for SID */
struct sound_s {
    
    /* SID registers */
    uint8_t d[32];
    
    /* internal constant used for sample rate dependent calculations */
    uint32_t speed1;
    
    /* constants needed to implement write-only register reads */
    uint8_t laststore;
};

typedef struct sound_s sound_t;


//! The virtual sound interface device (SID)
/*! SID is the sound chip of the Commodore 64.
	The SID chip occupied the memory mapped I/O space from address 0xD400 to 0xD7FF. 
*/
class FastSID : public SIDbase {

public:
    
    //! @brief   The three SID voices
    Voice voice[3];
    
    //! @brief   ADSR counter step lookup table
    int32_t adrs[16];
    
    //! @brief   Sustain comparison values loopup table
    uint32_t sz[16];
    
    // Fast SID state
    sound_s st;

    private:
    
    //! @brief   Low pass filter lookup table
    /*! @details Needs to be updated when the sample rate changes
     */
    float lowPassParam[0x800];

    //! @brief   Band pass filter lookup table
    /*! @details Needs to be updated when the sample rate changes
     */
    float bandPassParam[0x800];
    
    //! @brief   Filter resonance lookup table
    /*! @details Needs to be updated when the sample rate changes
     */
    float filterResTable[16];
    
    //! @brief   Amplifier lookup table
    signed char ampMod1x8[256];
    
public:
    
	//! Constructor.
	FastSID();
	
	//! Destructor.
	~FastSID();
	
    //! Bring the SID chip back to it's initial state.
    void reset();
    
    //! Load state
    void loadFromBuffer(uint8_t **buffer);
    
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
    
    //! Return samplerate.
    uint32_t getSampleRate() { return 0; } // TODO
    
    //! Set sample rate
    void setSampleRate(uint32_t sr) { } // TODO
    
    //! Get sampling method
    reSID::sampling_method getSamplingMethod() { return reSID::SAMPLE_FAST; }
    
    //! Set sampling method
    void setSamplingMethod(reSID::sampling_method value) { }
    
    //! Get chip model
    SIDChipModel getChipModel() { return chipModel; /* TODO: Update structures? */ }
    
    //! Set chip model
    void setChipModel(SIDChipModel value) { chipModel = value; /* TODO: Update structures? */ }
    
    //! Get clock frequency
    uint32_t getClockFrequency() { return 0; }
    
    //! Set clock frequency
    void setClockFrequency(uint32_t f) { };
    
private:
    
    //! @brief   Initializes SID
    int init(int sampleRate, int cycles_per_sec);
    
    //! @brief   Initializes filter lookup tables
    void initFilter(int sampleRate);
    
    
    //
    // Querying configuration items
    //
    
    //! @brief   Returns the currently set SID volume
    uint8_t sidVolume() { return st.d[0x18] & 0x0F; }
    
    //! @brief    Returns true iff voice 3 is disconnected from the audio output
    /*! @details  Setting voice 3 to bypass the filter (FILT3 = 0) and setting
     *            bit 7 in the Mod/Vol register to one prevents voice 3 from
     *            reaching the audio output.
     */
    bool voiceThreeDisconnected() { return filterOff(2) && (st.d[0x18] & 0x80); }
    
    // Filter related configuration items
    
    //! @brief   Returns the filter cutoff frequency (11 bit value)
    uint16_t filterCutoff() { return (st.d[0x16] << 3) | (st.d[0x15] & 0x07); }

    //! @brief    Returns the filter resonance (4 bit value)
    uint8_t filterResonance() { return st.d[0x17] >> 4; }

    //! @brief    Returns true iff the specified voice schould be filtered
    bool filterOn(unsigned voice) { return GET_BIT(st.d[0x17], voice) != 0; }

    //! @brief    Returns true iff the specified voice schould not be filtered
    bool filterOff(unsigned voice) { return GET_BIT(st.d[0x17], voice) == 0; }

    //! @brief    Returns true iff the EXTERNAL filter bit is set
    bool filterExtBit() { return st.d[0x17] & 0x08; }

    //! @brief   Returns the currently set filter type
    uint8_t filterType() { return st.d[0x18] & 0x70; }
    
    //! @brief    Updates internal data structures
    //! @details  This method is called on each filter related register change
    void updateInternals();
    
    int16_t fastsid_calculate_single_sample();
};

#endif
