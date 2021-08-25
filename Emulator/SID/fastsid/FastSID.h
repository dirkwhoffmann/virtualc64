/*
 * This file belongs to the FastSID implementation of VirtualC64, an adaption
 * of the code used in VICE 3.1, the Versatile Commodore Emulator.
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

#pragma once

#include "SubComponent.h"
#include "FastVoice.h"
#include "SIDStreams.h"
#include "Constants.h"

class FastSID : public SubComponent {
        
    // Reference to the SID bridge
    Muxer &bridge;
    
    // Number of this SID (0 = primary SID)
    int nr;

    
    //
    // Sub components
    //
    
private:
    
    // The three SID voices
    FastVoice voice[3] = { FastVoice(c64), FastVoice(c64), FastVoice(c64) };

    
public:
        
    // SID registers
    u8 sidreg[32];
    
    // Internal constant used for sample rate dependent calculations
    u32 speed1;
    
private:
            
    // Chip model
    SIDRevision model = MOS_6581;
    
    // Current CPU frequency
    u32 cpuFrequency = PAL_CLOCK_FREQUENCY;
    
    // Sample rate (44.1 kHz per default)
    double sampleRate = 44100.0;
        
    // Stores for how many cycles FastSID was executed so far
    u64 executedCycles;

    // Stores how many sound samples were computed so far
    u64 computedSamples;

    // Switches filter emulation on or off
    bool emulateFilter = true;
    
    // Last value on the data bus
    u8 latchedDataBus;
    
public:
    
    // ADSR counter step lookup table
    i32 adrs[16];
    
    // Sustain comparison values loopup table
    u32 sz[16];
    
private:
    
    // Filter lookup table
    float lowPassParam[0x800];
    float bandPassParam[0x800];
    float filterResTable[16];
    
    // Amplifier lookup table
    signed char ampMod1x8[256];
    
    
    //
    // Initializing
    //
    
public:
        
	FastSID(C64 &ref, Muxer &bridgeref, int n);
    
private:
    
    void init(double sampleRate, int cycles_per_sec);
    void initFilter(double sampleRate);

    
    //
    // Methods from C64Object
    //

    const char *getDescription() const override { return "FastSID"; }
    void _dump(dump::Category category, std::ostream& os) const override;

    
    //
    // Methods from C64Component
    //

private:

    void _reset(bool hard) override;
    
    template <class T>
    void applyToPersistentItems(T& worker)
    {
        worker
        
        << model
        << cpuFrequency
        << emulateFilter;
    }
    
    template <class T>
    void applyToResetItems(T& worker, bool hard = true)
    {
        if (hard) {
            
            worker
            
            << executedCycles
            << computedSamples;
        }
        
        worker
        
        << sidreg
        << speed1
        << latchedDataBus;
    }
    
    isize _size() override { COMPUTE_SNAPSHOT_SIZE }
    isize _load(const u8 *buffer) override { LOAD_SNAPSHOT_ITEMS }
    isize _save(u8 *buffer) override { SAVE_SNAPSHOT_ITEMS }
    

    //
    // Configuring
    //
    
public:
    
    u32 getClockFrequency() const { return cpuFrequency; }
    void setClockFrequency(u32 frequency);
    
    SIDRevision getRevision() const { return model; }
    void setRevision(SIDRevision m);
    
    double getSampleRate() const { return (double)sampleRate; }
    void setSampleRate(double rate);
    
    bool getAudioFilter() const { return emulateFilter; }
    void setAudioFilter(bool value) { emulateFilter = value; }
    
    
    //
    // Analyzing
    //
    
public:
    
    SIDInfo getInfo();
    VoiceInfo getVoiceInfo(isize voice);

    
    //
    // Accessing
    //
        
public:
        
    // Reads or writes a SID register
    u8 peek(u16 addr);
    u8 spypeek(u16 addr) const;
    void poke(u16 addr, u8 value);
    
    
    //
    // Emulating
    //
    
public:
    
    /* Runs SID for the specified amount of CPU cycles. The generated sound
     * samples are written into the provided ring buffer. The fuction returns
     * the number of written audio samples.
     */
    i64 executeCycles(isize numCycles, SampleStream &stream);
    i64 executeCycles(isize numCycles);
    
private:
    
    // Computes a single sound sample
    i16 calculateSingleSample();
    
     
    //
    // Accessing device properties
    //
    
    // Returns the currently set SID volume
    u8 sidVolume() const { return sidreg[0x18] & 0x0F; }
    
    /* Returns true iff voice 3 is disconnected from the audio output. Setting
     * voice 3 to bypass the filter (FILT3 = 0) and setting bit 7 in the Mod/Vol
     * register to one prevents voice 3 from reaching the audio output.
     */
    bool voiceThreeDisconnected() const { return filterOff(2) && (sidreg[0x18] & 0x80); }
    
    // Filter related configuration items
    
    // Returns the filter cutoff frequency (11 bit value)
    u16 filterCutoff() const { return (u16)(sidreg[0x16] << 3 | (sidreg[0x15] & 0x07)); }

    // Returns the filter resonance (4 bit value)
    u8 filterResonance() const { return sidreg[0x17] >> 4; }

    // Returns true iff the specified voice schould be filtered
    bool filterOn(unsigned voice) const { return GET_BIT(sidreg[0x17], voice) != 0; }

    // Returns true iff the specified voice schould not be filtered
    bool filterOff(unsigned voice) const { return GET_BIT(sidreg[0x17], voice) == 0; }

    // Returns true iff the external filter bit is set
    bool filterExtBit() const { return GET_BIT(sidreg[0x17], 7) != 0; }
    
    // Returns the currently set filter type
    u8 filterType() const { return sidreg[0x18] & 0x70; }
    
    
    /* Updates internal data structures. This method is called on each filter
     * related register change.
     */
    void updateInternals();
};
