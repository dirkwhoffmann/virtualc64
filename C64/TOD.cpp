/*
 * (C) 2006 Dirk W. Hoffmann. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "C64.h"

TOD::TOD()
{
	setDescription("TOD");
	debug(3, "    Creating TOD at address %p...\n", this);
    
    // Register snapshot items
    SnapshotItem items[] = {
                
        { &tod.value,        sizeof(tod.value),        CLEAR_ON_RESET },
        { &tod.time.oldValue, sizeof(tod.time.oldValue),     CLEAR_ON_RESET },
        { &alarm.value,      sizeof(alarm.value),      CLEAR_ON_RESET },
        { &latch.value,      sizeof(latch.value),      CLEAR_ON_RESET },
        { &frozen,           sizeof(frozen),           CLEAR_ON_RESET },
        { &stopped,          sizeof(stopped),          CLEAR_ON_RESET },
        { &hz,               sizeof(hz),               CLEAR_ON_RESET },
        { &frequencyCounter, sizeof(frequencyCounter), CLEAR_ON_RESET },
        { NULL,              0,                        0 }};
    
    registerSnapshotItems(items, sizeof(items));
}

TOD::~TOD()
{
}

void
TOD::reset() 
{
    VirtualComponent::reset();
    tod.time.hours = 1;
    stopped = true;
    hz = 60;
}

void 
TOD::dumpState()
{
	msg("            Time of day : %02X:%02X:%02X:%02X\n", tod.time.hours, tod.time.minutes, tod.time.seconds, tod.time.tenth);
	msg("                  Alarm : %02X:%02X:%02X:%02X\n", alarm.time.hours, alarm.time.minutes, alarm.time.seconds, alarm.time.tenth);
	msg("                  Latch : %02X:%02X:%02X:%02X\n", latch.time.hours, latch.time.minutes, latch.time.seconds, latch.time.tenth);
	msg("                 Frozen : %s\n", isFrozen() ? "yes" : "no");
	msg("                Stopped : %s\n", stopped ? "yes" : "no");
	msg("\n");
}

bool
TOD::increment()
{
    tod.time.oldValue = tod.value;
    
    if (stopped)
		return false;
    
    if (++frequencyCounter % hz != 0)
        return false;

    // 1/10 seconds
	if (tod.time.tenth != 0x09) {
        tod.time.tenth = incBCD(tod.time.tenth);
        return true;
    }
    tod.time.tenth = 0;
    
    // Seconds
    if (tod.time.seconds != 0x59) {
        tod.time.seconds = incBCD(tod.time.seconds) & 0x7F;
        return true;
    }
    tod.time.seconds = 0;
    
    // Minutes
    if (tod.time.minutes != 0x59) {
        tod.time.minutes = incBCD(tod.time.minutes) & 0x7F;
        return true;
    }
    tod.time.minutes = 0;
	
    // Hours
    uint8_t pm = tod.time.hours & 0x80;
    uint8_t hr = tod.time.hours & 0x1F;
				
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
                
    tod.time.hours = pm | hr;
    return true;
}

bool
TOD::alarming()
{
    return tod.value == alarm.value;
}

