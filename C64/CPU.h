/*!
 * @header      CPU.h
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   Dirk W. Hoffmann
 */
/*              This program is free software; you can redistribute it and/or modify
 *              it under the terms of the GNU General Public License as published by
 *              the Free Software Foundation; either version 2 of the License, or
 *              (at your option) any later version.
 *
 *              This program is distributed in the hope that it will be useful,
 *              but WITHOUT ANY WARRANTY; without even the implied warranty of
 *              MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *              GNU General Public License for more details.
 *
 *              You should have received a copy of the GNU General Public License
 *              along with this program; if not, write to the Free Software
 *              Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _CPU_INC
#define _CPU_INC

#include "Memory.h"

/*! @class  The virtual 6502 / 6510 processor
 */
class CPU : public VirtualComponent {

public:
	
	//! @brief    Bit position of the Negative flag
	static const uint8_t N_FLAG = 0x80;
    
	//! @brief    Bit position of the Overflow flag
	static const uint8_t V_FLAG = 0x40;
    
	//! @brief    Bit position of the Break flag
	static const uint8_t B_FLAG = 0x10;
    
	//! @brief    Bit position of the Decimal flag
	static const uint8_t D_FLAG = 0x08;
    
	//! @brief    Bit position of the Interrupt flag
	static const uint8_t I_FLAG = 0x04;
    
	//! @brief    Bit position of the Zero flag
	static const uint8_t Z_FLAG = 0x02;
    
	//! @brief    Bit position of the Carry flag
	static const uint8_t C_FLAG = 0x01;
	
    //! @brief    Possible interrupt sources
    typedef enum : uint8_t {
        CIA = 0x01,
        VIC = 0x02,
        ATN = 0x04,
        VIA = 0x08,
        EXPANSION = 0x10,
        KEYBOARD = 0x20
    } InterruptSource;

	//! @brief    Reference to the connected virtual memory
	Memory *mem;

    /*! @brief    Selected chip model
     *  @details  Right now, this atrribute is only used to distinguish the C64 CPU (MOS6510)
     *            from the VC1541 CPU (MOS6502). Hardware differences between both models
     *            are not emulated.
     */
    CPUChipModel chipModel;

private:
    
	//! @brief    Accumulator
	uint8_t A;
    
	//! @brief    X register
	uint8_t X;
    
	//! @brief    Y register
	uint8_t Y;
    
	//! @brief    Program counter
	uint16_t PC;
    
	//! @brief    Memory location of the currently executed command
	uint16_t PC_at_cycle_0;
    
	//! @brief    Stack pointer
	uint8_t SP;
    
	//! @brief    Negative flag
	uint8_t  N;
    
	//! @brief    Overflow flag
	uint8_t  V;
    
	//! @brief    Break flag
	uint8_t  B;
    
	//! @brief    Decimal flag
	uint8_t  D;
    
	//! @brief    Interrupt flag
	uint8_t  I;
    
	//! @brief    Zero flag
	uint8_t  Z;
    
	//! @brief    Carry flag
	uint8_t  C;
	
	//! @brief    Opcode of the currently executed command
	uint8_t opcode;
    
	//! @brief    Internal address register (low byte)
	uint8_t addr_lo;
    
	//! @brief    Internal address register (high byte)
	uint8_t addr_hi;
    
	//! @brief    Pointer for indirect addressing modes
	uint8_t ptr;
    
	//! @brief    Temporary storage for program counter (low byte)
	uint8_t pc_lo;
    
	//! @brief    Temporary storage for program counter (high byte)
	uint8_t pc_hi;
    
	/*! @brief    Address overflow indicater
	 *  @details  Used to indicate whether the page boundary has been crossed 
     */
	bool overflow;
    
	//! @brief    Internal data register
	uint8_t data;
			
	/*! @brief    RDY line (ready line)
	 *  @details  If this line is LOW, the CPU freezes on the next read access.
     *            RDY is pulled down by VIC to perform longer lasting read operations.
     */
	bool rdyLine;
	
