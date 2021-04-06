// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "TOD.h"
#include "CIA.h"

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
    
    tod.hour = 1;
    stopped = true;
    hz = 60;
}

void
TOD::_dump(dump::Category category, std::ostream& os) const
{
	msg("            Time of day : %02X:%02X:%02X:%02X\n",
        tod.hour, tod.min, tod.sec, tod.tenth);
	msg("                  Alarm : %02X:%02X:%02X:%02X\n",
        alarm.hour, alarm.min, alarm.sec, alarm.tenth);
	msg("                  Latch : %02X:%02X:%02X:%02X\n",
        latch.hour, latch.min, latch.sec, latch.tenth);
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
    return;
}

void
TOD::checkIrq()
{
    if (!matching && tod.value == alarm.value) {
        cia.todInterrupt();
    }
    
    matching = (tod.value == alarm.value);
}
