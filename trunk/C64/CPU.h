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

// Last review: 25.7.06

#ifndef _CPU_INC
#define _CPU_INC

#include "Memory.h"

class C64;
class IEC;

//! The virtual 6510 processor
class CPU : public VirtualComponent {
		
public:	
	//! Addressing modes of the 6510 processor
	enum AddressingMode { 
		ADDR_IMPLIED,
		ADDR_ACCUMULATOR,
		ADDR_IMMEDIATE,
		ADDR_ZERO_PAGE,
		ADDR_ZERO_PAGE_X,
		ADDR_ZERO_PAGE_Y,
		ADDR_ABSOLUTE,
		ADDR_ABSOLUTE_X,
		ADDR_ABSOLUTE_Y,
		ADDR_INDIRECT_X,
		ADDR_INDIRECT_Y,
		ADDR_RELATIVE,
		ADDR_DIRECT,
		ADDR_INDIRECT
	};
	
	//! Error states of the virtual CPU
	/*! CPU_OK indicates normal operation. When a (soft or hard) breakpoint is reached, the CPU enters
	the CPU_BREAKPOINT_REACHED state. CPU_ILLEGAL_INSTRUCTION is entered when an opcode is not understood
	by the CPU. Once the CPU enters a different state than CPU_OK, the execution thread is terminated. 
	*/ 
	enum ErrorState {
		OK,
		SOFT_BREAKPOINT_REACHED,
		HARD_BREAKPOINT_REACHED,
		ILLEGAL_INSTRUCTION
	};

	//! Breakpoint type
	/*! Each memory call is marked with a breakpoint tag. Originally, each cell is tagged with NO_BREAKPOINT
		which has no effect. CPU execution will stop if the memory cell is tagged with one of the following breakpoint types:

	    HARD_BREAKPOINT: execution is halted 
	    SOFT_BREAKPOINT: execution is halted and the tag is deleted
	*/	
	enum Breakpoint {
		NO_BREAKPOINT   = 0x00,
		HARD_BREAKPOINT = 0x01,
		SOFT_BREAKPOINT = 0x02
	};

	//! Clock frequency of the original C64 (NTSC version) in Hz
	static const uint32_t CLOCK_FREQUENCY_NTSC = 1022700;
	
	//! Clock frequency of the original C64 (PAL version) in Hz
	static const uint32_t CLOCK_FREQUENCY_PAL = 985248;
	
	//! Bit position of the Negative flag
	static const uint8_t N_FLAG = 0x80;
	//! Bit position of the Overflow flag
	static const uint8_t V_FLAG = 0x40;
	//! Bit position of the Break flag
	static const uint8_t B_FLAG = 0x10;
	//! Bit position of the Decimal flag
	static const uint8_t D_FLAG = 0x08;
	//! Bit position of the Interrupt flag
	static const uint8_t I_FLAG = 0x04;
	//! Bit position of the Zero flag
	static const uint8_t Z_FLAG = 0x02;
	//! Bit position of the Carry flag
	static const uint8_t C_FLAG = 0x01;
	
public:

	//! Reference to the connected virtual C64
	C64 *c64;

	//! Reference to the connected virtual memory
	Memory *mem;
	
private:

	// Accumulator register
	uint8_t A;
	// X register
	uint8_t X;
	// Y register
	uint8_t Y;	
	//! Program counter
	uint16_t PC;
	//! Memory location of the currently executed command
	uint16_t PC_at_cycle_0;
	// Stack pointer
	uint8_t SP;
	//! Negative flag
	/*! The negative flag is set when the most significant bit (sign bit) equals 1. */
	uint8_t  N;
	//! Overflow flag
	/*! The overflow flag is set iff an arithmetic operation causes a \a signed overflow. */
	uint8_t  V;
	//! Break flag
	/*! Is set to signal external interrupt. */
	uint8_t  B;
	//! Decimal flag
	/*! If set, the CPU operates in BCD mode. (BCD mode is not supported yet). */	
	uint8_t  D;
	//! Interrupt flag
	/*! If set, all interrupts are blocked. (No interrupt request will be answered). */	
	uint8_t  I;
	//! Zero flag
	/*! The zero flag is set iff the result of an arithmetic operation is zero. */	
	uint8_t  Z;
	//! Carry flag
	/*! The carry flag is set iff an arithmetic operation causes an \a unsigned overflow. */	
	uint8_t  C;
	
