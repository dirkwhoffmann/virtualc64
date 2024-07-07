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
#include "C64Types.h"
#include "ReSID.h"

namespace vc64 {

class SID final : public SubComponent, public Inspectable<SIDInfo>
{
    friend class SIDBridge;
    friend class AudioPort;

    Descriptions descriptions = {
        {
            .name           = "SID",
            .description    = "Primary Sound Interface Device"
        },
        {
            .name           = "SID2",
            .description    = "First Auxiliary SID"
        },
        {
            .name           = "SID3",
            .description    = "Second Auxiliary SID"
        },
        {
            .name           = "SID4",
            .description    = "Third Auxiliary SID"
        }
    };

    ConfigOptions options = {

        OPT_SID_ENABLE,
        OPT_SID_ADDRESS,
        OPT_SID_REVISION,
        OPT_SID_FILTER,
        OPT_SID_ENGINE,
        OPT_SID_SAMPLING,
        OPT_SID_POWER_SAVE
    };

    // Current configuration
    SIDConfig config = { };

    // Mirrored SID register contents (for spypeek)
    u8 sidreg[32];

    // This SID has been executed up to this cycle
    Cycle clock = 0;

    // The audio stream
    SampleStream stream;

public:

    // Backends
    ReSID resid = ReSID(c64, objid);


    //
    // Methods
    //

public:

    SID(C64 &ref, isize id);

    SID& operator= (const SID& other) {

        CLONE_ARRAY(sidreg)
        CLONE(clock)
        CLONE(resid)

        CLONE(config)

        return *this;
    }


    //
    // Methods from Serializable
    //

public:

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
        << config.sampling;
    }

    void operator << (SerResetter &worker) override { serialize(worker); }
    void operator << (SerChecker &worker) override { serialize(worker); }
    void operator << (SerCounter &worker) override { serialize(worker); }
    void operator << (SerReader &worker) override;
    void operator << (SerWriter &worker) override { serialize(worker); }


    //
    // Methods from CoreComponent
    //

public:

    const Descriptions &getDescriptions() const override { return descriptions; }

private:

    void _dump(Category category, std::ostream& os) const override;


    //
    // Methods from Inspectable
    //

private:

    void cacheInfo(SIDInfo &result) const override;


    //
    // Methods from Configurable
    //

public:

    const SIDConfig &getConfig() const { return config; }
    const ConfigOptions &getOptions() const override { return options; }
    i64 getFallback(Option opt) const override;
    i64 getOption(Option opt) const override;
    void checkOption(Option opt, i64 value) override;
    void setOption(Option opt, i64 value) override;


    //
    // Accessing
    //

public:

    // Checks if this SID is present
    bool isEnabled() const { return config.enabled; }

    // Reads the real value of a SID register (used by the debugger only)
    u8 spypeek(u16 addr) const;

    // Reads a SID register
    u8 peek(u16 addr);

    // Writes a SID register
    void poke(u16 addr, u8 value);


    //
    // Computing audio samples
    //

    /* Executes SID until a certain cycle is reached. The function returns the
     * number of produced sound samples (not yet).
     */
    void executeUntil(Cycle targetCycle);

    // Indicates if sample synthesis should be skipped
    bool powerSave() const;

    
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
