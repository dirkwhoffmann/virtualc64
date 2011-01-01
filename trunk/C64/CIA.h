/*
 * (C) 2006 - 2009 Dirk W. Hoffmann. All rights reserved.
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

// Last review: 25.7.06

#ifndef _CIA_INC
#define _CIA_INC

#include "Timer.h"
#include "TOD.h"

// Forward declarations
class CPU;
class VIC;
class IEC;
class Keyboard;
class Joystick;

// Adapted from PC64WIN
#define CountA0     0x00000001
#define CountA1     0x00000002
#define CountA2     0x00000004
#define CountA3     0x00000008
#define CountB0     0x00000010
#define CountB1     0x00000020
#define CountB2     0x00000040
#define CountB3     0x00000080
#define LoadA0      0x00000100
#define LoadA1      0x00000200
#define LoadA2      0x00000400
#define LoadB0      0x00000800
#define LoadB1      0x00001000
#define LoadB2      0x00002000
#define PB6Low0     0x00004000
#define PB6Low1     0x00008000
#define PB7Low0     0x00010000
#define PB7Low1     0x00020000
#define Interrupt0  0x00040000
#define Interrupt1  0x00080000
#define OneShotA0   0x00100000
#define OneShotB0   0x00200000
#define DelayMask ~(0x00400000 | CountA0 | CountB0 | LoadA0 | LoadB0 | PB6Low0 | PB7Low0 | Interrupt0 | OneShotA0 | OneShotB0)


//! Virtual complex interface adapter (CIA)
/*! The original C64 consists of two CIA chips (CIA 1 and CIA 2). Each CIA chip features two programmable
	hardware timers and a real-time clock. Furthermore, the CIA chips manage the communication with connected
	peripheral devices such as joysticks, printer or the keyboard.

	The \a CIA class implements the common functionality of both CIA chips.
*/
class CIA : public VirtualComponent {

public:
	//! Start address of the CIA I/O space (CIA 1 and CIA 2)
	static const uint16_t CIA_START_ADDR = 0xDC00;
	//! End address of the CIA I/O space (CIA 1 and CIA 2)
	static const uint16_t CIA_END_ADDR = 0xDDFF;
	
	//! Address offset of the CIA data port register A
	static const uint16_t CIA_DATA_PORT_A = 0x00;
	//! Address offset of the CIA data port register B
	static const uint16_t CIA_DATA_PORT_B = 0x01;
	//! Address offset of the CIA data direction register A
	static const uint16_t CIA_DATA_DIRECTION_A = 0x02;
	//! Address offset of the CIA data direction register B
	static const uint16_t CIA_DATA_DIRECTION_B = 0x03;
	
	//! Address offset to the low-byte of timer A
	/*! The register contains the low-Byte of the current value of timer A. */
	static const uint16_t CIA_TIMER_A_LOW = 0x04;
	//! Address offset to the high-byte of timer A
	/*! The register contains the high-Byte of the current value of timer A. */
	static const uint16_t CIA_TIMER_A_HIGH = 0x05;
	//! Address offset to the low-byte of timer B
	/*! The register contains the low-Byte of the current value of timer B. */
	static const uint16_t CIA_TIMER_B_LOW = 0x06;
	//! Address offset to the high-byte of timer B
	/*! The register contains the HIGH-Byte of the current value of timer B. */
	static const uint16_t CIA_TIMER_B_HIGH = 0x07;

	//! Address offset of the time of day register
	/*! The register contains the "10th of a second" portion of the current time. */
	static const uint16_t CIA_TIME_OF_DAY_SEC_FRAC = 0x08;
	//! Address offset of the time of day register
	/*! The register contains the "seconds" portion of the current time. */
	static const uint16_t CIA_TIME_OF_DAY_SECONDS = 0x09;
	//! Address offset of the time of day register
	/*! The register contains the "minutes" portion of the current time. */
	static const uint16_t CIA_TIME_OF_DAY_MINUTES = 0x0A;
	//! Address offset of the time of day register
	/*! The register contains the "hours" portion of the current time. Bit 7 serves as an AM/PM flag. */
	static const uint16_t CIA_TIME_OF_DAY_HOURS = 0x0B;
	
	//! Address offset of the Synchronous Serial I/O Data Buffer
	static const uint16_t CIA_SERIAL_IO_BUFFER = 0x0C;
	
