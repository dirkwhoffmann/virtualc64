/*
 * (C) 2007 Dirk W. Hoffmann. All rights reserved.
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

#ifndef _TOD_INC
#define _TOD_INC

#include "VirtualComponent.h"

//! Time of day clock (TOD)
/*! Each CIA chip contains a time of day clock, counting hours, minutes, seconds and tenth of a second.
Every TOD clock features an alarm mechanism. When the alarm time is reached, an interrupt is initiated.
*/
class TOD : public VirtualComponent {
	
private:
	typedef	union {
		struct { 
			uint8_t tenth;
			uint8_t seconds;
			uint8_t minutes;
			uint8_t hours;
		} time;
		uint32_t value;
	} TimeOfDay;
	
	TimeOfDay tod;
	TimeOfDay alarm;
	TimeOfDay latch;
	
	//! True, if the TOD registers are frozen
	/*! The CIA chip freezes the registers when the hours-part is read and reactivates them, when the 1/10th part is read.
		Although the values stay constant, the internal clock continues to run.
        Purpose: If you start reading with the hours-part, the clock won't change until you have read the whole time.
	*/
	bool frozen;
	
	//! True, if the TOD clock is stopped
	/*! The CIA chip stops the TOD clock, when the hours-part is written and restarts it, when the 1/10th part is written.
        Purpose: The clock will only start running when the time is completely set.
	*/
	bool stopped;
	

public:
	//! Constructor
	TOD();
	
	//! Destructor
	~TOD();
	
	//! Bring the CIA back to its initial state
	void reset();
	
	//! Load snapshot
	bool load(uint8_t **buffer);
	
	//! Save snapshot
	bool save(uint8_t **buffer);
	
	//! Dump internal state to console
	void dumpState();	
	
	inline void freeze() { latch.value = tod.value; frozen = true; }
	inline void defreeze() { frozen = false; }
	inline void stop() { stopped = true; }
	inline void cont() { stopped = false; }
		
	//! Returns the hours digits of the time of day clock
	/*! Note: The TOD clock freezes on a read or write access */
	uint8_t getTodHours() { return tod.time.hours; }
		
	//! Sets the hours digits of the time of day clock
	/*! Note: The TOD clock freezes on a read or write access */
	void setTodHours(uint8_t value) { tod.time.hours = value; }
	
	//! Returns the minutes digits of the time of day clock
	inline uint8_t getTodMinutes() { return frozen ? latch.time.minutes : tod.time.minutes; }
	
	//! Sets the minutes digits of the time of day clock
	inline void setTodMinutes(uint8_t value) { tod.time.minutes = value; }
	
	//! Returns the seconds digits of the time of day clock
	inline uint8_t getTodSeconds() { return frozen ? latch.time.seconds : tod.time.seconds; }
	
	//! Sets the seconds digits of the time of day clock
	inline void setTodSeconds(uint8_t value) { tod.time.seconds = value; }
	
	//! Returns the tenth-of-a-second digits of the time of day clock
	/*! Note: The TOD clock unfreezes on a read or write access */
	uint8_t getTodTenth() { return tod.time.tenth; }
	
	//! Sets the tenth-of-a-second digits of the time of day clock
	/*! Note: The TOD clock unfreezes on a read or write access */
	void setTodTenth(uint8_t value) { tod.time.tenth = value; }
	
	//! Returns the hours digits of the alarm time
	inline uint8_t getAlarmHours() { return alarm.time.hours; }
	
	//! Sets the hours digits of the alarm time
	inline void setAlarmHours(uint8_t value) { alarm.time.hours = value; }
	
	//! Returns the minutes digits of the alarm time
	inline uint8_t getAlarmMinutes() { return alarm.time.minutes; }
	
	//! Sets the minutes digits of the alarm time
	inline void setAlarmMinutes(uint8_t value) { alarm.time.minutes = value; }
	
	//! Returns the seconds digits of the alarm time
	inline uint8_t getAlarmSeconds() { return alarm.time.seconds; }
	
	//! Sets the seconds digits of the alarm time
	inline void setAlarmSeconds(uint8_t value) { alarm.time.seconds = value; }
	
	//! Returns the tenth-of-a-second digits of the alarm time
	inline uint8_t getAlarmTenth() { return alarm.time.tenth; }
	
	//! Sets the tenth-of-a-second digits of the time of day clock
	inline void setAlarmTenth(uint8_t value) { alarm.time.tenth = value; }
	
	//! Returns true, iff the TOD clock is currently frozen
	inline bool isFrozen() { return frozen; }
	
	//! Increment the TOD clock by one tenth of a second
	/*! The function increments the TOD clock. It returns true iff the currently set alarm time is reached.
		The function is supposed to be invoked whenever a frame is finished (during VBlank) 
		*/
	bool increment();
	
};

#endif


