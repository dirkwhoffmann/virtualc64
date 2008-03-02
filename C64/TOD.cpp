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
	reset();
}

TOD::~TOD()
{
}

void
TOD::reset() 
{
	tod.value = 0;
	alarm.value = 0;
	frozen = false;
	stopped = false;
}

// Loading and saving snapshots
bool
TOD::load(FILE *file)
{
	tod.value = read64(file);
	alarm.value = read64(file);
	frozen = read8(file);
	stopped = read8(file);
	return true;
}

bool
TOD::save(FILE *file)
{
	write64(file, tod.value);
	write64(file, alarm.value);
	write8(file, frozen);
	write8(file, stopped);
	return true;
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
