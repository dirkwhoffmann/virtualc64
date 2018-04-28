//
//  HoxsVIA.cpp
//  VirtualC64
//
//  Created by Dirk Hoffmann on 27.04.18.
//

#include "HoxsVIA.hpp"

#include <stdio.h>
#include <stdarg.h>
#include "C64.h"
/*
#include "boost2005.h"
#include "defines.h"
#include "bits.h"
#include "util.h"
#include "errormsg.h"
#include "hconfig.h"
#include "appstatus.h"
#include "register.h"
#include "savestate.h"
#include "c6502.h"
#include "d1541.h"
#include "via6522.h"
#include "p64.h"
#include "d64.h"
*/

void
HoxsVIA::reset()
{
    Reset(0, true);
}

void
HoxsVIA::dumpState()
{
    
}

void
HoxsVIA::execute()
{
    ExecuteCycle(0);
}

uint8_t
HoxsVIA::peek(uint16_t addr)
{
    return ReadRegister(addr, 0);
}

uint8_t HoxsVIA::read(uint16_t addr)
{
    return ReadRegister_no_affect(addr, 0);
}

void
HoxsVIA::poke(uint16_t addr, uint8_t value)
{
    WriteRegister(addr, 0, value);
}



// ORIGINAL HOXS STUFF
void HoxsVIA::InitReset(ICLK sysclock, bool poweronreset)
{
    CurrentClock = sysclock;
    DevicesClock = sysclock;
    bLatchA = false;
    bLatchB = false;
    ora=0;
    ira=0;
    orb=0;
    irb=0;
    ddra=0;
    ddrb=0;
    if (poweronreset)
    {
        timer1_counter.word=0x01AA;
        timer1_latch.word=0x01AA;
        timer2_counter.word=0x01AA;
        timer2_latch.word=0x01AA;
        shift=0;
    }
    acr=0;
    pcr=0;
    ca1_in=0;
    ca1_in_prev=0;
    ca2_in=0;
    ca2_in_prev=0;
    cb1_in=0;
    cb1_in_prev=0;
    cb2_in=0;
    cb2_in_prev=0;
    ca2_out=0;
    cb2_out=0;
    ifr=0;
    ier=0;
    delay=0;
    feed=VIACountA2 | VIACountB2;
    serial_active=0;
    serial_mode=VIA_SER_IDLE;
    modulo=0;
    
    bPB7TimerMode = 0;
    bPB7Toggle = 0;
    bPB7TimerOut = 0;
    
    dec_2=1;
    no_change_count=0;
    idle=0;
    
}

void HoxsVIA::Reset(ICLK sysclock, bool poweronreset)
{
    InitReset(sysclock, poweronreset);
    ClearSystemInterrupt();
    
    WriteRegister(11, CurrentClock, acr);
    WriteRegister(12, CurrentClock, pcr);
    
    SetPinsPortA(PortAOutput());
    SetPinsPortB(PortBOutput());
}

