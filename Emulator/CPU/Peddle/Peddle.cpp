// -----------------------------------------------------------------------------
// This file is part of Peddle - A MOS Technology 65xx CPU emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Published under the terms of the MIT License
// -----------------------------------------------------------------------------

#include "Peddle.h"

Peddle::Peddle(C64 &ref) : SubComponent(ref)
{
    static isize counter = 0;

    /* Assign a unique ID to this CPU, starting at 0. The ID is not needed for
     * emulation. It is merely provided to allow the client code to easily
     * distinguish between different CPU instances. For instance, VirtualC64
     * uses it to distinguish the CPUs of the attached floppy drives.
     */
    id = counter++;
}

Peddle::~Peddle()
{

}
