/*!
 * @header      CIA.h
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   2006 - 2018 Dirk W. Hoffmann
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

#ifndef _CIA_H
#define _CIA_H

#include "TOD.h"
#include "CIA_types.h"

// Forward declarations
class VIC;
class IEC;
class Keyboard;
class Joystick;

// Adapted from PC64WIN
#define CountA0     (1ULL << 0) // Decrements timer A
#define CountA1     (1ULL << 1)
#define CountA2     (1ULL << 2)
#define CountA3     (1ULL << 3)
#define CountB0     (1ULL << 4) // Decrements timer B
#define CountB1     (1ULL << 5)
#define CountB2     (1ULL << 6)
#define CountB3     (1ULL << 7)
#define LoadA0      (1ULL << 8) // Loads timer A
#define LoadA1      (1ULL << 9)
#define LoadA2      (1ULL << 10)
#define LoadB0      (1ULL << 11) // Loads timer B
#define LoadB1      (1ULL << 12)
#define LoadB2      (1ULL << 13)
#define PB6Low0     (1ULL << 14) // Sets pin PB6 low
#define PB6Low1     (1ULL << 15)
#define PB7Low0     (1ULL << 16) // Sets pin PB7 low
#define PB7Low1     (1ULL << 17)
#define SetInt0     (1ULL << 18) // Triggers an interrupt
#define SetInt1     (1ULL << 19)
#define ClearInt0   (1ULL << 20) // Releases the interrupt line
#define OneShotA0   (1ULL << 21)
#define OneShotB0   (1ULL << 22)
#define ReadIcr0    (1ULL << 23) // Indicates that ICR was read recently
#define ReadIcr1    (1ULL << 24)
#define ClearIcr0   (1ULL << 25) // Clears bit 8 in ICR register
#define ClearIcr1   (1ULL << 26)
#define ClearIcr2   (1ULL << 27)
#define SetIcr0     (1ULL << 28) // Sets bit 8 in ICR register
#define SetIcr1     (1ULL << 29)
#define TODInt0     (1ULL << 30) // Triggers an interrupt with TOD as source
#define SerInt0     (1ULL << 31) // Triggers an interrupt with serial register as source
#define SerInt1     (1ULL << 32)
#define SerInt2     (1ULL << 33)
#define SerLoad0    (1ULL << 34) // Loads the serial shift register
#define SerLoad1    (1ULL << 35)
#define SerClk0     (1ULL << 36) // Clock signal driving the serial register
#define SerClk1     (1ULL << 37)
#define SerClk2     (1ULL << 38)
#define SerClk3     (1ULL << 39)

#define DelayMask ~((1ULL << 40) | CountA0 | CountB0 | LoadA0 | LoadB0 | PB6Low0 | PB7Low0 | SetInt0 | ClearInt0 | OneShotA0 | OneShotB0 | ReadIcr0 | ClearIcr0 | SetIcr0 | TODInt0 | SerInt0 | SerLoad0 | SerClk0)


/*! @brief    Virtual complex interface adapter (CIA)
 *  @details  The original C64 contains two CIA chips (CIA 1 and CIA 2). Each
 *            chip features two programmable timers and a real-time clock.
 *            Furthermore, the CIA chips manage the communication with connected
 *            peripheral devices such as joysticks, printers or the keyboard.
 *            The CIA class implements the common functionality of both CIAs.
 */
class CIA : public VirtualComponent {
    
    friend C64;
    friend C64Memory;
    
public:

	//! @brief    Timer A counter
	uint16_t counterA;
	
	//! @brief    Timer A latch
	uint16_t latchA;

	//! @brief    Timer B counter
	uint16_t counterB;
	
	//! @brief    Timer B latch
	uint16_t latchB;
	
	//! @brief    Time of day clock
	TOD tod;
	
public:	
	
	// 
	// Adapted from PC64Win by Wolfgang Lorenz
	//
		
    //
	// Control
    //
    
    //! @brief    Performs delay by shifting left at each clock
	uint64_t delay;
    
    //! @brief    New bits to feed into dwDelay
	uint64_t feed;
    
    //! @brief    Control register A
	uint8_t CRA;

    //! @brief    Control register B
    uint8_t CRB;
    
    //! @brief    Interrupt control register
	uint8_t ICR;
    
    //! @brief    Interrupt mask register
	uint8_t IMR;

    //! @brief    Bit mask for PB outputs: 0 = port register, 1 = timer
    uint8_t PB67TimerMode;
    
    //! @brief    PB outputs bits 6 and 7 in timer mode
	uint8_t PB67TimerOut;
    
