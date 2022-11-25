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

    /* Assign a unique ID to this CPU, starting at 0. The ID is not needed for
     * emulation. It is merely provided to allow the client code to easily
     * distinguish between different CPU instances. For instance, VirtualC64
     * uses it to distinguish the CPUs of the attached floppy drives.
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

#define CHECK_WATCHPOINT \
if constexpr (ENABLE_WATCHPOINTS) { \
if ((flags & CPU_CHECK_WP) && debugger.watchpointMatches(addr)) { \
watchpointReached(addr); \
}}

template <CPURevision C> u8
Peddle::peek(u16 addr)
{
    CHECK_WATCHPOINT
    return read8(addr); // TODO: Apply address bus mask
}

template <CPURevision C> u8
Peddle::peekZP(u8 addr)
{
    CHECK_WATCHPOINT
    return read8(addr);
}

template <CPURevision C> u8
Peddle::peekStack(u8 addr)
{
    CHECK_WATCHPOINT
    return read8(u16(addr) + 0x100);
}

template <CPURevision C> u8
Peddle::spypeek(u16 addr) const
{
    return read8(addr);
}

template <CPURevision C> void
Peddle::peekIdle(u16 addr)
{
    if (EMULATE_IDLE_ACCESSES) {

        CHECK_WATCHPOINT
        (void)read8(addr);
    }
}

template <CPURevision C> void
Peddle::peekZPIdle(u8 addr)
{
    if (EMULATE_IDLE_ACCESSES) {

        CHECK_WATCHPOINT
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
Peddle::poke(u16 addr, u8 value)
{
    CHECK_WATCHPOINT
    write8(addr, value);
}

template <CPURevision C> void
Peddle::pokeZP(u8 addr, u8 value)
{
    CHECK_WATCHPOINT
    write8(u16(addr), value);
}

template <CPURevision C> void
Peddle::pokeStack(u8 addr, u8 value)
{
    CHECK_WATCHPOINT
    write8(u16(addr) + 0x100, value);
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

#include "PeddleExec.cpp"
}
