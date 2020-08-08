// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "C64.h"

DriveCPU::DriveCPU(CPUModel model, C64& ref, DriveMemory &memref) : CPU(model, ref), mem(memref)
{

}

u8
DriveCPU::peek(u16 addr)
{
    return mem.peek(addr);
}
 
u8
DriveCPU::peekZP(u16 addr)
{
    return mem.peekZP(addr);
}

u8
DriveCPU::peekStack(u16 addr)
{
    return mem.peekStack(addr);
}

void
DriveCPU::peekIdle(u16 addr)
{
    (void)mem.peek(addr);
}
 
void
DriveCPU::peekZPIdle(u16 addr)
{
    (void)mem.peekZP(addr);
}

void
DriveCPU::peekStackIdle(u16 addr)
{
    (void)mem.peekStack(addr);
}

u8
DriveCPU::spypeek(u16 addr)
{
    return mem.spypeek(addr);
}

void
DriveCPU::poke(u16 addr, u8 value)
{
    mem.poke(addr, value);
}

void
DriveCPU::pokeZP(u16 addr, u8 value)
{
    mem.pokeZP(addr, value);
}

void
DriveCPU::pokeStack(u16 addr, u8 value)
{
    mem.pokeStack(addr, value);
}
