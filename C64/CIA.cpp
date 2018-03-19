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
	setDescription("CIA");
    
    // Register sub components
    VirtualComponent *subcomponents[] = { &tod, NULL };
    registerSubComponents(subcomponents, sizeof(subcomponents));

    // Register snapshot items
    SnapshotItem items[] = {
        
        { &delay,           sizeof(delay),          CLEAR_ON_RESET },
        { &feed,            sizeof(feed),           CLEAR_ON_RESET },
        { &CRA,             sizeof(CRA),            CLEAR_ON_RESET },
        { &CRB,             sizeof(CRB),            CLEAR_ON_RESET },
        { &ICR,             sizeof(ICR),            CLEAR_ON_RESET },
        { &IMR,             sizeof(IMR),            CLEAR_ON_RESET },
        { &PB67TimerMode,   sizeof(PB67TimerMode),  CLEAR_ON_RESET },
        { &PB67TimerOut,    sizeof(PB67TimerOut),   CLEAR_ON_RESET },
        { &PB67Toggle,      sizeof(PB67Toggle),     CLEAR_ON_RESET },
        { &PALatch,         sizeof(PALatch),        CLEAR_ON_RESET },
        { &PBLatch,         sizeof(PBLatch),        CLEAR_ON_RESET },
        { &DDRA,            sizeof(DDRA),           CLEAR_ON_RESET },
        { &DDRB,            sizeof(DDRB),           CLEAR_ON_RESET },
        { &PA,              sizeof(PA),             CLEAR_ON_RESET },
        { &PB,              sizeof(PB),             CLEAR_ON_RESET },
        { &CNT,             sizeof(CNT),            CLEAR_ON_RESET },
        { &INT,             sizeof(INT),            CLEAR_ON_RESET },
        { &readICR,         sizeof(readICR),        CLEAR_ON_RESET },
        { &counterA,        sizeof(counterA),       CLEAR_ON_RESET },
        { &latchA,          sizeof(latchA),         CLEAR_ON_RESET },
        { &counterB,        sizeof(counterB),       CLEAR_ON_RESET },
        { &latchB,          sizeof(latchB),         CLEAR_ON_RESET },
        { NULL,             0,                      0 }};

    registerSnapshotItems(items, sizeof(items));
}

CIA::~CIA()
{
}

void
CIA::reset()
{
    VirtualComponent::reset();
    
    clearInterruptLine();

    
    // PB67TimerMode = 0x80;
    /*
    PA = PALatch | ~DDRA;
    PB = ((PBLatch | ~DDRB) & ~PB67TimerMode) | (PB67TimerOut & PB67TimerMode);
    */
    
    PA = 0xFF;
    PB = 0xFF;

	CNT = true; // CNT line is high by default
	INT = 1;
	
	latchA = 0xFFFF;
	latchB = 0xFFFF;
}

void
CIA::triggerRisingEdgeOnFlagPin()
{
    // ICR &= ~0x10; // Note: FLAG pin is inverted
}

void
CIA::triggerFallingEdgeOnFlagPin()
{
    ICR |= 0x10; // Note: FLAG pin is inverted
        
    // Trigger interrupt, if enabled
    if (IMR & 0x10) {
        INT = 0;
        ICR |= 0x80;
        raiseInterruptLine();
    }
}

