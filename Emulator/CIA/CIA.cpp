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
	setDescription("CIA");
    
    subComponents = vector<HardwareComponent *> { &tod };
    
    // Register snapshot items
    SnapshotItem items[] = {

        { &config.revision,  sizeof(config.revision),  KEEP_ON_RESET },
        { &config.timerBBug, sizeof(config.timerBBug), KEEP_ON_RESET },

        // { &idleCycles,       sizeof(idleCycles),       CLEAR_ON_RESET },
        { &counterA,         sizeof(counterA),         CLEAR_ON_RESET },
        { &latchA,           sizeof(latchA),           CLEAR_ON_RESET },
        { &counterB,         sizeof(counterB),         CLEAR_ON_RESET },
        { &latchB,           sizeof(latchB),           CLEAR_ON_RESET },
        { &delay,            sizeof(delay),            CLEAR_ON_RESET },
        { &feed,             sizeof(feed),             CLEAR_ON_RESET },
        { &CRA,              sizeof(CRA),              CLEAR_ON_RESET },
        { &CRB,              sizeof(CRB),              CLEAR_ON_RESET },
        { &icr,              sizeof(icr),              CLEAR_ON_RESET },
        { &icrAck,           sizeof(icrAck),           CLEAR_ON_RESET },
        { &imr,              sizeof(imr),              CLEAR_ON_RESET },
        { &PB67TimerMode,    sizeof(PB67TimerMode),    CLEAR_ON_RESET },
        { &PB67TimerOut,     sizeof(PB67TimerOut),     CLEAR_ON_RESET },
        { &PB67Toggle,       sizeof(PB67Toggle),       CLEAR_ON_RESET },
        { &PRA,              sizeof(PRA),              CLEAR_ON_RESET },
        { &PRB,              sizeof(PRB),              CLEAR_ON_RESET },
        { &DDRA,             sizeof(DDRA),             CLEAR_ON_RESET },
        { &DDRB,             sizeof(DDRB),             CLEAR_ON_RESET },
        { &PA,               sizeof(PA),               CLEAR_ON_RESET },
        { &PB,               sizeof(PB),               CLEAR_ON_RESET },
        { &SDR,              sizeof(SDR),              CLEAR_ON_RESET },
        { &serClk,           sizeof(serClk),           CLEAR_ON_RESET },
        { &serCounter,       sizeof(serCounter),       CLEAR_ON_RESET },
        { &CNT,              sizeof(CNT),              CLEAR_ON_RESET },
        { &INT,              sizeof(INT),              CLEAR_ON_RESET },
        { &tiredness,        sizeof(sleeping),         CLEAR_ON_RESET },
        { &idleCycles,       sizeof(idleCycles),       CLEAR_ON_RESET },
        { &sleepCycle,       sizeof(sleepCycle),       CLEAR_ON_RESET },
        { &wakeUpCycle,      sizeof(wakeUpCycle),      CLEAR_ON_RESET },
        { NULL,              0,                        0 }};

    registerSnapshotItems(items, sizeof(items));
    
    config.revision = MOS_6526;
    config.timerBBug = true;
}

void
CIA::_reset()
{
    // Clear snapshot items marked with 'CLEAR_ON_RESET'
     if (snapshotItems != NULL)
         for (unsigned i = 0; snapshotItems[i].data != NULL; i++)
             if (snapshotItems[i].flags & CLEAR_ON_RESET)
                 memset(snapshotItems[i].data, 0, snapshotItems[i].size);

	CNT = true;
	INT = 1;
	
	latchA = 0xFFFF;
	latchB = 0xFFFF;
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
        
        // info.sdr = sdr;
        // info.ssr = ssr;
        info.icr = icr;
        info.imr = imr;
        info.intLine = INT;
        
        info.tod = tod.info;
        info.todIntEnable= imr & 0x04;
        
        info.idleCycles = idleCycles;
        info.idlePercentage =  cpu.cycle ? (double)idleCycles / (double)cpu.cycle : 100.0;
    }
}

