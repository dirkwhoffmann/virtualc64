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

#include "FSTypes.h"
#include "PETName.h"
#include <filesystem>

namespace vc64 {

namespace fs = ::std::filesystem;

struct FSDirEntry
{
    u8 nextDirTrack;      // $00
    u8 nextDirSector;     // $01
    u8 fileType;          // $02
    u8 firstDataTrack;    // $03
    u8 firstDataSector;   // $04
    u8 fileName[16];      // $05 - $14
    u8 sideSecBlkTrack;   // $15          REL files only
    u8 sideSrcBlkSector;  // $16          REL files only
    u8 recLength;         // $17          REL files only
    u8 unused[6];         // $18 - $1D    Geos only
    u8 fileSizeLo;        // $1E
    u8 fileSizeHi;        // $1F

    // Initializes this entry
    void init(PETName<16> name, TSLink ref, isize numBlocks);
    void init(const string &name, TSLink ref, isize numBlocks);

    // Checks whether this entry if empty
    bool isEmpty() const;
    
    // Returns the name of this file
    PETName<16> getName() const { return PETName<16>(fileName); }

    // Return the name of this file with certain symbols escaped
    fs::path getFileSystemRepresentation() const;

    // Returns the file type of this file
    FSFileType getFileType() const;
    
    // Returns the file type as a string
    const string typeString() const;

    // Returns true if this file does not appear in a regular directory listing
    bool isHidden() const;
    
    // Returns the link to the first data block
    TSLink firstBlock() const { return TSLink{firstDataTrack,firstDataSector}; }
};

}
