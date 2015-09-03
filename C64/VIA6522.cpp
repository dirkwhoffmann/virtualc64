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
    
    // Register snapshot items
    SnapshotItem items[] = {
        { &ddra, sizeof(ddra) },
        { &ddrb, sizeof(ddrb) },
        { &ora, sizeof(ora) },
        { &orb, sizeof(orb) },
        { &ira, sizeof(ira) },
        { &irb, sizeof(irb) },
        { &t1, sizeof(t1) },
        { &t2, sizeof(t2) },
        { &t1_latch_lo, sizeof(t1_latch_lo) },
        { &t1_latch_hi, sizeof(t1_latch_hi) },
        { &t2_latch_lo, sizeof(t2_latch_lo) },
        { &t1_underflow, sizeof(t1_underflow) },
        { &t2_underflow, sizeof(t2_underflow) },
        { io, sizeof(io) },
        { NULL, 0 }
    };
    registerSnapshotItems(items, sizeof(items));
}

VIA6522::~VIA6522()
{
}
	
void VIA6522::reset(C64 *c64)
{
    VirtualComponent::reset(c64);

    // Establish bindings
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
    t1_underflow = false;
    t2_underflow = false;
    memset(io, 0, sizeof(io));
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
	msg("                     IO memory : ");
	for (int j = 0; j < 16; j ++) {
		msg("%02X ", io[j]);
	}
	msg("\n");
}

// -----------------------------------------------------------------------------------------------
//                                    Execution functions
// -----------------------------------------------------------------------------------------------

// One-shot mode timing [F. K.]
//               +-+ +-+ +-+ +-+ +-+ +-+   +-+ +-+ +-+ +-+ +-+ +-+
//          02 --+ +-+ +-+ +-+ +-+ +-+ +-#-+ +-+ +-+ +-+ +-+ +-+ +-
//                 |   |                           |
//                 +---+                           |
// WRITE T1C-H ----+   +-----------------#-------------------------
//  ___                |                           |
//  IRQ OUTPUT --------------------------#---------+
//                     |                           +---------------
//                     |                           |
//  PB7 OUTPUT --------+                           +---------------
//                     +-----------------#---------+
//                     | N |N-1|N-2|N-3|     | 0 |N| |N-1|N-2|N-3|
//                     |                           |
//                     |<---- N + 1.5 CYCLES ----->|

void
VIA6522::executeTimer1()
{
    if (t1_underflow) {

        t1_underflow = false;
        setInterruptFlag_T1();
        // TODO: PB7 output
        
        // Timer 1 has the special ability to run in free-run mode that reloads automatically
        // This generates a continous stream of interrupt events or a square wave on PB7 which
        // get inverted every time the timer underflows.
        if (freeRunMode1()) {
            t1 = HI_LO(t1_latch_hi, t1_latch_lo);
        }
        
        return;
    }
    
    if (t1) {
        // Keep on counting
        t1_underflow = (--t1 == 0);
    }
}

void
VIA6522::executeTimer2()
{
    if (t2_underflow) {
            
        t2_underflow = false;
        setInterruptFlag_T2();
        // TODO: PB7 output
        
        return;
    }

    if (t2){
        // Keep on counting
        t2_underflow = (--t2 == 0);
    }
}

bool
VIA6522::IRQ() {
    if (io[0xD] /* IFR */ & io[0xE] /* IER */) {
        floppy->cpu->setIRQLineVIA();
        return true;
    } else {
        floppy->cpu->clearIRQLineVIA();
        return false;
    }
}

// -----------------------------------------------------------------------------------------------
//                                Peek and Poke (Shared behaviour)
// -----------------------------------------------------------------------------------------------

