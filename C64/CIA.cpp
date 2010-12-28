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
	oldPA = 0xff;
	oldPB = 0xff;
	portLinesA = 0xff;
	portLinesB = 0xff;
	
	clearInterruptLine();

	// From PC64WIN
	// reset control
	dwDelay = 0;
	dwFeed = 0;
	bCRA = 0;
	bCRB = 0;
	bICR = 0;
	bIMR = 0;
	bPB67TimerMode = 0;
	bPB67TimerOut = 0;
	bPB67Toggle = 0;
	// reset ports
	bPALatch = 0;
	bPBLatch = 0;
	bDDRA = 0;
	bDDRB = 0;
	PA = 0xff; 
	PB = 0xff; 
	
	
	tod.reset();
	timerA.reset();
	timerB.reset();
}

// Loading and saving snapshots
bool CIA::load(uint8_t **buffer)
{
	
	// TODO
	debug(2, "  Loading CIA state...\n");
	
	for (unsigned i = 0; i < sizeof(16); i++)
		(void)read8(buffer);
	(void)read8(buffer);
	(void)read8(buffer);
	(void)read8(buffer);
	(void)read8(buffer);
	(void)read8(buffer);
	
	tod.load(buffer);
	timerA.load(buffer);
	timerB.load(buffer);

	return true;
}

bool 
CIA::save(uint8_t **buffer)
{
	 debug(2, "  Saving CIA state...\n");
	
	// TODO

	return true;	
}

void 
CIA::triggerInterrupt(uint8_t source)
{
	// Set interrupt source
	bICR |= source; 
	
	// Trigger interrupt, if enabled
	if (bIMR & source) {
		// The uppermost bit indicates that an interrupt occured
		// printf("Triggering CIA interrupt (source = %02X) at cycle %d\n", source, (int)cpu->getCycles());
		bICR |= 0x80;
		raiseInterruptLine();
	}
}

uint8_t CIA::peek(uint16_t addr)
{
	uint8_t result;
	
	switch(addr) {		
		case CIA_DATA_DIRECTION_A:	
			dump();
			return bDDRA;
		case CIA_DATA_DIRECTION_B:
			dump();
			return bDDRB;
		case CIA_TIMER_A_LOW:  
			dump();
			return timerA.getLatchLo();
		case CIA_TIMER_A_HIGH: 
			dump();
			return timerA.getLatchHi();
		case CIA_TIMER_B_LOW:  
			dump();
			return timerB.getLatchLo();
		case CIA_TIMER_B_HIGH: 
			dump();
			return timerB.getLatchHi();
		case CIA_TIME_OF_DAY_SEC_FRAC:
			dump();
			tod.defreeze();
			return BinaryToBCD(tod.getTodTenth());
		case CIA_TIME_OF_DAY_SECONDS:
			dump();
			return BinaryToBCD(tod.getTodSeconds());
		case CIA_TIME_OF_DAY_MINUTES:
			dump();
			return BinaryToBCD(tod.getTodMinutes());
		case CIA_TIME_OF_DAY_HOURS:
			dump();
			tod.freeze();
			return (tod.getTodHours() & 0x80) /* AM/PM */ | BinaryToBCD(tod.getTodHours() & 0x1F);
		case CIA_SERIAL_IO_BUFFER:
			dump();
			return 0x00; // 0xff;
			
		case CIA_INTERRUPT_CONTROL:
		
			result = bICR;
			
			// get status of the Int line into bit 7 and draw Int high
			if (getInterruptLine()) {
				
				// I guess this never happens, because the interrupt line is cleared before the CPU executes
				// The current order is: CIA -> Interrupt handling -> CPU
				result |= 0x80;
				clearInterruptLine();			
			}
			
			// discard pending interrupts
			dwDelay &= ~(Interrupt0 | Interrupt1);
			
			// set all events to 0
			bICR = 0;
			
			dump();
			return result;

		case CIA_CONTROL_REG_A:
			dump();
			return (uint8_t)(bCRA & ~0x10); // Bit 4 is always 0 when read
			
		case CIA_CONTROL_REG_B:
			dump();
			return (uint8_t)(bCRB & ~0x10); // Bit 4 is always 0 when read
			
		default:
			panic("Unknown CIA address %04X\n", addr);
	}
	return 0x00;
}

