// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
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
    memcpy((void *)dst, (const void *)byteView(offset, count).data(), count);
}

void
DiskImage::write(const u8 *src, isize offset, isize count)
{
    memcpy((void *)mutableByteView(offset, count).data(), (const void *)src, count);
}

utl::ByteView
DiskImage::trackView(TrackNr t) const
{
    return byteView(boffset(TS{t,0}), numSectors(t) * bsize());
}

utl::ByteView
DiskImage::sectorView(TrackNr t, SectorNr s) const
{
    return byteView(boffset(TS{t,s}), bsize());
}

utl::MutableByteView
DiskImage::mutableTrackView(TrackNr t)
{
    return mutableByteView(boffset(TS{t,0}), numSectors(t) * bsize());
}

utl::MutableByteView
DiskImage::mutableSectorView(TrackNr t, SectorNr s)
{
    return mutableByteView(boffset(TS{t,s}), bsize());
}

}
