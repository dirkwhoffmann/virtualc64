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

#pragma once

#include "SubComponent.h"

namespace vc64 {

/*
 * This implementation is mainly based on the document
 *
 *        "R6522 VERSATILE INTERFACE ADAPTER" by Frank Kontros [F. K.]
 *
 * and the Hoxs64 implementation by David Horrocks.
 */

#define VIACountA0       (1ULL << 0)  // Forces timer 1 to decrement every cycle
#define VIACountA1       (1ULL << 1)
#define VIACountB0       (1ULL << 2)  // Forces timer 2 to decrement every cycle
#define VIACountB1       (1ULL << 3)
#define VIAReloadA0      (1ULL << 4)  // Forces timer 1 to reload
#define VIAReloadA1      (1ULL << 5)
#define VIAReloadA2      (1ULL << 6)
#define VIAReloadB0      (1ULL << 7)  // Forces timer 2 to reload
#define VIAReloadB1      (1ULL << 8)
#define VIAReloadB2      (1ULL << 9)
#define VIAPostOneShotA0 (1ULL << 10) // Timer 1 fired in one shot mode
#define VIAPostOneShotB0 (1ULL << 11) // Ttimer 2 fired in one shot mode
#define VIAInterrupt0    (1ULL << 12) // Holds down the interrupt line
#define VIAInterrupt1    (1ULL << 13)
#define VIASetCA1out0    (1ULL << 14) // Sets CA2 pin high
#define VIASetCA1out1    (1ULL << 15)
#define VIAClearCA1out0  (1ULL << 16) // Sets CA2 pin low
#define VIAClearCA1out1  (1ULL << 17)
#define VIASetCA2out0    (1ULL << 18) // Sets CA2 pin high
#define VIASetCA2out1    (1ULL << 19)
#define VIAClearCA2out0  (1ULL << 20) // Sets CA2 pin low
#define VIAClearCA2out1  (1ULL << 21)
#define VIASetCB2out0    (1ULL << 22) // Sets CB2 pin high
#define VIASetCB2out1    (1ULL << 23)
#define VIAClearCB2out0  (1ULL << 24) // Sets CB2 pin low
#define VIAClearCB2out1  (1ULL << 25)
#define VIAPB7out0       (1ULL << 26) // Value of PB7 pin (if output is enabled)
#define VIAClrInterrupt0 (1ULL << 27) // Releases the interrupt line
#define VIAClrInterrupt1 (1ULL << 28)

#define VIAClearBits ~((1ULL << 29) | VIACountA0 | VIACountB0 | VIAReloadA0 | VIAReloadB0 | VIAPostOneShotA0 | VIAPostOneShotB0 | VIAInterrupt0 | VIASetCA1out0 | VIAClearCA1out0 | VIASetCA2out0 | VIAClearCA2out0 | VIASetCB2out0 | VIAClearCB2out0 | VIAPB7out0 | VIAClrInterrupt0)

class VIA6522 : public SubComponent {
    
    Descriptions descriptions = {{

        .type           = Class::VIA,
        .name           = "VIA",
        .description    = "VIA 6522",
        .shell          = "via"
    }};

    Options options = {

    };
    
    friend class Drive;
    friend class ParCable;
    
protected:
    
    // Owner of this VIA
    Drive &drive;


    //
    // Peripheral interface
    //
    
    /* Peripheral port A: "The Peripheral A port consists of 8 lines which can
     * be individually programmed to act as an input or an output under control
     * of a Data Direction Register. The polarity of output pins is controlled
     * by an Output Register and input data can be latched into an internal
     * register under control of the CA1 line."
     */
    u8 pa;
    
    /* Peripheral A control lines: "The two peripheral A control lines act as
     * interrupt inputs or as handshake outputs. Each line controls an internal
     * interrupt flag with a corresponding interrupt enable bit. In addition,
     * CA1controls the latching of data on Peripheral A Port input lines. The
     * various modes of operation are controlled by the system processor
     * through the internal control registers."
     */
    bool ca1;
    bool ca2;
    
    /* Peripheral port B: "The Peripheral B port consists of 8 lines which can
     * be individually programmed to act as an input or an output under control
     * of a Data Direction Register. The polarity of output pins is controlled
     * by an Output Register and input data can be latched into an internal
     * register under control of the CA1 line."
     */
    u8 pb;
    
    /* Peripheral B control lines: "The Peripheral B control lines act as
     * interrupt inputs or as handshake outputs. As with CA1 and CA2, each line
     * controls an interrupt flag with a corresponding interrupt enable bit. In
     * addition, these lines act as a serial port under control of the Shift
     * Register."
     */
    bool cb1;
    bool cb2;
    
    
    //
    // Port registers
    //
    
