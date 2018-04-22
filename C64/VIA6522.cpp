/*
 * (C) 2006 - 2018 Dirk W. Hoffmann. All rights reserved.
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
	setDescription("VIA");
    
    // Register snapshot items
    SnapshotItem items[] = {
        { &ddra,            sizeof(ddra),           CLEAR_ON_RESET },
        { &ddrb,            sizeof(ddrb),           CLEAR_ON_RESET },
        { &ora,             sizeof(ora),            CLEAR_ON_RESET },
        { &orb,             sizeof(orb),            CLEAR_ON_RESET },
        { &ira,             sizeof(ira),            CLEAR_ON_RESET },
        { &irb,             sizeof(irb),            CLEAR_ON_RESET },
        { &t1,              sizeof(t1),             CLEAR_ON_RESET },
        { &t2,              sizeof(t2),             CLEAR_ON_RESET },
        { &t1_latch_lo,     sizeof(t1_latch_lo),    CLEAR_ON_RESET },
        { &t1_latch_hi,     sizeof(t1_latch_hi),    CLEAR_ON_RESET },
        { &t2_latch_lo,     sizeof(t2_latch_lo),    CLEAR_ON_RESET },
        { &pb7toggle,       sizeof(pb7toggle),      CLEAR_ON_RESET },
        { &pb7timerOut,     sizeof(pb7timerOut),    CLEAR_ON_RESET },
        { &pcr,             sizeof(pcr),            CLEAR_ON_RESET },
        { &acr,             sizeof(acr),            CLEAR_ON_RESET },
        { &ier,             sizeof(ier),            CLEAR_ON_RESET },
        { &ifr,             sizeof(ifr),            CLEAR_ON_RESET },
        { &sr,              sizeof(sr),             CLEAR_ON_RESET },
        { &delay,           sizeof(delay),          CLEAR_ON_RESET },
        { &feed,            sizeof(feed),           CLEAR_ON_RESET },
        { NULL,             0,                      0 }};
    
    registerSnapshotItems(items, sizeof(items));
}

VIA6522::~VIA6522()
{
}
	
void VIA6522::reset()
{
    VirtualComponent::reset();

    
    t1 = 0x01AA;
    t2 = 0x01AA;
    t1_latch_hi = 0x01;
    t1_latch_lo = 0xAA;
    t2_latch_lo = 0xAA;
    
    feed |= (VIACountA0 | VIACountB0);
}

void 
VIA6522::dumpState()
{
	msg("VIA:\n");
	msg("----\n\n");
	msg("             Input register (IRA) : %02X\n", ira);
	msg("             Input register (IRB) : %02X\n", irb);
	msg("            Output register (ORA) : %02X\n", ora);
	msg("            Output register (ORB) : %02X\n", orb);
	msg("   Data direction register (DDRA) : %02X\n", ddra);
	msg("   Data direction register (DDRB) : %02X\n", ddrb);
    msg("Peripheral control register (PCR) : %02X\n", pcr);
    msg("         Auxiliary register (ACR) : %02X\n", acr);
    msg("  Interrupt enable register (IER) : %02X\n", ier);
    msg("    Interrupt flag register (IFR) : %02X\n", ifr);
    msg("              Shift register (SR) : %02X\n", sr);
	msg("              Input latching A : %s\n", inputLatchingEnabledA() ? "enabled" : "disabled");
	msg("              Input latching B : %s\n", inputLatchingEnabledB() ? "enabled" : "disabled");
	msg("                       Timer 1 : %d (latched: %d)\n", t1, LO_HI(t1_latch_lo, t1_latch_hi));
	msg("                       Timer 2 : %d (latched: %d)\n", t2, LO_HI(t2_latch_lo, 0));
	msg("                     IO memory : ");
	msg("\n");
}

// --------------------------------------------------------------------------------------------
//                                    Execution functions
// --------------------------------------------------------------------------------------------

void
VIA6522::execute()
{
    // Execute timers
    executeTimer1();
    executeTimer2();
    
    // Check for interrupt condition
    if (ifr & ier) {
        delay |= VIAInterrupt0;
    }
    
    // Trigger interrupt if requested
    if (delay & VIAInterrupt1) {
        c64->floppy.cpu.pullDownIrqLine(CPU::VIA);
    }
    
    // Move trigger event flags left and feed in new bits
    delay = ((delay << 1) & VIAClearBits) | feed;

}

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
    if (delay & VIAReloadA2) {
        t1 = HI_LO(t1_latch_hi, t1_latch_lo);
    }

    if (delay & VIACountA1) {
        t1--;
    }
    
    if (t1 == 0) {
        
        if (freeRunMode1()) {
            
            /* "In the free-running mode,
             * (1) the interrupt flag is set and
             * (2) the signal on PB7 is inverted each time the counter reaches zero.
             * (3) However, instead of continuing to decrement from zero after a time-out,
             *     the timer automatically transfers the contents of the latch into the
             *     counter and continues to decrement from there."
             */
            
            if (!(feed & VIAPostOneShotA0)) {
                SET_BIT(ifr,6);         // (1) 
                pb7toggle = !pb7toggle; // (2)
                delay |= VIAReloadA0;   // (3)
            }
            
        } else {
                
            /* "The Timer 1 one-shot mode generates a single interrupt for each timer load
             *  operation."
             */
            
            if (!(feed & VIAPostOneShotA0)) {
                SET_BIT(ifr,6);
                pb7toggle = !pb7toggle;
            }
        }
        
        feed |= VIAPostOneShotA0;
            
        /* "In addition to generating a single interrupt, Timer 1 can be programmed
         *  to produce a single negative pulse on the PB7 peripheral pin. With the
         *  output enabled (ACR7=1) a "write T1C-H" operation will cause PB7 to go low.
         *  PB7 will return high when Timer 1 times out. The result is a single
         *  programmable width pulse."
         */
            
        if (acr & 0x80) {
            pb7timerOut = pb7toggle;
        }
    }
}

