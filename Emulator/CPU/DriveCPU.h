// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _DRIVE_CPU_H
#define _DRIVE_CPU_H

#include "CPU.h"

class DriveCPU : public CPU {
    
    // Reference to the connected memory
    Memory &mem;

    //
    // Constructing and serializing
    //
    
public:
    
    DriveCPU(CPUModel model, C64& ref, Memory &memref);
    
    u8 peek(u16 addr) override;
    u8 peekZP(u16 addr) override;
    u8 peekStack(u16 addr) override;
    u8 spypeek(u16 addr) override;
    void poke(u16 addr, u8 value) override;
    void pokeZP(u16 addr, u8 value) override;
    void pokeStack(u16 addr, u8 value) override;
};

#endif
