// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "AnyFile.h"

class TAPFile : public AnyFile {
    
private:
    
    // Header signature
    static const u8 magicBytes[];
            
public:
    
    //
    // Class methods
    //
    
    static bool isCompatibleBuffer(const u8 *buffer, size_t length);
    static bool isCompatibleFile(const char *filename);
    
    
    //
    // Initializing
    //
    
    TAPFile() : AnyFile() { }
    const char *getDescription() override { return "TAPFile"; }

    
    //
    // Methods from AnyFile
    //
    
    void dealloc() override;
    FileType type() override { return FILETYPE_TAP; }
    const char *getName() override;
    bool matchingBuffer(const u8 *buf, size_t len) override;
    bool matchingFile(const char *path) override;
    
    // bool matchingFile(const char *filename) override { return isTAPFile(filename); }
    void readFromBuffer(const u8 *buffer, size_t length) override;
    
    //
    // Retrieving tape information
    //
    
    // Returns the TAP version (0 = original layout, 1 = updated layout)
    TAPVersion version() { return (TAPVersion)data[0x000C]; }
    
    // Returns the beginning of the data area
    u8 *getData() { return data + 0x14; }
    
    // Returns the size of the data area in bytes
    size_t getDataSize() { return size - 0x14; }
};
