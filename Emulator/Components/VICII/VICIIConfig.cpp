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
#include "VICII.h"

namespace vc64 {

i64
VICII::getOption(Option option) const
{
    switch (option) {

        case OPT_VICII_REVISION:        return config.awaiting;
        case OPT_VICII_POWER_SAVE:      return config.powerSave;
        case OPT_VICII_GRAY_DOT_BUG:    return config.grayDotBug;
        case OPT_GLUE_LOGIC:            return config.glueLogic;
        case OPT_VICII_HIDE_SPRITES:    return config.hideSprites;
        case OPT_VICII_SS_COLLISIONS:   return config.checkSSCollisions;
        case OPT_VICII_SB_COLLISIONS:   return config.checkSBCollisions;

        default:
            fatalError;
    }
}

void
VICII::setOption(Option option, i64 value)
{
    switch (option) {

        case OPT_VICII_REVISION:

            if (!VICIIRevisionEnum::isValid(value)) {
                throw VC64Error(ERROR_OPT_INVARG, VICIIRevisionEnum::keyList());
            }

            config.awaiting = VICIIRevision(value);

            // If the emulator is powered off, perform the change immediately
            if (isPoweredOff()) updateRevision();

            return;

        case OPT_VICII_POWER_SAVE:

            config.powerSave = bool(value);
            return;

        case OPT_VICII_GRAY_DOT_BUG:

            config.grayDotBug = bool(value);
            return;

        case OPT_VICII_HIDE_SPRITES:

            config.hideSprites = bool(value);
            return;

        case OPT_VICII_SS_COLLISIONS:

            config.checkSSCollisions = bool(value);
            return;

        case OPT_VICII_SB_COLLISIONS:

            config.checkSBCollisions = bool(value);
            return;

        case OPT_GLUE_LOGIC:

            if (!GlueLogicEnum::isValid(value)) {
                throw VC64Error(ERROR_OPT_INVARG, GlueLogicEnum::keyList());
            }

            config.glueLogic = GlueLogic(value);
            return;

        default:
            fatalError;
    }
}

}
