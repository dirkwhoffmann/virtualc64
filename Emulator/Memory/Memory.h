// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _MEMORY_H
#define _MEMORY_H

#include "C64Component.h"
// #include "MemoryTypes.h"


//! @brief    Common interface for C64 memory and VC1541 memory
class Memory : public C64Component {

    friend class CPU;
    friend class C64CPU;
    friend class DriveCPU;

protected:
    
    //
    // Constructing and serializing
    //
    
public:
        
    Memory(C64 &ref) : C64Component(ref) { };
    
    
private:
    
	virtual u8 peek(u16 addr) = 0;
    virtual u8 peekZP(u8 addr) = 0;
    virtual u8 peekStack(u8 sp) = 0;
    
public:
    
    virtual u8 spypeek(u16 addr) = 0;
    virtual void poke(u16 addr, u8 value) = 0;
    virtual void pokeZP(u8 addr, u8 value) = 0;
    virtual void pokeStack(u8 sp, u8 value) = 0;
};

#endif