void HoxsVIA::ExecuteCycle(ICLK sysclock)
{
    bit8 new_ifr=0;
    // ICLKS clocks;
    
    /*
    if ((ICLKS)(DevicesClock - sysclock) < 0)
    {
        DevicesClock = sysclock;
        ExecuteDevices(sysclock);
    }
    
    clocks = (ICLKS)(sysclock - CurrentClock);
    while (clocks-- > 0)
    */
    
    {
        CurrentClock++;
        
        /*
        if (idle)
        {
            timer1_counter.word -= 1;
            timer2_counter.word -= dec_2;
            if (timer1_counter.word < VIA_MINIMUM_STAY_IDLE)
            {
                idle=0;
                no_change_count=0;
            }
            if (timer2_counter.word < VIA_MINIMUM_STAY_IDLE && dec_2!=0)
            {
                idle=0;
                no_change_count=0;
            }
            continue;
        }
        */
        
        old_delay = delay;
        old_feed = feed;
        
        
        if (delay & VIALoadA2)
        {
            timer1_counter.word = timer1_latch.word;
        }
        else if (delay & VIACountA3)
        {
            timer1_counter.word -= 1;
        }
        
        if (timer1_counter.word==0)
        {
            if ((acr & 0x40) != 0 && (feed & VIAPostOneShotA0) == 0)
            {
                //Continuous; Free running; Square wave if PB7 out is enabled; nnnn ... 0001 0000 FFFF nnnn
                delay |= VIALoadA0;
                new_ifr |= VIA_INT_T1;
                
                bPB7Toggle = bPB7Toggle ^ 0x80;
            }
            else
            {
                //One shot; Timed interrupt; Underflow nnnn ... 0001 0000 FFFF nnnn
                if ((delay & VIAPostOneShotA0) == 0)
                {
                    new_ifr |= VIA_INT_T1;
                    bPB7Toggle = bPB7Toggle ^ 0x80;
                }
                feed |= VIAPostOneShotA0;
                delay |= VIALoadA0;
            }
            
            
            if ((bPB7TimerMode & 0x80) != 0)
            {
                //timer 1 to PB7 enabled
                bPB7TimerOut = bPB7Toggle;
            }
        }
        
        if (delay & VIALoadB2)
        {
            timer2_counter.byte.hiByte = 0;
            timer2_counter.byte.hiByte = timer2_latch.byte.loByte;
        }
        else if (delay & VIACountB3)
        {
            timer2_counter.word -= 1;
        }
        
        if (timer2_counter.word==0 && (delay & VIACountB2))
        {
            if ((delay & VIAPostOneShotB0) == 0)
                new_ifr |= VIA_INT_T2;
            feed |= VIAPostOneShotB0;
        }
        
        if ((pcr & 0x0C) == 0x08)
        {
            if (delay & VIACA2Low0)
            {
                ca2_out = 0;
                SetCA2Output(0);
            }
            else
            {
                ca2_out = 1;
                SetCA2Output(1);
            }
        }
        
        if ((pcr & 0xC0) == 0x80)
        {
            if (delay & VIACB2Low0)
            {
                cb2_out = 0;
                SetCB2Output(0);
            }
            else
            {
                cb2_out = 1;
                SetCB2Output(1);
            }
        }
        
        if (delay & VIATrans1)
        {
            if (delay & VIACA1Trans1)
                TransitionCA1();
            if (delay & VIACA2Trans1)
                TransitionCA2();
            if (delay & VIACB1Trans1)
                TransitionCB1();
            if (delay & VIACB2Trans1)
                TransitionCB2();
        }
        
        ifr |= new_ifr;
        
        if (new_ifr & ier)
        {
            delay |= VIAInterrupt0;
        }
        
        if (delay & VIAInterrupt1)
        {
            SetSystemInterrupt();
        }
        
        delay = ((delay << 1) & VIADelayMask) | feed;
        
        if (delay==old_delay && feed==old_feed)
        {
            if ((timer1_counter.word < VIA_MINIMUM_GO_IDLE_TIME) || (((acr & 0x20)==0) && timer2_counter.word < VIA_MINIMUM_GO_IDLE_TIME))
            {
                no_change_count=0;
            }
            else
            {
                no_change_count++;
                if (no_change_count>VIA_NO_CHANGE_TO_IDLE)
                {
                    idle=1;
                    dec_2=(bit16) ((acr & 0x20)==0);
                }
            }
        }
        else
        {
            no_change_count=0;
        }
    }
}

void HoxsVIA::WakeUp()
{
    no_change_count=0;
    idle=0;
}

