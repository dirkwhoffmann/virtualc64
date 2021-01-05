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

    //
    // Class methods
    //

    static bool isCompatibleName(const std::string &name);
    static bool isCompatibleStream(std::istream &stream);
     
    static Snapshot *makeWithC64(class C64 *c64);

    
    //
    // Initializing
    //
        
    Snapshot() { };
    Snapshot(usize capacity);
        
    
    //
    // Methods from C64Object
    //

    const char *getDescription() override { return "Snapshot"; }

    
    //
    // Methods from AnyFile
    //
        
    FileType type() override { return FILETYPE_V64; }
    
    
    //
    // Accessing
    //
        
    // Returns a pointer to the snapshot header
    SnapshotHeader *header() { return (SnapshotHeader *)data; }

    // Checks the snapshot version number
    bool isTooOld();
    bool isTooNew();
    bool matches() { return !isTooOld() && !isTooNew(); }
    
    u8 *getData() { return data + sizeof(SnapshotHeader); }
    
    // Queries time and screenshot properties
    time_t timeStamp() { return header()->timestamp; }
    u8 *imageData() { return (u8 *)(header()->screenshot.screen); }
    usize imageWidth() { return header()->screenshot.width; }
    usize imageHeight() { return header()->screenshot.height; }
    
    // Records a screenshot
    void takeScreenshot(class C64 *c64);
};
