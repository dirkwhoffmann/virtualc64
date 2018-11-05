/*!
 * @file        Snapshot.cpp
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   Dirk W. Hoffmann, all rights reserved.
 */
/* This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "C64.h"

const uint8_t Snapshot::magicBytes[] = { 'V', 'C', '6', '4', 0x00 };

bool
Snapshot::isSnapshot(const uint8_t *buffer, size_t length)
{
    assert(buffer != NULL);
    
    if (length < 0x15) return false;
    return checkBufferHeader(buffer, length, magicBytes);
}

bool
Snapshot::isSnapshot(const uint8_t *buffer, size_t length,
                     uint8_t major, uint8_t minor, uint8_t subminor)
{
    if (!isSnapshot(buffer, length)) return false;
    return buffer[4] == major && buffer[5] == minor && buffer[6] == subminor;
}

bool
Snapshot::isSupportedSnapshot(const uint8_t *buffer, size_t length)
{
    return isSnapshot(buffer, length, V_MAJOR, V_MINOR, V_SUBMINOR);
}

bool
Snapshot::isUnsupportedSnapshot(const uint8_t *buffer, size_t length)
{
    return isSnapshot(buffer, length) && !isSupportedSnapshot(buffer, length);
}

bool
Snapshot::isSnapshotFile(const char *path)
{
    assert(path != NULL);
    
    if (!checkFileHeader(path, magicBytes))
        return false;
    
    return true;
}

bool
Snapshot::isSnapshotFile(const char *path, uint8_t major, uint8_t minor, uint8_t subminor)
{
    uint8_t magicBytesWithVersion[] = { 'V', 'C', '6', '4', major, minor, subminor, 0x00 };
    
    assert(path != NULL);
    
    if (!checkFileHeader(path, magicBytesWithVersion))
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

bool
Snapshot::setCapacity(size_t newCapacity)
{
    size_t newSize = newCapacity + sizeof(SnapshotHeader);
    
    if (data != NULL && size == newSize)
        return true;
    
    dealloc();
    if ((data = new uint8_t[newSize]) == NULL)
        return false;
    size = newSize;
    
    SnapshotHeader *header = (SnapshotHeader *)data;
    header->magic[0] = magicBytes[0];
    header->magic[1] = magicBytes[1];
    header->magic[2] = magicBytes[2];
    header->magic[3] = magicBytes[3];
    header->major = V_MAJOR;
    header->minor = V_MINOR;
    header->subminor = V_SUBMINOR;
    header->timestamp = (time_t)0;
    
    return true;
}

Snapshot *
Snapshot::makeWithBuffer(const uint8_t *buffer, size_t length)
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
    
    snapshot = new Snapshot();
    snapshot->setCapacity(c64->stateSize());
    snapshot->setTimestamp(time(NULL));
    snapshot->takeScreenshot((uint32_t *)c64->vic.screenBuffer(), c64->vic.isPAL());
    uint8_t *ptr = snapshot->getData();
    c64->saveToBuffer(&ptr);
    
    return snapshot;
}

bool 
Snapshot::hasSameType(const char *filename)
{
    return Snapshot::isSnapshotFile(filename, V_MAJOR, V_MINOR, V_SUBMINOR);
}

void
Snapshot::takeScreenshot(uint32_t *buf, bool pal)
{
    SnapshotHeader *header = (SnapshotHeader *)data;
    unsigned x_start, y_start;
       
    if (pal) {
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
    
    uint32_t *target = header->screenshot.screen;
    buf += x_start + y_start * NTSC_PIXELS;
    for (unsigned i = 0; i < header->screenshot.height; i++) {
        memcpy(target, buf, header->screenshot.width * 4);
        target += header->screenshot.width;
        buf += NTSC_PIXELS;
    }
}
