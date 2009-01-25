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

#include "TOD.h"

// Forward declarations
class CPU;
class VIC;
class IEC;
class Keyboard;
class Joystick;

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
	
protected:
	
	//! Reference to the connected video interface controller (VIC). 
	/*! The CIA chip needs to know about the VIC chip, because 
	 1. the video memory bank selection is handled by the CIA chip (register 0xDD00).
	 2. lightpen interrupts can be simulated by writing into a CIA register
	 */
	VIC *vic;
	
	//! Reference to the connected CPU. 
	CPU *cpu;
	
	//! CIA I/O Memory
	/*! Whenever a value is poked to the CIA address space, it is stored here. */
	uint8_t iomem[16];
					
	//! External bus signals on data port A
	/*! \todo Create a separate interface (DeviceInterface or something)
		The keyboard could implement the interface and connect to the CIA (Note: Multiple connections would be needed)
	*/
	uint8_t portLinesA;
	
	//! External bus signals on data port B
	uint8_t portLinesB;
	
	//! Interrupt data register
	/*! "The CIA chip has five internal interrupt sources: timer A underflow, timer B underflow, time-of-day clock alarm, serial 
	     data buffer full or empty, and FLAG signal. The CIA can also generate an interrupt request output signal as a result of any 
	     of these conditions. The location actually has two different functions, depending on whether it is being read from or written to. 
	     When you read from this register, you see the contents of an internal interrupt data register that indicates which interrupts, 
	     if any, have occurred. When you write to this register, the value goes to an internal interrupt mask register that specifies which 
	     interrupts—if any—are to result in an external interrupt request being generated. The data register is read-only 
	     (it can't be written to), and the mask register is write-only (it can't be read from)." [Mapping128]
	 
		Note: The interrupt mask ist stored in iomem[CIA_INTERRUPT_CONTROL].
	*/
	uint8_t interruptDataRegister;
		 
	//! Indicates a change in the control register of timer A
	bool controlRegHasChangedA;

	//! Indicates a change in the control register of timer B
	bool controlRegHasChangedB;

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

	//! Load snapshot from file
	bool load(FILE *file);

	//! Save snapshot to file
	bool save(FILE *file);
	
	//! Dump internal state to console
	void dumpState();	
	
	//! Bind the CIA chip to the specified virtual CPU.
	void setCPU(CPU *c) { assert(cpu == NULL); cpu = c; }

	//! Bind the CIA chip to the specified VIC chip.
	void setVIC(VIC *v) { assert(vic == NULL); vic = v; }

	//! Returns the value of data port A
	inline uint8_t getDataPortA() { return peek(CIA_DATA_PORT_A); }

	//! Sets the current value of data port A
	inline void setDataPortA(uint8_t value) { iomem[CIA_DATA_PORT_A] = value; }

	//! Returns the value of the data port A direction register
	inline uint8_t getDataPortDirectionA() { return iomem[CIA_DATA_DIRECTION_A]; }
	
	//! Sets the current value of the data port A direction register
	inline void setDataPortDirectionA(uint8_t value) { iomem[CIA_DATA_DIRECTION_A] = value; }
	
	//! Returns the value of data port B
	inline uint8_t getDataPortB() { return peek(CIA_DATA_PORT_B); }
	
	//! Sets the current value of data port B
	inline void setDataPortB(uint8_t value) { iomem[CIA_DATA_PORT_B] = value; }
	
	//! Returns the value of the data port B direction register
	inline uint8_t getDataPortDirectionB() { return iomem[CIA_DATA_DIRECTION_B]; }
	
	//! Sets the current value of the data port B direction register
	inline void setDataPortDirectionB(uint8_t value) { iomem[CIA_DATA_DIRECTION_B] = value; }
		
	//! Special peek function for the I/O memory range
	/*! The peek function only handles those registers that are treated similarily by the CIA 1 and CIA 2 chip */
	virtual uint8_t peek(uint16_t addr);
	
	//! Special poke function for the I/O memory range
	/*! The poke function only handles those registers that are treated similarily by the CIA 1 and CIA 2 chip */
	virtual void poke(uint16_t addr, uint8_t value);

	// Interrupt control
	
	//! Returns true, if timer can trigger interrupts
	inline bool isInterruptEnabledA() { return iomem[CIA_INTERRUPT_CONTROL] & 0x01; }

	//! Set or delete interrupt enable flag
	inline void setInterruptEnabledA(bool b) { if (b) iomem[CIA_INTERRUPT_CONTROL] |= 0x01; else iomem[CIA_INTERRUPT_CONTROL] &= (0xff-0x01); }

	//! Toggle interrupt enable flag of timer A
	inline void toggleInterruptEnableFlagA() { setInterruptEnabledA(!isInterruptEnabledA()); }

	//! Returns true, if timer A has reached zero
	inline bool isSignalPendingA() { return interruptDataRegister & 0x01; }

	//! Set or delete signal pending flag
	inline void setSignalPendingA(bool b) { if (b) interruptDataRegister |= 0x01; else interruptDataRegister &= (0xff-0x01); }

	//! Toggle signal pending flag of timer A
	inline void togglePendingSignalFlagA() { setSignalPendingA(!isSignalPendingA()); }
		
	//! Returns true, if timer B can trigger interrupts
	inline bool isInterruptEnabledB() { return iomem[CIA_INTERRUPT_CONTROL] & 0x02; }

	//! Set or delete interrupt enable flag
	inline void setInterruptEnabledB(bool b) { if (b) iomem[CIA_INTERRUPT_CONTROL] |= 0x02; else iomem[CIA_INTERRUPT_CONTROL] &= (0xff-0x02); }

	//! Toggle interrupt enable flag of timer B
	inline void toggleInterruptEnableFlagB() { setInterruptEnabledB(!isInterruptEnabledB()); }

	//! Returns true, if timer B has reached zero
	inline bool isSignalPendingB() { return interruptDataRegister & 0x02; }

	//! Set or delete signal pending flag
	inline void setSignalPendingB(bool b) { if (b) interruptDataRegister |= 0x02; else interruptDataRegister &= (0xff-0x02); }

	//! Toggle signal pending flag of timer B
	inline void togglePendingSignalFlagB() { setSignalPendingB(!isSignalPendingB()); }

	//! Returns true, if the "time of day" interrupt alarm is enabled
	inline bool isInterruptEnabledTOD() { return iomem[CIA_INTERRUPT_CONTROL] & 0x04; }

	//! Enable or disable "time of day" interrupts 
	inline void setInterruptEnabledTOD(bool b) { if (b) iomem[CIA_INTERRUPT_CONTROL] |= 0x04; else iomem[CIA_INTERRUPT_CONTROL] &= (0xff-0x04); }

	// Timer A control

	//!
	inline bool getControlRegA() { return iomem[CIA_CONTROL_REG_A]; }

	//!
	inline void setControlRegA(uint8_t value) { iomem[CIA_CONTROL_REG_A] = value; }
	
	// Timer B control
	
	//!
	inline bool getControlRegB() { return iomem[CIA_CONTROL_REG_B]; }

	//!
	inline void setControlRegB(uint8_t value) { iomem[CIA_CONTROL_REG_B] = value; }
	
	//! Trigger pending interrupts
	inline void triggerInterrupts() {

		if (timerA.triggerInterrupt) {
			timerA.triggerInterrupt = false;
			triggerInterrupt(0x01);
		}
		if (timerB.triggerInterrupt) {
			timerB.triggerInterrupt = false;
			triggerInterrupt(0x02);
		}
	}
	
	// -----------------------------------------------------------------------------------------------
	//                                             General
	// -----------------------------------------------------------------------------------------------
	
	//! Pass control to the CIA chip
	/*! The CIA will be executed for one clock cycle
		The functions decreases all running counters and triggers an CPU interrput if necessary.
	*/
	inline void executeOneCycle() { 
		if (timerA.getState() != TIMER_STOP)
			timerA.executeOneCycle();	
		if (controlRegHasChangedA) {
			controlRegHasChangedA = false;
			timerA.setControlReg(iomem[CIA_CONTROL_REG_A]);
		}
		if (timerB.getState() != TIMER_STOP)
			timerB.executeOneCycle();
		if (controlRegHasChangedB) {
			controlRegHasChangedB = false;
			timerB.setControlReg(iomem[CIA_CONTROL_REG_B]);		
		} 		
	}
	
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
	Joystick *joystickPortA;
	Joystick *joystickPortB;
	Joystick *joy[2];

	bool bKeyboard[2];

	void pollJoystick( Joystick *joy, int joyDevNo );

	void raiseInterruptLine();
	void clearInterruptLine();
	
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
