// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _CPU_H
#define _CPU_H

#include "C64Component.h"

#include "CPUTypes.h"
#include "CPUInstructions.h"
#include "TimeDelayed.h"

class Memory;

class CPU : public C64Component {
    
    // Result of the latest inspection
    CPUInfo info;
    
    
    //
    // References to other components
    //
    
private:
    
    //! @brief    Reference to the connected  memory
    Memory *mem;


    //
    // Chip properties
    //

    /*! @brief    Selected model
     *  @details  Right now, this atrribute is only used to distinguish the
     *            C64 CPU (MOS6510) from the VC1541 CPU (MOS6502). Hardware
     *            differences between both models are not emulated.
     */
    CPUModel model;
    
    
    //
    // Lookup tables
    //
    
    //! @brief    Mapping from opcodes to microinstructions
    /*! @details  The mapped microinstruction is the first microinstruction to
     *            be executed after the fetch phase (second microcycle).
     */
    MicroInstruction actionFunc[256];
    
    /*! @brief    Textual representation for each opcode
     *  @note     Used by the disassembler, only.
     */
    const char *mnemonic[256];
    
    /*! @brief    Adressing mode of each opcode
     *  @note     Used by the disassembler, only.
     */
    AddressingMode addressingMode[256];
    
    //! @brief    Breakpoint tag for each memory cell
    u8 breakpoint[65536];

    
    //
    // Internal state
    //
    
    public:
    
    //! @brief    Elapsed C64 clock cycles since power up
    u64 cycle;
    
    //! @brief    Current error state
    ErrorState errorState;
    
    private:

    //! @brief    Next microinstruction to be executed
    /*! @see      executeOneCycle()
     */
    MicroInstruction next;
    
    
    //
    // Registers
    //
    
    public:
    
	//! @brief    Accumulator
	u8 regA;
    
	//! @brief    X register
	u8 regX;
    
	//! @brief    Y register
	u8 regY;
    
    //! @brief    Program counter
    u16 regPC;

    //! @brief    Stack pointer
    u8 regSP;

    private:
    
    /*! @brief     Processor status register (flags)
     *  @details   7 6 5 4 3 2 1 0
     *             N O - B D I Z C
     */
    u8 regP;
    
	//! @brief    Address data (low byte)
	u8 regADL;
    
	//! @brief    Address data (high byte)
	u8 regADH;
    
	//! @brief    Input data latch (indirect addressing modes)
	u8 regIDL;
    
    //! @brief    Data buffer
    u8 regD;
    
	/*! @brief    Address overflow indicater
	 *  @details  Indicates when the page boundary has been crossed.
     */
	bool overflow;
    
    /*! @brief    Memory location of the currently executed command.
     *  @details  This variable is overwritten with the current value of the
     *            program counter (regPC) when the CPU executes the fetch phase
     *            of an instruction. Hence, this value always contains the start
     *            address of the currently executed command, even if some
     *            microcycles of the command have already been computed.
     */
    u16 pc;
    
  
    //
    // Port lines
    //
    
    public:
    
	/*! @brief    Ready line (RDY)
	 *  @details  If this line is low, the CPU freezes on the next read access.
     *            RDY is pulled down by VICII to perform longer lasting read
     *            operations.
     */
	bool rdyLine;
    
    private:
    
    //! @brief    Cycle of the most recent rising edge of the rdyLine
    u64 rdyLineUp;
    
    //! @brief    Cycle of the most recent falling edge of the rdyLine
    u64 rdyLineDown;
    
    public:
    
    /*! @brief    NMI line (non maskable interrupts)
     *  @details  This variable is usually set to 0 which means that the NMI
     *            line is in high state. When an external component requests an
     *            NMI nterrupt, this line is pulled low. In that case, this
     *            variable has a positive value and the set bits indicate the
     *            interrupt source.
     */
    u8 nmiLine;
    
    
    /*! @brief    IRQ line (maskable interrupts)
     *  @details  This variable is usually set to 0 which means that the IRQ
     *            line is in high state. When an external component requests an
     *            IRQ nterrupt, this line is pulled low. In that case, this
     *            variable has a positive value and the set bits indicate the
     *            interrupt source.
     */
	u8 irqLine;