void
VIA6522::executeTimer2()
{
    if (delay & VIACountB1) {
        t2--;
    }
    
    if (t2 == 0) {
        
        if (!(delay & VIAPostOneShotB0)) {
            SET_BIT(ifr,5);
            feed |= VIAPostOneShotB0;
        }
    }
}

bool
VIA6522::IRQ() {
    if (ifr & ier) {
        c64->floppy.cpu.pullDownIrqLine(CPU::VIA);
        return true;
    } else {
        c64->floppy.cpu.releaseIrqLine(CPU::VIA);
        return false;
    }
}

// --------------------------------------------------------------------------------------------
//                                Peek and Poke (Shared behaviour)
// --------------------------------------------------------------------------------------------

uint8_t 
VIA6522::peek(uint16_t addr)
{
	assert (addr <= 0xF);
		
	switch(addr) {
            
        case 0x0: // ORB - Output register B

            // Clear flags in interrupt flag register (IFR)
            clearInterruptFlag_CB1();
            if (shouldClearCB2onRead())
                clearInterruptFlag_CB2();
            return 0;
            
        case 0x1: // ORA - Output register A

            // Clear flags in interrupt flag register (IFR)
            clearInterruptFlag_CA1();
            if (shouldClearCA2onRead())
                clearInterruptFlag_CA2();
            return 0;
            
        case 0x2: // DDRB - Data direction register B
            
			return ddrb;

        case 0x3: // DDRA - Data direction register A
            
			return ddra;
			
        case 0x4: // T1 low-order counter
            
            // "8 BITS FROM T1 LOW-ORDER COUNTER TRANSFERRED TO MPU. IN ADDITION T1 INTERRUPT FLAG
            //  IS RESET (BIT 6 IN INTERRUPT FLAG REGISTER)" [F. K.]
            
            clearInterruptFlag_T1();
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
			return LO_BYTE(t2);
			
		case 0x9: // T2 high-order counter COUNTER TRANSFERRED TO MPU" [F. K.]
            
            // "8 BITS FROM T2 HIGH-ORDER
			return HI_BYTE(t2);
            
        case 0xA: // Shift register

            clearInterruptFlag_SR();
            return sr;
			
		case 0xB: // Auxiliary control register

            return acr;
		
        case 0xC: // Peripheral control register

            return pcr;
            
        case 0xD: // IFR - Interrupt Flag Register
            
            assert((ifr & 0x80) == 0);
            assert((ier & 0x80) == 0);
            return ifr | ((ifr & ier) ? 0x80 : 0x00);
            
        case 0xE: // Interrupt enable register
            
            return ier | 0x80; // Bit 7 (set/clear bit) always shows up as 1

        case 0xF:
            
            // Clear flags in interrupt flag register (IFR)
            clearInterruptFlag_CA1();
            if (shouldClearCA2onRead())
                clearInterruptFlag_CA2();
            return 0;
    }

    assert(0);
    return 0;
}