void CIA::poke(uint16_t addr, uint8_t value)
{
	switch(addr) {
			
		case CIA_TIMER_A_LOW:

			timerA.setLatchLo(value);
			dump();
			return;
			
		case CIA_TIMER_A_HIGH:
			
			timerA.setLatchHi(value);			
			// load counter if timer is stopped
			if ((bCRA & 0x01) == 0) {
				dwDelay |= LoadA0;
			}
			dump();
			return;
			
		case CIA_TIMER_B_LOW:  
			
			timerB.setLatchLo(value);
			dump();
			return;
			
		case CIA_TIMER_B_HIGH: 
			
			timerB.setLatchHi(value);
			// load counter if timer is stopped
			if ((bCRB & 0x01) == 0) {
				dwDelay |= LoadB0;
			}
			dump();
			return;
			
		case CIA_TIME_OF_DAY_SEC_FRAC:
			if (value & 0x80) {
				tod.setAlarmTenth(BCDToBinary(value & 0x0F));
			} else { 
				tod.setTodTenth(BCDToBinary(value & 0x0F));
				tod.cont();
			}
			dump();
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
			// TODO
			//triggerInterrupt(0x08);
			return;
			
		case CIA_INTERRUPT_CONTROL:
			
			// bit 7 means set (1) or clear (0) the other bits
			if ((value & 0x80) != 0) {
				bIMR |= (value & 0x1F);
			} else {
				bIMR &= ~(value & 0x1F);
			}
			
			// raise an interrupt in the next cycle if condition matches
			if ((bIMR & bICR) != 0) {
				if (!getInterruptLine()) {
					dwDelay |= Interrupt0;
				}
			}
			dump();
			return;
			
		case CIA_CONTROL_REG_A:
		{

			// save old values for recognizing changes
			// uint16_t dwSavedDelay = dwDelay;
			// uint16_t dwSavedFeed = dwFeed;
			
			// output PB67 changes only once
			bool fPBChanged = false;
			
			// set clock in o2 mode // todo cnt
			if ((value & 0x21) == 0x01) {
				dwDelay |= CountA1 | CountA0;
				dwFeed |= CountA0;
			} else {
				dwDelay &= ~(CountA1 | CountA0);
				dwFeed &= ~CountA0;
			}
			
			// set one shot mode
			if ((value & 0x08) != 0) {
				dwFeed |= OneShotA0;
			} else {
				dwFeed &= ~OneShotA0;
			}
			
			// set force load
			if ((value & 0x10) != 0) {
				dwDelay |= LoadA0;
			}
			
			// set toggle high on rising edge of Start
			if ((value & 0x01) != 0 && (bCRA & 0x01) == 0) {
				if ((bCRA & 0x06) == 0x06 && (bPB67Toggle & 0x40) != 0x40) {
					fPBChanged = true;
				}
				bPB67Toggle |= 0x40;
			}
			
			// timer A output to PB6
			if ((value & 0x06) != (bCRA & 0x06)) {
				fPBChanged = true;
			}
			if ((value & 0x02) == 0) {
				bPB67TimerMode &= ~0x40;
			} else {
				bPB67TimerMode |= 0x40;
				if ((value & 0x04) == 0) {
					if ((dwDelay & PB7Low1) == 0) {
						bPB67TimerOut &= ~0x40;
					} else {
						bPB67TimerOut |= 0x40;
					}
				} else {
					bPB67TimerOut = (bPB67TimerOut & ~0x40) | (bPB67Toggle & 0x40);
				}
			}
			
			// write PB67 if they have changed
			if (fPBChanged) {
				PB = ((bPBLatch | ~bDDRB) & ~bPB67TimerMode) | (bPB67TimerOut & bPB67TimerMode);
			}
			
			// set the register
			bCRA = value;
			
			dump();
			return;
		}
			
		case CIA_CONTROL_REG_B:
		{
			// save old values for recognizing changes
			//dword dwSavedDelay = dwDelay;
			//dword dwSavedFeed = dwFeed;
			
			// output PB67 changes only once
			bool fPBChanged = false;
			
			// set clock in o2 mode // todo cnt
			if ((value & 0x61) == 0x01) {
				dwDelay |= CountB1 | CountB0;
				dwFeed |= CountB0;
			} else {
				dwDelay &= ~(CountB1 | CountB0);
				dwFeed &= ~CountB0;
			}
			
			// set one shot mode
			if ((value & 0x08) != 0) {
				dwFeed |= OneShotB0;
			} else {
				dwFeed &= ~OneShotB0;
			}
			
			// set force load
			if ((value & 0x10) != 0) {
				dwDelay |= LoadB0;
			}
			
			// set toggle high on rising edge of Start
			if ((value & 0x01) != 0 && (bCRB & 0x01) == 0) {
				if ((bCRB & 0x06) == 0x06 && (bPB67Toggle & 0x80) != 0x80) {
					fPBChanged = true;
				}
				bPB67Toggle |= 0x80;
			}
			
			// timer B output to PB7
			if ((value & 0x06) != (bCRB & 0x06)) {
				fPBChanged = true;
			}
			if ((value & 0x02) == 0) {
				bPB67TimerMode &= ~0x80;
			} else {
				bPB67TimerMode |= 0x80;
				if ((value & 0x04) == 0) {
					if ((dwDelay & PB7Low1) == 0) {
						bPB67TimerOut &= ~0x80;
					} else {
						bPB67TimerOut |= 0x80;
					}
				} else {
					bPB67TimerOut = (bPB67TimerOut & ~0x80) | (bPB67Toggle & 0x80);
				}
			}
			
			// write PB67 if they have changed
			if (fPBChanged) {
				PB = ((bPBLatch | ~bDDRB) & ~bPB67TimerMode) | (bPB67TimerOut & bPB67TimerMode);
			}
			
			// set the register
			bCRB = value;
			
			dump();
			return;			
		}
			
		default:
			panic("PANIC: Unknown CIA address (poke) %04X\n", addr);
	}	
}

