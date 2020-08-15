// -----------------------------------------------------------------------------
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

// Action flags
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
    // Internals
    //
        
protected:
    
    // Timer A counter
    u16 counterA;
    
    // Timer B counter
    u16 counterB;
        
    // Timer A latch
    u16 latchA;
    
    // Timer B latch
    u16 latchB;
	    
    		
    //
	// Control
    //
    
    // Action flags
	u64 delay;
	u64 feed;
    
    // Control registers
	u8 CRA;
    u8 CRB;
    
    // Interrupt control register
	u8 icr;

    // ICR bits that need to deleted when CIAAckIcr1 hits
    u8 icrAck;

    // Interrupt mask register
	u8 imr;

protected:
    
    // Bit mask for PB outputs (0 = port register, 1 = timer)
    u8 PB67TimerMode;
    
    // PB outputs bits 6 and 7 in timer mode
	u8 PB67TimerOut;
    
    // PB outputs bits 6 and 7 in toggle mode
	u8 PB67Toggle;
		
    
    //
    // Port registers
    //
    
protected:
    
    // Peripheral data registers
    u8 PRA;
    u8 PRB;
    
    // Data directon registers
    u8 DDRA;
    u8 DDRB;
    
    // Peripheral ports
    u8 PA;
    u8 PB;
	
    
    //
    // Shift register logic
    //
    
private:
    
    /* Serial data register
     * http://unusedino.de/ec64/technical/misc/cia6526/serial.html
     * "The serial port is a buffered, 8-bit synchronous shift register system.
     *  A control bit selects input or output mode. In input mode, data on the
     *  SP pin is shifted into the shift register on the rising edge of the
     *  signal applied to the CNT pin. After 8 CNT pulses, the data in the shift
     *  register is dumped into the Serial Data Register and an interrupt is
     *  generated. In the output mode, TIMER A is used for the baud rate
     *  generator. Data is shifted out on the SP pin at 1/2 the underflow rate
     *  of TIMER A. [...] Transmission will start following a write to the
     *  Serial Data Register (provided TIMER A is running and in continuous
     *  mode). The clock signal derived from TIMER A appears as an output on the
     *  CNT pin. The data in the Serial Data Register will be loaded into the
     *  shift register then shift out to the SP pin when a CNT pulse occurs.
     *  Data shifted out becomes valid on the falling edge of CNT and remains
     *  valid until the next falling edge. After 8 CNT pulses, an interrupt is
     *  generated to indicate more data can be sent. If the Serial Data Register
     *  was loaded with new information prior to this interrupt, the new data
     *  will automatically be loaded into the shift register and transmission
     *  will continue. If the microprocessor stays one byte ahead of the shift
     *  register, transmission will be continuous. If no further data is to be
     *  transmitted, after the 8th CNT pulse, CNT will return high and SP will
     *  remain at the level of the last data bit transmitted. SDR data is
     *  shifted out MSB first and serial input data should also appear in this
     *  format.
     */
    u8 sdr;
    
    // Clock signal for driving the serial register
    bool serClk;
    
    /* Shift register counter
     * The counter is set to 8 when the shift register is loaded and decremented
     * when a bit is shifted out.
     */
    u8 serCounter;
    
    //
	// Port pins
    //
        
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
    // Configuring and analyzing
    //

public:
    
    CIAConfig getConfig() { return config; }

    CIARevision getRevision() { return config.revision; }
    void setRevision(CIARevision revision);
              
    bool getTimerBBug() { return config.timerBBug; }
    void setTimerBBug(bool value) { config.timerBBug = value; }
    
    CIAInfo getInfo() { return HardwareComponent::getInfo(info); }
    
    
    //
    // Managing the component state
    //
    
protected:
    
	void _reset() override;
    void _inspect() override;
    void _dump() override;
    // size_t _size() override { COMPUTE_SNAPSHOT_SIZE }
    // size_t _load(u8 *buffer) override { LOAD_SNAPSHOT_ITEMS }
    // size_t _save(u8 *buffer) override { SAVE_SNAPSHOT_ITEMS }

        
    //
    // Accessing the I/O register space
    //
       
public:
    
    // Reads a value from a CIA register
    u8 peek(u16 addr);
    
    // Reads a value from a CIA register without causing side effects
    u8 spypeek(u16 addr);
    
    // Writes a value into a CIA register
    void poke(u16 addr, u8 value);
    
    //
    // Accessing the port registers
    //
    
public:
    
    // Returns the data registers (call updatePA() or updatePB() first)
    u8 getPA() { return PA; }
    u8 getPB() { return PB; }

private:
    
    // Returns the data direction register
    u8 getDDRA() { return DDRA; }
    u8 getDDRB() { return DDRB; }
    
    // Computes the value we currently see at port A
    virtual void updatePA() = 0;
    
    // Returns the value driving port A from inside the chip
    virtual u8 portAinternal() = 0;
    
    // Returns the value driving port A from outside the chip
    virtual u8 portAexternal() = 0;
    
    // Computes the value we currently see at port B
    virtual void updatePB() = 0;
    
    // Returns the value driving port B from inside the chip
    virtual u8 portBinternal() = 0;
    
    // Returns the value  driving port B from outside the chip
    virtual u8 portBexternal() = 0;
    
protected:
    
    // Action method for poking the PA register
    virtual void pokePA(u8 value) { PRA = value; updatePA(); }

    // Action method for poking the DDRA register
    virtual void pokeDDRA(u8 value) { DDRA = value; updatePA(); }

    
    //
    // Accessing the port pins
    //
    
public:
    
    // Simulates an edge on the flag pin
    void triggerRisingEdgeOnFlagPin();
    void triggerFallingEdgeOnFlagPin();
    
    
    //
    // Handling interrupts
    //
    
private:

    // Requests the CPU to interrupt
    virtual void pullDownInterruptLine() = 0;
    
    // Removes the interrupt requests
    virtual void releaseInterruptLine() = 0;
    
    // Loads a latched value into timer
    void reloadTimerA() { counterA = latchA; delay &= ~CIACountA2; }
    void reloadTimerB() { counterB = latchB; delay &= ~CIACountB2; }
    
    // Triggers an interrupt (invoked inside executeOneCycle())
    void triggerTimerIrq();
    void triggerTodIrq();
    void triggerSerialIrq();
    
public:
    
    // Handles an interrupt request from TOD
    void todInterrupt();
    
 
    //
    // Executing
    //
    
public:
    
	// Executes the CIA for one cycle
	void executeOneCycle();
    
	// Increments the TOD clock by one tenth of a second
	void incrementTOD();

 
    //
    // Speeding up (sleep logic)
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
    Cycle idleSince();
    
    // Total number of cycles the CIA was idle
    Cycle idleTotal() { return idleCycles; }
};


//
// CIA1
//

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
	

//
// CIA2
//

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
