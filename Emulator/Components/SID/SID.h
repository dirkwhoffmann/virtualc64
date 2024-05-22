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
#include "ReSID.h"

namespace vc64 {

class SID final : public SubComponent, public Inspectable<SIDInfo, Void>
{
    friend class SIDBridge;
    friend class AudioPort;

    Descriptions descriptions = {
        {
            .name           = "SID 1",
            .shellName      = "sid",
            .description    = "Primary Sound Interface Device"
        },
        {
            .name           = "SID 2",
            .shellName      = "sid2",
            .description    = "First Auxiliary SID"
        },
        {
            .name           = "SID 3",
            .shellName      = "sid3",
            .description    = "Second Auxiliary SID"
        },
        {
            .name           = "SID 4",
            .shellName      = "sid4",
            .description    = "Third Auxiliary SID"
        }
    };

    ConfigOptions options = {

        OPT_SID_ENABLE,
        OPT_SID_ADDRESS,
        OPT_SID_REVISION,
        OPT_SID_FILTER,
        OPT_SID_SAMPLING,
        OPT_AUD_VOL,
        OPT_AUD_PAN
    };

    // Current configuration
    SIDConfig config = { };

    // Mirrored SID register contents (for spypeek)
    u8 sidreg[32];

    // This SID has been executed up to this cycle
    Cycle clock = 0;

    // Channel volume
    float vol = 0.0;

    // Panning factor
    float pan = 0.0;

public:

    // Backends
    ReSID resid = ReSID(c64, objid);


    //
    // Methods
    //

public:

    SID(C64 &ref, isize id);
    const Descriptions &getDescriptions() const override { return descriptions; }
    void _dump(Category category, std::ostream& os) const override;

    SID& operator= (const SID& other) {

        CLONE(resid)
        CLONE(config)
        CLONE(clock)

        return *this;
    }


    template <class T>
    void serialize(T& worker)
    {
        worker

        << sidreg
        << clock
        << resid;

        if (isResetter(worker)) return;

        worker

        << config.enabled
        << config.address
        << config.revision
        << config.filter
        << config.sampling
        << config.vol
        << config.pan;

    } SERIALIZERS(serialize);

    void _reset(bool hard) override { };


    //
    // Methods from Inspectable
    //

public:

    void cacheInfo(SIDInfo &result) const override;


    //
    // Configuring
    //

public:

    void resetConfig() override;
    const SIDConfig &getConfig() const { return config; }
    const ConfigOptions &getOptions() const override { return options; }
    i64 getOption(Option opt) const override;
    i64 getFallback(Option opt) const override;
    void setOption(Option opt, i64 value) override;

    
    //
    // Accessing
    //

public:

    // Reads the real value of a SID register (used by the debugger only)
    u8 spypeek(u16 addr) const;

    // Writes a SID register
    void poke(u16 addr, u8 value);


    //
    // Computing audio samples
    //

    /* Executes SID until a certain cycle is reached. The function returns the
     * number of produced sound samples (not yet).
     */
    void executeUntil(Cycle targetCycle, SampleStream &stream);


    //
    // Bridge functions
    //

public:

    u32 getClockFrequency() const;
    void setClockFrequency(u32 frequency);

    SIDRevision getRevision() const;
    void setRevision(SIDRevision revision);

    double getSampleRate() const;
    void setSampleRate(double rate);

    bool getAudioFilter() const;
    void setAudioFilter(bool enable);

    SamplingMethod getSamplingMethod() const;
    void setSamplingMethod(SamplingMethod method);
};

};