void
CIA::setRevision(CIARevision revision)
{
    assert(isCIARevision(revision));
    config.revision = revision;
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

u8
CIA::peek(u16 addr)
{
	u8 result;

    wakeUp();

    assert(addr <= 0x000F);
	switch(addr) {
            
        case 0x00: // CIA_DATA_PORT_A
        {
            updatePA();
            return PA;
        }

        case 0x01: // CIA_DATA_PORT_B
        {
            updatePB();
            return PB;
        }
        case 0x02: // CIA_DATA_DIRECTION_A

			result = DDRA;
			break;

        case 0x03: // CIA_DATA_DIRECTION_B

			result = DDRB;
			break;
			
        case 0x04: // CIA_TIMER_A_LOW
			
            result = LO_BYTE(counterA);
			break;
			
        case 0x05: // CIA_TIMER_A_HIGH
            result = HI_BYTE(counterA);
			break;
			
        case 0x06: // CIA_TIMER_B_LOW

            result = LO_BYTE(counterB);
			break;
			
        case 0x07: // CIA_TIMER_B_HIGH
			
            result = HI_BYTE(counterB);
			break;
			
        case 0x08: // CIA_TIME_OF_DAY_SEC_FRAC
			
			result = tod.getTodTenth();
            tod.defreeze();
			break;
		
        case 0x09: // CIA_TIME_OF_DAY_SECONDS
			
			result = tod.getTodSeconds();
			break;
			
        case 0x0A: // CIA_TIME_OF_DAY_MINUTES
			
			result = tod.getTodMinutes();
			break;
			
        case 0x0B: // CIA_TIME_OF_DAY_HOURS

            tod.freeze();
			result = tod.getTodHours();
			break;
			
        case 0x0C: // CIA_SERIAL_DATA_REGISTER
			
			result = SDR;
			break;
			
        case 0x0D: // CIA_INTERRUPT_CONTROL
		
            // For new CIAs, set upper bit if an IRQ is being triggered
            if ((delay & CIASetInt1) && (icr & 0x1F) && config.revision == MOS_8521) {
                icr |= 0x80;
            }
            
            // Remember result
            result = icr;
            
            // Release interrupt request
            if (INT == 0) {
                delay |= CIAClearInt0;
            }
            
            // Discard pending interrupts
            delay &= ~(CIASetInt0 | CIASetInt1);
        
            // Schedule the ICR bits to be cleared
            if (config.revision == MOS_8521) {
                delay |= CIAClearIcr0; // Uppermost bit
                delay |= CIAAckIcr0;   // Other bits
                icrAck = icr;
            } else {
                delay |= CIAClearIcr0; // Uppermost bit
                icr &= 0x80;           // Other bits
            }

            // Remember the read access
            delay |= CIAReadIcr0;
			break;

        case 0x0E: // CIA_CONTROL_REG_A

			result = (u8)(CRA & ~0x10); // Bit 4 is always 0 when read
			break;
			
        case 0x0F: // CIA_CONTROL_REG_B
			
			result = (u8)(CRB & ~0x10); // Bit 4 is always 0 when read
			break;
			
		default:
			result = 0;
			panic("Unknown CIA address %04X\n", addr);
			break;
	}
	
	return result;
}

u8
CIA::spypeek(u16 addr)
{
    bool running;

    assert(addr <= 0x000F);
    switch(addr) {
          
        case 0x00: // CIA_DATA_PORT_A
            return PA;
            
        case 0x01: // CIA_DATA_PORT_B
            return PB;
            
        case 0x02: // CIA_DATA_DIRECTION_A
            return DDRA;
            
        case 0x03: // CIA_DATA_DIRECTION_B
            return DDRB;
            
        case 0x04: // CIA_TIMER_A_LOW
            running = delay & CIACountA3;
            return LO_BYTE(counterA - (running ? (u16)idle() : 0));
            
        case 0x05: // CIA_TIMER_A_HIGH
            running = delay & CIACountA3;
            return HI_BYTE(counterA - (running ? (u16)idle() : 0));
            
        case 0x06: // CIA_TIMER_B_LOW
            running = delay & CIACountB3;
            return LO_BYTE(counterB - (running ? (u16)idle() : 0));
            
        case 0x07: // CIA_TIMER_B_HIGH
            running = delay & CIACountB3;
            return HI_BYTE(counterB - (running ? (u16)idle() : 0));
            
        case 0x08: // CIA_TIME_OF_DAY_SEC_FRAC
            return tod.getTodTenth();
            
        case 0x09: // CIA_TIME_OF_DAY_SECONDS
            return tod.getTodSeconds();
            
        case 0x0A: // CIA_TIME_OF_DAY_MINUTES
            return tod.getTodMinutes();
            
        case 0x0B: // CIA_TIME_OF_DAY_HOURS
            return tod.getTodHours();
            
        case 0x0C: // CIA_SERIAL_DATA_REGISTER
            return SDR;
            
        case 0x0D: // CIA_INTERRUPT_CONTROL
            return icr;
            
        case 0x0E: // CIA_CONTROL_REG_A
            return CRA & ~0x10;
            
        case 0x0F: // CIA_CONTROL_REG_B
            return CRB & ~0x10;
            
        default:
            assert(0);
            return 0;
    }
}

void
CIA::poke(u16 addr, u8 value)
{
    wakeUp();
    
	switch(addr) {
		
        case 0x00: // CIA_DATA_PORT_A
            
            pokePA(value);
            // PRA = value;
            // updatePA();
            return;
            
        case 0x01: // CIA_DATA_PORT_B
            
            PRB = value;
            updatePB();
            return;
            
        case 0x02: // CIA_DATA_DIRECTION_A
            
            pokeDDRA(value);
            // DDRA = value;
            // updatePA();
            return;
            
        case 0x03: // CIA_DATA_DIRECTION_B
            
            DDRB = value;
            updatePB();
            return;
            
        case 0x04: // CIA_TIMER_A_LOW
			
            latchA = (latchA & 0xFF00) | value;
			if (delay & CIALoadA2) {
                counterA = (counterA & 0xFF00) | value;
			}
			return;
			
        case 0x05: // CIA_TIMER_A_HIGH
			
            latchA = (latchA & 0x00FF) | (value << 8);
            if (delay & CIALoadA2) {
                counterA = (counterA & 0x00FF) | (value << 8);
            }
            
			// Load counter if timer is stopped
			if (!(CRA & 0x01)) {
				delay |= CIALoadA0;
			}
			return;
			
        case 0x06: // CIA_TIMER_B_LOW

            latchB = (latchB & 0xFF00) | value;
			if (delay & CIALoadB2) {
                counterB = (counterB & 0xFF00) | value;
			}			
			return;
			
        case 0x07: // CIA_TIMER_B_HIGH
			
            latchB = (latchB & 0x00FF) | (value << 8);
            if (delay & CIALoadB2) {
                counterB = (counterB & 0x00FF) | (value << 8);
            }
            
			// Load counter if timer is stopped
			if ((CRB & 0x01) == 0) {
				delay |= CIALoadB0;
			}
			return;
			
        case 0x08: // CIA_TIME_OF_DAY_SEC_FRAC
            
			if (CRB & 0x80) {
				tod.setAlarmTenth(value);
			} else { 
				tod.setTodTenth(value);
                tod.cont();
			}
			return;
			
        case 0x09: // CIA_TIME_OF_DAY_SECONDS
            
            if (CRB & 0x80) {
				tod.setAlarmSeconds(value);
            } else {
				tod.setTodSeconds(value);
            }
			return;
			
        case 0x0A: // CIA_TIME_OF_DAY_MINUTES
            
            if (CRB & 0x80) {
				tod.setAlarmMinutes(value);
            } else {
				tod.setTodMinutes(value);
            }
			return;
			
        case 0x0B: // CIA_TIME_OF_DAY_HOURS
			
			if (CRB & 0x80) {
				tod.setAlarmHours(value);
			} else {
                // Writing 12 pm into hour register turns to 12 am and vice versa.
				if ((value & 0x1F) == 0x12)
					value ^= 0x80;
				tod.setTodHours(value);
                tod.stop();
			}
			return;
			
        case 0x0C: // CIA_DATA_REGISTER
            
            SDR = value;
            delay |= CIASerLoad0;
            feed |= CIASerLoad0;
            // delay &= ~SerLoad1;
			return;
			
        case 0x0D: // CIA_INTERRUPT_CONTROL
			
			// Bit 7 means set (1) or clear (0) the other bits
			if ((value & 0x80) != 0) {
				imr |= (value & 0x1F);
			} else {
				imr &= ~(value & 0x1F);
			}
            
			// Raise an interrupt in the next cycle if conditions match
			if ((imr & icr & 0x1F) && INT) {
                if (config.revision == MOS_8521) {
                    if (!(delay & CIAReadIcr1)) {
                        delay |= (CIASetInt1 | CIASetIcr1);
                    }
                } else {
                    delay |= (CIASetInt0 | CIASetIcr0);
                }
			}
            
            // Clear pending interrupt if a write has occurred in the previous cycle
            // Solution is taken from Hoxs64. It fixes dd0dtest (11)
            else if (delay & CIAClearIcr2) {
                if (config.revision == MOS_6526) {
                     delay &= ~(CIASetInt1 | CIASetIcr1);
                 }
            }
            
			return;
			
        case 0x0E: // CIA_CONTROL_REG_A
		
            // -------0 : Stop timer
            // -------1 : Start timer
            if (value & 0x01) {
                delay |= CIACountA1 | CIACountA0;
                feed |= CIACountA0;
                if (!(CRA & 0x01))
                    PB67Toggle |= 0x40; // Toggle is high on start
            } else {
                delay &= ~(CIACountA1 | CIACountA0);
                feed &= ~CIACountA0;
            }
            
            // ------0- : Don't indicate timer underflow on port B
            // ------1- : Indicate timer underflow on port B bit 6
            if (value & 0x02) {
                PB67TimerMode |= 0x40;
                if (!(value & 0x04)) {
                    if ((delay & CIAPB7Low1) == 0) {
                        PB67TimerOut &= ~0x40;
                    } else {
                        PB67TimerOut |= 0x40;
                    }
                } else {
                    PB67TimerOut = (PB67TimerOut & ~0x40) | (PB67Toggle & 0x40);
                }
            } else {
                PB67TimerMode &= ~0x40;
            }
            
            // -----0-- : Upon timer underflow, invert port B bit 6
            // -----1-- : Upon timer underflow, generate a positive edge
            //            on port B bit 6 for one cycle

            // ----0--- : Timer restarts upon underflow
            // ----1--- : Timer stops upon underflow (One shot mode)
            if (value & 0x08) {
                feed |= CIAOneShotA0;
            } else {
                feed &= ~CIAOneShotA0;
            }
            
            // ---0---- : Nothing to do
            // ---1---- : Load start value into timer
            if (value & 0x10) {
                delay |= CIALoadA0;
            }

            // --0----- : Timer counts system cycles
            // --1----- : Timer counts positive edges on CNT pin
            if (value & 0x20) {
                delay &= ~(CIACountA1 | CIACountA0);
                feed &= ~CIACountA0;
            }
    
            // -0------ : Serial shift register in input mode (read)
            // -1------ : Serial shift register in output mode (write)
            if ((value ^ CRA) & 0x40)
            {
                //serial direction changing
                delay &= ~(CIASerLoad0 | CIASerLoad1);
                feed &= ~CIASerLoad0;
                serCounter = 0;
            
                delay &= ~(CIASerClk0 | CIASerClk1 | CIASerClk2);
                feed &= ~CIASerClk0;
            }
            
            // 0------- : TOD speed = 60 Hz
            // 1------- : TOD speed = 50 Hz
            // TODO: We need to react on a change of this bit
            tod.setHz((value & 0x80) ? 5 /* 50 Hz */ : 6 /* 60 Hz */);
            
            updatePB(); // Because PB67timerMode and PB6TimerOut may have changed
			CRA = value;
			
			return;
			
        case 0x0F: // CIA_CONTROL_REG_B
		{
            // -------0 : Stop timer
            // -------1 : Start timer
            if (value & 0x01) {
                delay |= CIACountB1 | CIACountB0;
                feed |= CIACountB0;
                if (!(CRB & 0x01))
                    PB67Toggle |= 0x80; // Toggle is high on start
            } else {
                delay &= ~(CIACountB1 | CIACountB0);
                feed &= ~CIACountB0;
            }
            
            // ------0- : Don't indicate timer underflow on port B
            // ------1- : Indicate timer underflow on port B bit 7
            if (value & 0x02) {
                PB67TimerMode |= 0x80;
                if ((value & 0x04) == 0) {
                    if ((delay & CIAPB7Low1) == 0) {
                        PB67TimerOut &= ~0x80;
                    } else {
                        PB67TimerOut |= 0x80;
                    }
                } else {
                    PB67TimerOut = (PB67TimerOut & ~0x80) | (PB67Toggle & 0x80);
                }
            } else {
                PB67TimerMode &= ~0x80;
            }
            
            // -----0-- : Upon timer underflow, invert port B bit 7
            // -----1-- : Upon timer underflow, generate a positive edge
            //            on port B bit 7 for one cycle
            
            // ----0--- : Timer restarts upon underflow
            // ----1--- : Timer stops upon underflow (One shot mode)
            if (value & 0x08) {
                feed |= CIAOneShotB0;
            } else {
                feed &= ~CIAOneShotB0;
            }
            
            // ---0---- : Nothing to do
            // ---1---- : Load start value into timer
            if (value & 0x10) {
                delay |= CIALoadB0;
            }
            
            // -00----- : Timer counts system cycles
            // -01----- : Timer counts positive edges on CNT pin
            // -10----- : Timer counts underflows of timer A
            // -11----- : Timer counts underflows of timer A occurring along with a
            //            positive edge on CNT pin
            if (value & 0x60) {
                delay &= ~(CIACountB1 | CIACountB0);
                feed &= ~CIACountB0;
            }
            
            // 0------- : Writing into TOD registers sets TOD
            // 1------- : Writing into TOD registers sets alarm time
            
            updatePB(); // Because PB67timerMode and PB6TimerOut may have changed
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
    wakeUp();
    tod.increment();
}

void
CIA::todInterrupt()
{
    delay |= CIATODInt0;
}

void
CIA::_dump()
{
    _inspect();

    msg("                   Clock : %lld\n", clock);
    // msg("                Sleeping : %s\n", sleeping ? "yes" : "no");
    msg("               Tiredness : %d\n", tiredness);
    // msg(" Most recent sleep cycle : %lld\n", sleepCycle);
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
//     msg("              serCounter : %02X\n", serCounter);
    msg("\n");
    msg("                     CNT : %d\n", CNT);
    msg("                     INT : %d\n", INT);
    msg("\n");

    tod.dump();
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
CIA::sleep()
{
    // Don't call this method on a sleeping CIA
    assert(!sleeping);
    
    // Determine maximum possible sleep cycle based on timer counts
    Cycle sleepA = cpu.cycle + ((counterA > 2) ? (counterA - 1) : 0);
    Cycle sleepB = cpu.cycle + ((counterB > 2) ? (counterB - 1) : 0);
    Cycle sleep = MIN(sleepA, sleepB);
    
    // CIAs with stopped timers can sleep forever
    if (!(feed & CIACountA0)) sleepA = INT64_MAX;
    if (!(feed & CIACountB0)) sleepB = INT64_MAX;
    
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
    assert(idleCounter == missedCycles);
    
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
        idleCounter = 0;
    }

    sleeping = false;
}

Cycle
CIA::idle()
{
    return isAwake() ? 0 : cpu.cycle - sleepCycle;
}


// -----------------------------------------------------------------------------
// Complex Interface Adapter 1
// -----------------------------------------------------------------------------

CIA1::CIA1(C64 &ref) : CIA(ref)
{
    setDescription("CIA1");
}

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
CIA1::portAinternal()
{
    return PRA;
}

u8
CIA1::portAexternal()
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
        mouse.fallingStrobe(2 /* Port */);
    if (RISING_EDGE_BIT(oldPA, PA, 4))
        mouse.risingStrobe(2 /* Port */);
}

//                    -------
//   JOYA0, ROW0 <--> | PB0 |
//   JOYA1, ROW1 <--> | PB1 |
//   JOYA2, ROW2 <--> | PB2 |
//   JOYA3, ROW3 <--> | PB3 |
// BTNA/LP, ROW4 <--> | PB4 | --> LP (VIC)
//          ROW5 <--> | PB5 |
//          ROW6 <--> | PB6 |
//          ROW  <--> | PB7 |
//                    -------

u8
CIA1::portBinternal()
{
    return PRB;
}

u8
CIA1::portBexternal()
{
    return 0xFF;
    // return keyboard.getRowValues(PA);
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
        COPY_BIT(PB67TimerOut, PB, 6);
    
    // Check if timer B underflow shows up on PB7
    if (GET_BIT(PB67TimerMode, 7))
        COPY_BIT(PB67TimerOut, PB, 7);
    
    // The control port can always bring the port lines low
    PB &= port1.bitmask();
    
    // PB4 is connected to the VIC (LP pin).
    vic.setLP(GET_BIT(PB, 4) != 0);
    
    // An edge on PB4 triggers the NeosMouse on port 1
    if (FALLING_EDGE_BIT(oldPB, PB, 4))
        mouse.fallingStrobe(1 /* Port */);
    if (RISING_EDGE_BIT(oldPB, PB, 4))
        mouse.risingStrobe(1 /* Port */);
}


// -----------------------------------------------------------------------------
// Complex Interface Adapter 2
// -----------------------------------------------------------------------------

CIA2::CIA2(C64 &ref) : CIA(ref)
{
    setDescription("CIA2");
}

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
CIA2::portAinternal()
{
    return PRA;
}

u8
CIA2::portAexternal()
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
    
    // PA0 (VA14) and PA1 (VA15) determine the memory bank seen by the VIC
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
CIA2::portBinternal()
{
    u8 result = PRB;
    
    // Check if timer A underflow shows up on PB6
    if (GET_BIT(PB67TimerMode, 6))
        COPY_BIT(PB67TimerOut, result, 6);
    
    // Check if timer B underflow shows up on PB7
    if (GET_BIT(PB67TimerMode, 7))
        COPY_BIT(PB67TimerOut, result, 7);
    
    return result;
}

u8
CIA2::portBexternal()
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

