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

enum_long(MOUSE_MODEL)
{
    MOUSE_C1350,
    MOUSE_C1351,
    MOUSE_NEOS,
    MOUSE_COUNT
};
typedef MOUSE_MODEL MouseModel;

typedef struct
{
    MouseModel model;
}
MouseConfig;
