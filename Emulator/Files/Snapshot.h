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

typedef struct {
    
    // Header signature
    char magicBytes[4];
    
    // Version number
    u8 major;
    u8 minor;
    u8 subminor;
    
    // Thumbnail image
    struct {
        u16 width, height;
        u32 screen[TEX_HEIGHT * TEX_WIDTH];
        
    } screenshot;
    
    // Creation date
    time_t timestamp;
}
SnapshotHeader;

class Snapshot : public AnyFile {
     
public:
    
    static bool isCompatibleName(const std::string &name);
    static bool isCompatibleStream(std::istream &stream);
     
    static Snapshot *makeWithC64(class C64 *c64);

    
    //
    // Initializing
    //
        
    Snapshot() { };
    Snapshot(size_t capacity);
        
    
    //
    // Methods from C64Object
    //

    const char *getDescription() override { return "Snapshot"; }

    
    //
    // Methods from AnyFile
    //
    
public:
    
    FileType type() override { return FILETYPE_V64; }
    
    
    //
    // Accessing properties
    //
    
public:
    
    SnapshotHeader *getHeader() { return (SnapshotHeader *)data; }
    u8 *getData() { return data + sizeof(SnapshotHeader); }
    
    bool isTooOld();
    bool isTooNew();

    time_t getTimestamp() { return getHeader()->timestamp; }
    unsigned char *getImageData() { return (unsigned char *)(getHeader()->screenshot.screen); }
    unsigned getImageWidth() { return getHeader()->screenshot.width; }
    unsigned getImageHeight() { return getHeader()->screenshot.height; }
    
    void takeScreenshot(class C64 *c64);
};
