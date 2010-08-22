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

#include "C64.h"

// Memory layout
//
// DC00-DCFF	56320-56575	MOS 6526 Complex Interface Adapter (CIA) #1
// -----------------------------------------------------------------------------------------
// DC00       56320                 Data Port A (Keyboard, Joystick, Paddles, Light-Pen)
//                           7-0    Write Keyboard Column Values for Keyboard Scan
//                           7-6    Read Paddles on Port A / B (01 = Port A, 10 = Port B)
//                           4      Joystick A Fire Button: 1 = Fire
//                           3-2    Paddle Fire Buttons
//                           3-0    Joystick A Direction (0-15)
// DC01       56321                 Data Port B (Keyboard, Joystick, Paddles): Game Port 1
//                           7-0    Read Keyboard Row Values for Keyboard Scan
//                           7      Timer B Toggle/Pulse Output
//                           6      Timer A: Toggle/Pulse Output
//							 4      Joystick 1 Fire Button: 1 = Fire
//                           3-2    Paddle Fire Buttons
//                           3-0    Joystick 1 Direction
//
// DC02       56322                 Data Direction Register - Port A (56320)
// DC03       56323                 Data Direction Register - Port B (56321)
// DC04       56324                 Timer A: Low-Byte
// DC05       56325                 Timer A: High-Byte
// DC06       56326                 Timer B: Low-Byte
// DC07       56327                 Timer B: High-Byte
//
// DC08       56328                 Time-of-Day Clock: 1/10 Seconds
// DC09       56329                 Time-of-Day Clock: Seconds
// DC0A       56330                 Time-of-Day Clock: Minutes
// DC0B       56331                 Time-of-Day Clock: Hours + AM/PM Flag (Bit 7)
// DC0C       56332                 Synchronous Serial I/O Data Buffer
// DC0D       56333                 CIA Interrupt Control Register (Read IRQs/Write Mask)
//                           7      IRQ Flag (1 = IRQ Occurred) / Set-Clear Flag
//                           4      FLAG1 IRQ (Cassette Read / Serial Bus SRQ Input)
//                           3      Serial Port Interrupt
//                           2      Time-of-Day Clock Alarm Interrupt
//                           1      Timer B Interrupt
//                           0      Timer A Interrupt
// DC0E       56334                 CIA Control Register A
//                           7      Time-of-Day Clock Frequency: 1 = 50 Hz, 0 = 60 Hz
//                           6      Serial Port I/O Mode Output, 0 = Input
//                           5      Timer A Counts: 1 = CNT Signals, 0 = System 02 Clock
//                           4      Force Load Timer A: 1 = Yes
//                           3      Timer A Run Mode: 1 = One-Shot, 0 = Continuous
//							 2      Timer A Output Mode to PB6: 1 = Toggle, 0 = Pulse
//                           1      Timer A Output on PB6: 1 = Yes, 0 = No
//                           0      Start/Stop Timer A: 1 = Start, 0 = Stop
// DC0F       56335                 CIA Control Register B
//                           7      Set Alarm/TOD-Clock: 1 = Alarm, 0 = Clock
//                           6-5    Timer B Mode Select:
//                                  00 = Count System 02 Clock Pulses
//                                  01 = Count Positive CNT Transitions
//                                  10 = Count Timer A Underflow Pulses
//                                  11 = Count Timer A Underflows While	CNT Positive
//                           4-0    Same as CIA Control Reg. A - for Timer B

// DD00-DDFF  56576-56831           MOS 6526 Complex Interface Adapter (CIA) #2
// -----------------------------------------------------------------------------------------