void 
CIA::incrementTOD()
{
	if (tod.increment()) {
		triggerInterrupt(0x04);
	}
}

void CIA::dump()
{
	if (!cpu->c64->logfile)
		return;
	fprintf(cpu->c64->logfile, "\nPA: %02X (%02X) PB: %02X (%02X) DDRA: %02X DDRB: %02X CRA: %02X CRB: %02X ICR: %02X IMR: %02X ",
			PA, bPALatch, PB, bPBLatch, bDDRA, bDDRB, bCRA, bCRB, bICR, bIMR);
	fprintf(cpu->c64->logfile, "A: %02X (%02X) B: %02X (%02X)\n",
			timerA.counter, timerA.latch, timerB.counter, timerB.latch);
	fprintf(cpu->c64->logfile, "%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s\n\n",
			dwDelay & CountA0 ? "CountA0 " : "",
			dwDelay & CountA1 ? "CountA1 " : "",
			dwDelay & CountA2 ? "CountA2 " : "",
			dwDelay & CountA3 ? "CountA3 " : "",
			dwDelay & CountB0 ? "CountB0 " : "",
			dwDelay & CountB1 ? "CountB1 " : "",
			dwDelay & CountB2 ? "CountB2 " : "",
			dwDelay & CountB3 ? "CountB3 " : "",
			dwDelay & LoadA0 ? "LoadA0 " : "",
			dwDelay & LoadA1 ? "LoadA1 " : "",
			dwDelay & LoadB0 ? "LoadB0 " : "",
			dwDelay & LoadB1 ? "LoadB1 " : "",
			dwDelay & PB6Low0 ? "PB6Low0 " : "",
			dwDelay & PB6Low1 ? "PB6Low1 " : "",
			dwDelay & PB7Low0 ? "PB7Low0 " : "",
			dwDelay & PB7Low1 ? "PB7Low1 " : "",
			dwDelay & Interrupt0 ? "Interrupt0 " : "",
			dwDelay & Interrupt1 ? "Interrupt1 " : "",
			dwDelay & OneShotA0 ? "OneShotA0 " : "",
			dwDelay & OneShotB0 ? "OneShotB0 " : "");
	
}

