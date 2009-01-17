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
	debug("    Creating CIA Timer at address %p...\n", this);
}

Timer::~Timer()
{
}

void
Timer::reset() 
{
	debug("    Resetting CIA Timer...\n");
	count = timerLatch = 0;
	controlReg = 0x00;
	underflow = false;
	underflow_toggle = true; 
	count_clockticks = false;
	count_underflows = false;
	triggerInterrupt = false;
	state = TIMER_STOP;
}

bool
Timer::load(FILE *file)
{
	// TODO
	return true;
}

bool
Timer::save(FILE *file)
{
	// TODO
	return true;
}

void 
Timer::executeOneCycle(void)
{
	switch (state) {
		case TIMER_COUNT:
			if (count_clockticks || (count_underflows && otherTimer->underflow)) 
				if (!count || !--count) {
					timerAction();
					underflow = true;
					underflow_toggle = !underflow_toggle;
				}
			break;
			
		case TIMER_COUNT_STOP:
			state = TIMER_STOP;
			if (count_clockticks || (count_underflows && otherTimer->underflow)) 
				if (!count || !--count) {
					underflow = true;
					underflow_toggle = !underflow_toggle;
				}
			break;

		case TIMER_LOAD_STOP:
			reloadTimer();
			underflow = false;
			state = TIMER_STOP;
			break;

		case TIMER_LOAD_COUNT:
			reloadTimer();
			underflow = false;
			state = TIMER_COUNT;
			break;

		case TIMER_WAIT_COUNT:
			state = TIMER_COUNT;
			break;
			
		case TIMER_LOAD_WAIT_COUNT:
			state = TIMER_WAIT_COUNT;
			if (count == 1) {
				timerAction();
				underflow = true;
				underflow_toggle = !underflow_toggle;
			} else {
				reloadTimer();
			}
			break;
	}
}

void 
Timer::setControlReg(uint8_t value)
{	
	// Transition table (derived from Frodo SC)
	static const int table[7][4] = {
		// yes                 yes                    no                     no                     // running?
		// yes                 no                     yes                    no                     // force load strobe?
		TIMER_LOAD_WAIT_COUNT, TIMER_WAIT_COUNT,      TIMER_LOAD_STOP,       TIMER_STOP,            // from TIMER_STOP
		TIMER_LOAD_WAIT_COUNT, TIMER_COUNT,           TIMER_LOAD_STOP,       TIMER_COUNT_STOP,      // from TIMER_COUNT
		TIMER_COUNT_STOP,      TIMER_COUNT_STOP,      TIMER_COUNT_STOP,      TIMER_COUNT_STOP,      // from TIMER_COUNT_STOP
		TIMER_LOAD_WAIT_COUNT, TIMER_WAIT_COUNT,      TIMER_LOAD_STOP,       TIMER_LOAD_STOP,       // from TIMER_LOAD_STOP
		TIMER_STOP,            TIMER_STOP,            TIMER_STOP,            TIMER_STOP,            // from TIMER_LOAD_COUNT
		TIMER_STOP,            TIMER_STOP,            TIMER_STOP,            TIMER_STOP,            // from TIMER_WAIT_COUNT
		TIMER_LOAD_WAIT_COUNT, TIMER_LOAD_WAIT_COUNT, TIMER_LOAD_WAIT_COUNT, TIMER_LOAD_WAIT_COUNT, // from TIMER_LOAD_WAIT_COUNT
	};
	
	controlReg = value;

	// handle special cases
	if ((value & 1) && (state == TIMER_LOAD_COUNT || state == TIMER_WAIT_COUNT)) {
		if (value & 8) {		// One-shot?
			value &= 0xfe;	// Yes, stop timer
			state = TIMER_STOP;
		} else if (value & 0x10)	// Force load
			state = TIMER_LOAD_WAIT_COUNT;
		return;
	}
	
	// handle general cases
	
	// convert relevant bits into table index
	// int i = (isStarted() ? 0 : 2) + (forceLoadStrobe() ? 0 : 1);
	int i = ((value & 1) ? 0 : 2) + ((value & 0x10) ? 0 : 1);
	state = table[state][i];
	return; 
	
}


// -----------------------------------------------------------------------------------------
// Timer A
// -----------------------------------------------------------------------------------------

void 
TimerA::setCountingModes(uint8_t controlBits)
{
	count_clockticks = (controlBits & 0x20) == 0x00;
	count_underflows = false;
}

void 
TimerA::timerAction()
{
	reloadTimer();
	triggerInterrupt = true;
	cia->setSignalPendingA(true);
	if (isOneShot()) {
		controlReg &= 0xFE;	
		cia->setControlRegA(cia->getControlRegA() & 0xFE);		
		state = TIMER_LOAD_STOP;
	} else {
		state =  TIMER_LOAD_COUNT;
	}
}

void 
TimerA::setSignalPending()
{
	cia->setSignalPendingA(true);
}

uint8_t 
TimerA::getCIAControlReg()
{
	return cia->getControlRegA();	
}
void 
TimerA::setCIAControlReg(uint8_t value)
{
	cia->setControlRegA(value);	
}

// -----------------------------------------------------------------------------------------
// Timer B
// -----------------------------------------------------------------------------------------

void 
TimerB::setCountingModes(uint8_t controlBits)
{
	count_clockticks = (controlBits & 0x60) == 0x00;
	count_underflows = (controlBits & 0x60) == 0x40;
}

void 
TimerB::timerAction()
{
	reloadTimer();
	triggerInterrupt = true;
	cia->setSignalPendingB(true);
	if (isOneShot()) {
		controlReg &= 0xFE;	
		cia->setControlRegB(cia->getControlRegB() & 0xFE);	
		state = TIMER_LOAD_STOP;
	} else {
		state =  TIMER_LOAD_COUNT;
	}
}

void 
TimerB::setSignalPending()
{
	cia->setSignalPendingB(true);
}

uint8_t 
TimerB::getCIAControlReg()
{
	return cia->getControlRegB();	
}
void 
TimerB::setCIAControlReg(uint8_t value)
{
	cia->setControlRegB(value);	
}
