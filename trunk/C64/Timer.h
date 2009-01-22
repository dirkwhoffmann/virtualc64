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

#ifndef _TIMER_INC
#define _TIMER_INC

#include "VirtualComponent.h"

// Timer states (derived from Frodo SC)
#define TIMER_STOP 0
#define TIMER_COUNT 1
#define TIMER_COUNT_STOP 2
#define TIMER_LOAD_STOP 3
#define TIMER_LOAD_COUNT 4
#define TIMER_WAIT_COUNT 5
#define TIMER_LOAD_WAIT_COUNT 6

class CIA;
class CIA1;
class CIA2;

//! CIA Timer
/*! Each CIA chip contains two timers (A and B). */
class Timer : public VirtualComponent {
	
	friend class CIA;
	friend class CIA1;
	friend class CIA2;
	
protected:

	//! Reference to the connected CIA chip
	CIA *cia;
	
	//! Reference to the other timer of the same CIA chip
	Timer *otherTimer;

	//! Timer value
	uint16_t count;

	//! Timer latch
	uint16_t timerLatch;

	//! Current execution state
	int state;
	
	//! The timer control register
	uint8_t controlReg;

	//! True, if the timer is decremented in each clock cycle
	bool count_clockticks;
	
	//! True, if the timer is decremented if the "otherTimer" experiences a timer underflow.
	bool count_underflows;
		
	//! Is set to true when an underflow condition occurs
	bool underflow;
	
	//! Is inverted when an underflow condition occurs
	bool underflow_toggle;
	
	//! Indicates that an interrupt needs to be triggered in the next cycle
	bool triggerInterrupt;

public:
	//! Constructor
	Timer();
	
	//! Destructor
	~Timer();
	
	//! Reset timer to its initial state
	void reset();
	
	//! Load snapshot from file
	bool load(FILE *file);
	
	//! Save snapshot to file
	bool save(FILE *file);
	
	//! Dump internal state to console
	void dumpState();
	
	//! Set reference to the CIA chip
	void setCIA(CIA *c) { cia = c; }

	//! Set reference to the other timer
	void setOtherTimer(Timer *timer) { otherTimer = timer; }
	
	//! Set low byte of timer latch
	void setTimerLatchLo(uint8_t value) { timerLatch = (timerLatch & 0xFF00) | value; }

	//! Set high byte of timer latch
	void setTimerLatchHi(uint8_t value) { timerLatch = (value << 8) | (timerLatch & 0xFF); if (!(controlReg & 1)) count = timerLatch; }

	//! Return current timer state
	inline uint16_t getState() { return state; }
		
	//! Return the current timer value
	inline uint16_t getTimer() { return count; }
	
	//! Set the current timer value
	inline void setTimer(uint16_t value) { count = value; }
	
	//! Return the value of the timer latch
	inline uint16_t getTimerLatch() { return timerLatch; }
	
	//! Set the value of the timer latch
	inline void setTimerLatch(uint16_t value) { timerLatch = value; }
	
	//! Load latched value into timer 
	inline void reloadTimer() { count = timerLatch; }
	
	//! Returns true, if timer is running, 0 if stopped
	inline bool isStarted() { return controlReg & 0x01; }
	
	//! Start or stop timer
	inline void setStarted(bool b) { if (b) controlReg |= 0x01; else controlReg &= 0xFE; }
	
	//! Toggle start flag
	inline void toggleStartFlag() { setStarted(!isStarted()); }

	//! Returns true, if the force load strobe is 1
	inline bool forceLoadStrobe() { return controlReg & 0x10; }
	
	//! Returns true, if an underflow will be indicated in bit #6 in Port B register
	inline bool willIndicateUnderflow() { return controlReg & 0x02; }
	
	//! Returns true, if an underflow will be indicated as a single pulse
	inline bool willIndicateUnderflowAsPulse() { return !(controlReg & 0x04); }
	
	//! Enable or disable underflow indication
	inline void setIndicateUnderflow(bool b) { if (b) controlReg |= 0x02; else controlReg &= (0xFF-0x02); }
	
	//! Toggle underflow indication flag
	inline void toggleUnderflowFlag() { setIndicateUnderflow(!willIndicateUnderflow()); }
	
	//! Returns true, if A is in "one shot" mode
	inline bool isOneShot() { return controlReg & 0x08; }
	
	//! Enable or disable one-shot-mode 
	inline void setOneShot(bool b) { if (b) controlReg |= 0x08; else controlReg &= (0xff-0x08); }
	
	//! Toggle one shot flag 
	inline void toggleOneShotFlag() { setOneShot(!isOneShot()); }
	
	//! Returns true, if timer counts clock ticks
	inline bool isCountingClockTicks() { return count_clockticks; }
	
	//! Return contents of the timer control register
	inline uint16_t getControlReg() { return controlReg; }

	//! Update value of the control register 
	/*! This method is executed whenever the value of the control register changes. It updates the internal state
	    of the timer and performs the necessary actions. */ 
	void setControlReg(uint8_t value);
		
	//! Execute one cycle
	void executeOneCycle(void);

	//! Action to be taken when an underflow occurs
	virtual void timerAction() = 0;
	
	//! Set the "signal pending bit" for this timer
	virtual void setSignalPending() = 0;

	//! Get the control register of the corresponding timer in the CIA's IO memory space
	virtual uint8_t getCIAControlReg() = 0;

	//! Set the control register of the corresponding timer in the CIA's IO memory space
	virtual void setCIAControlReg(uint8_t value) = 0;

};


//! Timer A
class TimerA : public Timer {
		
public:

	void setCountingModes(uint8_t controlBits);
	void timerAction();
	void setSignalPending();
	uint8_t getCIAControlReg();
	void setCIAControlReg(uint8_t value);
};

//! Timer B
class TimerB : public Timer {
	
public:
	
	void setCountingModes(uint8_t controlBits);
	void timerAction();
	void setSignalPending();
	uint8_t getCIAControlReg();
	void setCIAControlReg(uint8_t value);
};

#endif
