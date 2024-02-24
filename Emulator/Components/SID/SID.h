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

class SID final : public SubComponent
{
    friend class Muxer;

    ConfigOptions options = {

        { OPT_SID_ENABLE,   "Enables or disables a SID" },
        { OPT_SID_ADDRESS,  "Mapped location in memory" },
        { OPT_AUD_VOL,      "Volume" },
        { OPT_AUD_PAN,      "Speaker location" }
    };
    
    // Number of this SID (0 = primary SID)
    int nr;

    // Current configuration
    SIDConfig config = { };

    // Channel volume
    float vol = 0.0;

    // Panning factor
    float pan = 0.0;

public:

    // Backends
    ReSID resid = ReSID(c64, nr);
    FastSID fastsid = FastSID(c64, nr);

    
    //
    // Initializing
    //

public:

    SID(C64 &ref, int n);


    //
    // Methods from CoreObject
    //

private:

    const char *getDescription() const override { return "SID"; }
    // void _dump(Category category, std::ostream& os) const override { };


    //
    // Methods from CoreComponent
    //

public:

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
    }

    void operator << (SerResetter &worker) override { serialize(worker); }
    void operator << (SerChecker &worker) override { serialize(worker); }
    void operator << (SerCounter &worker) override { serialize(worker); }
    void operator << (SerReader &worker) override  { serialize(worker); }
    void operator << (SerWriter &worker) override { serialize(worker); }

    void _reset(bool hard) override { };


    //
    // Methods from Configurable
    //

    const ConfigOptions &getOptions() const override { return options; }

    // Gets or sets a config option
    i64 getOption(Option opt) const override;
    void setOption(Option opt, i64 value) override;

    
    //
    // Configuring
    //

public:

    const SIDConfig &getConfig() const { return config; }
    void resetConfig() override;


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
