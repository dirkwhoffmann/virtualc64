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

#include "C64.h"

VIA6522::VIA6522()
{
	name = "VIA";
	floppy = NULL;
	reset();
}

VIA6522::~VIA6522()
{
}
	
void VIA6522::reset()
{
	ddra = 0;
	ddrb = 0;
	ora = 0;
	orb = 0;
	ira = 0;
	irb = 0;
	t1_latch_lo = 0;
	t1_latch_hi = 0;
	t1_counter_lo = 0;
	t1_counter_hi = 0;
	t2_latch_lo = 0;
	t2_counter_lo = 0;
	t2_counter_hi = 0;
		
	for (unsigned i = 0; i < sizeof(io); i++)
		io[i] = 0;
}

bool VIA6522::load(uint8_t **buffer)
{
	debug(2, "    Loading VIA6522 state...\n");

	ddra = read8(buffer);
	ddrb = read8(buffer);
	ora = read8(buffer);
	orb = read8(buffer);
	ira = read8(buffer);
	irb = read8(buffer);
	t1_latch_lo = read8(buffer);
	t1_latch_hi = read8(buffer);
	t1_counter_lo = read8(buffer);
	t1_counter_hi = read8(buffer);
	t2_latch_lo = read8(buffer);
	t2_counter_lo = read8(buffer);
	t2_counter_hi = read8(buffer);
	
	for (unsigned i = 0; i < sizeof(io); i++)
		io[i] = read8(buffer);
	
	return true;
}

bool VIA6522::save(uint8_t **buffer)
{
	debug(2, "    Saving VIA6522 state...\n");

	write8(buffer, ddra);
	write8(buffer, ddrb);
	write8(buffer, ora);
	write8(buffer, orb);
	write8(buffer, ira);
	write8(buffer, irb);
	write8(buffer, t1_latch_lo);
	write8(buffer, t1_latch_hi);
	write8(buffer, t1_counter_lo);
	write8(buffer, t1_counter_hi);
	write8(buffer, t2_latch_lo);
	write8(buffer, t2_counter_lo);
	write8(buffer, t2_counter_hi);
	
	for (unsigned i = 0; i < sizeof(io); i++)
		write8(buffer, io[i]);

	return true;
}

bool VIA6522::execute(int cycles)
{
	uint16_t timer1 = getTimer1();
	uint16_t timer2 = getTimer2();
	
	if (timer1 != 0) {
		if (timer1 <= cycles) {
			// Timer 1 time out
			signalTimeOut1();
			if (timerInterruptEnabled1()) {
				floppy->cpu->setIRQLineVIA1();	
			}
			// Reload timer
			setTimer1(0);
		} else {
			setTimer1(timer1 - cycles);
		}
	}
	
	if (timer2 != 0) {
		if (timer2 <= cycles) {
			// Timer 2 time out
			signalTimeOut2();
			if (timerInterruptEnabled2()) {
				floppy->cpu->setIRQLineVIA2();
			}
			// Reload timer
			setTimer2(0);
		} else {
			setTimer2(timer2 - cycles);
		}
	}
	return true;
}

void 
VIA6522::dumpState()
{
	msg("VIA:\n");
	msg("----\n\n");
	msg("          Input register (IRA) : %02X\n", ira);
	msg("          Input register (IRB) : %02X\n", irb);
	msg("         Output register (ORA) : %02X\n", ora);
	msg("         Output register (ORB) : %02X\n", orb);
	msg("Data direction register (DDRA) : %02X\n", ddra);
	msg("Data direction register (DDRB) : %02X\n", ddrb);
	msg("              Input latching A : %s\n", inputLatchingEnabledA() ? "enabled" : "disabled");
	msg("              Input latching B : %s\n", inputLatchingEnabledB() ? "enabled" : "disabled");
	msg("                       Timer 1 : %d (latched: %d)\n", LO_HI(t1_counter_lo, t1_counter_hi), LO_HI(t1_latch_lo, t1_latch_hi));
	msg("                       Timer 2 : %d (latched: %d)\n", LO_HI(t2_counter_lo, t2_counter_hi), LO_HI(t2_latch_lo, 0));
	msg("            Timer 1 interrupts : %s\n", timerInterruptEnabled1() ? "enabled" : "disabled");
	msg("            Timer 2 interrupts : %s\n", timerInterruptEnabled2() ? "enabled" : "disabled");
	msg("        Timer 1 interrupt flag : %d\n", (io[0x0D] & 0x40) != 0);
	msg("        Timer 2 interrupt flag : %d\n", (io[0x0D] & 0x20) != 0);
	msg("                     IO memory : ");
	for (int j = 0; j < 16; j ++) {
		msg("%02X ", io[j]);
	}
	msg("\n");
}

