// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "C64.h"

CIA::CIA(C64 &ref) : C64Component(ref)
{    
    subComponents = vector<HardwareComponent *> { &tod };
    
    config.revision = MOS_6526;
    config.timerBBug = true;
}

void
CIA::_reset()
{
    RESET_SNAPSHOT_ITEMS

	CNT = true;
	INT = 1;
	
	latchA = 0xFFFF;
	latchB = 0xFFFF;
}

long
CIA::getConfigItem(Option option) const
{
    switch (option) {
            
        case OPT_CIA_REVISION:  return config.revision;
        case OPT_TIMER_B_BUG:   return config.timerBBug;
        
        default:
            assert(false);
            return 0;
    }
}

bool
CIA::setConfigItem(Option option, long value)
{
    switch (option) {
            
        case OPT_CIA_REVISION:
            
            if (!CIARevisionEnum::verify(value)) return false;
            if (config.revision == value)  return false;
            
            config.revision = (CIARevision)value;
            return true;
            
        case OPT_TIMER_B_BUG:
            
            if (config.timerBBug == value) return false;

            config.timerBBug = value;
            return true;
            
        default:
            return false;
    }
}

void
CIA::_inspect()
{
    synchronized {
        
        updatePA();
        info.portA.port = PA;
        info.portA.reg = PRA;
        info.portA.dir = DDRA;
        
        updatePB();
        info.portB.port = PB;
        info.portB.reg = PRB;
        info.portB.dir = DDRB;
        
        info.timerA.count = LO_HI(spypeek(0x04), spypeek(0x05));
        info.timerA.latch = latchA;
        info.timerA.running = (delay & CIACountA3);
        info.timerA.toggle = CRA & 0x04;
        info.timerA.pbout = CRA & 0x02;
        info.timerA.oneShot = CRA & 0x08;
        
        info.timerB.count = LO_HI(spypeek(0x06), spypeek(0x07));
        info.timerB.latch = latchB;
        info.timerB.running = (delay & CIACountB3);
        info.timerB.toggle = CRB & 0x04;
        info.timerB.pbout = CRB & 0x02;
        info.timerB.oneShot = CRB & 0x08;
        
        info.sdr = sdr;
        info.ssr = sdr;  // ssr not yet implemented
        info.icr = icr;
        info.imr = imr;
        info.intLine = INT;
        
        info.tod = tod.info;
        info.todIntEnable= imr & 0x04;
        
        info.idleSince = idleSince();
        info.idleTotal = idleTotal();
        info.idlePercentage =  cpu.cycle ? (double)idleCycles / (double)cpu.cycle : 100.0;
    }
}

void
CIA::_dump() const
{
    msg("                Sleeping : %s\n", sleeping ? "yes" : "no");
    msg("               Tiredness : %d\n", tiredness);
    msg(" Most recent sleep cycle : %lld\n", sleepCycle);
    msg("Most recent wakeup cycle : %lld\n", wakeUpCycle);
    msg("\n");
    msg("               Counter A : %04X\n", info.timerA.count);
    msg("                 Latch A : %04X\n", info.timerA.latch);
    msg("         Data register A : %02X\n", info.portA.reg);
    msg("   Data port direction A : %02X\n", info.portA.dir);
    msg("             Data port A : %02X\n", info.portA.port);
    msg("      Control register A : %02X\n", CRA);
    msg("\n");
    msg("               Counter B : %04X\n", info.timerB.count);
    msg("                 Latch B : %04X\n", info.timerB.latch);
    msg("         Data register B : %02X\n", info.portB.reg);
    msg("   Data port direction B : %02X\n", info.portB.dir);
    msg("             Data port B : %02X\n", info.portB.port);
    msg("      Control register B : %02X\n", CRB);
    msg("\n");
    msg("   Interrupt control reg : %02X\n", info.icr);
    msg("      Interrupt mask reg : %02X\n", info.imr);
    msg("\n");
//    msg("                     SDR : %02X %02X\n", info.sdr, sdr);
//    msg("              serCounter : %02X\n", serCounter);
    msg("\n");
    msg("                     CNT : %d\n", CNT);
    msg("                     INT : %d\n", INT);
    msg("\n");

    // tod.dump();
}

