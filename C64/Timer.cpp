/*
 * (C) 2009 Dirk W. Hoffmann. All rights reserved.
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

Timer::Timer()
{
	name = "Timer";

	debug(2, "    Creating CIA Timer at address %p...\n", this);

	cia = NULL;
	otherTimer = NULL;
}

Timer::~Timer()
{
}

void
Timer::reset() 
{
	debug(2, "    Resetting CIA Timer...\n");
	counter = 0x0000;
	latch = 0xFFFF;
	// triggerInterrupt = false;
}

bool
Timer::load(uint8_t **buffer)
{
	debug(2, "    Loading timer state...\n");

	counter = read16(buffer);
	latch = read16(buffer);

	return true;
}

bool
Timer::save(uint8_t **buffer)
{
	debug(2, "    Saving timer state...\n");
	
	write16(buffer, counter);
	write16(buffer, latch);
	
	return true;
}

void
Timer::dumpState()
{
	debug(1, "            Timer value : %04X\n", counter);
	debug(1, "            Timer latch : %04X\n", latch);
	debug(1, "\n");
}

// -----------------------------------------------------------------------------------------
// Timer A
// -----------------------------------------------------------------------------------------

bool TimerA::isStarted() { return cia->bCRA & 0x01; }
void TimerA::setStarted(bool b) { if (b) cia->bCRA |= 0x01; else cia->bCRA &= 0xFE; }
bool TimerA::forceLoadStrobe() { return cia->bCRA & 0x10; }
bool TimerA::willIndicateUnderflow() { return cia->bCRA & 0x02; }
bool TimerA::willIndicateUnderflowAsPulse() { return !(cia->bCRA & 0x04); }
void TimerA::setIndicateUnderflow(bool b) { if (b) cia->bCRA |= 0x02; else cia->bCRA &= (0xFF-0x02); }
bool TimerA::isOneShot() { return cia->bCRA & 0x08; }
void TimerA::setOneShot(bool b) { if (b) cia->bCRA |= 0x08; else cia->bCRA &= (0xff-0x08); }
bool TimerA::isCountingClockTicks() { return (cia->bCRA & 0x20) == 0x00; }
uint16_t TimerA::getControlReg() { return cia->bCRA; }


// -----------------------------------------------------------------------------------------
// Timer B
// -----------------------------------------------------------------------------------------

bool TimerB::isStarted() { return cia->bCRB & 0x01; }
void TimerB::setStarted(bool b) { if (b) cia->bCRB |= 0x01; else cia->bCRB &= 0xFE; }
bool TimerB::forceLoadStrobe() { return cia->bCRB & 0x10; }
bool TimerB::willIndicateUnderflow() { return cia->bCRB & 0x02; }
bool TimerB::willIndicateUnderflowAsPulse() { return !(cia->bCRB & 0x04); }
void TimerB::setIndicateUnderflow(bool b) { if (b) cia->bCRB |= 0x02; else cia->bCRB &= (0xFF-0x02); }
bool TimerB::isOneShot() { return cia->bCRB & 0x08; }
void TimerB::setOneShot(bool b) { if (b) cia->bCRB |= 0x08; else cia->bCRB &= (0xff-0x08); }
bool TimerB::isCountingClockTicks() { return (cia->bCRB & 0x20) == 0x00; }
uint16_t TimerB::getControlReg() { return cia->bCRB; }	

