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

class Folder : public AnyCollection {
    
    class FileSystem *fs = nullptr;

public:

    static bool isCompatible(const fs::path &path);
    static bool isCompatible(std::istream &stream) { return false; }


    //
    // Initializing
    //
    
    Folder(const fs::path &path) throws { init(path); }

private:
    
    void init(const fs::path &path) throws;


    //
    // Methods from CoreObject
    //
    
public:
    
    const char *objectName() const override { return "Folder"; }
    
    
    //
    // Methods from AnyFile
    //

    bool isCompatiblePath(const fs::path &path) override { return isCompatible(path); }
    bool isCompatibleStream(std::istream &stream) override { return isCompatible(stream); }
    FileType type() const override { return FILETYPE_FOLDER; }
    
    
    //
    // Methods from AnyCollection
    //

    PETName<16> collectionName() override;
    isize collectionCount() const override;
    PETName<16> itemName(isize nr) const override;
    isize itemSize(isize nr) const override;
    u8 readByte(isize nr, isize pos) const override;
    void copyItem(isize nr, u8 *buf, isize len, isize offset) const override;
    
    //
    // Accessing
    //
    
    FileSystem *getFS() { return fs; }
};

}
