// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "CIA.h"
#include "C64.h"

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
            
            result = sdr;
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
            assert(false);
            result = 0;
    }
    
    trace(CIAREG_DEBUG, "peek(%x) = %x\n", addr, result);
    return result;
}

u8
CIA::spypeek(u16 addr) const
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
            return LO_BYTE(counterA - (running ? (u16)idleSince() : 0));
            
        case 0x05: // CIA_TIMER_A_HIGH
            running = delay & CIACountA3;
            return HI_BYTE(counterA - (running ? (u16)idleSince() : 0));
            
        case 0x06: // CIA_TIMER_B_LOW
            running = delay & CIACountB3;
            return LO_BYTE(counterB - (running ? (u16)idleSince() : 0));
            
        case 0x07: // CIA_TIMER_B_HIGH
            running = delay & CIACountB3;
            return HI_BYTE(counterB - (running ? (u16)idleSince() : 0));
            
        case 0x08: // CIA_TIME_OF_DAY_SEC_FRAC
            return tod.getTodTenth();
            
        case 0x09: // CIA_TIME_OF_DAY_SECONDS
            return tod.getTodSeconds();
            
        case 0x0A: // CIA_TIME_OF_DAY_MINUTES
            return tod.getTodMinutes();
            
        case 0x0B: // CIA_TIME_OF_DAY_HOURS
            return tod.getTodHours();
            
        case 0x0C: // CIA_SERIAL_DATA_REGISTER
            return sdr;
            
        case 0x0D: // CIA_INTERRUPT_CONTROL
            return icr;
            
        case 0x0E: // CIA_CONTROL_REG_A
            return CRA & ~0x10;
            
        case 0x0F: // CIA_CONTROL_REG_B
            return CRB & ~0x10;
            
        default:
            assert(false);
            return 0;
    }
}

void
CIA::poke(u16 addr, u8 value)
{
    trace(CIAREG_DEBUG, "poke(%x, %x)\n", addr, value);
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
            
            sdr = value;
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
            assert(false);
    }
}