void
CIA::triggerRisingEdgeOnFlagPin()
{
    // ICR &= ~0x10; // Note: FLAG pin is inverted
}

void
CIA::triggerFallingEdgeOnFlagPin()
{
    // TODO: CLEAN THIS UP (USE CORRECT TIMING Interrupt0 etc.)
    icr |= 0x10; // Note: FLAG pin is inverted
        
    // Trigger interrupt, if enabled
    if (imr & 0x10) {
        INT = 0;
        icr |= 0x80;
        pullDownInterruptLine();
    }
}

void
CIA::triggerTimerIrq()
{
    switch (config.revision) {
            
        case MOS_6526:
            delay |= CIASetInt0;
            delay |= CIASetIcr0;
            return;
            
        case MOS_8521:
            // Test cases:  (?)
            // testprogs\interrupts\irqnmi\cia-int-irq-new.prg
            // testprogs\interrupts\irqnmi\cia-int-nmi-new.prg
            delay |= (delay & CIAReadIcr0) ? CIASetInt0 : CIASetInt1;
            delay |= (delay & CIAReadIcr0) ? CIASetIcr0 : CIASetIcr1;
            return;
            
        default:
            assert(false);
    }
}

void
CIA::triggerTodIrq()
{
    delay |= CIASetInt0;
    delay |= CIASetIcr0;
}

void
CIA::triggerSerialIrq()
{
    delay |= CIASetInt0;
    delay |= CIASetIcr0;
}

void
CIA::todInterrupt()
{
    delay |= CIATODInt0;
}