//	DD00       56576                 Data Port A (Serial Bus, RS-232, VIC Memory Control)
//                            7      Serial Bus Data Input
//                            6      Serial Bus Clock Pulse Input
//                            5      Serial Bus Data Output
//                            4      Serial Bus Clock Pulse Output
//                            3      Serial Bus ATN Signal Output
//                            2      RS-232 Data Output (User Port)
//                            1-0    VIC Chip System Memory Bank Select (Default = 11)
//	DD01       56577                 Data Port B (User Port, RS-232)
//                            7      User / RS-232 Data Set Ready
//                            6      User / RS-232 Clear to Send
//                            5      User
//                            4      User / RS-232 Carrier Detect
//                            3      User / RS-232 Ring Indicator
//                            2      User / RS-232 Data Terminal Ready
//                            1      User / RS-232 Request to Send
//                            0      User / RS-232 Received Data
//  DD02       56578                 Data Direction Register - Port A
//  DD03       56579                 Data Direction Register - Port B
//  DD04       56580                 Timer A: Low-Byte
//  DD05       56581                 Timer A: High-Byte
//  DD06       56582                 Timer B: Low-Byte
//  DD07       56583                 Timer B: High-Byte
//  DD08       56584                 Time-of-Day Clock: 1/10 Seconds
//  DD09       56585                 Time-of-Day Clock: Seconds
//  DD0A       56586                 Time-of-Day Clock: Minutes
//  DD0B       56587                 Time-of-Day Clock: Hours + AM/PM Flag (Bit 7)
//  DD0C       56588                 Synchronous Serial I/O Data Buffer
//  DD0D       56589                 CIA Interrupt Control Register (Read NMls/Write Mask)
//	                          7      NMI Flag (1 = NMI Occurred) / Set-Clear Flag
//                            4      FLAG1 NMI (User/RS-232 Received Data Input)
//                            3      Serial Port Interrupt
//                            1      Timer B Interrupt
//                            0      Timer A Interrupt
//  DD0E       56590                 CIA Control Register A
//                            7      Time-of-Day Clock Frequency: 1 = 50 Hz, 0 = 60 Hz
//                            6      Serial Port I/O Mode Output, 0 = Input
//                            5      Timer A Counts: 1 = CNT Signals, 0 = System 02 Clock
//                            4      Force Load Timer A: 1 = Yes
//                            3      Timer A Run Mode: 1 = One-Shot, 0 = Continuous
//                            2      Timer A Output Mode to PB6: 1 = Toggle, 0 = Pulse
//                            1      Timer A Output on PB6: 1 = Yes, 0 = No
//                            0      Start/Stop Timer A: 1 = Start, 0 = Stop
//  DD0F       56591                 CIA Control Register B
//                            7      Set Alarm/TOD-Clock: 1=Alarm, 0=Clock
//                            6-5    Timer B Mode Select:
//                                   00 = Count System 02 Clock Pulses
//                                   01 = Count Positive CNT Transitions
//                                   10 = Count Timer A Underflow Pulses
//                                   11 = Count Timer A Underflows While CNT Positive
//                            4-0    Same as CIA Control Reg. A - for Timer B
//  DE00-DEFF  56832-57087           Reserved for Future I/O Expansion
//  DF00-DFFF  57088-57343           Reserved for Future I/O Expansion



// To be supported:
//
// DC0D, DD0D: Interrupt controll register
// DC00 : Data Port A
// Timer A Run Mode = One shot (CIA#1)
// Timer A Run Mode = One shot (CIA#2)
// Timer B Run Mode = One shot (CIA#1)
// Timer B Run Mode = One shot (CIA#2)
// CIA #1 Timer A, lo und hi byte
// Timer A, 50 Hz start

