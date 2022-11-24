// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "CPUTypes.h"
#include "Peddle.h"
#include "SubComponent.h"
#include "CPUDebugger.h"
#include "CPUInstructions.h"
#include "ProcessorPort.h"
#include "TimeDelayed.h"

class Memory;

class CPU : public Peddle {
        
    friend class CPUDebugger;
    friend class Breakpoints;
    friend class Watchpoints;
            
    // Result of the latest inspection
    mutable CPUInfo info = { };


    //
    // Sub components
    //
        
public:

    // Processor Port
    ProcessorPort pport = ProcessorPort(c64);
    
    // CPU debugger
    CPUDebugger debugger = CPUDebugger(cpu);
    

    //
    // Chip properties
    //
    
public:
    
    bool isC64CPU() const;
    bool isDriveCPU() const;

    
    //
    // Initializing
    //
    
public:
    
    CPU(C64& ref);
    CPU(CPURevision cpuModel, C64& ref);
    
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
    // Configuring
    //

public:

    // Selects the emulated CPU model
    void setModel(CPURevision cpuModel);


    //
    // Methods from C64Object
    //

    const char *getDescription() const override { return "CPU"; } // TODO
    void _dump(Category category, std::ostream& os) const override;

    
    //
    // Methods from C64Component
    //

private:
    
    void _reset(bool hard) override;
    void _inspect() const override;
    void _debugOn() override;
    void _debugOff() override;

    template <class T>
    void applyToPersistentItems(T& worker)
    {
    }
    
    template <class T>
    void applyToResetItems(T& worker, bool hard = true)
    {
        if (hard) {
            
            worker << clock;
        }
        
        worker
        
        << next
        << reg.a
        << reg.x
        << reg.y
        << reg.pc
        << reg.pc0
        << reg.sp
        << reg.sr.n
        << reg.sr.v
        << reg.sr.b
        << reg.sr.d
        << reg.sr.i
        << reg.sr.z
        << reg.sr.c
        << reg.adl
        << reg.adh
        << reg.idl
        << reg.d
        << reg.ovl
        << rdyLine
        << rdyLineUp
        << rdyLineDown
        << nmiLine
        << irqLine
        >> edgeDetector
        >> levelDetector
        << doNmi
        << doIrq;
    }
    
    isize _size() override { COMPUTE_SNAPSHOT_SIZE }
    u64 _checksum() override { COMPUTE_SNAPSHOT_CHECKSUM }
    isize _load(const u8 *buffer) override { LOAD_SNAPSHOT_ITEMS }
    isize _save(u8 *buffer) override { SAVE_SNAPSHOT_ITEMS }

    
    //
    // Analyzing
    //
    
public:
    
    // Returns the result of the latest inspection
    CPUInfo getInfo() const { return C64Component::getInfo(info); }

    
    //
    // Accessing properties
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

    /*
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
    u8 getPWithClearedB() const;
    void setP(u8 p);
    void setPWithoutB(u8 p);
     */


    //
    // Accessing memory
    //

private:

    u8 peek(u16 addr);
    u8 peekZP(u8 addr);
    u8 peekStack(u8 sp);

    void peekIdle(u16 addr) { (void)peek(addr); }
    void peekZPIdle(u8 addr) { (void)peekZP(addr); }
    void peekStackIdle(u8 sp) { (void)peekStack(sp); }

    u8 spypeek(u16 addr) const;

    void poke(u16 addr, u8 value);
    void pokeZP(u8 addr, u8 value);
    void pokeStack(u8 sp, u8 value);


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

    // Returns true if the CPU is jammed
    bool isJammed() const { return next == JAM || next == JAM_2; }
    
    // Returns true if the next cycle marks the beginning of an instruction
    bool inFetchPhase() const { return next == fetch; }

    // Executes the next micro instruction
    void executeOneCycle();

private:

    // Called after the last microcycle has been completed
    void done();
};
