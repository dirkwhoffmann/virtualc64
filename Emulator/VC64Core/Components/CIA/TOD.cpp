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
#include "TOD.h"
#include "C64.h"
#include "IOUtils.h"
#include "PowerPort.h"

namespace vc64 {

TOD::TOD(C64 &ref, CIA &ciaref) : SubComponent(ref), cia(ciaref)
{
}



void
TOD::_didReset(bool hard)
{    
    tod.hour = 1;
    stopped = true;
}

void
TOD::increment()
{
    auto incBCD = [&](u8 x) {

        return ((x & 0xF) == 9) ? u8((x & 0xF0) + 0x10) : u8((x & 0xF0) + ((x + 1) & 0xF));
    };

    // Check if a tenth of a second has passed
    if (stopped || cpu.clock < nextTodTrigger) return;
    
    cia.wakeUp();
    
    assert(!stopped);

    // 1/10 seconds
    if (tod.tenth != 0x09) {
        tod.tenth = incBCD(tod.tenth);
    } else {
        tod.tenth = 0;
        
        // Seconds
        if (tod.sec != 0x59) {
            tod.sec = incBCD(tod.sec) & 0x7F;
        } else {
            tod.sec = 0;
            
            // Minutes
            if (tod.min != 0x59) {
                tod.min = incBCD(tod.min) & 0x7F;
            } else {
                tod.min = 0;
                
                // Hours
                u8 pm = tod.hour & 0x80;
                u8 hr = tod.hour & 0x1F;
                
                if (hr == 0x11) {
                    pm ^= 0x80;
                }
                if (hr == 0x12) {
                    hr = 0x01;
                } else if (hr == 0x09) {
                    hr = 0x10;
                } else {
                    u8 hr_lo = hr & 0x0F;
                    u8 hr_hi = hr & 0x10;
                    hr = hr_hi | ((hr_lo + 1) & 0x0F);
                }
                
                tod.hour = pm | hr;
            }
        }
    }

    checkIrq();
    nextTodTrigger += powerSupply.todTickDelay(cia.CRA);
}

void
TOD::cont()
{
    stopped = false;
    nextTodTrigger = cpu.clock + powerSupply.todTickDelay(cia.CRA);
}

void
TOD::checkIrq()
{
    if (!matching && tod.value == alarm.value) {
        cia.todInterrupt();
    }
    
    matching = (tod.value == alarm.value);
}

}