void
CIA::executeOneCycle()
{
    if (sleeping) wakeUp(cpu.cycle - 1);
    
    u64 oldDelay = delay;
    u64 oldFeed  = feed;
    
    //
	// Layout of timer (A and B)
	//

    // Source: "A Software Model of the CIA6526" by Wolfgang Lorenz
	//
    //                           Phi2            Phi2                  Phi2
	//                            |               |                     |
	// timerA      -----    ------v------   ------v------     ----------v---------
	// input  ---->| & |--->| dwDelay & |-X-| dwDelay & |---->| decrement counter|
	//         --->|   |    |  CountA2  | | |  CountA3  |     |        (1)       |
	//         |   -----    ------------- | -------------     |                  |
	// -----------------          ^ Clr   |                   |                  |
	// | bCRA & 0x01   | Clr (3)  |       | ------------------| new counter = 0? |
	// | timer A start |<----     |       | |                 |                  |
	// -----------------    |     |       v v                 |                  |
 	//                    -----   |      -----                |      timer A     |
	//                    | & |   |      | & |                |  16 bit counter  |
	//                    |   |   |      |   |                |     and latch    |
	//                    -----   |      -----                |                  |
    //                     ^ ^    |        |(2)               |                  |
    //                     | |    ---------|-------------     |                  |
    //                     | |             |            |     |                  |
	// timer A             | |             |    -----   |     |                  |
	// output  <-----------|-X-------------X--->|>=1|---X---->| load from latch  |
	//                     |                --->|   |         |        (4)       |
	//                    -----             |   -----         --------------------
	//                    |>=1|             |
	//                    |   |             |       Phi2
	//                    -----             |        |
	//                     ^ ^              |  ------v------    ----------------
	//                     | |              ---| dwDelay & |<---| bcRA & 0x10  |
	//                     | ----------------  |  LoadA1   |    | force load   |
	//                     |       Phi2     |  -------------    ----------------
    //                     |        |       |                            ^ Clr
	// -----------------   |  ------v------ |                            |
	// | bCRA & 0x08   |   |  | dwDelay & | |                           Phi2
	// | one shot      |---X->| oneShotA0 |--
	// -----------------      -------------

				
	// Timer A

	// Decrement counter

	if (delay & CIACountA3)
		counterA--; // (1)
	
	// Check underflow condition
	bool timerAOutput = (counterA == 0 && (delay & CIACountA2)); // (2)
	
	if (timerAOutput) {
        
        icrAck &= ~0x01;
        
		// Stop timer in one shot mode
		if ((delay | feed) & CIAOneShotA0) { // (3)
			CRA &= ~0x01;
			delay &= ~(CIACountA2 | CIACountA1 | CIACountA0);
			feed &= ~CIACountA0;
		}
		
		// Timer A output to timer B in cascade mode
		if ((CRB & 0x61) == 0x41 || ((CRB & 0x61) == 0x61 && CNT)) {
			delay |= CIACountB1;
		}
        
        // Reload counter immediately
		delay |= CIALoadA1;
	}
    
	// Load counter
	if (delay & CIALoadA1) // (4)
		reloadTimerA(); 
	
	// Timer B
	
	// Decrement counter
	if (delay & CIACountB3) {
		counterB--; // (1)
	}

	// Check underflow condition
	bool timerBOutput = (counterB == 0 && (delay & CIACountB2)); // (2)
	
	if (timerBOutput) {
				
        icrAck &= ~0x02;
        
		// Stop timer in one shot mode
		if ((delay | feed) & CIAOneShotB0) { // (3)
			CRB &= ~0x01;
			delay &= ~(CIACountB2 | CIACountB1 | CIACountB0);
			feed &= ~CIACountB0;
		}
		delay |= CIALoadB1;
	}
	
	// Load counter
	if (delay & CIALoadB1) // (4)
		reloadTimerB();
		
    //
    // Serial register
    //
    
    // Generate clock signal
    if (timerAOutput && (CRA & 0x40) /* output mode */ ) {
        
        if (serCounter) {
            
            // Toggle serial clock signal
            feed ^= CIASerClk0;
            
        } else if (delay & CIASerLoad1) {
            
            // Load shift register
            delay &= ~(CIASerLoad1 | CIASerLoad0);
            feed &= ~CIASerLoad0;
            serCounter = 8;
            feed ^= CIASerClk0;
        }
    }
    
    // Run shift register with generated clock signal
    if (serCounter) {
        if ((delay & (CIASerClk2 | CIASerClk1)) == CIASerClk1) {      // Positive edge
            if (serCounter == 1) {
                delay |= CIASerInt0; // Trigger interrupt
            }
        }
        else if ((delay & (CIASerClk2 | CIASerClk1)) == CIASerClk2) { // Negative edge
            serCounter--;
        }
    }
	
	//
	// Timer output to PB6 (timer A) and PB7 (timer B)
    // 
	
	// Source: "A Software Model of the CIA6526" by Wolfgang Lorenz
	//
	//                     (7)            -----------------
    //         -------------------------->| bCRA & 0x04   |
    //         |                          | timer mode    |  ----------------
	//         |                          | 0x00: pulse   |->| 0x02 (timer) |
	// timerA  | Flip ---------------     |       (7)     |  |              |
    // output -X----->| bPB67Toggle |---->| 0x04: toggle  |  | bCRA & 0x02  |
	//            (5) |  ^ 0x40     |     |       (8)     |  | output mode  |-> PB6 out
	//                ---------------     -----------------  |              |
	//                       ^ Set        -----------------  | 0x00 (port)  |
	//                       |            | port B bit 6  |->|              |
	// ----------------- 0->1|            |    output     |  ----------------
	// | bCRA & 0x01   |------            -----------------
	// | timer A start |
	// -----------------

	// Timer A output to PB6
	
	if (timerAOutput) {
		
		PB67Toggle ^= 0x40; // (5) toggle underflow counter bit
		
		if (CRA & 0x02) { // (6)

			if ((CRA & 0x04) == 0) { 
				// (7) set PB6 high for one clock cycle
				PB67TimerOut |= 0x40;
				delay |= CIAPB6Low0;
				delay &= ~CIAPB6Low1;
			} else { 
				// (8) toggle PB6 (copy bit 6 from PB67Toggle)
				// PB67TimerOut = (PB67TimerOut & 0xBF) | (PB67Toggle & 0x40);
                PB67TimerOut ^= 0x40;
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
				delay |= CIAPB7Low0;
				delay &= ~CIAPB7Low1;
			} else {
				// (8) toggle PB7 (copy bit 7 from PB67Toggle)
				// PB67TimerOut = (PB67TimerOut & 0x7F) | (PB67Toggle & 0x80);
                PB67TimerOut ^= 0x80;
			}
		}
	}
	
	// Set PB67 back to low
	if (delay & CIAPB6Low1)
		PB67TimerOut &= ~0x40;

	if (delay & CIAPB7Low1)
		PB67TimerOut &= ~0x80;

	
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
	// ICR ------X---------------X-------------------           |
	//                           | (12)             |           |
	//                           v Clr              v Clr       |
	//           ------      ----------      ----------------   | (11)
	// Int    <--| -1 |<-----| bICR & |<-----|   dwDelay &  |<---
	// ouptput   |    |      |  0x80  | Set  |  Interrupt1  |
	// (14)      ------      ---------- (13) -------^--------
	//                                              |
	//                                             Phi2
    
	if (timerAOutput) { // (9)
		icr |= 0x01;
	}
	
	// if (timerBOutput && !(delay & CIAReadIcr0)) { // (10)
    if (timerBOutput) { // (10)
        
        if ((delay & CIAReadIcr0) && config.timerBBug) {
            
            // The old CIA chips (NMOS technology) exhibit a race condition here
            // which is known as the "timer B bug". If ICR is currently read,
            // the read access occurs *after* timer B sets bit 2. Hence, bit 2
            // won't show up.
            
        } else {
            icr |= 0x02;
        }
    }
    
    // Check for timer interrupt
    if ((timerAOutput && (imr & 0x01)) || (timerBOutput && (imr & 0x02))) { // (11)
        triggerTimerIrq();
    }

    // Check for TOD interrupt
    if (delay & CIATODInt0) {
        icr |= 0x04;
        if (imr & 0x04) {
            triggerTodIrq();
        }
    }
    
    // Check for Serial interrupt
    if (delay & CIASerInt2) {
        icr |= 0x08;
        if (imr & 0x08) {
            triggerSerialIrq();
        }
    }
    
    if (delay & (CIAClearIcr1 | CIAAckIcr1 | CIASetIcr1 | CIASetInt1 | CIAClearInt0)) {
        
        if (delay & CIAClearIcr1) { // (12)
            icr &= 0x7F;
        }
        if (delay & CIAAckIcr1) {
            icr &= ~icrAck;
        }
        if (delay & CIASetIcr1) { // (13)
            icr |= 0x80;
        }
        if (delay & CIASetInt1) { // (14)
            INT = 0;
            pullDownInterruptLine();
        }
        if (delay & CIAClearInt0) { // (14)
            INT = 1;
            releaseInterruptLine();
        }
    }

    // Move delay flags left and feed in new bits
    delay = ((delay << 1) & DelayMask) | feed;
    
    // Get tired if nothing has happened in this cycle
    if (oldDelay == delay && oldFeed == feed) tiredness++; else tiredness = 0;
    
    // Sleep if threshold is reached
    if (tiredness > 8 && !CIA_ON_STEROIDS) sleep();
}

