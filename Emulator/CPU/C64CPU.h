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

class C64CPU : public CPU {
    
    //
    // Constructing and serializing
    //
    
public:
    
    C64CPU(CPUModel model, C64& ref, Memory &memref);
    
};
    
#endif
