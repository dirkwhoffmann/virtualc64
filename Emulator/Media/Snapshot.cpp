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

#include "config.h"
#include "Snapshot.h"
#include "C64.h"
#include "IOUtils.h"

namespace vc64 {

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
    isize xStart = PAL::FIRST_VISIBLE_PIXEL;
    isize yStart = PAL::FIRST_VISIBLE_LINE;

    width = PAL::VISIBLE_PIXELS / dx;
    height = c64.vic.numVisibleLines() / dy;

    u32 *target = screen;
    u32 *source = (u32 *)c64.videoPort.getTexture();
    source += xStart + yStart * Texture::width;

    for (isize y = 0; y < height; y++) {
        for (isize x = 0; x < width; x++) {
            target[x] = source[x * dx];
        }
        source += Texture::width * dy;
        target += width;
    }
    
    timestamp = time(nullptr);
}

bool
Snapshot::isCompatible(const fs::path &path)
{
    return true;
}

bool
Snapshot::isCompatible(std::istream &stream)
{
    if (util::streamLength(stream) < 0x15) return false;
    return util::matchingStreamHeader(stream, "VC64");
}

Snapshot::Snapshot(isize capacity)
{
    size = capacity + sizeof(SnapshotHeader);
    data = new u8[size];
    
    SnapshotHeader *header = (SnapshotHeader *)data;
    
    header->magic[0] = 'V';
    header->magic[1] = 'C';
    header->magic[2] = '6';
    header->magic[3] = '4';
    header->major = SNP_MAJOR;
    header->minor = SNP_MINOR;
    header->subminor = SNP_SUBMINOR;
}

Snapshot::Snapshot(C64 &c64): Snapshot(c64.size())
{
    takeScreenshot(c64);

    if (SNP_DEBUG) c64.dump(Category::State);
    c64.save(getSnapshotData());
}

void
Snapshot::finalizeRead()
{
    if (FORCE_SNAP_TOO_OLD) throw Error(ERROR_SNAP_TOO_OLD);
    if (FORCE_SNAP_TOO_NEW) throw Error(ERROR_SNAP_TOO_NEW);
    if (FORCE_SNAP_IS_BETA) throw Error(ERROR_SNAP_IS_BETA);

    if (isTooOld()) throw Error(ERROR_SNAP_TOO_OLD);
    if (isTooNew()) throw Error(ERROR_SNAP_TOO_NEW);
    if (isBeta() && !betaRelease) throw Error(ERROR_SNAP_IS_BETA);
}

std::pair <isize,isize> 
Snapshot::previewImageSize() const
{
    return { getThumbnail().width, getThumbnail().height };
}

const u32 *
Snapshot::previewImageData() const
{
    return getThumbnail().screen;
}

time_t 
Snapshot::timestamp() const
{
    return getThumbnail().timestamp;
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

bool
Snapshot::isBeta() const
{
    return getHeader()->beta != 0;
}

void
Snapshot::takeScreenshot(C64 &c64)
{
    ((SnapshotHeader *)data)->screenshot.take(c64);
}

}