uint8_t 
VIA6522::peek(uint16_t addr)
{
	assert (addr <= 0xF);
		
	switch(addr) {
            
        case 0x0: assert(0); break; // Not reached. Handled individually by VIA1 and VIA2
        case 0x1: assert(0); break; // Not reached. Handled individually by VIA1 and VIA2
						
        case 0x2: // DDRB - Data direction register B
            
			return ddrb;

        case 0x3: // DDRA - Data direction register A
            
			return ddra;
			
        case 0x4: // T1 low-order counter
            
            // "8 BITS FROM T1 LOW-ORDER COUNTER TRANSFERRED TO MPU. IN ADDITION T1 INTERRUPT FLAG
            //  IS RESET (BIT 6 IN INTERRUPT FLAG REGISTER)" [F. K.]
            
            clearInterruptFlag_T1();
            floppy->cpu->clearIRQLineVIA();
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
            floppy->cpu->clearIRQLineVIA();
			return LO_BYTE(t2);
			
		case 0x9: // T2 high-order counter COUNTER TRANSFERRED TO MPU" [F. K.]
            
            // "8 BITS FROM T2 HIGH-ORDER
			return HI_BYTE(t2);
            
        case 0xA: // Shift register

            clearInterruptFlag_SR();
            // warn("peek(0xA): Shift register is not emulated!\n");
            break;
			
		case 0xB: // Auxiliary control register

            // warn("peek(0xB): Shift register is not emulated!\n");
            break;
		
        case 0xC: // Peripheral control register
            // TODO
            break;
            
        case 0xD: { // IFR - Interrupt Flag Register
            
            // "Bit 7 indicates the status of the IRQ output. This bit corresponds to the logic function:
            //  IRQ = IFR6xIER6 + IFR5xIER5 + IFR4xIER4 + IFR3xIER3 + IFR2xIER2 + IFR1xIER1 + IFR0xIER0
            //  x = logic AND, + = logic OR" [F. K.]
            
            io[0xD] &= 0x7F;
            uint8_t irq = (io[0xD] /* IFR */ & io[0xE] /* IER */) ? 0x80 : 0x00;
            return io[0xD] | irq;
            
			// OLD: return io[addr] | ((io[addr] & io[0x0E]) ? 0x80 : 0x00);
        }
            
        case 0xE: // Interrupt enable register
            
            return io[addr] | 0x80; // Bit 7 (set/clear bit) always shows up as 1

        case 0xF: assert(0); break; // Not reached. Handled individually by VIA1 and VIA2
	}

    return io[addr];
}

void VIA6522::poke(uint16_t addr, uint8_t value)
{
    assert (addr <= 0x0F);
    
    switch(addr) {
            
        case 0x0: assert(0); break; // Not reached. Handled individually by VIA1 and VIA2
        case 0x1: assert(0); break; // Not reached. Handled individually by VIA1 and VIA2
            
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
            floppy->cpu->clearIRQLineVIA();
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
            floppy->cpu->clearIRQLineVIA();
            return;
            
        case 0x9: // T2 high-order counter
            
            // "8 BITS LOADED INTO T2 HIGH-ORDER COUNTER. ALSO, LOW-ORDER LATCH TRANSFERRED
            //  TO LOW-ORDER COUNTER. IN ADDITION T2 INTERRUPT FLAG IS RESET" [F. K.]
            
            t2 = HI_LO(value, t2_latch_lo);
            clearInterruptFlag_T2();
            floppy->cpu->clearIRQLineVIA();
            return;
            
        case 0xA: // Shift register
            
            clearInterruptFlag_SR();
            // warn("poke(0xA,%02X): Shift register is not emulated! (PC = %04X)\n",
            //     value, c64->cpu->getPC_at_cycle_0());
            break;
            
        case 0xB: // Auxiliary control register
            
            // warn("poke(0xB,%02X): Shift register is not emulated! (PC = %04X)\n",
            //     value, c64->cpu->getPC_at_cycle_0());
            break;
            
        case 0xC: // Peripheral control register
            
            break;
            
        case 0xD: // IFR - Interrupt Flag Register
            
            // "... individual flag bits may be cleared by writing a "1" into the appropriate bit of the IFR."
            
            io[addr] &= ~value;
            return;
            
        case 0xE: // IER - Interrupt Enable Register
            
            // Bit 7 distinguishes between set and clear
            // If bit 7 is 1, each 1 in the provided value will set the corresponding bit
            // If bit 7 is 0, each 1 in the provided value will clear the corresponding bit
            
            if (value & 0x80) {
                io[addr] |= value & 0x7f;
            } else {
                io[addr] &= ~value;
            }
            return;
            
        case 0xF: assert(0); break; // Not reached. Handled individually by VIA1 and VIA2
    }
    
    io[addr] = value;
}

// -----------------------------------------------------------------------------------------------
//                                     Peek and Poke (VIA 1)
// -----------------------------------------------------------------------------------------------

