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


namespace vc64 {

class C64;

/// Preview image of a snapshot
struct Thumbnail {
    
    /// Image size
    i32 width, height;
    
    /// Raw texture data
    u32 screen[Tex::height * Tex::width];
    
    /// Creation date and time
    time_t timestamp;
    
    /// Factory methods
    static Thumbnail *makeWithC64(const C64 &c64, isize dx = 1, isize dy = 1);
    
    /// Takes a screenshot from the current texture
    void take(const C64 &c64, isize dx = 1, isize dy = 1);
};

/// Header of a snapshot
struct SnapshotHeader {
    
    /// Magic bytes ('V','C','6','4')
    char magic[4];
    
    // Version number (V major.minor.subminor)
    u8 major;                            ///< Major version number
    u8 minor;                            ///< Minor version number
    u8 subminor;                         ///< Subminor version number
    u8 beta;                             ///< Beta release number

    /// Size of this snapshot when uncompressed
    i32 rawSize;
    
    /// Applied compression method
    u8 compressor;

    /// Preview image
    Thumbnail screenshot;
};

/// A snapshot of the emulator state
///
/// A snapshot contains the complete internal state of the emulator together with a
/// preview image. Snapshots are created with C64API::takeSnapshot() and can be
/// restored with C64API::loadSnapshot().
class Snapshot : public AnyFile {

public:

    //
    // Class methods
    //

    /// Checks whether a file, a buffer or a memory region contains a snapshot
    static bool isCompatible(const fs::path &path);
    static bool isCompatible(const u8 *buf, isize len);
    static bool isCompatible(const Buffer<u8> &buffer);


    //
    // Initializing
    //

    /// Creates a copy of a snapshot
    Snapshot(const Snapshot &other) { init(other.data.ptr, other.data.size); }
    /// Reads a snapshot from a file
    Snapshot(const fs::path &path) { init(path); }
    /// Reads a snapshot from a memory buffer
    Snapshot(const u8 *buf, isize len) { init(buf, len); }
    /// Creates an empty snapshot with the given capacity
    Snapshot(isize capacity);
    /// Takes a snapshot of the emulator state
    Snapshot(C64 &c64);
    /// Takes a snapshot of the emulator state and compresses it
    Snapshot(C64 &c64, utl::Compressor compressor);


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

    /// Returns the size of the preview image (width, height)
    std::pair <isize,isize> previewImageSize() const;
    /// Returns the pixel data of the preview image
    const u32 *previewImageData() const;
    /// Returns the time the snapshot was taken
    time_t timestamp() const;

    // Checks the snapshot version number
    /// Returns true if the snapshot was created by an older, incompatible version
    bool isTooOld() const;
    /// Returns true if the snapshot was created by a newer version
    bool isTooNew() const;
    /// Returns true if the snapshot was created by a beta version
    bool isBeta() const;
    /// Returns true if the snapshot can be restored by this version
    bool matches() { return !isTooOld() && !isTooNew(); }

    // Returns a pointer to the snapshot header
    /// Returns a pointer to the snapshot header
    SnapshotHeader *getHeader() const { return (SnapshotHeader *)data.ptr; }

    // Returns a pointer to the thumbnail image
    /// Returns the preview image
    const Thumbnail &getThumbnail() const { return getHeader()->screenshot; }

    // Returns pointer to the core data
    /// Returns a pointer to the core data
    u8 *getSnapshotData() const { return data.ptr + sizeof(SnapshotHeader); }

    /// Records a screenshot
    void takeScreenshot(C64 &c64);


    //
    // Compressing
    //

    // Returns the compression method
    /// Returns the compression method
    utl::Compressor compressor() const { return utl::Compressor(getHeader()->compressor); }

    // Convenience wrapper
    /// Returns true if the snapshot is compressed
    bool isCompressed() const { return compressor() != utl::Compressor::NONE; }
    
    // Compresses or uncompresses the snapshot
    /// Compresses the snapshot with the given method
    void compress(utl::Compressor method);
    /// Uncompresses the snapshot
    void uncompress();
};

}
