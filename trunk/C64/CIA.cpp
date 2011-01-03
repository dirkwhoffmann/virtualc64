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

CIA::CIA()
{
	name = "CIA";

	cpu = NULL;
    vic = NULL;
}

CIA::~CIA()
{
}

void
CIA::reset() 
{	
	clearInterruptLine();

	// reset control
	delay = 0;
	feed = 0;
	CRA = 0;
	CRB = 0;
	ICR = 0;
	IMR = 0;
	PB67TimerMode = 0;
	PB67TimerOut = 0;
	PB67Toggle = 0;

	// reset ports
	PALatch = 0;
	PBLatch = 0;
	DDRA = 0;
	DDRB = 0;
	PA = 0xff; 
	PB = 0xff; 
	CNT = true; // CNT line is high by default
	INT = 1;
	
	counterA = 0x0000;
	latchA = 0xFFFF;
	counterB = 0x0000;
	latchB = 0xFFFF;
	
	tod.reset();
}

// Loading and saving snapshots
bool CIA::load(uint8_t **buffer)
{
	
	// TODO
	debug(2, "  Loading CIA state...\n");
	
	delay = read32(buffer);
	feed = read32(buffer);
	CRA = read8(buffer);
	CRB = read8(buffer);
	ICR = read8(buffer);
	IMR = read8(buffer);
	PB67TimerMode = read8(buffer);
	PB67TimerOut = read8(buffer);
	PB67Toggle = read8(buffer);
	
	PALatch = read8(buffer);
	PBLatch = read8(buffer);
	DDRA = read8(buffer);
	DDRB = read8(buffer);
	
	PA = read8(buffer);
	PB = read8(buffer);
	
	CNT = (bool)read8(buffer);
	INT = (bool)read8(buffer);
		
	counterA = read16(buffer);
	latchA = read16(buffer);
	counterB = read16(buffer);
	latchB = read16(buffer);

	tod.load(buffer);

	return true;
}

bool 
CIA::save(uint8_t **buffer)
{
	debug(2, "  Saving CIA state...\n");
	
	write32(buffer, delay);
	write32(buffer, feed);
	write8(buffer, CRA);
	write8(buffer, CRB);
	write8(buffer, ICR);
	write8(buffer, IMR);
	write8(buffer, PB67TimerMode);
	write8(buffer, PB67TimerOut);
	write8(buffer, PB67Toggle);
	
	write8(buffer, PALatch);
	write8(buffer, PBLatch);
	write8(buffer, DDRA);
	write8(buffer, DDRB);
	
	write8(buffer, PA);
	write8(buffer, PB);
	
	write8(buffer, (uint8_t)CNT);
	write8(buffer, (uint8_t)INT);
	
	write16(buffer, counterA);
	write16(buffer, latchA);
	write16(buffer, counterB);
	write16(buffer, latchB);

	tod.save(buffer);
	
	return true;	
}

uint8_t CIA::peek(uint16_t addr)
{
	uint8_t result;
	
	switch(addr) {		
		case CIA_DATA_DIRECTION_A:	

			result = DDRA;
			break;

		case CIA_DATA_DIRECTION_B:

			result = DDRB;
			break;
			
		case CIA_TIMER_A_LOW:  
			
			result = getCounterALo();
			break;
			
		case CIA_TIMER_A_HIGH: 
			result = getCounterAHi();
			break;
			
		case CIA_TIMER_B_LOW:  

			result = getCounterBLo();
			break;
			
		case CIA_TIMER_B_HIGH: 
			
			result = getCounterBHi();
			break;
			
		case CIA_TIME_OF_DAY_SEC_FRAC:
			
			// debug("peek CIA_TIME_OF_DAY_SEC_FRAC\n");
			tod.defreeze();
			result = BinaryToBCD(tod.getTodTenth());
			break;
		
		case CIA_TIME_OF_DAY_SECONDS:
			
			// debug("peek CIA_TIME_OF_DAY_SECONDS\n");
			result = BinaryToBCD(tod.getTodSeconds());
			break;
			
		case CIA_TIME_OF_DAY_MINUTES:
			
			// debug("peek CIA_TIME_OF_DAY_MINUTES\n");
			result = BinaryToBCD(tod.getTodMinutes());
			break;
			
		case CIA_TIME_OF_DAY_HOURS:

			// debug("peek CIA_TIME_OF_DAY_HOURS\n");
			tod.freeze();
			result = (tod.getTodHours() & 0x80) /* AM/PM */ | BinaryToBCD(tod.getTodHours() & 0x1F);
			break;
			
		case CIA_SERIAL_IO_BUFFER:
			
			// debug("peek CIA_SERIAL_IO_BUFFER\n");			
			result = 0x00;
			break;
			
		case CIA_INTERRUPT_CONTROL:
		
			result = ICR;
			
			// get status of the Int line into bit 7 and draw Int high
			if (INT == 0) {
				result |= 0x80;
				INT = 1;
				clearInterruptLine(); // really?
			}
			
			// discard pending interrupts
			delay &= ~(Interrupt0 | Interrupt1);
			
			// set all events to 0
			ICR = 0;
			break;

		case CIA_CONTROL_REG_A:

			result = (uint8_t)(CRA & ~0x10); // Bit 4 is always 0 when read
			break;
			
		case CIA_CONTROL_REG_B:
			
			result = (uint8_t)(CRB & ~0x10); // Bit 4 is always 0 when read
			break;
			
		default:
			result = 0;
			panic("Unknown CIA address %04X\n", addr);
			break;
	}
	
	return result;
}

