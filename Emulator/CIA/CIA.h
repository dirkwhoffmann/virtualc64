/// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _CIA_H
#define _CIA_H

#include "TOD.h"
#include "CIATypes.h"

// Forward declarations
class VIC;
class IEC;
class Keyboard;
class Joystick;

// Adapted from PC64WIN
#define CIACountA0     (1ULL << 0) // Decrements timer A
#define CIACountA1     (1ULL << 1)
#define CIACountA2     (1ULL << 2)
#define CIACountA3     (1ULL << 3)
#define CIACountB0     (1ULL << 4) // Decrements timer B
#define CIACountB1     (1ULL << 5)
#define CIACountB2     (1ULL << 6)
#define CIACountB3     (1ULL << 7)
#define CIALoadA0      (1ULL << 8) // Loads timer A
#define CIALoadA1      (1ULL << 9)
#define CIALoadA2      (1ULL << 10)
#define CIALoadB0      (1ULL << 11) // Loads timer B
#define CIALoadB1      (1ULL << 12)
#define CIALoadB2      (1ULL << 13)
#define CIAPB6Low0     (1ULL << 14) // Sets pin PB6 low
#define CIAPB6Low1     (1ULL << 15)
#define CIAPB7Low0     (1ULL << 16) // Sets pin PB7 low
#define CIAPB7Low1     (1ULL << 17)
#define CIASetInt0     (1ULL << 18) // Triggers an interrupt
#define CIASetInt1     (1ULL << 19)
#define CIAClearInt0   (1ULL << 20) // Releases the interrupt line
#define CIAOneShotA0   (1ULL << 21)
#define CIAOneShotB0   (1ULL << 22)
#define CIAReadIcr0    (1ULL << 23) // Indicates that ICR was read recently
#define CIAReadIcr1    (1ULL << 24)
#define CIAClearIcr0   (1ULL << 25) // Clears bit 8 in ICR register
#define CIAClearIcr1   (1ULL << 26)
#define CIAClearIcr2   (1ULL << 27)
#define CIAAckIcr0     (1ULL << 28) // Clears bit 0 - 7 in ICR register
#define CIAAckIcr1     (1ULL << 29)
#define CIASetIcr0     (1ULL << 30) // Sets bit 8 in ICR register
#define CIASetIcr1     (1ULL << 31)
#define CIATODInt0     (1ULL << 32) // Triggers an interrupt with TOD as source
#define CIASerInt0     (1ULL << 33) // Triggers an interrupt with serial register as source
#define CIASerInt1     (1ULL << 34)
#define CIASerInt2     (1ULL << 35)
#define CIASerLoad0    (1ULL << 36) // Loads the serial shift register
#define CIASerLoad1    (1ULL << 37)
#define CIASerClk0     (1ULL << 38) // Clock signal driving the serial register
#define CIASerClk1     (1ULL << 39)
#define CIASerClk2     (1ULL << 40)
#define CIASerClk3     (1ULL << 41)

#define DelayMask ~((1ULL << 42) | CIACountA0 | CIACountB0 | CIALoadA0 | CIALoadB0 | CIAPB6Low0 | CIAPB7Low0 | CIASetInt0 | CIAClearInt0 | CIAOneShotA0 | CIAOneShotB0 | CIAReadIcr0 | CIAClearIcr0 | CIAAckIcr0 | CIASetIcr0 | CIATODInt0 | CIASerInt0 | CIASerLoad0 | CIASerClk0)


/*! @brief    Virtual complex interface adapter (CIA)
 *  @details  The original C64 contains two CIA chips (CIA 1 and CIA 2). Each
 *            chip features two programmable timers and a real-time clock.
 *            Furthermore, the CIA chips manage the communication with connected
 *            peripheral devices such as joysticks, printers or the keyboard.
 *            The CIA class implements the common functionality of both CIAs.
 */
class CIA : public HardwareComponent {
    
    //! @brief    Selected chip model
    CIAModel model;
    
    //! @brief    Indicates if timer B bug should be emulated
    bool emulateTimerBBug;
    
protected:

	//! @brief    Timer A counter
	uint16_t counterA;
	
    //! @brief    Timer B counter
    uint16_t counterB;
    
private:
    
	//! @brief    Timer A latch
	uint16_t latchA;
	
	//! @brief    Timer B latch
	uint16_t latchB;
	
	//! @brief    Time of day clock
	TOD tod = TOD(this);
    
	
	// 
	// Adapted from PC64Win by Wolfgang Lorenz
	//
		
    //
	// Control
    //
    
    //! @brief    Performs delay by shifting left at each clock
	u64 delay;
    
    //! @brief    New bits to feed into dwDelay
	u64 feed;
    
    //! @brief    Control register A
	uint8_t CRA;

    //! @brief    Control register B
    uint8_t CRB;
    
    //! @brief    Interrupt control register
	uint8_t icr;

    //! @brief    ICR bits that need to deleted when CIAAckIcr1 hits
    uint8_t icrAck;

    //! @brief    Interrupt mask register
	uint8_t imr;

protected:
    
    //! @brief    Bit mask for PB outputs: 0 = port register, 1 = timer
    uint8_t PB67TimerMode;
    
    //! @brief    PB outputs bits 6 and 7 in timer mode
	uint8_t PB67TimerOut;
    