uint8_t
VIA6522::read(uint16_t addr)
{
    assert (addr <= 0xF);
    
    switch(addr) {
            
        case 0x4: // T1 low-order counter
        
            return LO_BYTE(t1);
            
        case 0x8: // T2 low-order latch/counter
            
            return LO_BYTE(t2);
            
        case 0xA: // Shift register
        case 0xB: // Auxiliary control register
        case 0xC: // Peripheral control register
            
            break; // TODO
            
        case 0xD: { // IFR - Interrupt Flag Register
            
            uint8_t ioD = ifr & 0x7F;
            uint8_t irq = (ifr & ier) ? 0x80 : 0x00;
            return ioD | irq;
        }
  
        default:
            return peek(addr);
    }
    
    return 0;
}

void VIA6522::poke(uint16_t addr, uint8_t value)
{
    assert (addr <= 0x0F);
    
    switch(addr) {
            
        case 0x0: // ORB - Output register B (shared functionality of VIA1 and VIA2)
            
            // Clear flags in interrupt flag register (IFR)
            clearInterruptFlag_CB1();
            if (shouldClearCB2onWrite())
                clearInterruptFlag_CB2();
            return;

        case 0x1: // ORA - Output register A (shared functionality of VIA1 and VIA2)
            
            // Clear flags in interrupt flag register (IFR)
            clearInterruptFlag_CA1();
            if (shouldClearCA2onWrite())
                clearInterruptFlag_CA2();
            return;
            
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
            feed &= ~(VIAPostOneShotA0);
            
            // Delay counting down for one cycle
            delay &= ~(VIACountA1);
            return;
            
        case 0x6: // T1 low-order latch
            
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
            c64->floppy.cpu.releaseIrqLine(CPU::VIA);
            return;
            
        case 0x9: // T2 high-order counter
            
            // "8 BITS LOADED INTO T2 HIGH-ORDER COUNTER. ALSO, LOW-ORDER LATCH TRANSFERRED
            //  TO LOW-ORDER COUNTER. IN ADDITION T2 INTERRUPT FLAG IS RESET" [F. K.]
            
            t2 = HI_LO(value, t2_latch_lo);
            clearInterruptFlag_T2();
            feed &= ~(VIAPostOneShotB0);
            return;
            
        case 0xA: // Shift register
            
            clearInterruptFlag_SR();
            sr = value;
            return;
            
        case 0xB: // Auxiliary control register
            
            acr = value;
            
            // TODO (Hoxs64)
            // if ((feed & VIAPostOneShotA0) != 0)
            // {
            //     bPB7Toggle = bPB7TimerMode ^ 0x80;
            // }
            
            if (acr & 0x20) {
                
                // In the pulse counting mode, T2 counts negative pulses on PB6,
                // so we disable automatic counting.
                
                delay &= ~(VIACountB0);
                feed &= ~VIACountB0;
                
            } else {
                
                // In the timed interrupt mode, T2 counts down every cycle.
                
                delay |= VIACountB0;
                feed |= VIACountB0;
            }
            
            if (acr & 0x80) {
                
                // Output shows up a port pin PB7
                pb7timerOut = pb7toggle;
            }
            return;
            
        case 0xC: // Peripheral control register
            
            pcr = value;
            return;
            
        case 0xD: // IFR - Interrupt Flag Register
            
            // Writing 1 will clear the corresponding bit
            ifr &= ~value;
            IRQ();
            return;
            
        case 0xE: // IER - Interrupt Enable Register
            
            // Bit 7 distinguishes between set and clear
            // If bit 7 is 1, writing 1 will set the corresponding bit
            // If bit 7 is 0, writing 1 will clear the corresponding bit
            if (value & 0x80) {
                ier |= value;
            } else {
                ier &= ~value;
            }
            ier &= 0x7F;
            IRQ();
            return;
            
        case 0xF:
            
            // Clear flags in interrupt flag register (IFR)
            clearInterruptFlag_CA1();
            if (shouldClearCA2onWrite())
                clearInterruptFlag_CA2();
            return;
    }
}

