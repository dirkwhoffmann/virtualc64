// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "C64.h"

/*
#define CHECK_FOR_WATCHPOINT \
if (flags & CPU_CHECK_WP && debugger.watchpointMatches(addr)) { \
    c64.signalWatchpoint(); \
}
*/

#define CHECK_FOR_WATCHPOINT

C64CPU::C64CPU(C64& ref, C64Memory& memref) : CPU(ref, memref)
{
    setDescription("CPU");
}

u8
C64CPU::peek(u16 addr)
{
    CHECK_FOR_WATCHPOINT
    return mem.peek(addr);
}

u8
C64CPU::peekZP(u16 addr)
{
    CHECK_FOR_WATCHPOINT
    return mem.peekZP(addr);
}

u8
C64CPU::peekStack(u16 addr)
{
    CHECK_FOR_WATCHPOINT
    return mem.peekStack(addr);
}

void
C64CPU::peekIdle(u16 addr)
{
    (void)mem.peek(addr);
}

void
C64CPU::peekZPIdle(u16 addr)
{
    (void)mem.peekZP(addr);
}

void
C64CPU::peekStackIdle(u16 addr)
{
    (void)mem.peekStack(addr);
}

u8
C64CPU::spypeek(u16 addr)
{
    return mem.spypeek(addr);
}

void
C64CPU::poke(u16 addr, u8 value)
{
    CHECK_FOR_WATCHPOINT
    return mem.poke(addr, value);
}

void
C64CPU::pokeZP(u16 addr, u8 value)
{
    CHECK_FOR_WATCHPOINT
    return mem.pokeZP(addr, value);
}

void
C64CPU::pokeStack(u16 addr, u8 value)
{
    CHECK_FOR_WATCHPOINT
    return mem.pokeStack(addr, value);
}
