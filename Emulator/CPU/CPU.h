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

    // Processor Port (DEPRECATED)
    ProcessorPort pport = ProcessorPort(c64);

    /* Processor port
     *
     * Clock cycle when floating bit values reach zero.
     * Bit 3, 6, and 7 of the processor need special attention. When the
     * direction of these bits is changed from output to input, there will be
     * no external signal driving them. As a result, these bits will be in a
     * floating state and act as capacitors. They will discharge slowly and
     * eventually reach zero. These variables are used to indicate when the
     * zero level is reached. All three variables are queried in readPort() and
     * comply to the following rules:
     *
     *    dischargeCycleBit > current cycle => bit reads as 1
     *                                         (if configured as input)
     *    otherwise                         => bit reads as 0
     *                                         (if configured as input)
     */
    u64 dischargeCycleBit3;
    u64 dischargeCycleBit6;
    u64 dischargeCycleBit7;

    
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
        << doIrq

        << dischargeCycleBit3
        << dischargeCycleBit6
        << dischargeCycleBit7;
    }
    
    isize _size() override { COMPUTE_SNAPSHOT_SIZE }
    u64 _checksum() override { COMPUTE_SNAPSHOT_CHECKSUM }
    isize _load(const u8 *buffer) override { LOAD_SNAPSHOT_ITEMS }
    isize _save(u8 *buffer) override { SAVE_SNAPSHOT_ITEMS }


    //
    // Methods from Peddle
    //

public:

    virtual u8 read8(u16 addr) const override;
    virtual u16 read16Reset(u16 addr) const override;
    virtual u8 read8Dasm(u16 addr) const override;
    virtual void write8(u16 addr, u8 val) const override;

    virtual void writePort(u8 val) override;
    virtual void writePortDir(u8 val) override;
    virtual u8 externalPortBits() const override;

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