    //! @brief    PB outputs bits 6 and 7 in toggle mode
	uint8_t PB67Toggle;
		
    
    //
    // Port registers
    //
    
protected:
    
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
    
private:
    
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
    // Speeding up emulation (CIA sleep logic)
    //
    
    //! @brief    Idle counter
    /*! @details  When the VIA state does not change during execution, this
     *            variable is increased by one. If it exceeds a certain
     *            threshhold, the chip is put into idle state via sleep()
     */
    uint8_t tiredness;

public:
    
    //! @brief    Wakeup cycle
    u64 wakeUpCycle;
    
    //! @brief    Number of skipped executions
    u64 idleCounter;
    
public:	
	
	//! @brief    Constructor
	CIA();
	
	//! @brief    Destructor
	~CIA();
	
	//! @brief    Bring the CIA back to its initial state
	void reset();
    	
	//! @brief    Dump internal state
	void dump();	

	//! @brief    Dump trace line
	void dumpTrace();	

    
    //
    //! @functiongroup Accessing device properties
    //
    
    //! @brief    Returns the currently plugged in chip model.
    CIAModel getModel() { return model; }
    
    //! @brief    Sets the chip model.
    void setModel(CIAModel m);
    
    //! @brief    Determines if the emulated model is affected by the timer B bug.
    bool hasTimerBBug() { return model == MOS_6526; }
    
    //! @brief    Returns true if the timer B bug should be emulated.
    bool getEmulateTimerBBug() { return emulateTimerBBug; }
    
    //! @brief    Enables or disables emulation of the timer B bug.
    void setEmulateTimerBBug(bool value) { emulateTimerBBug = value; }
    
    //! @brief    Getter for peripheral port A
    uint8_t getPA() { return PA; }
    uint8_t getDDRA() { return DDRA; }

    //! @brief    Getter for peripheral port B
    uint8_t getPB() { return PB; }
    uint8_t getDDRB() { return DDRB; }

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
    void reloadTimerA() { counterA = latchA; delay &= ~CIACountA2; }
	
	/*! @brief    Loads latched value into timer.
	 *  @details  As a side effect, CountB2 is cleared. This causes the timer to wait for
     *            one cycle before it continues to count.
     */
    void reloadTimerB() { counterB = latchB; delay &= ~CIACountB2; }

    /*! @brief    Triggers a timer interrupt
     *  @details  Invoked inside executeOneCycle() if IRQ conditions are met.
     */
    void triggerTimerIrq();

    /*! @brief    Triggers a TOD interrupt
     *  @details  Invoked inside executeOneCycle() if IRQ conditions are met.
     */
    void triggerTodIrq();

    /*! @brief    Triggers a serial interrupt
     *  @details  Invoked inside executeOneCycle() if IRQ conditions are met.
     */
    void triggerSerialIrq();

private:
    
    //
    // Port registers
    //
    
    //! @brief   Values driving port A from inside the chip
    virtual uint8_t portAinternal() = 0;
    
    //! @brief   Values driving port A from outside the chip
    virtual uint8_t portAexternal() = 0;
    
public:
    
    //! @brief   Computes the values which we currently see at port A
    virtual void updatePA() = 0;
    
private:
    
    //! @brief   Values driving port B from inside the chip
    virtual uint8_t portBinternal() = 0;
    
    //! @brief   Values driving port B from outside the chip
    virtual uint8_t portBexternal() = 0;
    
    //! @brief   Computes the values which we currently see at port B
    virtual void updatePB() = 0;

protected:
    
    //! @brief   Action method for poking the PA register
    virtual void pokePA(uint8_t value) { PRA = value; updatePA(); }

    //! @brief   Action method for poking the DDRA register
    virtual void pokeDDRA(uint8_t value) { DDRA = value; updatePA(); }

    
    //
    //! @functiongroup Accessing the I/O address space
    //
    
public:

    //! @brief    Peeks a value from a CIA register.
    uint8_t peek(uint16_t addr);
    
    //! @brief    Peeks a value from a CIA register without causing side effects.
    uint8_t spypeek(uint16_t addr);
    
    //! @brief    Pokes a value into a CIA register.
    void poke(uint16_t addr, uint8_t value);
    
    
    //
    //! @functiongroup Running the device
    //
    
public:
    
	//! @brief    Executes the CIA for one cycle
	void executeOneCycle();
    
	//! @brief    Increments the TOD clock by one tenth of a second
	void incrementTOD();

    
    //
    //! @functiongroup Handling interrupt requests
    //
    
    //! @brief    Handles an interrupt request from TOD
    void todInterrupt(); 

    
    //
    //! @functiongroup Speeding up emulation
    //
    
private:
    
    //! @brief    Puts the CIA into idle state.
    void sleep();
    
    //! @brief    Emulates all previously skipped cycles.
    void wakeUp();
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
    void dump();
    
private:
    
    void pullDownInterruptLine();
    void releaseInterruptLine();
    
    uint8_t portAinternal();
    uint8_t portAexternal();
    void updatePA();
    uint8_t portBinternal();
    uint8_t portBexternal();
    void updatePB();
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
    void dump();
    
private:

    void pullDownInterruptLine();
    void releaseInterruptLine();
    
    uint8_t portAinternal();
    uint8_t portAexternal();
    
public:
    
    void updatePA();
    
private:
    
    uint8_t portBinternal();
    uint8_t portBexternal();
    void updatePB();
    void pokePA(uint8_t value);
    void pokeDDRA(uint8_t value);
};

#endif
