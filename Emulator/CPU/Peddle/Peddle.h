// -----------------------------------------------------------------------------
// This file is part of Peddle - A MOS Technology 65xx CPU emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Published under the terms of the MIT License
// -----------------------------------------------------------------------------

#pragma once

#include "PeddleDebugger.h"
#include "SubComponent.h"
#include "PeddleExec.h"
#include "PeddleUtils.h"
#include "TimeDelayed.h"

namespace peddle {

class Peddle : public SubComponent {

    friend class Debugger;
    friend class Breakpoints;
    friend class Watchpoints;

    //
    // Configuration
    //

protected:

    // The emulated CPU model
    CPURevision cpuModel = MOS_6510;

    // Instance counter (to distinguish different CPU instances)
    isize id;


    //
    // Sub components
    //

public:

    // CPU debugger
    Debugger debugger = Debugger(*this);


    //
    // Lookup tables
    //

protected:

    /* Mapping from opcodes to microinstructions. This array stores the tags
     * of the second microcycle which is the microcycle following the fetch
     * phase.
     */
    MicroInstruction actionFunc[256];


    //
    // Internal state
    //

public:

    // Elapsed clock cycles since power up
    u64 clock;

protected:

    // State flags
    isize flags;

    // The next microinstruction to be executed
    MicroInstruction next;


    //
    // Registers
    //

public:

    Registers reg;


    //
    // Ports
    //

protected:

    // Ready line (RDY). If pulled low, the CPU freezes on the next read access
    bool rdyLine;

    // Cycle of the most recent rising edge of the RDY line
    u64 rdyLineUp;

    // Cycle of the most recent falling edge of the RDY line
    u64 rdyLineDown;

    /* Interrupt lines
     *
     * Usally both variables equal 0 which means that the two interrupt lines
     * are high. When an external component requests an interrupt, the NMI or
     * the IRQ line is pulled low. In that case, the corresponding variable is
     * set to a positive value which indicates the interrupt source. The
     * variables are used in form of bit fields since both interrupt lines are
     * driven by multiple sources.
     */
    IntSource nmiLine;
    IntSource irqLine;

    /* Edge detector (NMI line)
     * https://wiki.nesdev.com/w/index.php/CPU_interrupts
     * "The NMI input is connected to an edge detector. This edge detector polls
     *  the status of the NMI line during φ2 of each CPU cycle (i.e., during the
     *  second half of each cycle) and raises an internal signal if the input
     *  goes from being high during one cycle to being low during the next. The
     *  internal signal goes high during φ1 of the cycle that follows the one
     *  where the edge is detected, and stays high until the NMI has been
     *  handled."
     */
    TimeDelayed <u8,1> edgeDetector = TimeDelayed <u8,1> (&clock);

    /* Level detector of IRQ line.
     * https://wiki.nesdev.com/w/index.php/CPU_interrupts
     * "The IRQ input is connected to a level detector. If a low level is
     *  detected on the IRQ input during φ2 of a cycle, an internal signal is
     *  raised during φ1 the following cycle, remaining high for that cycle only
     *  (or put another way, remaining high as long as the IRQ input is low
     *  during the preceding cycle's φ2).
     */
    TimeDelayed <u8,1> levelDetector = TimeDelayed <u8,1> (&clock);

    /* Result of the edge detector polling operation.
     * https://wiki.nesdev.com/w/index.php/CPU_interrupts
     * "The output from the edge detector and level detector are polled at
     *  certain points to detect pending interrupts. For most instructions, this
     *  polling happens during the final cycle of the instruction, before the
     *  opcode fetch for the next instruction. If the polling operation detects
     *  that an interrupt has been asserted, the next "instruction" executed
     *  is the interrupt sequence. Many references will claim that interrupts
     *  are polled during the last cycle of an instruction, but this is true
     *  only when talking about the output from the edge and level detectors."
     */
    bool doNmi;

    /* Result of the level detector polling operation.
     * https://wiki.nesdev.com/w/index.php/CPU_interrupts
     * "If both an NMI and an IRQ are pending at the end of an instruction, the
     *  NMI will be handled and the pending status of the IRQ forgotten (though
     *  it's likely to be detected again during later polling)."
     */
    bool doIrq;


    //
    // Constructing
    //

public:

    Peddle(C64 &ref);
    virtual ~Peddle();


    //
    // Initializing (PeddleInit_cpp)
    //

private:

    // Registers the instruction set
    void registerInstructions();
    void registerLegalInstructions();
    void registerIllegalInstructions();

    // Registers a single instruction
    void registerCallback(u8 opcode,
                          const char *mnemonic,
                          AddressingMode mode,
                          MicroInstruction mInstr);


    //
    // Configuring the CPU
    //

public:

    // Selects the emulated CPU model
    void setModel(CPURevision cpuModel);


    //
    // Querying CPU properties and the CPU state
    //

public:

    // Informs whether the selected CPU model has a processor port
    u16 hasProcessorPort() const;
    template <CPURevision C> u16 hasProcessorPort() const;

    // Returns the address bus mask for this CPU model
    u16 addrMask() const;
    template <CPURevision C> u16 addrMask() const;

    // Returns true if the CPU is jammed
    bool isJammed() const { return next == JAM || next == JAM_2; }

    // Returns true if the next cycle marks the beginning of an instruction
    bool inFetchPhase() const { return next == fetch; }


    //
    // Running the CPU (PeddleExec_cpp)
    //

public:

    // Performs a hard reset (power up)
    void reset();
    template <CPURevision C> void reset();

