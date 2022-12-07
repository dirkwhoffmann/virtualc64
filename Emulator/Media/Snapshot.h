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

namespace vc64 {
class C64;
}

using namespace vc64;

struct Thumbnail {
    
    // Image size
    isize width, height;
    
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
	u8 beta;
	
    // Preview image
    Thumbnail screenshot;
};

class Snapshot : public AnyFile {

public:

    //
    // Class methods
    //

    static bool isCompatible(const string &name);
    static bool isCompatible(std::istream &stream);
     
    
    //
    // Initializing
    //
     
    Snapshot(const string &path) throws { init(path); }
    Snapshot(const u8 *buf, isize len) throws { init(buf, len); }
    Snapshot(isize capacity);
    Snapshot(C64 &c64);

    
    //
    // Methods from C64Object
    //

    const char *getDescription() const override { return "Snapshot"; }

    
    //
    // Methods from AnyFile
    //
      
	FileType type() const override { return FILETYPE_SNAPSHOT; }
    bool isCompatiblePath(const string &path) override { return isCompatible(path); }
    bool isCompatibleStream(std::istream &stream) override { return isCompatible(stream); }
	void finalizeRead() throws override;
	
    
    //
    // Accessing
    //
        
    // Checks the snapshot version number
    bool isTooOld() const;
    bool isTooNew() const;
	bool isBeta() const;
    bool matches() { return !isTooOld() && !isTooNew(); }

    // Returns a pointer to the snapshot header
    SnapshotHeader *getHeader() const { return (SnapshotHeader *)data; }

    // Returns a pointer to the thumbnail image
    const Thumbnail &getThumbnail() const { return getHeader()->screenshot; }

    // Returns pointer to the core data
    u8 *getData() const { return data + sizeof(SnapshotHeader); }
        
    // Records a screenshot
    void takeScreenshot(C64 &c64);
};
