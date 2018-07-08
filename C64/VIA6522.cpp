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

//
// VIA 6522 (Commons)
//

VIA6522::VIA6522()
{
	setDescription("VIA");
    
    // Register snapshot items
    SnapshotItem items[] = {
        { &pa,              sizeof(pa),             CLEAR_ON_RESET },
        { &ca1,             sizeof(ca1),            CLEAR_ON_RESET },
        { &ca1_prev,        sizeof(ca1_prev),       CLEAR_ON_RESET },
        { &ca2,             sizeof(ca2),            CLEAR_ON_RESET },
        { &ca2_prev,        sizeof(ca2_prev),       CLEAR_ON_RESET },
        { &ca2_out,         sizeof(ca2_out),        CLEAR_ON_RESET },
        { &pb,              sizeof(pb),             CLEAR_ON_RESET },
        { &cb1,             sizeof(cb1),            CLEAR_ON_RESET },
        { &cb1_prev,        sizeof(cb1_prev),       CLEAR_ON_RESET },
        { &cb2,             sizeof(cb2),            CLEAR_ON_RESET },
        { &cb2_prev,        sizeof(cb2_prev),       CLEAR_ON_RESET },
        { &cb2_out,         sizeof(cb2_out),        CLEAR_ON_RESET },
        { &ddra,            sizeof(ddra),           CLEAR_ON_RESET },
        { &ddrb,            sizeof(ddrb),           CLEAR_ON_RESET },
        { &ora,             sizeof(ora),            CLEAR_ON_RESET },
        { &orb,             sizeof(orb),            CLEAR_ON_RESET },
        { &ira,             sizeof(ira),            CLEAR_ON_RESET },
        { &irb,             sizeof(irb),            CLEAR_ON_RESET },
        { &t1,              sizeof(t1),             CLEAR_ON_RESET },
        { &t1_latch_lo,     sizeof(t1_latch_lo),    CLEAR_ON_RESET },
        { &t1_latch_hi,     sizeof(t1_latch_hi),    CLEAR_ON_RESET },
        { &t2,              sizeof(t2),             CLEAR_ON_RESET },
        { &t2_latch_lo,     sizeof(t2_latch_lo),    CLEAR_ON_RESET },
        { &pcr,             sizeof(pcr),            CLEAR_ON_RESET },
        { &acr,             sizeof(acr),            CLEAR_ON_RESET },
        { &ier,             sizeof(ier),            CLEAR_ON_RESET },
        { &ifr,             sizeof(ifr),            CLEAR_ON_RESET },
        { &newifr,          sizeof(newifr),         CLEAR_ON_RESET },
        { &sr,              sizeof(sr),             CLEAR_ON_RESET },
        { &delay,           sizeof(delay),          CLEAR_ON_RESET },
        { &feed,            sizeof(feed),           CLEAR_ON_RESET },
        { &tiredness,       sizeof(tiredness),      CLEAR_ON_RESET },
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
    feed = (VIACountA0 | VIACountB0);
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

//
// Execution functions
//

void
VIA6522::execute()
{
    wakeUp();
    
    uint64_t oldDelay = delay;
    uint64_t oldFeed  = feed;
    newifr = 0; // TODO: Make it a local variable (or is it used globally?)
    
    // Execute timers
    executeTimer1();
    executeTimer2();
    
    /*
    if (c64->floppy.cpu.tracingEnabled())
        debug("IFR: %02X IER: %02X Int0: %d Int1: %d INT line: %d\n", ifr, ier, (delay & VIAInterrupt0) != 0, (delay & VIAInterrupt1) != 0, (c64->floppy.cpu.irqLine & CPU::VIA) != 0);
    */
    
    // Check for interrupt condition
    ifr |= newifr;
    if (unlikely(newifr & ier)) {
        delay |= VIAInterrupt0;
    }
    
    // Trigger interrupt if requested
    if (unlikely(delay & VIAClrInterrupt1)) {
        releaseIrqLine();
    }
    if (unlikely(delay & VIAInterrupt1)) {
        pullDownIrqLine();
    }
    
    // Set or clear CA2 or CB2 if requested
    if (unlikely(delay & (VIASetCA2out1 | VIAClearCA2out1 | VIASetCB2out1 | VIAClearCB2out1))) {
        if (delay & VIASetCA2out1) { setCA2out(true); }
        if (delay & VIAClearCA2out1) { setCA2out(false); }
        if (delay & VIASetCB2out1) { setCB2out(true); }
        if (delay & VIAClearCB2out1) { setCB2out(false); }
    }
    
    // Simulate transitions on pins CA1, CA2, CB1, and CB2
    if (unlikely(delay & (VIACA1Trans1 | VIACA2Trans1 | VIACB1Trans1 | VIACB2Trans1))) {
        if (delay & (VIACA1Trans1)) { toggleCA1(); }
        if (delay & (VIACA2Trans1)) { toggleCA2(); }
        if (delay & (VIACB1Trans1)) { toggleCB1(); }
        if (delay & (VIACB2Trans1)) { toggleCB2(); }
    }
    
    // Set V flag if needed
    /*
    if (unlikely(delay & VIASetVFlag1)) {
        c64->floppy.cpu.setV(1);
    }
    */
    
    // Move trigger event flags left and feed in new bits
    delay = ((delay << 1) & VIAClearBits) | feed;
    
    // Go into idle state if possible
    if (oldDelay == delay && oldFeed == feed) {
        if (++tiredness > 4) {
            sleep();
            tiredness = 0;
        }
    } else {
        tiredness = 0;
    }
}

void
VIA6522::executeTimer1()
{
    // Reload counter
    if (delay & VIAReloadA2) {
         t1 = HI_LO(t1_latch_hi, t1_latch_lo);
    }
    
    // Decrement counter
    else if (delay & VIACountA1) {
        t1--;
    }
    
    // Check for timer underflow
    if (t1 == 0) {
        
        // Reload counter in 2 cycles
        delay |= VIAReloadA0;
        
        // Check if an interrupt should be triggered
        if (!(feed & VIAPostOneShotA0)) {

            // Set interrupt flag
            SET_BIT(newifr, 6);
            
            // Toggle PB7 output bit
            feed ^= VIAPB7out0;
        }
        
        // Prevent further interrupts in one-shot mode
        if (!freeRun()) {
            feed |= VIAPostOneShotA0;
        }
    }
}

void
VIA6522::executeTimer2()
{
    // Decrement counter
    if (delay & VIACountB1) {
        t2--;
    }
    
    // Check for timer underflow
    if (t2 == 0 && (delay & VIACountB0)) {

        // Check if an interrupt should be triggered
        if (!(delay & VIAPostOneShotB0)) {
            
            // Set interrupt flag
            SET_BIT(newifr,5);
            
            // Prevent further interrupts
            feed |= VIAPostOneShotB0;
        }
    }
}


//
// Peeking and poking
//

uint8_t 
VIA6522::peek(uint16_t addr)
{
	assert (addr <= 0xF);
		
    wakeUp();
    
	switch(addr) {
            
        case 0x0: // ORB - Output register B
            return peekORB();
            
        case 0x1: // ORA - Output register A
            return peekORA(true);
            
        case 0x2: // DDRB - Data direction register B
			return ddrb;

        case 0x3: // DDRA - Data direction register A
			return ddra;
			
        case 0x4: // T1 low-order counter
            
            // "8 BITS FROM T1 LOW-ORDER COUNTER TRANSFERRED TO MPU. IN ADDITION T1 INTERRUPT FLAG
            //  IS RESET (BIT 6 IN INTERRUPT FLAG REGISTER)" [F. K.]
            
            clearInterruptFlag_T1();
            releaseIrqLineIfNeeded();
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
            releaseIrqLineIfNeeded();
			return LO_BYTE(t2);
			
		case 0x9: // T2 high-order counter COUNTER TRANSFERRED TO MPU" [F. K.]
            
            // "8 BITS FROM T2 HIGH-ORDER
			return HI_BYTE(t2);
            
        case 0xA: // Shift register

            clearInterruptFlag_SR();
            releaseIrqLineIfNeeded();
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

        case 0xF: // ORA - Output register A (no handshake)
            return peekORA(false);
    }

    assert(0);
    return 0;
}

uint8_t
VIA6522::peekORA(bool handshake)
{
    clearInterruptFlag_CA1();

    uint8_t CA2control = (pcr >> 1) & 0x07; // ----xxx-
    
    switch (CA2control) {
        case 0: // Input mode: Interrupt on negative edge
            clearInterruptFlag_CA2();
            break;
        case 1: // Input mode: Interrupt on negative edge, no register clearance
            break;
        case 2: // Input mode: Interrupt on positive edge
            clearInterruptFlag_CA2();
            break;
        case 3: // Input mode: Interrupt on positive edge, no register clearance
            break;
        case 4: // Handshake output mode
                // Set CA2 output low on a read or write of the Peripheral A Output
                // Register. Reset CA2 high with an active transition on CAl.
            clearInterruptFlag_CA2();
            if (handshake) delay |= VIAClearCA2out1;
            break;
        case 5: // Pulse output mode
                // CA2 goes low for one cycle following a read or write of the
                // Peripheral A Output Register.
            clearInterruptFlag_CA2();
            if (handshake) delay |= VIAClearCA2out1 | VIASetCA2out0;
            break;
        case 6: // Manual output mode (keep line low)
            break;
        case 7: // Manual output mode (keep line low)
            break;
    }
    
    releaseIrqLineIfNeeded();
    
    // Update processor port
    updatePA();
    
    // Update input register
    if (!inputLatchingEnabledA()) {
        ira = pa;
    }
    return ira;
}

uint8_t
VIA6522::peekORB()
{
    clearInterruptFlag_CB1();
    
    uint8_t CB2control = (pcr >> 5) & 0x07; // xxx-----
    
    switch (CB2control) {
        case 0: // Input mode: Interrupt on negative edge
            clearInterruptFlag_CB2();
            break;
        case 1: // Input mode: Interrupt on negative edge, no register clearance
            break;
        case 2: // Input mode: Interrupt on positive edge
            clearInterruptFlag_CB2();
            break;
        case 3: // Input mode: Interrupt on positive edge, no register clearance
            break;
        case 4: // Handshake output mode
                // In contrast to CA2, CB2 is only affected on write accesses.
            break;
        case 5: // Pulse output mode
                // In contrast to CA2, CB2 is only affected on write accesses.
            break;
        case 6: // Manual output mode (keep line low)
            break;
        case 7: // Manual output mode (keep line low)
            break;
    }
    
    releaseIrqLineIfNeeded();

    // Update processor port
    updatePB();
    
    // Update input register
    if (!inputLatchingEnabledB()) {
        irb = pb;
    }
    return irb;
}

uint8_t
VIA6522::spypeek(uint16_t addr)
{
    assert (addr <= 0xF);
    
    // spypeek is not functional yet, because the VIA counter values are
    // wrong if the chip is in idle state. Fix this before using this function.
    // Look at CIA::spypeek to see how this can be done.
    assert(false);
    
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
    
    wakeUp();
    
    switch(addr) {
            
        case 0x0: // ORB - Output register B
            
            pokeORB(value);
            return;

        case 0x1: // ORA - Output register A
            
            pokeORA(value, true);
            return;
            
        case 0x2: // DDRB - Data direction register B
            
            // "0"  ASSOCIATED PB PIN IS AN INPUT (HIGH IMPEDANCE)
            // "1"  ASSOCIATED PB PIN IS AN OUTPUT WHOSE LEVEL IS DETERMINED BY ORB REGISTER BIT" [F. K.]
            ddrb = value;
            updatePB();
            return;
            
        case 0x3: // DDRA - Data direction register A
            
            // "0"  ASSOCIATED PB PIN IS AN INPUT (HIGH IMPEDANCE)
            // "1"  ASSOCIATED PB PIN IS AN OUTPUT WHOSE LEVEL IS DETERMINED BY ORA REGISTER BIT" [F. K.]
            
            ddra = value;
            updatePA();
            return;
            
        case 0x4: // T1L-L (write) / T1C-L (read)
            
            // (1) Write low order latch.
            
            t1_latch_lo = value; // (1)
            return;
            
        case 0x5: // T1C-H (read and write)
            
            // Write into high order latch.
            t1_latch_hi = value;
            
            // Write into high order counter
            // and transfer low order latch into low order counter.
            t1 = HI_LO(value, t1_latch_lo);
            
            // Reset T1 interrupt flag.
            clearInterruptFlag_T1();
            releaseIrqLineIfNeeded();
            
            // If ACR7 = 1, a "write T1C-H" operation will cause PB7 to go low.
            if (PB7OutputEnabled()) {
                feed ^= VIAPB7out0;
            }
            
            delay |= VIAReloadA2;
            feed &= ~(VIAPostOneShotA0);
            delay &= ~(VIAPostOneShotA0);
            delay &= ~(VIACountA1 | VIAReloadA1);
            updatePB();
            return;
            
        case 0x6: // T1L-L (read and write)
            
            // Write low order latch.
            t1_latch_lo = value;
            
            return;
            
        case 0x7: // T1L-H (read and write)
            
            // Write high order latch.
            t1_latch_hi = value;
            
            // (2) Reset Tl interrupt flag.
            clearInterruptFlag_T1();
            releaseIrqLineIfNeeded();
            return;
            
        case 0x8: // T2L-L (write) / T2C-L (read)
            
            t2_latch_lo = value;
            return;
            
        case 0x9: // T2C-H
            
            t2 = HI_LO(value, t2_latch_lo);
            clearInterruptFlag_T2();
            releaseIrqLineIfNeeded();
            feed &= ~(VIAPostOneShotB0);
            delay &= ~(VIAPostOneShotB0);
            delay &= ~(VIACountB1 | VIAReloadB1);
            return;
            
        case 0xA: // Shift register
            
            clearInterruptFlag_SR();
            releaseIrqLineIfNeeded();
            sr = value;
            return;
            
        case 0xB: // Auxiliary control register
            
            acr = value;
            
            if (acr & 0x20) {
                
                // In pulse counting mode, T2 counts negative pulses on PB6,
                // so we disable automatic counting.
                delay &= ~(VIACountB0);
                feed &= ~VIACountB0;
                
            } else {
                
                // In timed interrupt mode, T2 counts down every cycle.
                delay |= VIACountB0;
                feed |= VIACountB0;
            }
            
            if (acr & 0x80) {
                // Output shows up at port pin PB7, starting with '1'.
                feed |= VIAPB7out0;
            }
            updatePB();
            return;
            
        case 0xC: // Peripheral control register
            
            pokePCR(value);
            return;
            
        case 0xD: // IFR - Interrupt Flag Register
            
            // Writing 1 will clear the corresponding bit
            ifr &= ~value;
            // IRQ();
            if (ifr & ier) {
                pullDownIrqLine();
            } else {
                releaseIrqLine();
            }
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
            // IRQ();
            if (ifr & ier) {
                pullDownIrqLine();
            } else {
                releaseIrqLine();
            }
            return;
            
        case 0xF: // ORA - Output register A (no handshake)
            
            pokeORA(value, false);
            return;
    }
}

void
VIA6522::pokeORA(uint8_t value, bool handshake)
{
    clearInterruptFlag_CA1();
    
    // Take care of side effects
    switch ((pcr >> 1) & 0x07) {
        case 0: // Input mode: Interrupt on negative edge
            clearInterruptFlag_CA2();
            break;
        case 1: // Input mode: Interrupt on negative edge, no register clearance
            break;
        case 2: // Input mode: Interrupt on positive edge
            clearInterruptFlag_CA2();
            break;
        case 3: // Input mode: Interrupt on positive edge, no register clearance
            break;
        case 4: // Handshake output mode
            clearInterruptFlag_CA2();
            if (handshake) delay |= VIAClearCA2out1;
            break;
        case 5: // Pulse output mode
            clearInterruptFlag_CA2();
            if (handshake) delay |= VIAClearCA2out1 | VIASetCA2out0;
            break;
        case 6: // Manual output mode (keep line low)
            break;
        case 7: // Manual output mode (keep line low)
            break;
    }
    
    releaseIrqLineIfNeeded();
    ora = value;
    updatePA();
}

void
VIA6522::pokeORB(uint8_t value)
{
    clearInterruptFlag_CB1();
        
    // Take care of side effects
    switch ((pcr >> 5) & 0x07) {
        case 0: // Input mode: Interrupt on negative edge
            clearInterruptFlag_CB2();
            break;
        case 1: // Input mode: Interrupt on negative edge, no register clearance
            break;
        case 2: // Input mode: Interrupt on positive edge
            clearInterruptFlag_CB2();
            break;
        case 3: // Input mode: Interrupt on positive edge, no register clearance
            break;
        case 4: // Handshake output mode
            clearInterruptFlag_CB2();
            delay |= VIAClearCB2out1;
            break;
        case 5: // Pulse output mode
            clearInterruptFlag_CB2();
            delay |= VIAClearCB2out1 | VIAClearCB2out0;
            break;
        case 6: // Manual output mode (keep line low)
            break;
        case 7: // Manual output mode (keep line low)
            break;
    }
    
    releaseIrqLineIfNeeded();
    orb = value;
    updatePB();
}

void
VIA6522::pokePCR(uint8_t value)
{
    pcr = value;
    
    // Check CA2 control bits
    switch(ca2Control()) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            setCA2out(true);
            break;
        case 6: // Hold CA2 low
            setCA2out(false);
            break;
        case 7: // Hold CA2 high
            setCA2out(true);
            break;
    }
    
    // Check CB2 control bits
    switch(cb2Control()) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            setCB2out(true);
            break;
        case 6: // Hold CB2 low
            setCB2out(false);
            break;
        case 7: // Hold CB2 high
            setCB2out(true);
            break;
    }
}

