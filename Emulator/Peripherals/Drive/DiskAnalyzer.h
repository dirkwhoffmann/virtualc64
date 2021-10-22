// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "DiskAnalyzerTypes.h"
#include "DiskTypes.h"
#include "C64Object.h"
#include "IO.h"

class DiskAnalyzer: public C64Object {
  
    friend class Disk;
    
    // Length for each halftrack
    isize length[85];
    
    // Disk data (each halftrack repeated twice, one byte for each bit on disk)
    u8 *data[85];
        
    // Result of the analysis
    DiskInfo diskInfo = { };
    
    // Error log created by analyzeTrack
    std::vector<string> errorLog[85];

    // Stores the start offset of the erroneous bit sequence
    std::vector<isize> errorStartIndex[85];

    // Stores the end offset of the erroneous bit sequence
    std::vector<isize> errorEndIndex[85];

    // Textual representation of track data
    char text[maxBitsOnTrack + 1] = { };

    
    //
    // Class methods
    //
    
    // Decodes a GCR-encoded nibble or byte
    u8 decodeGcrNibble(u8 *gcrBits);
    u8 decodeGcr(u8 *gcrBits);
    
    
    //
    // Initializing
    //
    
public:

    DiskAnalyzer(const class Disk &disk);
    ~DiskAnalyzer();
    
    
    //
    // Methods from C64Object
    //

private:
    
    const char *getDescription() const override { return "DiskAnalyzer"; }


    //
    // Analyzing the disk
    //

public:
    
    // Returns the length of a halftrack in bits
    isize lengthOfTrack(Track t) const;
    isize lengthOfHalftrack(Halftrack ht) const;
    
private:
    
    // Analyzes the whole disk, a single track, or a single sector
    void analyzeDisk();
    TrackInfo analyzeTrack(Track t);
    TrackInfo analyzeHalftrack(Halftrack ht);

private:
    
    // Checks the integrity of the sector block structure
    void analyzeSectorBlocks(Halftrack ht, TrackInfo &trackInfo);
    void analyzeSectorHeaderBlock(Halftrack ht, isize offset, TrackInfo &trackInfo);
    void analyzeSectorDataBlock(Halftrack ht, isize offset, TrackInfo &trackInfo);

    // Writes an error message into the error log
    void log(Halftrack ht, isize begin, isize length, const char *fmt, ...);
    
public:
    
    // Returns the layout of a certain track
    const SectorInfo &sectorLayout(Halftrack ht, Sector nr);
    
    // Returns the number of entries in the error log
    isize numErrors(Halftrack ht) { return errorLog[ht].size(); }
    
    // Reads an error message from the error log
    string errorMessage(Halftrack ht, isize nr) const { return errorLog[ht].at(nr); }
    
    // Reads the error begin index from the error log
    isize firstErroneousBit(Halftrack ht, isize nr) const { return errorStartIndex[ht].at(nr); }
    
    // Reads the error end index from the error log
    isize lastErroneousBit(Halftrack ht, isize nr) const { return errorEndIndex[ht].at(nr); }
    
    // Returns a textual representation of the disk name
    const char *diskNameAsString();
    
    // Returns a textual representation of the data stored in trackInfo
    const char *trackBitsAsString(Halftrack ht);

    // Returns a textual representation of the data stored in trackInfo
    const char *sectorHeaderBytesAsString(Halftrack ht, Sector nr, bool hex);

    // Returns a textual representation of the data stored in trackInfo
    const char *sectorDataBytesAsString(Halftrack ht, Sector nr, bool hex);

private:
    
    // Returns a textual representation
    const char *sectorBytesAsString(u8 *buffer, isize length, bool hex);
};