void
CIA::incrementTOD()
{
    wakeUp();
    tod.increment();
}

void
CIA::sleep()
{
    // Don't call this method on a sleeping CIA
    assert(!sleeping);
    
    // Determine maximum possible sleep cycle based on timer counts
    Cycle sleepA = cpu.cycle + ((counterA > 2) ? (counterA - 1) : 0);
    Cycle sleepB = cpu.cycle + ((counterB > 2) ? (counterB - 1) : 0);
    
    // CIAs with stopped timers can sleep forever
    if (!(feed & CIACountA0)) sleepA = INT64_MAX;
    if (!(feed & CIACountB0)) sleepB = INT64_MAX;
    Cycle sleep = MIN(sleepA, sleepB);

    // ZZzzz
    sleepCycle = cpu.cycle;
    wakeUpCycle = sleep;
    tiredness = 0;
    sleeping = true;
}

void
CIA::wakeUp()
{
    wakeUp(cpu.cycle);
}

void
CIA::wakeUp(Cycle targetCycle)
{
    // Don't call this method on an active CIA
    // assert(sleeping);
    if (!sleeping) return;
        
    // Calculate the number of missed cycles
    wakeUpCycle = targetCycle;
    Cycle missedCycles = wakeUpCycle - sleepCycle;
    
    // Make up for missed cycles
    if (missedCycles > 0) {
        
        if (feed & CIACountA0) {
            assert(counterA >= missedCycles);
            counterA -= missedCycles;
        }
        if (feed & CIACountB0) {
            assert(counterB >= missedCycles);
            counterB -= missedCycles;
        }
        
        idleCycles += missedCycles;
    }

    sleeping = false;
}

