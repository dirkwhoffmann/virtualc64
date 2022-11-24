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
    // void setModel(CPURevision cpuModel);


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
    // Executing the device
    //
    
public:

    // Returns true if the CPU is jammed
    // bool isJammed() const { return next == JAM || next == JAM_2; }
    
    // Returns true if the next cycle marks the beginning of an instruction
    // bool inFetchPhase() const { return next == fetch; }

    // Executes the next micro instruction
    void executeOneCycle();

private:

    // Called after the last microcycle has been completed
    void done();
};
