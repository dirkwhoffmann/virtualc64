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

SID::SID(C64 &ref, int n) : SubComponent(ref), nr(n)
{
    setClockFrequency(PAL::CLOCK_FREQUENCY);
}

const char *
SID::getDescription() const
{
    assert(nr >= 0 && nr <= 3);
    return nr == 0 ? "SID" : nr == 1 ? "SID1" : nr == 2 ? "SID2" : "SID3";
}

void
SID::_dump(Category category, std::ostream& os) const
{
    using namespace util;

    if (category == Category::Config) {

        dumpConfig(os);

    } else {

        switch (muxer.getConfig().engine) {

            case SIDENGINE_RESID:   resid.dump(category, os); break;
            case SIDENGINE_FASTSID: fastsid.dump(category, os); break;

            default:
                fatalError;
        }
    }
}

void
SID::resetConfig()
{
    Configurable::resetConfig(emulator.defaults, nr);
}

i64
SID::getOption(Option option) const
{

    switch (option) {

        case OPT_SID_ENABLE:     return config.enabled;
        case OPT_SID_ADDRESS:    return config.address;
        case OPT_AUD_VOL:        return config.vol;
        case OPT_AUD_PAN:        return config.pan;

        default:
            fatalError;
    }
}

i64 
SID::getFallback(Option opt) const
{
    return emulator.defaults.getFallback(opt, nr);
}

void
SID::setOption(Option option, i64 value)
{
    bool wasMuted = c64.muxer.isMuted();

    switch (option) {

        case OPT_SID_ENABLE:
        {
            if (nr == 0 && value == false) {
                warn("SID 0 can't be disabled\n");
                return;
            }

            if (config.enabled == value) {
                return;
            }

            {   SUSPENDED

                config.enabled = value;
                c64.muxer.clearSampleBuffer(nr);
                c64.muxer.hardReset();
            }
            return;
        }

        case OPT_SID_ADDRESS:
        {
            if (nr == 0 && value != 0xD400) {
                warn("SID 0 can't be remapped\n");
                return;
            }

            if (value < 0xD400 || value > 0xD7E0 || (value & 0x1F)) {
                throw VC64Error(ERROR_OPT_INVARG, "D400, D420 ... D7E0");
            }

            if (config.address == value) {
                return;
            }

            {   SUSPENDED

                config.address = (u16)value;
                muxer.clearSampleBuffer(nr);
            }
            return;
        }
        case OPT_AUD_VOL:

            config.vol = std::clamp(value, 0LL, 100LL);
            vol = powf((float)config.vol / 100, 1.4f) * 0.000025f;
            if (emscripten) vol *= 0.15f;

            if (wasMuted != muxer.isMuted()) {
                msgQueue.put(MSG_MUTE, muxer.isMuted());
            }

            return;

        case OPT_AUD_PAN:

            config.pan = value;
            pan = float(0.5 * (sin(config.pan * M_PI / 200.0) + 1));
            return;

        default:
            fatalError;
    }
}

u32
SID::getClockFrequency() const
{
    assert(resid.getClockFrequency() == fastsid.getClockFrequency());
    return resid.getClockFrequency();
}

void
SID::setClockFrequency(u32 frequency)
{
    resid.setClockFrequency(frequency);
    fastsid.setClockFrequency(frequency);
}

SIDRevision 
SID::getRevision() const
{
    assert(resid.getRevision() == fastsid.getRevision());
    return resid.getRevision();
}

void 
SID::setRevision(SIDRevision revision)
{
    resid.setRevision(revision);
    fastsid.setRevision(revision);
}

double 
SID::getSampleRate() const
{
    assert(resid.getSampleRate() == fastsid.getSampleRate());
    return resid.getSampleRate();
}

void 
SID::setSampleRate(double rate)
{
    resid.setSampleRate(rate);
    fastsid.setSampleRate(rate);
}


bool 
SID::getAudioFilter() const
{
    assert(resid.getAudioFilter() == fastsid.getAudioFilter());
    return resid.getAudioFilter();
}

void 
SID::setAudioFilter(bool enable)
{
    resid.setAudioFilter(enable);
    fastsid.setAudioFilter(enable);
}

SamplingMethod 
SID::getSamplingMethod() const
{
    // Not available for FastSID
    return resid.getSamplingMethod();
}
void 
SID::setSamplingMethod(SamplingMethod method)
{
    // Not available for FastSID
    resid.setSamplingMethod(method);
}

}
