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
#include "ProcessorPort.h"
#include "CPUDebugger.h"

#include "CPUTypes.h"
#include "CPUInstructions.h"
#include "TimeDelayed.h"

class Memory;

class CPU : public C64Component {
        
    friend class CPUDebugger;
    
    // Reference to the connected memory
    Memory &mem;
    
    // Result of the latest inspection
    CPUInfo info;
    
    
    //
    // Sub components
    //
    
public:
    
    // Processor Port
    ProcessorPort pport = ProcessorPort(c64);
    
    // CPU debugger
    CPUDebugger debugger = CPUDebugger(c64);
    
    
    //
    // Internal state
    //
    
private:
    
    
    
    //
    // Chip properties
    //
    
    /*! @brief    Selected model (DEPRECATED, USE CPUConfig)
     *  @details  Right now, this atrribute is only used to distinguish the
     *            C64 CPU (MOS6510) from the VC1541 CPU (MOS6502). Hardware
     *            differences between both models are not emulated.
     */
    CPUModel model;
    
    
    //
    // Lookup tables
    //
    
    /* Mapping from opcodes to microinstructions.
     * The array stores pointers to the handlers of the second microcycle which
     * is microcycle cycle following the fetch phase.
     */
    MicroInstruction actionFunc[256];
            

    //
    // Internal state
    //
    
public:
    
    // Elapsed clock cycles since power up
    u64 cycle;
    
    // Indicates whether the CPU is jammed
    bool halted;
            
    // Enables or disables debug options
    bool checkForBreakpoints = false;
    bool checkForWatchpoints = false;
    bool logInstructions = false;
    
private:

    // The next microinstruction to be executed
    MicroInstruction next;
    
    
    //
    // Registers
    //
    
public:
    
    u8 regA;
    u8 regX;
    u8 regY;
    u16 regPC;
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
    
    // Cycle of the most recent rising edge of the RDY line
    u64 rdyLineUp;
    
    // Cycle of the most recent falling edge of the RDY line
    u64 rdyLineDown;
    
public:
    
    /* Interrupt lines.
     * Usally both variables equal 0 which means that the two interrupt lines
     * are high. When an external component requests an interrupt, the NMI or
     * the IRQ line is pulled low. In that case, the corresponding variable is
     * set to a positive value which indicates the interrupt source. The
     * variables are used in form of bit fields since both interrupt lines are
     * driven by multiple sources.
     */
    u8 nmiLine;
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
    // Constructing and serializing
    //
    
public:
    
    CPU(CPUModel model, C64& ref, Memory &memref);
    
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
    
    DisassembledInstruction getInstrInfo(long nr, u16 startAddr);
    DisassembledInstruction getInstrInfo(long nr);
    DisassembledInstruction getLoggedInstrInfo(long nr);
    
    
    //
    // Methods from HardwareComponent
    //
    
private:
    
    void _reset() override;
    void _inspect() override;
    void _inspect(u32 dasmStart);
    void _dump() override;
    void _setDebug(bool enable) override;
    size_t stateSize() override;
    void didLoadFromBuffer(u8 **buffer) override;
    void didSaveToBuffer(u8 **buffer) override;

    
    //
    // Handling registers and flags
    //

public:
    
    
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
    
private:
    
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
    // Operating the ALU (CPUInstructions.cpp)
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
    // Handling interrupts
    //
    
public:
    
    // Pulls down or releases an interrupt line
    void pullDownNmiLine(IntSource source);
    void releaseNmiLine(IntSource source);
    void pullDownIrqLine(IntSource source);
    void releaseIrqLine(IntSource source);
    
    // Sets the RDY line
    void setRDY(bool value);
    
        
    //
    // Executing the device
    //
    
public:
    
    // Executes the next micro instruction
    void executeOneCycle();

    // Returns true if the CPU is jammed
    bool isHalted() { return halted; }
    
    // Returns true if the next cycle marks the beginning of an instruction
    bool inFetchPhase() { return next == fetch; }

private:

    // Processes debug flags
    void processFlags();
};

#endif
