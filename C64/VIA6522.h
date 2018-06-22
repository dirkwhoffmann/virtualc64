/*!
 * @header      VIA.h
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @details     The implementation is mainly based on the document
 *              "R6522 VERSATILE INTERFACE ADAPTER" by Frank Kontros [F. K.]
 *              and the Hoxs64 implementation by David Horrocks.
 */
/* This program is free software; you can redistribute it and/or modify
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

#ifndef _VIA6522_INC
#define _VIA6522_INC

#include "VirtualComponent.h"

class VC1541;

#define VIACountA0       (1ULL << 0) // Forces timer 1 to decrement every cycle
#define VIACountA1       (1ULL << 1)
#define VIACountB0       (1ULL << 2) // Forces timer 2 to decrement every cycle
#define VIACountB1       (1ULL << 3)
#define VIAReloadA0      (1ULL << 4) // Forces timer 1 to reload
#define VIAReloadA1      (1ULL << 5)
#define VIAReloadA2      (1ULL << 6)
#define VIAReloadB0      (1ULL << 7) // Forces timer 2 to reload
#define VIAReloadB1      (1ULL << 8)
#define VIAReloadB2      (1ULL << 9)
#define VIAPostOneShotA0 (1ULL << 10) // Indicates that timer 1 has fired in one shot mode
#define VIAPostOneShotB0 (1ULL << 11) // Indicates that timer 2 has fired in one shot mode
#define VIAInterrupt0    (1ULL << 12) // Triggers an interrupt
#define VIAInterrupt1    (1ULL << 13)
#define VIASetCA2out0    (1ULL << 14) // Sets CA2 pin high
#define VIASetCA2out1    (1ULL << 15)
#define VIAClearCA2out0  (1ULL << 16) // Sets CA2 pin low
#define VIAClearCA2out1  (1ULL << 17)
#define VIASetCB2out0    (1ULL << 18) // Sets CB2 pin high
#define VIASetCB2out1    (1ULL << 19)
#define VIAClearCB2out0  (1ULL << 20) // Sets CB2 pin low
#define VIAClearCB2out1  (1ULL << 21)
#define VIAPB7out0       (1ULL << 22) // Current value of PB7 pin (if output is enabled)

#define VIAClearBits   ~((1ULL << 23) | VIACountA0 | VIACountB0 | VIAReloadA0 | VIAReloadB0 | VIAPostOneShotA0 | VIAPostOneShotB0 | VIAInterrupt0 | VIASetCA2out0 | VIAClearCA2out0 | VIASetCB2out0 | VIAClearCB2out0 | VIAPB7out0)

/*! @brief    Virtual VIA6522 controller
    @details  The VC1541 drive contains two VIAs on its logic board.
 */
class VIA6522 : public VirtualComponent {
	
public:
    
    //
    // Peripheral interface
    //
    
    //! @brief    Peripheral port A
    /*! @details  "The Peripheral A port consists of 8 lines which can be
     *             individually programmed to act as an input or an output
     *             under control of a Data Direction Register. The polarity
     *             of output pins is controlled by an Output Register and
     *             input data can be latched into an internal register under
     *             control of the CA1 line."
     */
    uint8_t pa;
    
    //! @brief    Peripheral A control lines
    /*! @details  "The two peripheral A control lines act as interrupt inputs
     *             or as handshake outputs. Each line controls an internal
     *             interrupt flag with a corresponding interrupt enable bit.
     *             In addition, CA1controls the latching of data on
     *             Peripheral A Port input lines. The various modes of
     *             operation are controlled by the system processor through
     *             the internal control registers."
     */
    bool ca1;
    bool ca2;
    bool ca2_out;
    
    //! @brief    Peripheral port B
    /*! @details  "The Peripheral B port consists of 8 lines which can be
     *             individually programmed to act as an input or an output
     *             under control of a Data Direction Register. The polarity
     *             of output pins is controlled by an Output Register and
     *             input data can be latched into an internal register under
     *             control of the CA1 line."
     */
    uint8_t pb;
    