uint8_t
VIA6522::portAinternal()
{
    return ora;
}

void
VIA6522::updatePA()
{
    pa = (portAinternal() & ddra) | (portAexternal() & ~ddra);
}

uint8_t
VIA6522::portBinternal()
{
    return orb;
}

void
VIA6522::updatePB()
{
    pb = (portBinternal() & ddrb) | (portBexternal() & ~ddrb);
    
    if (PB7OutputEnabled()) {
        if (feed & VIAPB7out0) {
            SET_BIT(pb, 7);
        } else {
            CLR_BIT(pb, 7);
        }
    }
}

void
VIA6522::toggleCA1()
{
    // Check for active transition (positive or negative edge)
    uint8_t ctrl = ca1Control();
    bool active = (ca1_prev && ctrl == 0) || (!ca1_prev && ctrl == 1);
    ca1_prev = !ca1_prev;
    
    if (!ca1_prev)
        CA1LowAction();
    
    if (!active)
        return;
    
    // Set interrupt flag
    setInterruptFlag_CA1();
    if (GET_BIT(ier, 1)) {
        delay |= VIAInterrupt1;
    }
    
    // Latch peripheral port into input register if latching is enabled
    if (inputLatchingEnabledA()) {
        updatePA();
        ira = pa;
    }
    
    // Check for handshake mode with CA2
    if (ca2Control() == 4) {
        setCA2out(true);
    }
}

