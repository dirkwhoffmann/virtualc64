// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _SNAPSHOT_H
#define _SNAPSHOT_H

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
    
    // Header signature
    static const u8 magicBytes[];
    
    
    //
    // Class methods
    //
    
public:
    
    // Checks whether a buffer contains a snapshot
    static bool isSnapshot(const u8 *buffer, size_t length);
    
    // Checks whether a buffer contains a snapshot of a specific version
    static bool isSnapshot(const u8 *buffer, size_t length,
                           u8 major, u8 minor, u8 subminor);
    
    // Checks whether a buffer contains a snapshot with a supported version number
    static bool isSupportedSnapshot(const u8 *buffer, size_t length);
    
    // Checks whether a buffer contains a snapshot with an outdated version number
    static bool isUnsupportedSnapshot(const u8 *buffer, size_t length);
    
    // Checks whether 'path' points to a snapshot
    static bool isSnapshotFile(const char *path);
    
    // Checks whether 'path' points to a snapshot of a specific version
    static bool isSnapshotFile(const char *path, u8 major, u8 minor, u8 subminor);
    
    // Checks whether 'path' points to a snapshot with a supported version number
    static bool isSupportedSnapshotFile(const char *path);
    
    // Checks whether 'path' points to a snapshot with an outdated version number
    static bool isUnsupportedSnapshotFile(const char *path);
    
    
    //
    // Factory methods
    //
    
public:
    
    static Snapshot *makeWithFile(const char *filename);
    static Snapshot *makeWithBuffer(const u8 *buffer, size_t size);
    static Snapshot *makeWithC64(class C64 *c64);

    
    //
    // Initializing
    //
        
    Snapshot() { };
    Snapshot(size_t capacity);
    const char *getDescription() override { return "Snapshot"; }

    void takeScreenshot(class C64 *c64);

private:
    
    // Allocates memory for storing the emulator state
    bool setCapacity(size_t size);
        
    
    //
    // Methods from AnyFile
    //
    
public:
    
    FileType type() override { return FileType_V64; }
    bool matchingBuffer(const u8 *buf, size_t len) override;
    bool matchingFile(const char *path) override;
    
    
    //
    // Accessing properties
    //
    
public:
    
    SnapshotHeader *getHeader() { return (SnapshotHeader *)data; }
    u8 *getData() { return data + sizeof(SnapshotHeader); }
    
    time_t getTimestamp() { return getHeader()->timestamp; }
    unsigned char *getImageData() { return (unsigned char *)(getHeader()->screenshot.screen); }
    unsigned getImageWidth() { return getHeader()->screenshot.width; }
    unsigned getImageHeight() { return getHeader()->screenshot.height; }
};

#endif