    //! @brief
    /*! @details  "The Peripheral B control lines act as interrupt inputs or
     *             as handshake outputs. As with CA1 and CA2, each line
     *             controls an interrupt flag with a corresponding interrupt
     *             enable bit. In addition, these lines act as a serial port
     *             under control of the Shift Register."
     */
    bool cb1;
    bool cb2;
    bool cb2_out;
     
    //
    // Port registers
    //
    
public:
    
    //! @brief    Data direction registers
    /*! @details  "Each port has a Data Direction Register (DDRA, DDRB) for
     *             specifying whether the peripheral pins are to act as
     *             inputs or outputs. A 0 in a bit of the Data Direction
     *             Register causes the corresponding peripheral pin to act
     *             as an input. A 1 causes the pin to act as an output."
     */
    uint8_t ddra;
    uint8_t ddrb;

    //! @brief    Output registers
    uint8_t ora;
    uint8_t orb;

    //! @brief    Input registers
    uint8_t ira;
    uint8_t irb;

public:
    
    //
    // Timers
    //
    
	/*! @brief    VIA timer 1
	 *  @details  "Interval Timer T1 consists of two 8-bit latches and a
     *             16-bit counter. The latches store data which is to be
     *             loaded into the counter. After loading, the counter
     *             decrements at 02 clock rate. Upon reaching zero, an
     *             interrupt flag is set, and IRQ goes low if the T1
     *             interrupt is enabled. Timer 1 then disables any further
     *             interrupts or automatically transfers the contents of
     *             the latches into the counter and continues to decrement.
     *             In addition, the timer may be programmed to invert the
     *             output signal on a peripheral pin (PB7) each time it
     *             "times-out."
     */
    uint16_t t1; // T1C
    uint8_t t1_latch_lo; // T1L_L
    uint8_t t1_latch_hi; // T1L_H

	/*! @brief    VIA timer 2
	 *  @details  "Timer 2 operates as an interval timer (in the "one-shot"
     *             mode only), or as a counter for counting negative pulses
     *             on the PB6 peripheral pin. A single control bit in the
     *             Auxiliary Control Register selects between these two
     *             modes. This timer is comprised of a "write-only" low-order
     *             latch (T2L-L), a "read-only" low-order counter (T2C-L) and
     *             a read/write high order counter (T2C-H). The counter
     *             registers act as a 16-bit counter which decrements at
     *             02 rate."
     */
    uint16_t t2; // T1C
    uint8_t t2_latch_lo; // T2L_L
	        
    //! @brief    Peripheral control register
    uint8_t pcr;

    //! @brief    Auxiliary register
    uint8_t acr;

    //! @brief    Interrupt enable register
    uint8_t ier;

    //! @brief    Interrupt flag register
    uint8_t ifr;

    //! @brief    Shift register
    uint8_t sr;
    
    //! @brief    Event triggering queue
    uint64_t delay;
    
    //! @brief    New bits to feed in
    //! @details  Bits set in this variable makes a trigger event persistent.
    uint64_t feed;
    
public:	
	//! @brief    Constructor
	VIA6522();
	
	//! @brief    Destructor
	~VIA6522();
		
	//! @brief    Brings the VIA back to its initial state.
	void reset();

    //! @brief    Dumps debug information.
    void dumpState();

    //! @brief    Executes the virtual VIA for one cycle.
    void execute(); 

    //! @brief    Executes timer 1 for one cycle.
    void executeTimer1();

    //! @brief    Executes timer 2 for one cycle.
    void executeTimer2();
	
	/*! @brief    Special peek function for the I/O memory range
	 *  @details  The peek function only handles those registers that are treated
     *            similarly by both VIA chips
     */
	virtual uint8_t peek(uint16_t addr);
	