Cycle
CIA::idleSince() const
{
    return isAwake() ? 0 : cpu.cycle - sleepCycle;
}


//
// CIA 1
//

void 
CIA1::pullDownInterruptLine()
{
    cpu.pullDownIrqLine(INTSRC_CIA);
}

void 
CIA1::releaseInterruptLine()
{
    cpu.releaseIrqLine(INTSRC_CIA);
}

//                    -------
//   JOYB0, COL0 <--> | PA0 |
//   JOYB1, COL1 <--> | PA1 |
//   JOYB2, COL2 <--> | PA2 |
//   JOYB3, COL3 <--> | PA3 |
//   BTNB,  COL4 <--> | PA4 |
//          COL5 <--> | PA5 |
//          COL6 <--> | PA6 |
//          COL  <--> | PA7 |
//                    -------

u8
CIA1::portAinternal() const
{
    return PRA;
}

u8
CIA1::portAexternal() const
{
    return 0xFF;
    // return keyboard.getColumnValues(PB);
}

void
CIA1::updatePA()
{
    u8 oldPA = PA;
    
    PA = (portAinternal() & DDRA) | (portAexternal() & ~DDRA);

    // Get lines which are driven actively low by port 2
    u8 rowMask = ~PRB & DDRB & port1.bitmask();
    
    // Pull lines low that are connected by a pressed key
    PA &= keyboard.getColumnValues(rowMask);
    
    // The control port can always bring the port lines low
    PA &= port2.bitmask();
    
    // An edge on PA4 triggers the NeosMouse on port 2
    if (FALLING_EDGE_BIT(oldPA, PA, 4))
        mouse.fallingStrobe(PORT_TWO);
    if (RISING_EDGE_BIT(oldPA, PA, 4))
        mouse.risingStrobe(PORT_TWO);
}

//                    -------
//   JOYA0, ROW0 <--> | PB0 |
//   JOYA1, ROW1 <--> | PB1 |
//   JOYA2, ROW2 <--> | PB2 |
//   JOYA3, ROW3 <--> | PB3 |
// BTNA/LP, ROW4 <--> | PB4 | --> LP (VICII)
//          ROW5 <--> | PB5 |
//          ROW6 <--> | PB6 |
//          ROW  <--> | PB7 |
//                    -------

u8
CIA1::portBinternal() const
{
    return PRB;
}

u8
CIA1::portBexternal() const
{
    return 0xFF;
}

