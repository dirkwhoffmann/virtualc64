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
    Halftrack selectedHalftrack = 0;
 
    // File pointer. An offset into the data range of the selected track
    long tFp = -1;
    
    // End of file position. This value equals the last valid offset plus 1
    long tEof = -1;

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
    int numberOfTracks() { return (numberOfHalftracks() + 1) / 2; }

    void selectHalftrack(Halftrack ht);
    void selectTrack(Track t) { selectHalftrack(2 * t - 1); }

    
    //
    // Reading data from a track
    //
    
    // Returns the size of the selected haltrack in bytes
    usize getSizeOfHalftrack();
    usize getSizeOfTrack() { return getSizeOfHalftrack(); }

    // Moves the file pointer to the specified offset
    void seekHalftrack(long offset);
    void seekTrack(long offset) { seekHalftrack(offset); }

    // Reads a byte from the selected track (-1 = EOF)
    virtual int readHalftrack();
    virtual int readTrack() { return readHalftrack(); }
        
    // Copies the selected track into the specified buffer
    virtual void copyHalftrack(u8 *buffer, usize offset = 0);
    virtual void copyTrack(u8 *buffer, usize offset = 0) { copyHalftrack(buffer, offset); }
    
private:
    
    long getStartOfHalftrack(Halftrack ht);
};