void CIA::dumpState()
{
	debug(1, "            Data port A : %02X\n", getDataPortA());
	debug(1, "            Data port B : %02X\n", getDataPortA());
	debug(1, "  Data port direction A : %02X\n", getDataPortDirectionA());
	debug(1, "  Data port direction B : %02X\n", getDataPortDirectionB());
	debug(1, "  External port lines A : %02X\n", portLinesA);
	debug(1, "  External port lines B : %02X\n", portLinesB);
	debug(1, "     Control register A : %02X\n", getControlRegA());
	debug(1, "     Control register B : %02X\n", getControlRegB());
//	debug(1, "Interrupt data register : %02X\n", ???);
	debug(1, "     Timer A interrupts : %s\n", isInterruptEnabledA() ? "enabled" : "disabled");	
	debug(1, "     Timer B interrupts : %s\n", isInterruptEnabledA() ? "enabled" : "disabled");	
	debug(1, "         TOD interrupts : %s\n", isInterruptEnabledTOD() ? "enabled" : "disabled");	
	debug(1, "       Signal pending A : %s\n", isSignalPendingA() ? "yes" : "no");
	debug(1, "       Signal pending B : %s\n", isSignalPendingB() ? "yes" : "no");
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

void CIA::_executeOneCycle()
{
	bool timerAOutput;
	
	// Adapted from PC64Win
	
	// Pictures taken from "A Software Model of the CIA6526" from Wolfgang Lorenz 

	// Figure 3: Layout of timer (A and B)
	//
    //                              Phi2            Phi2                  Phi2
	//                               |               |                     |
	// timerA      -----       ------v------   ------v------ (1) ----------v-----------
	// input  ---->| & |------>| dwDelay & |-X-| dwDelay & |---->| decrement counter  |
	//         --->|   |       |  CountA2  | | |  CountA3  |     |                    |
	//         |   -----       ------------- | -------------     |                    |
	// -----------------             ^ Clr   |                   |                    |
	// | bCRA & 0x01   |             |       | ------------------| new counter ==  0? |
	// | timer A start |<----        |       | |                 |                    |
	// -----------------    |        |       v v                 |                    |
 	//                    -----      |      -----                |      timer A       |
	//                    | & |      |      | & |                |   16 bit counter   |
	//                    |   |      |      |   |                |     and latch      |
	//                    -----      |      -----                |                    |
    //                     ^ ^       |        |(2)               |                    |
    //                     | |       ---------|-------------     |                    |
    //                     | |                |            |     |                    |    
	// timer A             | |                |    -----   |     |                    |
	// output  <-----------|-X----------------X--->|>=1|---X---->| load from latch    | 
	//                     |                   --->|   |(8)      |                    |
	//                    ----- (7)            |   -----         ----------------------
	//                    |>=1|                |
	//                    |   |                |       Phi2
	//                    -----                |        |
	//                     ^ ^                 |  ------v------      ----------------
	//                     | |                 ---| dwDelay & |<-----| bcRA & 0x10  |
	//                     | -----------------    |  LoadA1   |      | force load   |
	//                     |       Phi2      |    -------------      ----------------
    //                     |        |        |                              ^ Clr
	// -----------------   |  ------v------  |                              |
	// | bCRA & 0x08   |   |  | dwDelay & |  |                             Phi2
	// | one shot      |---X->| oneShotA0 |---
	// -----------------      -------------

	// Figure 4: Timer output to PB6 (timer A) and PB7 (timer B)
    //                            
	//                                      -----------------
	//          --------------------------->| 0x00 (pulse)  |
	//          |                           |               | (6.2) ----------------
	//          |                           | bCRA & 0x04   |------>| 0x02 (timer) |
	// timerA   |  Flip ---------------     | timer mode    |       |              |
	// output  -X------>| bPB67Toggle |---->| 0x04 (toggle) |       | bCRA & 0x02  | (6)
	//              (5) |  ^ 0x04     |     |               |       | output mode  |----> PB6 output
	//                  ---------------     -----------------       |              |
	//                        ^ Set                           (6.1) | 0x00 (port)  |
	//                        |                                ---->|              |
	// ----------------- 0->1 |             -----------------  |    ----------------
	// | bCRA & 0x01   |-------             | port B bit 6  |---  
	// | timer A start |                    |    output     |
	// -----------------                    -----------------
	

	// Figure 5: Interrupt logic
    //                            
	//                  ----------
	//                  | bIMR & |----
	//                  |  0x01  |   |    -----
	//                  ----------   ---->| & |----
	// timerA   (3) Set ----------   ---->|   |   |
	// output  -------->| bICR & |   |    -----   |
	//           ------>|  0x01  |----            |  -----
	//           |  Clr ----------                -->|>=1|---
	//           |      ----------                -->|   |  |
	//           |      | bIMR & |----            |  -----  |
	//           |      |  0x02  |   |    -----   |         |
	//           |      ----------   ---->| & |----         |
	// timerB    |  Set ----------   ---->|   |             |
	// output  --|----->| bICR & |   |    -----             |
	//           X----->|  0x01  |----                      |
	//           |  Clr ----------       	                |
	// read      |                                          |
	// ICR ------X-------------X----------------            |
	//                         |               |            |
	//                         v Clr           v Clr        |
	//           ------    ----------    ----------------   | (4)
	// Int    <--| -1 |<---| bICR & |<---|   dwDelay &  |<---
	// ouptput   |    |    |  0x80  |Set |  Interrupt1  |     
	//           ------    ----------    -------^--------   	
	//                                          |
	//                                         Phi2
	
	
	// don't output PB67 changes more than once
	bool fPBChanged = false; // ????
				
	// ------------------------- TIMER A --------------------------------
	// (1) : decrement counter A
	if ((dwDelay & CountA3) != 0) {
		timerA.counter--;
	}
	
	// (2) : underflow counter A
	timerAOutput = (timerA.counter == 0 && (dwDelay & CountA2) != 0);
	
	if (timerAOutput) {
		
		// (3) : signal underflow event
		bICR |= 0x01;
		
		// (4) : underflow interrupt in next clock
		if ((bIMR & 0x01) != 0) {
			dwDelay |= Interrupt0;
		}
		
		// (5) toggle underflow counter bit
		bPB67Toggle ^= 0x40;
		
		// (6) timer A output to PB6
		if ((bCRA & 0x02) != 0) {
			
			// (6.1) set PB6 high for one clock
			if ((bCRA & 0x04) == 0) {
				bPB67TimerOut |= 0x40;
				dwDelay |= PB6Low0;
				dwDelay &= ~PB6Low1;
				
			} else {
				// (6.2) toggle PB6 between high and low
				// Dirk: bP67TimerOut = bPB67Toggle ? (bP67TimerOut | 0x40) : (bP67TimerOut & 0xBF);
				bPB67TimerOut ^= 0x40; 
				assert((bPB67TimerOut & 0x40) == (bPB67Toggle & 0x40));
			}
			
			// output new state
			fPBChanged = true;
		}
		
		// (7) stop timer in one shot mode
		if (((dwDelay | dwFeed) & OneShotA0) != 0) {
			bCRA &= ~0x01;
			dwDelay &= ~(CountA2 | CountA1 | CountA0);
			dwFeed &= ~CountA0;
		}
		
		// timer A output to timer B in cascade mode
		if ((bCRB & 0x61) == 0x41) { // 		} || (bCRB & 0x61) == 0x61 && CNT.IsInputHigh()) {
			dwDelay |= CountB1;
		}
		
		// (8) : load counter A
		dwDelay |= LoadA1;
	}
	
	// (8) : load counter A
	if ((dwDelay & LoadA1) != 0) {
		timerA.reloadTimer(); // load counter from latch
		
		// don't decrement counter in next clock
		dwDelay &= ~CountA2;
	}
	
	// ------------------------- TIMER B --------------------------------
	// decrement counter B
	if ((dwDelay & CountB3) != 0) {
		timerB.counter--;
	}
	
	// underflow counter B
	if (timerB.counter == 0 && (dwDelay & CountB2) != 0) {
		
		// signal underflow event
		bICR |= 0x02;
		
		// underflow interrupt in next clock
		if ((bIMR & 0x02) != 0) {
			dwDelay |= Interrupt0;
		}
		
		// toggle underflow counter bit
		bPB67Toggle ^= 0x80;
		
		// timer B output to PB7
		if ((bCRB & 0x02) != 0) {
			
			// set PB7 high for one clock
			if ((bCRB & 0x04) == 0) {
				bPB67TimerOut |= 0x80;
				dwDelay |= PB7Low0;
				dwDelay &= ~PB7Low1;
				
				// toggle PB7 between high and low
			} else {
				bPB67TimerOut ^= 0x80;
				assert((bPB67TimerOut & 0x80) == (bPB67Toggle & 0x80));
			}
			
			// output new state
			fPBChanged = true;
		}
		
		// stop timer in one shot mode
		if (((dwDelay | dwFeed) & OneShotB0) != 0) {
			bCRB &= ~0x01;
			dwDelay &= ~(CountB2 | CountB1 | CountB0);
			dwFeed &= ~CountB0;
		}
		
		// load counter B
		dwDelay |= LoadB1;
	}
	
	// load counter B
	if ((dwDelay & LoadB1) != 0) {
		timerB.reloadTimer();
		
		// don't decrement counter in next clock
		dwDelay &= ~CountB2;
	}
	
	// set PB67 back to low
	if ((dwDelay & (PB6Low1 | PB7Low1)) != 0) {
		if ((dwDelay & PB6Low1) != 0) {
			bPB67TimerOut &= ~0x40;
		}
		if ((dwDelay & PB7Low1) != 0) {
			bPB67TimerOut &= ~0x80;
		}
		fPBChanged = true;
	}
	
	// write new PB if it has changed
	if (fPBChanged) {
		PB = ((bPBLatch | ~bDDRB) & ~bPB67TimerMode) | (bPB67TimerOut & bPB67TimerMode);
	}
	
	// set interrupt register and interrupt line
	if ((dwDelay & Interrupt1) != 0) {
		//if (Int.IsOutputHigh()) {
		//	Int.SetOutputLow();
		//}
		//bICR |= 0x80; // Dirk
		raiseInterruptLine();
	}


	// next clock
	 dwDelay = (dwDelay << 1) & DelayMask | dwFeed;

#if 0
	// link out of clock chain if there are no more pending events
	assert(!TimerIdle.IsCounting());
	if (gfAllowIdle && dwNewDelay == dwDelay) {
		switch (dwDelay & (CountA3 | CountB3)) {
			case CountA3:
				if (iCounterA >= giIdleThreshold) {
					TimerIdle.StartCounter(iCounterA - 1);
					iCounterA = 1;
					SetIdle();
				}
				break;
			case CountB3:
				if (iCounterB >= giIdleThreshold) {
					TimerIdle.StartCounter(iCounterB - 1);
					iCounterB = 1;
					SetIdle();
				}
				break;
			case CountA3 | CountB3:
				if (iCounterA >= giIdleThreshold && iCounterB >= giIdleThreshold) {
					int iClocks = min(iCounterA, iCounterB) - 1;
					TimerIdle.StartCounter(iClocks);
					iCounterA -= iClocks;
					iCounterB -= iClocks;
					SetIdle();
				}
				break;
			default:
				SetIdle();
		}
	}

	
	dwDelay = dwNewDelay;
#endif	
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
CIA1::getInterruptLine()
{
	return cpu->getIRQLineCIA();
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
			result = PA; // iomem[addr] | ~iomem[CIA_DATA_DIRECTION_A];
			
			// The external port lines can pull down any bit, even if it configured as output
			// result &= portLinesA; 
			
			// Check joystick movement
			result &= joystick[0];
			
			dump();
			return result;
			
		case CIA_DATA_PORT_B:
		{
			//uint8_t bitmask = oldPA; //CIA1::peek(CIA_DATA_PORT_A);
			uint8_t bitmask = CIA1::peek(CIA_DATA_PORT_A);
			uint8_t keyboardBits = keyboard->getRowValues(bitmask); 
			
			if ( joy[1] != NULL )
				pollJoystick( joy[1], 2 );
			
			result = PB;
			
			// Check joystick movement
			result &= joystick[1];
			
			// Check for pressed keys
			result &= keyboardBits;
						
			dump();
			return result;
		}
			
		default:
			return CIA::peek(addr);	
	}
}

void 
CIA1::poke(uint16_t addr, uint8_t value)
{
	assert(addr <= CIA1_END_ADDR - CIA1_START_ADDR);
	
	// The following registers need special handling	
	switch(addr) {
			
		case CIA_DATA_PORT_A: 
			
			bPALatch = value;
			PA = bPALatch | ~bDDRA;
			oldPA = PA;
			dump();
			return;
			
		case CIA_DATA_DIRECTION_A:

			bDDRA = value;
			PA = bPALatch | ~bDDRA;
			oldPA = PA;
			dump();
			return;
			
		case CIA_DATA_PORT_B:
			
			bPBLatch = value;
			PB = ((bPBLatch | ~bDDRB) & ~bPB67TimerMode) | (bPB67TimerOut & bPB67TimerMode);
			oldPB = PB;
			dump();
			return;
			
		case CIA_DATA_DIRECTION_B:

			bDDRB = value;
			PB = ((bPBLatch | ~bDDRB) & ~bPB67TimerMode) | (bPB67TimerOut & bPB67TimerMode);
			oldPB = PB;
			dump();
			return;
		
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
CIA2::getInterruptLine()
{
	return cpu->getNMILineCIA();
}

uint8_t 
CIA2::peek(uint16_t addr)
{
	uint8_t result;
	
	assert(addr <= CIA_END_ADDR - CIA_START_ADDR);
	
	switch(addr) {
		case CIA_DATA_PORT_A:
			
			result = PA;
			
			// The two upper bits are connected to the clock line and the data line
			result &= 0x3F;
			result |= (iec->getClockLine() ? 0x40 : 0x00);
			result |= (iec->getDataLine() ? 0x80 : 0x00);

			// The external port lines can pull down any bit, even if it configured as output.
			// Note that bits 0 and 1 are not connected to the bus and determine the memory bank seen by the VIC chip
			// result &= (portLinesB | 0x03);
			
			dump();
			return result;
						
		case CIA_DATA_PORT_B:
			
			result = PB;		
			dump();
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
			
			bPALatch = value;
			PA = bPALatch | ~bDDRA;

			// Bits 0 and 1 determine the memory bank seen the VIC
			vic->setMemoryBankAddr((~PA & 0x03) << 14);	

			// Bits 3 to 5 of PA are connected to the IEC bus
			iec->updateCiaPins(bPALatch, bDDRA);
			oldPA = PA;
			dump();
			return;
			
		case CIA_DATA_DIRECTION_A:
			
			bDDRA = value;
			PA = bPALatch | ~bDDRA;
			
			// Bits 0 and 1 determine the memory bank seen the VIC
			vic->setMemoryBankAddr((~PA & 0x03) << 14);	
			
			// Bits 3 to 5 of PA are connected to the IEC bus
			iec->updateCiaPins(bPALatch, bDDRA);
			oldPA = PA;
			dump();
			return;
			
		case CIA_DATA_PORT_B:
			
			bPBLatch = value;
			PB = ((bPBLatch | ~bDDRB) & ~bPB67TimerMode) | (bPB67TimerOut & bPB67TimerMode);
			oldPB = PB;
			dump();
			return;

		case CIA_DATA_DIRECTION_B:
			
			bDDRB = value;
			PB = ((bPBLatch | ~bDDRB) & ~bPB67TimerMode) | (bPB67TimerOut & bPB67TimerMode);
			oldPB = PB;
			dump();
			return;
			
		default:
			CIA::poke(addr, value);
	}
}
			
