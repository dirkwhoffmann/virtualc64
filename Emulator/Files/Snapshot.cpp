// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "C64.h"

const u8 Snapshot::magicBytes[] = { 'V', 'C', '6', '4' };

bool
Snapshot::isSnapshot(const u8 *buffer, size_t length)
{
    assert(buffer != NULL);
    
    if (length < 0x15) return false;
    return matchingBufferHeader(buffer, magicBytes, sizeof(magicBytes));
}

bool
Snapshot::isSnapshot(const u8 *buffer, size_t length,
                     u8 major, u8 minor, u8 subminor)
{
    if (!isSnapshot(buffer, length)) return false;
    return buffer[4] == major && buffer[5] == minor && buffer[6] == subminor;
}

bool
Snapshot::isSupportedSnapshot(const u8 *buffer, size_t length)
{
    return isSnapshot(buffer, length, V_MAJOR, V_MINOR, V_SUBMINOR);
}

bool
Snapshot::isUnsupportedSnapshot(const u8 *buffer, size_t length)
{
    return isSnapshot(buffer, length) && !isSupportedSnapshot(buffer, length);
}

bool
Snapshot::isSnapshotFile(const char *path)
{
    assert(path != NULL);
    
    if (!matchingFileHeader(path, magicBytes, sizeof(magicBytes)))
        return false;
    
    return true;
}

bool
Snapshot::isSnapshotFile(const char *path, u8 major, u8 minor, u8 subminor)
{
    u8 signature[] = { 'V', 'C', '6', '4', major, minor, subminor };
    
    assert(path != NULL);
    
    if (!matchingFileHeader(path, signature, sizeof(signature)))
        return false;
    
    return true;
}

bool
Snapshot::isSupportedSnapshotFile(const char *path)
{
    return isSnapshotFile(path, V_MAJOR, V_MINOR, V_SUBMINOR);
}

bool
Snapshot::isUnsupportedSnapshotFile(const char *path)
{
    return isSnapshotFile(path) && !isSupportedSnapshotFile(path);
}

Snapshot::Snapshot()
{
    setDescription("Snapshot");
}

Snapshot::Snapshot(size_t capacity)
{
    size = capacity + sizeof(SnapshotHeader);
    data = new u8[size];
    
    SnapshotHeader *header = (SnapshotHeader *)data;
    header->magicBytes[0] = magicBytes[0];
    header->magicBytes[1] = magicBytes[1];
    header->magicBytes[2] = magicBytes[2];
    header->magicBytes[3] = magicBytes[3];
    header->major = V_MAJOR;
    header->minor = V_MINOR;
    header->subminor = V_SUBMINOR;
    header->timestamp = time(NULL);
}

Snapshot *
Snapshot::makeWithBuffer(const u8 *buffer, size_t length)
{
    Snapshot *snapshot;
    
    snapshot = new Snapshot();
    if (!snapshot->readFromBuffer(buffer, length)) {
        delete snapshot;
        return NULL;
    }
    return snapshot;
}

Snapshot *
Snapshot::makeWithFile(const char *filename)
{
    Snapshot *snapshot;
    
    snapshot = new Snapshot();
    if (!snapshot->readFromFile(filename)) {
        delete snapshot;
        return NULL;
    }
    return snapshot;
}

Snapshot *
Snapshot::makeWithC64(C64 *c64)
{
    Snapshot *snapshot;
    
    snapshot = new Snapshot(c64->size());
    
    snapshot->takeScreenshot(c64);
    c64->save(snapshot->getData());
    
    return snapshot;
}

bool 
Snapshot::hasSameType(const char *filename)
{
    return Snapshot::isSnapshotFile(filename, V_MAJOR, V_MINOR, V_SUBMINOR);
}

void
Snapshot::takeScreenshot(C64 *c64)
{
    SnapshotHeader *header = (SnapshotHeader *)data;
    unsigned x_start, y_start;
       
    if (c64->vic.isPAL()) {
        x_start = PAL_LEFT_BORDER_WIDTH - 36;
        y_start = PAL_UPPER_BORDER_HEIGHT - 34;
        header->screenshot.width = 36 + PAL_CANVAS_WIDTH + 36;
        header->screenshot.height = 34 + PAL_CANVAS_HEIGHT + 34;
    } else {
        x_start = NTSC_LEFT_BORDER_WIDTH - 42;
        y_start = NTSC_UPPER_BORDER_HEIGHT - 9;
        header->screenshot.width = 36 + PAL_CANVAS_WIDTH + 36;
        header->screenshot.height = 9 + PAL_CANVAS_HEIGHT + 9;
    }
    
    u32 *source = (u32 *)c64->vic.screenBuffer();
    u32 *target = header->screenshot.screen;
    source += x_start + y_start * NTSC_WIDTH;
    for (unsigned i = 0; i < header->screenshot.height; i++) {
        memcpy(target, source, header->screenshot.width * 4);
        target += header->screenshot.width;
        source += NTSC_WIDTH;
    }
}
