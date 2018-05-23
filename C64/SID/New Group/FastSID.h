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
#include "FastVoice.h"


//! The virtual sound interface device (SID)
/*! SID is the sound chip of the Commodore 64.
	The SID chip occupied the memory mapped I/O space from address 0xD400 to 0xD7FF. 
*/
class FastSID : public VirtualComponent {

public:
    
    //! Pointer to bridge object
    class SIDBridge *bridge;
    
    //! @brief   SID registers
    uint8_t sidreg[32];
    
    //! @brief   Internal constant used for sample rate dependent calculations
    uint32_t speed1;
    
private:
    
    //! @brief   The three SID voices
    FastVoice voice[3];
        
    //! @brief   Chip model.
    SIDChipModel chipModel;
    
    //! @brief   Current CPU frequency
    uint32_t cpuFrequency;
    
    //! @brief   Sample rate (44.1 kHz per default)
    uint32_t sampleRate;
    
    //! @brief    Ratio between sample rate and cpu frequency
    double   samplesPerCycle;
    
    //! @brief   Stores for how many cycles FastSID was executed so far
    uint64_t executedCycles;

    //! @brief   Stores how many sound samples were computed so far
    uint64_t computedSamples;

    //! @brief   Switches filter emulation on or off.
    bool emulateFilter;
    
    //! @brief   Last value on the data bus
    uint8_t latchedDataBus;
    
public:
    
    //! @brief   ADSR counter step lookup table
    int32_t adrs[16];
    
    //! @brief   Sustain comparison values loopup table
    uint32_t sz[16];
    
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
	
    //! Method from VirtualComponent
    void reset();

    //! Load state
    void loadFromBuffer(uint8_t **buffer);
    
    //! Dump internal state to console
    void dumpState();
    
    //! @brief    Gathers all values that are displayed in the debugger
    SIDInfo getInfo();
    
    //! Special peek function for the I/O memory range.
    uint8_t peek(uint16_t addr);
    
    //! Special poke function for the I/O memory range.
    void poke(uint16_t addr, uint8_t value);
    
    /*! @brief   Execute SID
     *  @details Runs reSID for the specified amount of CPU cycles and writes
     *           the generated sound samples into the internal ring buffer.
     */
    void execute(uint64_t cycles);
    
    //! @brief   Computes a single sound sample
    int16_t calculateSingleSample();
    
    
    //
    //! @functiongroup Configuring the device
    //
    
    //! Returns the chip model
    SIDChipModel getChipModel() { return chipModel; }
    
    //! Sets the chip model
    void setChipModel(SIDChipModel model);
    
    //! Returns the clock frequency
    uint32_t getClockFrequency() { return cpuFrequency; }
    
    //! Sets the clock frequency
    void setClockFrequency(uint32_t frequency);
    
    //! Returns the sample rate
    uint32_t getSampleRate() { return sampleRate; }
    
    //! Sets the sample rate
    void setSampleRate(uint32_t rate);
    
    //! Returns true iff audio filters should be emulated.
    bool getAudioFilter() { return emulateFilter; }
    
    //! Enable or disable audio filter emulation
    void setAudioFilter(bool value) { emulateFilter = value; }
    
private:
    
    //! @brief   Initializes SID
    void init(int sampleRate, int cycles_per_sec);
    
    //! @brief   Initializes filter lookup tables
    void initFilter(int sampleRate);
    
    
    //
    //! @functiongroup Accessing device properties
    //
    
    //! @brief   Returns the currently set SID volume
    uint8_t sidVolume() { return sidreg[0x18] & 0x0F; }
    
    //! @brief    Returns true iff voice 3 is disconnected from the audio output
    /*! @details  Setting voice 3 to bypass the filter (FILT3 = 0) and setting
     *            bit 7 in the Mod/Vol register to one prevents voice 3 from
     *            reaching the audio output.
     */
    bool voiceThreeDisconnected() { return filterOff(2) && (sidreg[0x18] & 0x80); }
    
    // Filter related configuration items
    
    //! @brief   Returns the filter cutoff frequency (11 bit value)
    uint16_t filterCutoff() { return (sidreg[0x16] << 3) | (sidreg[0x15] & 0x07); }

    //! @brief    Returns the filter resonance (4 bit value)
    uint8_t filterResonance() { return sidreg[0x17] >> 4; }

    //! @brief    Returns true iff the specified voice schould be filtered
    bool filterOn(unsigned voice) { return GET_BIT(sidreg[0x17], voice) != 0; }

    //! @brief    Returns true iff the specified voice schould not be filtered
    bool filterOff(unsigned voice) { return GET_BIT(sidreg[0x17], voice) == 0; }

    //! @brief    Returns true iff the EXTERNAL filter bit is set
    bool filterExtBit() { return sidreg[0x17] & 0x08; }

    //! @brief   Returns the currently set filter type
    uint8_t filterType() { return sidreg[0x18] & 0x70; }
    
    
    //! @brief    Updates internal data structures
    //! @details  This method is called on each filter related register change
    void updateInternals();
};

#endif
