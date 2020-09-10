// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "C64.h"

//
// VIA 6522 (Commons)
//

VIA6522::VIA6522(C64 &ref, Drive &drvref) : C64Component(ref), drive(drvref)
{
	setDescription("VIA");
}
	
void VIA6522::_reset()
{    
    RESET_SNAPSHOT_ITEMS

    t1 = 0x01AA;
    t2 = 0x01AA;
    t1_latch_hi = 0x01;
    t1_latch_lo = 0x05; // Makes "drive/defaults.prg" happy
    t2_latch_lo = 0xAA;
    feed = (VIACountA0 | VIACountB0);
}

void 
VIA6522::_dump()
{
    const char *latchingA = inputLatchingEnabledA() ? "enabled" : "disabled";
    const char *latchingB = inputLatchingEnabledB() ? "enabled" : "disabled";
    u16 t1Latch = LO_HI(t1_latch_lo, t1_latch_hi);
    u16 t2Latch = LO_HI(t2_latch_lo, 0);

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
    msg("                 Input latching A : %s\n", latchingA);
	msg("                 Input latching B : %s\n", latchingB);
	msg("                          Timer 1 : %d (latched: %d)\n", t1, t1Latch);
	msg("                          Timer 2 : %d (latched: %d)\n", t2, t2Latch);
	msg("                        IO memory : ");
	msg("\n");
}

bool
VIA6522::isVia2()
{
    return this == &drive.via2;
}


//
// Execution functions
//

void
VIA6522::execute()
{
    wakeUp();
    
    u64 oldDelay = delay;
    u64 oldFeed  = feed;
    
    // Execute timers
    executeTimer1();
    executeTimer2();
    
    // Trigger interrupt if requested
    if (unlikely(delay & VIAClrInterrupt1)) {
        releaseIrqLine();
    }
    if (unlikely(delay & VIAInterrupt1)) {
        pullDownIrqLine();
    }
    
    // Set or clear CA2 or CB2 if requested
    if (unlikely(delay & (VIASetCA1out1 | VIAClearCA1out1 | VIASetCA2out1 | VIAClearCA2out1 | VIASetCB2out1 | VIAClearCB2out1))) {
        if (delay & VIASetCA1out1) { setCA1(true); }
        if (delay & VIAClearCA1out1) { setCA1(false); }
        if (delay & VIASetCA2out1) { ca2 = true; }
        if (delay & VIAClearCA2out1) { ca2 = false; }
        if (delay & VIASetCB2out1) { cb2 = true; }
        if (delay & VIAClearCB2out1) { cb2 = false; }
    }
        
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
            setInterruptFlag_T1();
            
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
            setInterruptFlag_T2();
            
            // Prevent further interrupts
            feed |= VIAPostOneShotB0;
        }
    }
}


//
// Peeking and poking
//

u8 
VIA6522::peek(u16 addr)
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

        case 0xF: // ORA - Output register A (no handshake)
            return peekORA(false);
            
        default:
            assert(false);
    }

    return 0;
}

u8
VIA6522::peekORA(bool handshake)
{
    clearInterruptFlag_CA1();

    u8 CA2control = (pcr >> 1) & 0x07; // ----xxx-
    
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
    
    // Update processor port
    updatePA();
    
    // Update input register
    if (!inputLatchingEnabledA()) {
        ira = pa;
    }
    return ira;
}

u8
VIA6522::peekORB()
{
    clearInterruptFlag_CB1();
    
    u8 CB2control = (pcr >> 5) & 0x07; // xxx-----
    
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

    // Update processor port
    updatePB();
    
    // Update input register
    if (!inputLatchingEnabledB()) {
        irb = pb;
    }
    return irb;
}

u8
VIA6522::spypeek(u16 addr)
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
            
            u8 ioD = ifr & 0x7F;
            u8 irq = (ifr & ier) ? 0x80 : 0x00;
            return ioD | irq;
        }
  
        default:
            return peek(addr);
    }
    
    return 0;
}