    /*! @brief    NMI line (non maskable interrupts)
     *  @details  This variable is usually set to 0 which means that the NMI line is in
     *            high state. When an external component requests an NMI nterrupt, this line
     *            is pulled low. In that case, this variable has a positive value and the set
     *            bits indicate the interrupt source.
     */
    uint8_t nmiLine;
    
    /*! @brief    IRQ line (maskable interrupts)
     *  @details  This variable is usually set to 0 which means that the IRQ line is in
     *            high state. When an external component requests an IRQ nterrupt, this line
     *            is pulled low. In that case, this variable has a positive value and the set
     *            bits indicate the interrupt source.
     */
	uint8_t irqLine;

	/*! @brief    Edge detector of NMI line
     *  @details  https://wiki.nesdev.com/w/index.php/CPU_interrupts
     *            "The NMI input is connected to an edge detector. This edge detector polls
     *             the status of the NMI line during φ2 of each CPU cycle (i.e., during the
     *             second half of each cycle) and raises an internal signal if the input goes
     *             from being high during one cycle to being low during the next. The internal
     *             signal goes high during φ1 of the cycle that follows the one where the edge
     *             is detected, and stays high until the NMI has been handled."
     */
    uint8_delayed edgeDetector;
    
    /*! @brief    Level detector of IRQ line
     *  @details  https://wiki.nesdev.com/w/index.php/CPU_interrupts
     *            "The IRQ input is connected to a level detector. If a low level is detected
     *             on the IRQ input during φ2 of a cycle, an internal signal is raised during
     *             φ1 the following cycle, remaining high for that cycle only (or put another
     *             way, remaining high as long as the IRQ input is low during the preceding
     *             cycle's φ2).
     */
    uint8_delayed levelDetector;
    
    //! @brief    Result of the edge detector polling operation
    /*! @details  https://wiki.nesdev.com/w/index.php/CPU_interrupts
     *            "The output from the edge detector and level detector are polled at certain
     *             points to detect pending interrupts. For most instructions, this polling
     *             happens during the final cycle of the instruction, before the opcode fetch
     *             for the next instruction. If the polling operation detects that an interrupt
     *             has been asserted, the next "instruction" executed is the interrupt sequence.
     *             Many references will claim that interrupts are polled during the last cycle
     *             of an instruction, but this is true only when talking about the output from
     *             the edge and level detectors."
     *            Variable is set in macro POLL_INTS (Instructions.h)
     */
    bool doNmi;
    
    //! @brief    Result of the level detector polling operation
    /*! details   https://wiki.nesdev.com/w/index.php/CPU_interrupts
     *  @note     "If both an NMI and an IRQ are pending at the end of an instruction, the
     *             NMI will be handled and the pending status of the IRQ forgotten (though it's
     *             likely to be detected again during later polling)."
     *            Variable is set in macro POLL_INTS (Instructions.h)
     */
    bool doIrq;
    
	//! @brief    Current error state
	ErrorState errorState;
    
	//! @brief    Breakpoint tag for each memory cell
	uint8_t breakpoint[65536];
	
	/*! @brief    Records all subroutine calls
	 *  @details  Whenever a JSR instruction is executed, the address of the instruction is recorded 
     *            in the callstack.
     */
	uint16_t callStack[256];
		
	//! @brief    Location of the next free cell of the callstack.
	uint8_t callStackPointer;

#include "Instructions.h"
		
public:

	//! @brief    Constructor
	CPU();
	
	//! @brief    Destructor
	~CPU();

	//! @brief    Restores the initial state.
	void reset();
	
	//! @brief    Prints debugging information.
	void dumpState();	

    //! @brief    Returns true iff this object is the C64 CPU (for debugging, only).
    bool isC64CPU() { return strcmp(getDescription(), "CPU") == 0; /* VC1541 CPU is calles "1541CPU" */ }

    
    //
    //! @functiongroup Handling registers and flags
    //

