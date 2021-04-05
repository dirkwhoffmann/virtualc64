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
    
    FSDevice *fs = nullptr;
            
public:

    //
    // Class methods
    //
        
    // Returns true iff the specified path points to a folder
    static bool isFolder(const char *path);
    
    
    //
    // Constructing
    //
    
    static Folder *makeWithFolder(const std::string &path) throws;
    static Folder *makeWithFolder(const std::string &path, ErrorCode *err);

        
    //
    // Methods from C64Object
    //
    
    const char *getDescription() const override { return "Folder"; }
    
    
    //
    // Methods from AnyFile
    //

    FileType type() const override { return FILETYPE_FOLDER; }
    
    
    //
    // Methods from AnyCollection
    //

    PETName<16> collectionName() override;
    u64 collectionCount() const override;
    PETName<16> itemName(unsigned nr) const override;
    u64 itemSize(unsigned nr) const override;
    u8 readByte(unsigned nr, u64 pos) const override;
    void copyItem(unsigned nr, u8 *buf, u64 len, u64 offset) const override;
    
    //
    // Accessing
    //
    
    FSDevice *getFS() { return fs; }
};
