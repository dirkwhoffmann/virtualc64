/*
 * (C) 2006 Dirk W. Hoffmann. All rights reserved.
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
	//! Total number of CIA registers
	static const uint16_t NO_OF_REGISTERS = 16;
	
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
	
protected:
				
	//! CIA I/O Memory
	/*! Whenever a value is poked to the CIA address space, it is stored here. */
	uint8_t iomem[NO_OF_REGISTERS];
		
	//! Reference to the connected CPU. 
	CPU *cpu;
	
	//! Current value of timer A
	uint16_t timerA;

	//! Current value of timer B
	uint16_t timerB;

	//! Latched value for data port A
	uint8_t dataPortA;
	
	//! Latched value for data port B
	uint8_t dataPortB;
	
	//! External bus signals on data port A
	/*! \todo Create a separate interface (DeviceInterface or something)
		The keyboard could implement the interface and connect to the CIA (Note: Multiple connections would be needed)
	*/
	uint8_t portLinesA;
	
	//! External bus signals on data port B
	uint8_t portLinesB;
	
	//! Interrupt control bits for timer A and timer B
	uint8_t interruptControl;
				
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
	//! Current value of the "time of day" clock (TOD)
	/*! The value is counted in 1/10th secods */
	TOD tod;
	
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
	
	//! Bind the CIA chip to the specified virtual CPU.
	/*! The binding is irreversible. */
	void setCPU(CPU *c) { assert(cpu == NULL); cpu = c; }

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

	//! Returns the current value of timer A
	inline uint16_t getTimerA() { return timerA; }

	//! Sets the current value of timer A
	inline void setTimerA(uint16_t value) { timerA = value; }

	//! Returns the start value of timer A
	inline uint16_t getTimerLatchA() { return LO_HI(iomem[CIA_TIMER_A_LOW], iomem[CIA_TIMER_A_HIGH]); }
	
	//! Sets the start value of timer A
	inline void setTimerLatchA(uint16_t value) { iomem[CIA_TIMER_A_LOW] = LO_BYTE(value); iomem[CIA_TIMER_A_HIGH] = HI_BYTE(value); }
	
	//! Returns the current value of timer B
	inline uint16_t getTimerB() { return timerB; }
	
	//! Sets the current value of timer B
	inline void setTimerB(uint16_t value) { timerB = value; }

	//! Returns the start value of timer B
	inline uint16_t getTimerLatchB() { return LO_HI(iomem[CIA_TIMER_B_LOW], iomem[CIA_TIMER_B_HIGH]); }

	//! Sets the start value of timer B
	inline void setTimerLatchB(uint16_t value) { iomem[CIA_TIMER_B_LOW] = LO_BYTE(value); iomem[CIA_TIMER_B_HIGH] = HI_BYTE(value); }

	//! Triggered when timer A reaches zero
	void timerActionA();

	//! Triggered when timer B reaches zero
	void timerActionB();
	
	//! Special peek function for the I/O memory range
	/*! The peek function only handles those registers that are treated similarily by the CIA 1 and CIA 2 chip */
	virtual uint8_t peek(uint16_t addr);
	
	//! Special poke function for the I/O memory range
	/*! The poke function only handles those registers that are treated similarily by the CIA 1 and CIA 2 chip */
	virtual void poke(uint16_t addr, uint8_t value);

	// Interrupt control
	
	//! Returns true, if timer can trigger interrupts
	inline bool isInterruptEnabledA() { return interruptControl & 0x01; }

	//! Set or delete interrupt enable flag
	inline void setInterruptEnabledA(bool b) { if (b) interruptControl |= 0x01; else interruptControl &= (0xff-0x01); }

	//! Toggle interrupt enable flag of timer A
	inline void toggleInterruptEnableFlagA() { setInterruptEnabledA(!isInterruptEnabledA()); }

	//! Returns true, if timer A has reached zero
	inline bool isSignalPendingA() { return iomem[CIA_INTERRUPT_CONTROL] & 0x01; }

	//! Set or delete signal pending flag
	inline void setSignalPendingA(bool b) { if (b) iomem[CIA_INTERRUPT_CONTROL] |= 0x01; else iomem[CIA_INTERRUPT_CONTROL] &= (0xff-0x01); }

	//! Toggle signal pending flag of timer A
	inline void togglePendingSignalFlagA() { setSignalPendingA(!isSignalPendingA()); }
	
	//! Returns true, if timer B can trigger interrupts
	inline bool isInterruptEnabledB() { return interruptControl & 0x02; }

	//! Set or delete interrupt enable flag
	inline void setInterruptEnabledB(bool b) { if (b) interruptControl |= 0x02; else interruptControl &= (0xff-0x02); }

	//! Toggle interrupt enable flag of timer B
	inline void toggleInterruptEnableFlagB() { setInterruptEnabledB(!isInterruptEnabledB()); }

	//! Returns true, if timer B has reached zero
	inline bool isSignalPendingB() { return iomem[CIA_INTERRUPT_CONTROL] & 0x02; }

	//! Set or delete signal pending flag
	inline void setSignalPendingB(bool b) { if (b) iomem[CIA_INTERRUPT_CONTROL] |= 0x02; else iomem[CIA_INTERRUPT_CONTROL] &= (0xff-0x02); }

	//! Toggle signal pending flag of timer B
	inline void togglePendingSignalFlagB() { setSignalPendingB(!isSignalPendingB()); }

	//! Returns true, if the "time of day" interrupt alarm is enabled
	inline bool isInterruptEnabledTOD() { return interruptControl & 0x04; }

	//! Enable or disable "time of day" interrupts 
	inline void setInterruptEnabledTOD(bool b) { if (b) interruptControl |= 0x04; else interruptControl &= (0xff-0x04); }

	
	// Timer A control
	
	//! Returns true, if timer A is running, 0 if stopped
	inline bool isStartedA() { return iomem[CIA_CONTROL_REG_A] & 0x01; }

	//! Start or stop timer A
	inline void setStartedA(bool b) { if (b) iomem[CIA_CONTROL_REG_A] |= 0x01; else iomem[CIA_CONTROL_REG_A] &= (0xff-0x01); }

	//! Toggle start flag of timer A
	inline void toggleStartFlagA() { setStartedA(!isStartedA()); }
		
	//! Returns true, if an underflow of timer A will be indicated in bit #6 in Port B register
	inline bool willIndicateUnderflowA() { return iomem[CIA_CONTROL_REG_A] & 0x02; }

	//! Enable or disable underflow indication
	inline void setIndicateUnderflowA(bool b) { if (b) iomem[CIA_CONTROL_REG_A] |= 0x02; else iomem[CIA_CONTROL_REG_A] &= (0xFF-0x02); }

	//! Toggle underflow indication flag of timer A
	inline void toggleUnderflowFlagA() { setIndicateUnderflowA(!willIndicateUnderflowA()); }
	
	//! Returns true, if A is in "one shot" mode
	inline bool isOneShotA() { return iomem[CIA_CONTROL_REG_A] & 0x08; }

	//! Enable or disable one-shot-mode 
	inline void setOneShotA(bool b) { if (b) iomem[CIA_CONTROL_REG_A] |= 0x08; else iomem[CIA_CONTROL_REG_A] &= (0xff-0x08); }

	//! Toggle one shot flag of timer A
	inline void toggleOneShotFlagA() { setOneShotA(!isOneShotA()); }
		
	//! Load latched value into timer 
	inline void reloadTimerA() { timerA = (iomem[CIA_TIMER_A_HIGH] << 8) | iomem[CIA_TIMER_A_LOW]; }
	
	
	// Timer B control

	//! Returns true, if timer B is started, 0 if stopped
	inline bool isStartedB() { return iomem[CIA_CONTROL_REG_B] & 0x01; }

	//! Start or stop timer B
	inline void setStartedB(bool b) { if (b) iomem[CIA_CONTROL_REG_B] |= 0x01; else iomem[CIA_CONTROL_REG_B] &= (0xff-0x01); }

	//! Toggle start flag of timer B
	inline void toggleStartFlagB() { setStartedB(!isStartedB()); }

	//! Returns true, if an underflow of timer B will be indicated in bit #7 in Port B register
	inline bool willIndicateUnderflowB() { return iomem[CIA_CONTROL_REG_B] & 0x02; }

	//! Enable or disable underflow indication
	inline void setIndicateUnderflowB(bool b) { if (b) iomem[CIA_CONTROL_REG_B] |= 0x02; else iomem[CIA_CONTROL_REG_B] &= (0xFF-0x02); }

	//! Toggle underflow indication flag of timer B
	inline void toggleUnderflowFlagB() { setIndicateUnderflowB(!willIndicateUnderflowB()); }

	//! Returns true, if B is in "one shot" mode
	inline bool isOneShotB() { return iomem[CIA_CONTROL_REG_B] & 0x08; }

	//! Enable or disable one-shot-mode of timer B
	inline void setOneShotB(bool b) { if (b) iomem[CIA_CONTROL_REG_B] |= 0x08; else iomem[CIA_CONTROL_REG_B] &= (0xff-0x08); }	

	//! Toggle one shot flag of timer B
	inline void toggleOneShotFlagB() { setOneShotB(!isOneShotB()); }

	//! Returns true, if B couts underflows
	inline bool getCountUnderflowFlagB() { return iomem[CIA_CONTROL_REG_B] & 0x40; }
	
	//! Set count-underflow-flag of timer B
	inline void setCountUnderflowFlagB(bool b) { if (b) iomem[CIA_CONTROL_REG_B] |= 0x40; else iomem[CIA_CONTROL_REG_B] &= (0xff-0x40); }	
	
	//! Toggle count-underflow-flag of timer B
	inline void toggleCountUnderflowFlagB() { setCountUnderflowFlagB(!getCountUnderflowFlagB()); }
	
	//! Load latched value into timer 
	inline void reloadTimerB() { timerB = (iomem[CIA_TIMER_B_HIGH] << 8) | iomem[CIA_TIMER_B_LOW]; }

	
	// -----------------------------------------------------------------------------------------------
	//                                             General
	// -----------------------------------------------------------------------------------------------
	
	//! Pass control to the CIA chip
	/*! The CIA will be executed for one clock cycle
		The functions decreases all running counters and triggers an CPU interrput if necessary.
	*/
	void executeOneCycle(); 

	//! Pass control to the CIA chip
	/*! The CIA will be executed for the specified number of clock cycles.
		\param cycles Number of cycles to execute
	*/
	void execute(int cycles); 
	
	//! Increment the TOD clock by one tenth of a second
	/*! Issues an interrupt if the alarm time is reached.
		The function is supposed to be invoked whenever a frame is finished (during VBlank) 
	*/
	void incrementTOD();

	void dumpState();	
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
	
	//! Bind the VIC chip to the specified keyboard.
	/*! The binding is irreversible. */
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
	void joystickAction(int nr, uint8_t value);
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
		
	//! Reference to the connected video interface controller (VIC). 
	/*! Use \a setVIC to set the value during initialization.
		The CIA chip needs to know about the VIC chip, because the video memory bank selection 
		is handled by the CIA chip (register 0xDD00).
		
		\warning The variable is "write once".
	*/
	VIC *vic;

	//! Reference to the connected IEC bus
	IEC *iec;
		
	void raiseInterruptLine();
	void clearInterruptLine();
		
public:

	//! Constructor
	CIA2();
	//! Destructor
	~CIA2();
	
	//! Bind the CIA chip to the specified VIC chip.
	/*! The binding is irreversible. */
	void setVIC(VIC *v) { assert(vic == NULL); vic = v; }

	//! Bind the CIA chip to the specified IEC bus.
	/*! The binding is irreversible. */
	void setIEC(IEC *i) { assert(iec == NULL); iec = i; }
	
	//! Returns true if the \a addr is located in the I/O range of the CIA 2 chip
	static inline bool isCia2Addr(uint16_t addr) 
		{ return (CIA2_START_ADDR <= addr && addr <= CIA2_END_ADDR); }

	uint8_t peek(uint16_t addr);
	void poke(uint16_t addr, uint8_t value);
	
	void dumpState();
};

#endif
