// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "AnyFile.h"
#include "Disk.h"

class G64File : public AnyFile {

public:

    //
    // Class methods
    //

    static bool isCompatibleName(const std::string &name);
    static bool isCompatibleStream(std::istream &stream);
    
    static G64File *makeWithDisk(Disk *disk) throws;
    static G64File *makeWithDisk(Disk *disk, ErrorCode *err);

    
    //
    // Initializing
    //
    
    G64File() { };
    G64File(usize capacity);
    
    
    //
    // Methods from C64Object
    //
    
    const char *getDescription() override { return "G64File"; }

    
    //
    // Methods from AnyFile
    //
    
    FileType type() override { return FILETYPE_G64; }
        
  
    //
    // Reading data from a track
    //

public:
    
    // Returns the size of a certain haltrack in bytes
    usize getSizeOfHalftrack(Halftrack ht);
        
    // Copies a certain track into a buffer
    void copyHalftrack(Halftrack ht, u8 *buf);
    
private:
    
    usize getStartOfHalftrack(Halftrack ht);
};
