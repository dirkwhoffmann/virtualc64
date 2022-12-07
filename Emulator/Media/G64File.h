// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "AnyFile.h"

namespace vc64 {

class G64File : public AnyFile {

public:

    //
    // Class methods
    //

    static bool isCompatible(const string &name);
    static bool isCompatible(std::istream &stream);

    
    //
    // Initializing
    //
    
    G64File() { };
    G64File(isize capacity);
    G64File(const string &path) throws { init(path); }
    G64File(const u8 *buf, isize len) throws { init(buf, len); }
    G64File(class Disk &disk) throws { init(disk); }

private:
    
    using AnyFile::init;
    void init(class Disk &disk) throws;
    
    
    //
    // Methods from C64Object
    //
    
public:
    
    const char *getDescription() const override { return "G64File"; }

    
    //
    // Methods from AnyFile
    //
    
    bool isCompatiblePath(const string &path) override { return isCompatible(path); }
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
