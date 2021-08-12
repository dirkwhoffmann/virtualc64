// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "AnyFile.h"
#include "Constants.h"

class C64;

struct Thumbnail {
    
    // Image size
    u16 width, height;
    
    // Raw texture data
    u32 screen[TEX_HEIGHT * TEX_WIDTH];
    
    // Creation date and time
    time_t timestamp;
    
    // Factory methods
    static Thumbnail *makeWithC64(const C64 &c64, isize dx = 1, isize dy = 1);
    
    // Takes a screenshot from a given Amiga
    void take(const C64 &c64, isize dx = 1, isize dy = 1);
};

struct SnapshotHeader {
    
    // Magic bytes ('V','C','6','4')
    char magic[4];
    
    // Version number (V major.minor.subminor)
    u8 major;
    u8 minor;
    u8 subminor;
    
    // Preview image
    Thumbnail screenshot;
};

class Snapshot : public AnyFile {

public:

    //
    // Class methods
    //

    static bool isCompatiblePath(const string &name);
    static bool isCompatibleStream(std::istream &stream);
     
    
    //
    // Initializing
    //
     
    Snapshot() { };
    Snapshot(isize capacity);
     
    static Snapshot *makeWithC64(class C64 *c64);

    
    //
    // Methods from C64Object
    //

    const char *getDescription() const override { return "Snapshot"; }

    
    //
    // Methods from AnyFile
    //
        
    FileType type() const override { return FILETYPE_V64; }
    
    
    //
    // Accessing
    //
        
    // Checks the snapshot version number
    bool isTooOld() const;
    bool isTooNew() const;
    bool matches() { return !isTooOld() && !isTooNew(); }

    // Returns a pointer to the snapshot header
    SnapshotHeader *getHeader() const { return (SnapshotHeader *)data; }

    // Returns a pointer to the thumbnail image
    const Thumbnail &getThumbnail() const { return getHeader()->screenshot; }

    // Returns pointer to the core data
    u8 *getData() const { return data + sizeof(SnapshotHeader); }
    
    // Queries time and screenshot properties
    /*
    time_t timeStamp() const { return getHeader()->timestamp; }
    u8 *imageData() const { return (u8 *)(getHeader()->screenshot.screen); }
    isize imageWidth() const { return getHeader()->screenshot.width; }
    isize imageHeight() const { return getHeader()->screenshot.height; }
    */
    
    // Records a screenshot
    void takeScreenshot(class C64 *c64);
};
