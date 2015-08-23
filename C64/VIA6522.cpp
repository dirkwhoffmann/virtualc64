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


// -----------------------------------------------------------------------------------------------
//                                   VIA 6522 (Commons)
// -----------------------------------------------------------------------------------------------


VIA6522::VIA6522()
{
	name = "VIA";
}

VIA6522::~VIA6522()
{
}
	
void VIA6522::reset(C64 *c64)
{
    // Establish bindings
    this->c64 = c64;
    floppy = c64->floppy;
    
    // Reset state
	ddra = 0;
	ddrb = 0;
	ora = 0;
	orb = 0;
	ira = 0;
	irb = 0;
    t1 = 0;
    t2 = 0;
    t1_latch_lo = 0;
	t1_latch_hi = 0;
    t2_latch_lo = 0;
		
	for (unsigned i = 0; i < sizeof(io); i++)
		io[i] = 0;
}

uint32_t
VIA6522::stateSize()
{
    return 13 + sizeof(io);
}

void VIA6522::loadFromBuffer(uint8_t **buffer)
{
    uint8_t *old = *buffer;
    
	ddra = read8(buffer);
	ddrb = read8(buffer);
	ora = read8(buffer);
	orb = read8(buffer);
	ira = read8(buffer);
	irb = read8(buffer);
    t1 = read16(buffer);
    t2 = read16(buffer);
    t1_latch_lo = read8(buffer);
	t1_latch_hi = read8(buffer);
	t2_latch_lo = read8(buffer);
	
	for (unsigned i = 0; i < sizeof(io); i++)
		io[i] = read8(buffer);

    debug(2, "  VIA6522 state loaded (%d bytes)\n", *buffer - old);
    assert(*buffer - old == stateSize());
}

void VIA6522::saveToBuffer(uint8_t **buffer)
{
    uint8_t *old = *buffer;
    
	write8(buffer, ddra);
	write8(buffer, ddrb);
	write8(buffer, ora);
	write8(buffer, orb);
	write8(buffer, ira);
	write8(buffer, irb);
    write16(buffer, t1);
    write16(buffer, t2);
	write8(buffer, t1_latch_lo);
	write8(buffer, t1_latch_hi);
	write8(buffer, t2_latch_lo);
	
	for (unsigned i = 0; i < sizeof(io); i++)
		write8(buffer, io[i]);

    debug(4, "  VIA6522 state saved (%d bytes)\n", *buffer - old);
    assert(*buffer - old == stateSize());
}

void 
VIA6522::dumpState()
{
	msg("VIA:\n");
	msg("----\n\n");
	msg("          Input register (IRA) : %02X\n", ira);
	msg("          Input register (IRB) : %02X\n", irb);
	msg("         Output register (ORA) : %02X\n", ora);
	msg("         Output register (ORB) : %02X\n", orb);
	msg("Data direction register (DDRA) : %02X\n", ddra);
	msg("Data direction register (DDRB) : %02X\n", ddrb);
	msg("              Input latching A : %s\n", inputLatchingEnabledA() ? "enabled" : "disabled");
	msg("              Input latching B : %s\n", inputLatchingEnabledB() ? "enabled" : "disabled");
	msg("                       Timer 1 : %d (latched: %d)\n", t1, LO_HI(t1_latch_lo, t1_latch_hi));
	msg("                       Timer 2 : %d (latched: %d)\n", t2, LO_HI(t2_latch_lo, 0));
	msg("            Timer 1 interrupts : %s\n", timerInterruptEnabled1() ? "enabled" : "disabled");
	msg("            Timer 2 interrupts : %s\n", timerInterruptEnabled2() ? "enabled" : "disabled");
	msg("        Timer 1 interrupt flag : %d\n", (io[0x0D] & 0x40) != 0);
	msg("        Timer 2 interrupt flag : %d\n", (io[0x0D] & 0x20) != 0);
	msg("                     IO memory : ");
	for (int j = 0; j < 16; j ++) {
		msg("%02X ", io[j]);
	}
	msg("\n");
}

// -----------------------------------------------------------------------------------------------
//                                    Execution functions
// -----------------------------------------------------------------------------------------------

void
VIA6522::executeTimer1()
{
    if (t1-- == 1) {
        // Timer 1 time out
        signalTimeOut1();
        if (timerInterruptEnabled1()) {
            floppy->cpu->setIRQLineVIA1();
        }
        
        // Reload timer in free-run mode
        if (freeRunMode1()) {
            t1 = HI_LO(t1_latch_hi, t1_latch_lo);
        }
    }
}