uint8_t
CIA::peek(uint16_t addr)
{
	uint8_t result;
	
	switch(addr) {		
        case 0x02: // CIA_DATA_DIRECTION_A

			result = DDRA;
			break;

        case 0x03: // CIA_DATA_DIRECTION_B

			result = DDRB;
			break;
			
        case 0x04: // CIA_TIMER_A_LOW
			
			result = getCounterALo();
			break;
			
        case 0x05: // CIA_TIMER_A_HIGH
			result = getCounterAHi();
			break;
			
        case 0x06: // CIA_TIMER_B_LOW

			result = getCounterBLo();
			break;
			
        case 0x07: // CIA_TIMER_B_HIGH
			
			result = getCounterBHi();
			break;
			
        case 0x08: // CIA_TIME_OF_DAY_SEC_FRAC
			
			result = tod.getTodTenth();
			break;
		
        case 0x09: // CIA_TIME_OF_DAY_SECONDS
			
			result = tod.getTodSeconds();
			break;
			
        case 0x0A: // CIA_TIME_OF_DAY_MINUTES
			
			result = tod.getTodMinutes();
			break;
			
        case 0x0B: // CIA_TIME_OF_DAY_HOURS

			result = tod.getTodHours();
			break;
			
        case 0x0C: // CIA_SERIAL_IO_BUFFER
			
			result = 0x00;
			break;
			
        case 0x0D: // CIA_INTERRUPT_CONTROL
		
			result = ICR;

			// get status of the Int line into bit 7 and draw Int high
			if (INT == 0) {
				result |= 0x80;
				INT = 1;
				clearInterruptLine(); // really?
			}
			
			// discard pending interrupts
			delay &= ~(Interrupt0 | Interrupt1);

			// Remember read access
			readICR = true;

			// set all events to 0
			ICR = 0;
						
			break;

        case 0x0E: // CIA_CONTROL_REG_A

			result = (uint8_t)(CRA & ~0x10); // Bit 4 is always 0 when read
			break;
			
        case 0x0F: // CIA_CONTROL_REG_B
			
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
			
        case 0x04: // CIA_TIMER_A_LOW
			
			setLatchALo(value);
			
			// If timer A is currently in LOAD state, this value goes directly into the counter
			if (delay & LoadA2) {
				setCounterALo(value);
			}
			return;
			
        case 0x05: // CIA_TIMER_A_HIGH
						
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
			
        case 0x06: // CIA_TIMER_B_LOW

			setLatchBLo(value);

			// If timer B is currently in LOAD state, this value goes directly into the counter
			if (delay & LoadB2) {
				setCounterBLo(value);
			}			
			return;
			
        case 0x07: // CIA_TIMER_B_HIGH
			
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
			
        case 0x08: // CIA_TIME_OF_DAY_SEC_FRAC
            
			if (CRB & 0x80) {
				tod.setAlarmTenth(value);
                checkForTODInterrupt();
			} else { 
				tod.setTodTenth(value);
                // if (tod.tod.oldValue != tod.tod.value)
                    checkForTODInterrupt();
			}
			return;
			
        case 0x09: // CIA_TIME_OF_DAY_SECONDS
            
            if (CRB & 0x80) {
				tod.setAlarmSeconds(value);
                checkForTODInterrupt();
            } else {
				tod.setTodSeconds(value);
                // if (tod.tod.oldValue != tod.tod.value)
                    checkForTODInterrupt();
            }
			return;
			
        case 0x0A: // CIA_TIME_OF_DAY_MINUTES
            
            if (CRB & 0x80) {
				tod.setAlarmMinutes(value);
                checkForTODInterrupt();
            } else {
				tod.setTodMinutes(value);
                // if (tod.tod.oldValue != tod.tod.value)
                    checkForTODInterrupt();
            }
			return;
			
        case 0x0B: // CIA_TIME_OF_DAY_HOURS
			
			if (CRB & 0x80) {
				tod.setAlarmHours(value);
                checkForTODInterrupt();
			} else {
				// Note: A real C64 shows strange behaviour when writing 0x12 or 0x92 
				// into this register. In this case, the AM/PM flag is inverted
				if ((value & 0x1F) == 0x12)
					value ^= 0x80;
				tod.setTodHours(value);
                // if (tod.tod.oldValue != tod.tod.value)
                    checkForTODInterrupt();
			}
			return;
			
        case 0x0C: // CIA_SERIAL_IO_BUFFER
            
			// Serial I/O communication is not (yet) implemented
			//triggerInterrupt(0x08);
			// debug("poke CIA_SERIAL_IO_BUFFER: %0x2X\n", value);
			return;
			
        case 0x0D: // CIA_INTERRUPT_CONTROL
			
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
			
        case 0x0E: // CIA_CONTROL_REG_A
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
			
            // Set real time clock frequency in Hz
            tod.hz = (value & 0x80) ? 5 /* 50 Hz */ : 6 /* 60 Hz */;
            
			// set the register
			CRA = value;
			
			return;
		}
			
        case 0x0F: // CIA_CONTROL_REG_B
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
    tod.increment();
    checkForTODInterrupt();
}