// -----------------------------------------------------------------------------------------------
//                                     Peek and Poke (VIA 1)
// -----------------------------------------------------------------------------------------------

uint8_t VIA1::peek(uint16_t addr)
{
    switch(addr) {
            
        case 0x0: { // ORB - Output register B
            
            (void)VIA6522::peek(addr);
            
            // |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   |
            // -----------------------------------------------------------------
            // |  ATN  | Device addr.  |  ATN  | Clock | Clock | Data  | Data  |
            // |  in   |               |  out  |  out  |  in   |  out  |  in   |
            
            // Port values (outside the chip)
            uint8_t external =
            (c64->floppy.iec->getAtnLine() /* 7 */ ? 0x00 : 0x80) |
            (c64->floppy.iec->getClockLine() /* 2 */ ? 0x00 : 0x04) |
            (c64->floppy.iec->getDataLine() /* 0 */ ? 0x00 : 0x01);
            
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
            if (shouldClearCA2onRead())
                clearInterruptFlag_CA2();

            // Clean this up ...
            c64->floppy.cpu.releaseIrqLine(CPU::ATN);
            return ora;
            
        default:
            return VIA6522::peek(addr);
    }
}

uint8_t VIA1::read(uint16_t addr)
{
    switch(addr) {
            
        case 0x0: { // ORB - Output register B
            
            // Port values (outside the chip)
            uint8_t external =
            (c64->floppy.iec->getAtnLine() /* 7 */ ? 0x00 : 0x80) |
            (c64->floppy.iec->getClockLine() /* 2 */ ? 0x00 : 0x04) |
            (c64->floppy.iec->getDataLine() /* 0 */ ? 0x00 : 0x01);
            
            // Determine read value
            uint8_t result =
            (ddrb & orb) |      // Values of bits configured as outputs
            (~ddrb & external); // Values of bits configured as inputs
            
            result &= 0x9F;
            return result;
        }
            
        case 0x1: // ORA - Output register A
        case 0xF:
            return ora;
            
        default:
            return VIA6522::read(addr);
    }
}

void VIA1::poke(uint16_t addr, uint8_t value)
{
	switch(addr) {

        case 0x0: // ORB - Output register B
            VIA6522::poke(addr, value);
 
            // |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   |
            // -----------------------------------------------------------------
            // |  ATN  | Device addr.  |  ATN  | Clock | Clock | Data  | Data  |
            // |  in   |               |  out  |  out  |  in   |  out  |  in   |

			orb = value;
			c64->floppy.iec->updateDevicePins(orb, ddrb);
			return;

		case 0x1: // ORA - Output register A
        case 0xF:
            VIA6522::poke(addr, value);
			ora = value;
			return;
		
		case 0x2:
			ddrb = value;
			c64->floppy.iec->updateDevicePins(orb, ddrb);
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
            
            (void)VIA6522::peek(addr);
            
            // |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   |
            // -----------------------------------------------------------------
            // | SYNC  | Timer control | Write |  LED  | Rot.  | Stepper motor |
            // |       | (4 disk zones)|protect|       | motor | (head move)   |
            
            // Collect values on the external port lines
            bool SYNC = c64->floppy.getSync();
            uint8_t external = (SYNC /* 7 */ ? 0x00 : 0x80) |
            (c64->floppy.getLightBarrier() /* 4 */ ? 0x00 : 0x10) |
            (c64->floppy.getRedLED() /* 3 */ ? 0x00 : 0x08) |
            (c64->floppy.isRotating() /* 2 */ ? 0x00 : 0x04);
            
            uint8_t result =
            (ddrb & orb) |      // Values of bits configured as outputs
            (~ddrb & external); // Values of bits configures as inputs
            
            return result;
        }
            
        case 0x1: // ORA - Output register A
        case 0xF: {
            
            uint8_t result;
            
            (void)VIA6522::peek(addr);
            
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
            return result;
        }
            
        case 0x4:
            return VIA6522::peek(addr);
            
        default:
            return VIA6522::peek(addr);
    }
}