    private:
    
	/*! @brief    Edge detector of NMI line
     *  @details  https://wiki.nesdev.com/w/index.php/CPU_interrupts
     *            "The NMI input is connected to an edge detector. This edge
     *             detector polls the status of the NMI line during φ2 of each
     *             CPU cycle (i.e., during the second half of each cycle) and
     *             raises an internal signal if the input goes from being high
     *             during one cycle to being low during the next. The internal
     *             signal goes high during φ1 of the cycle that follows the one
     *             where the edge is detected, and stays high until the NMI has
     *             been handled."
     */
    TimeDelayed<u8> edgeDetector = TimeDelayed<u8>(1, &cycle);
    
    /*! @brief    Level detector of IRQ line
     *  @details  https://wiki.nesdev.com/w/index.php/CPU_interrupts
     *            "The IRQ input is connected to a level detector. If a low
     *             level is detected on the IRQ input during φ2 of a cycle, an
     *             internal signal is raised during φ1 the following cycle,
     *             remaining high for that cycle only (or put another way,
     *             remaining high as long as the IRQ input is low during the
     *             preceding cycle's φ2).
     */
    TimeDelayed<u8> levelDetector = TimeDelayed<u8>(1, &cycle);
    
    //! @brief    Result of the edge detector polling operation
    /*! @details  https://wiki.nesdev.com/w/index.php/CPU_interrupts
     *            "The output from the edge detector and level detector are
     *             polled at certain points to detect pending interrupts. For
     *             most instructions, this polling happens during the final
     *             cycle of the instruction, before the opcode fetch for the
     *             next instruction. If the polling operation detects that an
     *             interrupt has been asserted, the next "instruction" executed
     *             is the interrupt sequence. Many references will claim that
     *             interrupts are polled during the last cycle of an
     *             instruction, but this is true only when talking about the
     *             output from the edge and level detectors."
     *            Variable is set in macro POLL_INTS (CPUInstructions.h)
     */
    bool doNmi;
    
    //! @brief    Result of the level detector polling operation
    /*! details   https://wiki.nesdev.com/w/index.php/CPU_interrupts
     *  @note     "If both an NMI and an IRQ are pending at the end of an
     *             instruction, the NMI will be handled and the pending status
     *             of the IRQ forgotten (though it's likely to be detected again
     *             during later polling)."
     *            Variable is set in macro POLL_INTS (CPUInstructions.h)
     */
    bool doIrq;
    
    
    //
    // Trace buffer
    //
    
    //! @brief  Trace buffer size
    static const unsigned traceBufferSize = 1024;
    
    //! @brief  Ring buffer for storing the CPU state
    RecordedInstruction traceBuffer[traceBufferSize];
    
    //! @brief  Trace buffer read pointer
    unsigned readPtr;
    
    //! @brief  Trace buffer write pointer
    unsigned writePtr;
    
    
    //
    // Constructing and serializing
    //
    
public:
    
    CPU(CPUModel model, Memory *mem, C64& ref);
    
private:
    
    // Registers a single opcode
    void registerCallback(u8 opcode, const char *mnemonic,
                          AddressingMode mode, MicroInstruction mInstr);
    
    // Registers all instructions
    void registerInstructions();
    
    // Registers all legal instructions
    void registerLegalInstructions();
    
    // Registers all illegal instructionss
    void registerIllegalInstructions();
    
    
    //
    // Configuring
    //
    
    // Returns true if this is the C64's CPU
    bool isC64CPU() { return model == MOS_6510; }

    
    //
    // Analyzing
    //

public:
    
