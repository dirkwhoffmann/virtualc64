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

// D64 files come in six different sizes
#define D64_683_SECTORS 174848
#define D64_683_SECTORS_ECC 175531
#define D64_768_SECTORS 196608
#define D64_768_SECTORS_ECC 197376
#define D64_802_SECTORS 205312
#define D64_802_SECTORS_ECC 206114

class D64File : public AnyFile {
        
    // Error information stored in the D64 archive
    u8 errors[802];
    
public:
    
    static bool isCompatibleName(const std::string &name);
    static bool isCompatibleStream(std::istream &stream);
  
    static D64File *makeWithDisk(class Disk &disk) throws;
    static D64File *makeWithFileSystem(class FSDevice &volume) throws;


    //
    // Initializing
    //
    
    D64File();
    D64File(unsigned tracks, bool ecc);
    
    
    //
    // Methods from C64Object
    //
    
    const char *getDescription() override { return "D64File"; }

        
    //
    // Methods from AnyFile
    //
    
    FileType type() override { return FILETYPE_D64; }
    PETName<16> getName() override;
    usize readFromStream(std::istream &stream) override;

        
    //
    // Accessing tracks and sectors
    //
    
public:
    
    // Returns the number of halftracks or tracks stored in this file
    Track numHalftracks();
    Track numTracks() { return numHalftracks() / 2; }

    // Returns the error code for the specified sector (01 = no error)
    u8 getErrorCode(Block b);
    
private:
    
    // Translates a track and sector number into an offset (-1 if invalid)
    int offset(Track track, Sector sector);
    
    
    //
    // Debugging
    //
    
public:
    
    // Dumps the contents of a sector
    void dump(Track track, Sector sector);
};
