// -----------------------------------------------------------------------------
// This file is part of Peddle - A MOS Technology 65xx CPU emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Published under the terms of the MIT License
// -----------------------------------------------------------------------------

#pragma once

#include "PeddleTypes.h"
#include "CPUDebugger.h"
#include "SubComponent.h"

class Peddle : public SubComponent {

    friend class CPUDebugger;
    friend class Breakpoints;
    friend class Watchpoints;

    //
    // Configuration
    //

protected:

    // Instance counter (to easily distinguish different CPUs)
    isize id;

    // Emulated CPU model
    CPURevision cpuModel = MOS_6510;


    //
    // Constructing
    //

public:

    Peddle(C64 &ref);
    virtual ~Peddle();

};