	//! Address offset of the CIA Interrupt Control Register
	static const uint16_t CIA_INTERRUPT_CONTROL = 0x0D;
	//! Address offset of the CIA Control Register A
	static const uint16_t CIA_CONTROL_REG_A = 0x0E;
	//! Address offset of the CIA Control Register B
	static const uint16_t CIA_CONTROL_REG_B = 0x0F;
	
	//! First timer
	TimerA timerA;
	
	//! Second timer
	TimerB timerB;
	
	//! Time of day clock
	TOD tod;
	
public:
	
	//! Reference to the connected video interface controller (VIC). 
	/*! The CIA chip needs to know about the VIC chip, because 
	 1. the video memory bank selection is handled by the CIA chip (register 0xDD00).
	 2. lightpen interrupts can be simulated by writing into a CIA register
	 */
	VIC *vic;
	
	//! Reference to the connected CPU. 
	CPU *cpu;

public:	
	
	// 
	// Adapted from PC64Win by Wolfgang Lorenz
	//
		
	// control
	uint32_t delay;        // performs delay by shifting left at each clock
	uint32_t feed;         // new bits to feed into dwDelay
	uint8_t CRA;            // control register A
	uint8_t CRB;            // control register B
	uint8_t ICR;            // interrupt control register
	uint8_t IMR;            // interrupt mask register
	uint8_t PB67TimerMode;  // bit mask for PB outputs: 0 = port register, 1 = timer
	uint8_t PB67TimerOut;   // PB outputs bits 6 and 7 in timer mode
	uint8_t PB67Toggle;     // PB outputs bits 6 and 7 in toggle mode
	
	// ports
	uint8_t PALatch;        // buffered output values
	uint8_t PBLatch;
	uint8_t DDRA;           // directions: 0 = input, 1 = output
	uint8_t DDRB;
	
	// interfaces
	uint8_t PA;
	uint8_t PB;
	bool CNT;   // serial clock or input timer clock or timer gate
	bool INT;
	
	 //! Trigger interrupt
	/*! Annotates the interrupt source in the interrupt control register and triggers a CPU interrupt. */
	void triggerInterrupt(uint8_t source);	

	//! Activates the interrupt line
	/*! The function is abstract and will be implemented differently by the CIA 1 and CIA 2 class.
		Whereas the CIA 1 activates the IRQ line, the CIA 2 activates clears the NMI line.
	*/
	virtual void raiseInterruptLine() = 0;	
	
	//! Clears the interrupt line
	/*! The function is abstract and will be implemented differently by the CIA 1 and CIA 2 class.
		Whereas the CIA 1 clears the IRQ line, the CIA 2 chip clears the NMI line.
	*/
	virtual void clearInterruptLine() = 0;	

	//! Get current value of the interrupt line
	/*! The function is abstract and will be implemented differently by the CIA 1 and CIA 2 class.
	 Whereas the CIA 1 polls the IRQ line, the CIA 2 chip polls the NMI line.
	 */
	virtual uint8_t getInterruptLine() = 0;	
	
public:	
	
	//! Returns true if the \a addr is located in the I/O range of one of the two CIA chips
	static inline bool isCiaAddr(uint16_t addr) 
		{ return (CIA_START_ADDR <= addr && addr <= CIA_END_ADDR); }
	
	//! Constructor
	CIA();
	
	//! Destructor
	~CIA();
	
	//! Bring the CIA back to its initial state
	void reset();

	//! Load snapshot
	bool load(uint8_t **buffer);

	//! Save snapshot
	bool save(uint8_t **buffer);
	
	//! Dump internal state
	void dumpState();	

	//! Dump trace line
	void dumpTrace();	

	//! Bind the CIA chip to the specified virtual CPU.
	void setCPU(CPU *c) { assert(cpu == NULL); cpu = c; }

	//! Bind the CIA chip to the specified VIC chip.
	void setVIC(VIC *v) { assert(vic == NULL); vic = v; }

	//! Returns the value of data port A
	inline uint8_t getDataPortA() { return peek(CIA_DATA_PORT_A); }

	//! Sets the current value of data port A
	inline void setDataPortA(uint8_t value) { poke(CIA_DATA_PORT_A, value); }

	//! Returns the value of the data port A direction register
	inline uint8_t getDataPortDirectionA() { return DDRA; }
	
