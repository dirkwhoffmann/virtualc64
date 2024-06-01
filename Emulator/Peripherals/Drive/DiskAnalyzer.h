// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// This FILE is dual-licensed. You are free to choose between:
//
//     - The GNU General Public License v3 (or any later version)
//     - The Mozilla Public License v2
//
// SPDX-License-Identifier: GPL-3.0-or-later OR MPL-2.0
// -----------------------------------------------------------------------------

#pragma once

#include "DiskAnalyzerTypes.h"
#include "DiskTypes.h"
#include "CoreObject.h"
#include "IOUtils.h"

namespace vc64 {

class DiskAnalyzer final : public CoreObject {

    // Lengths of all halftracks
    isize length[85];
    
    // Data of all halftracks (repeated twice, one byte for each bit on disk)
    u8 *data[85];

    // Result of the analysis
    DiskInfo diskInfo = { };
    
    // Error log (one for each halftrack)
    string logbook[85];

    // Error log created by analyzeTrack
    std::vector<string> errorLog[85];

    // Stores the start offset of the erroneous bit sequence
    std::vector<isize> errorStartIndex[85];

    // Stores the end offset of the erroneous bit sequence
    std::vector<isize> errorEndIndex[85];

    // Textual representation of track data
    char text[maxBitsOnTrack + 1] = { };


    //
    // Methods
    //
    
public:

    DiskAnalyzer(const class Disk &disk);
    ~DiskAnalyzer();
   

    //
    // Methods from CoreObject
    //

private:
    
    const char *objectName() const override { return "DiskAnalyzer"; }


    //
    // Analyzing the disk
    //

public:
    
    // Returns the length of a halftrack in bits
    isize lengthOfTrack(Track t) const;
    isize lengthOfHalftrack(Halftrack ht) const;
    
    // Decodes a GCR-encoded nibble or byte
    u8 decodeGcrNibble(Halftrack ht, isize offset);
    u8 decodeGcr(Halftrack ht, isize offset);

private:
    
    // Analyzes the whole disk
    void analyzeDisk();
    
    // Analyzes a certain track or halftrack
    TrackInfo analyzeTrack(Track t);
    TrackInfo analyzeHalftrack(Halftrack ht);

    // Analyzes all sectors of a certain track
    void analyzeSectorBlocks(Halftrack ht, TrackInfo &trackInfo);

    // Analyzes a single sector header block or sector data block
    void analyzeSectorHeaderBlock(Halftrack ht, isize offset, TrackInfo &trackInfo);
    void analyzeSectorDataBlock(Halftrack ht, isize offset, TrackInfo &trackInfo);

    // Writes an error message into the error log
    void log(Halftrack ht, isize begin, isize length, const char *fmt, ...);
    
public:
    
    // Returns the layout of a certain track
    const SectorInfo &sectorLayout(Halftrack ht, Sector nr);
    
    // Returns the logbook for a certain track
    const string &getLogbook(Halftrack ht) { return logbook[ht]; }
    
    // Returns the number of entries in the error log
    isize numErrors(Halftrack ht) { return isize(errorLog[ht].size()); }
    
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
    const char *sectorBytesAsString(Halftrack ht, isize offset, isize length, bool hex);
};

}
