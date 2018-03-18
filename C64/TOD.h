/*!
 * @header      TOD.h
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   2007 - 2016 Dirk W. Hoffmann
 */
/*              This program is free software; you can redistribute it and/or modify
 *              it under the terms of the GNU General Public License as published by
 *              the Free Software Foundation; either version 2 of the License, or
 *              (at your option) any later version.
 *
 *              This program is distributed in the hope that it will be useful,
 *              but WITHOUT ANY WARRANTY; without even the implied warranty of
 *              MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *              GNU General Public License for more details.
 *
 *              You should have received a copy of the GNU General Public License
 *              along with this program; if not, write to the Free Software
 *              Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _TOD_INC
#define _TOD_INC

#include "VirtualComponent.h"

/*! @brief    Time of day clock (TOD)
 *  @details  Each CIA chip contains a time of day clock, counting hours, minutes, 
 *            seconds and tenth of a second. Every TOD clock features an alarm mechanism. 
 *            When the alarm time is reached, an interrupt is initiated.
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
	
	/*! @brief    If set to true, the TOD registers are frozen
	 *  @details  The CIA chip freezes the registers when the hours-part is read and reactivates them,
     *            when the 1/10th part is read. Although the values stay constant, the internal clock 
     *            continues to run. Purpose: If you start reading with the hours-part, the clock won't 
     *            change until you have read the whole time.
     */
	bool frozen;
	
	/*! @brief    If set to true, the TOD clock is stopped
	 *  @details  The CIA chip stops the TOD clock, when the hours-part is written and restarts it, 
     *            when the 1/10th part is written. Purpose: The clock will only start running when the 
     *            time is completely set.
     */
	bool stopped;
	

public:
	//! @brief    Constructor
	TOD();
	
	//! @brief    Destructor
	~TOD();
	
	//! @brief    Restores the initial state.
	void reset();
	
    //! @brief    Returns the size of the internal state.
    // TODO: Get rid of this. Should be calculated automatically
    size_t stateSize();
    
	//! @brief    Prints debug information.
	void dumpState();	
	
    //! @brief    Freezes the time of day clock.
	inline void freeze() { latch.value = tod.value; frozen = true; }

    //! @brief    Defreezes the time of day clock.
    inline void defreeze() { frozen = false; }

    //! @brief    Stops the time of day clock.
    inline void stop() { stopped = true; }

    //! @brief    Starts the time of day clock.
    inline void cont() { stopped = false; }
		
	/*! @brief    Returns the hours digits of the time of day clock.
	 *  @note     The TOD clock freezes when a read or write access occurrs.
     */
	uint8_t getTodHours() { return frozen ? latch.time.hours & 0x9F : tod.time.hours & 0x9F; }
		
	/*! @brief    Sets the hours digits of the time of day clock.
     *  @note     The TOD clock freezes when a read or write access occurrs.
     */
	void setTodHours(uint8_t value) { tod.time.hours = value & 0x9F; }
	
	//! @brief    Returns the minutes digits of the time of day clock.
	inline uint8_t getTodMinutes() { return frozen ? latch.time.minutes & 0x7F : tod.time.minutes & 0x7F; }
	
	//! @brief    Sets the minutes digits of the time of day clock.
	inline void setTodMinutes(uint8_t value) { tod.time.minutes = value & 0x7F; }
	
	//! @brief    Returns the seconds digits of the time of day clock.
	inline uint8_t getTodSeconds() { return frozen ? latch.time.seconds & 0x7F : tod.time.seconds & 0x7F; }
	
	//! @brief    Sets the seconds digits of the time of day clock.
	inline void setTodSeconds(uint8_t value) { tod.time.seconds = value & 0x7F; }
	
	/*! @brief    Returns the tenth-of-a-second digits of the time of day clock.
	 *  @note     The TOD clock unfreezes on a read or write access.
     */
	uint8_t getTodTenth() { return frozen ? latch.time.tenth & 0x0F : tod.time.tenth & 0x0F; }
	
	/*! @brief    Sets the tenth-of-a-second digits of the time of day clock.
	 *  @note     The TOD clock unfreezes on a read or write access.
     */
	void setTodTenth(uint8_t value) { tod.time.tenth = value & 0x0F; }
	
	//! @brief    Returns the hours digits of the alarm time.
	inline uint8_t getAlarmHours() { return alarm.time.hours & 0x9F; }
	
	//! @brief    Sets the hours digits of the alarm time.
	inline void setAlarmHours(uint8_t value) { alarm.time.hours = value & 0x9F; }
	
	//! @brief    Returns the minutes digits of the alarm time.
	inline uint8_t getAlarmMinutes() { return alarm.time.minutes & 0x7F; }
	
	//! @brief    Sets the minutes digits of the alarm time.
	inline void setAlarmMinutes(uint8_t value) { alarm.time.minutes = value & 0x7F; }
	
	//! @brief    Returns the seconds digits of the alarm time.
	inline uint8_t getAlarmSeconds() { return alarm.time.seconds & 0x7F; }
	
	//! @brief    Sets the seconds digits of the alarm time.
	inline void setAlarmSeconds(uint8_t value) { alarm.time.seconds = value & 0x7F; }
	
	//! @brief    Returns the tenth-of-a-second digits of the alarm time.
	inline uint8_t getAlarmTenth() { return alarm.time.tenth & 0x0F; }
	
	//! @brief    Sets the tenth-of-a-second digits of the time of day clock.
	inline void setAlarmTenth(uint8_t value) { alarm.time.tenth = value & 0x0F; }
	
	//! @brief    Returns true, iff the TOD clock is currently frozen.
	inline bool isFrozen() { return frozen; }
	
	/*! @brief    Increments the TOD clock by one tenth of a second.
	 *  @details  The function increments the TOD clock. It returns true iff the currently set
     *            alarm time is reached. The function is supposed to be invoked whenever a 
     *            frame is finished (during VBlank).
     */
	bool increment();
	
};

#endif


