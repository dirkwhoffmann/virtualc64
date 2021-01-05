// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "AnyCollection.h"

class P00File : public AnyCollection {

public:

    static bool isCompatibleName(const std::string &name);
    static bool isCompatibleStream(std::istream &stream);
    
    static P00File *makeWithFileSystem(class FSDevice *fs, int item = 0);
    
    P00File() : AnyCollection() { }
    P00File(size_t capacity) : AnyCollection(capacity) { }

    //
    // Methods from C64Object
    //

    const char *getDescription() override { return "P00File"; }


    //
    // Methods from AnyFile
    //
    
    PETName<16> getName() override;
    FileType type() override { return FILETYPE_P00; }
    

    //
    // Methods from AnyCollection
    //

    PETName<16> collectionName() override;
    u64 collectionCount() override;
    PETName<16> itemName(unsigned nr) override;
    u64 itemSize(unsigned nr) override;
    u8 readByte(unsigned nr, u64 pos) override;
};