void CIA::poke(uint16_t addr, uint8_t value)
{
	switch(addr) {
			
		case CIA_TIMER_A_LOW:
			
			setLatchALo(value);
			
			// If timer A is currently in LOAD state, this value goes directly into the counter
			if (delay & LoadA2) {
				setCounterALo(value);
			}
			return;
			
		case CIA_TIMER_A_HIGH:
						
			setLatchAHi(value);		
			
			// load counter if timer is stopped
			if ((CRA & 0x01) == 0) {
				delay |= LoadA0;
			}
			
			// If timer A is currently in LOAD state, this value goes directly into the counter
			if (delay & LoadA2) {
				setCounterAHi(value);
			}
			return;
			
		case CIA_TIMER_B_LOW:  

			setLatchBLo(value);

			// If timer B is currently in LOAD state, this value goes directly into the counter
			if (delay & LoadB2) {
				setCounterBLo(value);
			}			
			return;
			
		case CIA_TIMER_B_HIGH: 
			
			setLatchBHi(value);
			// load counter if timer is stopped
			if ((CRB & 0x01) == 0) {
				delay |= LoadB0;
			}
			
			// If timer B is currently in LOAD state, this value goes directly into the counter
			if (delay & LoadB2) {
				setCounterBHi(value);
			}						
			return;
			
		case CIA_TIME_OF_DAY_SEC_FRAC:
			// debug("poke CIA_TIME_OF_DAY_SEC_FRAC: %02X\n", value);
			if (value & 0x80) {
				tod.setAlarmTenth(BCDToBinary(value & 0x0F));
			} else { 
				tod.setTodTenth(BCDToBinary(value & 0x0F));
				tod.cont();
			}
			return;
			
		case CIA_TIME_OF_DAY_SECONDS:
			// debug("poke CIA_TIME_OF_DAY_SECONDS: %02X\n", value);
			if (value & 0x80)
				tod.setAlarmSeconds(BCDToBinary(value & 0x7F));
			else 
				tod.setTodSeconds(BCDToBinary(value & 0x7F));
			return;
			
		case CIA_TIME_OF_DAY_MINUTES:
			// debug("poke CIA_TIME_OF_DAY_MINUTES: %02X\n", value);
			if (value & 0x80)
				tod.setAlarmMinutes(BCDToBinary(value & 0x7F));
			else 
				tod.setTodMinutes(BCDToBinary(value & 0x7F));
			return;
			
		case CIA_TIME_OF_DAY_HOURS:
			// debug("poke CIA_TIME_OF_DAY_HOURS: %02X\n", value);
			if (value & 0x80) {
				tod.setAlarmHours((value & 0x80) /* AM/PM */ | BCDToBinary(value & 0x1F));
			} else {
				tod.setTodHours((value & 0x80) /* AM/PM */ | BCDToBinary(value & 0x1F));
				tod.stop();
			}
			return;
			
		case CIA_SERIAL_IO_BUFFER:
			// Serial I/O communication is not (yet) implemented
			//triggerInterrupt(0x08);
			// debug("poke CIA_SERIAL_IO_BUFFER: %0x2X\n", value);
			return;
			
		case CIA_INTERRUPT_CONTROL:
			
			//if ((value & 0x84) == 0x84)
			//	debug("SETTING TIME OF DAY ALARM (%02X)\n", value);
			
			// bit 7 means set (1) or clear (0) the other bits
			if ((value & 0x80) != 0) {
				IMR |= (value & 0x1F);
			} else {
				IMR &= ~(value & 0x1F);
			}
			
			// raise an interrupt in the next cycle if condition matches
			if ((IMR & ICR) != 0) {
				if (INT) {
					delay |= Interrupt0;
				}
			}
			return;
			
		case CIA_CONTROL_REG_A:
		{
			// 
			// Adapted from PC64Win by Wolfgang Lorenz
			//
						
			// set clock in o2 mode // todo cnt
			if ((value & 0x21) == 0x01) {
				delay |= CountA1 | CountA0;
				feed |= CountA0;
			} else {
				delay &= ~(CountA1 | CountA0);
				feed &= ~CountA0;
			}
			
			// set one shot mode
			if ((value & 0x08) != 0) {
				feed |= OneShotA0;
			} else {
				feed &= ~OneShotA0;
			}
			
			// set force load
			if ((value & 0x10) != 0) {
				delay |= LoadA0;
			}
			
			// set toggle high on rising edge of Start
			if ((value & 0x01) != 0 && (CRA & 0x01) == 0) {
				PB67Toggle |= 0x40;
			}
			
			// timer A output to PB6
			if ((value & 0x02) == 0) {
				PB67TimerMode &= ~0x40;
			} else {
				PB67TimerMode |= 0x40;
				if ((value & 0x04) == 0) {
					if ((delay & PB7Low1) == 0) {
						PB67TimerOut &= ~0x40;
					} else {
						PB67TimerOut |= 0x40;
					}
				} else {
					PB67TimerOut = (PB67TimerOut & ~0x40) | (PB67Toggle & 0x40);
				}
			}
			
			// write PB67 
			PB = ((PBLatch | ~DDRB) & ~PB67TimerMode) | (PB67TimerOut & PB67TimerMode);
			
			// set the register
			CRA = value;
			
			return;
		}
			
		case CIA_CONTROL_REG_B:
		{
			// 
			// Adapted from PC64Win by Wolfgang Lorenz
			//
						
			// set clock in o2 mode // todo cnt
			if ((value & 0x61) == 0x01) {
				delay |= CountB1 | CountB0;
				feed |= CountB0;
			} else {
				delay &= ~(CountB1 | CountB0);
				feed &= ~CountB0;
			}
			
			// set one shot mode
			if ((value & 0x08) != 0) {
				feed |= OneShotB0;
			} else {
				feed &= ~OneShotB0;
			}
			
			// set force load
			if ((value & 0x10) != 0) {
				delay |= LoadB0;
			}
			
			// set toggle high on rising edge of Start
			if ((value & 0x01) != 0 && (CRB & 0x01) == 0) {
				PB67Toggle |= 0x80;
			}
			
			// timer B output to PB7
			if ((value & 0x02) == 0) {
				PB67TimerMode &= ~0x80;
			} else {
				PB67TimerMode |= 0x80;
				if ((value & 0x04) == 0) {
					if ((delay & PB7Low1) == 0) {
						PB67TimerOut &= ~0x80;
					} else {
						PB67TimerOut |= 0x80;
					}
				} else {
					PB67TimerOut = (PB67TimerOut & ~0x80) | (PB67Toggle & 0x80);
				}
			}
			
			// write PB67
			PB = ((PBLatch | ~DDRB) & ~PB67TimerMode) | (PB67TimerOut & PB67TimerMode);
			
			// set the register
			CRB = value;
			
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
		// Set interrupt source
		ICR |= 0x04; 
		
		// Trigger interrupt, if enabled
		if (IMR & 0x04) {
			// The uppermost bit indicates that an interrupt occured
			// printf("Triggering CIA interrupt (source = %02X) at cycle %d\n", source, (int)cpu->getCycles());
			ICR |= 0x80;
			raiseInterruptLine();
		}
	}
}

void CIA::dumpTrace()
{
	const char *indent = "                                                                      ";

	if (!tracingEnabled()) 
		return;
	
	debug(1, "%sICR: %02X IMR: %02X ", indent, ICR, IMR);
	debug(1, "%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s\n",
			delay & CountA0 ? "CntA0 " : "",
			delay & CountA1 ? "CntA1 " : "",
			delay & CountA2 ? "CntA2 " : "",
			delay & CountA3 ? "CntA3 " : "",
			delay & CountB0 ? "CntB0 " : "",
			delay & CountB1 ? "CntB1 " : "",
			delay & CountB2 ? "CntB2 " : "",
			delay & CountB3 ? "CntB3 " : "",
			delay & LoadA0 ? "LdA0 " : "",
			delay & LoadA1 ? "LdA1 " : "",
			delay & LoadA2 ? "LdA2 " : "",
			delay & LoadB0 ? "LdB0 " : "",
			delay & LoadB1 ? "LdB1 " : "",
			delay & LoadB1 ? "LdB2 " : "",
			delay & PB6Low0 ? "PB6Lo0 " : "",
			delay & PB6Low1 ? "PB6Lo1 " : "",
			delay & PB7Low0 ? "PB7Lo0 " : "",
			delay & PB7Low1 ? "PB7Lo1 " : "",
			delay & Interrupt0 ? "Int0 " : "",
			delay & Interrupt1 ? "Int1 " : "",
			delay & OneShotA0 ? "1ShotA0 " : "",
			delay & OneShotB0 ? "1ShotB0 " : "");

	debug(1, "%sA: %04X (%04X) PA: %02X (%02X) DDRA: %02X CRA: %02X\n",
		  indent, counterA, latchA, PA, PALatch, DDRA, CRA);
	debug(1, "%sB: %04X (%04X) PB: %02X (%02X) DDRB: %02X CRB: %02X\n",
		  indent, counterB, latchB, PB, PBLatch, DDRB, CRB);
}

void CIA::dumpState()
{
	debug(1, "              Counter A : %02X\n", getCounterA());
	debug(1, "                Latch A : %02X\n", getLatchA());
	debug(1, "            Data port A : %02X\n", getDataPortA());
	debug(1, "  Data port direction A : %02X\n", getDataPortDirectionA());
	debug(1, "     Control register A : %02X\n", getControlRegA());
	debug(1, "     Timer A interrupts : %s\n", isInterruptEnabledA() ? "enabled" : "disabled");	
	debug(1, "\n");
	debug(1, "              Counter B : %02X\n", getCounterB());
	debug(1, "                Latch B : %02X\n", getLatchB());
	debug(1, "            Data port B : %02X\n", getDataPortB());
	debug(1, "  Data port direction B : %02X\n", getDataPortDirectionB());
	debug(1, "     Control register B : %02X\n", getControlRegB());
	debug(1, "     Timer B interrupts : %s\n", isInterruptEnabledB() ? "enabled" : "disabled");	
	debug(1, "\n");
	debug(1, "  Interrupt control reg : %02X\n", ICR);
	debug(1, "     Interrupt mask reg : %02X\n", IMR);
	debug(1, "\n");	
	tod.dumpState();
}

void CIA::executeOneCycle()
{
#if 0
	if (cpu->c64->event2 && this == cpu->c64->cia1)
	{
		dumpTrace();	
	}
#endif
	
	//
	// Layout of timer (A and B)
	//

    // Source: "A Software Model of the CIA6526" by Wolfgang Lorenz
	//
    //                              Phi2            Phi2                  Phi2
	//                               |               |                     |
	// timerA      -----       ------v------   ------v------     ----------v-----------
	// input  ---->| & |------>| dwDelay & |-X-| dwDelay & |---->| decrement counter  |
	//         --->|   |       |  CountA2  | | |  CountA3  |     |        (1)         |
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
	//                     |                   --->|   |         |        (4)         |
	//                    -----                |   -----         ----------------------
	//                    |>=1|                |
	//                    |   |                |       Phi2
	//                    -----                |        |
	//                     ^ ^                 |  ------v------      ----------------
	//                     | | (3)             ---| dwDelay & |<-----| bcRA & 0x10  |
	//                     | -----------------    |  LoadA1   |      | force load   |
	//                     |       Phi2      |    -------------      ----------------
    //                     |        |        |                              ^ Clr
	// -----------------   |  ------v------  |                              |
	// | bCRA & 0x08   |   |  | dwDelay & |  |                             Phi2
	// | one shot      |---X->| oneShotA0 |---
	// -----------------      -------------

				
	// Timer A

	// Decrement counter

	if (delay & CountA3)
		counterA--; // (1)
	
	// Check underflow condition
	bool timerAOutput = (counterA == 0 && (delay & CountA2)); // (2)
	
	if (timerAOutput) {
		
		// Stop timer in one shot mode
		if ((delay | feed) & OneShotA0) { // (3)
			CRA &= ~0x01;
			delay &= ~(CountA2 | CountA1 | CountA0);
			feed &= ~CountA0;
		}
		
		// Timer A output to timer B in cascade mode
		if ((CRB & 0x61) == 0x41 || (CRB & 0x61) == 0x61 && CNT) {
			delay |= CountB1;
		}
		delay |= LoadA1;
	}
	
	// Load counter
	if (delay & LoadA1) // (4)
		reloadTimerA(); 
	
	// Timer B
	
	// Decrement counter
	if (delay & CountB3) {
		counterB--; // (1)
	}

	// Check underflow condition
	bool timerBOutput = (counterB == 0 && (delay & CountB2)); // (2)
	
	if (timerBOutput) {
						
		// Stop timer in one shot mode
		if ((delay | feed) & OneShotB0) { // (3)
			CRB &= ~0x01;
			delay &= ~(CountB2 | CountB1 | CountB0);
			feed &= ~CountB0;
		}
		delay |= LoadB1;
	}
	
	// Load counter
	if (delay & LoadB1) // (4)
		reloadTimerB();
		
	
	//
	// Timer output to PB6 (timer A) and PB7 (timer B)
    // 
	
	// Source: "A Software Model of the CIA6526" by Wolfgang Lorenz
	//
	//                       (7)            -----------------
	//          --------------------------->| 0x00 (pulse)  |
	//          |                           |               |       ----------------
	//          |                           | bCRA & 0x04   |------>| 0x02 (timer) |
	// timerA   |  Flip --------------- (8) | timer mode    |       |              |
	// output  -X------>| bPB67Toggle |---->| 0x04 (toggle) |       | bCRA & 0x02  | 
	//              (5) |  ^ 0x04     |     |     (6)       |       | output mode  |----> PB6 output
	//                  ---------------     -----------------       |              |
	//                        ^ Set                                 | 0x00 (port)  |
	//                        |                                ---->|              |
	// ----------------- 0->1 |             -----------------  |    ----------------
	// | bCRA & 0x01   |-------             | port B bit 6  |---  
	// | timer A start |                    |    output     |
	// -----------------                    -----------------

	// Timer A output to PB6
	
	if (timerAOutput) {
		
		PB67Toggle ^= 0x40; // (5) toggle underflow counter bit
		
		if (CRA & 0x02) { // (6)

			if ((CRA & 0x04) == 0) { 
				// (7) set PB6 high for one clock cycle
				PB67TimerOut |= 0x40;
				delay |= PB6Low0;
				delay &= ~PB6Low1;
			} else { 
				// (8) toggle PB6 (copy bit 6 from PB67Toggle)
				PB67TimerOut = (PB67TimerOut & 0xBF) | (PB67Toggle & 0x40);
			}
		}
	}

	// Timer B output to PB7
	
	if (timerBOutput) {
		
		PB67Toggle ^= 0x80; // (5) toggle underflow counter bit
	
		if (CRB & 0x02) { // (6)
		
			if ((CRB & 0x04) == 0) {
				// (7) set PB7 high for one clock cycle
				PB67TimerOut |= 0x80;
				delay |= PB7Low0;
				delay &= ~PB7Low1;				
			} else {
				// (8) toggle PB7 (copy bit 7 from PB67Toggle)
				PB67TimerOut = (PB67TimerOut & 0x7F) | (PB67Toggle & 0x80);
			}
		}
	}
	
	// Set PB67 back to low
	if (delay & PB6Low1) 
		PB67TimerOut &= ~0x40;

	if (delay & PB7Low1)
		PB67TimerOut &= ~0x80;

	
	// Write new PB 
	PB = ((PBLatch | ~DDRB) & ~PB67TimerMode) | (PB67TimerOut & PB67TimerMode);
	
	
	//
	// Interrupt logic
    //
	
	// Source: "A Software Model of the CIA6526" by Wolfgang Lorenz
	//
	//                      ----------
	//                      | bIMR & |----
	//                      |  0x01  |   |    -----
	//                      ----------   ---->| & |----
	// timerA       (9) Set ----------   ---->|   |   |
	// output  ------------>| bICR & |   |    -----   |
	//           ---------->|  0x01  |----            |  -----
	//           |      Clr ----------                -->|>=1|---
	//           |          ----------                -->|   |  |
	//           |          | bIMR & |----            |  -----  |
	//           |          |  0x02  |   |    -----   |         |
	//           |          ----------   ---->| & |----         |
	// timerB    | (10) Set ----------   ---->|   |             |
	// output  --|--------->| bICR & |   |    -----             |
	//           X--------->|  0x01  |----                      |
	//           |      Clr ----------       	                |
	// read      |                                              |
	// ICR ------X-------------X----------------                |
	//                         |               |                |
	//                         v Clr           v Clr            |
	//           ------    ----------    ----------------       | (11)
	// Int    <--| -1 |<---| bICR & |<---|   dwDelay &  |<-------
	// ouptput   |    |    |  0x80  |Set |  Interrupt1  |     
	//           ------    ----------    -------^--------   	
	//                                          |
	//                                         Phi2
	
	// Set interrupt register and interrupt line
	if (delay & Interrupt1) {
		INT = 0;
		raiseInterruptLine();
	}
	
	if (timerAOutput) // (9)
		ICR |= 0x01;
	
	if (timerBOutput) // (10)
		ICR |= 0x02;

	if ((timerAOutput && (IMR & 0x01)) || (timerBOutput && (IMR & 0x02))) // (11)
		delay |= Interrupt0;
	

	// move delay flags left and feed in new bits
	delay = (delay << 1) & DelayMask | feed;
}


// -----------------------------------------------------------------------------------------
// Complex Interface Adapter 1
// -----------------------------------------------------------------------------------------

CIA1::CIA1()
{
	debug(2, "  Creating CIA1 at address %p...\n", this);

	name = "CIA1";
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
	//debug(1, "CIA 1:\n");
	//debug(1, "------\n\n");
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
			break;
			
		case CIA_DATA_PORT_B:
		{
			uint8_t bitmask = CIA1::peek(CIA_DATA_PORT_A);
			uint8_t keyboardBits = keyboard->getRowValues(bitmask); 
			
			if ( joy[1] != NULL )
				pollJoystick( joy[1], 2 );
			
			result = PB;
			
			// Check joystick movement
			result &= joystick[1];
			
			// Check for pressed keys
			result &= keyboardBits;
			break;
		}
			
		default:
			result = CIA::peek(addr);	
			break;
	}
	
	// log("PEEKING %04X: %02X\n", 0xDC00 + addr, result);
	return result;
}