    /* Data direction registers: "Each port has a Data Direction Register
     * (DDRA, DDRB) for specifying whether the peripheral pins are to act as
     * inputs or outputs. A 0 in a bit of the Data Direction Register causes
     * the corresponding peripheral pin to act as an input. A 1 causes the pin
     * to act as an output."
     */
    u8 ddra;
    u8 ddrb;
    
    /* Output registers: "Each peripheral pin is also controlled by a bit in
     * the Output Register (ORA, ORB) and an Input Register (IRA, IRB). When
     * the pin is programmed to act as an output, the voltage on the pin is
     * controlled by the corre­sponding bit of the Output Register. A 1 in the
     * Output Register causes the pin to go high, and a 0 causes the pin to go
     * low. Data can be written into Output Register bits corresponding to pins
     * which are programmed to act as inputs; however, the pin will be
     * unaffected.
     */
    u8 ora;
    u8 orb;
    
    /* Input registers: "Reading a peripheral port causes the contents of the
     * Input Register (IRA, IRB) to be transferred onto the Data Bus. With
     * input latching disabled, IRA will always reflect the data on the PA pins.
     * With input latching enabled, IRA will reflect the contents of the Port A
     * prior to setting the CA1 Interrupt Flag (IFRl) by an active transition
     * on CA1.
     */
    u8 ira;
    u8 irb;
    
    
    //
    // Timers
    //
    
    /* VIA timer 1: "Interval Timer T1 consists of two 8-bit latches and a
     * 16-bit counter. The latches store data which is to be loaded into the
     * counter. After loading, the counter decrements at 02 clock rate. Upon
     * reaching zero, an interrupt flag is set, and IRQ goes low if the T1
     * interrupt is enabled. Timer 1 then disables any further interrupts or
     * automatically transfers the contents of the latches into the counter and
     * continues to decrement. In addition, the timer may be programmed to
     * invert the output signal on a peripheral pin (PB7) each time it
     * times-out."
     */
    u16 t1;          // T1C
    u8 t1_latch_lo;  // T1L_L
    u8 t1_latch_hi;  // T1L_H
    
    /* VIA timer 2: "Timer 2 operates as an interval timer (in the 'one-shot'
     * mode only), or as a counter for counting negative pulses on the PB6
     * peripheral pin. A single control bit in the Auxiliary Control Register
     * selects between these two modes. This timer is comprised of a
     * 'write-only' low-order latch (T2L-L), a 'read-only' low-order counter
     * (T2C-L) and a read/write high order counter (T2C-H). The counter
     * registers act as a 16-bit counter which decrements at 02 rate."
     */
    u16 t2;          // T1C
    u8 t2_latch_lo;  // T2L_L
    
    // Peripheral control register
    u8 pcr;
    
    // Auxiliary register
    u8 acr;
    
    // Interrupt enable register
    u8 ier;
    
    // Interrupt flag register
    u8 ifr;
    
    // Shift register
    u8 sr;
    
    // Event triggering queue
    u64 delay;
    
    /* New bits to feed in. Bits set in this variable makes a trigger event
     * persistent.
     */
    u64 feed;
    
    
    //
    // Speeding up emulation (sleep logic)
    //
    
    /* Idle counter. When the VIA state does not change during execution, this
     * variable is increased by one. If it exceeds a certain threshhold, the
     * chip is put into idle state via sleep().
     */
    u8 tiredness;
    
    // Wakeup cycle
    i64 wakeUpCycle;
    
    // Number of skipped executions
    i64 idleCounter;
    
    
    //
    // Initializing
    //
    
public:
    
    VIA6522(C64 &ref, Drive &drvref);
    virtual bool isVia1() const = 0;

    VIA6522& operator= (const VIA6522& other) {

        CLONE(pa)
        CLONE(ca1)
        CLONE(ca2)
        CLONE(pb)
        CLONE(cb1)
        CLONE(cb2)
        CLONE(ddra)
        CLONE(ddrb)
        CLONE(ora)
        CLONE(orb)
        CLONE(ira)
        CLONE(irb)
        CLONE(t1)
        CLONE(t1_latch_lo)
        CLONE(t1_latch_hi)
        CLONE(t2)
        CLONE(t2_latch_lo)
        CLONE(pcr)
        CLONE(acr)
        CLONE(ier)
        CLONE(ifr)
        CLONE(sr)
        CLONE(delay)
        CLONE(feed)
        CLONE(tiredness)
        CLONE(wakeUpCycle)
        CLONE(idleCounter)

        return *this;
    }


    //
    // Methods from Serializable
    //

public:

