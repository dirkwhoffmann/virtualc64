// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "FSTypes.h"
#include "C64Object.h"
#include "PETName.h"

class FSBlock : C64Object {
    
    // The device this block belongs to
    class FSDevice &device;
    
public:
    
    // The number of this block
    Block nr;
        
    // Outcome of the last integrity check (0 = OK, n = n-th corrupted block)
    u32 corrupted = 0;

    // The actual block data
    u8 data[256];
    
    // Error code (imported from D64 files, 1 = No error)
    u8 errorCode = 1;
    
    
    //
    // Constructing
    //

public:
    
    FSBlock(FSDevice& _device, u32 _nr);
    virtual ~FSBlock() { }
    const char *getDescription() const override { return "FSBlock"; }

    
    //
    // Querying block properties
    //

public:
    
    // Returns the type of this block
    FSBlockType type() const;
    
    // Returns the track / sector link stored in the fist two bytes
    TSLink tsLink() { return TSLink { data[0], data[1] }; }

    
    //
    // Formatting
    //
    
    // Writes the Block Availability Map (BAM)
    void writeBAM(const char *name = "");
    void writeBAM(PETName<16> &name);

    
    //
    // Debugging
    //
    
public:
    
    // Prints some debug information for this block
    void dump() const;
    
    
    //
    // Integrity checking
    //

public:
    
    // Returns the role of a certain byte in this block
    FSUsage itemType(u32 byte) const;

    // Checks the integrity of a certain byte in this block
    ErrorCode check(u32 byte, u8 *expected, bool strict) const;

    // Scans the block data and returns the number of errors
    unsigned check(bool strict) const;

      
    //
    // Importing and exporting
    //
    
public:
    
    // Imports this block from a buffer (bsize must match the volume block size)
    void importBlock(const u8 *src);

    // Exports this block to a buffer (bsize must match the volume block size)
    void exportBlock(u8 *dst);
 
};
    
typedef FSBlock* BlockPtr;


//
// Convenience macros used inside the check() methods
//

typedef FSBlock* BlockPtr;

#define EXPECT_BYTE(exp) { \
if (value != (exp)) { *expected = (exp); return ERROR_FS_EXPECTED_VAL; } }

#define EXPECT_MIN(min) { \
if (value < (min)) { *expected = (min); return ERROR_FS_EXPECTED_MIN; } }

#define EXPECT_MAX(max) { \
if (value > (max)) { *expected = (max); return ERROR_FS_EXPECTED_MAX; } }

#define EXPECT_RANGE(min,max) { \
EXPECT_MIN(min); EXPECT_MAX(max) }

#define EXPECT_TRACK_REF(s) \
EXPECT_RANGE(0, device.layout.numTracks() + 1)

#define EXPECT_SECTOR_REF(t) { \
if (u32 num = device.layout.numSectors(t)) \
EXPECT_RANGE(0,num) else if (strict) EXPECT_MAX(254) }
