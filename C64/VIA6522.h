/*!
 * @header      VIA.h
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   2008 - 2016 Dirk W. Hoffmann
 * @brief       Declares VC1541 class
 * @details     The implementation is mainly based on the document
 *              "R6522 VERSATILE INTERFACE ADAPTER" by Frank Kontros [F. K.]
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

/*! @brief   Virtual VIA6522 controller
    @details The VC1541 drive contains two VIAs on its logic board.
 */
class VIA6522 : public VirtualComponent {
	
public:
	
	//! @brief Reference to the connected disk drive.
	VC1541 *floppy;

public:
	
	//! @brief Peripheral ports
	/*! @details 
     * "The  R6522  VIA  has  two  8-bit  bidirectional  I/O ports (Port A and Port B)
     *  and each port has two associated control lines.
     *
	 *	Each  8-bit  peripheral  port  has  a Data Direction Register (DDRA, DDRB) for
	 * 	specifying  whether  the  peripheral pins are to act as inputs or outputs. A 0
     *  in  a  bit  of the Data Direction Register causes the corresponding peripheral
     *  pin to act as an input. A 1 causes the pin to act as an output.
     *
     *  Each  peripheral  pin  is  also controlled  by  a  bit in the Output Register
	 *	(ORA,  ORB)  and  the Input Register (IRA, IRB). When the pin is programmed as
	 *	an  output,  the  voltage on the pin is controlled by the corresponding bit of
	 *	the  Output  Register.  A  1  in  the  Output Register causes the output to go
	 * 	high,  and  a  0  causes the output to go low. Data may be written into Output
	 *	Register  bits  corresponding  to pins which are programmed as inputs. In this
	 *	case, however, the output signal is unaffected." [F. K.]
     */
	uint8_t ddra, ddrb;
	uint8_t ira, irb;
    uint8_t ora, orb;
	
// protected:
public:
    
	//! VIA I/O Memory
	/*! Whenever a value is poked to the VIA address space, it is stored here. */
	uint8_t io[16];
		
	//! VIA timer 1
	/*! Interval  Timer  T1  consists  of  two  8-bit latches and a 16-bit
		counter.  The  latches store data which is to be loaded into the
		counter.  After  loading,  the  counter  decrements  at  02  clock  rate. Upon
		reaching  zero,  an  interrupt  flag  is  set,  and  IRQ  goes  low  if the T1
		interrupt  is  enabled.  Timer  1  then  disables  any  further  interrupts or
		automatically  transfers  the  contents  of  the  latches into the counter and
		continues  to  decrement.  In  addition, the timer may be programmed to invert
		the  output  signal  on  a peripheral pin (PB7) each time it "times-out". Each
		of these modes is discussed separately below.
	*/

    uint16_t t1;
    uint8_t t1_latch_lo, t1_latch_hi;

	//! VIA timer 2
	/*! Timer  2  operates  as  an interval timer (in the "one-shot" mode only), or as
		a  counter  for  counting  negative pulses on the PB6 peripheral pin. A single
		control  bit  in  the  Auxiliary  Control  Register  selects between these two
		modes.  This  timer  is comprised of a "write-only" low-order latch (T2L-L), a
		"read-only"  low-order  counter  (T2C-L)  and  a read/write high order counter
		(T2C-H).  The  counter  registers  act as a 16-bit counter which decrements at
		02 rate.
	*/
    uint16_t t2;
	uint8_t t2_latch_lo;
	
    //! Indicates that timer 1 or timer 2 has reached zero
    bool t1_underflow;
    bool t2_underflow;
    
public:	
	//! Constructor
	VIA6522();
	
	//! Destructor
	~VIA6522();
		
	//! Bring the VIA back to its initial state
	void reset();

    //! Dump debug information
    void dumpState();

    //! Execute virtual VIA for one cycle
    inline void execute() {
        if (t1 || t1_underflow) executeTimer1();
        if (t2 || t2_underflow) executeTimer2();
    }

    //! Execution function for timer 1
    void executeTimer1();

    //! Execution function for timer 2
    void executeTimer2();
	
	//! Special peek function for the I/O memory range
	/*! The peek function only handles those registers that are treated similarily by both VIA chips */
	virtual uint8_t peek(uint16_t addr);
	
	//! Special poke function for the I/O memory range
	/*! The poke function only handles those registers that are treated similarily by both VIA chips */
	virtual void poke(uint16_t addr, uint8_t value);

    // -----------------------------------------------------------------------------------------------
    //                                Internal Configuration
    // -----------------------------------------------------------------------------------------------

    //! Returns true iff timer 1 is in free-run mode (continous interrupts)
    bool freeRunMode1() { return (io[0x0B] & 0x40) != 0; }

    //! Check if input latching is enabled
    bool inputLatchingEnabledA() { return (GET_BIT(io[0x0B],0)); }
    bool inputLatchingEnabledB() { return (GET_BIT(io[0x0B],1)); }

    
    // -----------------------------------------------------------------------------------------------
    //                                        Ports
    // -----------------------------------------------------------------------------------------------

