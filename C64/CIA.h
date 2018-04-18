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
#define CountA0     0x0000000000000001
#define CountA1     0x0000000000000002
#define CountA2     0x0000000000000004
#define CountA3     0x0000000000000008 // Decrements timer A
#define CountB0     0x0000000000000010
#define CountB1     0x0000000000000020
#define CountB2     0x0000000000000040
#define CountB3     0x0000000000000080 // Decrements timer B
#define LoadA0      0x0000000000000100
#define LoadA1      0x0000000000000200
#define LoadA2      0x0000000000000400 // Loads timer A
#define LoadB0      0x0000000000000800
#define LoadB1      0x0000000000001000
#define LoadB2      0x0000000000002000 // Loads timer B
#define PB6Low0     0x0000000000004000
#define PB6Low1     0x0000000000008000
#define PB7Low0     0x0000000000010000
#define PB7Low1     0x0000000000020000
#define Interrupt0  0x0000000000040000
#define Interrupt1  0x0000000000080000 // Triggers an interrupt
#define OneShotA0   0x0000000000100000
#define OneShotB0   0x0000000000200000
#define ReadIcr0    0x0000000000400000 // ICR register was read
#define ReadIcr1    0x0000000000800000
#define ClearIcr0   0x0000000001000000
#define ClearIcr1   0x0000000002000000
#define ClearIcr2   0x0000000004000000 // Clears bit 8 in ICR register
#define SetIcr0     0x0000000008000000
#define SetIcr1     0x0000000010000000 // Sets bit 8 in ICR register
#define TODInt0     0x0000000020000000 // Triggers an interrupt with TOD as source
#define SerInt0     0x0000000040000000
#define SerInt1     0x0000000080000000
#define SerInt2     0x0000000100000000 // Triggers an interrupt with serial register as source
#define SerLoad0    0x0000000200000000
#define SerLoad1    0x0000000400000000 // Loads the serial shift register
#define SerClk0     0x0000000800000000 // Clock signal driving the serial register
#define SerClk1     0x0000001000000000
#define SerClk2     0x0000002000000000
#define SerClk3     0x0000004000000000

#define DelayMask ~(0x0000008000000000 | CountA0 | CountB0 | LoadA0 | LoadB0 | PB6Low0 | PB7Low0 | Interrupt0 | OneShotA0 | OneShotB0 | ReadIcr0 | ClearIcr0 | SetIcr0 | TODInt0 | SerInt0 | SerLoad0 | SerClk0)


/*! @brief    Virtual complex interface adapter (CIA)
 *  @details  The original C64 contains two CIA chips (CIA 1 and CIA 2). Each chip features
 *            two programmable timers and a real-time clock. Furthermore, the CIA chips manage
 *            the communication with connected peripheral devices such as joysticks, printers
 *            or the keyboard. The CIA class implements the common functionality of both CIAs.
 */
class CIA : public VirtualComponent {
    
    friend C64;
    friend C64Memory;
    
    // ---------------------------------------------------------------------------------------
    //                                          Properties
    // ---------------------------------------------------------------------------------------

public:
    
	//! @brief    Start address of the CIA I/O space (CIA 1 and CIA 2)
	static const uint16_t CIA_START_ADDR = 0xDC00;

    //! @brief    End address of the CIA I/O space (CIA 1 and CIA 2)
	static const uint16_t CIA_END_ADDR = 0xDDFF;
        
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
    // Ports
    //
    
    //! @brief    Bbuffered output value of port A
    uint8_t PALatch;

    //! @brief    Bbuffered output value of port B
    uint8_t PBLatch;
    
    //! @brief    Data directon register for port A (0 = input, 1 = output)
	uint8_t DDRA;

    //! @brief    Data directon register for port B (0 = input, 1 = output)
    uint8_t DDRB;
	
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
    
	uint8_t PA;
	uint8_t PB;
    
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
    
    // ------------------------------------------------------------------------------------------
    //                                             Methods
    // ------------------------------------------------------------------------------------------

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
	
    
    //
    //! @functiongroup Accessing the I/O address space
    //
    
private:

    /*! @brief    Returns the current value of the CIA's data port A register
     *! @details  This method is implemented differently in CIA1 and CIA2
     */
    virtual uint8_t readDataPortA() = 0;
    
    /*! @brief    Returns the current value of the CIA's data port B register
     * @details  This method is implemented differently in CIA1 and CIA2
     */
    virtual uint8_t readDataPortB() = 0;
    
    //! @brief    Peek function for the CIA's I/O memory
    uint8_t peek(uint16_t addr);
    
    //! @brief    Same as peek, but without side affects.
    uint8_t read(uint16_t addr);
    
    //! @brief    Poke function for the CIA's I/O memory
    void poke(uint16_t addr, uint8_t value);
    
    //! @brief    Poke function for the CIA's data port A register
    virtual void pokeDataPortA(uint8_t value) = 0;

    //! @brief    Poke function for the CIA's data port B register
    virtual void pokeDataPortB(uint8_t value) = 0;

    //! @brief    Poke function for the CIA's data port A direction register
    virtual void pokeDataPortDirectionA(uint8_t value) = 0;

    //! @brief    Poke function for the CIA's data port B direction register
    virtual void pokeDataPortDirectionB(uint8_t value) = 0;
    
    //
    //! @functiongroup Running the device
    //
    
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
 *  @details  The CIA 1 chips differs from the CIA 2 chip in several smaller aspects. For example, 
 *            the CIA 1 interrupts the CPU via the IRQ line (maskable interrupts). Furthermore, 
 *            the keyboard is connected to the the C64 via the CIA 1 chip.
 */
class CIA1 : public CIA {
	
public:

    CIA1();
    ~CIA1();
    void dumpState();
    
private:
    
    void pullDownInterruptLine();
    void releaseInterruptLine();
    uint8_t readDataPortA();
    uint8_t readDataPortB();
    void pokeDataPortA(uint8_t value);
    void pokeDataPortB(uint8_t value);
    void pokeDataPortDirectionA(uint8_t value);
    void pokeDataPortDirectionB(uint8_t value);
    uint64_t wakeUpCycle();
    void setWakeUpCycle(uint64_t cycle);
    uint64_t idleCounter();
    void resetIdleCounter();
    
    
};
	
/*! @brief    The second virtual complex interface adapter (CIA 2)
 *  @details  The CIA 2 chips differs from the CIA 1 chip in several smaller aspects. For example,
 *            the CIA 2 interrupts the CPU via the NMI line (non maskable interrupts). Furthermore, 
 *            the CIA 2 controlls the memory bank seen by the video controller. Therefore, CIA 2 
 *            needs to know about the VIC chip, in contrast to CIA 1.
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
    uint8_t readDataPortA();
    uint8_t readDataPortB();
    void pokeDataPortA(uint8_t value);
    void pokeDataPortB(uint8_t value);
    void pokeDataPortDirectionA(uint8_t value);
    void pokeDataPortDirectionB(uint8_t value);
    uint64_t wakeUpCycle();
    void setWakeUpCycle(uint64_t cycle);
    uint64_t idleCounter();
    void resetIdleCounter();
};

#endif