	// Opcode of the currently executed command
	uint8_t opcode;
	// Internal address register (low byte)
	uint8_t addr_lo;
	// Internal address register (high byte)
	uint8_t addr_hi;
	// Pointer for indirect addressing modes
	uint8_t ptr;
	// Temporary storage for program counter (low byte)
	uint8_t pc_lo;
	// Temporary storage for program counter (high byte)
	uint8_t pc_hi;
	// Address overflow indicater
	/* Used to indicate whether the page boundary has been crossed */
	bool overflow;
	// Internal data register
	uint8_t data;
			
	//! Processor port register
	uint8_t port;	
	//! Processor port data direction register
	uint8_t port_direction;
	//! Experimental
	uint8_t external_port_bits;
	
	//! RDY line (ready line).
	/*! If pulled low (set to 0), the CPU freezes. The signal is used by the VIC chip to freeze the CPU during memory access. */
	bool rdyLine;
	
	//! IRQ line (maskable interrupts)
	/*! The CPU checks the IRQ line before the next instruction is executed.
		If the Interrupt flag is cleared and at least one bit is set, the CPU performs an interrupt. 
		The IRQ line of the real CPU is driven by multiple sources (CIA, VIC). Each source is represented by a separate bit.
		\see CPU::I CPU::I_FLAG
	*/
	uint8_t irqLine;
	
	//! NMI line (non maskable interrupts)
	/*! The CPU checks the IRQ line before the next instruction is executed.
		If at least one bit is set, the CPU performs an interrupt, regardless of the value of the I flag. 
		The IRQ line of the real CPU is driven by multiple sources (CIA, VIC). Each source is represented by a separate bit.
	*/
	uint8_t nmiLine; 
	
	//! Indicates the occurance of a negative edge on the NMI line
	/*! The variable is set to 1, when the value of variable nmiLine is changed from 0 to another value. The variable is
	    used to determine when an NMI interrupt needs to be triggered. */
	bool nmiNegEdge;
	
	//! This variable is set when a negative edge occurs on the irq line and stores the next cycle in which an IRQ can occur.
	/*! The value is needed to determine the exact time to trigger the interrupt */
	uint64_t nextPossibleIrqCycle;
	
	//! This variable is set when a negative edge occurs on the nmi line and stores the next cycle in which an NMI can occur.
	/*! The value is needed to determine the exact time to trigger the interrupt */
	uint64_t nextPossibleNmiCycle;
		
	//! Current error state
	ErrorState errorState;

	//! Next function to be executed
	/*! Each function performs the actions of a single cycle */
	void (CPU::*next)(void);
	 
	//! Callback function array pointing to the execution function of each instruction.
	void (CPU::*actionFunc[256])(void);
	
	//! Breakpoint tag for each memory cell
	/*! \see Breakpoint */
	uint8_t breakpoint[65536];
	
	//! Records all subroutine calls
	/*! Whenever a JSR instruction is executed, the address of the instruction is recorded in the callstack.
	*/
	uint16_t callStack[256];
		
	//! Location of the next free cell of the callstack
	uint8_t callStackPointer;

	//! Value of the I flag before it got changed with the SEI command
	uint8_t oldI;
			
	//! Returns true iff IRQs are blocked
	/*! IRQs are blocked by setting the I flag to 1. The I flag is set with the SEI command and cleared with the CLI command.
		Note that the timing is important here! When an interrupt occures while SEI or CLI is executed, the previous value of I 
	    determines whether an interrupt is triggered or not. To handle timing correctly, the previous value of I is stored in 
		variable oldI whenever SEI or CLI is executed. */
	bool IRQsAreBlocked();
	
public: 
	// Enable auto trace
	int autotracing;
	
	// Auto enable trace when reaching this address
	uint16_t trace_enable_address;
	
	// Trace counter
	int current_trace;
	
	// How many instructions should be traced?
	int max_traces;

	
#include "Instructions.h"
		
public:

	// Constructor
	CPU();
	
	// Destructor
	~CPU();

	// Brings CPU back to its initial state
	void reset();

	//! Load state
	void loadFromBuffer(uint8_t **buffer);
	
	//! Save state
	void saveToBuffer(uint8_t **buffer);	
	
	//! Dump internal state to console
	void dumpState();	

	//! Binds CPU and C64 together
	void setC64(C64 *c) { assert(c64 == NULL); c64 = c; }

	//! Binds CPU and memory together
	void setMemory(Memory *m) { assert(mem == NULL); mem = m; }
		