bit8 HoxsVIA::ReadRegister(bit16 address, ICLK sysclock)
{
    bit8 t;
    
    ExecuteCycle(sysclock);
    
    switch (address & 15)
    {
        case 0://port b
            switch ((pcr>>5) & 7)
        {
            case 0://cb2 int on neg edge, clear int on port b access
                ifr &= ~(VIA_INT_CB2);
                break;
            case 1://cb2 independent int on neg edge
                break;
            case 2://cb2 int on pos edge, clear int on port b access
                ifr &= ~(VIA_INT_CB2);
                break;
            case 3://cb2 independent int on pos edge
                break;
            case 4://handshake, cb2 goes low
                // No read handshake on port b
                break;
            case 5://pulse cb2 for 1 clock (not on read)
                // No read handshake on port b
                break;
            case 6://hold cb2 low
                break;
            case 7://hold cb2 hi
                break;
        }
            ifr &= ~(VIA_INT_CB1);
            if ((ifr & ier)==0)
                ClearSystemInterrupt();
            
            //TEST latchPortB
            if (((acr & 2) !=0) && bLatchB)
            {
                //Port latching
                bLatchB = false;
                t = irb;
            }
            else
            {
                //No latching
                t = ReadPinsPortB();
            }
            t = (((orb & ddrb) | (t & ~ddrb)) & ~bPB7TimerMode) | (bPB7TimerMode & bPB7TimerOut);
            return t;
        case 1://port a
            switch ((pcr>>1) & 7)
        {
                //ca1 int on neg edge
            case 0://ca2 int on neg edge, clear int on port a access
                ifr &= ~(VIA_INT_CA2);
                break;
            case 1://ca2 independent int on neg edge
                break;
            case 2://ca2 int on pos edge, clear int on port a access
                ifr &= ~(VIA_INT_CA2);
                break;
            case 3://ca2 independent int on pos edge
                break;
            case 4://handshake, ca2 goes low
                ifr &= ~(VIA_INT_CA2);
                delay |= VIACA2Low0;
                feed |= VIACA2Low0;
                WakeUp();
                break;
            case 5://pulse ca2 for 1 clock
                ifr &= ~(VIA_INT_CA2);
                delay |= VIACA2Low0;
                feed &= ~VIACA2Low0;
                WakeUp();
                break;
            case 6://hold ca2 low
                break;
            case 7://hold ca2 hi
                break;
        }
            ifr &= ~(VIA_INT_CA1);
            if ((ifr & ier)==0)
                ClearSystemInterrupt();
            
            //TEST latchPortA
            if (((acr & 1) !=0) && bLatchA)
            {
                //Port latching
                bLatchA = false;
                return ira;
            }
            else
            {
                //No latching
                return ReadPinsPortA();
            }
            
        case 2://ddrb
            return ddrb;
        case 3://ddra
            return ddra;
        case 4://t1c-l
            WakeUp();
            ifr &= (~VIA_INT_T1);
            if ((ifr & ier)==0)
                ClearSystemInterrupt();
            return timer1_counter.byte.loByte;
        case 5://t1c-h
            return timer1_counter.byte.hiByte;
        case 6://t1l-l
            return timer1_latch.byte.loByte;
        case 7://t1l-h
            return timer1_latch.byte.hiByte;
        case 8://t2c-l
            WakeUp();
            ifr &= (~VIA_INT_T2);
            if ((ifr & ier)==0)
                ClearSystemInterrupt();
            return timer2_counter.byte.loByte;
        case 9://t2c-h
            return timer2_counter.byte.hiByte;
        case 10://sr
            ifr &= (~VIA_INT_SER);
            if ((ifr & ier)==0)
                ClearSystemInterrupt();
            return shift;
        case 11://acr
            return acr;
        case 12://pcr
            return pcr;
        case 13://ifr
            if (ifr & ier)
            {
                SetSystemInterrupt();
                return 0x80 | ifr;
            }
            else
            {
                ClearSystemInterrupt();
                return ifr;
            }
        case 14://ier
            return ier | 0x80;
        case 15://port a no handshake
            switch ((pcr>>1) & 7)
        {
                //ca1 int on neg edge
            case 0://ca2 int on neg edge, clear int on port a access
                ifr &= ~(VIA_INT_CA2);
                break;
            case 1://ca2 independent int on neg edge
                break;
            case 2://ca2 int on pos edge, clear int on port a access
                ifr &= ~(VIA_INT_CA2);
                break;
            case 3://ca2 independent int on pos edge
                break;
            case 4://handshake, ca2 goes low
                ifr &= ~(VIA_INT_CA2);
                break;
            case 5://pulse ca2 for 1 clock
                ifr &= ~(VIA_INT_CA2);
                break;
            case 6://hold ca2 low
                break;
            case 7://hold ca2 hi
                break;
        }
            ifr &= ~(VIA_INT_CA1);
            if ((ifr & ier)==0)
                ClearSystemInterrupt();
            
            //TEST latchPortA
            if (((acr & 1) != 0) && bLatchA)
            {
                //Port latching
                bLatchA = false;
                return ira;
            }
            else
            {
                //No latching
                return ReadPinsPortA();
            }
        default:
            return 0;
    }
}

