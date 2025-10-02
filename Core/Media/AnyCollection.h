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

#include "AnyFile.h"

namespace vc64 {

class AnyCollection : public AnyFile {

public:
    
    AnyCollection() : AnyFile() { }
    AnyCollection(isize capacity) : AnyFile() { init(capacity); }
    virtual ~AnyCollection() = default;
    
    // Returns the name of this collection
    virtual PETName<16> collectionName() = 0;
    
    // Returns the number of items stored in this collection
    virtual isize collectionCount() const = 0;
    
    // Returns the name of a certain item
    virtual PETName<16> itemName(isize nr) const = 0;
    
    // Returns the size of a certain in bytes
    virtual isize itemSize(isize nr) const = 0;

    // Reads a byte from a certain item
    virtual u8 readByte(isize nr, isize pos) const = 0;

    
    //
    // Derived functions
    //

    // Reads a word from a certain item in little endian or big endian format
    u16 readWordBE(isize nr, isize pos) const;
    u16 readWordLE(isize nr, isize pos) const;
    
    // Return the load address of this item
    u16 itemLoadAddr(isize nr) const;

    // Copies an item into a buffer
    virtual void copyItem(isize nr, u8 *buf, isize len, isize offset = 0) const;
};

}