	//! @brief    Returns the contents of the accumulator.
    uint8_t getA() { return A; }
    
	//! @brief    Returns current value of the X register.
    uint8_t getX() { return X; }
    
	//! @brief    Returns current value of the Y register.
    uint8_t getY() { return Y; }

    //! @brief    Initialize PC with its start up value.
    void initPC() { PC = LO_HI(mem->peek(0xFFFC), mem->peek(0xFFFD)); }

	//! @brief    Returns current value of the program counter.
    uint16_t getPC() { return PC; }
    
	//! @brief    Returns "freezed" program counter.
    uint16_t getPC_at_cycle_0() { return PC_at_cycle_0; }
    
	//! @brief    Returns current value of the program counter.
    uint8_t getSP() { return SP; }
	
	//! @brief    Returns 1, if Negative flag is set, 0 otherwise.
    uint8_t getN() { return (N ? N_FLAG : 0); }
    
	//! @brief    Returns 1, if Overflow flag is set, 0 otherwise.
    uint8_t getV() { return (V ? V_FLAG : 0); }
    
	//! @brief    Returns 1, if Break flag is set, 0 otherwise.
    uint8_t getB() { return (B ? B_FLAG : 0); }
    
	//! @brief    Returns 1, if Decimal flag is set, 0 otherwise.
    uint8_t getD() { return (D ? D_FLAG : 0); }
    
	//! @brief    Returns 1, if Interrupt flag is set, 0 otherwise.
    uint8_t getI() { return (I ? I_FLAG : 0); }
    
	//! @brief    Returns 1, if Zero flag is set, 0 otherwise.
    uint8_t getZ() { return (Z ? Z_FLAG : 0); }
    
	//! @brief    Returns 1, if Carry flag is set, 0 otherwise.
    uint8_t getC() { return (C ? C_FLAG : 0); }
    
	/*! @brief    Returns the contents of the status register
	 *  @details  Each bit in the status register corresponds to the value of a single flag, 
     *            except bit 5 which is always set. 
     */
    uint8_t getP() { return getN() | getV() | 32 | getB() | getD() | getI() | getZ() | getC(); }
    
	/*! @brief    Returns the status register without the B flag
	 *  @details  The bit position of the B flag is always 0. This function is needed for proper 
     *            interrupt handling. When an IRQ or NMI is triggered internally, the status 
     *            register is pushed on the stack with the B-flag cleared. 
     */
    uint8_t getPWithClearedB() { return getN() | getV() | 32 | getD() | getI() | getZ() | getC(); }
	
    //! @brief    Returns current opcode.
    uint8_t getOpcode() { return opcode; }
    
	//! @brief    Writes value to the accumulator register. Flags remain untouched.
    void setA(uint8_t a) { A = a; }
    
	//! @brief    Writes value to the the X register. Flags remain untouched.
    void setX(uint8_t x) { X = x; }
    
	//! @brief    Writes value to the the Y register. Flags remain untouched.
    void setY(uint8_t y) { Y = y; }
    
	//! @brief    Writes value to the the program counter.
    void setPC(uint16_t pc) { PC = pc; }
    
	//! @brief    Writes value to the freezend program counter.
    void setPC_at_cycle_0(uint16_t pc) { PC_at_cycle_0 = PC = pc; next = fetch;}
    
	//! @brief    Changes low byte of the program counter only.
    void setPCL(uint8_t lo) { PC = (PC & 0xff00) | lo; }
    
	//! @brief    Changes high byte of the program counter only.
    void setPCH(uint8_t hi) { PC = (PC & 0x00ff) | ((uint16_t)hi << 8); }
    
	/*! @brief    Increments the program counter by the specified amount.
	 *  @details  If no argument is provided, the program counter is incremented by one. 
     */
    void incPC(uint8_t offset = 1) { PC += offset; }
    