bit8 HoxsVIA::ReadRegister_no_affect(bit16 address, ICLK sysclock)
{
    bit8 t;
    ExecuteCycle(sysclock);
    
    switch (address & 15)
    {
        case 0://port b
            //TEST latchPortB
            if (((acr & 2) !=0) && bLatchB)
            {
                //Port latching
                t = irb;
            }
            else
            {
                //No latching
                t = ReadPinsPortB();
            }
            
            return (orb & ddrb) | (t & ~ddrb);
        case 1://port a
            //TEST latchPortA
            if (((acr & 1) !=0) && bLatchA)
            {
                //Port latching
                return ira;
            }
            else
            {
                //No latching
                return ReadPinsPortA();
            }
        case 4://t1c-l
            return timer1_counter.byte.loByte;
        case 5://t1c-h
            return timer1_counter.byte.hiByte;
        case 6://t1l-l
            return timer1_latch.byte.loByte;
        case 7://t1l-h
            return timer1_latch.byte.hiByte;
        case 8://t2c-l
            return timer2_counter.byte.loByte;
        case 9://t2c-h
            return timer2_counter.byte.hiByte;
        case 10://sr
            return shift;
        case 11://acr
            return acr;
        case 12://pcr
            return pcr;
        case 13://ifr
            if (ifr & ier)
            {
                return 0x80 | ifr;
            }
            else
            {
                return ifr;
            }
        case 15://port a no handshake
            //TEST latchPortA
            if (((acr & 1) !=0) && bLatchA)
            {
                //Port latching
                return ira;
            }
            else
            {
                //No latching
                return ReadPinsPortA();
            }
            break;
        default:
            return ReadRegister(address, sysclock);
    }
}