void
VIA6522::executeTimer2()
{
    if (t1-- == 1) {
        // Timer 2 time out
        signalTimeOut2();
        if (timerInterruptEnabled2()) {
            floppy->cpu->setIRQLineVIA2();
        }
    }
}

#if 0
void
VIA6522::executeTimer1()
{
    if (t1-- == 1) {
        // Timer 1 time out
        signalTimeOut1();
        if (timerInterruptEnabled1()) {
            floppy->cpu->setIRQLineVIA1();
        }
        
        // Reload timer in free-run mode
        if (freeRunMode1()) {
            t1 = HI_LO(t1_latch_hi, t1_latch_lo);
        }
    }
}

void
VIA2::executeTimer2()
{
    if (t1-- == 1) {
        // Timer 2 time out
        signalTimeOut2();
        if (timerInterruptEnabled2()) {
            floppy->cpu->setIRQLineVIA2();
        }
    }
}
#endif

// -----------------------------------------------------------------------------------------------
//                                         Peek and poke
// -----------------------------------------------------------------------------------------------

uint8_t 
VIA6522::peek(uint16_t addr)
{
	assert (addr <= 0xF);
		
	switch(addr) {
            
        case 0x0: // Should not reach. Handled individually by VIA1 and VIA2
            
			assert(0);
			break;

        case 0x1: // Should not reach. Handled individually by VIA1 and VIA2
            
			assert(0);
			break;
						
        case 0x2: // DDRB - Data direction register B
            
			return ddrb;

        case 0x3: // DDRA - Data direction register A
            
			return ddra;
			
        case 0x4: // T1 low-order counter
            
            // "8 BITS FROM T1 LOW-ORDER COUNTER TRANSFERRED TO MPU. IN ADDITION T1 INTERRUPT FLAG
            //  IS RESET (BIT 6 IN INTERRUPT FLAG REGISTER)" [F. K.]
            
            clearInterruptFlag_T1();
            floppy->cpu->clearIRQLineVIA1();
            return LO_BYTE(t1);

        case 0x5: // T1 high-order counter
            
            // "8 BITS FROM T1 HIGH-ORDER COUNTER TRANSFERRED TO MPU2" [F. K.]
            
			return HI_BYTE(t1);
          
		case 0x6: // T1 low-order latch
            
            // "8 BITS FROM T1 LOW ORDER-LATCHES TRANSFERRED TO MPU. UNLIKE REG 4 OPERATION,
            //  THIS DOES NOT CAUSE RESET OF T1 INTERRUPT FLAG" [F. K.]
            
			return t1_latch_lo;
            
		case 0x7: // T1 high-order latch
            
            // "8 BITS FROM T1 HIGH-ORDER LATCHES TRANSFERRED TO MPU
			return t1_latch_hi;

        case 0x8: // T2 low-order latch/counter
            
            // "8 BITS FROM T2 LOW-ORDER COUNTER TRANSFERRED TO MPU. T2 INTERRUPT FLAG IS RESET" [F. K.]
            
            clearInterruptFlag_T2();
            floppy->cpu->clearIRQLineVIA2();
			return LO_BYTE(t2);
			
		case 0x9: // T2 high-order counter COUNTER TRANSFERRED TO MPU" [F. K.]
            
            // "8 BITS FROM T2 HIGH-ORDER
			return HI_BYTE(t2);
            
        case 0xA: // Shift register

            clearInterruptFlag_SR();
            warn("peek(0xA): Shift register is not emulated!");
			break;
			
		case 0xB: // Auxiliary control register

            warn("peek(0xB): Shift register is not emulated!");
			break;
		
        case 0xC: // Peripheral control register
            
            // TODO
            break;
			
		//      REG 13 -- INTERRUPT FLAG REGISTER
		// +-+-+-+-+-+-+-+-+
		// |7|6|5|4|3|2|1|0|             SET BY                    CLEARED BY
		// +-+-+-+-+-+-+-+-+    +-----------------------+------------------------------+
		//  | | | | | | | +--CA2| CA2 ACTIVE EDGE       | READ OR WRITE REG 1 (ORA)*   |
		//  | | | | | | |       +-----------------------+------------------------------+
		//  | | | | | | +--CA1--| CA1 ACTIVE EDGE       | READ OR WRITE REG 1 (ORA)    |
		//  | | | | | |         +-----------------------+------------------------------+
		//  | | | | | +SHIFT REG| COMPLETE 8 SHIFTS     | READ OR WRITE SHIFT REG      |
		//  | | | | |           +-----------------------+------------------------------+
		//  | | | | +-CB2-------| CB2 ACTIVE EDGE       | READ OR WRITE ORB*           |
		//  | | | |             +-----------------------+------------------------------+
		//  | | | +-CB1---------| CB1 ACTIVE EDGE       | READ OR WRITE ORB            |
		//  | | |               +-----------------------+------------------------------+
		//  | | +-TIMER 2-------| TIME-OUT OF T2        | READ T2 LOW OR WRITE T2 HIGH |
		//  | |                 +-----------------------+------------------------------+
		//  | +-TIMER 1---------| TIME-OUT OF T1        | READ T1 LOW OR WRITE T1 HIGH |
		//  |                   +-----------------------+------------------------------+
		//  +-IRQ---------------| ANY ENABLED INTERRUPT | CLEAR ALL INTERRUPTS         |
		//                      +-----------------------+------------------------------+
		// 
		//   * IF THE CA2/CB2 CONTROL IN THE PCR IS SELECTED AS "INDEPENDENT"
		//	INTERRUPT INPUT, THEN READING OR WRITING THE OUTPUT REGISTER
		//     ORA/ORB WILL NOT CLEAR THE FLAG BIT. INSTEAD, THE BIT MUST BE
		//     CLEARED BY WRITING INTO THE IFR, AS DESCRIBED PREVIOUSLY.

        case 0xD:
            // printf("Reading 0xD: %02X (%02X)", io[addr] | ((io[addr] & io[0x0E]) ? 0x80 : 0x00), io[addr]);
			return io[addr] | ((io[addr] & io[0x0E]) ? 0x80 : 0x00);
			
		//                 REG 14 -- INTERRUPT ENABLE REGISTER
		//                           +-+-+-+-+-+-+-+-+
		//                           |7|6|5|4|3|2|1|0|
		//                           +-+-+-+-+-+-+-+-+            -+
		//                            | | | | | | | +--- CA2       |
		//                            | | | | | | +----- CA1       |  0 = INTERRUPT
		//                            | | | | | +------- SHIFT REG |      DISABLED
		//                            | | | | +--------- CB2       |_
		//                            | | | +----------- CB1       |
		//                            | | +------------- TIMER 2   |  1 = INTERRUPT
		//                            | +--------------- TIMER 1   |      ENABLED
		//                            +----------------- SET/CLEAR |
		//                                                       -+
		// 
		//          NOTES:
		//          1  IF BIT 7 IS A "0", THEN EACH "1" IN BITS 0-6 DISABLES THE
		//             CORRESPONDING INTERRUPT.
		//          2  IF BIT 7 IS A "1",  THEN EACH "1" IN BITS 0-6 ENABLES THE
		//             CORRESPONDING INTERRUPT.
		//          3  IF A READ OF THIS REGISTER IS DONE, BIT 7 WILL BE "1" AND
		//             ALL OTHER BITS WILL REFLECT THEIR ENABLE/DISABLE STATE.

        case 0xE:
			return io[addr] | 0x80;
            
        case 0xF: // Should not reach. Handled individually by VIA1 and VIA2
            assert(0);
            break;
	}

	// default behavior
	return io[addr];
}