	//! @brief    Increments low byte of program counter (hi byte remains unchanged).
    void incPCL(uint8_t offset = 1) { setPCL(LO_BYTE(PC) + offset); }
    
	//! @brief    Increments high byte of program counter (lo byte remains unchanged).
    void incPCH(uint8_t offset = 1) { setPCH(HI_BYTE(PC) + offset); }
	
	//! @brief    Writes value to the stack pointer.
    void setSP(uint8_t sp) { SP = sp; }
	
	//! @brief    0: Negative-flag is cleared, any other value: flag is set.
    void setN(uint8_t n) { N = n; }
    
	//! @brief    0: Overflow-flag is cleared, any other value: flag is set.
    void setV(uint8_t v) { V = v; }
    
	//! @brief    0: Break-flag is cleared, any other value: flag is set.
    void setB(uint8_t b) { B = b; }
    
	//! @brief    0: Decimal-flag is cleared, any other value: flag is set.
    void setD(uint8_t d) { D = d; }
    
	//! @brief    0: Interrupt-flag is cleared, any other value: flag is set.
    void setI(uint8_t i) { I = i; }
    
	//! @brief    0: Zero-flag is cleared, any other value: flag is set.
    void setZ(uint8_t z) { Z = z; }
    
	//! @brief    0: Carry-flag is cleared, any other value: flag is set.
    void setC(uint8_t c) { C = c; }
    
	//! @brief    Write value to the status register. The value of bit 5 is ignored.
    void setP(uint8_t p)
		{ setN(p & N_FLAG); setV(p & V_FLAG); setB(p & B_FLAG); setD(p & D_FLAG); setI(p & I_FLAG); setZ(p & Z_FLAG); setC(p & C_FLAG); }
    void setPWithoutB(uint8_t p)
		{ setN(p & N_FLAG); setV(p & V_FLAG); setD(p & D_FLAG); setI(p & I_FLAG); setZ(p & Z_FLAG); setC(p & C_FLAG); }
			
	//! @brief    Loads the accumulator. The Z- and N-flag may change.
    void loadA(uint8_t a) { A = a; N = a & 128; Z = (a == 0); }
    
	//! @brief    Loads the X register. The Z- and N-flag may change.
    void loadX(uint8_t x) { X = x; N = x & 128; Z = (x == 0); }
    
	//! @brief    Loads the Y register. The Z- and N-flag may change.
    void loadY(uint8_t y) { Y = y; N = y & 128; Z = (y == 0); }
    
	//! @brief    Loads the stack register. The Z- and N-flag may change.
    void loadSP(uint8_t s) { SP = s; N = s & 128; Z = (s == 0); }
    
	//! @brief    Loads a value into memory. The Z- and N-flag may change.
    void loadM(uint16_t addr, uint8_t s) { mem->poke(addr, s); N = s & 128; Z = (s == 0); }

    
    //
    //! @functiongroup Handling interrupts
    //
    
    /*! @brief    Pulls down the NMI line
     *  @details  Pulling down the NMI line requests the CPU to interrupt.
     */
    void pullDownNmiLine(InterruptSource source);
    
    /*! @brief    Releases the NMI line
     *  @note     Other sources might still hold the line down.
     */
    void releaseNmiLine(InterruptSource source);
    
    /*! @brief    Pulls down the IRQ line
     *  @details  Pulling down the IRQ line requests the CPU to interrupt.
     */
    void pullDownIrqLine(InterruptSource source);
    
    /*! @brief    Releases the IRQ line
     *  @note     Other sources might still hold the line down.
     */
    void releaseIrqLine(InterruptSource source);
    
	//! @brief    Sets the RDY line.
    void setRDY(bool value) { rdyLine = value; }
		
    
    //
    //! @functiongroup Examining the currently executed instruction
    //
    
	//! @brief    Returns the three letter mnemonic for a given opcode.
	const char *getMnemonic(uint8_t opcode);
    
	//! @brief    Returns the adressing mode for a given opcode.
	AddressingMode getAddressingMode(uint8_t opcode);
    