void HoxsVIA::WriteRegister(bit16 address, ICLK sysclock, bit8 data)
{
    ExecuteCycle(sysclock);
    
    switch (address & 15)
    {
        case 0://port b
            switch ((pcr>>5) & 7)
        {
            case 0://cb2 int on neg edge, clear int on port b access
                ifr &= ~(VIA_INT_CB2);
                break;
            case 1://cb2 independent int on neg edge
                break;
            case 2://cb2 int on pos edge, clear int on port b access
                ifr &= ~(VIA_INT_CB2);
                break;
            case 3://cb2 independent int on pos edge
                break;
            case 4://handshake, cb2 goes low
                ifr &= ~(VIA_INT_CB2);
                delay |= VIACB2Low0;
                feed |= VIACB2Low0;
                WakeUp();
                break;
            case 5://pulse cb2 for 1 clock (not on read)
                ifr &= ~(VIA_INT_CB2);
                delay |= VIACB2Low0;
                feed &= ~VIACB2Low0;
                WakeUp();
                break;
            case 6://hold cb2 low
                break;
            case 7://hold cb2 hi
                break;
        }
            ifr &= ~(VIA_INT_CB1);
            if ((ifr & ier) == 0)
                ClearSystemInterrupt();
            
            orb = data;
            SetPinsPortB(PortBOutput());
            break;
        case 1://port a
            switch ((pcr>>1) & 7)
        {
                //ca1 int on neg edge
            case 0://ca2 int on neg edge, clear int on port a access
                ifr &= ~(VIA_INT_CA2);
                break;
            case 1://ca2 independent int on neg edge
                break;
            case 2://ca2 int on pos edge, clear int on port a access
                ifr &= ~(VIA_INT_CA2);
                break;
            case 3://ca2 independent int on pos edge
                break;
            case 4://handshake, ca2 goes low
                ifr &= ~(VIA_INT_CA2);
                delay |= VIACA2Low0;
                feed |= VIACA2Low0;
                WakeUp();
                break;
            case 5://handshake pulse ca2 for 1 clock
                ifr &= ~(VIA_INT_CA2);
                delay |= VIACA2Low0;
                feed &= ~VIACA2Low0;
                WakeUp();
                break;
            case 6://hold ca2 low
                break;
            case 7://hold ca2 hi
                break;
        }
            ifr &= ~(VIA_INT_CA1);
            if ((ifr & ier) == 0)
                ClearSystemInterrupt();
            
            ora = data;
            SetPinsPortA(PortAOutput());
            break;
        case 2://ddrb
            ddrb = data;
            SetPinsPortB(PortBOutput());
            break;
        case 3://ddra
            ddra = data;
            SetPinsPortA(PortAOutput());
            break;
        case 4://t1c-l
            timer1_latch.byte.loByte = data;
            break;
        case 5://t1c-h
            //Load T1 latch high then load T1 counter from the T1 latch
            WakeUp();
            timer1_latch.byte.hiByte = data;
            timer1_counter.word = timer1_latch.word;
            
            ifr &= ~(VIA_INT_T1);
            if ((ifr & ier) == 0)
                ClearSystemInterrupt();
            
            delay |= VIALoadA2;
            delay &= ~VIACountA3;
            delay &= ~(VIACountA3 | VIALoadA1);
            feed &= ~VIAPostOneShotA0;
            delay &= ~VIAPostOneShotA0;
            
            //Toggle PB7 on write to Timer 1 counter high byte
            bPB7Toggle = bPB7TimerMode ^ 0x80;
            if ((bPB7TimerMode & 0x80) != 0)
            {
                //timer 1 to PB7 enabled
                bPB7TimerOut = bPB7Toggle;
            }
            break;
        case 6://t1l-l
            timer1_latch.byte.loByte = data;
            break;
        case 7://t1l-h
            timer1_latch.byte.hiByte = data;
            ifr &= (~VIA_INT_T1);
            if ((ifr & ier)==0)
                ClearSystemInterrupt();
            break;
        case 8://t2c-l
            timer2_latch.byte.loByte = data;
            break;
        case 9://t2c-h
            //Load T2 latch high then load T2 counter from the T2 latch
            WakeUp();
            timer2_counter.byte.hiByte = data;
            timer2_counter.byte.loByte = timer2_latch.byte.loByte;
            ifr &= ~(VIA_INT_T2);
            if ((ifr & ier) == 0)
                ClearSystemInterrupt();
            
            delay &= ~VIACountB3;
            delay &= ~(VIACountB3 | VIALoadB1);
            feed &= ~VIAPostOneShotB0;
            delay &= ~VIAPostOneShotB0;
            
            break;
        case 10://sr
            ifr &= (~VIA_INT_SER);
            if ((ifr & ier) == 0)
                ClearSystemInterrupt();
            shift = data;
            break;
        case 11://acr
            acr = data;
            if ((feed & VIAPostOneShotA0) != 0)
            {
                bPB7Toggle = bPB7TimerMode ^ 0x80;
            }
            if ((data & 0x20) == 0)
            {
                dec_2 = 1;
                feed |= VIACountB2;
                delay |= VIACountB2;
            }
            else
            {
                dec_2 = 0;
                delay &= ~(VIACountB2);
                feed &= ~VIACountB2;
            }
            if ((data & 0x80) == 0)
            {
                //timer 1 to PB7 disabled
                bPB7TimerMode = 0;
            }
            else
            {
                //timer 1 to PB7 enabled
                bPB7TimerMode = 0x80;
                bPB7TimerOut = bPB7Toggle;
            }
            
            //TEST latchPortA
            //TEST latchPortB
            if ((data & 1) == 0)
                bLatchA = false;
            if ((data & 2) == 0)
                bLatchB = false;
            
            WakeUp();
            break;
        case 12://pcr
            pcr = data;
            switch ((data>>1) & 7)
        {
                //ca1 int on neg edge
            case 0://ca2 int on neg edge, clear int on port a access
            case 1://ca2 independent int on neg edge
            case 2://ca2 int on pos edge, clear int on port a access
            case 3://ca2 independent int on pos edge
                delay &= ~VIACA2Low0;
                feed &= ~VIACA2Low0;
                ca2_out = 1;
                SetCA2Output(1);
                break;
            case 4://handshake, ca2 goes low
            case 5://pulse ca2 for 1 clock
                delay &= ~VIACA2Low0;
                feed &= ~VIACA2Low0;
                WakeUp();
                break;
            case 6://hold ca2 low
                delay |= VIACA2Low0;
                feed |= VIACA2Low0;
                ca2_out = 0;
                SetCA2Output(0);
                WakeUp();
                break;
            case 7://hold ca2 hi
                delay &= ~VIACA2Low0;
                feed &= ~VIACA2Low0;
                ca2_out = 1;
                SetCA2Output(1);
                WakeUp();
                break;
        }
            
            switch ((pcr>>5) & 7)
        {
                //cb1 int on neg edge
            case 0://cb2 int on neg edge, clear int on port a access
            case 1://cb2 independent int on neg edge
            case 2://cb2 int on pos edge, clear int on port a access
            case 3://cb2 independent int on pos edge
                delay &= ~VIACB2Low0;
                feed &= ~VIACB2Low0;
                cb2_out = 1;
                SetCB2Output(1);
                break;
            case 4://handshake, cb2 goes low
            case 5://pulse cb2 for 1 clock
                delay &= ~VIACB2Low0;
                feed &= ~VIACB2Low0;
                WakeUp();
                break;
            case 6://hold cb2 low
                delay |= VIACB2Low0;
                feed |= VIACB2Low0;
                cb2_out = 0;
                SetCB2Output(0);
                WakeUp();
                break;
            case 7://hold cb2 hi
                delay &= ~VIACB2Low0;
                feed &= ~VIACB2Low0;
                cb2_out = 1;
                SetCB2Output(1);
                WakeUp();
                break;
        }
            break;
        case 13://ifr
            ifr = ifr & ~data;
            if (ifr & ier)
                SetSystemInterrupt();
            else
                ClearSystemInterrupt();
            break;
        case 14://ier
            if (data & 0x80)
            {
                ier |= (data & 0x7f);
            }
            else
            {
                ier &= (~data & 0x7f);
            }
            if (ifr & ier)
                SetSystemInterrupt();
            else
                ClearSystemInterrupt();
            break;
        case 15://port a no handshake
            switch ((pcr>>1) & 7)
        {
                //ca1 int on neg edge
            case 0://ca2 int on neg edge, clear int on port a access
                ifr &= ~(VIA_INT_CA2);
                break;
            case 1://ca2 independent int on neg edge
                break;
            case 2://ca2 int on pos edge, clear int on port a access
                ifr &= ~(VIA_INT_CA2);
                break;
            case 3://ca2 independent int on pos edge
                break;
            case 4://no handshake, ca2 goes low
                ifr &= ~(VIA_INT_CA2);
                break;
            case 5://no handshake pulse ca2 for 1 clock
                ifr &= ~(VIA_INT_CA2);
                break;
            case 6://hold ca2 low
                break;
            case 7://hold ca2 hi
                break;
        }
            ifr &= ~(VIA_INT_CA1);
            if ((ifr & ier) == 0)
                ClearSystemInterrupt();
            
            ora = data;
            SetPinsPortA(PortAOutput());
            break;
    }
}

