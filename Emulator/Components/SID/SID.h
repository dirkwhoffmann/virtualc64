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
    // Number of this SID (0 = primary SID)
    int nr;

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


    //
    // Methods from CoreComponent
    //

public:

    SID& operator= (const SID& other) {

        CLONE(resid)
        CLONE(fastsid)

        // CLONE(config)

        return *this;
    }


    template <class T>
    void serialize(T& worker)
    {
        if (isResetter(worker)) return;

        worker

        << resid
        << fastsid;

        // TODO: Config
    }

    void operator << (SerResetter &worker) override { serialize(worker); }
    void operator << (SerChecker &worker) override { serialize(worker); }
    void operator << (SerCounter &worker) override { serialize(worker); }
    void operator << (SerReader &worker) override  { serialize(worker); }
    void operator << (SerWriter &worker) override { serialize(worker); }

    void _reset(bool hard) override { };


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