uint8_t VIA1::peek(uint16_t addr)
{
    switch(addr) {
            
        case 0x0: { // ORB - Output register B
            
            // Clear flags in interrupt flag register (IFR)
            clearInterruptFlag_CB1();
            if (!CB2selectedAsIndependent())
                clearInterruptFlag_CB2();
            
            // |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   |
            // -----------------------------------------------------------------
            // |  ATN  | Device addr.  |  ATN  | Clock | Clock | Data  | Data  |
            // |  in   |               |  out  |  out  |  in   |  out  |  in   |
            
            // Port values (outside the chip)
            uint8_t external =
            (floppy->iec->getAtnLine() /* 7 */ ? 0x00 : 0x80) |
            (floppy->iec->getClockLine() /* 2 */ ? 0x00 : 0x04) |
            (floppy->iec->getDataLine() /* 0 */ ? 0x00 : 0x01);
            
            // Determine read value
            uint8_t result =
            (ddrb & orb) |      // Values of bits configured as outputs
            (~ddrb & external); // Values of bits configured as inputs
            
            // Set device address to zero
            // TODO: Device address is "hard-wired". Set it in "external", above
            result &= 0x9F;
            
            return result;
        }
            
        case 0x1: // ORA - Output register A
        case 0xF:
            
            // Clear flags in interrupt flag register (IFR)
            clearInterruptFlag_CA1();
            if (!CA2selectedAsIndependent())
                clearInterruptFlag_CA2();

            // Clean this up ...
            floppy->cpu->clearIRQLineATN();
            return ora;
            
        default:
            return VIA6522::peek(addr);
    }
}

void VIA1::poke(uint16_t addr, uint8_t value)
{
	switch(addr) {

        case 0x0: // ORB - Output register B

            // Clear flags in interrupt flag register (IFR)
            clearInterruptFlag_CB1();
            if (!CB2selectedAsIndependent())
                clearInterruptFlag_CB2();
 
            // |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   |
            // -----------------------------------------------------------------
            // |  ATN  | Device addr.  |  ATN  | Clock | Clock | Data  | Data  |
            // |  in   |               |  out  |  out  |  in   |  out  |  in   |

			orb = value;
			floppy->iec->updateDevicePins(orb, ddrb);
			return;

		case 0x1: // ORA - Output register A
        case 0xF:
            
            // Clear flags in interrupt flag register (IFR)
            clearInterruptFlag_CA1();
            if (!CA2selectedAsIndependent())
                clearInterruptFlag_CA2();

            
            printf("VIA:pokeORA %02X\n", ora);
			ora = value;
            
            // Clean this up ...
			// floppy->cpu->clearIRQLineATN();
			return;
		
		case 0x2:
			ddrb = value;
			floppy->iec->updateDevicePins(orb, ddrb);
			return; 
						
		default:
			VIA6522::poke(addr, value);	
	}
}


// -----------------------------------------------------------------------------------------------
//                                     Peek and Poke (VIA 1)
// -----------------------------------------------------------------------------------------------

uint8_t VIA2::peek(uint16_t addr)
{
    switch(addr) {
            
        case 0x0: { // ORB - Output register B
            
            // Clear flags in interrupt flag register (IFR)
            clearInterruptFlag_CB1();
            if (!CB2selectedAsIndependent())
                clearInterruptFlag_CB2();
            
            // |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   |
            // -----------------------------------------------------------------
            // | SYNC  | Timer control | Write |  LED  | Rot.  | Stepper motor |
            // |       | (4 disk zones)|protect|       | motor | (head move)   |
            
            // Collect values on the external port lines
            bool SYNC = floppy->getBitAccuracy() ? floppy->getSync() : floppy->getFastLoaderSync();
            uint8_t external = (SYNC /* 7 */ ? 0x00 : 0x80) |
            (floppy->getLightBarrier() /* 4 */ ? 0x00 : 0x10) |
            (floppy->getRedLED() /* 3 */ ? 0x00 : 0x08) |
            (floppy->isRotating() /* 2 */ ? 0x00 : 0x04);
            
            uint8_t result =
            (ddrb & orb) |      // Values of bits configured as outputs
            (~ddrb & external); // Values of bits configures as inputs
            
            return result;
        }
            
        case 0x1: // ORA - Output register A
        case 0xF: {
            
            uint8_t result;
            
            // Clear flags in interrupt flag register (IFR)
            clearInterruptFlag_CA1();
            if (!CA2selectedAsIndependent())
                clearInterruptFlag_CA2();

            // If bit accurate emulation is disabled, we perform the read action here ...
            if (!floppy->getBitAccuracy())
                floppy->fastLoaderRead();

            if (inputLatchingEnabledA()) {
                // This is the normal operation mode of the drive.
                // Every byte that comes from
                result =
                (ddra & ora) | // Values of bits configured as outputs
                (~ddra & ira); // Values of bits configures as inputs
            } else {
                warn("INPUT LATCHING OF VIA2 IS DISABLED!");
                result = 0;
            }
            
            if (tracingEnabled()) {
                msg("%02X ", ora);
            }
            
            // return ora;
            return result; 
        }
            
        case 0x4:
            floppy->cpu->clearIRQLineVIA();
            return VIA6522::peek(addr);
            
        default:
            return VIA6522::peek(addr);
    }
}