void
VIA6522::toggleCA2()
{
    // Check for active transition (positive or negative edge)
    uint8_t ctrl = ca2Control();
    bool active = (ca2_prev && (ctrl == 0 || ctrl == 1)) || (!ca2_prev && (ctrl == 2 || ctrl == 3));
    
    ca2_prev = !ca2_prev;
    
    if (!active)
        return;
    
    // Set interrupt flag
    setInterruptFlag_CA2();
    if (GET_BIT(ier, 0)) {
        delay |= VIAInterrupt1;
    }
}

void
VIA6522::toggleCB1()
{
    // Check for active transition (positive or negative edge)
    uint8_t ctrl = cb1Control();
    bool active = (cb1_prev && ctrl == 0) || (!cb1_prev && ctrl == 1);
    cb1_prev = !cb1_prev;
    
    if (!ca1_prev)
        CA1LowAction();
    
    if (!active)
        return;
    
    // Set interrupt flag
    setInterruptFlag_CB1();
    if (GET_BIT(ier, 4)) {
        delay |= VIAInterrupt1;
    }
    
    // Latch peripheral port into input register if latching is enabled
    if (inputLatchingEnabledB()) {
        updatePB();
        irb = pb;
    }
    
    // Check for handshake mode with CB2
    if (cb2Control() == 0x4)
        setCB2out(true);
}