uint8_t 
VIA6522::peek(uint16_t addr)
{
	assert (addr <= 0x0F);
		
	switch(addr) {
		//              REG 0 -- ORB/IRB
		//      +---+---+---+---+---+---+---+---+
		//      | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
		//      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+       -+
		//        |   |   |   |   |   |   |   +----- PB0 |
		//        |   |   |   |   |   |   |              |
		//        |   |   |   |   |   |   +--------- PB1 |
		//        |   |   |   |   |   |                  |
		//        |   |   |   |   |   +------------- PB2 |  OUTPUT REGISTER
		//        |   |   |   |   |                      |  "B" (ORB)
		//        |   |   |   |   +----------------- PB3 |
		//        |   |   |   |                          +-       OR
		//        |   |   |   +--------------------- PB4 |
		//        |   |   |                              |  INPUT REGISTER
		//        |   |   +------------------------- PB5 |  "B" (IRB)
		//        |   |                                  |
		//        |   +----------------------------- PB6 |
		//        |                                      |
		//        +--------------------------------- PB7 |
		//                                              -+
		//
		// Output register B (ORB), Input register B (IRB)
		// +-----------------------+-----------------------+---------------------------+
		// |       PIN             |                       |                           |
		// |  DATA DIRECTION       |        WRITE          |           READ            |
		// |    SELECTION          |                       |                           |
		// +-----------------------+-----------------------+---------------------------+
		// |DDRB = 1  OUTPUT       |MPU WRITES OUTPUT LEVEL|MPU READS OUTPUT REGISTER  |
		// |                       |ORB                    |BIT, ORB PIN LEVEL HAS NO  |
		// |                       |                       |AFFECT                     |
		// +-----------------------+-----------------------+---------------------------+
		// |DDRB = 0  INPUT        |MPU WRITES INTO ORB BUT|MPU READS INPUT LEVEL ON PB|
		// |INPUT LATCHING DISABLED|NO AFFECT ON PIN LEVEL |PIN                        |
		// |                       |UNTIL DDRB CHANGED     |                           |
		// +-----------------------+                       +---------------------------+
		// |DDRB = 0  INPUT        |                       |MPU READS IRB BIT WHICH IS |
		// |INPUT LATCHING ENABLED |                       |THE LEVEL OF THE PB PIN AT |
		// |                       |                       |THE TIME OF THE LAST CB1   |
		// |                       |                       |ACTIVE TRANSITION          |
		// +-----------------------+-----------------------+---------------------------+
		case 0x00:
			debug(1, "PANIC: VIA register 0 needs individual handling!\n");
			assert(0);
			break;

		//               REG 1 -- ORA/IRA
		//      +---+---+---+---+---+---+---+---+
		//      | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
		//      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+       -+
		//        |   |   |   |   |   |   |   +----- PA0 |
		//        |   |   |   |   |   |   |              |
		//        |   |   |   |   |   |   +--------- PA1 |
		//        |   |   |   |   |   |                  |
		//        |   |   |   |   |   +------------- PA2 |  OUTPUT REGISTER
		//        |   |   |   |   |                      |  "A" (ORA)
		//        |   |   |   |   +----------------- PA3 |
		//        |   |   |   |                          +-       OR
		//        |   |   |   +--------------------- PA4 |
		//        |   |   |                              |  INPUT REGISTER
		//        |   |   +------------------------- PA5 |  "A" (IRA)
		//        |   |                                  |
		//        |   +----------------------------- PA6 |
		//        |                                      |
		//        +--------------------------------- PA7 |
		//												-+
		//
		// Output register A (ORA), Input register A (IRA)
		// +-----------------------+-----------------------+---------------------------+
		// |       PIN             |                       |                           |
		// |  DATA DIRECTION       |        WRITE          |           READ            |
		// |    SELECTION          |                       |                           |
		// +-----------------------+-----------------------+---------------------------+
		// |DDRA = 1  OUTPUT       |MPU WRITES OUTPUT LEVEL|MPU READS LEVEL ON PA PIN  |
		// |INPUT LATCHING DISABLED|ORA                    |                           |
		// +-----------------------+                       +---------------------------+
		// |DDRA = 1  INPUT        |                       |MPU READS IRA BIT WHICH IS |
		// |INPUT LATCHING ENABLED |                       |THE LEVEL OF THE PA PIN AT |
		// |                       |                       |THE TIME OF THE LAST CA1   |
		// |                       |                       |ACTIVE TRANSITION          |
		// +-----------------------+-----------------------+---------------------------+
		// |DDRA = 0  INPUT        |MPU WRITES INTO ORA BUT|MPU READS LEVEL ON PA PIN  |
		// |INPUT LATCHING DISABLED|NO AFFECT ON PIN LEVEL |                           |
		// |                       |UNTIL DDRA CHANGED     |                           |
		// +-----------------------+                       +---------------------------+
		// |DDRA = 0  INPUT        |                       |MPU READS IRA BIT WHICH IS |
		// |INPUT LATCHING ENABLED |                       |THE LEVEL OF THE PA PIN AT |
		// |                       |                       |THE TIME OF THE LAST CA1   |
		// |                       |                       |ACTIVE TRANSITION          |
		// +-----------------------+-----------------------+---------------------------+
		case 0x01:
			debug(1, "PANIC: VIA register 1 needs individual handling!\n");
			assert(0);
			break;
						
		//    REG 2 -- DDRB                          REG 3 -- DDRA
		// +-+-+-+-+-+-+-+-+                      +-+-+-+-+-+-+-+-+
		// |7|6|5|4|3|2|1|0|                      |7|6|5|4|3|2|1|0|
		// +-+-+-+-+-+-+-+-+      -+              +-+-+-+-+-+-+-+-+      -+
		// | | | | | | | +--- PB0 |               | | | | | | | +--- PA0 |
		// | | | | | | +----- PB1 |               | | | | | | +----- PA1 |
		// | | | | | +------- PB2 |  DATA         | | | | | +------- PA2 |  DATA
		// | | | | +--------- PB3 |_ DIRECTION    | | | | +--------- PA3 |_ DIRECTION
		// | | | +----------- PB4 |  REGISTER     | | | +----------- PA4 |  REGISTER
		// | | +------------- PB5 |  "B" (DDRB)   | | +------------- PA5 |  "A" (DDRA)
		// | +--------------- PB6 |               | +--------------- PA6 |
		// +----------------- PB7 |               +----------------- PA7 |
		//						  -+                                     -+
		//
		//  "0"  ASSOCIATED PB PIN IS AN INPUT     "0"  ASSOCIATED PA PIN IS AN INPUT
		//       (HIGH IMPEDANCE)                       (HIGH IMPEDANCE)
		//  "1"  ASSOCIATED PB PIN IS AN OUTPUT    "1"  ASSOCIATED PA PIN IS AN OUTPUT
		//       WHOSE LEVEL IS DETERMINED BY           WHOSE LEVEL IS DETERMINED BY
		//       ORB REGISTER BIT                       ORA REGISTER BIT
		case 0x02: 
			return ddrb;
		case 0x03: 
			return ddra;
			
		// REG 4 -- T1 LOW-ORDER COUNTER           REG 5 -- T1 HIGH-ORDER COUNTER
		//  +-+-+-+-+-+-+-+-+                       +-+-+-+-+-+-+-+-+
		//  |7|6|5|4|3|2|1|0|                       |7|6|5|4|3|2|1|0|
		//  +-+-+-+-+-+-+-+-+      -+               +-+-+-+-+-+-+-+-+        -+
		//   | | | | | | | +--- 1   |                | | | | | | | +--- 256   |
		//   | | | | | | +----- 2   |                | | | | | | +----- 512   |
		//   | | | | | +------- 4   |                | | | | | +------- 1024  |
		//   | | | | +--------- 8   |_ COUNT         | | | | +--------- 2048  |_ COUNT
		//   | | | +----------- 16  |  VALUE         | | | +----------- 4096  |  VALUE
		//   | | +------------- 32  |                | | +------------- 8192  |
		//   | +--------------- 64  |                | +--------------- 16384 |
		//   +----------------- 128 |                +----------------- 32768 |
		//                         -+                                        -+
		//
		// WRITE - 8 BITS LOADED INTO T1           WRITE - 8 BITS LOADED INTO T1
		//         LOW-ORDER LATCHES. LATCH                HIGH-ORDER LATCHES. ALSO
		//         CONTENTS ARE TRANSFERRED                AT THIS TIME BOTH HIGH- AND
		//         INTO LOW-ORDER COUNTER AT               LOW-ORDER LATCHES TRANSFERRED
		//         THE TIME THE HIGH-ORDER                 INTO T1 COUNTER. T1 INTERRUPT
		//         COUNTER IS LOADED (REG 5)               FLAG ALSO IS RESET
		//
		// READ  - 8 BITS FROM T1 LOW-ORDER        READ  - 8 BITS FROM T1 HIGH-ORDER
		//         COUNTER TRANSFERRED TO MPU.             COUNTER TRANSFERRED TO MPU
		//         IN ADDITION T1 INTERRUPT FLAG
		//         IS RESET (BIT 6 IN INTERRUPT
		//         FLAG REGISTER)
		case 0x04: 
			clearTimer1Indicator();
			floppy->cpu->clearIRQLineVIA1();
			return t1_counter_lo;
		case 0x05: 
			return t1_counter_hi;

		//  REG 6 -- T1 LOW-ORDER LATCH             REG 7 -- T1 HIGH-ORDER LATCH
		//  +-+-+-+-+-+-+-+-+                       +-+-+-+-+-+-+-+-+
		//  |7|6|5|4|3|2|1|0|                       |7|6|5|4|3|2|1|0|
		//  +-+-+-+-+-+-+-+-+      -+               +-+-+-+-+-+-+-+-+        -+
		//   | | | | | | | +--- 1   |                | | | | | | | +--- 256   |
		//   | | | | | | +----- 2   |                | | | | | | +----- 512   |
		//   | | | | | +------- 4   |                | | | | | +------- 1024  |
		//   | | | | +--------- 8   |_ COUNT         | | | | +--------- 2048  |_ COUNT
		//   | | | +----------- 16  |  VALUE         | | | +----------- 4096  |  VALUE
		//   | | +------------- 32  |                | | +------------- 8192  |
		//   | +--------------- 64  |                | +--------------- 16384 |
		//   +----------------- 128 |                +----------------- 32768 |
		//                         -+                                        -+
		//
		// WRITE - 8 BITS LOADED INTO T1           WRITE - 8 BITS LOADED INTO T1 HIGH-
		//         LOW-ORDER LATCHES. THIS                 ORDER LATCHES. UNLIKE REG 4
		//         OPERATION IS NO DIFFERENT               OPERATION NO LATCH TO
		//         THAN A WRITE INTO REG 4                 COUNTER TRANSFERS TAKE PLACE
		//
		// READ  - 8 BITS FROM T1 LOW ORDER-       READ  - 8 BITS FROM T1 HIGH-ORDER
		//         LATCHES TRANSFERRED TO MPU.             LATCHES TRANSFERRED TO MPU
		//         UNLIKE REG 4 OPERATION,
		//         THIS DOES NOT CAUSE RESET
		//         OF T1 INTERRUPT FLAG
		case 0x06: 
			return t1_latch_lo;
		case 0x07: 
			return t1_latch_hi;

		//  REG 8 - T2 LOW-ORDER LATCH/COUNTER      REG 9 - T2 HIGH-ORDER COUNTER
		//  +-+-+-+-+-+-+-+-+                       +-+-+-+-+-+-+-+-+
		//  |7|6|5|4|3|2|1|0|                       |7|6|5|4|3|2|1|0|
		//  +-+-+-+-+-+-+-+-+      -+               +-+-+-+-+-+-+-+-+        -+
		//   | | | | | | | +--- 1   |                | | | | | | | +--- 256   |
		//   | | | | | | +----- 2   |                | | | | | | +----- 512   |
		//   | | | | | +------- 4   |                | | | | | +------- 1024  |
		//   | | | | +--------- 8   |_ COUNT         | | | | +--------- 2048  |_ COUNT
		//   | | | +----------- 16  |  VALUE         | | | +----------- 4096  |  VALUE
		//   | | +------------- 32  |                | | +------------- 8192  |
		//   | +--------------- 64  |                | +--------------- 16384 |
		//   +----------------- 128 |                +----------------- 32768 |
		//                         -+                                        -+
		//
		//
		// WRITE - 8 BITS LOADED INTO T2           WRITE - 8 BITS LOADED INTO T2
		//         LOW-ORDER LATCH                         HIGH-ORDER COUNTER. ALSO,
		//                                                 LOW-ORDER LATCH TRANSFERRED
		//  READ  - 8 BITS FROM T2 LOW-ORDER                TO LOW-ORDER COUNTER. IN
		//         COUNTER TRANSFERRED TO MPU.             ADDITION T2 INTERRUPT FLAG
		//         T2 INTERRUPT FLAG IS RESET              IS RESET
		//
        //										   READ  - 8 BITS FROM T2 HIGH-ORDER
		//                                                 COUNTER TRANSFERRED TO MPU
		case 0x08:
			clearTimer2Indicator();
			floppy->cpu->clearIRQLineVIA2();
			return t2_counter_lo;
			
		case 0x09:
			return t2_counter_hi;
		
		// REG 10 -- SHIFT REGISTER              REG 11 -- AUXILIARY CONTROL REGISTER
		//  +-+-+-+-+-+-+-+-+                               +-+-+-+-+-+-+-+-+
		//  |7|6|5|4|3|2|1|0|                               |7|6|5|4|3|2|1|0|
		//  +-+-+-+-+-+-+-+-+   -+                          +-+-+-+-+-+-+-+-+
		//   | | | | | | | +---- |                                 |   |
		//   | | | | | | +------ |                                 +-+-+
		//   | | | | | +-------- |  SHIFT                            |
		//   | | | | +---------- |_ REGISTER                         |  SHIFT REGISTER
		//   | | | +------------ |  BITS                             +- MODE CONTROL
		//   | | +-------------- |              +-+-+-+---------------------------------+
		//   | +---------------- |              |4|3|2|OPERATION                        |
		//   +------------------ |              +-+-+-+---------------------------------+
		//                      -+              |0|0|0|DISABLED                         |
		//                                      |0|0|1|SHIFT IN UNDER CONTROL OF T2     |
		//  NOTES                               |0|1|0|SHIFT IN UNDER CONTROL OF 02     |
		//  1  WHEN SHIFTING OUT BIT 7 IS THE   |0|1|1|SHIFT IN UNDER CONT. OF EXT.CLK  |
		//     FIRST BIT OUT AND SIMULTANEOUSLY |1|0|0|SHIFT OUT FREE RUNNING AT T2 RATE|
		//     IS ROTATED BACK INTO BIT 0       |1|0|1|SHIFT OUT UNDER CONTROL OF T2    |
		//  2  WHEN SHIFTING IN BITS INITIALLY  |1|1|0|SHIFT OUT UNDER CONTROL OF 02    |
		//     ENTER BIT 0 AND ARE SHIFTED      |1|1|1|SHIFT OUT UNDER CONT. OF EXT.CLK |
		//     TOWARDS BIT 7                    +-+-+-+---------------------------------+
		case 0x0A:
			// Shift register
			// debug(2, "Drive is peeking the shift register (from %p)\n", floppy->cpu->getPC());
			break;
			
		case 0x0B:
			// Auxiliary control register
			// debug(2, "Drive is peeking the auxiliary control register (from %p)\n", floppy->cpu->getPC());
			break;
		
		// 		                 REG 12 -- PERIPHERAL CONTROL REGISTER
		//                     +---+---+---+---+---+---+---+---+
		//                     | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
		//                     +---+---+---+---+---+---+---+---+
		//                      |         |  |  |         |  |
		//                      +----+----+  |  +----+----+  |
		//                           |       |       |       |
		//          CB2 CONTROL -----+       |       |       +- CA1 INTERRUPT CONTROL
		// +-+-+-+------------------------+  |       |   +--------------------------+
		// |7|6|5| OPERATION              |  |       |   | 0 = NEGATIVE ACTIVE EDGE |
		// +-+-+-+------------------------+  |       |   | 1 = POSITIVE ACTIVE EDGE |
		// |0|0|0| INPUT NEG. ACTIVE EDGE |  |       |   +--------------------------+
		// +-+-+-+------------------------+  |       +---- CA2 INTERRUPT CONTROL
		// |0|0|1| INDEPENDENT INTERRUPT  |  |       +-+-+-+------------------------+
		// | | | | INPUT NEGATIVE EDGE    |  |       |3|2|1| OPERATION              |
		// +-+-+-+------------------------+  |       +-+-+-+------------------------+
		// |0|1|0| INPUT POS. ACTIVE EDGE |  |       |0|0|0| INPUT NEG. ACTIVE EDGE |
		// +-+-+-+------------------------+  |       +-+-+-+------------------------+
		// |0|1|1| INDEPENDENT INTERRUPT  |  |       |0|0|1| INDEPENDENT INTERRUPT  |
		// | | | | INPUT POSITIVE EDGE    |  |       | | | | INPUT NEGATIVE EDGE    |
		// +-+-+-+------------------------+  |       +-+-+-+------------------------+
		// |1|0|0| HANDSHAKE OUTPUT       |  |       |0|1|0| INPUT POS. ACTIVE EDGE |
		// +-+-+-+------------------------+  |       +-+-+-+------------------------+
		// |1|0|1| PULSE OUTPUT           |  |       |0|1|1| INDEPENDENT INTERRUPT  |
		// +-+-+-+------------------------+  |       | | | | INPUT POSITIVE EDGE    |
		// |1|1|0| LOW OUTPUT             |  |       +-+-+-+------------------------+
		// +-+-+-+------------------------+  |       |1|0|0| HANDSHAKE OUTPUT       |
		// |1|1|1| HIGH OUTPUT            |  |       +-+-+-+------------------------+
		// +-+-+-+------------------------+  |       |1|0|1| PULSE OUTPUT           |
		//     CB1 INTERRUPT CONTROL --------+       +-+-+-+------------------------+
		// +--------------------------+              |1|1|0| LOW OUTPUT             |
		// | 0 = NEGATIVE ACTIVE EDGE |              +-+-+-+------------------------+
		// | 1 = POSITIVE ACTIVE EDGE |              |1|1|1| HIGH OUTPUT            |
		// +--------------------------+              +-+-+-+------------------------+
		case 0x0C:
			// Unused
			break;
			
		//      REG 13 -- INTERRUPT FLAG REGISTER
		// +-+-+-+-+-+-+-+-+
		// |7|6|5|4|3|2|1|0|             SET BY                    CLEARED BY
		// +-+-+-+-+-+-+-+-+    +-----------------------+------------------------------+
		//  | | | | | | | +--CA2| CA2 ACTIVE EDGE       | READ OR WRITE REG 1 (ORA)*   |
		//  | | | | | | |       +-----------------------+------------------------------+
		//  | | | | | | +--CA1--| CA1 ACTIVE EDGE       | READ OR WRITE REG 1 (ORA)    |
		//  | | | | | |         +-----------------------+------------------------------+
		//  | | | | | +SHIFT REG| COMPLETE 8 SHIFTS     | READ OR WRITE SHIFT REG      |
		//  | | | | |           +-----------------------+------------------------------+
		//  | | | | +-CB2-------| CB2 ACTIVE EDGE       | READ OR WRITE ORB*           |
		//  | | | |             +-----------------------+------------------------------+
		//  | | | +-CB1---------| CB1 ACTIVE EDGE       | READ OR WRITE ORB            |
		//  | | |               +-----------------------+------------------------------+
		//  | | +-TIMER 2-------| TIME-OUT OF T2        | READ T2 LOW OR WRITE T2 HIGH |
		//  | |                 +-----------------------+------------------------------+
		//  | +-TIMER 1---------| TIME-OUT OF T1        | READ T1 LOW OR WRITE T1 HIGH |
		//  |                   +-----------------------+------------------------------+
		//  +-IRQ---------------| ANY ENABLED INTERRUPT | CLEAR ALL INTERRUPTS         |
		//                      +-----------------------+------------------------------+
		// 
		//   * IF THE CA2/CB2 CONTROL IN THE PCR IS SELECTED AS "INDEPENDENT"
		//	INTERRUPT INPUT, THEN READING OR WRITING THE OUTPUT REGISTER
		//     ORA/ORB WILL NOT CLEAR THE FLAG BIT. INSTEAD, THE BIT MUST BE
		//     CLEARED BY WRITING INTO THE IFR, AS DESCRIBED PREVIOUSLY.
		case 0x0D:
			return io[addr] | (io[addr] & io[0x0E] ? 0x80 : 0);
			
		//                 REG 14 -- INTERRUPT ENABLE REGISTER
		//                           +-+-+-+-+-+-+-+-+
		//                           |7|6|5|4|3|2|1|0|
		//                           +-+-+-+-+-+-+-+-+            -+
		//                            | | | | | | | +--- CA2       |
		//                            | | | | | | +----- CA1       |  0 = INTERRUPT
		//                            | | | | | +------- SHIFT REG |      DISABLED
		//                            | | | | +--------- CB2       |_
		//                            | | | +----------- CB1       |
		//                            | | +------------- TIMER 2   |  1 = INTERRUPT
		//                            | +--------------- TIMER 1   |      ENABLED
		//                            +----------------- SET/CLEAR |
		//                                                       -+
		// 
		//          NOTES:
		//          1  IF BIT 7 IS A "0", THEN EACH "1" IN BITS 0-6 DISABLES THE
		//             CORRESPONDING INTERRUPT.
		//          2  IF BIT 7 IS A "1",  THEN EACH "1" IN BITS 0-6 ENABLES THE
		//             CORRESPONDING INTERRUPT.
		//          3  IF A READ OF THIS REGISTER IS DONE, BIT 7 WILL BE "1" AND
		//             ALL OTHER BITS WILL REFLECT THEIR ENABLE/DISABLE STATE.
		case 0x0E:
			return io[addr] | 0x80;			

		case 0x0F:
			break;
	}

	// default behavior
	return io[addr];
}

