// -----------------------------------------------------------------------------
// This file is part of Peddle - A MOS Technology 65xx CPU emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Published under the terms of the MIT License
// -----------------------------------------------------------------------------

#include "PeddleConfig.h"
#include "Peddle.h"

namespace peddle {

Peddle::Peddle(C64 &ref) : SubComponent(ref)
{
    static isize counter = 0;

    /* Assign a unique ID to this CPU. The ID is not needed for emulation. It
     * is merely provided to allow the client code to easily distinguish
     * between different CPU instances. For instance, VirtualC64 uses it to
     * distinguish the CPUs of the attached floppy drives.
     */
    id = counter++;

    // Initialize the microinstruction lookup table
    registerInstructions();
}

Peddle::~Peddle()
{

}

void
Peddle::setModel(CPURevision cpuModel)
{
    this->cpuModel = cpuModel;
}

u16
Peddle::hasProcessorPort() const
{
    switch (cpuModel) {

        case MOS_6502:  return hasProcessorPort<MOS_6502>();
        case MOS_6507:  return hasProcessorPort<MOS_6507>();
        case MOS_6510:  return hasProcessorPort<MOS_6510>();
        case MOS_8502:  return hasProcessorPort<MOS_8502>();

        default:
            fatalError;
    }

}

template <CPURevision C> u16
Peddle::hasProcessorPort() const
{
    return C == MOS_6510 || C == MOS_8502;
}

u16
Peddle::addrMask() const
{
    switch (cpuModel) {

        case MOS_6502:  return addrMask<MOS_6502>();
        case MOS_6507:  return addrMask<MOS_6507>();
        case MOS_6510:  return addrMask<MOS_6510>();
        case MOS_8502:  return addrMask<MOS_8502>();

        default:
            fatalError;
    }
}

template <CPURevision C> u16
Peddle::addrMask() const
{
    switch (C) {

        case MOS_6507:  return 0x1FFF;
        default:        return 0xFFFF;
    }
}

void
Peddle::pullDownNmiLine(IntSource bit)
{
    assert(bit != 0);

    // Check for falling edge on physical line
    if (!nmiLine) {
        edgeDetector.write(1);
    }

    nmiLine |= bit;
}

void
Peddle::releaseNmiLine(IntSource source)
{
    nmiLine &= ~source;
}

void
Peddle::pullDownIrqLine(IntSource source)
{
    assert(source != 0);

    irqLine |= source;
    levelDetector.write(irqLine);
}

void
Peddle::releaseIrqLine(IntSource source)
{
    irqLine &= ~source;
    levelDetector.write(irqLine);
}

void
Peddle::setRDY(bool value)
{
    if (rdyLine)
    {
        rdyLine = value;
        if (!rdyLine) rdyLineDown = clock;
    }
    else
    {
        rdyLine = value;
        if (rdyLine) rdyLineUp = clock;
    }
}

u8
Peddle::getP() const
{
    u8 result = 0b00100000;

    if (reg.sr.n) result |= N_FLAG;
    if (reg.sr.v) result |= V_FLAG;
    if (reg.sr.b) result |= B_FLAG;
    if (reg.sr.d) result |= D_FLAG;
    if (reg.sr.i) result |= I_FLAG;
    if (reg.sr.z) result |= Z_FLAG;
    if (reg.sr.c) result |= C_FLAG;

    return result;
}

u8
Peddle::getPWithClearedB() const
{
    return getP() & ~B_FLAG;
}

void
Peddle::setP(u8 p)
{
    setPWithoutB(p);
    reg.sr.b = (p & B_FLAG);
}

void
Peddle::setPWithoutB(u8 p)
{
    reg.sr.n = (p & N_FLAG);
    reg.sr.v = (p & V_FLAG);
    reg.sr.d = (p & D_FLAG);
    reg.sr.i = (p & I_FLAG);
    reg.sr.z = (p & Z_FLAG);
    reg.sr.c = (p & C_FLAG);
}


#define CHECK_WATCHPOINT \
if constexpr (ENABLE_WATCHPOINTS) { \
if ((flags & CPU_CHECK_WP) && debugger.watchpointMatches(addr)) { \
watchpointReached(addr); \
}}

template <CPURevision C> u8
Peddle::peek(u16 addr)
{
    CHECK_WATCHPOINT

    if (hasProcessorPort<C>()) {

        if (addr < 2) return addr ? readPort() : readPortDir();
    }
    return read8(addr & addrMask<C>());
}

template <CPURevision C> u8
Peddle::peekZP(u8 addr)
{
    CHECK_WATCHPOINT

    if (hasProcessorPort<C>()) {

        if (addr < 2) return addr ? readPort() : readPortDir();
    }
    return read8(addr & addrMask<C>());
}

template <CPURevision C> u8
Peddle::peekStack(u8 addr)
{
    CHECK_WATCHPOINT
    return read8(u16(addr) + 0x100);
}

template <CPURevision C> void
Peddle::peekIdle(u16 addr)
{
    if (EMULATE_IDLE_ACCESSES) {

        CHECK_WATCHPOINT

        if (hasProcessorPort<C>()) {

            if (addr < 2) return addr ? (void)readPort() : (void)readPortDir();
        }
        (void)read8(addr & addrMask<C>());
    }
}

template <CPURevision C> void
Peddle::peekZPIdle(u8 addr)
{
    if (EMULATE_IDLE_ACCESSES) {

        CHECK_WATCHPOINT

        if (hasProcessorPort<C>()) {

            if (addr < 2) return addr ? (void)readPort() : (void)readPortDir();
        }
        (void)read8(u16(addr));
    }
}

template <CPURevision C> void
Peddle::peekStackIdle(u8 addr)
{
    if (EMULATE_IDLE_ACCESSES) {

        CHECK_WATCHPOINT
        (void)read8(u16(addr) + 0x100);
    }
}

template <CPURevision C> void
Peddle::poke(u16 addr, u8 val)
{
    CHECK_WATCHPOINT

    if (hasProcessorPort<C>()) {

        if (addr < 2) { addr ? writePort(val) : writePortDir(val); return; }
    }
    write8(addr & addrMask<C>(), val);
}

template <CPURevision C> void
Peddle::pokeZP(u8 addr, u8 val)
{
    CHECK_WATCHPOINT

    if (hasProcessorPort<C>()) {

        if (addr < 2) { addr ? writePort(val) : writePortDir(val); return; }
    }
    write8(u16(addr), val);
}

template <CPURevision C> void
Peddle::pokeStack(u8 addr, u8 val)
{
    CHECK_WATCHPOINT
    write8(u16(addr) + 0x100, val);
}

u8
Peddle::readPort() const
{
    return (reg.pport.data & reg.pport.direction) | (externalPortBits() & ~reg.pport.direction);
}

#include "PeddleExec.cpp"
}
