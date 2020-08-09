// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _C64_CPU_H
#define _C64_CPU_H

#include "CPU.h"

class C64CPU : public CPU<C64Memory> {
            
public:
    
    C64CPU(C64& ref, C64Memory& memref);
    
    CPUModel model() override { return MOS_6510; }
    bool isC64CPU() override { return true; }
    bool isDriveCPU() override { return false; }
    
    /*
    u8 peek(u16 addr) override;
    u8 peekZP(u16 addr) override;
    u8 peekStack(u16 addr) override;
    void peekIdle(u16 addr) override;
    void peekZPIdle(u16 addr) override;
    void peekStackIdle(u16 addr) override;
    u8 spypeek(u16 addr) override;
    void poke(u16 addr, u8 value) override;
    void pokeZP(u16 addr, u8 value) override;
    void pokeStack(u16 addr, u8 value) override;
    */
};
    
#endif
