// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "Snapshot.h"
#include "C64.h"
#include "IO.h"

Thumbnail *
Thumbnail::makeWithC64(const C64 &c64, isize dx, isize dy)
{
    Thumbnail *screenshot = new Thumbnail();
    screenshot->take(c64, dx, dy);
    
    return screenshot;
}

void
Thumbnail::take(const C64 &c64, isize dx, isize dy)
{
    u32 *source = (u32 *)c64.vic.stableEmuTexture();
    u32 *target = screen;

    isize xStart = FIRST_VISIBLE_PIXEL;
    isize yStart = FIRST_VISIBLE_LINE;

    width = VISIBLE_PIXELS;
    height = c64.vic.numVisibleLines();

    source += xStart + yStart * TEX_WIDTH;
    
    for (isize y = 0; y < height; y++) {
        for (isize x = 0; x < width; x++) {
            target[x] = source[x * dx];
        }
        source += TEX_WIDTH;
        target += width;
    }
    
    timestamp = time(nullptr);
}

bool
Snapshot::isCompatiblePath(const std::string &path)
{
    return true;
}

bool
Snapshot::isCompatibleStream(std::istream &stream)
{
    const u8 magicBytes[] = { 'V', 'C', '6', '4' };
    
    if (util::streamLength(stream) < 0x15) return false; 
    return util::matchingStreamHeader(stream, magicBytes, sizeof(magicBytes));
}

Snapshot::Snapshot(isize capacity)
{
    u8 signature[] = { 'V', 'A', 'S', 'N', 'A', 'P' };

    size = capacity + sizeof(SnapshotHeader);
    data = new u8[size];
    
    SnapshotHeader *header = (SnapshotHeader *)data;
    
    for (isize i = 0; i < isizeof(signature); i++)
        header->magic[i] = signature[i];
    header->major = SNP_MAJOR;
    header->minor = SNP_MINOR;
    header->subminor = SNP_SUBMINOR;
}

Snapshot *
Snapshot::makeWithC64(C64 *c64)
{
    Snapshot *snapshot;
    
    snapshot = new Snapshot(c64->size());
    
    snapshot->takeScreenshot(c64);

    if (SNP_DEBUG) c64->dump();
    c64->save(snapshot->getData());
    
    return snapshot;
}

bool
Snapshot::isTooOld() const
{
    auto header = getHeader();
    
    if (header->major < SNP_MAJOR) return true;
    if (header->major > SNP_MAJOR) return false;
    if (header->minor < SNP_MINOR) return true;
    if (header->minor > SNP_MINOR) return false;
    
    return header->subminor < SNP_SUBMINOR;
}

bool
Snapshot::isTooNew() const
{
    auto header = getHeader();
    
    if (header->major > SNP_MAJOR) return true;
    if (header->major < SNP_MAJOR) return false;
    if (header->minor > SNP_MINOR) return true;
    if (header->minor < SNP_MINOR) return false;

    return header->subminor > SNP_SUBMINOR;
}

void
Snapshot::takeScreenshot(C64 *c64)
{
    SnapshotHeader *header = (SnapshotHeader *)data;
    
    isize xStart = FIRST_VISIBLE_PIXEL;
    isize yStart = FIRST_VISIBLE_LINE;
    header->screenshot.width = VISIBLE_PIXELS;
    header->screenshot.height = c64->vic.numVisibleLines();
    
    u32 *source = (u32 *)c64->vic.stableEmuTexture();
    u32 *target = header->screenshot.screen;
    source += xStart + yStart * TEX_WIDTH;
    for (isize i = 0; i < header->screenshot.height; i++) {
        memcpy(target, source, header->screenshot.width * 4);
        target += header->screenshot.width;
        source += TEX_WIDTH;
    }
}