	//! Get value of processor port
	inline uint8_t getPort() { return port; }
	//! Set value of processor port register
	void setPort(uint8_t value);
	//! Get value of processor port
	inline uint8_t getPortDirection() { return port_direction; }
	//! Experimental
	inline uint8_t getExternalPortBits() { return external_port_bits; }
	//! Set value of processor port data direction register
	void setPortDirection(uint8_t value);
	//! Get physical values of port lines 
	uint8_t getPortLines() { return (port | ~port_direction); }
	
	//! Returns current value of the accumulator register
	inline uint8_t getA() { return A; };
	//! Returns current value of the X register
	inline uint8_t getX() { return X; };
	//! Returns current value of the Y register
	inline uint8_t getY() { return Y; };
	//! Returns current value of the program counter
	inline uint16_t getPC() { return PC; };
	//! Returns "freezed" program counter
	inline uint16_t getPC_at_cycle_0() { return PC_at_cycle_0; };
	//! Returns current value of the program counter	
	inline uint8_t getSP() { return SP; };
	
	//! Returns current value of the memory cell addressed by the program counter
	inline uint8_t peekPC() { return mem->peek(PC); }

	//! Returns 1, if Negative flag is set, 0 otherwise
	inline uint8_t getN() { return (N ? N_FLAG : 0); }
	//! Returns 1, if Overflow flag is set, 0 otherwise
	inline uint8_t getV() { return (V ? V_FLAG : 0); }
	//! Returns 1, if Break flag is set, 0 otherwise
	inline uint8_t getB() { return (B ? B_FLAG : 0); }
	//! Returns 1, if Decimal flag is set, 0 otherwise
	inline uint8_t getD() { return (D ? D_FLAG : 0); }
	//! Returns 1, if Interrupt flag is set, 0 otherwise
	inline uint8_t getI() { return (I ? I_FLAG : 0); }
	//! Returns 1, if Zero flag is set, 0 otherwise
	inline uint8_t getZ() { return (Z ? Z_FLAG : 0); }
	//! Returns 1, if Carry flag is set, 0 otherwise
	inline uint8_t getC() { return (C ? C_FLAG : 0); }
	//! Returns the status register 
	/*! Each bit in the status register corresponds to the value of a single flag, except bit 5 which is always set. */
	inline uint8_t getP() { return getN() | getV() | 32 | getB() | getD() | getI() | getZ() | getC(); }
	//! Returns the status register without the B flag
	/*! The bit position of the B flag is always 0. This function is needed for proper interrupt handling. When an IRQ
		or NMI is triggered internally, the status register is pushed on the stack with the B-flag cleared. */
	inline uint8_t getPWithClearedB() { return getN() | getV() | 32 | getD() | getI() | getZ() | getC(); }
	//! Pack CPU state
	//inline uint64_t packState() { return (((((((((((uint64_t)PC << 8) | SP) << 8) | getP()) << 8) | A) << 8) | X) << 8) | Y); }
	
	//! Write value to the accumulator register. Flags remain untouched.
	inline void setA(uint8_t a) { A = a; }
	//! Write value to the the X register. Flags remain untouched.
	inline void setX(uint8_t x) { X = x; }
	//! Write value to the the Y register. Flags remain untouched.
	inline void setY(uint8_t y) { Y = y; }
	//! Write value to the the program counter.
	inline void setPC(uint16_t pc) { PC = pc; }
	//! Write value to the freezend program counter.
	inline void setPC_at_cycle_0(uint16_t pc) { PC_at_cycle_0 = PC = pc; next = &CPU::fetch;}
	//! Change low byte of the program counter only
	inline void setPCL(uint8_t lo) { PC = (PC & 0xff00) | lo; }
	//! Change high byte of the program counter only
	inline void setPCH(uint8_t hi) { PC = (PC & 0x00ff) | ((uint16_t)hi << 8); }
	//! Increment the program counter by the specified amount. 
	/*! If no argument is provided, the program counter is incremented by one. */
	inline void incPC(uint8_t offset = 1) { PC += offset; }
	//! Increment low byte of program counter (hi byte remains unchanged)
	inline void incPCL(uint8_t offset = 1) { setPCL(LO_BYTE(PC) + offset); }
	//! Increment high byte of program counter (lo byte remains unchanged)
	inline void incPCH(uint8_t offset = 1) { setPCH(HI_BYTE(PC) + offset); }
	
	//! Write value to the stack pointer
	inline void setSP(uint8_t sp) { SP = sp; }
	
