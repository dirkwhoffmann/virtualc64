// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include "FileSystems/Amiga/FSTypes.h"

namespace retro::vault::amiga {

typedef struct
{
    const char *name;
    u16 signature[14];
    const u8 *image;
    isize size;
    BootBlockType type;
}
BBRecord;

class FSBootBlockImage {

    // Image data
    u8 data[1024];
    
public:
    
    // Result of the data inspection
    BootBlockType type = BootBlockType::CUSTOM;
    const char *name = "Custom boot block";
    
    // Constructors
    FSBootBlockImage(const u8 *buf1, const u8 *buf2);
    FSBootBlockImage(const u8 *buf);
    FSBootBlockImage(const string &name);
    FSBootBlockImage(BootBlockId id);
    
    // Exports the image
    void write(u8 *buffer, isize first = 0, isize last = 0);
};

}