    // Returns the result of the latest inspection
    CPUInfo getInfo() { return HardwareComponent::getInfo(info); }

    
    //
    // Methods from HardwareComponent
    //
    
public:

	void _reset() override;
    void _inspect() override;
    size_t stateSize() override;
    void didLoadFromBuffer(u8 **buffer) override;
    void didSaveToBuffer(u8 **buffer) override;
    
private:

    void _dump() override;

    
    //
    //! @functiongroup Gathering debug information
    //
    
public:
        
    //
    // Handling registers and flags
    //

	/*! @brief    Returns the frozen program counter.
     *  @note     This function returns variable pc that always points to the
     *            beginning of the currently executed command. If execution is
     *            not in microcycle 0 (fetch phase) and the currently executed
     *            command spans over multiple bytes in memory, the program
     *            counter (regPC) may already have been incremented.
     */
    u16 getPC() { return pc; }
    
    //! @brief    Redirects the CPU to a new instruction in memory.
    void jumpToAddress(u16 addr) { pc = regPC = addr; next = fetch; }

	//! @brief    Returns N_FLAG, if Negative flag is set, 0 otherwise.
    u8 getN() { return regP & N_FLAG; }
    
    //! @brief    0: Negative-flag is cleared, any other value: flag is set.
    void setN(u8 bit) { bit ? regP |= N_FLAG : regP &= ~N_FLAG; }
    
	//! @brief    Returns V_FLAG, if Overflow flag is set, 0 otherwise.
    u8 getV() { return regP & V_FLAG; }
    
    //! @brief    0: Overflow-flag is cleared, any other value: flag is set.
    void setV(u8 bit) { bit ? regP |= V_FLAG : regP &= ~V_FLAG; }

	//! @brief    Returns B_FLAG, if Break flag is set, 0 otherwise.
    u8 getB() { return regP & B_FLAG; }
    
    //! @brief    0: Break-flag is cleared, any other value: flag is set.
    void setB(u8 bit) { bit ? regP |= B_FLAG : regP &= ~B_FLAG; }
    
	//! @brief    Returns D_FLAG, if Decimal flag is set, 0 otherwise.
    u8 getD() { return regP & D_FLAG; }
    
    //! @brief    0: Decimal-flag is cleared, any other value: flag is set.
    void setD(u8 bit) { bit ? regP |= D_FLAG : regP &= ~D_FLAG; }

	//! @brief    Returns I_FLAG, if Interrupt flag is set, 0 otherwise.
    u8 getI() { return regP & I_FLAG; }
    
    //! @brief    0: Interrupt-flag is cleared, any other value: flag is set.
    void setI(u8 bit) { bit ? regP |= I_FLAG : regP &= ~I_FLAG; }
    
	//! @brief    Returns Z_FLAG, if Zero flag is set, 0 otherwise.
    u8 getZ() { return regP & Z_FLAG; }
    
    //! @brief    0: Zero-flag is cleared, any other value: flag is set.
    void setZ(u8 bit) { bit ? regP |= Z_FLAG : regP &= ~Z_FLAG; }
    
	//! @brief    Returns C_FLAG, if Carry flag is set, 0 otherwise.
    u8 getC() { return regP & C_FLAG; }
    
    //! @brief    0: Carry-flag is cleared, any other value: flag is set.
    void setC(u8 bit) { bit ? regP |= C_FLAG : regP &= ~C_FLAG; }

    private:
    
	/*! @brief    Returns the contents of the status register
	 *  @details  Each bit in the status register corresponds to the value of
     *            a single flag, except bit 5 which is always set.
     */
    u8 getP() { return regP | 0b00100000; }

	/*! @brief    Returns the status register without the B flag
	 *  @details  The bit position of the B flag is always 0. This function is
     *            needed for proper interrupt handling. When an IRQ or NMI is
     *            triggered internally, the status register is pushed on the
     *            stack with the B-flag cleared.
     */
    u8 getPWithClearedB() { return getP() & 0b11101111; }
    
