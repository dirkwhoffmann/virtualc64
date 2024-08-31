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

#include "AnyCollection.h"

namespace vc64 {

class PRGFile : public AnyCollection {

public:
    
    //
    // Class methods
    //
    
    static bool isCompatible(const fs::path &path);
    static bool isCompatible(const u8 *buf, isize len);
    static bool isCompatible(const Buffer<u8> &buffer);


    //
    // Initializing
    //
    
    PRGFile() : AnyCollection() { }
    PRGFile(isize capacity) : AnyCollection(capacity) { }
    PRGFile(const fs::path &path) throws { init(path); }
    PRGFile(const u8 *buf, isize len) throws { init(buf, len); }
    PRGFile(const class FileSystem &fs) throws { init(fs); }

private:
    
    using AnyFile::init;
    void init(const FileSystem &fs) throws;
    
    
    //
    // Methods from CoreObject
    //
    
    const char *objectName() const override { return "PRGFile"; }
    
    
    //
    // Methods from AnyFile
    //

    bool isCompatiblePath(const fs::path &path) override { return isCompatible(path); }
    bool isCompatibleBuffer(const u8 *buf, isize len) override { return isCompatible(buf, len); }
    FileType type() const override { return FILETYPE_PRG; }

    
    //
    // Methods from AnyCollection
    //

    PETName<16> collectionName() override;
    isize collectionCount() const override;
    PETName<16> itemName(isize nr) const override;
    isize itemSize(isize nr) const override;
    u8 readByte(isize nr, isize pos) const override;
};

}
