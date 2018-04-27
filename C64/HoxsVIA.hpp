//
//  HoxsVIA.hpp
//  VirtualC64
//
//  Created by Dirk Hoffmann on 27.04.18.
//

#ifndef HoxsVIA_hpp
#define HoxsVIA_hpp

#include <stdio.h>
#include <stdint.h>

typedef int8_t bit8;
typedef int16_t bit16;

typedef union {
    
    unsigned short word;
    struct {
        unsigned char loByte;
        unsigned char hiByte;
    } byte;
    
} bit16u;

typedef int64_t ICLK;
typedef int64_t ICLKS;

#define VIA_SER_IDLE        0x1
#define VIA_SER_DISABLED    0
#define VIA_SER_IN_T2        0x4
#define VIA_SER_IN_02        0x8
#define VIA_SER_IN_CB1        0xc0
#define VIA_SER_OUT_T2_CIRC    0x10
#define VIA_SER_OUT_T2        0x14
#define VIA_SER_OUT_02        0x18
#define VIA_SER_OUT_CB1        0x1c

#define VIA_INT_CA2 1
#define VIA_INT_CA1    2
#define VIA_INT_SER    4
#define VIA_INT_CB2    8
#define VIA_INT_CB1    16
#define VIA_INT_T2    32
#define VIA_INT_T1    64
#define VIA_INT_IRQ    128


#define VIACountA0         0x000000001ULL
#define VIACountA1         0x000000002ULL
#define VIACountA2         0x000000004ULL
#define VIACountA3         0x000000008ULL
#define VIACountB0         0x000000010ULL
#define VIACountB1         0x000000020ULL
#define VIACountB2         0x000000040ULL
#define VIACountB3         0x000000080ULL
#define VIALoadA0          0x000000100ULL
#define VIALoadA1          0x000000200ULL
#define VIALoadA2          0x000000400ULL
#define VIALoadB0          0x000000800ULL
#define VIALoadB1          0x000001000ULL
#define VIALoadB2          0x000002000ULL
#define VIACA2Low0         0x000004000ULL
#define VIACA2Low1         0x000008000ULL
#define VIACB2Low0         0x000010000ULL
#define VIACB2Low1         0x000020000ULL
#define VIAPB7Low0         0x000040000ULL
#define VIAPB7Low1         0x000080000ULL
#define VIAInterrupt0      0x000100000ULL
#define VIAInterrupt1      0x000200000ULL
#define VIAOneShotA0       0x000400000ULL
#define VIAOneShotB0       0x000800000ULL
#define VIAPostOneShotA0   0x001000000ULL
#define VIAPostOneShotB0   0x002000000ULL

#define VIACA1Trans0 (1ULL << 26)
#define VIACA1Trans1 (1ULL << 27)
#define VIACA2Trans0 (1ULL << 28)
#define VIACA2Trans1 (1ULL << 29)
#define VIACB1Trans0 (1ULL << 30)
#define VIACB1Trans1 (1ULL << 31)
#define VIACB2Trans0 (1ULL << 32)
#define VIACB2Trans1 (1ULL << 33)

#define VIAMaskMSB         (1ULL << 34)

#define VIADelayMask     ~(VIAMaskMSB | VIACountA0 | VIACountB0 | VIALoadA0 | VIALoadB0 | VIACA2Low0 | VIACB2Low0 | VIAPB7Low0 | VIAInterrupt0 | VIAOneShotA0 | VIAOneShotB0 | VIAPostOneShotA0 | VIAPostOneShotB0 | VIACA1Trans0 | VIACA2Trans0 | VIACB1Trans0 | VIACB2Trans0)

#define VIATrans1 (VIACA1Trans1 | VIACA2Trans1 | VIACB1Trans1 | VIACB2Trans1)

#define VIA_NO_CHANGE_TO_IDLE 4
#define VIA_MINIMUM_STAY_IDLE 3
#define VIA_MINIMUM_GO_IDLE_TIME 5

class HoxsVIA
{
    
    ICLK CurrentClock;
    ICLK DeviceClock;

    
public:
    virtual void SetCA2Output(bit8)=0;
    virtual void SetCB2Output(bit8)=0;
    virtual bit8 ReadPinsPortA()=0;
    virtual bit8 ReadPinsPortB()=0;
    virtual void SetPinsPortA(bit8 newPin)=0;
    virtual void SetPinsPortB(bit8 newPin)=0;
    virtual void SetSystemInterrupt()=0;
    virtual void ClearSystemInterrupt()=0;
    
    void InitReset(ICLK sysclock, bool poweronreset);
    //IRegister
    virtual void Reset(ICLK sysclock, bool poweronreset);
    virtual void ExecuteCycle(ICLK sysclock);
    virtual bit8 ReadRegister(bit16 address, ICLK sysclock);
    virtual void WriteRegister(bit16 address, ICLK sysclock, bit8 data);
    virtual bit8 ReadRegister_no_affect(bit16 address, ICLK sysclock);
    virtual ICLK GetCurrentClock();
    virtual void SetCurrentClock(ICLK sysclock);
    
    virtual void ExecuteDevices(ICLK sysclock)=0;
    
    bit8 PortAOutput();
    bit8 PortBOutput();
    void SetCA1Input(bit8, int phase);
    void SetCA2Input(bit8, int phase);
    void SetCB1Input(bit8, int phase);
    void SetCB2Input(bit8, int phase);
    void WakeUp();
    void SetPortA();
    void SetPortB();
    
protected:
    virtual void OnTransitionCA1Low();
    
public:
    int ID;
    ICLK DevicesClock;
    
    bool bLatchA;
    bool bLatchB;
    
    bit8 ora;
    bit8 ira;
    bit8 orb;
    bit8 irb;
    bit8 ddra;
    bit8 ddrb;
    bit16u timer1_counter;
    bit16u timer2_counter;
    bit16u timer1_latch;
    bit16u timer2_latch;
    bit8 acr;
    bit8 pcr;
    bit8 ca1_in;
    bit8 ca1_in_prev;
    bit8 ca2_in;
    bit8 ca2_in_prev;
    bit8 cb1_in;
    bit8 cb1_in_prev;
    bit8 cb2_in;
    bit8 cb2_in_prev;
    bit8 ca2_out;
    bit8 cb2_out;
    bit8 shift;
    bit8 ifr;
    bit8 ier;
    bit8 serial_active;
    bit8 serial_mode;
    uint64_t delay;
    uint64_t feed;
    uint64_t old_delay;
    uint64_t old_feed;
    bit8 modulo;
    bit8 Interrupt;
    
    bit8 bPB7TimerMode;
    bit8 bPB7Toggle;
    bit8 bPB7TimerOut;
    
    bit8 no_change_count;
    bit16 dec_2;
    bit8 idle;
    
private:
    void ActiveTransitionCA1();
    void ActiveTransitionCA2();
    void ActiveTransitionCB1();
    void ActiveTransitionCB2();
    
    void TransitionCA1();
    void TransitionCA2();
    void TransitionCB1();
    void TransitionCB2();
    
};

#endif /* HoxsVIA_hpp */
