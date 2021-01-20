// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------
// THIS FILE MUST CONFORM TO ANSI-C TO BE COMPATIBLE WITH SWIFT
// -----------------------------------------------------------------------------

#pragma once

//
// Enumerations
//

enum_long(MOUSE_MODEL)
{
    MOUSE_C1350,
    MOUSE_C1351,
    MOUSE_NEOS,
    
    MOUSE_COUNT
};
typedef MOUSE_MODEL MouseModel;

enum_long(PORT_ID)
{
    PORT_NONE,
    PORT_ONE,
    PORT_TWO,
    
    PORT_COUNT
};
typedef PORT_ID PortId;


//
// Structures
//

typedef struct
{
    MouseModel model;
}
MouseConfig;