void
CIA::checkForTODInterrupt()
{
    if (tod.alarming()) {
        
        // Set interrupt source
        ICR |= 0x04;
        
        // Trigger interrupt, if enabled
        if (IMR & 0x04) {
            // The uppermost bit indicates that an interrupt occured
            ICR |= 0x80;
            raiseInterruptLineTOD();
        }
    }  else {
        clearInterruptLineTOD();
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
	msg("              Counter A : %02X\n", getCounterA());
	msg("                Latch A : %02X\n", getLatchA());
	msg("            Data port A : %02X\n", getDataPortA());
	msg("  Data port direction A : %02X\n", getDataPortDirectionA());
	msg("     Control register A : %02X\n", getControlRegA());
	msg("     Timer A interrupts : %s\n", isInterruptEnabledA() ? "enabled" : "disabled");	
	msg("\n");
	msg("              Counter B : %02X\n", getCounterB());
	msg("                Latch B : %02X\n", getLatchB());
	msg("            Data port B : %02X\n", getDataPortB());
	msg("  Data port direction B : %02X\n", getDataPortDirectionB());
	msg("     Control register B : %02X\n", getControlRegB());
	msg("     Timer B interrupts : %s\n", isInterruptEnabledB() ? "enabled" : "disabled");	
	msg("\n");
	msg("  Interrupt control reg : %02X\n", ICR);
	msg("     Interrupt mask reg : %02X\n", IMR);
	msg("\n");	
	tod.dumpState();
}

void CIA::executeOneCycle()
{	
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
		if ((CRB & 0x61) == 0x41 || ((CRB & 0x61) == 0x61 && CNT)) {
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
        // debug("Counter B down to %04X \n", counterB);
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
	
	if (timerAOutput) { // (9)
		// On a real C64, there is a race condition here. If ICR is currently read, 
		// the read access occurs *before* timer A sets bit 1. Hence, bit 1 always shows up.
		ICR |= 0x01;
	}
	
	if (timerBOutput && !readICR) { // (10)
		// On a real C64, there is a race condition here. If ICR is currently read, 
		// the read access occurs *after* timer B sets bit 2. Hence, bit 2 won't show up.
		ICR |= 0x02;
	}
	
	if ((timerAOutput && (IMR & 0x01)) || (timerBOutput && (IMR & 0x02))) // (11)
		delay |= Interrupt0;
	
	readICR = false;

	// move delay flags left and feed in new bits
	delay = ((delay << 1) & DelayMask) | feed;
}


// -----------------------------------------------------------------------------------------
// Complex Interface Adapter 1
// -----------------------------------------------------------------------------------------

CIA1::CIA1()
{
    setDescription("CIA1");
	debug(3, "  Creating CIA1 at address %p...\n", this);
}

CIA1::~CIA1()
{
	debug(3, "  Releasing CIA1\n");
}

void 
CIA1::reset()
{
    joystick[0] = 0xff;
    joystick[1] = 0xff;
	CIA::reset();
}

void 
CIA1::dumpState()
{
	msg("CIA 1:\n");
	msg("------\n\n");
	CIA::dumpState();
}

void 
CIA1::raiseInterruptLine()
{
	c64->cpu.setIRQLineCIA();
}

void
CIA1::raiseInterruptLineTOD()
{
    c64->cpu.setIRQLineTOD();
}

void 
CIA1::clearInterruptLine()
{
	c64->cpu.clearIRQLineCIA();
}

void
CIA1::clearInterruptLineTOD()
{
    c64->cpu.clearIRQLineTOD();
}

void 
CIA1::pollJoystick(Joystick *joy, int joyDevNo)
{
    assert (joy != NULL);
    
	// up/down
	// set the down bit: 2, 2 and clear up bit: 2, 1		
	// Remember: clearJoystickBits(x, y) means pressed
    //           setJoystickBits( x, y ) means released
    
	if(joy->isPulledUp()) {
		clearJoystickBits(joyDevNo, 1);
		setJoystickBits(joyDevNo, 2);
	} else if(joy->isPulledDown()) {
		clearJoystickBits(joyDevNo, 2);
		setJoystickBits(joyDevNo, 1);
	} else {
		setJoystickBits(joyDevNo, 1);			
		setJoystickBits(joyDevNo, 2);
	}
	
	// left/right
	if(joy->isPulledLeft()) {
		clearJoystickBits(joyDevNo, 4);
		setJoystickBits(joyDevNo, 8);
    } else if(joy->isPulledRight()) {
		clearJoystickBits(joyDevNo, 8);			
		setJoystickBits(joyDevNo, 4);
	} else {
		setJoystickBits(joyDevNo, 4);			
		setJoystickBits(joyDevNo, 8);
	}
	
	// fire
	if(joy->getButton()) {
		clearJoystickBits(joyDevNo, 16);
	} else {
		setJoystickBits(joyDevNo, 16);
	}
}

uint8_t 
CIA1::peek(uint16_t addr)
{
	uint8_t result;
    uint8_t rows, columnBits, columns, rowBits;
    
	assert(addr <= CIA1_END_ADDR - CIA1_START_ADDR);
	
	switch(addr) {		
        case 0x00: // CIA_DATA_PORT_A
				
            result = PA;
            
            pollJoystick(&c64->joystickA, 1);
            pollJoystick(&c64->joystickB, 2);

            rows = PB & joystick[0];
            columnBits = c64->keyboard.getColumnValues(rows);
            
			// Check joystick movement
			result &= joystick[1];
            
            // Check for pressed keys
            result &= columnBits;
            
			break;
			
        case 0x01: // CIA_DATA_PORT_B
		{
            result = PB;
            
            columns = PA & joystick[1];
			rowBits = c64->keyboard.getRowValues(columns);
			
            pollJoystick(&c64->joystickA, 1);
			pollJoystick(&c64->joystickB, 2);
            
            // Check joystick movement
            result &= joystick[0];
			
			// Check for pressed keys
			result &= rowBits;

            // printf("PC: %04X bitmask = %02X keyboardBits = %02X PALatch = %04X PBLatch = %04X DDRA = %02X DDRB = %02X PB67TimerMode = %02X PB67TimerOut = %02X PA = %02X PB = %02X result: %02X\n", c64->cpu.getPC_at_cycle_0(), bitmask, keyboardBits, PALatch, PBLatch, DDRA, DDRB, PB67TimerMode, PB67TimerOut, PA, PB, result);

            break;
		}
			
		default:
			result = CIA::peek(addr);	
			break;
	}
	
	return result;
}

void 
CIA1::poke(uint16_t addr, uint8_t value)
{
    uint8_t PBold;
    
	assert(addr <= CIA1_END_ADDR - CIA1_START_ADDR);
	
	// The following registers need special handling	
	switch(addr) {
			
        case 0x00: // CIA_DATA_PORT_A
			
			PALatch = value;
			PA = PALatch | ~DDRA;
			return;
			
        case 0x01: // CIA_DATA_PORT_B
			
            PBold = PB;
            
			PBLatch = value;
			PB = ((PBLatch | ~DDRB) & ~PB67TimerMode) | (PB67TimerOut & PB67TimerMode);
            
            if ((PBold & 0x10) != (PB & 0x10)) { // edge on lightpen bit?
                c64->vic.triggerLightPenInterrupt();
            }
			return;
			
        case 0x02: // CIA_DATA_DIRECTION_A
            
            DDRA = value;
            PA = PALatch | ~DDRA;
            return;
            
        case 0x03: // CIA_DATA_DIRECTION_B

            PBold = PB;

            // printf("CIA1::poke Setting DDRB = %02X\n", value);
            
			DDRB = value;
			PB = ((PBLatch | ~DDRB) & ~PB67TimerMode) | (PB67TimerOut & PB67TimerMode);

            if ((PBold & 0x10) != (PB & 0x10)) { // edge on lightpen bit?
                c64->vic.triggerLightPenInterrupt();
            }
            
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
    setDescription("CIA2");
	debug(3, "  Creating CIA2 at address %p...\n", this);
}

CIA2::~CIA2()
{
	debug(3, "  Releasing CIA2...\n");
}

void CIA2::reset()
{
	CIA::reset();
}

void 
CIA2::dumpState()
{
	msg("CIA 2:\n");
	msg("------\n\n");
	CIA::dumpState();
}

void 
CIA2::raiseInterruptLine()
{
	c64->cpu.setNMILineCIA();
}

void
CIA2::raiseInterruptLineTOD()
{
    c64->cpu.setNMILineTOD();
}

void 
CIA2::clearInterruptLine()
{
	c64->cpu.clearNMILineCIA();
}

void
CIA2::clearInterruptLineTOD()
{
    c64->cpu.clearNMILineTOD();
}

uint8_t 
CIA2::peek(uint16_t addr)
{
	uint8_t result;
	
	assert(addr <= CIA_END_ADDR - CIA_START_ADDR);
	
	switch(addr) {
        case 0x00: // CIA_DATA_PORT_A
			
			result = PA;
			
			// The two upper bits are connected to the clock line and the data line
			result &= 0x3F;
			result |= (c64->iec.getClockLine() ? 0x40 : 0x00);
			result |= (c64->iec.getDataLine() ? 0x80 : 0x00);

			// The external port lines can pull down any bit, even if it configured as output.
			// Note that bits 0 and 1 are not connected to the bus and determine the memory bank seen by the VIC chip
			// result &= (portLinesB | 0x03);
			
			return result;
						
        case 0x01: // CIA_DATA_PORT_B
			
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
        case 0x00: // CIA_DATA_PORT_A
			
			PALatch = value;
			PA = PALatch | ~DDRA;

			// Bits 0 and 1 determine the memory bank seen the VIC
			c64->vic.setMemoryBankAddr((~PA & 0x03) << 14);

			// Bits 3 to 5 of PA are connected to the IEC bus
			c64->iec.updateCiaPins(PALatch, DDRA);
			return;
			
        case 0x01: // CIA_DATA_PORT_B
			
			PBLatch = value;
			PB = ((PBLatch | ~DDRB) & ~PB67TimerMode) | (PB67TimerOut & PB67TimerMode);
			// oldPB = PB;
			return;

        case 0x02: // CIA_DATA_DIRECTION_A
            
            DDRA = value;
            PA = PALatch | ~DDRA;
            
            // Bits 0 and 1 determine the memory bank seen the VIC
            c64->vic.setMemoryBankAddr((~PA & 0x03) << 14);
            
            // Bits 3 to 5 of PA are connected to the IEC bus
            c64->iec.updateCiaPins(PALatch, DDRA);
            return;
            
        case 0x03: // CIA_DATA_DIRECTION_B
			
			DDRB = value;
			PB = ((PBLatch | ~DDRB) & ~PB67TimerMode) | (PB67TimerOut & PB67TimerMode);
			// oldPB = PB;
			return;
			
		default:
			CIA::poke(addr, value);
	}
}
			