/* Programmierung des CIA
Zur Steuerung von CIA-Interrupts dient das schon erwähnte Interrupt-Control-Register (ICR) $DC0D. 
Dieses Register hat zwei Funktionen, je nachdem, ob schreibend oder lesend darauf zugegriffen wird. 
Bei Lesezugriff zeigt es an, ob, und wenn ja, woher ein Interrupt ausgelöst wurde.
Zugleich wird das Register gelöscht und die Interrupt-Anforderung zurückgenommen 
(Die IRQ-Leitung geht von low auf high). Die Bits 0-4 sind dabei verschiedenen Interruptquellen zugeordnet. 
Uns interessieren hier nur die Bits 0 und 1, welche zu den Timer-Interrupts gehören. Durch einen Schreibzugriff 
wird dagegen ein Masken-Register angesprochen. Damit lassen sich die Interruptquellen einzeln freigeben oder sperren. 
Die Bits 0-4 kann man einzeln setzen oder zurücksetzen. Ist im geschriebenen Byte Bit 7 gesetzt, wird jedes mit 
einer 1 beschriebene Bit gesetzt, während die anderen Bits unverändert bleiben. Ist Bit 7 rückgesetzt, so wird jedes 
mit einer 1 beschriebene Bit zurückgesetzt, während die anderen Bits wieder unverändert bleiben. Gesetzte Bits ermöglichen 
eine Interrupterzeugung durch die jeweilige Quelle. Die Freigabe der Interrupterzeugung durch Timer B sieht also so aus:

LDA #%10000010
STA $DD0D ;ICR Bit 1 setzen

Der Timer selbst wird durch drei Register gesteuert.

Das Registerpaar TIMER B ($DC06/$DC07) liefert bei Lesezugriff den aktuellen 16-Bit-Zählerstand. Dieser Wert wird 
kontinuierlich heruntergezählt. Bei Erreichen von Null stoppt der Timer entweder (One-Shot-Mode) oder lädt einen Wert aus 
einem Timer-Latch (Latch = Zwischenspeicher) nach und zählt von neuem herunter (Continous Mode ). Bei diesem Timer-Unterlauf 
wird ein Interrupt erzeugt, wenn Bit 1 im ICR gesetzt ist. Ein Schreibzugriff auf TIMER A bezieht sich dagegen auf das 16-Bit-Latch. 
Mit dem Latch-Wert kann man die Zeit zwischen zwei Interrupts im Bereich von 1 bis 65535 Mikrosekunden steuern.

Das Register CRB (Control Register B, $DC0F) steuert die Betriebsart des Timers (Start/Stop, One Shot/Contmous, u.a.) 
Durch LDA #%00010001
STA $DD0E
wird der Zählerstand mit dem Latch-Wert geladen und der Timer gestartet.
*/

CIA::CIA()
{
	name = "CIA";
	cpu = NULL;
    vic = NULL;
	timerA.setCIA(this);
	timerB.setCIA(this);
	timerA.setOtherTimer(&timerB);
	timerB.setOtherTimer(&timerA);
}

CIA::~CIA()
{
}

void
CIA::reset() 
{
	memset(iomem, 0, sizeof(iomem));	
	iomem[CIA_DATA_PORT_A] = 0xff;
	iomem[CIA_DATA_PORT_B] = 0xff;	

	portLinesA = 0xff;
	portLinesB = 0xff;
	interruptDataRegister = 0;
	controlRegHasChangedA = false;
	controlRegHasChangedB = false;
	tod.reset();
	timerA.reset();
	timerB.reset();
}

// Loading and saving snapshots
bool CIA::load(uint8_t **buffer)
{
	debug(2, "  Loading CIA state...\n");
	
	for (unsigned i = 0; i < sizeof(iomem); i++)
		iomem[i] = read8(buffer);
	portLinesA = read8(buffer);
	portLinesB = read8(buffer);
	interruptDataRegister = read8(buffer);
	controlRegHasChangedA = (bool)read8(buffer);
	controlRegHasChangedB = (bool)read8(buffer);
	tod.load(buffer);
	timerA.load(buffer);
	timerB.load(buffer);

	return true;
}

bool 
CIA::save(uint8_t **buffer)
{
	 debug(2, "  Saving CIA state...\n");
	
	for (unsigned i = 0; i < sizeof(iomem); i++)
		write8(buffer, iomem[i]);
	write8(buffer, portLinesA);
	write8(buffer, portLinesB);
	write8(buffer, interruptDataRegister);
	write8(buffer, controlRegHasChangedA);
	write8(buffer, controlRegHasChangedB);
	tod.save(buffer);
	timerA.save(buffer);
	timerB.save(buffer);

	return true;	
}

void 
CIA::triggerInterrupt(uint8_t source)
{
	// Set interrupt source
	interruptDataRegister |= source; 
	
	// Trigger interrupt, if enabled
	if (iomem[CIA_INTERRUPT_CONTROL] & source) {
		// The uppermost bit indicates that an interrupt occured
		// printf("Triggering CIA interrupt (source = %02X) at cycle %d\n", source, (int)cpu->getCycles());
		interruptDataRegister |= 0x80;
		raiseInterruptLine();
	}
}

