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
#include "AudioPort.h"
#include "Emulator.h"

namespace vc64 {

void
AudioPort::clear()
{
    loginfo(AUDBUF_DEBUG, "Clearing the audio sample buffer\n");

    // Wipe out the ringbuffer
    stream.wipeOut();
    stream.alignWritePtr();
}

void
AudioPort::_dump(Category category, std::ostream &os) const
{
    using namespace utl;

    if (category == Category::Config) {

        dumpConfig(os);
    }

    if (category == Category::State) {

        os << tab("Master volume left");
        os << flt(volL.current) << " (0 ... " << flt(volL.maximum) << ")" << std::endl;
        os << tab("Master volume right");
        os << flt(volR.current) << " (0 ... " << flt(volR.maximum) << ")" << std::endl;
        os << tab("Channel 0 volume");
        os << flt(vol[0]) << std::endl;
        os << tab("Channel 1 volume");
        os << flt(vol[1]) << std::endl;
        os << tab("Channel 2 volume");
        os << flt(vol[2]) << std::endl;
        os << tab("Channel 3 volume");
        os << flt(vol[3]) << std::endl;
        os << tab("Channel 0 pan");
        os << flt(pan[0]) << std::endl;
        os << tab("Channel 1 pan");
        os << flt(pan[1]) << std::endl;
        os << tab("Channel 2 pan");
        os << flt(pan[2]) << std::endl;
        os << tab("Channel 3 pan");
        os << flt(pan[3]) << std::endl;
        os << tab("Sample rate correction");
        os << flt(sampleRateCorrection) << " Hz" << std::endl;
    }
}

void
AudioPort::cacheInfo(AudioPortInfo &result) const
{

}

void
AudioPort::cacheStats(AudioPortStats &result) const
{
    {   SYNCHRONIZED
        
        result.fillLevel = stream.fillLevel();
    }
}

bool
AudioPort::isMuted() const
{
    if (volL.isFading() || volR.isFading()) return false;
    return volL + volR == 0.0 || vol[0] + vol[1] + vol[2] + vol[3] == 0.0;
}

void
AudioPort::_didLoad()
{

}

void
AudioPort::_didReset(bool hard)
{
    stats = { };
    clear();
}

void 
AudioPort::_powerOn()
{

}

void
AudioPort::_run()
{
    if (!emulator.isWarping()) unmute(10000);
}

void
AudioPort::_pause()
{
    eliminateCracks();
    mute();
}

void
AudioPort::_warpOn()
{
    eliminateCracks();
    mute();
}

void
AudioPort::_warpOff()
{
    if (emulator.isRunning()) unmute(10000);
}

void
AudioPort::_focus()
{
    if (emulator.isRunning() && !emulator.isWarping()) unmute(100000);
}

void
AudioPort::_unfocus()
{
    mute(100000);
}

i64
AudioPort::getOption(Opt option) const
{
    switch (option) {

        case Opt::AUD_VOL0:         return config.vol[0];
        case Opt::AUD_VOL1:         return config.vol[1];
        case Opt::AUD_VOL2:         return config.vol[2];
        case Opt::AUD_VOL3:         return config.vol[3];
        case Opt::AUD_PAN0:         return config.pan[0];
        case Opt::AUD_PAN1:         return config.pan[1];
        case Opt::AUD_PAN2:         return config.pan[2];
        case Opt::AUD_PAN3:         return config.pan[3];
        case Opt::AUD_VOL_L:        return config.volL;
        case Opt::AUD_VOL_R:        return config.volR;
        case Opt::AUD_BUFFER_SIZE:  return (i64)config.bufferSize;
        case Opt::AUD_ASR:          return (i64)config.asr;

        default:
            fatalError;
    }
}

void
AudioPort::checkOption(Opt opt, i64 value)
{
    switch (opt) {

        case Opt::AUD_VOL3:
        case Opt::AUD_VOL2:
        case Opt::AUD_VOL1:
        case Opt::AUD_VOL0:

            return;

        case Opt::AUD_PAN3:
        case Opt::AUD_PAN2:
        case Opt::AUD_PAN1:
        case Opt::AUD_PAN0:

            return;

        case Opt::AUD_VOL_L:
        case Opt::AUD_VOL_R:

            return;

        case Opt::AUD_BUFFER_SIZE:

            if (value < 512 || value > 65536) {
                throw CoreError(CoreError::OPT_INV_ARG, "512 ... 65536");
            }
            return;

        case Opt::AUD_ASR:

            return;

        default:
            throw CoreError(CoreError::OPT_UNSUPPORTED);
    }
}

void
AudioPort::setOption(Opt opt, i64 value)
{
    checkOption(opt, value);

    isize channel = 0;

    switch (opt) {

        case Opt::AUD_VOL3: channel++;
        case Opt::AUD_VOL2: channel++;
        case Opt::AUD_VOL1: channel++;
        case Opt::AUD_VOL0:

            config.vol[channel] = std::clamp(value, 0LL, 100LL);
            vol[channel] = powf(config.vol[channel] / 100.0f, 1.4f) * 0.000025f;
            if (emscripten) vol[channel] *= 0.15f;
            return;

        case Opt::AUD_PAN3: channel++;
        case Opt::AUD_PAN2: channel++;
        case Opt::AUD_PAN1: channel++;
        case Opt::AUD_PAN0:

            config.pan[channel] = value;
            pan[channel] = float(0.5 * (sin(config.pan[channel] * M_PI / 200.0) + 1));
            return;

        case Opt::AUD_VOL_L:

            config.volL = std::clamp(value, 0LL, 100LL);
            volL.maximum = powf((float)config.volL / 50, 1.4f);
            return;

        case Opt::AUD_VOL_R:

            config.volR = std::clamp(value, 0LL, 100LL);
            volR.maximum = powf((float)config.volR / 50, 1.4f);
            return;

        case Opt::AUD_BUFFER_SIZE:

            config.bufferSize = isize(value);
            // TODO: stream.resize(isize(value));
            return;

        case Opt::AUD_ASR:

            config.asr = (bool)value;
            return;

        default:
            fatalError;
    }
}

void
AudioPort::setSampleRate(double hz)
{
    // Set the sample rate or get it from the detector if none is provided
    if (hz != 0.0) {

        sampleRate = hz;
        logdebug(AUD_DEBUG, "setSampleRate(%.2f)\n", sampleRate);

    } else {

        sampleRate = detector.sampleRate();
        logdebug(AUD_DEBUG, "setSampleRate(%.2f) (predicted)\n", sampleRate);
    }
}

}
