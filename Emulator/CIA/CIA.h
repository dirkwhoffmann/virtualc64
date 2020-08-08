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

#include "C64Component.h"

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

// Virtual complex interface adapter (CIA)
class CIA : public C64Component {
    
    // Current configuration
    CIAConfig config;
    
    // Result of the latest inspection
    CIAInfo info;
    
    
    //
    // Sub components
    //
    
    TOD tod = TOD(c64, *this);
    
    
    //
    // Internal state
    //
        
protected:

    // Total number of skipped cycles (used by the debugger, only)
    // Cycle idleCycles;
    
    // Timer A counter
    u16 counterA;
    
    // Timer B counter
    u16 counterB;
        
    // Timer A latch
    u16 latchA;
    
    // Timer B latch
    u16 latchB;
	    
    
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
	u8 CRA;

    //! @brief    Control register B
    u8 CRB;
    
    //! @brief    Interrupt control register
	u8 icr;

    //! @brief    ICR bits that need to deleted when CIAAckIcr1 hits
    u8 icrAck;

    //! @brief    Interrupt mask register
	u8 imr;

protected:
    
    //! @brief    Bit mask for PB outputs: 0 = port register, 1 = timer
    u8 PB67TimerMode;
    
    //! @brief    PB outputs bits 6 and 7 in timer mode
	u8 PB67TimerOut;
    
    //! @brief    PB outputs bits 6 and 7 in toggle mode
	u8 PB67Toggle;
		
    
    //
    // Port registers
    //
    
protected:
    
    //! @brief    Peripheral data register A
    u8 PRA;
    
    //! @brief    Peripheral data register B
    u8 PRB;
    
    //! @brief    Data directon register A (0 = input, 1 = output)
    u8 DDRA;
    
    //! @brief    Data directon register B (0 = input, 1 = output)
    u8 DDRB;
    
    //! @brief    Peripheral port A (pins PA0 to PA7)
    u8 PA;
    
    //! @brief    Peripheral port A (pins PB0 to PB7)
    u8 PB;
	
    
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
    u8 SDR;
    
    //! @brief   Clock signal for driving the serial register
    bool serClk;
    
    //! @brief   Shift register counter
    /*! @details The counter is set to 8 when the shift register is loaded and decremented
     *           when a bit is shifted out.
     */
    u8 serCounter;
    
    //
	// Chip interface (port pins)
    //
        
    //! @brief    Serial clock or input timer clock or timer gate
	bool CNT;
	bool INT;

    
    //
    // Speeding up emulation (sleep logic)
    //
    
    /* Idle counter. When the CIA's state does not change during execution,
     * this variable is increased by one. If it exceeds a certain threshhold,
     * the chip is put into idle state via sleep().
     */
    u8 tiredness;

    // Total number of skipped cycles (used by the debugger, only)
    Cycle idleCycles;
        
public:
    
    // Indicates if the CIA is currently idle
    bool sleeping;
    
    /* The last executed cycle before the chip went idle
     * The variable is set in sleep()
     */
    Cycle sleepCycle;
    
    /* The first cycle to be executed after the chip went idle
     * The variable is set in sleep()
     */
    Cycle wakeUpCycle;
    
    
    //
    // Constructing and serializing
    //
    
public:
    
	CIA(C64 &ref);
    
    
    //
    // Configuring
    //

    CIAConfig getConfig() { return config; }

    CIARevision getRevision() { return config.revision; }
    void setRevision(CIARevision revision);
              
    bool getTimerBBug() { return config.timerBBug; }
    void setTimerBBug(bool value) { config.timerBBug = value; }
    
    
    //
    // Methods from HardwareComponent
    //
    
protected:
    
	void _reset() override;
    void _inspect() override;
    void _dump() override;
    // size_t _size() override { COMPUTE_SNAPSHOT_SIZE }
    // size_t _load(u8 *buffer) override { LOAD_SNAPSHOT_ITEMS }
    // size_t _save(u8 *buffer) override { SAVE_SNAPSHOT_ITEMS }

    
public:
        
