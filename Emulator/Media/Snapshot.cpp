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
#include "IOUtils.h"

using namespace vc64;

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
Snapshot::isCompatible(const string &path)
{
    return true;
}

bool
Snapshot::isCompatible(std::istream &stream)
{
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

    if constexpr (SNP_DEBUG) c64.dump(Category::State);
    c64.save(getData());
}

void
Snapshot::finalizeRead()
{
	if constexpr (FORCE_SNAP_TOO_OLD) throw VC64Error(ERROR_SNAP_TOO_OLD);
	if constexpr (FORCE_SNAP_TOO_NEW) throw VC64Error(ERROR_SNAP_TOO_NEW);
	if constexpr (FORCE_SNAP_IS_BETA) throw VC64Error(ERROR_SNAP_IS_BETA);

	if (isTooOld()) throw VC64Error(ERROR_SNAP_TOO_OLD);
	if (isTooNew()) throw VC64Error(ERROR_SNAP_TOO_NEW);
	if (isBeta() && !betaRelease) throw VC64Error(ERROR_SNAP_IS_BETA);
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
	auto header = getHeader();

	return header->beta != 0;
}

void
Snapshot::takeScreenshot(C64 &c64)
{
    SnapshotHeader *header = (SnapshotHeader *)data;
    
    header->screenshot.width = VISIBLE_PIXELS;
    header->screenshot.height = c64.vic.numVisibleLines();
    
    u32 *source = (u32 *)c64.vic.stableEmuTexture();
    u32 *target = header->screenshot.screen;

    isize xStart = FIRST_VISIBLE_PIXEL;
    isize yStart = FIRST_VISIBLE_LINE;
    source += xStart + yStart * TEX_WIDTH;
    
    for (isize i = 0; i < header->screenshot.height; i++) {
        
        std::memcpy(target, source, header->screenshot.width * 4);
        target += header->screenshot.width;
        source += TEX_WIDTH;
    }
}