void
CIA1::updatePB()
{
    u8 oldPB = PB;
    
    PB = (portBinternal() & DDRB) | (portBexternal() & ~DDRB);
 
    // Get lines which are driven actively low by port 1
    u8 columnMask = ~PRA & DDRA & port2.bitmask();
    
    // Pull lines low that are connected by a pressed key
    PB &= keyboard.getRowValues(columnMask);
    
    // Check if timer A underflow shows up on PB6
    if (GET_BIT(PB67TimerMode, 6))
        REPLACE_BIT(PB, 6, PB67TimerOut & (1 << 6));
    
    // Check if timer B underflow shows up on PB7
    if (GET_BIT(PB67TimerMode, 7))
        REPLACE_BIT(PB, 7, PB67TimerOut & (1 << 7));
    
    // The control port can always bring the port lines low
    PB &= port1.bitmask();
    
    // PB4 is connected to the VICII (LP pin).
    vic.setLP(GET_BIT(PB, 4) != 0);
    
    // An edge on PB4 triggers the NeosMouse on port 1
    if (FALLING_EDGE_BIT(oldPB, PB, 4))
        mouse.fallingStrobe(PORT_ONE);
    if (RISING_EDGE_BIT(oldPB, PB, 4))
        mouse.risingStrobe(PORT_ONE);
}


//
// CIA 2
//

void
CIA2::_reset()
{
    CIA::_reset();

    counterA = 0xFFFF;
    counterB = 0xFFFF;
}

void 
CIA2::pullDownInterruptLine()
{
    cpu.pullDownNmiLine(INTSRC_CIA);
}

void 
CIA2::releaseInterruptLine()
{
    cpu.releaseNmiLine(INTSRC_CIA);
}

//                        -------
//              VA14 <--- | PA0 |
//              VA15 <--- | PA1 |
// User port (pin M) <--> | PA2 |
//               ATN <--- | PA3 |
//               CLK <--- | PA4 |
//              DATA <--- | PA5 |
//               CLK ---> | PA6 |
//              DATA ---> | PA7 |
//                        -------

u8
CIA2::portAinternal() const
{
    return PRA;
}

u8
CIA2::portAexternal() const
{
    u8 result = 0x3F;
    result |= (iec.clockLine ? 0x40 : 0x00);
    result |= (iec.dataLine ? 0x80 : 0x00);
    
    return result;
}

void
CIA2::updatePA()
{
    PA = (portAinternal() & DDRA) | (portAexternal() & ~DDRA);
    
    // PA0 (VA14) and PA1 (VA15) determine the memory bank seen by the VICII
    // vic.updateBankAddr();
    
    // Mark IEC bus as dirty
    iec.setNeedsUpdateC64Side();
}

//                        -------
// User port (pin C) <--> | PB0 |
// User port (pin D) <--> | PB1 |
// User port (pin E) <--> | PB2 |
// User port (pin F) <--> | PB3 |
// User port (pin H) <--> | PB4 |
// User port (pin J) <--> | PB5 |
// User port (pin K) <--> | PB6 |
// User port (pin L) <--> | PB7 |
//                        -------

u8
CIA2::portBinternal() const
{
    u8 result = PRB;
    
    // Check if timer A underflow shows up on PB6
    if (GET_BIT(PB67TimerMode, 6))
        REPLACE_BIT(result, 6, PB67TimerOut & (1 << 6));
    
    // Check if timer B underflow shows up on PB7
    if (GET_BIT(PB67TimerMode, 7))
        REPLACE_BIT(result, 7, PB67TimerOut & (1 << 7));
    
    return result;
}

u8
CIA2::portBexternal() const
{
    // User port is not implemented. All pins are high if nothing is connected.
    return 0xFF;
}

void
CIA2::updatePB()
{
    PB = (portBinternal() & DDRB) | (portBexternal() & ~DDRB);
}

void
CIA2::pokePA(u8 value)
{
    CIA::pokePA(value);
    
    // PA0 (VA14) and PA1 (VA15) determine the memory bank seen by VICII
    vic.switchBank(0xDD00);
}

void
CIA2::pokeDDRA(u8 value)
{
    CIA::pokeDDRA(value);
    
    // PA0 (VA14) and PA1 (VA15) determine the memory bank seen by VICII
    vic.switchBank(0xDD02);

}