    //
    // Configuring
    //
    
    // Returns the result of the most recent call to inspect()
    CIAInfo getInfo() { return HardwareComponent::getInfo(info); }
    
    
    //
    // Accessing properties
    //
       
    //! @brief    Getter for peripheral port A
    u8 getPA() { return PA; }
    u8 getDDRA() { return DDRA; }

    //! @brief    Getter for peripheral port B
    u8 getPB() { return PB; }
    u8 getDDRB() { return DDRB; }
    
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
    virtual u8 portAinternal() = 0;
    
    //! @brief   Values driving port A from outside the chip
    virtual u8 portAexternal() = 0;
    
public:
    
    //! @brief   Computes the values which we currently see at port A
    virtual void updatePA() = 0;
    
private:
    
    //! @brief   Values driving port B from inside the chip
    virtual u8 portBinternal() = 0;
    
    //! @brief   Values driving port B from outside the chip
    virtual u8 portBexternal() = 0;
    
    //! @brief   Computes the values which we currently see at port B
    virtual void updatePB() = 0;

protected:
    
    //! @brief   Action method for poking the PA register
    virtual void pokePA(u8 value) { PRA = value; updatePA(); }

    //! @brief   Action method for poking the DDRA register
    virtual void pokeDDRA(u8 value) { DDRA = value; updatePA(); }

    
    //
    //! @functiongroup Accessing the I/O address space
    //
    
public:

    //! @brief    Peeks a value from a CIA register.
    u8 peek(u16 addr);
    
    //! @brief    Peeks a value from a CIA register without causing side effects.
    u8 spypeek(u16 addr);
    
    //! @brief    Pokes a value into a CIA register.
    void poke(u16 addr, u8 value);
    
    
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
    // Speeding up emulation (sleep logic)
    //
    
private:
    
    // Puts the CIA into idle state
    void sleep();
    
public:
        
    // Emulates all previously skipped cycles
    void wakeUp();
    void wakeUp(Cycle targetCycle);
    
    // Returns true if the CIA is in idle state
    bool isSleeping() { return sleeping; }
    
    // Returns true if the CIA is awake
    bool isAwake() { return !sleeping; }
    
    // The CIA is idle since this number of cycles
    Cycle idle();
    
    // Total number of cycles the CIA was idle
    Cycle idleTotal() { return idleCycles; }
};


/* The first virtual complex interface adapter (CIA 1).
 * The CIA 1 chips differs from the CIA 2 chip in several smaller aspects.
 * For example, the CIA 1 interrupts the CPU via the IRQ line (maskable
 * interrupts). Furthermore, the keyboard is connected to the the C64 via the
 * CIA 1 chip.
 */
class CIA1 : public CIA {
	
public:

    CIA1(C64 &ref);

private:
        
    void pullDownInterruptLine() override;
    void releaseInterruptLine() override;
    
    u8 portAinternal() override;
    u8 portAexternal() override;
    void updatePA() override;
    u8 portBinternal() override;
    u8 portBexternal() override;
    void updatePB() override;
};
	
/* The second virtual complex interface adapter (CIA 2).
 * The CIA 2 chips differs from the CIA 1 chip in several smaller aspects.
 * For example, the CIA 2 interrupts the CPU via the NMI line (non maskable
 * interrupts). Furthermore, the CIA 2 controlls the memory bank seen by the
 * video controller.
 */
class CIA2 : public CIA {

public:

    CIA2(C64 &ref);
    
private:
    
    void _reset() override;
    
    void pullDownInterruptLine() override;
    void releaseInterruptLine() override;
    
    u8 portAinternal() override;
    u8 portAexternal() override;
    
public:
    
    void updatePA() override;
    
private:
    
    u8 portBinternal() override;
    u8 portBexternal() override;
    void updatePB() override;
    void pokePA(u8 value) override;
    void pokeDDRA(u8 value) override;
};

#endif
