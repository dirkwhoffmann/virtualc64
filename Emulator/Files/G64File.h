// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "AnyFile.h"

class G64File : public AnyFile {

public:

    //
    // Class methods
    //

    static bool isCompatiblePath(const std::string &name);
    static bool isCompatibleStream(std::istream &stream);
    
    static G64File *makeWithDisk(class Disk &disk) throws;
    static G64File *makeWithDisk(class Disk &disk, ErrorCode *err);

    
    //
    // Initializing
    //
    
    G64File() { };
    G64File(isize capacity);
    
    
    //
    // Methods from C64Object
    //
    
    const string getDescription() const override { return "G64File"; }

    
    //
    // Methods from AnyFile
    //
    
    FileType type() const override { return FILETYPE_G64; }
        
  
    //
    // Reading data from a track
    //

public:
    
    // Returns the size of a certain haltrack in bytes
    isize getSizeOfHalftrack(Halftrack ht) const;
        
    // Copies a certain track into a buffer
    void copyHalftrack(Halftrack ht, u8 *buf) const;
    
private:
    
    isize getStartOfHalftrack(Halftrack ht) const;
};
