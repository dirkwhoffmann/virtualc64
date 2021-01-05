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

    // Number of the currently selected halftrack (0 = nothing selected)
    [[deprecated]] Halftrack selectedHalftrack = 0;
 
    // File pointer. An offset into the data range of the selected track
    [[deprecated]] long tFp = -1;
    
    // End of file position. This value equals the last valid offset plus 1
    [[deprecated]] long tEof = -1;

public:

    static bool isCompatibleName(const std::string &name);
    static bool isCompatibleStream(std::istream &stream);
    
    static G64File *makeWithDisk(Disk *disk);

    
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
    // Selecting tracks or halftracks
    //
    
    int numberOfHalftracks() { return 84; }
    
    [[deprecated]] void selectHalftrack(Halftrack ht);

    
    //
    // Reading data from a track
    //
    
    // Returns the size of a certain haltrack in bytes
    usize getSizeOfHalftrack(Halftrack ht);

    // Moves the file pointer to the specified offset
    [[deprecated]] void seekHalftrack(Halftrack ht, long offset);
        
    // Copies a certain track into a buffer
    void copyHalftrack(Halftrack ht, u8 *buf);
    
private:
    
    usize getStartOfHalftrack(Halftrack ht);
};
