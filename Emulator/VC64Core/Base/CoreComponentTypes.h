// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// This FILE is dual-licensed. You are free to choose between:
//
//     - The GNU General Public License v3 (or any later version)
//     - The Mozilla Public License v2
//
// SPDX-License-Identifier: GPL-3.0-or-later OR MPL-2.0
// -----------------------------------------------------------------------------

#pragma once

#include "BasicTypes.h"

namespace vc64 {

//
// Enumerations
//

enum class Class : long
{
    Generic,
    
    // Components
    C64,
    CPU,
    CIA,
    Host,
    Memory,
    PIA,
    SID,
    SIDBridge,
    TOD,
    VIA,
    VICII,
    
    // Ports
    AudioPort,
    ControlPort,
    ExpansionPort,
    PowerPort,
    RS232,
    SerialPort,
    UserPort,
    VideoPort,
    
    // Peripherals
    Cartridge,
    Datasette,
    Drive,
    DriveMemory,
    Joystick,
    Keyboard,
    Monitor,
    Mouse,
    Paddle,
    ParallelCable
};

}
