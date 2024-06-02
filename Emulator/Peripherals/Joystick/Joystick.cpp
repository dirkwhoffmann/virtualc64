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
#include "Joystick.h"
#include "Emulator.h"
#include "IOUtils.h"

namespace vc64 {

Joystick::Joystick(C64& ref, ControlPort& pref) : SubComponent(ref, pref.objid), port(pref)
{
};

u8
Joystick::getControlPort() const
{
    u8 result = 0xFF;
    
    if (axisY == -1) CLR_BIT(result, 0);
    if (axisY ==  1) CLR_BIT(result, 1);
    if (axisX == -1) CLR_BIT(result, 2);
    if (axisX ==  1) CLR_BIT(result, 3);
    if (button)      CLR_BIT(result, 4);

    return result;
}

void
Joystick::trigger(GamePadAction event)
{
    debug(JOY_DEBUG, "Port %ld: %s\n", objid, GamePadActionEnum::key(event));

    switch (event) {

        case PULL_UP:    axisY = -1; break;
        case PULL_DOWN:  axisY =  1; break;
        case PULL_LEFT:  axisX = -1; break;
        case PULL_RIGHT: axisX =  1; break;
        case RELEASE_X:  axisX =  0; break;
        case RELEASE_Y:  axisY =  0; break;
        case RELEASE_XY: axisX = axisY = 0; break;
            
        case PRESS_FIRE:
            
            button = true;
            
            if (config.autofire) {

                if (isAutofiring() && !config.autofireBursts) {
                    reload(0); // Stop shooting
                } else {
                    reload();  // Start or continue shooting
                }
            }
            break;

        case RELEASE_FIRE:
            
            if (!config.autofire) button = false;
            break;

        default:
            fatalError;
    }
    
    port.device = CPDEVICE_JOYSTICK;
}

void
Joystick::processEvent()
{
    // Get the number of remaining bullets
    auto shots = objid == PORT_1 ? (isize)c64.data[SLOT_AFI1] : (isize)c64.data[SLOT_AFI2];
    assert(shots > 0);

    // Cancel the current event
    objid == PORT_1 ? c64.cancel<SLOT_AFI1>() : c64.cancel<SLOT_AFI2>();

    // Fire and reload
    if (button) {

        button = false;
        reload(shots - 1);

    } else {

        button = true;
        reload(shots);
    }
}

bool
Joystick::isAutofiring()
{
    return objid == PORT_1 ? c64.isPending<SLOT_AFI1>() : c64.isPending<SLOT_AFI2>();
}

void
Joystick::reload()
{
    reload(config.autofireBursts ? config.autofireBullets : LONG_MAX);
}

void
Joystick::reload(isize bullets)
{
    objid == PORT_1 ? reload <SLOT_AFI1> (bullets) : reload <SLOT_AFI2> (bullets);
}

template <EventSlot Slot> void
Joystick::reload(isize bullets)
{
    if (bullets > 0 && config.autofire) {

        if (c64.isPending<Slot>()) {

            // Just add bullets (shooting is in progress)
            trace(JOY_DEBUG, "Filling up to %ld bullets\n", bullets);
            c64.data[Slot] = bullets;

        } else {

            // Fire the first shot
            auto cycle = config.autofireDelay * vic.getCyclesPerFrame();
            trace(JOY_DEBUG, "Next auto-fire event in %ld cycles\n", cycle);
            c64.scheduleRel<Slot>(cycle, AFI_FIRE, bullets);
        }

    } else {

        // Release the fire button and cancel any pending event
        c64.cancel<Slot>();
        button = false;
    }
}

}