    //! @brief    Writes a value to the status register.
    void setP(u8 p) { regP = p; }
    
    //! @brief    Writes a value to the status register without overwriting B.
    void setPWithoutB(u8 p) { regP = (p & 0b11101111) | (regP & 0b00010000); }
    
	//! @brief    Changes low byte of the program counter only.
    void setPCL(u8 lo) { regPC = (regPC & 0xff00) | lo; }
    
	//! @brief    Changes high byte of the program counter only.
    void setPCH(u8 hi) { regPC = (regPC & 0x00ff) | ((u16)hi << 8); }
    
	//! @brief    Increments the program counter by the specified amount.
    void incPC(u8 offset = 1) { regPC += offset; }
    
	/*! @brief    Increments the program counter's low byte.
     *  @note     The high byte does not change.
     */
    void incPCL(u8 offset = 1) { setPCL(LO_BYTE(regPC) + offset); }
    
	/*! @brief    Increments the program counter's high byte.
     *  @note     The low byte does not change.
     */
    void incPCH(u8 offset = 1) { setPCH(HI_BYTE(regPC) + offset); }
	
	//! @brief    Loads the accumulator. The Z- and N-flag may change.
    void loadA(u8 a) { regA = a; setN(a & 0x80); setZ(a == 0); }

	//! @brief    Loads the X register. The Z- and N-flag may change.
    void loadX(u8 x) { regX = x; setN(x & 0x80); setZ(x == 0); }
    
	//! @brief    Loads the Y register. The Z- and N-flag may change.
    void loadY(u8 y) { regY = y; setN(y & 0x80); setZ(y == 0); }
    
    
    //
    //! @functiongroup Performing ALU operations (CPUInstructions.cpp)
    //
    
    void adc(u8 op);
    void adc_binary(u8 op);
    void adc_bcd(u8 op);
    void sbc(u8 op);
    void sbc_binary(u8 op);
    void sbc_bcd(u8 op);
    void branch(i8 offset);
    void cmp(u8 op1, u8 op2);
    u8 ror(u8 op);
    u8 rol(u8 op);
    
    
    //
    //! @functiongroup Handling interrupts
    //
    
    public:
    
    /*! @brief    Pulls down the NMI line.
     *  @details  Pulling down the NMI line requests the CPU to interrupt.
     */
    void pullDownNmiLine(IntSource source);
    
    /*! @brief    Releases the NMI line.
     *  @note     Other sources might still hold the line down.
     */
    void releaseNmiLine(IntSource source);
    
    /*! @brief    Pulls down the IRQ line.
     *  @details  Pulling down the IRQ line requests the CPU to interrupt.
     */
    void pullDownIrqLine(IntSource source);
    
    /*! @brief    Releases the IRQ line.
     *  @note     Other sources might still hold the line down.
     */
    void releaseIrqLine(IntSource source);
    
	//! @brief    Sets the RDY line.
    void setRDY(bool value);
		
    
    //
    //! @functiongroup Examining the currently executed instruction
    //
        
	/*! @brief    Returns the length of an instruction in bytes.
	 *  @result   Integer value between 1 and 3.
     */
	unsigned getLengthOfInstruction(u8 opcode);
    
	/*! @brief    Returns the length of instruction in bytes.
     *  @result   Integer value between 1 and 3.
     */
    unsigned getLengthOfInstructionAtAddress(u16 addr) {
        return getLengthOfInstruction(mem->spypeek(addr)); }
    
	/*! @brief    Returns the length of the currently executed instruction.
     *  @result   Integer value between 1 and 3.
     */
    unsigned getLengthOfCurrentInstruction() {
        return getLengthOfInstructionAtAddress(pc); }
    
	/*! @brief    Returns the address of the next instruction to execute.
     *  @result   Integer value between 1 and 3.
     */
    u16 getAddressOfNextInstruction() {
        return pc + getLengthOfCurrentInstruction(); }
    