bit8 HoxsVIA::PortAOutput()
{
    return (~ddra) | (ora & ddra);
}

bit8 HoxsVIA::PortBOutput()
{
    return (orb & ddrb & ~bPB7TimerMode) | (bPB7TimerOut & ddrb & bPB7TimerMode) | (~ddrb);
}

void HoxsVIA::OnTransitionCA1Low()
{
}

void HoxsVIA::TransitionCA1()
{
    if (pcr & 1)
    {
        //CA1 positive active edge
        if (ca1_in_prev == 0)
        {
            ActiveTransitionCA1();
        }
    }
    else
    {
        //CA1 negative active edge
        if (ca1_in_prev != 0)
        {
            ActiveTransitionCA1();
        }
    }
    if (ca1_in_prev != 0)
    {
        OnTransitionCA1Low();
    }
    ca1_in_prev = !ca1_in_prev;
}

void HoxsVIA::TransitionCA2()
{
    switch ((pcr >> 1) & 7)
    {
        case 0://CA2 negative active edge
            if (ca2_in_prev != 0)
            {
                ActiveTransitionCA2();
            }
            break;
        case 1://CA2 independent interrupt negative active edge
            if (ca2_in_prev != 0)
            {
                ActiveTransitionCA2();
            }
        case 2://CA2 positive active edge
            if (ca2_in_prev == 0)
            {
                ActiveTransitionCA2();
            }
            break;
        case 3://CA2 independent interrupt positive active edge
            if (ca2_in_prev == 0)
            {
                ActiveTransitionCA2();
            }
            break;
    }
    //ca2_in = !ca2_in;
    ca2_in_prev = !ca2_in_prev;
}

void HoxsVIA::TransitionCB1()
{
    if (pcr & 0x10)
    {
        //CB1 positive active edge
        if (cb1_in_prev == 0)
        {
            ActiveTransitionCB1();
        }
    }
    else
    {
        //CB1 negative active edge
        if (cb1_in_prev != 0)
        {
            ActiveTransitionCB1();
        }
    }
    cb1_in_prev = !cb1_in_prev;
    //cb1_in = !cb1_in;
}

