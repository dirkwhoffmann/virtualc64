// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "AnyDisk.h"
#include "Disk.h"

class G64File : public AnyDisk {

    // Number of the currently selected halftrack (0 = nothing selected)
    Halftrack selectedHalftrack = 0;
 
public:

    static bool isCompatibleName(const std::string &name);
    static bool isCompatibleStream(std::istream &stream);
    
    static G64File *makeWithDisk(Disk *disk);

    
    //
    // Initializing
    //
    
    G64File() : AnyDisk() { };
    G64File(size_t capacity);
    
    
    //
    // Methods from C64Object
    //
    
    const char *getDescription() override { return "G64File"; }

    
    //
    // Methods from AnyFile
    //
    
    FileType type() override { return FILETYPE_G64; }
        
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
