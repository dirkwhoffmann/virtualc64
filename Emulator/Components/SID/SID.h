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
#include "FastSID.h"

namespace vc64 {

class SID final : public SubComponent, public Dumpable
{
    friend class Muxer;

    ConfigOptions options = {

        OPT_SID_ENABLE,
        OPT_SID_ADDRESS,
        OPT_AUD_VOL,
        OPT_AUD_PAN
    };

    // Current configuration
    SIDConfig config = { };

    // Channel volume
    float vol = 0.0;

    // Panning factor
    float pan = 0.0;

public:

    // Backends
    ReSID resid = ReSID(c64, id);
    FastSID fastsid = FastSID(c64, id);

    
    //
    // Methods
    //

public:

    SID(C64 &ref, isize id);

    const char *getDescription() const override;
    void _dump(Category category, std::ostream& os) const override;

    SID& operator= (const SID& other) {

        CLONE(resid)
        CLONE(fastsid)

        CLONE(config)

        return *this;
    }


    template <class T>
    void serialize(T& worker)
    {
        worker

        << resid
        << fastsid;

        if (isResetter(worker)) return;

        worker

        << config.enabled
        << config.address
        << config.vol
        << config.pan;

    } SERIALIZERS(serialize);

    void _reset(bool hard) override { };


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
