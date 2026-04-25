// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include "FileSystems/CBM/FSService.h"

namespace retro::vault::cbm {

class FSAllocator;

class FSDoctor final : public FSService {

    FSAllocator &allocator;

public:

    // Result of the latest examination
    FSDiagnosis diagnosis;

    explicit FSDoctor(FileSystem& fs, FSAllocator &a);


    //
    // Dumping debug information
    //

public:

    void dump(BlockNr nr, std::ostream &os);


    //
    // Checking the file system integrity
    //

public:

    // Scans all blocks. Returns the number of errors. Stores details in 'diagnosis'
    isize xray(bool strict);
    isize xray(bool strict, std::ostream &os, bool verbose);

    // Scans a single block and returns the number of errors
    isize xray(BlockNr ref, bool strict) const;
    isize xray(BlockNr ref, bool strict, std::ostream &os) const;

    // Checks the integrity of a certain byte or long word in this block
    FSBlockError xray8(BlockNr ref, isize pos, bool strict,
                       optional<u8> &expected) const;
    FSBlockError xray8(BlockNr ref, isize pos, bool strict,
                       optional<u8> &expected, const std::vector<BlockNr> &dirBlocks) const;

    // Checks the allocation table. Returns the number of errors. Stores details in 'diagnosis'
    isize xrayBitmap(bool strict = false);
    isize xrayBitmap(std::ostream &os, bool strict = false);

    // Rectifies all blocks
    void rectify(bool strict);

    // Rectifies a single block
    void rectify(BlockNr ref, bool strict);

    // Rectifies the allocation table
    void rectifyBitmap(bool strict = false);


    //
    // GUI helper functions
    //

public:

    // Returns a portion of the block as an ASCII dump
    string ascii(BlockNr nr, isize offset, isize len) const noexcept;

    // Returns a block summary for creating the block usage image
    void createUsageMap(u8 *buffer, isize len) const;

    // Returns a usage summary for creating the block allocation image
    void createAllocationMap(u8 *buffer, isize len) const;

    // Returns a block summary for creating the diagnose image
    void createHealthMap(u8 *buffer, isize len) const;

    // Searches the block list for a block of a specific type
    isize nextBlockOfType(FSBlockType type, BlockNr after) const;
};

}
