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

public:
    
	typedef	union {
		struct { 
			uint8_t tenth;
			uint8_t seconds;
			uint8_t minutes;
			uint8_t hours;
            uint32_t oldValue;
		} time;
		uint32_t value;
	} TimeOfDay;
	
	TimeOfDay tod;
	TimeOfDay alarm;
	TimeOfDay latch;
	
	/*! @brief    If set to true, the TOD registers are frozen
	 *  @details  The CIA chip freezes the registers when the hours-part is read and reactivates
     *            them, when the 1/10th part is read. Although the values stay constant, the
     *            internal clock continues to run. Purpose: If you start reading with the
     *            hours-part, the clock won't change until you have read the whole time.
     */
	bool frozen;
	
	/*! @brief    If set to true, the TOD clock is stopped
	 *  @details  The CIA chip stops the TOD clock when the hours-part is written and restarts
     *            it, when the 1/10th part is written. Purpose: The clock will only start running
     *            when the time is completely set.
     */
	bool stopped;
	
    //! @brief    Indicates if TOD is driven by a 50 Hz or 60 Hz signal
    /*! @details  Valid values are 5 (50 Hz mode) and 6 (60 Hz mode)
     */
    uint8_t hz;
    
    //! @brief    Frequency counter
    /*! @details  This counter is driven by the A/C power frequency and determines when the
     *            the TOD should increment. This variable is incremented in function 'increment'
     *            which is called after each frame. As a result, frequencyCounter is a 50 Hz
     *            signal in PAL mode and a 60 Hz signal in NTSC mode.
     */
    uint64_t frequencyCounter;
    
public:
	//! @brief    Constructor
	TOD();
	
	//! @brief    Destructor
	~TOD();
	
	//! @brief    Restores the initial state.
	void reset();
	    
	//! @brief    Prints debug information.
	void dumpState();	
	
    //! @brief    Freezes the time of day clock.
    void freeze() { if (!frozen) { latch.value = tod.value; frozen = true; } }

    //! @brief    Defreezes the time of day clock.
    void defreeze() { frozen = false; }

    //! @brief    Stops the time of day clock.
    void stop() { frequencyCounter = 0; stopped = true; }

    //! @brief    Starts the time of day clock.
    void cont() { stopped = false; }
		
	/*! @brief    Returns the hours digits of the time of day clock.
	 *  @note     The TOD clock freezes when a read or write access occurrs.
     */
	uint8_t getTodHours() {
        freeze(); return frozen ? latch.time.hours & 0x9F : tod.time.hours & 0x9F; }
		
	/*! @brief    Sets the hours digits of the time of day clock.
     *  @note     The TOD clock freezes when a read or write access occurrs.
     */
    void setTodHours(uint8_t value) {
        tod.time.oldValue = tod.value; tod.time.hours = value & 0x9F; stop(); }
	
	//! @brief    Returns the minutes digits of the time of day clock.
    uint8_t getTodMinutes() { return frozen ? latch.time.minutes & 0x7F : tod.time.minutes & 0x7F; }
	
	//! @brief    Sets the minutes digits of the time of day clock.
    void setTodMinutes(uint8_t value) {
        tod.time.oldValue = tod.value; tod.time.minutes = value & 0x7F; }
	
	//! @brief    Returns the seconds digits of the time of day clock.
    uint8_t getTodSeconds() { return frozen ? latch.time.seconds & 0x7F : tod.time.seconds & 0x7F; }
	
	//! @brief    Sets the seconds digits of the time of day clock.
    void setTodSeconds(uint8_t value) {
        tod.time.oldValue = tod.value; tod.time.seconds = value & 0x7F; }
	
	/*! @brief    Returns the tenth-of-a-second digits of the time of day clock.
	 *  @note     The TOD clock unfreezes on a read or write access.
     */
	uint8_t getTodTenth() {
        uint8_t result = frozen ? latch.time.tenth & 0x0F : tod.time.tenth & 0x0F;
        defreeze();
        return result;
    }
	
	/*! @brief    Sets the tenth-of-a-second digits of the time of day clock.
	 *  @note     The TOD clock unfreezes on a read or write access.
     */
	void setTodTenth(uint8_t value) {
        tod.time.oldValue = tod.value; tod.time.tenth = value & 0x0F; cont(); }
	
	//! @brief    Returns the hours digits of the alarm time.
    uint8_t getAlarmHours() { return alarm.time.hours & 0x9F; }
	
	//! @brief    Sets the hours digits of the alarm time.
    void setAlarmHours(uint8_t value) {
        alarm.time.oldValue = alarm.value; alarm.time.hours = value & 0x9F; }
	
	//! @brief    Returns the minutes digits of the alarm time.
    uint8_t getAlarmMinutes() { return alarm.time.minutes & 0x7F; }
	
	//! @brief    Sets the minutes digits of the alarm time.
    void setAlarmMinutes(uint8_t value) {
        alarm.time.oldValue = alarm.value; alarm.time.minutes = value & 0x7F; }
	
	//! @brief    Returns the seconds digits of the alarm time.
    uint8_t getAlarmSeconds() { return alarm.time.seconds & 0x7F; }
	
	//! @brief    Sets the seconds digits of the alarm time.
    void setAlarmSeconds(uint8_t value) {
        alarm.time.oldValue = alarm.value; alarm.time.seconds = value & 0x7F; }
	
	//! @brief    Returns the tenth-of-a-second digits of the alarm time.
    uint8_t getAlarmTenth() { return alarm.time.tenth & 0x0F; }
	
	//! @brief    Sets the tenth-of-a-second digits of the time of day clock.
    void setAlarmTenth(uint8_t value) {
        alarm.time.oldValue = alarm.value; alarm.time.tenth = value & 0x0F; }
	
	//! @brief    Returns true, iff the TOD clock is currently frozen.
    bool isFrozen() { return frozen; }
	
	/*! @brief    Increments the TOD clock by one tenth of a second.
	 *  @details  The function increments the TOD clock and is called after each frame.
     *  @return   true if TOD value changed
     */
	bool increment();
	
    //! @brief    Returns true if current time matches alarm time
    bool alarming();
};

#endif