uint8_t VIA1::peek(uint16_t addr)
{
	switch(addr) {
            
        case 0x0: { // ORB - Output register B
            
            // |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   |
            // -----------------------------------------------------------------
            // |  ATN  | Device addr.  |  ATN  | Clock | Clock | Data  | Data  |
            // |  in   |               |  out  |  out  |  in   |  out  |  in   |

            uint8_t external =
                (floppy->iec->getAtnLine() ? 0x00 : 0x80) | // 7
                (floppy->iec->getClockLine() ? 0x00 : 0x04) | // 2
                (floppy->iec->getDataLine() ? 0x00 : 0x01); // 0

            // Determine read value
            uint8_t result =
                (ddrb & orb) |        // value of pins that are configured as outputs
                (~ddrb & external); // value of pins that are configures as inputs
            
            // Set device address to zero
            // TODO: Device address is "hard-wired". Set it in "external", above
            result &= 0x9F;
            
			return result;
        }
            
		case 0x1: // ORA - Output register A
            
            // Only PA0 is connected (VC1541 Schaltplan: "TROO sense")
            
            // warn("READ FROM VIA1:ORA DETECTED\n");
            
            // DOES THIS REALLY HAPPEN??
			clearAtnIndicator();
			floppy->cpu->clearIRQLineATN();
			return ora;

		default:
			return VIA6522::peek(addr);	
	}
}

uint8_t h[16];