    //! @brief    PB outputs bits 6 and 7 in toggle mode
	uint8_t PB67Toggle;
		
    //
    // Port registers
    //
    
    //! @brief    Peripheral data register A
    uint8_t PRA;
    
    //! @brief    Peripheral data register B
    uint8_t PRB;
    
    //! @brief    Data directon register A (0 = input, 1 = output)
    uint8_t DDRA;
    
    //! @brief    Data directon register B (0 = input, 1 = output)
    uint8_t DDRB;
    
    //! @brief    Peripheral port A (pins PA0 to PA7)
    uint8_t PA;
    
    //! @brief    Peripheral port A (pins PB0 to PB7)
    uint8_t PB;
	
    
    //
    // Shift register logic
    //
    
    //! @brief    Serial data register
    /*! @details  http://unusedino.de/ec64/technical/misc/cia6526/serial.html
     *            "The serial port is a buffered, 8-bit synchronous shift register system.
     *             A control bit selects input or output mode. In input mode, data on the SP pin
     *             is shifted into the shift register on the rising edge of the signal applied
     *             to the CNT pin. After 8 CNT pulses, the data in the shift register is dumped
     *             into the Serial Data Register and an interrupt is generated. In the output
     *             mode, TIMER A is used for the baud rate generator. Data is shifted out on the
     *             SP pin at 1/2 the underflow rate of TIMER A. [...] Transmission will start
     *             following a write to the Serial Data Register (provided TIMER A is running
     *             and in continuous mode). The clock signal derived from TIMER A appears as an
     *             output on the CNT pin. The data in the Serial Data Register will be loaded
     *             into the shift register then shift out to the SP pin when a CNT pulse occurs.
     *             Data shifted out becomes valid on the falling edge of CNT and remains valid
     *             until the next falling edge. After 8 CNT pulses, an interrupt is generated to
     *             indicate more data can be sent. If the Serial Data Register was loaded with
     *             new information prior to this interrupt, the new data will automatically be
     *             loaded into the shift register and transmission will continue. If the
     *             microprocessor stays one byte ahead of the shift register, transmission will
     *             be continuous. If no further data is to be transmitted, after the 8th CNT
     *             pulse, CNT will return high and SP will remain at the level of the last data
     *             bit transmitted. SDR data is shifted out MSB first and serial input data
     *             should also appear in this format.
     */
    uint8_t SDR;
    
    //! @brief   Clock signal for driving the serial register
    bool serClk;
    
    //! @brief   Shift register counter
    /*! @details The counter is set to 8 when the shift register is loaded and decremented
     *           when a bit is shifted out.
     */
    uint8_t serCounter;
    
    //
	// Chip interface (port pins)
    //
        
    //! @brief    Serial clock or input timer clock or timer gate
	bool CNT;
	bool INT;

    
    //
    // Sleep logic (speedup)
    //
    
    //! @brief    Idle counter
    /*! @details  When the CIA state does not change during execution, this variable is
     *            increased by one. If it exceeds a certain threshhold value, the chip
     *            is put into idle state via sleep()
     */
    uint8_t tiredness;
    
    
    //
    // Methods
    //

public:	
	
	//! @brief    Constructor
	CIA();
	
	//! @brief    Destructor
	~CIA();
	
	//! @brief    Bring the CIA back to its initial state
	void reset();
    	
	//! @brief    Dump internal state
	void dumpState();	

	//! @brief    Dump trace line
	void dumpTrace();	

    
    //
    //! @functiongroup Accessing device properties
    //
    
    //! @brief    Collects all data to be shown in the GUI's debug panel
    CIAInfo getInfo();
    
    //! @brief    Simulates a rising edge on the flag pin
    void triggerRisingEdgeOnFlagPin();

    //! @brief    Simulates a falling edge on the flag pin
    void triggerFallingEdgeOnFlagPin();
    
private:

    //
	// Interrupt control
	//
    
    /*! @brief    Requests the CPU to interrupt
     *  @details  This function is abstract and implemented differently by CIA1 and CIA2.
     *            CIA 1 activates the IRQ line and CIA 2 the NMI line.
     */
    virtual void pullDownInterruptLine() = 0;
    
    /*! @brief    Removes the interrupt requests
     *  @details  This function is abstract and implemented differently by CIA1 and CIA2.
     *            CIA 1 clears the IRQ line and CIA 2 the NMI line.
     */
    virtual void releaseInterruptLine() = 0;
    
