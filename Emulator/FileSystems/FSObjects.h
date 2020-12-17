// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _FS_OBJECTS_H
#define _FS_OBJECTS_H

#include "C64Object.h"

struct FSName : C64Object {
    
    // PETASCII string
    u8 pet[16 + 1];
    
    // ASCII representation (for pretty printing)
    char str[16 + 1];
    
    FSName(const u8 *_pet);
    const char *getDescription() override { return "FSName"; }
    const char *c_str() { return str; }
    bool operator== (FSName &rhs);
    void write(u8 *p);
};

#endif