uint8_t VIA2::peek(uint16_t addr)
{
	switch(addr) {

        case 0x0: {
            
            // |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   |
            // -----------------------------------------------------------------
            // | SYNC  | Timer control | Write |  LED  | Rot.  | Stepper motor |
            // |       | (4 disk zones)|protect|       | motor | (head move)   |

            // Value of external pins
            uint8_t external =
                (floppy->getSyncMark() ? 0x00 : 0x80) | // 7
                (floppy->isWriteProtected() ? 0x00 : 0x10); // 4
            
            // Determine read value
            uint8_t result =
                (ddrb & orb) |        // value of pins that are configured as outputs
                (~ddrb & external); // value of pins that are configures as inputs

			return result;
        }
            
		case 0x1:
            // Hard wired to the data lines of the Gate Array (U10)
            // TODO: Implement proper read/write behavior
            
            if (tracingEnabled()) {
				msg("%02X ", ora);
			}
            return ora;
            
        case 0x4:
            floppy->cpu->clearIRQLineVIA1();
            return VIA6522::peek(addr);
            
        case 0xF:
            if (tracingEnabled()) {
                msg("%02X ", ora);
            }
            return ora;

		default:
			return VIA6522::peek(addr);	
	}
}


void VIA6522::poke(uint16_t addr, uint8_t value)
{
	assert (addr <= 0x0F);
		
	switch(addr) {

        case 0x0: // Should not reach. Handled individually by VIA1 and VIA2
			assert(false);
			break;
		
		case 0x1: // Should not reach. Handled individually by VIA1 and VIA2
			assert(false);
			break;
            
		case 0x2: // DDRB - Data direction register B
            
            // "0"  ASSOCIATED PB PIN IS AN INPUT (HIGH IMPEDANCE)
            // "1"  ASSOCIATED PB PIN IS AN OUTPUT WHOSE LEVEL IS DETERMINED BY ORB REGISTER BIT" [F. K.]
            
			ddrb = value;
			return;
			
		case 0x3: // DDRB - Data direction register A
            
            // "0"  ASSOCIATED PB PIN IS AN INPUT (HIGH IMPEDANCE)
            // "1"  ASSOCIATED PB PIN IS AN OUTPUT WHOSE LEVEL IS DETERMINED BY ORA REGISTER BIT" [F. K.]

			ddra = value;
			return;
		
		case 0x4: // T1 low-order counter
            
            // "8 BITS LOADED INTO T1 LOW-ORDER LATCHES. LATCH CONTENTS ARE TRANSFERRED
            //  INTO LOW-ORDER COUNTER AT THE TIME THE HIGH-ORDER COUNTER IS LOADED (REG 5)" [F. K.]
            
			t1_latch_lo = value;
			return;
            
		case 0x5: // T1 high-order counter
            
            // "8 BITS LOADED INTO T1 HIGH-ORDER LATCHES. ALSO AT THIS TIME BOTH HIGH- AND
            //  LOW-ORDER LATCHES TRANSFERRED INTO T1 COUNTER. T1 INTERRUPT FLAG ALSO IS RESET" [F. K.]
            
			t1_latch_hi = value;
            t1 = HI_LO(t1_latch_hi, t1_latch_lo);
            
            clearInterruptFlag_T1();
			floppy->cpu->clearIRQLineVIA1();
			return;

		case 0x6: // T1 low-order latct

            // "8 BITS LOADED INTO T1 LOW-ORDER LATCHES. THIS OPERATION IS NO DIFFERENT
            //  THAN A WRITE INTO REG 4" [F. K.]

            t1_latch_lo = value;
			return;
			
		case 0x7: // T1 high-order latch
            
            // "8 BITS LOADED INTO T1 HIGH-ORDER LATCHES. UNLIKE REG 4 OPERATION NO LATCH TO
            //  COUNTER TRANSFERS TAKE PLACE" [F. K.]
            
			t1_latch_hi = value;
			return;
		
		case 0x8: // T2 low-order latch/counter

            // "8 BITS FROM T2 LOW-ORDER COUNTER TRANSFERRED TO MPU. T2 INTERRUPT FLAG IS RESET" [F. K.]
            
            t2_latch_lo = value;
            clearInterruptFlag_T2();
            floppy->cpu->clearIRQLineVIA2();
            return;
		
		case 0x9: // T2 high-order counter
            
            // "8 BITS LOADED INTO T2 HIGH-ORDER COUNTER. ALSO, LOW-ORDER LATCH TRANSFERRED
            //  TO LOW-ORDER COUNTER. IN ADDITION T2 INTERRUPT FLAG IS RESET" [F. K.]
            
            t2 = HI_LO(value, t2_latch_lo);
            clearInterruptFlag_T2();
			floppy->cpu->clearIRQLineVIA2();
			return;

        case 0xA: // Shift register
            
            clearInterruptFlag_SR();
            warn("poke(0xA): Shift register is not emulated!");
            break;
            
        case 0xB: // Auxiliary control register
            
            warn("poke(0xB): Shift register is not emulated!");
            break;

		case 0xC:
			break;

		case 0xD:
			io[addr] &= ~value;
			return;

		case 0xE:
			if (value & 0x80) {
				io[addr] |= value & 0x7f;
            } else {
				io[addr] &= ~value;
			}
			return;

        case 0xF: // Should not reach. Handled individually by VIA1 and VIA2
            assert(false);
            break;
    }

	// default bevahior
	io[addr] = value;
}