	//! Sets the current value of the data port A direction register
	inline void setDataPortDirectionA(uint8_t value) { DDRA = value; }
	
	//! Returns the value of data port B
	inline uint8_t getDataPortB() { return PB; }
	
	//! Sets the current value of data port B
	inline void setDataPortB(uint8_t value) { poke(CIA_DATA_PORT_B,value); }
	
	//! Returns the value of the data port B direction register
	inline uint8_t getDataPortDirectionB() { return DDRB; }
	
	//! Sets the current value of the data port B direction register
	inline void setDataPortDirectionB(uint8_t value) { DDRB = value; }
		
	//! Special peek function for the I/O memory range
	/*! The peek function only handles those registers that are treated similarily by the CIA 1 and CIA 2 chip */
	virtual uint8_t peek(uint16_t addr);
	
	//! Special poke function for the I/O memory range
	/*! The poke function only handles those registers that are treated similarily by the CIA 1 and CIA 2 chip */
	virtual void poke(uint16_t addr, uint8_t value);

	// Interrupt control
	
	//! Returns true, if timer can trigger interrupts
	inline bool isInterruptEnabledA() { return ICR & 0x01; }

	//! Set or delete interrupt enable flag
	inline void setInterruptEnabledA(bool b) { if (b) ICR |= 0x01; else ICR &= (0xff-0x01); }

	//! Toggle interrupt enable flag of timer A
	inline void toggleInterruptEnableFlagA() { setInterruptEnabledA(!isInterruptEnabledA()); }

	//! Returns true, if timer A has reached zero
	inline bool isSignalPendingA() { return false; } // TODO
	//inline bool isSignalPendingA() { return interruptDataRegister & 0x01; }

	//! Set or delete signal pending flag
	inline void setSignalPendingA(bool b) { } // TODO
	//inline void setSignalPendingA(bool b) { if (b) interruptDataRegister |= 0x01; else interruptDataRegister &= (0xff-0x01); }

	//! Toggle signal pending flag of timer A
	inline void togglePendingSignalFlagA() { setSignalPendingA(!isSignalPendingA()); }
		
	//! Returns true, if timer B can trigger interrupts
	inline bool isInterruptEnabledB() { return ICR & 0x02; }

	//! Set or delete interrupt enable flag
	inline void setInterruptEnabledB(bool b) { if (b) ICR |= 0x02; else ICR &= (0xff-0x02); }

	//! Toggle interrupt enable flag of timer B
	inline void toggleInterruptEnableFlagB() { setInterruptEnabledB(!isInterruptEnabledB()); }

	//! Returns true, if timer B has reached zero
	inline bool isSignalPendingB() { return false; } // TODO
	//inline bool isSignalPendingB() { return interruptDataRegister & 0x02; }

	//! Set or delete signal pending flag
	inline void setSignalPendingB(bool b) { } // TODO
	//inline void setSignalPendingB(bool b) { if (b) interruptDataRegister |= 0x02; else interruptDataRegister &= (0xff-0x02); }
	
	//! Toggle signal pending flag of timer B
	inline void togglePendingSignalFlagB() { setSignalPendingB(!isSignalPendingB()); }

	//! Returns true, if the "time of day" interrupt alarm is enabled
	inline bool isInterruptEnabledTOD() { return ICR & 0x04; }

	//! Enable or disable "time of day" interrupts 
	inline void setInterruptEnabledTOD(bool b) { if (b) ICR |= 0x04; else ICR &= (0xff-0x04); }

	// Timer A control

	//!
	inline bool getControlRegA() { return CRA; }

	//!
	inline void setControlRegA(uint8_t value) { CRA = value; }
	
	// Timer B control
	
	//!
	inline bool getControlRegB() { return CRB; }

	//!
	inline void setControlRegB(uint8_t value) { CRB = value; }
	
	
	// -----------------------------------------------------------------------------------------------
	//                                             General
	// -----------------------------------------------------------------------------------------------
	
	//! Pass control to the CIA chip
	/*! The CIA will be executed for one clock cycle
		The functions decreases all running counters and triggers an CPU interrput if necessary.
	*/
	
inline void executeOneCycle() { 
	_executeOneCycle();
}
	
	void _executeOneCycle();

	//! Increment the TOD clock by one tenth of a second
	/*! Issues an interrupt if the alarm time is reached.
		The function is supposed to be invoked whenever a frame is finished (during VBlank) 
	*/
	void incrementTOD();
};