void 
CIA1::poke(uint16_t addr, uint8_t value)
{
	assert(addr <= CIA1_END_ADDR - CIA1_START_ADDR);
	
	// log("Poking %02X to %04X\n", value, 0xDC00 + addr);
	
	// The following registers need special handling	
	switch(addr) {
			
		case CIA_DATA_PORT_A: 
			
			PALatch = value;
			PA = PALatch | ~DDRA;
			return;
			
		case CIA_DATA_DIRECTION_A:

			DDRA = value;
			PA = PALatch | ~DDRA;
			return;
			
		case CIA_DATA_PORT_B:
			
			PBLatch = value;
			PB = ((PBLatch | ~DDRB) & ~PB67TimerMode) | (PB67TimerOut & PB67TimerMode);
			// oldPB = PB;
			return;
			
		case CIA_DATA_DIRECTION_B:

			DDRB = value;
			PB = ((PBLatch | ~DDRB) & ~PB67TimerMode) | (PB67TimerOut & PB67TimerMode);
			// oldPB = PB;
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

	name = "CIA2";
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
	// debug(1, "CIA 2:\n");
	// debug(1, "------\n\n");
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
			
			return result;
						
		case CIA_DATA_PORT_B:
			
			result = PB;		
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
			
			PALatch = value;
			PA = PALatch | ~DDRA;

			// Bits 0 and 1 determine the memory bank seen the VIC
			vic->setMemoryBankAddr((~PA & 0x03) << 14);	

			// Bits 3 to 5 of PA are connected to the IEC bus
			iec->updateCiaPins(PALatch, DDRA);
			return;
			
		case CIA_DATA_DIRECTION_A:
			
			DDRA = value;
			PA = PALatch | ~DDRA;
			
			// Bits 0 and 1 determine the memory bank seen the VIC
			vic->setMemoryBankAddr((~PA & 0x03) << 14);	
			
			// Bits 3 to 5 of PA are connected to the IEC bus
			iec->updateCiaPins(PALatch, DDRA);
			return;
			
		case CIA_DATA_PORT_B:
			
			PBLatch = value;
			PB = ((PBLatch | ~DDRB) & ~PB67TimerMode) | (PB67TimerOut & PB67TimerMode);
			// oldPB = PB;
			return;

		case CIA_DATA_DIRECTION_B:
			
			DDRB = value;
			PB = ((PBLatch | ~DDRB) & ~PB67TimerMode) | (PB67TimerOut & PB67TimerMode);
			// oldPB = PB;
			return;
			
		default:
			CIA::poke(addr, value);
	}
}
			
