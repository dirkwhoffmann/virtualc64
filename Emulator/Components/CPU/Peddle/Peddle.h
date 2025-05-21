// -----------------------------------------------------------------------------
// This file is part of Peddle - A MOS 65xx CPU emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Published under the terms of the MIT License
// -----------------------------------------------------------------------------

#pragma once

#include "PeddleConfig.h"
#include "PeddleTypes.h"
#include "PeddleDisassembler.h"
#include "PeddleDebugger.h"
#include "SubComponent.h"
#include "PeddleUtils.h"
#include "TimeDelayed.h"

// using namespace vc64;

namespace vc64::peddle {

class Peddle : public SubComponent {

    friend class Debugger;
    friend class Disassembler;
    friend class Breakpoints;
    friend class Watchpoints;

    //
    // Static lookup tables
    //

public:

    // Table storing the first microinstruction for each opcode
    static MicroInstruction actionFunc[256];

    // Table storing a textual representation for each opcode
    static const char *mnemonic[256];

    // Table storing the adressing mode for each opcode
    static AddrMode addressingMode[256];


    //
    // Configuration
    //

protected:

    // Instance counter (to distinguish different CPU instances)
    isize id;

    // Emulated CPU model
    CPURevision cpuModel = CPURevision::MOS_6510;


    //
    // Subcomponents
    //

public:

    Debugger debugger = Debugger(*this);
    Disassembler disassembler = Disassembler(*this);


    //
    // Internal state
    //

public:

    // Elapsed clock cycles since power up
    i64 clock;

protected:

    // State flags
    isize flags;

    // The next microinstruction to be executed
    MicroInstruction next;

    // Pending read operation (used in PEDDLE_ASYNC_READS mode)
    Async::ReadTarget pendingRead {};

    
    //
    // Registers
    //

public:

    Registers reg;


    //
    // Ports
    //

protected:

    /* Ready line (RDY)
     * 
     * The variable is usually 0, indicating the RDY line is high. If the
     * variable differs from 0, the RDY line is considered low. In this case,
     * the CPU freezes on the next read (writes are unaffected). The variable
     * is a bit mask since multiple sources can drive the RDY line low. The
     * line is considered low precisely if one or more bits are set.
     */
    u8 rdyLine;

    // Cycle of the most recent rising edge of the RDY line
    i64 rdyLineUp;

    // Cycle of the most recent falling edge of the RDY line
    i64 rdyLineDown;

    /* Interrupt lines (NMI and IRQ)
     *
     * Usually, both variables equal 0, indicating the two interrupt lines are
     * high. When an external component requests an interrupt, the NMI or the
     * IRQ line is pulled low. In that case, the corresponding variable is set
     * to a positive value, indicating the interrupt source. The variables are
     * used as bit masks since both interrupt lines are driven by multiple
     * sources. The corresponding line is considered low precisely if one or
     * more bits are set.
     */
    u8 nmiLine;
    u8 irqLine;

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
    virtual ~Peddle() = default;


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
                          AddrMode mode,
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

    // Returns true if the next cycle marks the beginning of an instruction
    bool inFetchPhase() const { return next == fetch; }


    //
    // Examining instructions
    //

public:
    
    // Returns the length of an instruction in bytes
    isize getLengthOfInstruction(u8 opcode) const;
    isize getLengthOfInstructionAt(u16 addr) const;
    isize getLengthOfCurrentInstruction() const;

    // Returns the address of the instruction following the current one
    u16 getAddressOfNextInstruction() const;


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
    // Handling interrupts and the Ready line
    //

public:

    // Pulls down a line
    void pullDownNmiLine(u8 mask);
    void pullDownIrqLine(u8 mask);
    void pullDownRdyLine(u8 mask);

    // Releases a line
    void releaseNmiLine(u8 mask);
    void releaseIrqLine(u8 mask);
    void releaseRdyLine(u8 mask);

    // Checks the status of a line
    u8 getNmiLine() const { return nmiLine; }
    u8 getIrqLine() const { return irqLine; }
    u8 getRdyLine() const { return rdyLine; }


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

    u16 getSP() const { return reg.sp; }

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

private:
    
    u8 getPWithClearedB() const;
    void setPWithoutB(u8 p);


    //
    // Interfacing with the memory (high-level interface)
    //

private:

    template <CPURevision C> u8 read(u16 addr);
    template <CPURevision C> u8 readZeroPage(u8 addr);
    template <CPURevision C> u8 readStack(u8 sp);

    template <CPURevision C> void readIdle(u16 addr);
    template <CPURevision C> void readZeroPageIdle(u8 addr);
    template <CPURevision C> void readStackIdle(u8 sp);

    template <CPURevision C> void write(u16 addr, u8 value);
    template <CPURevision C> void writeZeroPage(u8 addr, u8 value);
    template <CPURevision C> void writeStack(u8 sp, u8 value);

    template <CPURevision C> u16 readDasm(u16 addr) const;

    
    //
    // Interfacing with the memory (low-level interface)
    //

protected:

    virtual u8 read(u16 addr) { return 0; }
    virtual void write(u16 addr, u8 val) { };
    virtual u8 readDasm(u16 addr) const { return 0; }
    virtual u16 readResetVector();

public:

    // Feeds the result of an asynchronous read operation into the CPU
    void concludeRead(u8 value);


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
    virtual void breakpointReached(u16 addr) const { }
    virtual void watchpointReached(u16 addr) const { }
    virtual void instructionLogged() const { }
    virtual void jumpedTo(u16 addr) const { }


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