uint8_t CIA::peek(uint16_t addr)
{
	uint8_t result;
	
	switch(addr) {		
		case CIA_DATA_DIRECTION_A:	
			return iomem[addr];
		case CIA_DATA_DIRECTION_B:
			return iomem[addr];
		case CIA_TIMER_A_LOW:  
			return LO_BYTE(timerA.getTimer());			
		case CIA_TIMER_A_HIGH: 
			return HI_BYTE(timerA.getTimer());			
		case CIA_TIMER_B_LOW:  
			return LO_BYTE(timerB.getTimer());
		case CIA_TIMER_B_HIGH: 
			return HI_BYTE(timerB.getTimer());
		case CIA_TIME_OF_DAY_SEC_FRAC:
			tod.defreeze();
			return BinaryToBCD(tod.getTodTenth());
		case CIA_TIME_OF_DAY_SECONDS:
			return BinaryToBCD(tod.getTodSeconds());
		case CIA_TIME_OF_DAY_MINUTES:
			return BinaryToBCD(tod.getTodMinutes());
		case CIA_TIME_OF_DAY_HOURS:
			tod.freeze();
			return (tod.getTodHours() & 0x80) /* AM/PM */ | BinaryToBCD(tod.getTodHours() & 0x1F);
		case CIA_SERIAL_IO_BUFFER:
			return 0xff;
		case CIA_INTERRUPT_CONTROL:
			result = interruptDataRegister;
			interruptDataRegister = 0;
			// Clear interrupt request
			clearInterruptLine();			
			return result;
		case CIA_CONTROL_REG_A:
			return timerA.getControlReg() & 0xEF; // Bit 4 is always 0 when read
		case CIA_CONTROL_REG_B:
			return timerB.getControlReg() & 0xEF; // Bit 4 is always 0 when read
		default:
			debug(1, "PANIC: Unknown CIA address %04X\n", addr);
			assert(0);
	}
	return 0x00;
}

void CIA::poke(uint16_t addr, uint8_t value)
{
	switch(addr) {
		case CIA_TIMER_A_LOW:
			timerA.setTimerLatchLo(value);
			iomem[addr] = value; 
			return;
			
		case CIA_TIMER_A_HIGH:
			timerA.setTimerLatchHi(value);
			iomem[addr] = value; 
			return;
			
		case CIA_TIMER_B_LOW:  
			timerB.setTimerLatchLo(value);		
			iomem[addr] = value; 
			return;
			
		case CIA_TIMER_B_HIGH: 
			timerB.setTimerLatchHi(value);
			iomem[addr] = value; 
			return;
			
		case CIA_TIME_OF_DAY_SEC_FRAC:
			if (value & 0x80) {
				tod.setAlarmTenth(BCDToBinary(value & 0x0F));
			} else { 
				tod.setTodTenth(BCDToBinary(value & 0x0F));
				tod.cont();
			}
			return;
			
		case CIA_TIME_OF_DAY_SECONDS:
			if (value & 0x80)
				tod.setAlarmSeconds(BCDToBinary(value & 0x7F));
			else 
				tod.setTodSeconds(BCDToBinary(value & 0x7F));
			return;
			
		case CIA_TIME_OF_DAY_MINUTES:
			if (value & 0x80)
				tod.setAlarmMinutes(BCDToBinary(value & 0x7F));
			else 
				tod.setTodMinutes(BCDToBinary(value & 0x7F));
			return;
			
		case CIA_TIME_OF_DAY_HOURS:
			if (value & 0x80) {
				tod.setAlarmHours((value & 0x80) /* AM/PM */ | BCDToBinary(value & 0x1F));
			} else {
				tod.setTodHours((value & 0x80) /* AM/PM */ | BCDToBinary(value & 0x1F));
				tod.stop();
			}
			return;
			
		case CIA_SERIAL_IO_BUFFER:
			// Serial I/O communication is not (yet) implemented
			// We simply acknowledge the operation (interrupt) and discard the value
			triggerInterrupt(0x08);
			return;
			
		case CIA_INTERRUPT_CONTROL:
			if (value & 0x80) {
				uint8_t mask = (iomem[addr] ^ value) & value & 0x7F; // get bits 0..6 with a raising edge
				iomem[addr] |= (value & 0x7F);
				// trigger pending interrupts
				triggerInterrupt(interruptDataRegister & mask);
			} else { 
				iomem[addr] &= ~value;
			}
			return;
			
		case CIA_CONTROL_REG_A:
			if (iomem[addr] != value) {
				if (!(iomem[addr] & 0x01) && (value & 0x01)) {
					timerA.underflow_toggle = true; // Rising edge on start bit
				}
				iomem[addr] = value; 
				timerA.setCountingModes(value);
				controlRegHasChangedA = true; // Note: The value will be pushed to the timer in the next cycle
			}
			return;
			
		case CIA_CONTROL_REG_B:
			if (!(iomem[addr] & 0x01) && (value & 0x01)) // Rising edge on start bit
				timerB.underflow_toggle = true;
			
			if (iomem[addr] != value) {
				iomem[addr] = value; 
				timerB.setCountingModes(value);
				controlRegHasChangedB = true; // Note: The value will be pushed to the timer in the next cycle
			}
			return;			

		default:
			debug(1, "PANIC: Unknown CIA address (poke) %04X\n", addr);
			assert(0);
	}	
}