void HoxsVIA::TransitionCB2()
{
    switch ((pcr >> 5) & 7)
    {
        case 0://CB2 negative active edge
            if (cb2_in_prev != 0)
            {
                ActiveTransitionCB2();
            }
            break;
        case 1://CB2 independant interrupt negative active edge
            if (cb2_in_prev != 0)
            {
                ActiveTransitionCB2();
            }
            break;
        case 2://CB2 positive active edge
            if (cb2_in_prev == 0)
            {
                ActiveTransitionCB2();
            }
            break;
        case 3://CB2 independant interrupt positive active edge
            if (cb2_in_prev == 0)
            {
                ActiveTransitionCB2();
            }
            break;
    }
    cb2_in_prev = !cb2_in_prev;
    //cb2_in = !cb2_in;
}

void HoxsVIA::ActiveTransitionCA1()
{
    //TEST latchPortA
    if (((acr & 1) !=0) && !bLatchA)
    {
        bLatchA = true;
        ira = ReadPinsPortA();
    }
    
    if ((pcr & 0xe) == 0x8)
    {
        //Handshake CA2 CA2 goes high on CA1 active transition
        delay &= ~VIACA2Low0;
        feed &= ~VIACA2Low0;
        if (ca2_out==0)
        {
            ca2_out = 1;
            SetCA2Output(1);
        }
        WakeUp();
    }
    ifr |= VIA_INT_CA1;
    
    if (VIA_INT_CA1 & ier)
    {
        delay |= VIAInterrupt1;
        WakeUp();
    }
}

void HoxsVIA::ActiveTransitionCA2()
{
    ifr |= VIA_INT_CA2;
    
    if (VIA_INT_CA2 & ier)
    {
        delay |= VIAInterrupt1;
        WakeUp();
    }
}


void HoxsVIA::ActiveTransitionCB1()
{
    //TEST latchPortB
    if (((acr & 2) !=0) && !bLatchB)
    {
        bLatchB = true;
        irb = ReadPinsPortB();
    }
    
    if ((pcr & 0xe0) == 0x80)
    {
        //Handshake CB2 CB2 goes high on CB1 active transition
        delay &= ~VIACB2Low0;
        feed &= ~VIACB2Low0;
        if (cb2_out==0)
        {
            cb2_out = 1;
            SetCB2Output(1);
        }
        WakeUp();
    }
    
    ifr |= VIA_INT_CB1;
    
    if (VIA_INT_CB1 & ier)
    {
        delay |= VIAInterrupt1;
        WakeUp();
    }
}

void HoxsVIA::ActiveTransitionCB2()
{
    ifr |= VIA_INT_CB2;
    
    if (VIA_INT_CB2 & ier)
    {
        delay |= VIAInterrupt1;
        WakeUp();
    }
}

void HoxsVIA::SetCA1Input(bit8 value, int phase)
{
    ca1_in = value;
    if (phase == 0)
    {
        if (ca1_in_prev != value)
            delay = (delay) | VIACA1Trans1;
        else
            delay = delay & ~(VIACA1Trans1);
    }
    else
    {
        bit8 next_in = (delay & VIACA1Trans1) ? !ca1_in_prev : ca1_in_prev;
        if (next_in != value)
            delay|=VIACA1Trans0;
        else
            delay&=~VIACA1Trans0;
    }
    WakeUp();
}

void HoxsVIA::SetCA2Input(bit8 value, int phase)
{
    ca2_in = value;
    if (phase == 0)
    {
        if (ca2_in_prev != value)
            delay = (delay & ~(VIACA2Trans0 | VIACA2Trans1)) | VIACA2Trans1;
        else
            delay&=~VIACA2Trans1;
    }
    else
    {
        bit8 next_in = (delay & VIACA2Trans1) ? !ca2_in_prev : ca2_in_prev;
        if (next_in != value)
            delay|=VIACA2Trans0;
        else
            delay&=~VIACA2Trans0;
    }
}

void HoxsVIA::SetCB1Input(bit8 value, int phase)
{
    cb1_in = value;
    if (phase == 0)
    {
        if (cb1_in_prev != value)
            delay = (delay & ~(VIACB1Trans0 | VIACB1Trans1)) | VIACB1Trans1;
        else
            delay&=~VIACB1Trans1;
    }
    else
    {
        bit8 next_in = (delay & VIACB1Trans1) ? !cb1_in_prev : cb1_in_prev;
        if (next_in != value)
            delay|=VIACB1Trans0;
        else
            delay&=~VIACB1Trans0;
    }
}

