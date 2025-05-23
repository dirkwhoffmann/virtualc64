// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// This FILE is dual-licensed. You are free to choose between:
//
//     - The GNU General Public License v3 (or any later version)
//     - The Mozilla Public License v2
//
// SPDX-License-Identifier: GPL-3.0-or-later OR MPL-2.0
// -----------------------------------------------------------------------------

#include "VirtualC64Config.h"
#include "CIA.h"
#include "Emulator.h"
#include "IOUtils.h"

namespace vc64 {

CIA::CIA(C64 &ref, isize id) : SubComponent(ref, id)
{    
    subComponents = std::vector<CoreComponent *> { &tod };
}

void
CIA::_didReset(bool hard)
{
    assert(!sleeping);

    CNT = true;
    INT = 1;
    
    counterA = 0xFFFF;
    counterB = 0xFFFF;
    
    latchA = 0xFFFF;
    latchB = 0xFFFF;
    
    updatePA();
    updatePB();

    c64.scheduleAbs<SLOT_CIA1>(cpu.clock, CIA_EXECUTE);
    c64.scheduleAbs<SLOT_CIA2>(cpu.clock, CIA_EXECUTE);
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
CIA::reloadTimerA(u64 *delay)
{
    counterA = latchA;
    *delay &= ~CIACountA2;
}

void
CIA::reloadTimerB(u64 *delay)
{
    counterB = latchB;
    *delay &= ~CIACountB2;
}

void
CIA::triggerTimerIrq(u64 *delay)
{
    switch (config.revision) {
            
        case CIARev::MOS_6526:
            
            *delay |= CIASetInt0;
            *delay |= CIASetIcr0;
            return;
            
        case CIARev::MOS_8521:
            
            // Test cases:  (?)
            // testprogs\interrupts\irqnmi\cia-int-irq-new.prg
            // testprogs\interrupts\irqnmi\cia-int-nmi-new.prg
            *delay |= (*delay & CIAReadIcr0) ? CIASetInt0 : CIASetInt1;
            *delay |= (*delay & CIAReadIcr0) ? CIASetIcr0 : CIASetIcr1;
            return;
            
        default:
            fatalError;
    }
}

void
CIA::triggerTodIrq(u64 *delay)
{
    *delay |= CIASetInt0;
    *delay |= CIASetIcr0;
}

void
CIA::triggerSerialIrq(u64 *delay)
{
    *delay |= CIASetInt0;
    *delay |= CIASetIcr0;
}

void
CIA::todInterrupt()
{
    delay |= CIATODInt0;
}

void
CIA::serviceEvent(EventID id)
{
    switch(id) {

        case CIA_EXECUTE:

            executeOneCycle();
            break;

        case CIA_WAKEUP:

            wakeUp();
            break;

        default:
            fatalError;
    }
}

void
CIA::scheduleNextExecution()
{
    if (isCIA1()) {
        c64.scheduleRel<SLOT_CIA1>(1, CIA_EXECUTE);
    } else {
        c64.scheduleRel<SLOT_CIA2>(1, CIA_EXECUTE);
    }
}

void
CIA::scheduleWakeUp()
{
    auto event = isSleeping() ? CIA_WAKEUP : CIA_EXECUTE;

    if (isCIA1()) {
        c64.scheduleAbs<SLOT_CIA1>(wakeUpCycle, event);
    } else {
        c64.scheduleAbs<SLOT_CIA2>(wakeUpCycle, event);
    }
}

void
CIA::executeOneCycle()
{
    // Make a local copy for speed
    u64 delay = this->delay;
    
    u64 oldDelay = delay;
    u64 oldFeed  = feed;
    
    //
    // Layout of timer (A and B)
    //

    // Source: "A Software Model of the CIA6526" by Wolfgang Lorenz
    //
    //                           Phi2            Phi2                Phi2
    //                            |               |                   |
    // timerA      -----    ------v------   ------v------   ----------v---------
    // input  ---->| & |--->| dwDelay & |-X-| dwDelay & |-->| decrement counter|
    //         --->|   |    |  CountA2  | | |  CountA3  |   |        (1)       |
    //         |   -----    ------------- | -------------   |                  |
    // -----------------          ^ Clr   |                 |                  |
    // | bCRA & 0x01   | Clr (3)  |       | ----------------| new counter = 0? |
    // | timer A start |<----     |       | |               |                  |
    // -----------------    |     |       v v               |                  |
    //                    -----   |      -----              |      timer A     |
    //                    | & |   |      | & |              |  16 bit counter  |
    //                    |   |   |      |   |              |     and latch    |
    //                    -----   |      -----              |                  |
    //                     ^ ^    |        |(2)             |                  |
    //                     | |    ---------|-------------   |                  |
    //                     | |             |            |   |                  |
    // timer A             | |             |    -----   |   |                  |
    // output  <-----------|-X-------------X--->|>=1|---X-->| load from latch  |
    //                     |                --->|   |       |        (4)       |
    //                    -----             |   -----       --------------------
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

    //
    // Timer A
    //
    
    // (1) : Decrement counter
    if (delay & CIACountA3) counterA--;

    // (2) : Check underflow condition
    bool timerAOutput = (counterA == 0 && (delay & CIACountA2));

    if (timerAOutput) {
        
        icrAck &= ~0x01;
        
        // (3) : Stop timer in one shot mode
        if ((delay | feed) & CIAOneShotA0) {
            
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
    
    // (4) : Load counter
    if (delay & CIALoadA1) reloadTimerA(&delay);

    //
    // Timer B
    //
    
    // (1) Decrement counter
    if (delay & CIACountB3) counterB--;

    // Check underflow condition
    bool timerBOutput = (counterB == 0 && (delay & CIACountB2)); // (2)

    if (timerBOutput) {

        icrAck &= ~0x02;
        
        // (3) : Stop timer in one shot mode
        if ((delay | feed) & CIAOneShotB0) {
            
            CRB &= ~0x01;
            delay &= ~(CIACountB2 | CIACountB1 | CIACountB0);
            feed &= ~CIACountB0;
        }
        delay |= CIALoadB1;
    }

    // (4) : Load counter
    if (delay & CIALoadB1) reloadTimerB(&delay);
    
    //
    // Serial register
    //
    
    // Generate clock signal
    if (timerAOutput && (CRA & 0x40)) {
        
        // Output mode
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
        
        if ((delay & (CIASerClk2 | CIASerClk1)) == CIASerClk1) {
            
            // Positive edge
            if (serCounter == 1) delay |= CIASerInt0;

        } else if ((delay & (CIASerClk2 | CIASerClk1)) == CIASerClk2) {
            
            // Negative edge
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
    //                ---------------     -----------------  |     (6)      |
    //                       ^ Set        -----------------  | 0x00 (port)  |
    //                       |            | port B bit 6  |->|              |
    // ----------------- 0->1|            |    output     |  ----------------
    // | bCRA & 0x01   |------            -----------------
    // | timer A start |
    // -----------------

    //
    // Timer A output to PB6
    //
    
    if (timerAOutput) {

        // (5) : Toggle underflow counter bit
        PB67Toggle ^= 0x40;

        // (6)
        if (CRA & 0x02) {

            if ((CRA & 0x04) == 0) {
                
                // (7) : Set PB6 high for one clock cycle
                PB67TimerOut |= 0x40;
                delay |= CIAPB6Low0;
                delay &= ~CIAPB6Low1;
                
            } else {
                
                // (8) : Toggle PB6
                PB67TimerOut ^= 0x40;
            }
        }
    }

    //
    // Timer B output to PB7
    //
    
    if (timerBOutput) {

        // (5) : Toggle underflow counter bit
        PB67Toggle ^= 0x80;

        // (6)
        if (CRB & 0x02) {

            if ((CRB & 0x04) == 0) {
                
                // (7) : Set PB7 high for one clock cycle
                PB67TimerOut |= 0x80;
                delay |= CIAPB7Low0;
                delay &= ~CIAPB7Low1;
                
            } else {
                
                // (8) : Toggle PB7
                PB67TimerOut ^= 0x80;
            }
        }
    }

    // Set PB67 back to low
    if (delay & CIAPB6Low1) { PB67TimerOut &= ~0x40; }
    if (delay & CIAPB7Low1) { PB67TimerOut &= ~0x80; }


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
    
    if (timerAOutput) { icr |= 0x01; } // (9)
    if (timerBOutput) { // (10)
        
        if ((delay & CIAReadIcr0) && config.timerBBug) {
            
            /* The old CIA chips (NMOS technology) exhibit a race condition
             * here which is known as the "timer B bug". If ICR is currently
             * read, the read access occurs *after* timer B sets bit 2. Hence,
             * bit 2 won't show up.
             */
            
        } else {
            icr |= 0x02;
        }
    }
    
    // (11) : Check for timer interrupt
    if ((timerAOutput && (imr & 0x01)) || (timerBOutput && (imr & 0x02))) {
        triggerTimerIrq(&delay);
    }

    // Check for TOD interrupt
    if (delay & CIATODInt0) {
        icr |= 0x04;
        if (imr & 0x04) {
            triggerTodIrq(&delay);
        }
    }
    
    // Check for Serial interrupt
    if (delay & CIASerInt2) {
        icr |= 0x08;
        if (imr & 0x08) {
            triggerSerialIrq(&delay);
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
    delay = ((delay << 1) & CIADelayMask) | feed;
    
    // Get tired if nothing has happened in this cycle
    if (oldDelay == delay && oldFeed == feed) tiredness++; else tiredness = 0;
    
    // Write back local copy
    this->delay = delay;
    
    // Sleep when threshold is reached
    if (tiredness > 8 && config.idleSleep) {
        sleep();
        scheduleWakeUp();
    } else {
        scheduleNextExecution();
    }
}

void
CIA::sleep()
{
    // Don't call this method on a sleeping CIA
    assert(!sleeping);

    // Determine maximum possible sleep cycle based on timer counts
    Cycle sleepA = cpu.clock + ((counterA > 2) ? (counterA - 1) : 0);
    Cycle sleepB = cpu.clock + ((counterB > 2) ? (counterB - 1) : 0);

    // CIAs with stopped timers can sleep forever
    if (!(feed & CIACountA0)) sleepA = INT64_MAX;
    if (!(feed & CIACountB0)) sleepB = INT64_MAX;

    // Determine the wakeup cycle
    auto wakeupAt = std::min(sleepA, sleepB);

    if (wakeupAt > cpu.clock) {

        // ZZzzz
        sleepCycle = cpu.clock;
        wakeUpCycle = wakeupAt;
        sleeping = true;
    }
    tiredness = 0;
}

void
CIA::wakeUp()
{
    if (!sleeping) return;
    sleeping = false;

    // Calculate the number of missed cycles
    wakeUpCycle = cpu.clock;
    Cycle missedCycles = wakeUpCycle - sleepCycle;
    
    // Make up for missed cycles
    if (missedCycles > 0) {
        
        if (feed & CIACountA0) {

            assert(counterA >= missedCycles);
            counterA -= u16(missedCycles);
        }
        if (feed & CIACountB0) {

            assert(counterB >= missedCycles);
            counterB -= u16(missedCycles);
        }
        
        idleCycles += missedCycles;
    }

    // Schedule the next execution event
    scheduleNextExecution();
}

Cycle
CIA::idleSince() const
{
    return isAwake() ? 0 : cpu.clock - sleepCycle;
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
}

void
CIA1::updatePA()
{
    u8 oldPA = PA;
    PA = computePA();

    // An edge on PA4 triggers the NeosMouse on port 2
    if (FALLING_EDGE_BIT(oldPA, PA, 4)) port2.mouse.fallingStrobe();
    if (RISING_EDGE_BIT(oldPA, PA, 4)) port2.mouse.risingStrobe();
}

u8
CIA1::computePA() const
{
    u8 result = (portAinternal() & DDRA) | (portAexternal() & ~DDRA);

    // Get lines which are driven actively low by port 2
    u8 rowMask = ~PRB & DDRB & port1.getControlPort();
    
    // Pull lines low that are connected by a pressed key
    result &= keyboard.getColumnValues(rowMask);
    
    // The control port can always bring the port lines low
    result &= port2.getControlPort();
    
    return result;
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
    PB = computePB();

    // PB4 is connected to the VICII (LP pin)
    vic.setLP(GET_BIT(PB, 4) != 0);
    
    // An edge on PB4 triggers the NeosMouse on port 1
    if (FALLING_EDGE_BIT(oldPB, PB, 4)) port1.mouse.fallingStrobe();
    if (RISING_EDGE_BIT(oldPB, PB, 4)) port1.mouse.risingStrobe();
}

u8
CIA1::computePB() const
{
    u8 result = (portBinternal() & DDRB) | (portBexternal() & ~DDRB);

    // Get lines which are driven actively low by port 2
    u8 columnMask = ~PRA & DDRA & port2.getControlPort();
    
    // Pull lines low that are connected by a pressed key
    result &= keyboard.getRowValues(columnMask, PRB & DDRB);

    // Check if timer A underflow shows up on PB6
    if (GET_BIT(PB67TimerMode, 6)) REPLACE_BIT(result, 6, PB67TimerOut & (1 << 6));
    
    // Check if timer B underflow shows up on PB7
    if (GET_BIT(PB67TimerMode, 7)) REPLACE_BIT(result, 7, PB67TimerOut & (1 << 7));
    
    // The control port can always bring the port lines low
    result &= port1.getControlPort();
    
    return result;
}


//
// CIA 2
//

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
    result |= (serialPort.clockLine ? 0x40 : 0x00);
    result |= (serialPort.dataLine ? 0x80 : 0x00);
    
    return result;
}

void
CIA2::updatePA()
{
    PA = computePA();
    serialPort.setNeedsUpdate();
}

u8
CIA2::computePA() const
{
    return (portAinternal() & DDRA) | (portAexternal() & ~DDRA);
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
    return 0xFF;
}

void
CIA2::updatePB()
{
    PB = computePB();
}

u8
CIA2::computePB() const
{
    return parCable.getValue();
}

void
CIA2::pokePRA(u8 value)
{
    CIA::pokePRA(value);
    
    // PA0 (VA14) and PA1 (VA15) determine the memory bank seen by VICII
    vic.switchBank(0xDD00);
    
    // PA2 is connected to the user port
    userPort.setPA2(GET_BIT(value, 2));
}

void
CIA2::pokePRB(u8 value)
{
    CIA::pokePRB(value);
}

void
CIA2::pokeDDRA(u8 value)
{
    CIA::pokeDDRA(value);
    
    // PA0 (VA14) and PA1 (VA15) determine the memory bank seen by VICII
    vic.switchBank(0xDD02);
}

void
CIA2::pulsePC()
{
    parCable.c64Handshake();
}

}
