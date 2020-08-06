// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _CPU_DEBUGGER_H
#define _CPU_DEBUGGER_H

#include "C64Component.h"

class CPUDebugger : public C64Component {
    
    //
    // Constructing and serializing
    //
    
public:
    
    CPUDebugger(C64 &ref);
    
    
    //
    // Methods from HardwareComponent
    //
    
private:
    
    void _reset() override;
};

#endif
