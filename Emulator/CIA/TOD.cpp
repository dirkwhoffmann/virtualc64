// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "C64.h"

TOD::TOD(CIA *cia)
{
	setDescription("TOD");
	debug(3, "    Creating TOD at address %p...\n", this);
    
    this->cia = cia;
    
    // Register snapshot items
    SnapshotItem items[] = {
                
        { &tod.value,        sizeof(tod.value),        CLEAR_ON_RESET },
        { &latch.value,      sizeof(latch.value),      CLEAR_ON_RESET },
        { &alarm.value,      sizeof(alarm.value),      CLEAR_ON_RESET },
        { &frozen,           sizeof(frozen),           CLEAR_ON_RESET },
        { &stopped,          sizeof(stopped),          CLEAR_ON_RESET },
        { &matching,         sizeof(matching),         CLEAR_ON_RESET },
        { &hz,               sizeof(hz),               CLEAR_ON_RESET },
        { &frequencyCounter, sizeof(frequencyCounter), CLEAR_ON_RESET },
        { NULL,              0,                        0 }};
    
    registerSnapshotItems(items, sizeof(items));
}

void
TOD::reset() 
{
    VirtualComponent::reset();
    tod.hours = 1;
    stopped = true;
    hz = 60;
}

void 
TOD::dump()
{
	msg("            Time of day : %02X:%02X:%02X:%02X\n",
        tod.hours, tod.minutes, tod.seconds, tod.tenth);
	msg("                  Alarm : %02X:%02X:%02X:%02X\n",
        alarm.hours, alarm.minutes, alarm.seconds, alarm.tenth);
	msg("                  Latch : %02X:%02X:%02X:%02X\n",
        latch.hours, latch.minutes, latch.seconds, latch.tenth);
	msg("                 Frozen : %s\n", frozen ? "yes" : "no");
	msg("                Stopped : %s\n", stopped ? "yes" : "no");
	msg("\n");
}

TODInfo
TOD::getInfo()
{
    TODInfo info;
    
    info.time = tod;
    info.latch = latch;
    info.alarm = alarm;
    
    return info;
}


void
TOD::increment()
{
    if (stopped)
        return;
    
    if (++frequencyCounter % hz != 0)
        return;
    
    // 1/10 seconds
    if (tod.tenth != 0x09) {
        tod.tenth = incBCD(tod.tenth);
    } else {
        tod.tenth = 0;
        
        // Seconds
        if (tod.seconds != 0x59) {
            tod.seconds = incBCD(tod.seconds) & 0x7F;
        } else {
            tod.seconds = 0;
            
            // Minutes
            if (tod.minutes != 0x59) {
                tod.minutes = incBCD(tod.minutes) & 0x7F;
            } else {
                tod.minutes = 0;
                
                // Hours
                uint8_t pm = tod.hours & 0x80;
                uint8_t hr = tod.hours & 0x1F;
                
                if (hr == 0x11) {
                    pm ^= 0x80;
                }
                if (hr == 0x12) {
                    hr = 0x01;
                } else if (hr == 0x09) {
                    hr = 0x10;
                } else {
                    uint8_t hr_lo = hr & 0x0F;
                    uint8_t hr_hi = hr & 0x10;
                    hr = hr_hi | ((hr_lo + 1) & 0x0F);
                }
                
                tod.hours = pm | hr;
            }
        }
    }

    checkForInterrupt(); 
    return;
}

void
TOD::checkForInterrupt()
{
    if (!matching && tod.value == alarm.value) {
        cia->todInterrupt();
    }
    
    matching = (tod.value == alarm.value);
}
