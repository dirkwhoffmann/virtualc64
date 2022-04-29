// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "AnyCollection.h"

class T64File : public AnyCollection {
     
public:

    static bool isCompatible(const string &name);
    static bool isCompatible(std::istream &stream);
    
    // static T64File *makeWithFileSystem(class FSDevice &fs);

    
    //
    // Initializing
    //
    
    T64File() : AnyCollection() { }
    T64File(isize capacity) : AnyCollection(capacity) { }
    T64File(const string &path) throws { init(path); }
    T64File(const u8 *buf, isize len) throws { init(buf, len); }
    T64File(class FileSystem &fs) throws { init(fs); }
    
private:
    
    using AnyFile::init;
    void init(FileSystem &fs) throws;

    
    //
    // Methods from C64Object
    //
    
    const char *getDescription() const override { return "T64File"; }

    
    //
    // Methods from AnyFile
    //
    
    FileType type() const override { return FILETYPE_T64; }
    PETName<16> getName() const override;
    bool isCompatiblePath(const string &path) override { return isCompatible(path); }
    bool isCompatibleStream(std::istream &stream) override { return isCompatible(stream); }
    void finalizeRead() override;

    //
    // Methods from AnyCollection
    //

    PETName<16> collectionName() override;
    isize collectionCount() const override;
    PETName<16> itemName(isize nr) const override;
    u64 itemSize(isize nr) const override;
    u8 readByte(isize nr, u64 pos) const override;
    
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
