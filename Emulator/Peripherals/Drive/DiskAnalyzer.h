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
    
    // Disk data (each halftrack repeated twice, one byte for each bit on disk)
    u8 *data[85];
    
    // Maps a byte to an expanded 64 bit representation (DEPRECATED)
    u64 bitExpansion[256];

    // The disk under inspection
    class Disk *disk;
    
    // Track layout as determined by analyzeTrack
    TrackInfo trackInfo = { };

    // Error log created by analyzeTrack
    std::vector<string> errorLog;

    // Stores the start offset of the erroneous bit sequence
    std::vector<isize> errorStartIndex;

    // Stores the end offset of the erroneous bit sequence
    std::vector<isize> errorEndIndex;

    // Textual representation of track data
    char text[maxBitsOnTrack + 1] = { };

    
    //
    // Initializing
    //
    
public:

    DiskAnalyzer(class Disk *disk);
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
    u16 lengthOfTrack(Track t) const;
    u16 lengthOfHalftrack(Halftrack ht) const;
    
    /* Analyzes the sector layout. The functions determines the start and end
     * offsets of all sectors and writes them into variable trackLayout.
     */
    void analyzeTrack(Track t);
    void analyzeHalftrack(Halftrack ht);
    
private:
    
    // Checks the integrity of the sector block structure
    void analyzeSectorBlocks(Halftrack ht, TrackInfo &trackInfo);
    void analyzeSectorHeaderBlock(Halftrack ht, isize offset, TrackInfo &trackInfo);
    void analyzeSectorDataBlock(Halftrack ht, isize offset, TrackInfo &trackInfo);

    // Writes an error message into the error log
    void log(isize begin, isize length, const char *fmt, ...);
    
public:
    
    // Returns a sector layout from variable trackInfo
    SectorInfo sectorLayout(Sector nr) {
        assert(isSectorNumber(nr)); return trackInfo.sectorInfo[nr]; }
    
    // Returns the number of entries in the error log
    isize numErrors() { return errorLog.size(); }
    
    // Reads an error message from the error log
    string errorMessage(isize nr) const { return errorLog.at(nr); }
    
    // Reads the error begin index from the error log
    isize firstErroneousBit(isize nr) const { return errorStartIndex.at(nr); }
    
    // Reads the error end index from the error log
    isize lastErroneousBit(isize nr) const { return errorEndIndex.at(nr); }
    
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
