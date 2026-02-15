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

    width = i32(PAL::VISIBLE_PIXELS / dx);
    height = i32(c64.vic.numVisibleLines() / dy);

    u32 *target = screen;
    u32 *source = (u32 *)c64.videoPort.getTexture().pixels.ptr; // oldGetTexture();
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
    auto suffix = utl::uppercased(path.extension().string());
    return suffix == ".VCSNAP";
}

bool
Snapshot::isCompatible(const u8 *buf, isize len)
{
    if (len < isize(sizeof(SnapshotHeader))) return false;
    return utl::matchingBufferHeader(buf, string("VC64"));
}

bool
Snapshot::isCompatible(const Buffer<u8> &buf)
{
    return isCompatible(buf.ptr, buf.size);
}

Snapshot::Snapshot(isize capacity)
{
    init(capacity + sizeof(SnapshotHeader));

    SnapshotHeader *header = (SnapshotHeader *)data.ptr;

    header->magic[0] = 'V';
    header->magic[1] = 'C';
    header->magic[2] = '6';
    header->magic[3] = '4';
    header->major = SNP_MAJOR;
    header->minor = SNP_MINOR;
    header->subminor = SNP_SUBMINOR;
    header->beta = SNP_BETA;
    header->rawSize = i32(data.size);
}

Snapshot::Snapshot(C64 &c64) : Snapshot(c64.size())
{
    takeScreenshot(c64);

    if (SNP_DEBUG) c64.dump(Category::State);
    c64.save(getSnapshotData());
}

Snapshot::Snapshot(C64 &c64, Compressor compressor) : Snapshot(c64)
{
    compress(compressor);
}

void
Snapshot::finalizeRead()
{
    if (FORCE_SNAP_TOO_OLD) throw AppError(Fault::SNAP_TOO_OLD);
    if (FORCE_SNAP_TOO_NEW) throw AppError(Fault::SNAP_TOO_NEW);
    if (FORCE_SNAP_IS_BETA) throw AppError(Fault::SNAP_IS_BETA);

    if (isTooOld()) throw AppError(Fault::SNAP_TOO_OLD);
    if (isTooNew()) throw AppError(Fault::SNAP_TOO_NEW);
    if (isBeta() && !betaRelease) throw AppError(Fault::SNAP_IS_BETA);
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
    
    return
    header->major != SNP_MAJOR ? header->major < SNP_MAJOR :
    header->minor != SNP_MINOR ? header->minor < SNP_MINOR :
    header->subminor < SNP_SUBMINOR;

    /*
    if (header->major < SNP_MAJOR) return true;
    if (header->major > SNP_MAJOR) return false;
    if (header->minor < SNP_MINOR) return true;
    if (header->minor > SNP_MINOR) return false;
    
    return header->subminor < SNP_SUBMINOR;
    */
}

bool
Snapshot::isTooNew() const
{
    auto header = getHeader();
    
    return
    header->major != SNP_MAJOR ? header->major > SNP_MAJOR :
    header->minor != SNP_MINOR ? header->minor > SNP_MINOR :
    header->subminor > SNP_SUBMINOR;

    /*
    if (header->major > SNP_MAJOR) return true;
    if (header->major < SNP_MAJOR) return false;
    if (header->minor > SNP_MINOR) return true;
    if (header->minor < SNP_MINOR) return false;

    return header->subminor > SNP_SUBMINOR;
    */
}

bool
Snapshot::isBeta() const
{
    return getHeader()->beta != 0;
}

void
Snapshot::takeScreenshot(C64 &c64)
{
    ((SnapshotHeader *)data.ptr)->screenshot.take(c64);
}

void 
Snapshot::compress(Compressor compressor)
{
    loginfo(SNP_DEBUG, "compress(%s)\n", CompressorEnum::key(compressor));

    if (!isCompressed()) {

        loginfo(SNP_DEBUG, "Compressing %ld bytes (hash: 0x%x)...", data.size, data.fnv32());

        {   auto watch = utl::StopWatch(SNP_DEBUG, "");
            
            switch (compressor) {
                    
                case Compressor::NONE:  break;
                case Compressor::GZIP:  data.gzip(sizeof(SnapshotHeader)); break;
                case Compressor::LZ4:   data.lz4 (sizeof(SnapshotHeader)); break;
                case Compressor::RLE2:  data.rle2(sizeof(SnapshotHeader)); break;
                case Compressor::RLE3:  data.rle3(sizeof(SnapshotHeader)); break;
            }
            
            getHeader()->compressor = u8(compressor);
        }
        loginfo(SNP_DEBUG, "Compressed size: %ld bytes\n", data.size);
    }
    
    /*
    if (!isCompressed()) {

        loginfo(SNP_DEBUG, "Compressing %ld bytes (hash: 0x%x)...\n", data.size, data.fnv32());

        data.rle2(sizeof(SnapshotHeader));
        getHeader()->compressed = true;

        loginfo(SNP_DEBUG, "Compressed size: %ld bytes\n", data.size);
    }
    */
}
void 
Snapshot::uncompress()
{
    loginfo(SNP_DEBUG, "uncompress(%s)\n", CompressorEnum::key(compressor()));

    if (isCompressed()) {
        
        isize expectedSize = getHeader()->rawSize;
        
        loginfo(SNP_DEBUG, "Uncompressing %ld bytes...", data.size);
        
        {   auto watch = utl::StopWatch(SNP_DEBUG, "");
        
            switch (compressor()) {
                    
                case Compressor::NONE:  break;
                case Compressor::GZIP:  data.gunzip(sizeof(SnapshotHeader), expectedSize); break;
                case Compressor::LZ4:   data.unlz4 (sizeof(SnapshotHeader), expectedSize); break;
                case Compressor::RLE2:  data.unrle2(sizeof(SnapshotHeader), expectedSize); break;
                case Compressor::RLE3:  data.unrle3(sizeof(SnapshotHeader), expectedSize); break;
            }
            
            getHeader()->compressor = u8(Compressor::NONE);
        }
        loginfo(SNP_DEBUG, "Uncompressed size: %ld bytes (hash: 0x%x)\n", data.size, data.fnv32());
        
        if (getHeader()->rawSize != expectedSize) {
         
            warn("Snaphot size: %ld. Expected: %ld\n", data.size, expectedSize);
            fatalError;
        }
    }
    
    /*
    if (isCompressed()) {

        loginfo(SNP_DEBUG, "Uncompressing %ld bytes...\n", data.size);

        data.unrle2(sizeof(SnapshotHeader));
        getHeader()->compressed = false;

        loginfo(SNP_DEBUG, "Uncompressed size: %ld bytes (hash: 0x%x)\n", data.size, data.fnv32());
    }
    */
}

}
