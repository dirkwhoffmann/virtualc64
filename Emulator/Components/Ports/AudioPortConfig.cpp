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

i64
AudioPort::getOption(Option option) const
{
    switch (option) {

        case OPT_AUD_VOL_L:
            return config.volL;

        case OPT_AUD_VOL_R:
            return config.volR;

        default:
            fatalError;
    }
}

void
AudioPort::setOption(Option option, i64 value)
{
    // bool wasMuted = isMuted();

    switch (option) {

        case OPT_AUD_VOL_L:

            config.volL = std::clamp(value, 0LL, 100LL);
            volL.maximum = powf((float)config.volL / 50, 1.4f);

            /*
            if (wasMuted != isMuted()) {
                msgQueue.put(MSG_MUTE, isMuted());
            }
            */
            return;

        case OPT_AUD_VOL_R:

            config.volR = std::clamp(value, 0LL, 100LL);
            volR.maximum = powf((float)config.volR / 50, 1.4f);

            /*
            if (wasMuted != isMuted()) {
                msgQueue.put(MSG_MUTE, isMuted());
            }
            */
            return;

        default:
            fatalError;
    }
}

}