void
VIA6522::toggleCB2()
{
    // Check for active transition
    uint8_t ctrl = cb2Control();
    bool active = (cb2_prev && (ctrl == 0 || ctrl == 1)) || (!cb2_prev && (ctrl == 2 || ctrl == 3));
    
    cb2_prev = !cb2_prev;
    
    if (!active)
        return;
    
    // Set interrupt flag
    setInterruptFlag_CB2();
    if (GET_BIT(ier, 3)) {
        delay |= VIAInterrupt1;
    }
}




/*
void
VIA6522::setCA1(bool value)
{
    if (ca1 == value) return;
    
    ca1 = value;

    // Check for active transition (positive or negative edge)
    uint8_t ctrl = ca1Control();
    bool active = (!ca1 && ctrl == 0) || (ca1 && ctrl == 1);
    if (!active) return;
    
    // Set interrupt flag
    setInterruptFlag_CA1();
    if (GET_BIT(ier, 1)) {
        delay |= VIAInterrupt1;
    }
    
    // Latch peripheral port into input register if latching is enabled
    if (inputLatchingEnabledA()) {
        updatePA();
        // debug("LATCH IN VALUE ca1 = %d PA = %02X readshift = %02X\n", ca1, pa, c64->floppy.readShiftreg);
        ira = pa;
    }
    
    // Check for handshake mode with CA2
    if (ca2Control() == 0x4) {
        // debug("HANDSHAKING\n");
        setCA2out(true);
    }
}
*/
void
VIA6522::setCA1early(bool value)
{
    assert((delay & VIACA1Trans1) == 0);
    
    wakeUp();
    
    delay |= VIACA1Trans1;
    /*
    if (ca1_prev != value) {
        delay |= VIACA1Trans1;
    } else {
        delay &= ~VIACA1Trans1;
    }
    */
}

