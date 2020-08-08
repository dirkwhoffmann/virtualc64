// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "C64.h"

C64CPU::C64CPU(CPUModel model, C64& ref, Memory &memref) : CPU(model, ref, memref)
{
    
}

u8
C64CPU::peek(u16 addr)
{
    return mem.peek(addr);
}

u8
C64CPU::peekZP(u16 addr)
{
    return mem.peekZP(addr);
}

u8
C64CPU::peekStack(u16 addr)
{
    return mem.peekStack(addr);
}

void
C64CPU::poke(u16 addr, u8 value)
{
    return mem.poke(addr, value);
}

void
C64CPU::pokeZP(u16 addr, u8 value)
{
    return mem.pokeZP(addr, value);
}

void
C64CPU::pokeStack(u16 addr, u8 value)
{
    return mem.pokeStack(addr, value);
}