    // Exexutes the CPU for a single cycle
    void execute();
    template <CPURevision C> void execute();

    // Executes the CPU for the specified number of cycles
    void execute(int count);
    template <CPURevision C> void execute(int count);

    // Executes the CPU for a single instruction
    void executeInstruction();
    template <CPURevision C> void executeInstruction();

    // Executes the CPU for the specified number of instructions
    void executeInstruction(int count);
    template <CPURevision C> void executeInstruction(int count);

    // Executes the CPU until the fetch phase is reached
    void finishInstruction();
    template <CPURevision C> void finishInstruction();

protected:

    // Called after the last microcycle has been completed
    template <CPURevision C> void done();


    //
    // Handling interrupts
    //

public:

    // Pulls down an interrupt line
    void pullDownNmiLine(IntSource source);
    void pullDownIrqLine(IntSource source);

    // Releases an interrupt line
    void releaseNmiLine(IntSource source);
    void releaseIrqLine(IntSource source);

    // Checks the status of an interrupt line
    IntSource getNmiLine() const { return nmiLine; }
    IntSource getIrqLine() const { return irqLine; }

    // Sets the value on the RDY line
    void setRDY(bool value);


    //
    // Accessing registers and flags
    //

public:

    /* Returns the frozen program counter.
     * Variable pc0 matches the value of the program counter when the CPU
     * starts to execute an instruction. In contrast to the real program
     * counter, the value isn't changed until the CPU starts to process the
     * next instruction. In other words: This value always contains the start
     * address of the currently executed command, even if some microcycles of
     * the command have already been computed.
     */
    u16 getPC0() const { return reg.pc0; }

    void jumpToAddress(u16 addr) { reg.pc0 = reg.pc = addr; next = fetch; }
    void setPCL(u8 lo) { reg.pc = (u16)((reg.pc & 0xff00) | lo); }
    void setPCH(u8 hi) { reg.pc = (u16)((reg.pc & 0x00ff) | hi << 8); }
    void incPC(u8 offset = 1) { reg.pc += offset; }
    void incPCL(u8 offset = 1) { setPCL(LO_BYTE(reg.pc) + offset); }
    void incPCH(u8 offset = 1) { setPCH(HI_BYTE(reg.pc) + offset); }

    bool getN() const { return reg.sr.n; }
    void setN(bool value) { reg.sr.n = value; }

    bool getV() const { return reg.sr.v; }
    void setV(bool value) { reg.sr.v = value; }

    bool getB() const { return reg.sr.b; }
    void setB(bool value) { reg.sr.b = value; }

    bool getD() const { return reg.sr.d; }
    void setD(bool value) { reg.sr.d = value; }

    bool getI() const { return reg.sr.i; }
    void setI(bool value) { reg.sr.i = value; }

    bool getZ() const { return reg.sr.z; }
    void setZ(bool value) { reg.sr.z = value; }

    bool getC() const { return reg.sr.c; }
    void setC(bool value) { reg.sr.c = value; }

    u8 getP() const;
    void setP(u8 p);

    u8 getPWithClearedB() const;
    void setPWithoutB(u8 p);


    //
    // Interfacing with the memory (high-level interface)
    //

private:

    template <CPURevision C> u8 peek(u16 addr);
    template <CPURevision C> u8 peekZP(u8 addr);
    template <CPURevision C> u8 peekStack(u8 sp);

    template <CPURevision C> void peekIdle(u16 addr);
    template <CPURevision C> void peekZPIdle(u8 addr);
    template <CPURevision C> void peekStackIdle(u8 sp);

    template <CPURevision C> void poke(u16 addr, u8 value);
    template <CPURevision C> void pokeZP(u8 addr, u8 value);
    template <CPURevision C> void pokeStack(u8 sp, u8 value);


    //
    // Interfacing with the memory (low-level interface)
    //

protected:

    // Reads a byte from memory
    virtual u8 read8(u16 addr) const = 0;

    // Special variants used by the reset routine and the disassembler
    virtual u16 read16Reset(u16 addr) const { return u16(read8(addr) | read8(addr + 1) << 8); }
    virtual u8 read8Dasm(u16 addr) const { return read8(addr); }

    // Writes a byte into memory
    virtual void write8(u16 addr, u8 val) const = 0;


    //
    // Accessing the processor port
    //

public:

    // Reads from the port register or the port direction register
    virtual u8 readPort() const;
    virtual u8 readPortDir() const { return reg.pport.direction; }

    // Writes into the port register or the port direction register
    virtual void writePort(u8 val) { reg.pport.data = val; }
    virtual void writePortDir(u8 val) { reg.pport.direction = val; }

protected:
    
    // Data provider for the external port bits
    virtual u8 externalPortBits() const { return 0; }

    
    //
    // Delegation methods
    //

protected:

    // State delegates
    virtual void cpuDidJam() { }

    // Exception delegates
    virtual void irqWillTrigger() { }
    virtual void irqDidTrigger() { }
    virtual void nmiWillTrigger() { }
    virtual void nmiDidTrigger() { }

    // Debugger delegates
    virtual void breakpointReached(u16 addr) { };
    virtual void watchpointReached(u16 addr) { };
    virtual void instructionLogged() { }


    //
    // Operating the Arithmetical Logical Unit (ALU)
    //

private:

    void adc(u8 op);
    void adcBinary(u8 op);
    void adcBcd(u8 op);
    void sbc(u8 op);
    void sbcBinary(u8 op);
    void sbcBcd(u8 op);
    void cmp(u8 op1, u8 op2);
};

}