void VIA1::poke(uint16_t addr, uint8_t value)
{
	switch(addr) {

        case 0x0:
            
            // |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   |
            // -----------------------------------------------------------------
            // |  ATN  | Device addr.  |  ATN  | Clock | Clock | Data  | Data  |
            // |  in   |               |  out  |  out  |  in   |  out  |  in   |

			orb = value;
            
            // Clear flags in interrupt flag register (IFR)
            clearInterruptFlag_CB1();
            if (!CB2selectedAsIndependent())
                clearInterruptFlag_CB2();
            
            // Clean this up ...
			floppy->iec->updateDevicePins(orb, ddrb);
			return;

		case 0x1:
        case 0xF:
			ora = value;
            
            // Clear flags in interrupt flag register (IFR)
            clearInterruptFlag_CA1();
            if (!CA2selectedAsIndependent())
                clearInterruptFlag_CA2();

            // Clean this up ...
			floppy->cpu->clearIRQLineATN();
			return;
		
		case 0x2:
			ddrb = value;
			// debug(2, "Writing %d into ddrb via 1\n", value);
			floppy->iec->updateDevicePins(orb, ddrb);
			return; 
						
		default:
			VIA6522::poke(addr, value);	
	}
}

void VIA2::poke(uint16_t addr, uint8_t value)
{
	switch(addr) {

        case 0x0:
			// Port B, Steuerport
			// Bit 0: Schrittmotor Spule 0
			// Bit 1: Schrittmotor Spule 1
			// Bit 2: 1 = Laufwerksmotor an
			// Bit 3: 1 = rote LED an
			// Bit 4: 0 = Diskette schreibgeschützt
			// Bit 5,6: Timersteuerung
			// Bit 7: 0 = SYNC-Signal
			if ((orb & 0x03) != (value & 0x03)) {
				// Bits #0-#1: Head step direction. 
				// Decrease value (%00-%11-%10-%01-%00...) to move head downwards; 
				// Increase value (%00-%01-%10-%11-%00...) to move head upwards.
				if ((value & 0x03) == ((orb+1) & 0x03)) {
					// Move head upwards...
					floppy->moveHeadUp();
				} else if ((value & 0x03) == ((orb-1) & 0x03)) {
					// Move head downwards...
					floppy->moveHeadDown();
				} else {
					warn("Unexpected stepper motor control sequence in VC1541 detected\n");
				}
			}

			if ((orb & 0x04) != (value & 0x04)) {
				if (value & 0x04)
					floppy->startRotating();
				else 
					floppy->stopRotating();
			}

			if ((orb & 0x08) != (value & 0x08)) {
				if (value & 0x08)
					floppy->activateRedLED();
				else 
					floppy->deactivateRedLED();
			}

			orb = value;
			return;

		case 0x1:
        case 0xF:
			// Port A: Daten zum Tonkopf
			if (tracingEnabled()) {
				msg(" W%02X", value);
			}
			ora = value;
			return;

		case 0x3:
			ddra = value;
			if (ddra != 0x00 && ddra != 0xFF) {
				debug(1, "Data direction bits of VC1541 contain suspicious values\n");
			}
			return;
			
		case 0xC:
			io[addr] = value;
			return;
			
		default:
			VIA6522::poke(addr, value);	
	}
}


VIA1::VIA1()
{
    name = "VIA1";
	debug(2, "  Creating VIA1 at address %p...\n", this);
}
	
VIA1::~VIA1()
{
	debug(2, "  Releasing VIA1...\n");
}

void VIA1::reset(C64 *c64)
{
	debug(2, "  Resetting VIA1...\n");
	VIA6522::reset(c64);
}

VIA2::VIA2()
{
    name = "VIA2";
	debug(2, "  Creating VIA2 at address %p...\n", this);
}
	
VIA2::~VIA2()
{
	debug(2, "  Releasing VIA2...\n");
}

void VIA2::reset(C64 *c64)
{
	debug(2, "  Resetting VIA2...\n");
	VIA6522::reset(c64);
}