    //! Returns the current value on chip pin CA2
    bool CA2() {
        switch ((io[0xC] >> 1) & 0x07) {
            case 6: return false; // LOW OUTPUT
            case 7: return true; // HIGH OUTPUT
            default:
                warn("UNUSAL OPERATION MODE FOR CA2 DETECTED");
                return false;
        }
    }
    
    // -----------------------------------------------------------------------------------------------
    //                                   Interrupt handling
    // -----------------------------------------------------------------------------------------------

    // Returns the value of the IRQ pin
    // This method updates the IRQ pin of the connected CPU as a side effect and is therefore
    // invoked on every change in register IFR or register IER.
    bool IRQ();

    //
    // |    7    |    6    |    5    |    4    |    3    |    2    |    1    |    0    |
    // ---------------------------------------------------------------------------------
    // |   IRQ   | Timer 1 | Timer 2 |   CB1   |   CB2   |Shift Reg|   CA1   |   CA2   |

    // Timer 1 - Set by:     Time-out of T1
    //           Cleared by: Read t1 low or write t1 high
    
    inline void setInterruptFlag_T1() { SET_BIT(io[0xD],6); IRQ(); }
    inline void clearInterruptFlag_T1() { CLR_BIT(io[0xD],6); IRQ(); }

    // Timer 2 - Set by:     Time-out of T2
    //           Cleared by: Read t2 low or write t2 high
    
    inline void setInterruptFlag_T2() { SET_BIT(io[0xD],5); IRQ(); }
    inline void clearInterruptFlag_T2() { CLR_BIT(io[0xD],5); IRQ(); }

    // CB1 - Set by:     Active edge on CB1
    //       Cleared by: Read or write to register 0 (ORB)
    
    inline void setInterruptFlag_CB1() { SET_BIT(io[0xD],4); IRQ(); }
    inline void clearInterruptFlag_CB1() { CLR_BIT(io[0xD],4); IRQ(); }

    // CB2 - Set by:     Active edge on CB2
    //       Cleared by: Read or write to register 0 (ORB) (only if CB2 is not selected as "INDEPENDENT")
    
    inline void setInterruptFlag_CB2() { SET_BIT(io[0xD],3); IRQ(); }
    inline void clearInterruptFlag_CB2() { CLR_BIT(io[0xD],3); IRQ(); }
    inline bool CB2selectedAsIndependent() {
        uint8_t b765 = (io[0xC] >> 5) & 0x07; return (b765 == 0x01) || (b765 == 0x03); }

    // Shift register - Set by:     8 shifts completed
    //                  Cleared by: Read or write to register 10 (0xA) (shift register)
    
    inline void setInterruptFlag_SR() { SET_BIT(io[0xD],2); IRQ(); }
    inline void clearInterruptFlag_SR() { CLR_BIT(io[0xD],2); IRQ(); }

    // CA1 - Set by:     Active edge on CA1
    //       Cleared by: Read or write to register 1 (ORA)
    
    inline void setInterruptFlag_CA1() { SET_BIT(io[0x0D],1); IRQ(); }
    inline void clearInterruptFlag_CA1() { CLR_BIT(io[0x0D],1); IRQ(); }

    // CA2 - Set by:     Active edge on CA2
    //       Cleared by: Read or write to register 1 (ORA) (only if CA2 is not selected as "INDEPENDENT")
    
    inline void setInterruptFlag_CA2() { SET_BIT(io[0xD],0); IRQ(); }
    inline void clearInterruptFlag_CA2() { CLR_BIT(io[0xD],0); IRQ(); }
    inline bool CA2selectedAsIndependent() {
        uint8_t b321 = (io[0xC] >> 1) & 0x07; return (b321 == 0x01) || (b321 == 0x03); }
};


/*! @brief   First virtual VIA6522 controller
 *  @details VIA1 serves as hardware interface between the VC1541 CPU and the IEC bus.
 */
class VIA1 : public VIA6522 {
	
public:

	//! Constructor
	VIA1();
	
	//! Destructor
	~VIA1();
    
    //! Execution function for timer 1
    void executeTimer1();
    
    //! Execution function for timer 2
    void executeTimer2();
    
	uint8_t peek(uint16_t addr);
	void poke(uint16_t addr, uint8_t value);
	
    
	inline bool atnInterruptsEnabled() { return io[0x0E] & 0x02; }
	inline void indicateAtnInterrupt() { io[0x0D] |= 0x02; }
	inline void clearAtnIndicator() { io[0x0D] &= (0xFF-0x02); }
};

//! The second versatile interface adapter (VIA2)
class VIA2 : public VIA6522 {
	
public:

	//! Constructor
	VIA2();
	
	//! Destructor
	~VIA2();
    
    //! Execution function for timer 1
    void executeTimer1();
    
    //! Execution function for timer 2
    void executeTimer2();
    
	uint8_t peek(uint16_t addr);
	void poke(uint16_t addr, uint8_t value);

	bool stepperActive0() { return (orb & 0x01) != 0; }
	bool stepperActive1() { return (orb & 0x02) != 0; }
	bool engineRunning() { return (orb & 0x04) != 0; }
	bool redLEDshining() { return (orb & 0x08) != 0; }

    
	bool overflowEnabled() { return (io[0x0C] & 0x02); }

    void debug0xC();
};

#endif