void VIA6522::poke(u16 addr, u8 value)
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
            return;
            
        case 0x8: // T2L-L (write) / T2C-L (read)
            
            t2_latch_lo = value;
            return;
            
        case 0x9: // T2C-H
            
            t2 = HI_LO(value, t2_latch_lo);
            clearInterruptFlag_T2();
            feed &= ~(VIAPostOneShotB0);
            delay &= ~(VIAPostOneShotB0);
            delay &= ~(VIACountB1 | VIAReloadB1);
            return;
            
        case 0xA: // Shift register
            
            clearInterruptFlag_SR();
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
VIA6522::pokeORA(u8 value, bool handshake)
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
    
    ora = value;
    updatePA();
}

void
VIA6522::pokeORB(u8 value)
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
    
    orb = value;
    updatePB();
}

void
VIA6522::pokePCR(u8 value)
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
            ca2 = true;
            break;
        case 6: // Hold CA2 low
            ca2 = false;
            break;
        case 7: // Hold CA2 high
            ca2 = true;
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
            cb2 = true;
            break;
        case 6: // Hold CB2 low
            cb2 = false;
            break;
        case 7: // Hold CB2 high
            cb2 = true;
            break;
    }
}

u8
VIA6522::portAinternal()
{
    return ora;
}

void
VIA6522::updatePA()
{
    pa = (portAinternal() & ddra) | (portAexternal() & ~ddra);
}

u8
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

/*
void
VIA6522::toggleCA1()
{
    // Check for active transition (positive or negative edge)
    u8 ctrl = ca1Control();
    bool active = (ca1 && ctrl == 0) || (!ca1 && ctrl == 1);
    ca1 = !ca1;
    
    if (!ca1)
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
        ca2 = true;
    }
}
*/

void
VIA6522::setCA1(bool value)
{
    if (ca1 == value)
        return;
 
    ca1 = value;

    // VIA2 sets the V flag on a negative transition
    if (!value && isVia2()) drive.cpu.setV(1);
    
    // Check for active transition (can be positive or negative)
    u8 ctrl = ca1Control();
    bool active = (!ca1 && ctrl == 0) || (ca1 && ctrl == 1);
    if (!active) return;
    
    // Set interrupt flag
    SET_BIT(ifr, 1);
    if (GET_BIT(ier, 1)) {
        delay |= VIAInterrupt0;
    }
    
    // Latch peripheral port into input register if latching is enabled
    if (inputLatchingEnabledA()) {
        updatePA();
        ira = pa;
    }
    
    // Check for handshake mode with CA2
    if (ca2Control() == 4) {
        ca2 = true;
    }
}

void
VIA6522::CA1action(bool value)
{
    wakeUp();
    
    if (value) {
        // delay |= VIASetCA1out0;
        delay |= VIASetCA1out1;
    } else {
        // delay |= VIAClearCA1out0;
        delay |= VIAClearCA1out1;
    }
}

void
VIA6522::sleep()
{
    assert(idleCounter == 0);
    
    // Determine maximum possible sleep cycles based on timer counts
    u64 sleepA = (t1 > 2) ? (drive.cpu.cycle + t1 - 1) : 0;
    u64 sleepB = (t2 > 2) ? (drive.cpu.cycle + t2 - 1) : 0;
    
    // VIAs with stopped timers can sleep forever
    if (!(delay & VIACountA1)) sleepA = UINT64_MAX;
    if (!(delay & VIACountB1)) sleepB = UINT64_MAX;
    
    wakeUpCycle = MIN(sleepA, sleepB);
}

void
VIA6522::wakeUp()
{
    u64 idleCycles = idleCounter;
    
    // Make up for missed cycles
    if (idleCycles) {
        if (delay & VIACountA1) {
            assert((delay & (VIACountA0)) != 0);
            assert((feed & (VIACountA0)) != 0);
            assert(t1 > idleCycles);
            t1 -= idleCycles;
        } else {
            assert((delay & (VIACountA0)) == 0);
            assert((feed & (VIACountA0)) == 0);
        }
        if (delay & VIACountB1) {
            assert((delay & (VIACountB0)) != 0);
            assert((feed & (VIACountB0)) != 0);
            assert(t2 > idleCycles);
            t2 -= idleCycles;
        } else {
            assert((delay & (VIACountB0)) == 0);
            assert((feed & (VIACountB0)) == 0);
        }
        idleCounter = 0;
    }
    wakeUpCycle = 0;
}