	//! 0: Negative-flag is cleared, any other value: flag is set
	inline void setN(uint8_t n) { N = n; }
	//! 0: Overflow-flag is cleared, any other value: flag is set
	inline void setV(uint8_t v) { V = v; }
	//! 0: Break-flag is cleared, any other value: flag is set
	inline void setB(uint8_t b) { B = b; }
	//! 0: Decimal-flag is cleared, any other value: flag is set
	inline void setD(uint8_t d) { D = d; }
	//! 0: Interrupt-flag is cleared, any other value: flag is set
	inline void setI(uint8_t i) { I = i; }
	//! 0: Zero-flag is cleared, any other value: flag is set
	inline void setZ(uint8_t z) { Z = z; }
	//! 0: Carry-flag is cleared, any other value: flag is set
	inline void setC(uint8_t c) { C = c; }
	//! Write value to the status register. The value of bit 5 is ignored. */
	inline void setP(uint8_t p) 
		{ setN(p & N_FLAG); setV(p & V_FLAG); setB(p & B_FLAG); setD(p & D_FLAG); setI(p & I_FLAG); setZ(p & Z_FLAG); setC(p & C_FLAG); }
	inline void setPWithoutB(uint8_t p) 
		{ setN(p & N_FLAG); setV(p & V_FLAG); setD(p & D_FLAG); setI(p & I_FLAG); setZ(p & Z_FLAG); setC(p & C_FLAG); }
			
	//! Load value into the accumulator. The Z- and N-flag may change. */ 
	inline void loadA(uint8_t a) { A = a; N = a & 128; Z = (a == 0); }
	//! Load value into the X register. The Z- and N-flag may change. */ 
	inline void loadX(uint8_t x) { X = x; N = x & 128; Z = (x == 0); }
	//! Load value into the Y register. The Z- and N-flag may change. */ 
	inline void loadY(uint8_t y) { Y = y; N = y & 128; Z = (y == 0); }
	//! Load value into the stack register. The Z- and N-flag may change. */ 
	inline void loadSP(uint8_t s) { SP = s; N = s & 128; Z = (s == 0); }
	//! Load value into memory. The Z- and N-flag may change. */ 
	inline void loadM(uint16_t addr, uint8_t s) { mem->poke(addr, s); N = s & 128; Z = (s == 0); }

	//! Set bit of IRQ line
	void setIRQLine(uint8_t bit);
	
	//! Clear bit of IRQ line
	inline void clearIRQLine(uint8_t bit) { irqLine &= (0xff - bit); }
		
	//! Get bit of IRQ line
	inline uint8_t getIRQLine(uint8_t bit) { return irqLine & bit; }
	
	//! Check if IRQ line has been activated for at least 2 cycles
	bool IRQLineRaisedLongEnough();
	
	//! Set bit of NMI line
	void setNMILine(uint8_t bit);
	
	//! Clear bit of NMI line
	inline void clearNMILine(uint8_t bit) { nmiLine &= (0xff - bit); }
	
	//! Get bit of IRQ line
	inline uint8_t getNMILine(uint8_t bit) { return nmiLine & bit; }

	//! Check if NMI line has been activated for at least 2 cycles
	bool NMILineRaisedLongEnough();
	
	//! Get CIA bit of IRQ line
	inline uint8_t getIRQLineCIA() { return getIRQLine(0x01); }	
	//! Set CIA bit of IRQ line
	inline void setIRQLineCIA() { setIRQLine(0x01); }
	//! Set VIC bit of IRQ line
	inline void setIRQLineVIC() { setIRQLine(0x02); }
	//! Set VIA 1 bit of IRQ line (1541 drive)
	inline void setIRQLineVIA1() { setIRQLine(0x10); }
	//! Set VIA 2 bit of IRQ line (1541 drive)
	inline void setIRQLineVIA2() { setIRQLine(0x20); }
	//! Set ATN bit of IRQ line (1541 drive)
	inline void setIRQLineATN() { setIRQLine(0x40); }
	//! Clear CIA bit of IRQ line
	inline void clearIRQLineCIA() { clearIRQLine(0x01); }	
	//! Clear VIC bit of IRQ line
	inline void clearIRQLineVIC() { clearIRQLine(0x02); }	
	//! Clear VIA 1 bit of IRQ line (1541 drive)
	inline void clearIRQLineVIA1() { clearIRQLine(0x10); }	
	//! Clear VIA 2 bit of IRQ line (1541 drive)
	inline void clearIRQLineVIA2() { clearIRQLine(0x20); }	
	//! Clear ATN bit of IRQ line (1541 drive)
	inline void clearIRQLineATN() { clearIRQLine(0x40); }	
	