	/*! @brief    Returns the length in bytes of the instruction with the specified opcode.
	 *  @result   Integer value between 1 and 3.
     */
	int getLengthOfInstruction(uint8_t opcode);
    
	/*! @brief    Returns the length in bytes of the instruction with the specified address.
     *  @result   Integer value between 1 and 3.
     */
    int getLengthOfInstructionAtAddress(uint16_t addr) { return getLengthOfInstruction(mem->peek(addr)); }
    
	/*! @brief    Returns the length in bytes of the next instruction to execute.
     *  @result   Integer value between 1 and 3.
     */
    int getLengthOfCurrentInstruction() { return getLengthOfInstructionAtAddress(PC_at_cycle_0); }
    
	/*! @brief    Returns the address of the instruction following the current instruction.
     *  @result   Integer value between 1 and 3.
     */
    uint16_t getAddressOfNextInstruction() { return PC_at_cycle_0 + getLengthOfCurrentInstruction(); }
    
	//! @brief    Disassembles the current instruction.
	char *disassemble();
				
	//! @brief    Returns true, iff the next cycle is the first cycle of a command.
    bool atBeginningOfNewCommand() { return next == fetch; }
	
    
    //
    //! @functiongroup Executing the device
    //
    
	/*! @brief    Runs the CPU for one cycle.
	 *  @details  This is the normal operation mode. Interrupt requests are handled. 
     */
    bool executeOneCycle() { executeMicroInstruction(); return errorState == CPU_OK; }
    
    //! @brief    Executes the next micro instruction.
    void executeMicroInstruction();
    
	//! @brief    Returns the current error state.
    ErrorState getErrorState() { return errorState; }
    
	//! @brief    Sets the error state.
    void setErrorState(ErrorState state);
    
	//! @brief    Sets the error state back to normal.
    void clearErrorState() { setErrorState(CPU_OK); }
    
    
    //
    //! @functiongroup Handling breakpoints
    //
    
	//! @brief    Returns breakpoint tag for the specified address.
    uint8_t getBreakpointTag(uint16_t addr) { return breakpoint[addr]; }
	
	//! @brief    Returns the breakpoint tag for the specified address.
	uint8_t getBreakpoint(uint16_t addr) { return breakpoint[addr]; }

	//! @brief    Sets a breakpoint tag at the specified address.
	void setBreakpoint(uint16_t addr, uint8_t tag) { breakpoint[addr] = tag; }
	
	//! @brief    Sets a hard breakpoint at the specified address.
    void setHardBreakpoint(uint16_t addr) { breakpoint[addr] |= HARD_BREAKPOINT; }
	
	//! @brief    Deletes a hard breakpoint at the specified address.
	void deleteHardBreakpoint(uint16_t addr) { breakpoint[addr] &= (255-HARD_BREAKPOINT); }
	
	//! @brief    Sets or deletes a hard breakpoint at the specified address.
	void toggleHardBreakpoint(uint16_t addr) { breakpoint[addr] ^= HARD_BREAKPOINT; }
    
	//! @brief    Sets a soft breakpoint at the specified address.
	void setSoftBreakpoint(uint16_t addr) { breakpoint[addr] |= SOFT_BREAKPOINT; }
    
	//! @brief    Deletes a soft breakpoint at the specified address.
	void deleteSoftBreakpoint(uint16_t addr) { breakpoint[addr] &= (255-SOFT_BREAKPOINT); }
    
	//! @brief    Sets or deletes a hard breakpoint at the specified address.
	void toggleSoftBreakpoint(uint16_t addr) { breakpoint[addr] ^= SOFT_BREAKPOINT; }

    
    //
    //! @functiongroup Querying the callstack
    //
    
	//! @brief    Reads entry from callstack.
	int getTopOfCallStack() { return (callStackPointer > 0) ? callStack[callStackPointer-1] : -1; }

};

#endif