    template <class T>
    void serialize(T& worker)
    {
        worker

        << pa
        << ca1
        << ca2
        << pb
        << cb1
        << cb2
        << ddra
        << ddrb
        << ora
        << orb
        << ira
        << irb
        << t1
        << t1_latch_lo
        << t1_latch_hi
        << t2
        << t2_latch_lo
        << pcr
        << acr
        << ier
        << ifr
        << sr
        << delay
        << feed
        << tiredness
        << wakeUpCycle
        << idleCounter;

    } SERIALIZERS(serialize);


    //
    // Methods from CoreComponent
    //

public:

    const Descriptions &getDescriptions() const override { return descriptions; }

private:

    void _dump(Category category, std::ostream &os) const override;
    void _didReset(bool hard) override;

    
    //
    // Methods from Configurable
    //

public:

    const Options &getOptions() const override { return options; }


    //
    //
    //
    
public:
    
    // Returns true if this object emulates is VIA2
    bool isVia2() const;
    
    // Getters for the data directon registers
    u8 getDDRA() const { return ddra; }
    u8 getDDRB() const { return ddrb; }
    
    // Getters for peripheral ports
    u8 getPA() const { return pa; }
    u8 getPB() const { return pb; }
    
    // Getter for the peripheral control pins
    bool getCA2() const { return ca2; }
    bool getCB2() const { return cb2; }
    
    // Emulates the virtual VIA for one cycle
    void execute();
    
private:
    
    // Emulates a timer for one cycle
    void executeTimer1();
    void executeTimer2();
    
public:
    
    /* Special peek function for the I/O memory range. The peek function only
     * handles those registers that are treated similarly by both VIA chips.
     */
    virtual u8 peek(u16 addr);
    
protected:
    
    /* Special peek function for output register A. Variable handshake is
     * needed to distiguish if ORA is read via address 0x1 (handshake enabled)
     * or address 0xF (no handshake).
     */
    virtual u8 peekORA(bool handshake);
    
    // Special peek function for output register B
    u8 peekORB();
    
public:
    
    // Same as peek, but without side effects
    u8 spypeek(u16 addr) const;
    
    /* Special poke function for the I/O memory range. The poke function only
     * handles those registers that are treated similarly by both VIA chips.
     */
    void poke(u16 addr, u8 value);
    
protected:
    
    /* Special poke function for output register A. Variable handshake is
     * needed to distiguish if ORA is written via address 0x1 (handshake
     * enabled) or address 0xF (no handshake).
     */
    virtual void pokeORA(u8 value, bool handshake);
    
    // Special poke function for output register B
    void pokeORB(u8 value);
    
    // Special poke function for the PCR register
    void pokePCR(u8 value);
    
    
    //
    // Internal Configuration
    //
    
    // Returns true iff timer 1 is in free-run mode (continous interrupts)
    bool freeRun() const { return (acr & 0x40) != 0; }
    
    // Returns true iff timer 2 counts pulses on pin PB6
    bool countPulses() const { return (acr & 0x20) != 0; }
    
    // Returns true iff an output pulse is generated on each T1 load operation
    bool PB7OutputEnabled() const { return (acr & 0x80) != 0; }
    
    // Checks if input latching is enabled
    bool inputLatchingEnabledA() const { return (GET_BIT(acr,0)); }
    bool inputLatchingEnabledB() const { return (GET_BIT(acr,1)); }
    
    
    //
    // Peripheral Control Register (PCR)
    //
    
protected:
    
    // Reads the control bits from the peripheral control register
    u8 ca1Control() const { return pcr & 0x01; }
    u8 ca2Control() const { return (pcr >> 1) & 0x07; }
    u8 cb1Control() const { return (pcr >> 4) & 0x01; }
    u8 cb2Control() const { return (pcr >> 5) & 0x07; }
    
    
    //
    // Ports
    //
    
protected:
    
    // Bit values driving port A from inside the chip
    u8 portAinternal() const;
    
    // Bit values driving port A from outside the chip
    virtual u8 portAexternal() const = 0;
    
    // Updates variable pa with bit values visible at port A
    virtual void updatePA();
    
    // Bit values driving port B from inside the chip
    u8 portBinternal() const;
    
    // Bit values driving port B from outside the chip
    virtual u8 portBexternal() const = 0;
    
    // Updates variable pa with bit values visible at port B
    virtual void updatePB();
    
    
    //
    // Peripheral control lines
    //
    
public:
    
    // Schedules a transition on the CA1 pin for the next cycle
    void CA1action(bool value);
    
private:
    
    // Performs a transition on the CA1 pin
    void setCA1(bool value);
    
    
    //
    // Interrupt handling
    //
    
public:
    
    // Pulls down or releases the IRQ line
    virtual void pullDownIrqLine() = 0;
    virtual void releaseIrqLine() = 0;
    
