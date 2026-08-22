// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "rvconfig.h"
#include "Images/DiskImage.h"
#include "utl/support/Strings.h"

#include "Images/ADF/ADFFile.h"
#include "Images/EADF/EADFFile.h"
#include "Images/HDF/HDFFile.h"
#include "Images/IMG/IMGFile.h"
#include "Images/ST/STFile.h"
#include "Images/DMS/DMSFile.h"
#include "Images/EXE/EXEFile.h"
#include "Images/D64/D64File.h"

namespace retro::vault {

using utl::IOError;
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
    /* tryMake, not make, on both lines. make() throws FILE_TYPE_UNSUPPORTED
     * when it recognises nothing, so calling it here meant this function threw
     * for every non-floppy instead of answering nullptr -- and never reached
     * the hard disk line at all, so a valid HDF was reported as unsupported.
     */
    if (auto img = FloppyDiskImage::tryMake(path)) return img;
    if (auto img = HardDiskImage::tryMake(path))   return img;

    return nullptr;
}

std::unique_ptr<DiskImage>
DiskImage::make(const fs::path& path)
{
    if (auto img = tryMake(path)) return img;
    throw utl::IOError(utl::IOError::FILE_TYPE_UNSUPPORTED);
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
}

utl::ByteView
DiskImage::byteView(TrackNr t) const
{
    return utl::ByteView(data.ptr + boffset(TS{t,0}), numSectors(t) * bsize());
}

utl::ByteView
DiskImage::byteView(TrackNr t, SectorNr s) const
{
    return utl::ByteView(data.ptr + boffset(TS{t,s}), bsize());
}

utl::MutableByteView
DiskImage::byteView(TrackNr t)
{
    return utl::MutableByteView(data.ptr + boffset(TS{t,0}), numSectors(t) * bsize());
}

utl::MutableByteView
DiskImage::byteView(TrackNr t, SectorNr s)
{
    return utl::MutableByteView(data.ptr + boffset(TS{t,s}), bsize());
}

void
DiskImage::saveBlocks(const utl::Range<BlockNr> range)
{
    save(utl::Range<isize>{range.lower * bsize(), range.upper * bsize()});
}

void
DiskImage::saveBlocks(const std::vector<utl::Range<BlockNr>> ranges)
{
    for (auto &range: ranges) save(range);
}

}