void
VIA6522::setCA1late(bool value)
{
    wakeUp();
    
    uint8_t next = (delay & VIACA1Trans1) ? !ca1_prev : ca1_prev;
    if (next != value) {
        delay |= VIACA1Trans0;
    } else {
        delay &= ~VIACA1Trans0;
    }
}

/*
void
VIA6522::setCA2(bool value)
{
    if (ca2 == value) return;
 
    ca2 = value;
    
    // Check for active transition (positive or negative edge)
    uint8_t ctrl = ca2Control();
    bool active = (!ca2 && (ctrl == 0 || ctrl == 1)) || (ca2 && (ctrl == 2 || ctrl == 3));
    if (!active) return;
    
    // Set interrupt flag
    setInterruptFlag_CA2();
    if (GET_BIT(ier, 0)) {
        delay |= VIAInterrupt1;
    }
}
*/

/*
void
VIA6522::setCB1(bool value)
{
    if (cb1 == value) return;
    
    cb1 = value;
    
    // Check for active transition (positive or negative edge)
    uint8_t ctrl = cb1Control();
    bool active = (!cb1 && ctrl == 0) || (cb1 && ctrl == 1);
    if (!active) return;
    
    // Set interrupt flag
    setInterruptFlag_CB1();
    if (GET_BIT(ier, 4)) {
        delay |= VIAInterrupt1;
    }
    
    // Latch peripheral port into input register if latching is enabled
    if (inputLatchingEnabledB()) {
        updatePB();
        irb = pb;
    }
    
    // Check for handshake mode with CB2
    if (cb2Control() == 0x4)
        setCB2out(true);
}
*/