//
// VIA 1
//

VIA1::VIA1(C64 &ref, Drive &drvref) : VIA6522(ref, drvref)
{
    setDescription("VIA1");
}

VIA1::~VIA1()
{
}

void
VIA1::pullDownIrqLine() {
    drive.cpu.pullDownIrqLine(INTSRC_VIA1);
}

void
VIA1::releaseIrqLine() {
    drive.cpu.releaseIrqLine(INTSRC_VIA1);
}

u8
VIA1::portAexternal()
{
    return 0xFF;
}

u8
VIA1::portBexternal()
{
    // |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   |
    // -----------------------------------------------------------------
    // |  ATN  | Device addr.  |  ATN  | Clock | Clock | Data  | Data  |
    // |  in   |               |  ack  |  out  |  in   |  out  |  in   |
    
    u8 external =
    (iec.atnLine ? 0x00 : 0x80) |
    (iec.clockLine ? 0x00 : 0x04) |
    (iec.dataLine ? 0x00 : 0x01);
    
    external |= 0x1A; // All "out" pins are read as 1
    
    // Assign device address
    if (drive.getDeviceNr() == DRIVE9) external |= 0x20;
    
    return external;
}

void
VIA1::updatePB()
{
    VIA6522::updatePB();
    iec.setNeedsUpdateDriveSide();
}

//
// VIA 2
// 

VIA2::VIA2(C64 &ref, Drive &drvref) : VIA6522(ref, drvref)
{
    setDescription("VIA2");
}
	
VIA2::~VIA2()
{
}

u8
VIA2::portAexternal()
{
    // TODO: Which value is returned in write mode?
    return drive.readShiftreg & 0xFF;
}

u8
VIA2::portBexternal()
{
    bool sync     = drive.getSync();
    bool barrier  = drive.getLightBarrier();

    return (sync ? 0x80 : 0x00) | (barrier ? 0x00 : 0x10) | 0x6F;
}

void
VIA2::updatePB()
{
    u8 oldPb = pb;
    VIA6522::updatePB();
    u8 newPb = pb;
    
    // |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   |
    // -----------------------------------------------------------------
    // | SYNC  | Timer control | Write |  LED  | Rot.  | Stepper motor |
    // |       | (4 disk zones)|protect|       | motor | (head move)   |
    
    // Bits 6 and 5
    if ((newPb & 0x60) != (oldPb & 0x60))
        drive.setZone((newPb >> 5) & 0x03);
    
    // Bit 3
    if (GET_BIT(newPb, 3) != GET_BIT(oldPb, 3))
        drive.setRedLED(GET_BIT(newPb, 3));
    
    // Bit 2
    if (GET_BIT(newPb, 2) != GET_BIT(oldPb, 2))
        drive.setRotating(GET_BIT(newPb, 2));
    
    // Head stepper motor
    
    // Bits 1 and 0
    /*
    if ((pb & 0x03) != (oldPb & 0x03)) {
        
        // A  decrease (00-11-10-01-00...) moves the head down
        // An increase (00-01-10-11-00...) moves the head up
        
        if ((pb & 0x03) == ((oldPb + 1) & 0x03)) {
            drive->moveHeadUp();
        } else if ((pb & 0x03) == ((oldPb - 1) & 0x03)) {
            drive->moveHeadDown();
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
        
        int oldPos = (int)((drive.getHalftrack() - 1) & 0x03);
        int newPos = (int)(newPb & 0x03);
        
        if (newPos != oldPos) {
            if (newPos == ((oldPos + 1) & 0x03)) {
                drive.moveHeadUp();
                // assert(newPos == ((drive->getHalftrack() - 1) & 0x03));
            } else if (newPos == ((oldPos - 1) & 0x03)) {
                drive.moveHeadDown();
                // assert(newPos == ((drive->getHalftrack() - 1) & 0x03));
            } else {
                debug(VIA_DEBUG, "Unexpected stepper motor control sequence\n");
            }
        }
    }
}

void
VIA2::pullDownIrqLine()
{
    drive.cpu.pullDownIrqLine(INTSRC_VIA2);
}

void
VIA2::releaseIrqLine()
{
    drive.cpu.releaseIrqLine(INTSRC_VIA2);
}


