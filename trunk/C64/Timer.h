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
	uint16_t counter;

	//! Timer latch
	uint16_t latch;
	
public:
	//! Constructor
	Timer();
	
	//! Destructor
	~Timer();
	
	//! Reset timer to its initial state
	void reset();
	
	//! Load snapshot
	bool load(uint8_t **buffer);
	
	//! Save snapshot
	bool save(uint8_t **buffer);
	
	//! Dump internal state to console
	void dumpState();
	
	//! Set reference to the CIA chip
	void setCIA(CIA *c) { cia = c; }

	//! Set reference to the other timer
	void setOtherTimer(Timer *timer) { otherTimer = timer; }

	//! Return latch value
	inline uint16_t getLatch() { return latch; }
	
	//! Set latch value
	inline void setLatch(uint16_t value) { latch = value; }
	
	//! Get low byte of latch
	uint8_t getLatchLo() { return (uint8_t)(latch & 0xFF); }

	//! Set low byte of latch
	void setLatchLo(uint8_t value) { latch = (latch & 0xFF00) | value; }

	//! Get high byte of latch
	uint8_t getLatchHi() { return (uint8_t)(latch >> 8); }

	//! Set high byte of latch
	void setLatchHi(uint8_t value) { latch = (value << 8) | (latch & 0xFF); }
		
	//! Return current timer value
	inline uint16_t getCounter() { return counter; }
	
	//! Set current timer value
	inline void setCounter(uint16_t value) { counter = value; }
		
	//! Get low byte of current timer value
	uint8_t getCounterLo() { return (uint8_t)(counter & 0xFF); }
	
	//! Set low byte of current timer value
	void setCounterLo(uint8_t value) { counter = (counter & 0xFF00) | value; }
	
	//! Get high byte of current timer value
	uint8_t getCounterHi() { return (uint8_t)(counter >> 8); }
	
	//! Set high byte of current timer value
	void setCounterHi(uint8_t value) { counter = (value << 8) | (counter & 0xFF); }
		
	//! Load latched value into timer 
	inline void reloadTimer() { counter = latch; }
	
	//! Returns true, if timer is running, 0 if stopped
	virtual bool isStarted() = 0; 
	
	//! Start or stop timer
	virtual void setStarted(bool b) = 0;
	
	//! Toggle start flag
	inline void toggleStartFlag() { setStarted(!isStarted()); }

	//! Returns true, if the force load strobe is 1
	virtual bool forceLoadStrobe() = 0;
	
	//! Returns true, if an underflow will be indicated in bit #6 in Port B register
	virtual bool willIndicateUnderflow() = 0;
	
	//! Returns true, if an underflow will be indicated as a single pulse
	virtual bool willIndicateUnderflowAsPulse() = 0;
	
	//! Enable or disable underflow indication
	virtual void setIndicateUnderflow(bool b) = 0;
	
	//! Toggle underflow indication flag
	inline void toggleUnderflowFlag() { setIndicateUnderflow(!willIndicateUnderflow()); }
	
	//! Returns true, if A is in "one shot" mode
	virtual bool isOneShot() = 0;
	
	//! Enable or disable one-shot-mode 
	virtual void setOneShot(bool b) = 0;
	
	//! Toggle one shot flag 
	inline void toggleOneShotFlag() { setOneShot(!isOneShot()); }
	
	//! Returns true, if timer counts clock ticks
	virtual bool isCountingClockTicks() = 0;
	
	//! Return contents of the timer control register
	virtual uint16_t getControlReg() = 0;

	//! Update value of the control register 
	/*! This method is executed whenever the value of the control register changes. It updates the internal state
	    of the timer and performs the necessary actions. */ 
	//void setControlReg(uint8_t value);
		
	//! Execute one cycle
	//void executeOneCycle(void);

	//! Action to be taken when an underflow occurs
	//virtual void timerAction() = 0;
	
	//! Set the "signal pending bit" for this timer
	//virtual void setSignalPending() = 0;

	//! Get the control register of the corresponding timer in the CIA's IO memory space
	//virtual uint8_t getCIAControlReg() = 0;

	//! Set the control register of the corresponding timer in the CIA's IO memory space
	//virtual void setCIAControlReg(uint8_t value) = 0;

};


//! Timer A
class TimerA : public Timer {
		
public:

	//void setCountingModes(uint8_t controlBits);
	//void timerAction();
	//void setSignalPending();
	//uint8_t getCIAControlReg();
	//void setCIAControlReg(uint8_t value);
	
	bool isStarted();
	void setStarted(bool b);
	bool forceLoadStrobe();
	bool willIndicateUnderflow();
	bool willIndicateUnderflowAsPulse();
	void setIndicateUnderflow(bool b);
	bool isOneShot();
	void setOneShot(bool b);
	bool isCountingClockTicks();
	uint16_t getControlReg();
};

//! Timer B
class TimerB : public Timer {
	
public:
		
	bool isStarted();
	void setStarted(bool b);
	bool forceLoadStrobe();
	bool willIndicateUnderflow();
	bool willIndicateUnderflowAsPulse();
	void setIndicateUnderflow(bool b);
	bool isOneShot();
	void setOneShot(bool b);
	bool isCountingClockTicks();
	uint16_t getControlReg();
};

#endif
