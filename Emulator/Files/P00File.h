// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "AnyCollection.h"

class P00File : public AnyCollection {

public:

    static bool isCompatibleName(const std::string &name);
    static bool isCompatibleStream(std::istream &stream);
    
    static P00File *makeWithFileSystem(class FSDevice &fs);
    
    P00File() : AnyCollection() { }
    P00File(usize capacity) : AnyCollection(capacity) { }

    //
    // Methods from C64Object
    //

    const char *getDescription() const override { return "P00File"; }


    //
    // Methods from AnyFile
    //
    
    FileType type() const override { return FILETYPE_P00; }
    PETName<16> getName() const override;


    //
    // Methods from AnyCollection
    //

    PETName<16> collectionName() override;
    u64 collectionCount() const override;
    PETName<16> itemName(unsigned nr) const override;
    u64 itemSize(unsigned nr) const override;
    u8 readByte(unsigned nr, u64 pos) const override;
};