uint8_t VIA1::peek(uint16_t addr)
{
	switch(addr) {
		case 0x00:
			uint8_t result, pb_pins;
			// Bit 0: Data in
			// Bit 1: Data out
			// Bit 2: Clock in
			// Bit 3: Clock out
			// Bit 4: ATN out
			// Bit 5,6: Geräteadresse
			// Bit 7: ATN in
			pb_pins = 
				(floppy->iec->getClockLine() ? 0x00 : 0x04) | 
				(floppy->iec->getDataLine() ? 0x00 : 0x01) | 
				(floppy->iec->getAtnLine() ? 0x00 : 0x80);				
			result = (ddrb & orb) | (~ddrb & pb_pins);									
			return result & 0x9F; // Set device address to zero

		case 0x01:
			clearAtnIndicator();
			floppy->cpu->clearIRQLineATN();
			return ora;

		default:
			return VIA6522::peek(addr);	
	}
}

uint8_t h[16];

uint8_t VIA2::peek(uint16_t addr)
{
	switch(addr) {
		case 0x00:
			// Bit 4: 0 = disc is write protected
			if (floppy->isWriteProtected())
				orb &= 0xEF;
			else
				orb |= 0x10;
				
			// Bit 7: 0 = SYNC mark
			if (floppy->readHead() == 0xFF)
				orb &= 0x7F;
			else
				orb |= 0x80;
	
			return orb;

		case 0x01:
			if (tracingEnabled()) {
				debug(1, "%02X ", ora);			
			}
			return ora;
		
		default:
			return VIA6522::peek(addr);	
	}
}


