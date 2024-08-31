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

#include "AnyFile.h"
#include "DriveTypes.h"

namespace vc64 {

class D64File : public AnyFile {

public:
    
    // D64 files come in six different sizes
    static const isize D64_683_SECTORS     = 174848;
    static const isize D64_683_SECTORS_ECC = 175531;
    static const isize D64_768_SECTORS     = 196608;
    static const isize D64_768_SECTORS_ECC = 197376;
    static const isize D64_802_SECTORS     = 205312;
    static const isize D64_802_SECTORS_ECC = 206114;
    
    // Error information stored in the D64 archive
    u8 errors[802];
    
    static bool isCompatible(const fs::path &name);
    static bool isCompatible(const u8 *buf, isize len);
    static bool isCompatible(const Buffer<u8> &buffer);


    //
    // Initializing
    //
    
    D64File();
    D64File(isize tracks, bool ecc);
    D64File(const fs::path &path) throws : D64File() { init(path); }
    D64File(const u8 *buf, isize len) throws : D64File() { init(buf, len); }
    D64File(const class FileSystem &fs) throws : D64File() { init(fs); }

private:
    
    using AnyFile::init;
    void init(isize tracks, bool ecc);
    void init(const FileSystem &fs) throws;
    
    
    //
    // Methods from CoreObject
    //
    
public:
    
    const char *objectName() const override { return "D64File"; }


    //
    // Methods from AnyFile
    //
    
    bool isCompatiblePath(const fs::path &path) override { return isCompatible(path); }
    bool isCompatibleBuffer(const u8 *buf, isize len) override { return isCompatible(buf, len); }
    FileType type() const override { return FILETYPE_D64; }
    PETName<16> getName() const override;
    void finalizeRead() throws override;


    //
    // Querying properties
    //
    
public:
    
    // Returns the number of halftracks or tracks stored in this file
    Track numHalftracks() const;
    Track numTracks() const { return numHalftracks() / 2; }

    // Returns the error code for the specified sector (01 = no error)
    u8 getErrorCode(Block b) const;
    
private:
    
    // Translates a track and sector number into an offset (-1 if invalid)
    isize offset(Track track, Sector sector) const;
    
    
    //
    // Debugging
    //
    
public:
    
    // Dumps the contents of a sector
    void dump(Track track, Sector sector) const;
};

}
