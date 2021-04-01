// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "C64.h"
#include "IO.h"

bool
Snapshot::isCompatibleName(const std::string &name)
{
    return true; // name == "VC64"; 
}

bool
Snapshot::isCompatibleStream(std::istream &stream)
{
    const u8 magicBytes[] = { 'V', 'C', '6', '4' };
    
    if (streamLength(stream) < 0x15) return false; 
    return util::matchingStreamHeader(stream, magicBytes, sizeof(magicBytes));
}

Snapshot::Snapshot(usize capacity)
{
    size = capacity + sizeof(SnapshotHeader);
    data = new u8[size];
    
    SnapshotHeader *header = (SnapshotHeader *)data;
    header->magicBytes[0] = 'V';
    header->magicBytes[1] = 'C';
    header->magicBytes[2] = '6';
    header->magicBytes[3] = '4';
    header->major = V_MAJOR;
    header->minor = V_MINOR;
    header->subminor = V_SUBMINOR;
    header->timestamp = time(nullptr);
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
    if (data[4] < V_MAJOR) return true; else if (data[4] > V_MAJOR) return false;
    if (data[5] < V_MINOR) return true; else if (data[5] > V_MINOR) return false;
    return data[6] < V_SUBMINOR;
}

bool
Snapshot::isTooNew() const
{
    if (data[4] > V_MAJOR) return true; else if (data[4] < V_MAJOR) return false;
    if (data[5] > V_MINOR) return true; else if (data[5] < V_MINOR) return false;
    return data[6] > V_SUBMINOR;
}

void
Snapshot::takeScreenshot(C64 *c64)
{
    SnapshotHeader *header = (SnapshotHeader *)data;
    
    unsigned xStart = FIRST_VISIBLE_PIXEL;
    unsigned yStart = FIRST_VISIBLE_LINE;
    header->screenshot.width = VISIBLE_PIXELS;
    header->screenshot.height = c64->vic.numVisibleRasterlines();
    
    u32 *source = (u32 *)c64->vic.stableEmuTexture();
    u32 *target = header->screenshot.screen;
    source += xStart + yStart * TEX_WIDTH;
    for (unsigned i = 0; i < header->screenshot.height; i++) {
        memcpy(target, source, header->screenshot.width * 4);
        target += header->screenshot.width;
        source += TEX_WIDTH;
    }
}
