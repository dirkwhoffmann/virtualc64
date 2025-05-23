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

#include "VirtualC64Config.h"
#include "DiskAnalyzer.h"
#include "Drive.h"
#include "Disk.h"

#include <stdarg.h>

namespace vc64 {

DiskAnalyzer::DiskAnalyzer(const Disk &disk)
{
    init(disk);
}

DiskAnalyzer::DiskAnalyzer(const class Drive &drive)
{
    auto disk = drive.disk.get();
    if (disk == nullptr) throw Error(Fault::DRV_NO_DISK);

    init(*disk);
}

DiskAnalyzer::~DiskAnalyzer()
{    
    for (isize ht = 1; ht < 85; ht++) delete [] data[ht];
}

void 
DiskAnalyzer::init(const class Disk &disk)
{
    // Extract the GCR encoded bit stream from the disk
    for (Halftrack ht = 1; ht < 85; ht++) {

        length[ht] = disk.length.halftrack[ht];
        data[ht] = new u8[2 * maxBitsOnTrack]();

        for (isize i = 0, j = 0; i < maxBytesOnTrack; i++) {

            auto byte = disk.data.halftrack[ht][i];
            for (isize k = 7; k >= 0; k--) data[ht][j++] = !!GET_BIT(byte, k);
        }

        assert(length[ht] <= maxBitsOnTrack);
        std::memcpy(data[ht] + length[ht], data[ht], length[ht]);
    }

    // Analyze the bit stream
    analyzeDisk();
}

isize
DiskAnalyzer::lengthOfTrack(Track t) const
{
    assert(isTrackNumber(t));
    return length[2 * t - 1];
}

isize
DiskAnalyzer::lengthOfHalftrack(Halftrack ht) const
{
    assert(isHalftrackNumber(ht));
    return length[ht];
}

u8
DiskAnalyzer::decodeGcrNibble(Halftrack ht, isize offset)
{
    auto gcr = data[ht] + offset;

    auto codeword = gcr[0] << 4 | gcr[1] << 3 | gcr[2] << 2 | gcr[3] << 1 | gcr[4];
    assert(codeword < 32);
    
    return Disk::invgcr[codeword];
}

u8
DiskAnalyzer::decodeGcr(Halftrack ht, isize offset)
{
    u8 nibble1 = decodeGcrNibble(ht, offset);
    u8 nibble2 = decodeGcrNibble(ht, offset + 5);

    return (u8)(nibble1 << 4 | nibble2);
}

void DiskAnalyzer::analyzeDisk()
{
    for (isize ht = 1; ht < 85; ht++) {
        diskInfo.trackInfo[ht] = analyzeHalftrack(ht);
    }
}

TrackInfo
DiskAnalyzer::analyzeTrack(Track t)
{
    assert(isTrackNumber(t));
    return analyzeHalftrack(2 * t - 1);
}

TrackInfo
DiskAnalyzer::analyzeHalftrack(Halftrack ht)
{
    TrackInfo trackInfo = {};
    trackInfo.length = lengthOfHalftrack(ht);

    assert(errorLog[ht].empty());
    assert(errorStartIndex[ht].empty());
    assert(errorEndIndex[ht].empty());

    // Indicates where the sector headers blocks and the sectors data blocks start
    u8 sync[2 * maxBitsOnTrack] = { };

    // Scan for SYNC sequences and decode the byte that follows
    isize noOfOnes = 0;
    long stop = (long)(2 * trackInfo.length - 10);
    for (long i = 0; i < stop; i++) {
        
        assert(data[ht][i] == 0 || data[ht][i] == 1);
        if (data[ht][i] == 0 && noOfOnes >= 10) {
            
            // <--- SYNC ---><-- sync[i] -->
            // 11111 .... 1110
            //               ^ <- We are at offset i which is here
            sync[i] = decodeGcr(ht, i);
            
            if (sync[i] == 0x08) {
                trace(GCR_DEBUG, "Sector header block found at offset %ld\n", i);
            } else if (sync[i] == 0x07) {
                trace(GCR_DEBUG, "Sector data block found at offset %ld\n", i);
            } else {
                log(ht, i, 10, "Invalid sector ID %02X at index %d. Should be 0x07 or 0x08.", sync[i], i);
            }
        }
        noOfOnes = data[ht][i] ? (noOfOnes + 1) : 0;
    }
    
    // Lookup first sector header block
    isize startOffset;
    for (startOffset = 0; startOffset < trackInfo.length; startOffset++) {
        if (sync[startOffset] == 0x08) {
            break;
        }
    }
    if (startOffset == trackInfo.length) {
        
        log(ht, 0, trackInfo.length, "This track contains no sector header block.");
        return trackInfo;

    } else {

        // Compute offsets to all sectors
        u8 sector = UINT8_MAX;
        for (isize i = startOffset; i < startOffset + trackInfo.length; i++) {
            
            if (sync[i] == 0x08) {
                
                sector = decodeGcr(ht, i + 20);

                if (isSectorNumber(sector)) {
                    if (trackInfo.sectorInfo[sector].headerEnd != 0)
                        break; // We've seen this sector already, so we are done
                    trackInfo.sectorInfo[sector].headerBegin = i;
                    trackInfo.sectorInfo[sector].headerEnd = i + headerBlockSize;
                } else {
                    log(ht, i + 20, 10, "Header block at index %d contains an invalid sector number (%d).", i, sector);
                }
                
            } else if (sync[i] == 0x07) {
                
                if (isSectorNumber(sector)) {
                    trackInfo.sectorInfo[sector].dataBegin = i;
                    trackInfo.sectorInfo[sector].dataEnd = i + dataBlockSize;
                } else {
                    log(ht, i + 20, 10, "Data block at index %d contains an invalid sector number (%d).", i, sector);
                }
            }
        }
    }

    analyzeSectorBlocks(ht, trackInfo);
    return trackInfo;
}

void
DiskAnalyzer::analyzeSectorBlocks(Halftrack ht, TrackInfo &trackInfo)
{
    Track t = (ht + 1) / 2;
    
    for (Sector s = 0; s < Disk::trackDefaults[t].sectors; s++) {
        
        const SectorInfo &info = trackInfo.sectorInfo[s];
        bool hasHeader = info.headerBegin != info.headerEnd;
        bool hasData = info.dataBegin != info.dataEnd;

        if (!hasHeader && !hasData) {
            log(ht, 0, 0, "Sector %d is missing.\n", s);
            continue;
        }
        
        if (hasHeader) {
            analyzeSectorHeaderBlock(ht, info.headerBegin, trackInfo);
        } else {
            log(ht, 0, 0, "Sector %d has no header block.\n", s);
        }
        
        if (hasData) {
            analyzeSectorDataBlock(ht, info.dataBegin, trackInfo);
        } else {
            log(ht, 0, 0, "Sector %d has no data block.\n", s);
        }
    }
}

void
DiskAnalyzer::analyzeSectorHeaderBlock(Halftrack ht, isize offset, TrackInfo &trackInfo)
{
    // The first byte must be 0x08 (indicating a header block)
    assert(decodeGcr(ht, offset) == 0x08);
    offset += 10;
    
    u8 s = decodeGcr(ht, offset + 10);
    u8 t = decodeGcr(ht, offset + 20);
    u8 id2 = decodeGcr(ht, offset + 30);
    u8 id1 = decodeGcr(ht, offset + 40);

    u8 checksum = id1 ^ id2 ^ t ^ s;

    if (checksum != decodeGcr(ht, offset)) {
        log(ht, offset, 10, "Header block at index %d contains an invalid checksum.\n", offset);
    }
}

void
DiskAnalyzer::analyzeSectorDataBlock(Halftrack ht, isize offset, TrackInfo &trackInfo)
{
    // The first byte must be 0x07 (indicating a header block)
    assert(decodeGcr(ht, offset) == 0x07);
    offset += 10;
    
    u8 checksum = 0;
    for (isize i = 0; i < 256; i++, offset += 10) {
        checksum ^= decodeGcr(ht, offset);
    }
    
    if (checksum != decodeGcr(ht, offset)) {
        log(ht, offset, 10, "Data block at index %d contains an invalid checksum.\n", offset);
    }
}

const SectorInfo &
DiskAnalyzer::sectorLayout(Halftrack ht, Sector nr) {

    assert(isHalftrackNumber(ht));
    assert(isSectorNumber(nr));
    
    return diskInfo.trackInfo[ht].sectorInfo[nr];
}

void
DiskAnalyzer::log(Halftrack ht, isize begin, isize length, const char *fmt, ...)
{
    char buf[256];
    
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);

