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

#include "AnyArchive.h"
#include "AnyCollection.h"

class PRGFile : public AnyArchive {

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
    static PRGFile *makeWithAnyArchive(AnyArchive *other, int item = 0);
    static PRGFile *makeWithAnyCollection(AnyCollection *collection, int item = 0);

    
    //
    // Initializing
    //
    
    PRGFile() : AnyArchive() { }
    PRGFile(size_t capacity) : AnyArchive(capacity) { }
    
    const char *getDescription() override { return "PRGFile"; }
    
    
    //
    // Methods from AnyFile
    //

    FileType type() override { return FILETYPE_PRG; }
    bool hasSameType(const char *filename) override { return isPRGFile(filename); }
    
    
    //
    // Methods from AnyCollection
    //

    std::string collectionName() override;
    u64 collectionCount() override;
    std::string itemName(unsigned nr) override;
    u64 itemSize(unsigned nr) override;
    u8 readByte(unsigned nr, u64 pos) override;
 
    
    //
    // Methods from AnyArchive
    //
    
    int numberOfItems() override { return 1; }
    void selectItem(unsigned item) override;
    const char *getTypeOfItem() override { return "PRG"; }
    const char *getNameOfItem() override { return getName(); }
    size_t getSizeOfItem() override { return size - 2; }
    void seekItem(long offset) override;
    u16 getDestinationAddrOfItem() override;
};

#endif