void HoxsVIA::SetCB2Input(bit8 value, int phase)
{
    cb2_in = value;
    if (phase == 0)
    {
        if (cb2_in_prev != value)
            delay = (delay & ~(VIACB2Trans0 | VIACB2Trans1)) | VIACB2Trans1;
        else
            delay&=~VIACB2Trans1;
    }
    else
    {
        bit8 next_in = (delay & VIACB2Trans1) ? !cb2_in_prev : cb2_in_prev;
        if (next_in != value)
            delay|=VIACB2Trans0;
        else
            delay&=~VIACB2Trans0;
    }
}

ICLK HoxsVIA::GetCurrentClock()
{
    return CurrentClock;
}

void HoxsVIA::SetCurrentClock(ICLK sysclock)
{
    ICLK v = sysclock - CurrentClock;
    CurrentClock = sysclock;
    DevicesClock += v;
}


//
// HOXS VIA 1
//

HoxsVIA1::HoxsVIA1()
{
    // appStatus=0;
    // cpu=0;
    // disk=0;
}

// HRESULT VIA1::Init(int ID, CAppStatus *appStatus, CPUDisk *cpu, DiskInterface *disk)
bool HoxsVIA1::Init(int ID)
{
    this->ID = ID;
    // this->appStatus = appStatus;
    // this->cpu = cpu;
    // this->disk = disk;
    return true;
}

void HoxsVIA1::InitReset(ICLK sysclock, bool poweronreset)
{
    HoxsVIA::InitReset(sysclock, poweronreset);
}

void HoxsVIA1::Reset(ICLK sysclock, bool poweronreset)
{
    InitReset(sysclock, poweronreset);
    HoxsVIA::Reset(sysclock, poweronreset);
}

void HoxsVIA1::ExecuteDevices(ICLK sysclock)
{
}

bit8 HoxsVIA1::ReadPinsPortA()
{
    return 0x00;
    /*
    switch (appStatus->m_TrackZeroSensorStyle)
    {
        case HCFG::TZSSPullHigh:
            return 0x01;
        case HCFG::TZSSPullLow:
            return 0x00;
        case HCFG::TZSSPositiveHigh:
            if (disk->m_currentTrackNumber==0)
                return 0x01;
            else
                return 0x00;
        case HCFG::TZSSPositiveLow:
            if (disk->m_currentTrackNumber==0)
                return 0x00;
            else
                return 0x01;
        default:
            return 0x00;
    }
    */
}

bit8 HoxsVIA1::ReadPinsPortB()
{
    /*
    bit8 t;
    t=(disk->m_d64_serialbus & disk->m_c64_serialbus_diskview);
    t=(t >> 7) | ((t & 0x40) >> 4) | ((disk->m_c64_serialbus_diskview & 0x20) <<2);
    t|=0x1a;
    return ((t^0x85) | disk->m_d64_dipswitch);
    */
    
    // |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   |
    // -----------------------------------------------------------------
    // |  ATN  | Device addr.  |  ATN  | Clock | Clock | Data  | Data  |
    // |  in   |               |  out  |  out  |  in   |  out  |  in   |
    
    uint8_t external =
    (c64->floppy.iec->getAtnLine() ? 0x00 : 0x80) |
    (c64->floppy.iec->getClockLine() ? 0x00 : 0x04) |
    (c64->floppy.iec->getDataLine() ? 0x00 : 0x01);
    external &= 0x9F; // Device address 8
    
    return external;
}

void HoxsVIA1::SetCA2Output(bit8 value)
{
}

void HoxsVIA1::SetCB2Output(bit8 value)
{
}

void HoxsVIA1::SetPinsPortA(bit8 newPin)
{
    
}

void HoxsVIA1::SetPinsPortB(bit8 newPin)
{
    c64->floppy.iec->updateDevicePins(orb, ddrb);
    bool ATN = c64->iec.getAtnLine();
    SetCA1Input(ATN!=0, 1);//1
}

void HoxsVIA1::SetSystemInterrupt()
{
    c64->floppy.cpu.pullDownIrqLine(CPU::VIA);
    // cpu->Set_VIA1_IRQ(CurrentClock);
}

void HoxsVIA1::ClearSystemInterrupt()
{
    c64->floppy.cpu.releaseIrqLine(CPU::VIA);
    // cpu->Clear_VIA1_IRQ();
}


