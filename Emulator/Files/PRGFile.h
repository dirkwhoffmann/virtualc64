// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _PRG_FILE_H
#define _PRG_FILE_H

#include "AnyCollection.h"

class PRGFile : public AnyCollection {

public:

    //
    // Class methods
    //
    
    /* Returns true if buffer contains a PRG file. Since PRG files can only be
     * determined by their suffix, this function returns true unless you
     * provide a buffer with less than two bytes.
     */
    static bool isPRGBuffer(const u8 *buffer, size_t length);
    
    // Returns true iff the specified file is a PRG file
    static bool isPRGFile(const char *filename);
    
    
    //
    // Constructing
    //
    
    static PRGFile *makeWithBuffer(const u8 *buffer, size_t length);
    static PRGFile *makeWithFile(const char *path);
    static PRGFile *makeWithFileSystem(class FSDevice *fs, int item = 0);

    
    //
    // Initializing
    //
    
    PRGFile() : AnyCollection() { }
    PRGFile(size_t capacity) : AnyCollection(capacity) { }
    
    const char *getDescription() override { return "PRGFile"; }
    
    
    //
    // Methods from AnyFile
    //

    FileType type() override { return FileType_PRG; }
    bool hasSameType(const char *filename) override { return isPRGFile(filename); }
    
    
    //
    // Methods from AnyCollection
    //

    PETName<16> collectionName() override;
    u64 collectionCount() override;
    PETName<16> itemName(unsigned nr) override;
    u64 itemSize(unsigned nr) override;
    u8 readByte(unsigned nr, u64 pos) override;
};

#endif
