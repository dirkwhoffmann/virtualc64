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


//
// Enumerations
//

enum_long(COMP_TYPE)
{
    GenericClass,

    // Components
    C64Class,
    CPUClass,
    CIAClass,
    HostClass,
    MemoryClass,
    PIAClass,
    SIDClass,
    SIDBridgeClass,
    TODClass,
    VIAClass,
    VICIIClass,

    // Ports
    AudioPortClass,
    ControlPortClass,
    ExpansionPortClass,
    PowerPortClass,
    RS232Class,
    SerialPortClass,
    UserPortClass,
    VideoPortClass,

    // Peripherals
    CartridgeClass,
    DatasetteClass,
    DriveClass,
    DriveMemoryClass,
    JoystickClass,
    KeyboardClass,
    MonitorClass,
    MouseClass,
    PaddleClass,
    ParCableClass
};
typedef COMP_TYPE CType;
