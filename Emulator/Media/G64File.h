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

class G64File : public AnyFile {

public:

    //
    // Class methods
    //

    static bool isCompatible(const fs::path &path);
    static bool isCompatible(std::istream &stream);

    
    //
    // Initializing
    //
    
    G64File() { };
    G64File(isize capacity);
    G64File(const fs::path &path) throws { init(path); }
    G64File(const u8 *buf, isize len) throws { init(buf, len); }
    G64File(class Disk &disk) throws { init(disk); }

private:
    
    using AnyFile::init;
    void init(class Disk &disk) throws;
    
    
    //
    // Methods from CoreObject
    //
    
public:
    
    const char *objectName() const override { return "G64File"; }

    
    //
    // Methods from AnyFile
    //
    
    bool isCompatiblePath(const fs::path &path) override { return isCompatible(path); }
    bool isCompatibleStream(std::istream &stream) override { return isCompatible(stream); }
    FileType type() const override { return FILETYPE_G64; }


    //
    // Reading data from a track
    //

public:
    
    // Returns the size of a certain haltrack in bytes
    isize getSizeOfHalftrack(Halftrack ht) const;

    // Copies a certain track into a buffer
    void copyHalftrack(Halftrack ht, u8 *buf) const;
    
private:
    
    isize getStartOfHalftrack(Halftrack ht) const;
};

}