void 
CIA::incrementTOD()
{
	if (tod.increment())
		triggerInterrupt(0x04);
}
		
void CIA::dumpState()
{
	debug(1, "            Data port A : %02X\n", getDataPortA());
	debug(1, "            Data port B : %02X\n", getDataPortA());
	debug(1, "  Data port direction A : %02X\n", getDataPortDirectionA());
	debug(1, "  Data port direction B : %02X\n", getDataPortDirectionB());
	debug(1, "  External port lines A : %02X\n", portLinesA);
	debug(1, "  External port lines B : %02X\n", portLinesB);
	debug(1, "     Control register A : %02X %s\n", getControlRegA(), controlRegHasChangedA ? "(just changed)" : "(stable)");
	debug(1, "     Control register B : %02X %s\n", getControlRegB(), controlRegHasChangedB ? "(just changed)" : "(stable)");
	debug(1, "Interrupt data register : %02X\n", portLinesB);
	debug(1, "     Timer A interrupts : %s\n", isInterruptEnabledA() ? "enabled" : "disabled");	
	debug(1, "     Timer B interrupts : %s\n", isInterruptEnabledA() ? "enabled" : "disabled");	
	debug(1, "         TOD interrupts : %s\n", isInterruptEnabledTOD() ? "enabled" : "disabled");	
	debug(1, "       Signal pending A : %s\n", isSignalPendingA() ? "yes" : "no");
	debug(1, "       Signal pending B : %s\n", isSignalPendingB() ? "yes" : "no");
	
	debug(1, "              IO memory : ");
	for (unsigned j = 0; j < sizeof(iomem); j++) {
			debug(1, "%02X ", iomem[j]);
	}
	debug(1, "\n\n");
	debug(1, "Timer A:\n");
	debug(1, "--------\n\n");
	timerA.dumpState();
	debug(1, "Timer B:\n");
	debug(1, "--------\n\n");
	timerB.dumpState();
	debug(1, "Time of day clock:\n");
	debug(1, "------------------\n\n");
	tod.dumpState();
}

// -----------------------------------------------------------------------------------------
// Complex Interface Adapter 1
// -----------------------------------------------------------------------------------------

CIA1::CIA1()
{
	debug(2, "  Creating CIA1 at address %p...\n", this);

	keyboard = NULL;
	joystick[0] = 0xff;
	joystick[1] = 0xff;	
}

CIA1::~CIA1()
{
	debug(2, "  Releasing CIA1\n");
}

void 
CIA1::reset()
{
	debug(2, "  Resetting CIA1...\n");
	CIA::reset();
}

void 
CIA1::dumpState()
{
	debug(1, "CIA 1:\n");
	debug(1, "------\n\n");
	CIA::dumpState();
}

void 
CIA1::setJoystickToPort( int portNo, Joystick *j ) {
	joy[portNo] = j;
}

void 
CIA1::setKeyboardToPort( int portNo, bool b ) {
	bKeyboard[portNo] = b;
}

void 
CIA1::raiseInterruptLine()
{
	cpu->setIRQLineCIA();
}

void 
CIA1::clearInterruptLine()
{
	cpu->clearIRQLineCIA();
}

