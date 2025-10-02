// -----------------------------------------------------------------------------
// This file is part of Peddle - A MOS 65xx CPU emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Published under the terms of the MIT License
// -----------------------------------------------------------------------------

#pragma once

#include "BasicTypes.h"

namespace vc64::peddle {

// Base structure for a single breakpoint or watchpoint
struct Guard {

    // The observed address
    u32 addr;

    // Disabled guards never trigger
    bool enabled;

    // Counts the number of hits
    long hits;

    // Ignore counter
    long ignore;

public:

    // Returns true if the guard hits
    bool eval(u32 addr);

    // Replaces the address by another
    void moveTo(u32 newAddr);
};

}