//! The first virtual complex interface adapter (CIA 1)
/*! The CIA 1 chips differs from the CIA 2 chip in several smaller aspects. For example, the CIA 1 interrupts the
	CPU via the IRQ line (maskable interrupts). Furthermore, the keyboard is connected to the the C64 via the CIA 1 chip.
*/
class CIA1 : public CIA {
	
public:
	
	//! Start address of the CIA 1 I/O space
	static const uint16_t CIA1_START_ADDR = 0xDC00;
	//! End address of the CIA 1 I/O space
	static const uint16_t CIA1_END_ADDR = 0xDCFF;

	//! Joystick bits
	uint8_t joystick[2];
	
private:

	//! Reference to the virtual keyboard
	Keyboard *keyboard;
	Joystick *joy[2];

	bool bKeyboard[2];

	void pollJoystick( Joystick *joy, int joyDevNo );

	void raiseInterruptLine();
	void clearInterruptLine();
	uint8_t getInterruptLine();
	
public:

	//! Constructor
	CIA1();

	//! Destructor
	~CIA1();
	
	//! Bring the CIA back to its initial state
	void reset();
	
	//! Bind the CIA chip to the specified keyboard.
	void setKeyboard(Keyboard *k) { assert(keyboard == NULL); keyboard = k; }
	
	//! Bind the joystick port on the VIC chip to the specified joystick
	void setJoystickToPort( int portNo, Joystick *j );
	
	//! Get the joysick mapped to port A
	Joystick *getJoystickOnPortA() { return joy[0]; }
	
	//! Get the joystick mapped to port B
	Joystick *getJoystickOnPortB() { return joy[1]; }

	//! Bind the keyboard on the VIC chip to the specified game port
	void setKeyboardToPort( int portNo, bool b );
	
	//! Return true, if the keyboard simulates the joystick on gameport A */
	bool getKeyboardOnPortA() { return bKeyboard[0]; }
	
	//! Return true, if the keyboard simulates the joystick on gameport B */
	bool getKeyboardOnPortB() { return bKeyboard[1]; }

	//! Returns true if the \a addr is located in the I/O range of the CIA 1 chip
	static inline bool isCia1Addr(uint16_t addr) 
		{ return (CIA1_START_ADDR <= addr && addr <= CIA1_END_ADDR); }
	
	uint8_t peek(uint16_t addr);
	void poke(uint16_t addr, uint8_t value);
	
	//! Simulates a joystick movement
	/*! \param nr r number (1 or 2)
		\param value bit pattern of joystick movement */
	void setJoystickBits(int nr, uint8_t mask);
	void clearJoystickBits(int nr, uint8_t mask);

	void dumpState();
};
	
//! The second virtual complex interface adapter (CIA 2)
/*! The CIA 2 chips differs from the CIA 1 chip in several smaller aspects. For example, the CIA 2 interrupts the
	CPU via the NMI line (non maskable interrupts). Furthermore, the CIA 2 controlls the memory bank seen by the video controller.
	Therefore, CIA 2 needs to know about the VIC chip, in contrast to CIA 1.
*/
class CIA2 : public CIA {

public:
	
	//! Start address of the CIA 2 I/O space
	static const uint16_t CIA2_START_ADDR = 0xDD00;
	//! End address of the CIA 1 2/O space
	static const uint16_t CIA2_END_ADDR = 0xDDFF;

private:
	
	//! Reference to the connected IEC bus
	IEC *iec;
		
	void raiseInterruptLine();
	void clearInterruptLine();
	uint8_t getInterruptLine();
		
public:

	//! Constructor
	CIA2();
	
	//! Destructor
	~CIA2();
	
	//! Bring the CIA back to its initial state
	void reset();

	//! Bind the CIA chip to the specified IEC bus.
	void setIEC(IEC *i) { assert(iec == NULL); iec = i; }
	
	//! Returns true if the \a addr is located in the I/O range of the CIA 2 chip
	static inline bool isCia2Addr(uint16_t addr) 
		{ return (CIA2_START_ADDR <= addr && addr <= CIA2_END_ADDR); }

	uint8_t peek(uint16_t addr);
	void poke(uint16_t addr, uint8_t value);
	
	void dumpState();
};

#endif
