// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _G64_FILE_H
#define _G64_FILE_H

#include "AnyDisk.h"
#include "Disk.h"

class G64File : public AnyDisk {

    // Header signature
    static const u8 magicBytes[];
    
    // Number of the currently selected halftrack (0 = nothing selected)
    Halftrack selectedHalftrack = 0;
    
    
    //
    // Class functions
    //
    
public:

    static bool isG64Buffer(const u8 *buffer, size_t length);
    static bool isG64File(const char *filename);
    
    
    //
    // Initializing
    //
    
public:
    
    G64File() : AnyDisk() { };
    G64File(size_t capacity);
    const char *getDescription() override { return "G64File"; }

    static G64File *makeWithBuffer(const u8 *buffer, size_t length);
    static G64File *makeWithFile(const char *path);
    static G64File *makeWithDisk(Disk *disk);
    
    
    //
    // Methods from AnyC64File
    //
    
    FileType type() override { return FILETYPE_G64; }
    bool hasSameType(const char *path) override { return G64File::isG64File(path); }
    
    
    //
    // Methods from AnyArchive (this API is not supported by the G64 format)
    //
    
    /*
    int numberOfItems() override { assert(false); return 0; };
    size_t getSizeOfItem() override { assert(false); return 0; }
    const char *getNameOfItem() override { assert(false); return ""; }
    const char *getTypeOfItem() override { assert(false); return ""; }
    u16 getDestinationAddrOfItem() override { assert(false); return 0; }
    void selectItem(unsigned n) override { assert(false); }
    */
    
    //
    // Methods from AnyDisk
    //
    
    int numberOfHalftracks() override { return 84; }
    void selectHalftrack(Halftrack ht) override;
    size_t getSizeOfHalftrack() override;
    void seekHalftrack(long offset) override;
    
private:
    
    long getStartOfHalftrack(Halftrack ht);
};

#endif

