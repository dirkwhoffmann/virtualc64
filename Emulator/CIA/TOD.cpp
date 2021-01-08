// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "C64.h"

TOD::TOD(C64 &ref, CIA &ciaref) : C64Component(ref), cia(ciaref)
{
}

const char *
TOD::getDescription() const
{
    return cia.isCIA1() ? "TOD1" : "TOD2";
}

void
TOD::_inspect()
{
    synchronized {
        
        info.time = tod;
        info.latch = latch;
        info.alarm = alarm;
    }
}

void
TOD::_reset() 
{
    RESET_SNAPSHOT_ITEMS
    
    tod.hours = 1;
    stopped = true;
    hz = 60;
}

void 
TOD::_dump() const
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
                u8 pm = tod.hours & 0x80;
                u8 hr = tod.hours & 0x1F;
                
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
        cia.todInterrupt();
    }
    
    matching = (tod.value == alarm.value);
}