/*
void
VIA6522::setCB2(bool value)
{
    if (cb2 == value) return;
    
    cb2 = value;
    
    // Check for active transition (positive or negative edge)
    uint8_t ctrl = cb2Control();
    bool active = (!cb2 && (ctrl == 0 || ctrl == 1)) || (cb2 && (ctrl == 2 || ctrl == 3));
    if (!active) return;
    
    // Set interrupt flag
    setInterruptFlag_CB2();
    if (GET_BIT(ier, 3)) {
        delay |= VIAInterrupt1;
    }
}
*/

void
VIA6522::setCA2out(bool value)
{
    ca2_out = value;
}

void
VIA6522::setCB2out(bool value)
{
    cb2_out = value;
}

void
VIA6522::sleep()
{
    assert(idleCounter() == 0);
    
    // Determine maximum possible sleep cycles based on timer counts
    uint64_t sleepA = (t1 > 2) ? (c64->floppy.cpu.getCycle() + t1 - 1) : 0;
    uint64_t sleepB = (t2 > 2) ? (c64->floppy.cpu.getCycle() + t2 - 1) : 0;
    
    // VIAs with stopped timers can sleep forever
    if (!(feed & CountA0)) sleepA = UINT64_MAX;
    if (!(feed & CountB0)) sleepB = UINT64_MAX;
    
    // uint64_t c = c64->floppy.cpu.getCycle();
    // debug("VIA can sleep for %d cycles (MIN(%d,%d)\n", MIN(sleepA, sleepB) - c, sleepA - c, sleepB - c);
    setWakeUpCycle(MIN(sleepA, sleepB));
}