    // logbook[ht] += std::to_string(begin) + " - " + std::to_string(begin + length) + ": ";
    logbook[ht] += string(buf) + "\n";

    errorLog[ht].push_back(string(buf));
    errorStartIndex[ht].push_back(begin);
    errorEndIndex[ht].push_back(begin + length);
}

const char *
DiskAnalyzer::diskNameAsString()
{
    auto &info = diskInfo.trackInfo[18].sectorInfo[0];
    
    isize i;
    isize offset = info.dataBegin + (0x90 * 10);
    
    for (i = 0; i < 255; i++, offset += 10) {
        u8 value = decodeGcr(18, offset);
        if (value == 0xA0)
            break;
        text[i] = value;
    }
    text[i] = 0;
    return text;
}


const char *
DiskAnalyzer::trackBitsAsString(Halftrack ht)
{
    assert(isHalftrackNumber(ht));

    isize i, l;

    for (i = 0, l = lengthOfHalftrack(ht); i < l; i++) {
        if (data[ht][i]) {
            text[i] = '1';
        } else {
            text[i] = '0';
        }
    }
    text[i] = 0;
    return text;
}

const char *
DiskAnalyzer::sectorHeaderBytesAsString(Halftrack ht, Sector nr, bool hex)
{
    assert(isHalftrackNumber(ht));
    assert(isSectorNumber(nr));
    
    auto info = diskInfo.trackInfo[ht].sectorInfo[nr];
    
    isize begin = info.headerBegin;
    isize end = info.headerEnd;
    return (begin == end) ? "" : sectorBytesAsString(ht, begin, 10, hex);
}

const char *
DiskAnalyzer::sectorDataBytesAsString(Halftrack ht, Sector nr, bool hex)
{
    assert(isHalftrackNumber(ht));
    assert(isSectorNumber(nr));
    
    auto info = diskInfo.trackInfo[ht].sectorInfo[nr];

    isize begin = info.dataBegin;
    isize end = info.dataEnd;
    return (begin == end) ? "" : sectorBytesAsString(ht, begin, 256, hex);
}

const char *
DiskAnalyzer::sectorBytesAsString(Halftrack ht, isize offset, isize length, bool hex)
{
    isize gcrOffset = 0;
    isize strOffset = 0;
    
    for (isize i = 0; i < length; i++, gcrOffset += 10) {

        u8 value = decodeGcr(ht, offset + gcrOffset);

        if (hex) {
            util::sprint8x(text + strOffset, value);
            text[strOffset + 2] = ' ';
            strOffset += 3;
        } else {
            util::sprint8d(text + strOffset, value);
            text[strOffset + 3] = ' ';
            strOffset += 4;
        }
    }
    text[strOffset] = 0;
    return text;
}

}
