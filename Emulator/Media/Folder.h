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

class Folder : public AnyCollection {
    
    class FSDevice *fs = nullptr;
            
public:
        
    static bool isCompatible(const string &path);
    static bool isCompatible(std::istream &stream) { return false; }

    
    //
    // Constructing
    //
    
    // static Folder *makeWithFolder(const string &path) throws;

    
    //
    // Initializing
    //
    
    Folder(const string &path) throws { init(path); }

private:
    
    void init(const string &path) throws;
    
        
    //
    // Methods from C64Object
    //
    
public:
    
    const char *getDescription() const override { return "Folder"; }
    
    
    //
    // Methods from AnyFile
    //

    bool isCompatiblePath(const string &path) override { return isCompatible(path); }
    bool isCompatibleStream(std::istream &stream) override { return isCompatible(stream); }
    FileType type() const override { return FILETYPE_FOLDER; }
    
    
    //
    // Methods from AnyCollection
    //

    PETName<16> collectionName() override;
    isize collectionCount() const override;
    PETName<16> itemName(isize nr) const override;
    u64 itemSize(isize nr) const override;
    u8 readByte(isize nr, u64 pos) const override;
    void copyItem(isize nr, u8 *buf, u64 len, u64 offset) const override;
    
    //
    // Accessing
    //
    
    FSDevice *getFS() { return fs; }
};
