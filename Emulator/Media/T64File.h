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

class T64File : public AnyCollection {

public:

    static bool isCompatible(const fs::path &path);
    static bool isCompatible(std::istream &stream);

    
    //
    // Initializing
    //
    
    T64File() : AnyCollection() { }
    T64File(isize capacity) : AnyCollection(capacity) { }
    T64File(const fs::path &path) throws { init(path); }
    T64File(const u8 *buf, isize len) throws { init(buf, len); }
    T64File(class FileSystem &fs) throws { init(fs); }
    
private:
    
    using AnyFile::init;
    void init(FileSystem &fs) throws;

    
    //
    // Methods from CoreObject
    //
    
    const char *objectName() const override { return "T64File"; }

    
    //
    // Methods from AnyFile
    //
    
    FileType type() const override { return FILETYPE_T64; }
    PETName<16> getName() const override;
    bool isCompatiblePath(const fs::path &path) override { return isCompatible(path); }
    bool isCompatibleStream(std::istream &stream) override { return isCompatible(stream); }
    void finalizeRead() override;

    //
    // Methods from AnyCollection
    //

    PETName<16> collectionName() override;
    isize collectionCount() const override;
    PETName<16> itemName(isize nr) const override;
    isize itemSize(isize nr) const override;
    u8 readByte(isize nr, isize pos) const override;
    
private:

    u16 memStart(isize nr) const;
    u16 memEnd(isize nr) const;
    
    
    //
    // Scanning
    //
    
public:
    
    // Checks if the header contains information at the specified location
    bool directoryItemIsPresent(isize n);
};

}