uint8_t VIA2::read(uint16_t addr)
{
    switch(addr) {
            
        case 0x0: { // ORB - Output register B
            
            // Collect values on the external port lines
            bool SYNC = c64->floppy.getSync();
            uint8_t external = (SYNC /* 7 */ ? 0x00 : 0x80) |
            (c64->floppy.getLightBarrier() /* 4 */ ? 0x00 : 0x10) |
            (c64->floppy.getRedLED() /* 3 */ ? 0x00 : 0x08) |
            (c64->floppy.isRotating() /* 2 */ ? 0x00 : 0x04);
            
            uint8_t result =
            (ddrb & orb) |      // Values of bits configured as outputs
            (~ddrb & external); // Values of bits configures as inputs
            
            return result;
        }
            
        case 0x1: // ORA - Output register A
        case 0xF: {
            
            uint8_t result;
            
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
        
            return result;
        }
            
        default:
            return VIA6522::peek(addr);
    }
}

void VIA2::poke(uint16_t addr, uint8_t value)
{
	switch(addr) {

        case 0x0: { // ORB - Output register B
            
            VIA6522::poke(addr, value);
            
            // |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   |
            // -----------------------------------------------------------------
            // | SYNC  | Timer control | Write |  LED  | Rot.  | Stepper motor |
            // |       | (4 disk zones)|protect|       | motor | (head move)   |

            // Disable bits that are not configured as outputs
            value &= ddrb;
            
            // Bits 6 and 5
            c64->floppy.setZone((value >> 5) & 0x03);
            
            // Bit 3
            c64->floppy.setRedLED(GET_BIT(value,3));

            // Bit 2
            c64->floppy.setRotating(GET_BIT(value,2));

            // Bits 1 and 0
			if ((orb & 0x03) != (value & 0x03)) {
                
				// A decrease (00-11-10-01-00...) moves the the head down
				// An increase (00-01-10-11-00...) moves the head up
                
				if ((value & 0x03) == ((orb+1) & 0x03)) {
					c64->floppy.moveHeadUp();
				} else if ((value & 0x03) == ((orb-1) & 0x03)) {
					c64->floppy.moveHeadDown();
				} else {
					warn("Unexpected stepper motor control sequence in VC1541 detected\n");
				}
			}

			orb = value;
			return;
        }
            
		case 0x1: // ORA - Output register A
        case 0xF:
            
            VIA6522::poke(addr, value);
            
            // Hard wired to the data lines of the Gate Array (U10) (read/write head)
            // TODO: Take care of ddra

            ora = value;
			return;

		case 0x3:
			ddra = value;
			if (ddra != 0x00 && ddra != 0xFF) {
				debug(1, "Data direction bits of VC1541 contain suspicious values\n");
			}
			return;

        case 0xC:
            
            if (!(pcr & 0x20) && (value & 0x20)) {
                
                debug(2, "Switching to read mode mode\n");
            }
            if ((pcr & 0x20) && !(value & 0x20)) {
                
                debug(2, "Switching to write mode\n");
            }

            pcr = value;
            return;
            
		default:
			VIA6522::poke(addr, value);	
	}
}


VIA1::VIA1()
{
    setDescription("VIA1");
	debug(3, "  Creating VIA1 at address %p...\n", this);
}
	
VIA1::~VIA1()
{
	debug(3, "  Releasing VIA1...\n");
}

VIA2::VIA2()
{
    setDescription("VIA2");
	debug(3, "  Creating VIA2 at address %p...\n", this);
}
	
VIA2::~VIA2()
{
	debug(3, "  Releasing VIA2...\n");
}

void VIA2::debug0xC() {
    
    uint8_t value = pcr; 
    
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
     case 6: debug(2,"  LOW OUTPUT %04X\n", c64->floppy.cpu.getPC_at_cycle_0()); break;
     case 7: debug(2,"  HIGH OUTPUT %04X\n", c64->floppy.cpu.getPC_at_cycle_0()); break;
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