uint8_t 
CIA1::peek(uint16_t addr)
{
	uint8_t result;
	
	assert(addr <= CIA1_END_ADDR - CIA1_START_ADDR);
	
	switch(addr) {		
		case CIA_DATA_PORT_A:

			if ( joy[0] != NULL )
					pollJoystick( joy[0], 1 );

			// We change only those bits that are configured as outputs, all input bits are 1
			result = iomem[addr] | ~iomem[CIA_DATA_DIRECTION_A];
			
			// The external port lines can pull down any bit, even if it configured as output
			result &= portLinesA; 

			// Check joystick movement
			result &= joystick[0];
						
			return result;

		case CIA_DATA_PORT_B:
		{
			uint8_t bitmask = CIA1::peek(CIA_DATA_PORT_A);

			if ( joy[1] != NULL )
					pollJoystick( joy[1], 2 );

			// We change only those bits that are configured as outputs, all input bits are 1
			result = iomem[addr] | ~iomem[CIA_DATA_DIRECTION_B];
			// The external port lines can pull down any bit, even if it configured as output
			result &= portLinesB; 

			// Check joystick movement
			result &= joystick[1];
			
			// Check for pressed keys
			result &= keyboard->getRowValues(bitmask); 
			
			// Bit 6 and 7 can have special timer output functions
			if (timerA.willIndicateUnderflow()) {
				result &= 0xBF; // Clear bit 6
				if (timerA.willIndicateUnderflowAsPulse() ? timerA.underflow : timerA.underflow_toggle)
					result |= 0x40; // Set bit 6
			}
			if (timerB.willIndicateUnderflow()) {
				result &= 0x7F; // Clear bit 7
				if (timerB.willIndicateUnderflowAsPulse() ? timerB.underflow : timerB.underflow_toggle)
					result |= 0x80; // Set bit 7
			}
			
			return result;
		}
		default:
			return CIA::peek(addr);	
	}
}


void 
CIA1::pollJoystick( Joystick *joy, int joyDevNo ) {
	JoystickAxisState leftRightState	= joy->GetAxisX(); 
	JoystickAxisState upDownState		= joy->GetAxisY();
	bool buttonState					= joy->GetButtonPressed();
		
	// up/down
	// set the down bit: 2, 2 and clear up bit: 2, 1		
	// ATTENTION: clearJoystickBits( x, y ) means pressed and setJoystickBits( x, y ) means released
	if( upDownState == JOYSTICK_AXIS_Y_UP ) {
		clearJoystickBits(joyDevNo, 1);
		setJoystickBits(joyDevNo, 2);
	} else if( upDownState == JOYSTICK_AXIS_Y_DOWN ) {
		clearJoystickBits(joyDevNo, 2);
		setJoystickBits(joyDevNo, 1);
	} else {
		setJoystickBits(joyDevNo, 1);			
		setJoystickBits(joyDevNo, 2);
	}
	
	// left/right
	if( leftRightState == JOYSTICK_AXIS_X_LEFT ) {
		clearJoystickBits(joyDevNo, 4);
		setJoystickBits(joyDevNo, 8);
	} else if( leftRightState == JOYSTICK_AXIS_X_RIGHT ) {
		clearJoystickBits(joyDevNo, 8);			
		setJoystickBits(joyDevNo, 4);
	} else {
		setJoystickBits(joyDevNo, 4);			
		setJoystickBits(joyDevNo, 8);
	}
		
	// fire
	if( buttonState ) {
		clearJoystickBits(joyDevNo, 16);
	} else {
		setJoystickBits(joyDevNo, 16);
	}
}

void 
CIA1::poke(uint16_t addr, uint8_t value)
{
	assert(addr <= CIA1_END_ADDR - CIA1_START_ADDR);
	
	// The following registers need special handling	
	switch(addr) {
		case CIA_DATA_PORT_A: 
		case CIA_DATA_DIRECTION_A:
			iomem[addr] = value;
			return;
		case CIA_DATA_PORT_B:
		case CIA_DATA_DIRECTION_B:
		{
			uint8_t lp_bit_old = (iomem[CIA_DATA_PORT_B] | ~iomem[CIA_DATA_DIRECTION_B]) & 0x10;
			iomem[addr] = value;
			uint8_t lp_bit_new = (iomem[CIA_DATA_PORT_B] | ~iomem[CIA_DATA_DIRECTION_B]) & 0x10;
			if (lp_bit_old != lp_bit_new) {
				// edge on lightpen bit
				debug(1, "Lightpen edge detected\n");
				vic->simulateLightPenInterrupt();
			}
			return;
		}
		default:
			CIA::poke(addr, value);
	}
}
	
