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
SID::operator << (SerReader &worker)
{
    serialize(worker);
    stream.clear(0);
}

u8
SID::spypeek(u16 addr) const
{
    return sidreg[addr & 0x1F];
}

u8 
SID::peek(u16 addr)
{
    switch (config.engine) {

        case SIDENGINE_RESID:   return resid.peek(addr);

        default:
            fatalError;
    }
}

void
SID::poke(u16 addr, u8 value)
{
    sidreg[addr & 0x1F] = value;
    
    switch (config.engine) {

        case SIDENGINE_RESID:   resid.poke(addr & 0x1F, value); break;

        default:
            fatalError;
    }
}

void 
SID::executeUntil(Cycle targetCycle)
{
    if (isEnabled() && !powerSave()) {

        // Compute the number of missing cycles
        Cycle missing = targetCycle - clock;

        // Check if SID is in sync with the CPU
        if (missing < -1000 || missing > 1000000) {

            warn("Resyncing SID %ld (%lld cycles off)\n", objid, missing);

        } else {

            // Make sure to run for at least one cycle to make pipelined writes worke
            if (missing < 1) missing = 1;

            // Compute the missing samples
            auto numSamples = resid.executeCycles(isize(missing), stream);
            debug(SID_EXEC, "%ld: target: %lld missing: %lld generated: %ld", objid, targetCycle, missing, numSamples);
        }
    } else {

        // trace(true, "Power safe mode\n");
    }

    clock = targetCycle;
}

bool
SID::powerSave() const
{
    if (emulator.isWarping() && config.powerSave) {

        /* https://sourceforge.net/p/vice-emu/bugs/1374/
         *
         * Due to a bug in reSID, pending register writes are dropped if we
         * skip sample synthesis if SAMPLE_FAST and MOS8580 are selected both.
         * As a workaround, we ignore the power-saving setting in this case.
         */
        return config.revision != MOS_8580 || config.sampling != SAMPLING_FAST;
    }

    return false;
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