	/*! @brief    Returns true if the next microcycle is the fetch cycle.
     *  @details  The fetch cycle is the first microinstruction of each command.
     */
    bool inFetchPhase() { return next == fetch; }
	
    
    //
    //! @functiongroup Executing the device
    //
    
	/*! @brief    Executes the next micro instruction.
	 *  @return   true, if the micro instruction was processed successfully.
     *            false, if the CPU was halted, e.g., by reaching a breakpoint.
     */
    bool executeOneCycle();
    
	//! @brief    Returns the current error state.
    ErrorState getErrorState() { return errorState; }
    
	//! @brief    Sets the error state.
    void setErrorState(ErrorState state);
    
	//! @brief    Sets the error state back to normal.
    void clearErrorState() { setErrorState(CPU_OK); }
    
    // Returns true if the CPU is halted
     bool isHalted() { return errorState == CPU_ILLEGAL_INSTRUCTION; }
    
    //
    //! @functiongroup Handling breakpoints
    //
    
    //! @brief    Checks if a hard breakpoint is set at the provided address.
    bool hardBreakpoint(u16 addr) { return (breakpoint[addr] & HARD_BREAKPOINT) != 0; }
    
	//! @brief    Sets a hard breakpoint at the provided address.
    void setHardBreakpoint(u16 addr) { breakpoint[addr] |= HARD_BREAKPOINT; }
	
	//! @brief    Deletes a hard breakpoint at the provided address.
	void deleteHardBreakpoint(u16 addr) { breakpoint[addr] &= ~HARD_BREAKPOINT; }
	
	//! @brief    Sets or deletes a hard breakpoint at the provided address.
	void toggleHardBreakpoint(u16 addr) { breakpoint[addr] ^= HARD_BREAKPOINT; }
    
    //! @brief    Checks if a soft breakpoint is set at the provided address.
    bool softBreakpoint(u16 addr) { return (breakpoint[addr] & SOFT_BREAKPOINT) != 0; }

	//! @brief    Sets a soft breakpoint at the provided address.
	void setSoftBreakpoint(u16 addr) { breakpoint[addr] |= SOFT_BREAKPOINT; }
    
	//! @brief    Deletes a soft breakpoint at the specified address.
	void deleteSoftBreakpoint(u16 addr) { breakpoint[addr] &= ~SOFT_BREAKPOINT; }
    
	//! @brief    Sets or deletes a hard breakpoint at the specified address.
	void toggleSoftBreakpoint(u16 addr) { breakpoint[addr] ^= SOFT_BREAKPOINT; }
    
    
    //
    //! @functiongroup Tracing the program execution
    //
    
    //! @brief  Clears the trace buffer.
    void clearTraceBuffer() { readPtr = writePtr = 0; }
    
    //! @brief   Returns the number of recorded instructions.
    unsigned recordedInstructions(); 
    
    //! @brief   Records an instruction.
    void recordInstruction();
    
    /*! @brief   Reads and removes a recorded instruction from the trace buffer.
     *  @note    The trace buffer must not be empty.
     */
    RecordedInstruction readRecordedInstruction();

    /*! @brief   Reads a recorded instruction from the trace buffer.
     *  @note    'previous' must be smaller than the number of recorded
     *           instructions.
     */
    RecordedInstruction readRecordedInstruction(unsigned previous);

    
    //
    //! @functiongroup Disassembling instructions
    //
    
    //! @brief    Disassembles a previously recorded instruction
    DisassembledInstruction disassemble(RecordedInstruction instr, bool hex);

    //! @brief    Disassembles an instruction at the specified memory location
    DisassembledInstruction disassemble(u16 addr, bool hex);
    
    //! @brief    Disassembles the current instruction.
    DisassembledInstruction disassemble(bool hex) { return disassemble(pc, hex); }

};

#endif
