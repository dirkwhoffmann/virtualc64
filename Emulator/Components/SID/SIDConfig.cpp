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

void
SID::resetConfig()
{
    Configurable::resetConfig(emulator.defaults, objid);
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
    return emulator.defaults.getFallback(opt, objid);
}

void
SID::setOption(Option option, i64 value)
{
    bool wasMuted = c64.sidBridge.isMuted();

    switch (option) {

        case OPT_SID_ENABLE:
        {
            if (objid == 0 && value == false) {
                warn("SID 0 can't be disabled\n");
                return;
            }

            if (config.enabled == value) {
                return;
            }

            {   SUSPENDED

                config.enabled = value;
                c64.sidBridge.clearSampleBuffer(objid);
                c64.sidBridge.hardReset();
            }
            return;
        }

        case OPT_SID_ADDRESS:
        {
            if (objid == 0 && value != 0xD400) {
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
                sidBridge.clearSampleBuffer(objid);
            }
            return;
        }
        case OPT_AUD_VOL:

            config.vol = std::clamp(value, 0LL, 100LL);
            vol = powf((float)config.vol / 100, 1.4f) * 0.000025f;
            if (emscripten) vol *= 0.15f;

            if (wasMuted != sidBridge.isMuted()) {
                msgQueue.put(MSG_MUTE, sidBridge.isMuted());
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

}
