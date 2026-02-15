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

#include "AnyFile.h"
#include "Constants.h"
#include "Texture.h"

namespace vc64 {

class C64;

struct Thumbnail {
    
    // Image size
    i32 width, height;
    
    // Raw texture data
    u32 screen[Texture::height * Texture::width];
    
    // Creation date and time
    time_t timestamp;
    
    // Factory methods
    static Thumbnail *makeWithC64(const C64 &c64, isize dx = 1, isize dy = 1);
    
    // Takes a screenshot from the current texture
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

    // Size of this snapshot when uncompressed
    i32 rawSize;
    
    // Applied compression method
    u8 compressor;

    // Preview image
    Thumbnail screenshot;
};

class Snapshot : public AnyFile {

public:

    //
    // Class methods
    //

    static bool isCompatible(const fs::path &path);
    static bool isCompatible(const u8 *buf, isize len);
    static bool isCompatible(const Buffer<u8> &buffer);


    //
    // Initializing
    //

    Snapshot(const Snapshot &other) { init(other.data.ptr, other.data.size); }
    Snapshot(const fs::path &path) { init(path); }
    Snapshot(const u8 *buf, isize len) { init(buf, len); }
    Snapshot(isize capacity);
    Snapshot(C64 &c64);
    Snapshot(C64 &c64, Compressor compressor);


    //
    // Methods from CoreObject
    //

    const char *objectName() const override { return "Snapshot"; }


    //
    // Methods from AnyFile
    //

    FileType type() const override { return FileType::SNAPSHOT; }
    bool isCompatiblePath(const fs::path &path) const override { return isCompatible(path); }
    bool isCompatibleBuffer(const u8 *buf, isize len) const override { return isCompatible(buf, len); }
    void finalizeRead() override;


    //
    // Accessing
    //

    std::pair <isize,isize> previewImageSize() const override;
    const u32 *previewImageData() const override;
    time_t timestamp() const override;

    // Checks the snapshot version number
    bool isTooOld() const;
    bool isTooNew() const;
    bool isBeta() const;
    bool matches() { return !isTooOld() && !isTooNew(); }

    // Returns a pointer to the snapshot header
    SnapshotHeader *getHeader() const { return (SnapshotHeader *)data.ptr; }

    // Returns a pointer to the thumbnail image
    const Thumbnail &getThumbnail() const { return getHeader()->screenshot; }

    // Returns pointer to the core data
    u8 *getSnapshotData() const { return data.ptr + sizeof(SnapshotHeader); }

    // Records a screenshot
    void takeScreenshot(C64 &c64);


    //
    // Compressing
    //

    // Returns the compression method
    Compressor compressor() const override { return Compressor(getHeader()->compressor); }

    // Compresses or uncompresses the snapshot
    void compress(Compressor method) override;
    void uncompress() override;
};

}