    //! @brief    Special peek function for output register A
    /*! @details  Variable handshake is needed to distiguish if ORA is read
     *            via address 0x1 (handshake enabled) or address 0xF (no handshake).
     */
    uint8_t peekORA(bool handshake);

    //! @brief    Special peek function for output register B
    uint8_t peekORB();
    
    //! @brief    Same as peek, but without side effects
    virtual uint8_t spypeek(uint16_t addr);
    
	/*! @brief    Special poke function for the I/O memory range
	 *  @details  The poke function only handles those registers that are treated
     *            similarly by both VIA chips
     */
	virtual void poke(uint16_t addr, uint8_t value);

    //! @brief    Special poke function for output register A
    /*! @details  Variable handshake is needed to distiguish if ORA is written
     *            via address 0x1 (handshake enabled) or address 0xF (no handshake).
     */
    void pokeORA(uint8_t value, bool handshake);
    
    //! @brief    Special poke function for output register B
    void pokeORB(uint8_t value);

    //! @brief    Special poke function for the PCR register
    void pokePCR(uint8_t value);
    
    
    // ----------------------------------------------------------------------------------------
    //                                Internal Configuration
    // ----------------------------------------------------------------------------------------

    //! @brief    Returns true iff timer 1 is in free-run mode (continous interrupts)
    bool freeRun() { return (acr & 0x40) != 0; }

    //! @brief    Returns true iff timer 2 counts pulses on pin PB6
    bool countPulses() { return (acr & 0x20) != 0; }
    
    //! @brief    Returns true iff an output pulse is generated on each T1 load operation
    bool PB7OutputEnabled() { return (acr & 0x80) != 0; }
    
    //! @brief    Checks if input latching is enabled
    bool inputLatchingEnabledA() { return (GET_BIT(acr,0)); }

    //! @brief    Checks if input latching is enabled
    bool inputLatchingEnabledB() { return (GET_BIT(acr,1)); }

    // ----------------------------------------------------------------------------------------
    //                         Peripheral Control Register (PCR)
    // ----------------------------------------------------------------------------------------
    
    //! @brief    Returns the current value of the peripheral control register
    uint8_t getPcr() { return pcr & 0x20; }

    //! @brief    Returns the CA1 control bit of the peripheral control register
    uint8_t ca1Control() { return (pcr >> 0) & 0x01; }
    
    //! @brief    Returns the three CA2 control bits of the peripheral control register
    uint8_t ca2Control() { return (pcr >> 1) & 0x07; }

    //! @brief    Returns the CB1 control bit of the peripheral control register
    uint8_t cb1Control() { return (pcr >> 4) & 0x01; }
    
    //! @brief    Returns the three CB2 control bits of the peripheral control register
    uint8_t cb2Control() { return (pcr >> 5) & 0x07; }

    
    // ----------------------------------------------------------------------------------------
    //                                        Ports
    // ----------------------------------------------------------------------------------------

    //! @brief   Bit values driving port A from inside the chip
    virtual uint8_t portAinternal() = 0;

    //! @brief   Bit values driving port A from outside the chip
    virtual uint8_t portAexternal() = 0;

    /*! @brief   Computes the current bit values visible at port A
     *  @details Value is stored in variable pa
     */
    virtual void updatePA();

    //! @brief   Bit values driving port B from inside the chip
    virtual uint8_t portBinternal();
    
    //! @brief   Bit values driving port B from outside the chip
    virtual uint8_t portBexternal() = 0;
    
    /*! @brief   Computes the current bit values visible at port B
     *  @details Value is stored in variable pb
     */
    virtual void updatePB();
    
    //! Returns the current value on chip pin CA2
    /*
    // DEPRECATED
    bool CA2() {
        switch ((pcr >> 1) & 0x07) {
            case 6: return false; // LOW OUTPUT
            case 7: return true; // HIGH OUTPUT
            default:
                warn("UNUSAL OPERATION MODE FOR CA2 DETECTED");
                return false;
        }
    }
    */
    