void
VIA6522::wakeUp()
{
    uint64_t idleCycles = idleCounter();
    
    // Make up for missed cycles
    if (idleCycles) {
        // debug("Making up %d cycles\n", idleCycles);
        if (feed & CountA0) {
            assert(t1 >= idleCycles);
            t1 -= idleCycles;
        }
        if (feed & CountB0) {
            assert(t2 >= idleCycles);
            t2 -= idleCycles;
        }
        resetIdleCounter();
    }
    setWakeUpCycle(0);
}


//
// VIA 1
//

VIA1::VIA1()
{
    setDescription("VIA1");
    debug(3, "  Creating VIA1 at address %p...\n", this);
}

VIA1::~VIA1()
{
    debug(3, "  Releasing VIA1...\n");
}

void
VIA1::pullDownIrqLine() {
    c64->floppy.cpu.pullDownIrqLine(CPU::INTSRC_VIA1);
}

void
VIA1::releaseIrqLine() {
    c64->floppy.cpu.releaseIrqLine(CPU::INTSRC_VIA1);
}

/*
uint8_t
VIA1::portAinternal()
{
    return getORA();
}
*/

uint8_t
VIA1::portAexternal()
{
    return 0xFF;
}

uint8_t
VIA1::portBexternal()
{
    // |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   |
    // -----------------------------------------------------------------
    // |  ATN  | Device addr.  |  ATN  | Clock | Clock | Data  | Data  |
    // |  in   |               |  ack  |  out  |  in   |  out  |  in   |
    
    uint8_t external =
    (c64->iec.atnLine ? 0x00 : 0x80) |
    (c64->iec.clockLine ? 0x00 : 0x04) |
    (c64->iec.dataLine ? 0x00 : 0x01);
    
    external |= 0x1A; // All "out" pins are read as 1
    external &= 0x9F; // Device address 8
    
    return external;
}

void
VIA1::updatePB()
{
    VIA6522::updatePB();
    // c64->iec.updateDevicePins(orb, ddrb);

    // Mark IEC bus as dirty
    c64->iec.setNeedsUpdate();
}

uint64_t
VIA1::wakeUpCycle()
{
    return c64->floppy.wakeUpCycleVIA1;
}

void
VIA1::setWakeUpCycle(uint64_t cycle)
{
    c64->floppy.wakeUpCycleVIA1 = cycle;
}

uint64_t
VIA1::idleCounter()
{
    return c64->floppy.idleCounterVIA1;
}

void
VIA1::resetIdleCounter()
{
    c64->floppy.idleCounterVIA1 = 0;
}


//
// VIA 2
// 

