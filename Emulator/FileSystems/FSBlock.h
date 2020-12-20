// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _FS_BLOCK_H
#define _FS_BLOCK_H

#include "FSObjects.h"

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
    
    
    //
    // Constructing
    //

public:
    
    FSBlock(FSDevice& _device, u32 _nr);
    virtual ~FSBlock() { }
    const char *getDescription() override { return "FSBlock"; }

    
    //
    // Querying block properties
    //

public:
    
    // Returns the type of this block
    FSBlockType type();
    

    //
    // Formatting
    //
    
    // Writes the Block Availability Map (BAM)
    void writeBAM(const char *name);
    void writeBAM(FSName &name);

    
    //
    // Debugging
    //
    
public:
    
    // Prints some debug information for this block
    void dump();
    
    
    //
    // Integrity checking
    //

public:
    
    // Returns the role of a certain byte in this block
    FSItemType itemType(u32 byte);

    // Checks the integrity of a certain byte in this block
    FSError check(u32 byte, u8 *expected, bool strict);

    // Scans the block data and returns the number of errors
    unsigned check(bool strict);

      
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
if (value != (exp)) { *expected = (exp); return FS_EXPECTED; } }

#define EXPECT_MIN(min) { \
if (value < (min)) { *expected = (min); return FS_EXPECTED_MIN; } }

#define EXPECT_MAX(max) { \
if (value > (max)) { *expected = (max); return FS_EXPECTED_MAX; } }

#define EXPECT_RANGE(min,max) { \
EXPECT_MIN(min); EXPECT_MAX(max) }

#define EXPECT_TRACK_REF(s) \
EXPECT_RANGE(0, device.layout.numTracks() + 1)

#define EXPECT_SECTOR_REF(t) { \
if (u32 num = device.layout.numSectors(t)) EXPECT_RANGE(0,num) }
// if (u32 num = device.layout.numSectors(t)) EXPECT_RANGE(0,num) else if (strict) EXPECT_BYTE(0) }

#endif