    // ----------------------------------------------------------------------------------------
    //                              Peripheral control lines
    // ----------------------------------------------------------------------------------------

    void setCA1(bool value);
    void setCA2(bool value);
    void setCB1(bool value);
    void setCB2(bool value);
    virtual void setCA2out(bool value);
    virtual void setCB2out(bool value);

    
    
    // ----------------------------------------------------------------------------------------
    //                                   Interrupt handling
    // ----------------------------------------------------------------------------------------

    /*! @brief    Updates the IRQ line
     *  @details  This method is invoked whenever register IFR or register IER changes.
     */
    void IRQ();

    //
    // |    7    |    6    |    5    |    4    |    3    |    2    |    1    |    0    |
    // ---------------------------------------------------------------------------------
    // |   IRQ   | Timer 1 | Timer 2 |   CB1   |   CB2   |Shift Reg|   CA1   |   CA2   |

    // Timer 1 - Set by:     Time-out of T1
    //           Cleared by: Read t1 low or write t1 high
    
    void setInterruptFlag_T1() { SET_BIT(ifr,6); IRQ(); }
    void clearInterruptFlag_T1() { CLR_BIT(ifr,6); IRQ(); }

    // Timer 2 - Set by:     Time-out of T2
    //           Cleared by: Read t2 low or write t2 high
    
    void setInterruptFlag_T2() { SET_BIT(ifr,5); IRQ(); }
    void clearInterruptFlag_T2() { CLR_BIT(ifr,5); IRQ(); }

    // CB1 - Set by:     Active edge on CB1
    //       Cleared by: Read or write to register 0 (ORB)
    
    void setInterruptFlag_CB1() { SET_BIT(ifr,4); IRQ(); }
    void clearInterruptFlag_CB1() { CLR_BIT(ifr,4); IRQ(); }

    // CB2 - Set by:     Active edge on CB2
    //       Cleared by: Read or write to register 0 (ORB) (only if CB2 is not selected as "INDEPENDENT")
    
    void setInterruptFlag_CB2() { SET_BIT(ifr,3); IRQ(); }
    void clearInterruptFlag_CB2() { CLR_BIT(ifr,3); IRQ(); }
    
    // Shift register - Set by:     8 shifts completed
    //                  Cleared by: Read or write to register 10 (0xA) (shift register)
    
    void setInterruptFlag_SR() { SET_BIT(ifr,2); IRQ(); }
    void clearInterruptFlag_SR() { CLR_BIT(ifr,2); IRQ(); }

    // CA1 - Set by:     Active edge on CA1
    //       Cleared by: Read or write to register 1 (ORA)
    
    void setInterruptFlag_CA1() { SET_BIT(ifr,1); IRQ(); }
    void clearInterruptFlag_CA1() { CLR_BIT(ifr,1); IRQ(); }

    // CA2 - Set by:     Active edge on CA2
    //       Cleared by: Read or write to register 1 (ORA) (only if CA2 is not selected as "INDEPENDENT")
    
    void setInterruptFlag_CA2() { SET_BIT(ifr,0); IRQ(); }
    void clearInterruptFlag_CA2() { CLR_BIT(ifr,0); IRQ(); }
};


/*! @brief   First virtual VIA6522 controller
 *  @details VIA1 serves as hardware interface between the VC1541 CPU
 *           and the IEC bus.
 */
class VIA1 : public VIA6522 {
	
public:

	VIA1();
	~VIA1();
    
    uint8_t portAinternal();
    uint8_t portAexternal();
    uint8_t portBexternal();
    void updatePB();
};

/*! @brief   Second virtual VIA6522 controller
 *  @details VIA2 serves as hardware interface between the VC1541 CPU
 *           and the drive logic.
 */
class VIA2 : public VIA6522 {
	
public:

	VIA2();
	~VIA2();
 
    uint8_t portAinternal();
    uint8_t portAexternal();
    uint8_t portBexternal();
    void updatePB();
};

#endif
