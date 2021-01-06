// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "Aliases.h"
#include "AnyFile.h"
#include "PETName.h"
#include <string>

class AnyCollection : public AnyFile {

public:
    
    AnyCollection() : AnyFile() { }
    AnyCollection(usize capacity) : AnyFile(capacity) { }
    virtual ~AnyCollection() { }
    
    // Returns the name of this collection
    virtual PETName<16> collectionName() = 0;
    
    // Returns the number of items stored in this collection
    virtual u64 collectionCount() = 0;
    
    // Returns the name of a certain item
    virtual PETName<16> itemName(unsigned nr) = 0;
    
    // Returns the size of a certain in bytes
    virtual u64 itemSize(unsigned nr) = 0;
        
    // Reads a byte from a certain item
    virtual u8 readByte(unsigned nr, u64 pos) = 0;

    
    //
    // Derived functions
    //

    // Reads a word from a certain item in little endian or big endian format
    u16 readWordBE(unsigned nr, u64 pos);
    u16 readWordLE(unsigned nr, u64 pos);
    
    // Return the load address of this item
    u16 itemLoadAddr(unsigned nr);

    // Copies an item into a buffer
    virtual void copyItem(unsigned nr, u8 *buf, u64 len, u64 offset = 0);
};
