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

#include "config.h"
#include "SID.h"
#include "Emulator.h"

namespace vc64 {

SID::SID(C64 &ref, isize id) : SubComponent(ref, id)
{
    setClockFrequency(PAL::CLOCK_FREQUENCY);
}

void
SID::_dump(Category category, std::ostream& os) const
{
    using namespace util;

    if (category == Category::Config) {

        dumpConfig(os);
    }

    if (category == Category::State) {

        os << tab("Chip");
        os << dec(objid) << std::endl;
        os << tab("Clock");
        os << dec(clock) << std::endl;
        os << tab("Volume");
        os << flt(vol) << std::endl;
        os << tab("Pan");
        os << flt(pan) << std::endl;
        
        resid.dump(category, os);
    }
}

u8
SID::spypeek(u16 addr) const
{
    return sidreg[addr & 0x1F];
}

void
SID::poke(u16 addr, u8 value)
{
    sidreg[addr & 0x1F] = value;
    
    resid.poke(addr & 0x1F, value);
}

void 
SID::executeUntil(Cycle targetCycle, SampleStream &stream)
{
    if (!muxer.powerSave()) {

        // Compute the number of missing cycles
        Cycle missing = targetCycle - clock;

        // Check if SID is in sync with the CPU
        if (missing < -1000 || missing > 1000000) {

            warn("Resyncing SID %ld (%lld cycles off)\n", objid, missing);

        } else {

            // Make sure to run for at least one cycle to make pipelined writes worke
            if (missing < 1) missing = 1;

            // Compute the missing samples
            auto numSamples = resid.executeCycles(missing, stream);
            debug(SID_EXEC, "%ld: target: %lld missing: %lld generated: %ld", objid, targetCycle, missing, numSamples);
        }
    }

    clock = targetCycle;
}

u32
SID::getClockFrequency() const
{
    return resid.getClockFrequency();
}

void
SID::setClockFrequency(u32 frequency)
{
    resid.setClockFrequency(frequency);
}

SIDRevision 
SID::getRevision() const
{
    return resid.getRevision();
}

void 
SID::setRevision(SIDRevision revision)
{
    resid.setRevision(revision);
}

double 
SID::getSampleRate() const
{
    return resid.getSampleRate();
}

void 
SID::setSampleRate(double rate)
{
    resid.setSampleRate(rate);
}


bool 
SID::getAudioFilter() const
{
    return resid.getAudioFilter();
}

void 
SID::setAudioFilter(bool enable)
{
    resid.setAudioFilter(enable);
}

SamplingMethod 
SID::getSamplingMethod() const
{
    return resid.getSamplingMethod();
}
void 
SID::setSamplingMethod(SamplingMethod method)
{
    resid.setSamplingMethod(method);
}

}
