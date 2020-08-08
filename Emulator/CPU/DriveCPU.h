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
    
    //
    // Constructing and serializing
    //
    
public:
    
    DriveCPU(CPUModel model, C64& ref, Memory &memref);
    
};

#endif
