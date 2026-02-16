// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "DiskImage.h"
#include "utl/support/Strings.h"

#include "ADFFile.h"
#include "ADZFile.h"
#include "EADFFile.h"
#include "HDFFile.h"
#include "HDZFile.h"
#include "IMGFile.h"
#include "STFile.h"
#include "DMSFile.h"
#include "EXEFile.h"
#include "D64File.h"

namespace retro::vault {

using CHS = TrackDevice::CHS;
using TS  = TrackDevice::TS;

optional<ImageInfo>
DiskImage::about(const fs::path& path)
{
    if (auto info = FloppyDiskImage::about(path)) return info;
    if (auto info = HardDiskImage::about(path))   return info;

    return {};
}

std::unique_ptr<DiskImage>
DiskImage::tryMake(const fs::path& path)
{
    if (auto img = FloppyDiskImage::make(path)) return img;
    if (auto img = HardDiskImage::make(path))   return img;

    return nullptr;
}

std::unique_ptr<DiskImage>
DiskImage::make(const fs::path& path)
{
    if (auto img = tryMake(path)) return img;
    throw IOError(IOError::FILE_TYPE_UNSUPPORTED);
}

void
DiskImage::read(u8 *dst, isize offset, isize count) const
{
    assert(offset + count <= data.size);
    memcpy((void *)dst, (void *)(data.ptr + offset), count);
}

void
DiskImage::write(const u8 *src, isize offset, isize count)
{
    assert(offset + count <= data.size);
    memcpy((void *)(data.ptr + offset), (void *)src, count);
    
    /*
    if (writeThrough && file) {
        
        printf("Write through...\n");
        
        // Move to the correct position
        file.seekp(offset, std::ios::beg);

        // Write the data to the stream
        file.write((char *)(data.ptr + offset), count);
        
        // Update the file on disk
        file.flush();
    }
    */
}

ByteView
DiskImage::byteView(TrackNr t) const
{
    return ByteView(data.ptr + boffset(TS{t,0}), numSectors(t) * bsize());
}

ByteView
DiskImage::byteView(TrackNr t, SectorNr s) const
{
    return ByteView(data.ptr + boffset(TS{t,s}), bsize());
}

MutableByteView
DiskImage::byteView(TrackNr t)
{
    return MutableByteView(data.ptr + boffset(TS{t,0}), numSectors(t) * bsize());
}

MutableByteView
DiskImage::byteView(TrackNr t, SectorNr s)
{
    return MutableByteView(data.ptr + boffset(TS{t,s}), bsize());
}

void
DiskImage::saveBlocks(const Range<BlockNr> range)
{
    save(Range<isize>{range.lower * bsize(), range.upper * bsize()});
}

void
DiskImage::saveBlocks(const std::vector<Range<BlockNr>> ranges)
{
    for (auto &range: ranges) save(range);
}

}