void VIA6522::poke(uint16_t addr, uint8_t value)
{
	assert (addr <= 0x0F);
		
	switch(addr) {
		case 0x00:
			// Not reached
			assert(false);
			break;
		
		case 0x01:
			// Not reached
			assert(false);
			break;
					
		case 0x02: 
			ddrb = value;
			return;
			
		case 0x03: 
			ddra = value;
			return;
		
		case 0x04: 
			t1_latch_lo = value;
			return;
			
		case 0x05: 
			t1_latch_hi = value;
			t1_counter_hi = t1_latch_hi;
			t1_counter_lo = t1_latch_lo;
			clearTimer1Indicator();
			floppy->cpu->clearIRQLineVIA1();
			return;

		case 0x06:
			t1_latch_lo = value;
			return;
			
		case 0x07:
			t1_latch_hi = value;
			return;
		
		case 0x08:
			t2_latch_lo = value;
			return;
		
		case 0x09:
			t2_counter_hi = value;
			t2_counter_lo = t2_latch_lo;
			clearTimer2Indicator();
			floppy->cpu->clearIRQLineVIA2();
			return;

		case 0x0A:
			break;

		case 0x0B:
			break;

		case 0x0C:
			break;

		case 0x0D:
			io[addr] &= ~value;
			return;

		case 0x0E:
			if (value & 0x80) {
				io[addr] |= value & 0x7f;
            } else {
				io[addr] &= ~value;
			}
			return;

		case 0x0F:
			break;
	}

	// default bevahior
	io[addr] = value;
}