	/*! @brief    Load latched value into timer.
	 *  @details  As a side effect, CountA2 is cleared. This causes the timer to wait
     *            for one cycle before it continues to count.
     */
    void reloadTimerA() { counterA = latchA; delay &= ~CountA2; }
	
	/*! @brief    Loads latched value into timer.
	 *  @details  As a side effect, CountB2 is cleared. This causes the timer to wait for
     *            one cycle before it continues to count.
     */
    void reloadTimerB() { counterB = latchB; delay &= ~CountB2; }
	
private:
    
    //
    // Port registers
    //
    
    //! @brief   Values driving port A from inside the chip
    virtual uint8_t portAinternal() = 0;
    
    //! @brief   Values driving port A from outside the chip
    virtual uint8_t portAexternal() = 0;
    
    //! @brief   Computes the values which we currently see at port A
    virtual void updatePA() = 0;
    
    //! @brief   Values driving port B from inside the chip
    virtual uint8_t portBinternal() = 0;
    
    //! @brief   Values driving port B from outside the chip
    virtual uint8_t portBexternal() = 0;
    
    //! @brief   Computes the values which we currently see at port B
    virtual void updatePB() = 0;

    
    //
    //! @functiongroup Accessing the I/O address space
    //
    
private:

    //! @brief    Peeks a value from a CIA register.
    uint8_t peek(uint16_t addr);

public:
    
    //! @brief    Peeks a value from a CIA register without causing side effects.
    uint8_t spypeek(uint16_t addr);
    
    //! @brief    Pokes a value into a CIA register.
    void poke(uint16_t addr, uint8_t value);
    
    
    //
    //! @functiongroup Running the device
    //
    
private:
    
	//! @brief    Executes the CIA for one cycle
	void executeOneCycle();
    
	//! @brief    Increments the TOD clock by one tenth of a second
	void incrementTOD();

    //
    //! @functiongroup Handling interrupt requests
    //

public:
    
    //! @brief    Handles an interrupt request from TOD
    void todInterrupt(); 

    
    //
    //! @functiongroup Speeding up the emulation
    //
    
private:
    
    //! @brief    Puts the CIA chip into idle state
    virtual void sleep();
    
    //! @brief    Emulate all previously skipped cycles
    virtual void wakeUp();
    
    //! @brief    Returns the wake up cycle for this CIA chip
    virtual uint64_t wakeUpCycle() = 0;

    //! @brief    Sets the wake up cycle for this CIA chip
    virtual void setWakeUpCycle(uint64_t cycle) = 0;
    
    //! @brief    Returns the number of skipped executions for this CIA chip
    virtual uint64_t idleCounter() = 0;
    
    //! @brief    Resets the skipped execution cycle counter to zero
    virtual void resetIdleCounter() = 0;
};


/*! @class    The first virtual complex interface adapter (CIA 1)
 *  @details  The CIA 1 chips differs from the CIA 2 chip in several smaller
 *            aspects. For example, the CIA 1 interrupts the CPU via the
 *            IRQ line (maskable interrupts). Furthermore, the keyboard is
 *            connected to the the C64 via the CIA 1 chip.
 */
class CIA1 : public CIA {
	
public:

    CIA1();
    ~CIA1();
    void dumpState();
    
private:
    
    void pullDownInterruptLine();
    void releaseInterruptLine();
    
    uint8_t portAinternal();
    uint8_t portAexternal();
    void updatePA();
    uint8_t portBinternal();
    uint8_t portBexternal();
    void updatePB();
    
    uint64_t wakeUpCycle();
    void setWakeUpCycle(uint64_t cycle);
    uint64_t idleCounter();
    void resetIdleCounter();
    
    
};
	
/*! @brief    The second virtual complex interface adapter (CIA 2)
 *  @details  The CIA 2 chips differs from the CIA 1 chip in several smaller
 *            aspects. For example, the CIA 2 interrupts the CPU via the
 *            NMI line (non maskable interrupts). Furthermore, the CIA 2
 *            controlls the memory bank seen by the video controller. 
 */
class CIA2 : public CIA {

public:

    CIA2();
    ~CIA2();
    void reset(); 
    void dumpState();
    
private:

    void pullDownInterruptLine();
    void releaseInterruptLine();
    
    uint8_t portAinternal();
    uint8_t portAexternal();
    void updatePA();
    uint8_t portBinternal();
    uint8_t portBexternal();
    void updatePB();
    
    uint64_t wakeUpCycle();
    void setWakeUpCycle(uint64_t cycle);
    uint64_t idleCounter();
    void resetIdleCounter();
};

#endif
