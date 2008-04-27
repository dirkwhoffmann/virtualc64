/*
 * (C) 2006 Dirk W. Hoffmann. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
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

class VC1541;

class VIA6522 : public VirtualComponent {

protected:
	//! VIA I/O Memory
	/*! Whenever a value is poked to the VIA address space, it is stored here. */
	uint8_t io[16];
	
public:
	//! Reference to the connected disk drive. 
	VC1541 *floppy;

	//! Peripheral ports
	/*! The  R6522  VIA  has  two  8-bit  bidirectional  I/O ports (Port A and Port B)
		and each port has two associated control lines. 
		
		Each  8-bit  peripheral  port  has  a Data Direction Register (DDRA, DDRB) for
		specifying  whether  the  peripheral pins are to act as inputs or outputs. A 0
		in  a  bit  of the Data Direction Register causes the corresponding peripheral
		pin to act as an input. A 1 causes the pin to act as an output.

		Each  peripheral  pin  is  also controlled  by  a  bit in the Output Register
		(ORA,  ORB)  and  the Input Register (IRA, IRB). When the pin is programmed as
		an  output,  the  voltage on the pin is controlled by the corresponding bit of
		the  Output  Register.  A  1  in  the  Output Register causes the output to go
		high,  and  a  0  causes the output to go low. Data may be written into Output
		Register  bits  corresponding  to pins which are programmed as inputs. In this
		case, however, the output signal is unaffected.
	*/
	uint8_t ddra, ddrb;
	uint8_t ora, orb;
	uint8_t ira, irb;
	
private:
	
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
	uint8_t t1_latch_lo, t1_latch_hi, t1_counter_lo, t1_counter_hi;

	//! VIA timer 2
	/*! Timer  2  operates  as  an interval timer (in the "one-shot" mode only), or as
		a  counter  for  counting  negative pulses on the PB6 peripheral pin. A single
		control  bit  in  the  Auxiliary  Control  Register  selects between these two
		modes.  This  timer  is comprised of a "write-only" low-order latch (T2L-L), a
		"read-only"  low-order  counter  (T2C-L)  and  a read/write high order counter
		(T2C-H).  The  counter  registers  act as a 16-bit counter which decrements at
		02 rate.
	*/
	uint8_t t2_latch_lo, t2_counter_lo, t2_counter_hi;
	
public:	
	//! Constructor
	VIA6522();
	
	//! Destructor
	~VIA6522();
	
	//! Bind components
	void setDrive(VC1541 *d) { assert(floppy == NULL); floppy = d; }
	
	//! Bring the VIA back to its initial state
	void reset();

	//! Pass control to the virtual VIA
	/*! The VIA chip will be executed for the specified number of clock cycles. */
	bool execute(int cycles); 

	//! Load snapshot from file
	bool load(FILE *file);

	//! Save snapshot to file
	bool save(FILE *file);
	
	//! Special peek function for the I/O memory range
	/*! The peek function only handles those registers that are treated similarily by both VIA chips */
	virtual uint8_t peek(uint16_t addr);
	
	//! Special poke function for the I/O memory range
	/*! The poke function only handles those registers that are treated similarily by both VIA chips */
	virtual void poke(uint16_t addr, uint8_t value);

	//! Get 16 bit timer values
	uint16_t getTimer1() { return ((uint16_t)t1_counter_hi << 8) | t1_counter_lo; }
	uint16_t getTimer2() { return ((uint16_t)t2_counter_hi << 8) | t2_counter_lo; }

	//! Set 16 bit timer values
	void setTimer1(uint16_t value) { t1_counter_hi = value >> 8; t1_counter_lo = value & 0xFF; }
	void setTimer2(uint16_t value) { t2_counter_hi = value >> 8; t2_counter_lo = value & 0xFF; }			

	//! Reload timer from latched values
	void reloadTimer1() { t1_counter_hi = t1_latch_hi; t1_counter_lo = t1_latch_lo; }

	//! Signal time out 
	void signalTimeOut1() { io[0x0D] |= 0x40; }
	void signalTimeOut2() { io[0x0D] |= 0x20; }

	inline void clearTimer1Indicator() { io[0x0D] &= (0xFF-0x40); }
	inline void clearTimer2Indicator() { io[0x0D] &= (0xFF-0x20); }

	//! Check if a time out will interrupt the CPU
	bool timerInterruptEnabled1() { return (io[0x0E] & 0x40) != 0; }
	bool timerInterruptEnabled2() { return (io[0x0E] & 0x20) != 0; }	

	//! Check if input latching is enabled
	bool inputLatchingEnabledA() { return (io[0x0B] & 0x01) != 0; }
	bool inputLatchingEnabledB() { return (io[0x0B] & 0x02) != 0; }	
	void dumpState();
};


//! The first versatile interface adapter (VIA1)
class VIA1 : public VIA6522 {
	
public:

	//! Constructor
	VIA1();
	
	//! Destructor
	~VIA1();

	//! Bring the VIA back to its initial state
	void reset();

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

	//! Bring the VIA back to its initial state
	void reset();

	uint8_t peek(uint16_t addr);
	void poke(uint16_t addr, uint8_t value);

	bool stepperActive0() { return (orb & 0x01) != 0; }
	bool stepperActive1() { return (orb & 0x02) != 0; }
	bool engineRunning() { return (orb & 0x04) != 0; }
	bool redLEDshining() { return (orb & 0x08) != 0; }

	//! Set sync bit
	void setSyncSignal(bool b) { if (b) orb |= 0x80; else orb &= 0x7F; }
	
	bool overflowEnabled() { return (io[0x0C] & 0x02); }
	bool isReadMode() { return (io[0x0C] & 0x20); }
};

#endif