	//! Get CIA bit of NMI line
	inline uint8_t getNMILineCIA() { return getNMILine(0x01); }		
	//! Set CIA bit of NMI line
	inline void setNMILineCIA() { setNMILine(0x01); }	
	//! Clear CIA bit of NMI line
	inline void clearNMILineCIA() { clearNMILine(0x01); }
	//! Set Reset bit of NMI line
	inline void setNMILineReset() { setNMILine(0x08); }	
	//! Clear Reset bit of NMI line
	inline void clearNMILineReset() { clearNMILine(0x08); }
	//! Set RDY line 
	inline void setRDY(bool value) { rdyLine = value; }
		
	//! Returns the three letter mnemonic for a given opcode
	const char *getMnemonic(uint8_t opcode);
	//! Returns the three letter mnemonic of the next instruction to execute
	const char *getMnemonic() { return getMnemonic(mem->peek(PC)); }
	//! Returns the adressing mode for a given opcode
	AddressingMode getAddressingMode(uint8_t opcode);
	//! Returns the adressing mode of the next instruction to execute
	AddressingMode getAddressingMode() { return getAddressingMode(mem->peek(PC)); }	
	//! Returns the length in bytes of the instruction with the specified opcode
	/*! Possible values: 1 to 3 */
	int getLengthOfInstruction(uint8_t opcode);
	//! Returns the length in bytes of the next instruction to execute
	/*! Possible values: 1 to 3 */
	inline int getLengthOfCurrentInstruction() { return getLengthOfInstruction(mem->peek(PC)); }
	//! Returns the address of the i-th next instruction, starting from the provided address
	/*! i = 0 will return the current value of the program counter */
	int getAddressOfNextIthInstruction(int i, uint16_t addr);
	//! Returns the address of the instruction following the current instruction
	/*! Possible values: 1 to 3 */
	inline int getAddressOfNextInstruction() { return getAddressOfNextIthInstruction(1, PC_at_cycle_0); }
	//! Disassemble current instruction
	char *disassemble();
	// char *disassemble(uint64_t state);
				
	//! Returns true, iff the next cycle is the first cycle of a command
	inline bool atBeginningOfNewCommand() { return next == &CPU::fetch; }
	
	//! Execute CPU for one cycle
	/*! This is the normal operation mode. Interrupt requests are handled. */
	inline bool executeOneCycle() { (*this.*next)(); return errorState == CPU::OK; }

	//! Returns the current error state
	ErrorState getErrorState();
	//! Sets the current error state
	void setErrorState(ErrorState state);
	//! Reset the error state to "OK"
	void clearErrorState();
	//! Return breakpoint tag for the specified address
	inline uint8_t getBreakpointTag(uint16_t addr) { return breakpoint[addr]; }
	
	//! Returns the breakpoint tag for the specified address
	uint8_t getBreakpoint(uint16_t addr) { return breakpoint[addr]; }

	//! Set a breakpoint tag at the specified address
	void setBreakpoint(uint16_t addr, uint8_t tag) { breakpoint[addr] = tag; }
	
	//! Sets a hard breakpoint at the specified address
	void setHardBreakpoint(uint16_t addr) { 
		debug(1, "Setting hard breakpoint at address %d (%4X)\n", addr, addr);
		breakpoint[addr] |= HARD_BREAKPOINT; 
	}
	
	//! Deletes a hard breakpoint at the specified address
	void deleteHardBreakpoint(uint16_t addr) {
		debug(1, "Deleting hard breakpoint at address %d (%4X)\n", addr, addr);
		breakpoint[addr] &= (255-HARD_BREAKPOINT); 
	}
	
	//! Sets or deletes a hard breakpoint at the specified address 
	void toggleHardBreakpoint(uint16_t addr) { breakpoint[addr] ^= HARD_BREAKPOINT; }
	//! Sets a soft breakpoint at the specified address
	void setSoftBreakpoint(uint16_t addr) { breakpoint[addr] |= SOFT_BREAKPOINT; }
	//! Deletes a soft breakpoint at the specified address
	void deleteSoftBreakpoint(uint16_t addr) { breakpoint[addr] &= (255-SOFT_BREAKPOINT); }
	//! Sets or deletes a hard breakpoint at the specified address 
	void toggleSoftBreakpoint(uint16_t addr) { breakpoint[addr] ^= SOFT_BREAKPOINT; }

	//! Read entry from callstack
	int getTopOfCallStack() { return (callStackPointer > 0) ? callStack[callStackPointer-1] : -1; }
	
	// void dumpHistory();
};
#endif