void 
CIA1::setJoystickBits(int nr, uint8_t mask)
{
	assert(nr == 1 || nr == 2);
	
	if (nr == 1) joystick[0] |= mask;
	else if (nr == 2) joystick[1] |= mask;
}

void 
CIA1::clearJoystickBits(int nr, uint8_t mask)
{
	assert(nr == 1 || nr == 2);
	
	if (nr == 1) joystick[0] &= (0xff-mask);
	else if (nr == 2) joystick[1] &= (0xff-mask);
}


// -----------------------------------------------------------------------------------------
// Complex Interface Adapter 2
// -----------------------------------------------------------------------------------------

CIA2::CIA2()
{
	debug(2, "  Creating CIA2 at address %p...\n", this);

	iec = NULL;
}

CIA2::~CIA2()
{
	debug(2, "  Releasing CIA2...\n");
}

void CIA2::reset()
{
	debug(2, "  Resetting CIA2...\n");
	CIA::reset();
}

void 
CIA2::dumpState()
{
	debug(1, "CIA 2:\n");
	debug(1, "------\n\n");
	CIA::dumpState();
}

void 
CIA2::raiseInterruptLine()
{
	cpu->setNMILineCIA();
}

void 
CIA2::clearInterruptLine()
{
	cpu->clearNMILineCIA();
}

uint8_t 
CIA2::peek(uint16_t addr)
{
	uint8_t result;
	
	assert(addr <= CIA_END_ADDR - CIA_START_ADDR);
	
	switch(addr) {
		case CIA_DATA_PORT_A:
			// We change only those bits that are configured as outputs, all input bits are 1
			result = iomem[addr] | ~iomem[CIA_DATA_DIRECTION_A];

			// The two upper bits are connected to the clock line and the data line
			result &= 0x3F;
			result |= (iec->getClockLine() ? 0x40 : 0x00);
			result |= (iec->getDataLine() ? 0x80 : 0x00);

			// The external port lines can pull down any bit, even if it configured as output.
			// Note that bits 0 and 1 are not connected to the bus and determine the memory bank seen by the VIC chip
			result &= (portLinesB | 0x03);
			return result;
						
		case CIA_DATA_PORT_B:
			result = iomem[addr] | ~iomem[CIA_DATA_DIRECTION_B];
					
			// Bit 6 and 7 can have special timer output functions
			if (timerA.willIndicateUnderflow()) {
				result &= 0xBF; // Clear bit 6
				if (timerA.willIndicateUnderflowAsPulse() ? timerA.underflow : timerA.underflow_toggle)
					result |= 0x40; // Set bit 6
			}
			if (timerB.willIndicateUnderflow()) {
				result &= 0x7F; // Clear bit 7
				if (timerB.willIndicateUnderflowAsPulse() ? timerB.underflow : timerB.underflow_toggle)
					result |= 0x80; // Set bit 7
			}
						
			return result;
			
		default:
			return CIA::peek(addr);
	}
}

void 
CIA2::poke(uint16_t addr, uint8_t value)
{
	assert(addr <= CIA2_END_ADDR - CIA2_START_ADDR);

	switch(addr) {
		case CIA_DATA_PORT_A:
			iomem[addr] = value;					
			// Bits 0 and 1 determine the memory bank seen the VIC
			vic->setMemoryBankAddr((~(value | ~iomem[CIA_DATA_DIRECTION_A]) & 0x03) << 14);	
			// Bits 3 to 5 are connected to the IEC bus
			iec->updateCiaPins(iomem[CIA_DATA_PORT_A], iomem[CIA_DATA_DIRECTION_A]);			
			return;
			
		case CIA_DATA_PORT_B:
			iomem[addr] = value;
			return;

		case CIA_DATA_DIRECTION_A:
			iomem[addr] = value;
			iec->updateCiaPins(iomem[CIA_DATA_PORT_A], iomem[CIA_DATA_DIRECTION_A]);
			return;

		case CIA_DATA_DIRECTION_B:
			iomem[addr] = value;
			return;
			
		default:
			CIA::poke(addr, value);
	}
}
			
