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

    // Header signature
    static const u8 magicBytes[];
    
    
    //
    // Class methods
    //
    
public:

    // Returns true iff buffer contains a P00 file
    static bool isCompatibleBuffer(const u8 *buffer, size_t length);
    
    // Returns true iff the specified file is a P00 file
    static bool isCompatibleFile(const char *filename);
    
    
    //
    // Constructing
    //
    
    static P00File *makeWithFileSystem(class FSDevice *fs, int item = 0);
    
    P00File() : AnyCollection() { }
    P00File(size_t capacity) : AnyCollection(capacity) { }
    
    const char *getDescription() override { return "P00File"; }


    //
    // Methods from AnyFile
    //
    
    const char *getName() override;
    FileType type() override { return FILETYPE_P00; }
    bool matchingBuffer(const u8 *buf, size_t len) override;
    bool matchingFile(const char *path) override;
    

    //
    // Methods from AnyCollection
    //

    PETName<16> collectionName() override;
    u64 collectionCount() override;
    PETName<16> itemName(unsigned nr) override;
    u64 itemSize(unsigned nr) override;
    u8 readByte(unsigned nr, u64 pos) override;
};
