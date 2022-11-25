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
#include "ProcessorPort.h"

using namespace peddle;

class CPU : public peddle::Peddle {

    // Result of the latest inspection
    mutable CPUInfo info = { };

public:

    // Processor Port
    ProcessorPort pport = ProcessorPort(c64);

    
    //
    // Initializing
    //
    
public:
    
    CPU(C64& ref);
    CPU(CPURevision cpuModel, C64& ref);

    bool isC64CPU() const { return cpuModel == MOS_6510; }
    bool isDriveCPU() const { return cpuModel == MOS_6502; }
    bool isDriveCPU(isize nr) const { return isDriveCPU() && id == nr + 1; }


    //
    // Methods from C64Object
    //

private:

    const char *getDescription() const override;
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

        << flags
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
    // Methods from Peddle
    //

    virtual u8 read8(u16 addr) const override;
    virtual u16 read16Reset(u16 addr) const override;
    virtual u8 read8Dasm(u16 addr) const override;
    virtual void write8(u16 addr, u8 val) const override;


    virtual void nmiWillTrigger() override;
    virtual void nmiDidTrigger() override;
    virtual void cpuDidJam() override;
    virtual void instructionLogged() override;
    virtual void breakpointReached(u16 addr) override;
    virtual void watchpointReached(u16 addr) override;


    //
    // Analyzing
    //
    
public:
    
    // Returns the result of the latest inspection
    CPUInfo getInfo() const { return C64Component::getInfo(info); }
};
