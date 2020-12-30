// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _P00_FILE_H
#define _P00_FILE_H

#include "AnyFile.h"
#include "AnyCollection.h"

#include "AnyArchive.h" // DEPRECATED

class P00File : public AnyArchive {

    // Header signature
    static const u8 magicBytes[];
    
    
    //
    // Class methods
    //
    
public:

    // Returns true iff buffer contains a P00 file
    static bool isP00Buffer(const u8 *buffer, size_t length);
    
    // Returns true iff the specified file is a P00 file
    static bool isP00File(const char *filename);
    
    
    //
    // Constructing
    //
    
    static P00File *makeWithBuffer(const u8 *buffer, size_t length);
    static P00File *makeWithFile(const char *path);
    static P00File *makeWithAnyArchive(AnyArchive *otherArchive); // DEPRECATED
    // static P00File *makeWithAnyCollection(AnyCollection *collection); // DEPRECATED
    static P00File *makeWithFileSystem(class FSDevice *fs, int item = 0);
    
    P00File() : AnyArchive() { }
    P00File(size_t capacity) : AnyArchive(capacity) { }
    
    const char *getDescription() override { return "P00File"; }


    //
    // Methods from AnyFile
    //
    
    const char *getName() override;
    FileType type() override { return FILETYPE_P00; }
    bool hasSameType(const char *filename) override { return isP00File(filename); }
    

    //
    // Methods from AnyCollection
    //

    std::string collectionName() override;
    u64 collectionCount() override;
    std::string itemName(unsigned nr) override;
    u64 itemSize(unsigned nr) override;
    u8 readByte(unsigned nr, u64 pos) override;
    
    
    //
    // Methods from AnyArchive (DEPRECATED)
    //
    
    int numberOfItems() override { return 1; }
    void selectItem(unsigned item) override;
    const char *getTypeOfItem() override { return "PRG"; }
    const char *getNameOfItem() override;
    size_t getSizeOfItem() override { return size - 0x1C; }
    void seekItem(long offset) override;
    u16 getDestinationAddrOfItem() override;
};
#endif