VIA2::VIA2()
{
    setDescription("VIA2");
	debug(3, "  Creating VIA2 at address %p...\n", this);
}
	
VIA2::~VIA2()
{
	debug(3, "  Releasing VIA2...\n");
}

/*
uint8_t
VIA2::portAinternal()
{
    return getORA();
}
*/

uint8_t
VIA2::portAexternal()
{
    // TODO: Which value is returned in write mode?
    return c64->floppy.readShiftreg & 0xFF;
}

uint8_t
VIA2::portBexternal()
{
    bool sync     = c64->floppy.getSync();
    bool barrier  = c64->floppy.getLightBarrier();

    return (sync ? 0x80 : 0x00) | (barrier ? 0x00 : 0x10) | 0x6F;
}

void
VIA2::updatePB()
{
    uint8_t oldPb = getPB();
    VIA6522::updatePB();
    uint8_t newPb = getPB();
    
    // |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   |
    // -----------------------------------------------------------------
    // | SYNC  | Timer control | Write |  LED  | Rot.  | Stepper motor |
    // |       | (4 disk zones)|protect|       | motor | (head move)   |
    
    // Bits 6 and 5
    if ((newPb & 0x60) != (oldPb & 0x60))
        c64->floppy.setZone((newPb >> 5) & 0x03);
    
    // Bit 3
    if (GET_BIT(newPb, 3) != GET_BIT(oldPb, 3))
        c64->floppy.setRedLED(GET_BIT(newPb, 3));
    
    // Bit 2
    if (GET_BIT(newPb, 2) != GET_BIT(oldPb, 2))
        c64->floppy.setRotating(GET_BIT(newPb, 2));
    
    // Head stepper motor
    
    // Bits 1 and 0
    /*
    if ((pb & 0x03) != (oldPb & 0x03)) {
        
        // A  decrease (00-11-10-01-00...) moves the head down
        // An increase (00-01-10-11-00...) moves the head up
        
        if ((pb & 0x03) == ((oldPb + 1) & 0x03)) {
            c64->floppy.moveHeadUp();
        } else if ((pb & 0x03) == ((oldPb - 1) & 0x03)) {
            c64->floppy.moveHeadDown();
        } else {
            warn("Unexpected stepper motor control sequence\n");
        }
    }
    */
    
    if (newPb & 0x04) { // We only move the head if the motor is on
        
        // Relationship between halftracks and stepper positions:
        //
        // Halftrack number: 01  02  03  04  05  06  07  08 ...
        // Stepper position:  0   1   2   3   0   1   2   3 ...
        
        int oldPos = (int)((c64->floppy.getHalftrack() - 1) & 0x03);
        int newPos = (int)(newPb & 0x03);
        
        if (newPos != oldPos) {
            if (newPos == ((oldPos + 1) & 0x03)) {
                c64->floppy.moveHeadUp();
                // assert(newPos == ((c64->floppy.getHalftrack() - 1) & 0x03));
            } else if (newPos == ((oldPos - 1) & 0x03)) {
                c64->floppy.moveHeadDown();
                // assert(newPos == ((c64->floppy.getHalftrack() - 1) & 0x03));
            } else {
                debug(2, "Unexpected stepper motor control sequence\n");
            }
        }
    }
}

void
VIA2::CA1LowAction()
{
    // delay |= VIASetVFlag1;
    c64->floppy.cpu.setV(1);
}

void
VIA2::pullDownIrqLine() {
    c64->floppy.cpu.pullDownIrqLine(CPU::INTSRC_VIA2);
}

void
VIA2::releaseIrqLine() {
    c64->floppy.cpu.releaseIrqLine(CPU::INTSRC_VIA2);
}

uint64_t
VIA2::wakeUpCycle()
{
    return c64->floppy.wakeUpCycleVIA2;
}

void
VIA2::setWakeUpCycle(uint64_t cycle)
{
    c64->floppy.wakeUpCycleVIA2 = cycle;
}

uint64_t
VIA2::idleCounter()
{
    return c64->floppy.idleCounterVIA2;
}

void
VIA2::resetIdleCounter()
{
    c64->floppy.idleCounterVIA2 = 0;
}


