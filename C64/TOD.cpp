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
	debug(2, "    Creating TOD at address %p...\n", this);
	name = "TOD";
}

TOD::~TOD()
{
}

void
TOD::reset() 
{
	debug(2, "    Resetting TOD...\n");
	tod.value = 0;
	alarm.value = 0;
	latch.value = 0;
	frozen = false;
	stopped = false;
}

// Loading and saving snapshots
bool
TOD::load(uint8_t **buffer)
{
	debug(2, "    Loading TOD state...\n");

	tod.value = read32(buffer);
	alarm.value = read32(buffer);
	latch.value = read32(buffer);
	frozen = read8(buffer);
	stopped = read8(buffer);
	return true;
}

bool
TOD::save(uint8_t **buffer)
{
	debug(2, "    Saving TOD state...\n");

	write32(buffer, tod.value);
	write32(buffer, alarm.value);
	write32(buffer, latch.value);
	write8(buffer, frozen);
	write8(buffer, stopped);
	return true;
}

void 
TOD::dumpState()
{
	debug(1, "            Time of day : %02X:%02X:%02X:%02X\n", tod.time.hours, tod.time.minutes, tod.time.seconds, tod.time.tenth);
	debug(1, "                  Alarm : %02X:%02X:%02X:%02X\n", alarm.time.hours, alarm.time.minutes, alarm.time.seconds, alarm.time.tenth);
	debug(1, "                  Latch : %02X:%02X:%02X:%02X\n", latch.time.hours, latch.time.minutes, latch.time.seconds, latch.time.tenth);
	debug(1, "                 Frozen : %s\n", isFrozen() ? "yes" : "no");
	debug(1, "                Stopped : %s\n", stopped ? "yes" : "no");
	debug(1, "\n");
}

bool 
TOD::increment()
{
	if (stopped)
		return false;
	
	if (tod.time.tenth == 9) {
		tod.time.tenth = 0;
		if (tod.time.seconds == 59) {
			tod.time.seconds = 0;
			if (tod.time.minutes == 59) {
				tod.time.minutes = 0;
				if (tod.time.hours == 11) {
					tod.time.hours = ((tod.time.hours & 0x80) ? 0 : 0x80);
				} else {
					tod.time.hours++;
				}
			} else {
				tod.time.minutes++;
			}
		} else {
			tod.time.seconds++;
		}
	} else {
		tod.time.tenth++;
	}
	
	return (tod.value == alarm.value);
}
