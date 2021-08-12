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

class PRGFile : public AnyCollection {

public:
    
    static bool isCompatiblePath(const string &name);
    static bool isCompatibleStream(std::istream &stream);
    
    static PRGFile *makeWithFileSystem(class FSDevice &fs);

    
    //
    // Initializing
    //
    
    PRGFile() : AnyCollection() { }
    PRGFile(isize capacity) : AnyCollection(capacity) { }
    
    
    //
    // Methods from C64Object
    //
    
    const char *getDescription() const override { return "PRGFile"; }
    
    
    //
    // Methods from AnyFile
    //

    FileType type() const override { return FILETYPE_PRG; }
    
    
    //
    // Methods from AnyCollection
    //

    PETName<16> collectionName() override;
    isize collectionCount() const override;
    PETName<16> itemName(isize nr) const override;
    u64 itemSize(isize nr) const override;
    u8 readByte(isize nr, u64 pos) const override;
};