    /* Releases the IRQ line if IFR and IER have no matching bits. This method
     * is invoked whenever a bit in the IFR or IER is is cleared.
     */
    void releaseIrqLineIfNeeded() { if ((ifr & ier) == 0) delay |= VIAClrInterrupt0; }
    
    /* |    7    |    6    |    5    |    4    |    3    |    2    |    1    |    0    |
     * ---------------------------------------------------------------------------------
     * |   IRQ   | Timer 1 | Timer 2 |   CB1   |   CB2   |Shift Reg|   CA1   |   CA2   |
     *
     *    Timer 1 - Set by:     Time-out of T1
     *              Cleared by: Read t1 low or write t1 high
     *    Timer 2 - Set by:     Time-out of T2
     *              Cleared by: Read t2 low or write t2 high
     *        CB1 - Set by:     Active edge on CB1
     *              Cleared by: Read or write to register 0 (ORB)
     *        CB2 - Set by:     Active edge on CB2
     *              Cleared by: Read or write to register 0 (ORB),
     *                             if CB2 is not selected as "INDEPENDENT".
     *  Shift Reg - Set by:     8 shifts completed
     *              Cleared by: Read or write to register 10 (0xA)
     *        CA1 - Set by:     Active edge on CA1
     *              Cleared by: Read or write to register 1 (ORA)
     *        CA2 - Set by:     Active edge on CA2
     *              Cleared by: Read or write to register 1 (ORA),
     *                             if CA2 is not selected as "INDEPENDENT".
     */
    
    // Sets or clears the Timer 1 interrupt flag
    void setInterruptFlag_T1() {
        if (!GET_BIT(ifr, 6) && GET_BIT(ier, 6)) delay |= VIAInterrupt0;
        SET_BIT(ifr, 6);
    }
    void clearInterruptFlag_T1() { CLR_BIT(ifr, 6); releaseIrqLineIfNeeded(); }
    
    // Sets or clears the Timer 2 interrupt flag
    void setInterruptFlag_T2() {
        if (!GET_BIT(ifr, 5) && GET_BIT(ier, 5)) delay |= VIAInterrupt0;
        SET_BIT(ifr, 5);
    }
    void clearInterruptFlag_T2() { CLR_BIT(ifr, 5); releaseIrqLineIfNeeded(); }
    
    // Sets or clears the CB1 interrupt flag
    void setInterruptFlag_CB1() {
        if (!GET_BIT(ifr, 4) && GET_BIT(ier, 4)) delay |= VIAInterrupt0;
        SET_BIT(ifr, 4);
    }
    void clearInterruptFlag_CB1() { CLR_BIT(ifr, 4); releaseIrqLineIfNeeded(); }
    
    // Clears the CB2 interrupt flag
    void clearInterruptFlag_CB2() { CLR_BIT(ifr, 3); releaseIrqLineIfNeeded(); }
    
    // Clears the Shift Register interrupt flag
    void clearInterruptFlag_SR() { CLR_BIT(ifr, 2); releaseIrqLineIfNeeded(); }
    
    // Clears the CB1 interrupt flag
    void clearInterruptFlag_CA1() { CLR_BIT(ifr, 1); releaseIrqLineIfNeeded(); }
    
    // Clears the CA2 interrupt flag
    void clearInterruptFlag_CA2() { CLR_BIT(ifr, 0); releaseIrqLineIfNeeded(); }
    
    
    //
    // Speeding up emulation
    //
    
    // Puts the VIA into idle state
    void sleep();
    
    // Emulates all previously skipped cycles
    void wakeUp();
};


/* First virtual VIA6522 controller. VIA1 serves as hardware interface between
 * the VC1541 CPU and the serial port (IEC bus).
 */
class VIA1 final : public VIA6522 {

public:
    
    VIA1(C64 &ref, Drive &drvref) : VIA6522(ref, drvref) { }
    ~VIA1() { }
    bool isVia1() const override { return true; }
    
    u8 peekORA(bool handshake) override;
    void pokeORA(u8 value, bool handshake) override;
    u8 portAexternal() const override;
    u8 portBexternal() const override;
    void updatePA() override;
    void updatePB() override;
    void pullDownIrqLine() override;
    void releaseIrqLine() override;
};

/* Second virtual VIA6522 controller. VIA2 serves as hardware interface between
 * the VC1541 CPU and the drive logic.
 */
class VIA2 final : public VIA6522 {

public:
    
    VIA2(C64 &ref, Drive &drvref) : VIA6522(ref, drvref) { }
    ~VIA2() { }
    bool isVia1() const override { return false; }

    u8 portAexternal() const override;
    u8 portBexternal() const override;
    void updatePB() override;
    void pullDownIrqLine() override;
    void releaseIrqLine() override;
};

}