void VIA1::poke(uint16_t addr, uint8_t value)
{
	switch(addr) {
		case 0x00:
			orb = value;
			io[0x0D] &= 0xF7;
			io[0x0D] &= 0xEF; 
			floppy->iec->updateDevicePins(orb, ddrb);
			return;

		case 0x01:
			ora = value;
			clearAtnIndicator();
			io[0x0D] &= 0xFE; 
			floppy->cpu->clearIRQLineATN();
			return;
		
		case 0x02: 
			ddrb = value;
			// debug(2, "Writing %d into ddrb via 1\n", value);
			floppy->iec->updateDevicePins(orb, ddrb);
			return; 
						
		default:
			VIA6522::poke(addr, value);	
	}
}

void VIA2::poke(uint16_t addr, uint8_t value)
{
	switch(addr) {
		case 0x00:
			// Port B, Steuerport
			// Bit 0: Schrittmotor Spule 0
			// Bit 1: Schrittmotor Spule 1
			// Bit 2: 1 = Laufwerksmotor an
			// Bit 3: 1 = rote LED an
			// Bit 4: 0 = Diskette schreibgeschützt
			// Bit 5,6: Timersteuerung
			// Bit 7: 0 = SYNC-Signal
			if ((orb & 0x03) != (value & 0x03)) {
				// Bits #0-#1: Head step direction. 
				// Decrease value (%00-%11-%10-%01-%00...) to move head downwards; 
				// Increase value (%00-%01-%10-%11-%00...) to move head upwards.
				if ((value & 0x03) == ((orb+1) & 0x03)) {
					// Move head upwards...
					floppy->moveHead(1);
				} else if ((value & 0x03) == ((orb-1) & 0x03)) {
					// Move head downwards...
					floppy->moveHead(-1);
				} else {
					warn("Unexpected stepper motor control sequence in VC1541 detected\n");
				}
			}

			if ((orb & 0x04) != (value & 0x04)) {
				if (value & 0x04)
					floppy->startRotating();
				else 
					floppy->stopRotating();
			}

			if ((orb & 0x08) != (value & 0x08)) {
				if (value & 0x08)
					floppy->activateRedLED();
				else 
					floppy->deactivateRedLED();
			}

			orb = value;
			return;

		case 0x01: 
			// Port A: Daten vom/zum Tonkopf
			if (tracingEnabled()) {
				debug(1, " W%02X", value);
			}
			ora = value;
			return;

		case 0x03:
			ddra = value;
			if (ddra != 0x00 && ddra != 0xFF) {
				debug(1, "Data direction bits of VC1541 contain suspicious values\n");
			}
			return;
			
		case 0x0C:
			// Auxiliary control register
			if ((value & 0x02) != (io[addr] & 0x02)) {
				// debug("%s V-flag to drive head\n", value & 0x02 ? "Attach" : "Detach");
			}
			if ((value & 0x20) != (io[addr] & 0x20)) {
				debug(2, "Switching to %s mode (%04X) ByteReadyTimer = %d\n", value & 0x20 ? "READ" : "WRITE", floppy->cpu->getPC(),floppy->byteReadyTimer);
			}
			io[addr] = value;
			return;

		case 0x15:
			debug(1, "WARNING: ACCESS TO VIA 2 0x015 detected!\n");
			return;
			
		default:
			VIA6522::poke(addr, value);	
	}
}


VIA1::VIA1()
{
	debug(2, "  Creating VIA1 at address %p...\n", this);
	name = "VIA1";
}
	
VIA1::~VIA1()
{
	debug(2, "  Releasing VIA1...\n");
}

void VIA1::reset()
{
	debug(2, "  Resetting VIA1...\n");
	VIA6522::reset();
}

VIA2::VIA2()
{
	debug(2, "  Creating VIA2 at address %p...\n", this);
	name = "VIA2";
}
	
VIA2::~VIA2()
{
	debug(2, "  Releasing VIA2...\n");
}

void VIA2::reset()
{
	debug(2, "  Resetting VIA2...\n");
	VIA6522::reset();
}