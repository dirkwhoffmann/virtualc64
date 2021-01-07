// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "AnyCollection.h"

using std::vector;

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

    static Folder *makeWithFolder(const char *path) throws;
    static Folder *makeWithFolder(const char *path, ErrorCode *err);

        
    //
    // Methods from C64Object
    //
    
    const char *getDescription() override { return "Folder"; }
    
    
    //
    // Methods from AnyFile
    //

    FileType type() override { return FILETYPE_FOLDER; }
    
    
    //
    // Methods from AnyCollection
    //

    PETName<16> collectionName() override;
    u64 collectionCount() override;
    PETName<16> itemName(unsigned nr) override;
    u64 itemSize(unsigned nr) override;
    u8 readByte(unsigned nr, u64 pos) override;
    void copyItem(unsigned nr, u8 *buf, u64 len, u64 offset) override;
    
    //
    // Accessing
    //
    
    FSDevice *getFS() { return fs; }
};