void VIA2::poke(uint16_t addr, uint8_t value)
{
	switch(addr) {

        case 0x0: { // ORB - Output register B
            
            // Clear flags in interrupt flag register (IFR)
            clearInterruptFlag_CB1();
            if (!CB2selectedAsIndependent())
                clearInterruptFlag_CB2();

            // |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   |
            // -----------------------------------------------------------------
            // | SYNC  | Timer control | Write |  LED  | Rot.  | Stepper motor |
            // |       | (4 disk zones)|protect|       | motor | (head move)   |

            // Disable bits that are not configured as outputs
            value &= ddrb;
            
            // Bits 6 and 5
            floppy->setZone((value >> 5) & 0x03);
            
            // Bit 3
            floppy->setRedLED(GET_BIT(value,3));

            // Bit 2
            floppy->setRotating(GET_BIT(value,2));

            // Bits 1 and 0
			if ((orb & 0x03) != (value & 0x03)) {
                
				// A decrease (00-11-10-01-00...) moves the the head down
				// An increase (00-01-10-11-00...) moves the head up
                
				if ((value & 0x03) == ((orb+1) & 0x03)) {
					floppy->moveHeadUp();
				} else if ((value & 0x03) == ((orb-1) & 0x03)) {
					floppy->moveHeadDown();
				} else {
					warn("Unexpected stepper motor control sequence in VC1541 detected\n");
				}
			}

			orb = value;
			return;
        }
            
		case 0x1: // ORA - Output register A
        case 0xF:
            
            // Clear flags in interrupt flag register (IFR)
            clearInterruptFlag_CA1();
            if (!CA2selectedAsIndependent())
                clearInterruptFlag_CA2();

            // Hard wired to the data lines of the Gate Array (U10) (read/write head)
            // TODO: Take care of ddra
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

void VIA2::debug0xC() {
    
    uint8_t value = io[0xC];
    
     debug(2,"CA1:\n");
     debug(2,"  %s ACTIVE EDGE\n", (GET_BIT(value,0) ? "POSITIVE" : "NEGATIVE"));
     debug(2,"CA2:\n");
     switch ((value >> 1) & 0x07) {
     case 0: debug(2,"  INPUT NEG. ACTIVE EDGE\n"); break;
     case 1: debug(2,"  INDEPENDENT INTERRUPT INPUT NEGATIVE EDGE\n"); break;
     case 2: debug(2,"  INPUT POS. ACTIVE EDGE\n"); break;
     case 3: debug(2,"  INDEPENDENT INTERRUPT INPUT POSITIVE EDGE\n"); break;
     case 4: debug(2,"  HANDSHAKE OUTPUT\n"); break;
     case 5: debug(2,"  PULSE OUTPUT\n"); break;
     case 6: debug(2,"  LOW OUTPUT %04X\n", floppy->cpu->getPC_at_cycle_0()); break;
     case 7: debug(2,"  HIGH OUTPUT %04X\n", floppy->cpu->getPC_at_cycle_0()); break;
     }
    
    debug(2,"CB1:\n");
    debug(2,"  %s ACTIVE EDGE\n", (GET_BIT(value,4) ? "POSITIVE" : "NEGATIVE"));
    debug(2,"CB2:\n");
    switch ((value >> 5) & 0x07) {
        case 0: debug(2,"  INPUT NEG. ACTIVE EDGE\n"); break;
        case 1: debug(2,"  INDEPENDENT INTERRUPT INPUT NEGATIVE EDGE\n"); break;
        case 2: debug(2,"  INPUT POS. ACTIVE EDGE\n"); break;
        case 3: debug(2,"  INDEPENDENT INTERRUPT INPUT POSITIVE EDGE\n"); break;
        case 4: debug(2,"  HANDSHAKE OUTPUT\n"); break;
        case 5: debug(2,"  PULSE OUTPUT\n"); break;
        case 6: debug(2,"  LOW OUTPUT\n"); break;
        case 7: debug(2,"  HIGH OUTPUT\n"); break;
    }
}