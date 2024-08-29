// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// This FILE is dual-licensed. You are free to choose between:
//
//     - The GNU General Public License v3 (or any later version)
//     - The Mozilla Public License v2
//
// SPDX-License-Identifier: GPL-3.0-or-later OR MPL-2.0
// -----------------------------------------------------------------------------

#pragma once

#include "SIDTypes.h"
#include "SubComponent.h"
#include "AudioPort.h"
#include "resid/sid.h"

namespace vc64 {

/* This class is a wrapper around the third-party reSID library.
 *
 *   List of modifications applied to reSID:
 *
 *     - Changed visibility of some objects from protected to public
 *
 *   Good candidate for testing sound emulation:
 *
 *     - INTERNAT.P00
 *     - DEFEND1.PRG  ("Das Boot" intro music)
 *     - To Norah (Elysium)
 *     - Vortex (LMan)
 */

class ReSID final : public SubComponent, public Inspectable<SIDInfo> {

    friend class SID;
    
    Descriptions descriptions = {{

        .name           = "ReSID",
        .description    = "ReSID Backend",
        .shell          = "resid"
    }};

    Options options = {

    };
    
    // Entry point to the reSID backend
    reSID::SID *sid;
    
    // Result of the latest inspection
    mutable VoiceInfo voiceInfo[3] = { };

private:
    
    // ReSID state
    reSID::SID::State st = { };

    // The emulated chip model
    SIDRevision model = SIDRevision(0);

    // Clock frequency
    u32 clockFrequency = 0;

    // Sample rate (usually set to 44.1 kHz or 48.0 kHz)
    double sampleRate = 0;

    // Sampling method
    SamplingMethod samplingMethod = SamplingMethod(0);

    // Switches filter emulation on or off
    bool emulateFilter = true;

    
    //
    // Methods
    //
    
public:
    
    ReSID(C64 &ref, isize id);
    ~ReSID();

    ReSID& operator= (const ReSID& other) {

        CLONE(st)
        CLONE(model)
        CLONE(clockFrequency)
        CLONE(samplingMethod)
        CLONE(emulateFilter)

        return *this;
    }


    //
    // Methods from Serializable
    //

public:

    template <class T>
    void serialize(T& worker)
    {
        if (isResetter(worker)) return;

        worker

        << st.sid_register
        << st.bus_value
        << st.bus_value_ttl
        << st.write_pipeline
        << st.write_address
        << st.voice_mask
        << st.accumulator
        << st.shift_register
        << st.shift_register_reset
        << st.shift_pipeline
        << st.pulse_output
        << st.floating_output_ttl
        << st.rate_counter
        << st.rate_counter_period
        << st.exponential_counter
        << st.exponential_counter_period
        << st.envelope_counter
        << st.envelope_state
        << st.hold_zero
        << st.envelope_pipeline

        << model
        << clockFrequency
        << samplingMethod
        << emulateFilter;
    }
    
    void operator << (SerResetter &worker) override { serialize(worker); }
    void operator << (SerChecker &worker) override { }
    void operator << (SerCounter &worker) override { serialize(worker); }
    void operator << (SerReader &worker) override;
    void operator << (SerWriter &worker) override;


    //
    // Methods from CoreComponent
    //

public:

    const Descriptions &getDescriptions() const override { return descriptions; }

private:

    void _didReset(bool hard) override;


    //
    // Methods from Inspectable
    //

public:

    virtual void record() const override;
    void cacheInfo(SIDInfo &result) const override;


    //
    // Methods from Configurable
    //

public:

    const Options &getOptions() const override { return options; }
    

    //
    // Configuring
    //
    
public:
    
    u32 getClockFrequency() const;
    void setClockFrequency(u32 frequency);
    
    SIDRevision getRevision() const;
    void setRevision(SIDRevision m);
    
    double getSampleRate() const { return sampleRate; }
    void setSampleRate(double rate);
    
    bool getAudioFilter() const { return emulateFilter; }
    void setAudioFilter(bool enable);
    
    SamplingMethod getSamplingMethod() const;
    void setSamplingMethod(SamplingMethod value);
    
    
    //
    // Analyzing
    //

private:
    
    void _dump(Category category, std::ostream& os) const override;
    

    //
    // Accessing
    //
    
public:

    // Reads or writes a SID register
    u8 peek(u16 addr);
    void poke(u16 addr, u8 value);

    
    //
    // Emulating
    //
    
    /* Runs SID for the specified amount of CPU cycles. The generated sound
     * samples are written into the provided ring buffer. The fuction returns
     * the number of written audio samples.
     */
    isize executeCycles(isize numCycles, SampleStream &stream);
    isize executeCycles(isize numCycles);
};

}
