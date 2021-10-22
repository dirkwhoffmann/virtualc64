// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "DiskAnalyzer.h"
#include "Disk.h"

DiskAnalyzer::DiskAnalyzer(Disk *disk) : disk(disk)
{
    msg("DiskAnalyzer::DiskAnalyzer\n");
    
    /* Create the bit expansion table. The table maps a byte to an expanded
     * 64 bit representation. This lookup table is utilized to quickly inflate
     * a bit stream into a byte stream.
     *
     *     Example: 0110 ... -> 00000000 00000001 0000001 00000000 ...
     *
     * Note that this table expects a Little Endian architecture to work. If
     * you compile the emulator on a Big Endian architecture, the byte order
     * needs to be reversed.
     *
     * TODO: GET RID OF THIS TABLE
     */
    for (isize i = 0; i < 256; i++) {
        
        bitExpansion[i] = 0;
        
        if (i & 0x80) bitExpansion[i] |= 0x0000000000000001;
        if (i & 0x40) bitExpansion[i] |= 0x0000000000000100;
        if (i & 0x20) bitExpansion[i] |= 0x0000000000010000;
        if (i & 0x10) bitExpansion[i] |= 0x0000000001000000;
        if (i & 0x08) bitExpansion[i] |= 0x0000000100000000;
        if (i & 0x04) bitExpansion[i] |= 0x0000010000000000;
        if (i & 0x02) bitExpansion[i] |= 0x0001000000000000;
        if (i & 0x01) bitExpansion[i] |= 0x0100000000000000;
    }
}

DiskAnalyzer::~DiskAnalyzer()
{
    msg("DiskAnalyzer::~DiskAnalyzer\n");
}

u16
DiskAnalyzer::lengthOfTrack(Track t) const
{
    assert(isTrackNumber(t));
    return disk->length.track[t][0];
}

u16
DiskAnalyzer::lengthOfHalftrack(Halftrack ht) const
{
    assert(isHalftrackNumber(ht));
    return disk->length.halftrack[ht];
}

void
DiskAnalyzer::analyzeTrack(Track t)
{
    assert(isTrackNumber(t));
    analyzeHalftrack(2 * t - 1);
}

void
DiskAnalyzer::analyzeHalftrack(Halftrack ht)
{
    assert(isHalftrackNumber(ht));
    
    u16 len = disk->length.halftrack[ht];

    errorLog.clear();
    errorStartIndex.clear();
    errorEndIndex.clear();
    
    // The result of the analysis is stored in variable trackInfo
    memset(&trackInfo, 0, sizeof(trackInfo));
    trackInfo.length = len;
    
    // Setup working buffer (two copies of the track, each bit represented by one byte)
    for (isize i = 0; i < maxBytesOnTrack; i++)
        trackInfo.byte[i] = bitExpansion[disk->data.halftrack[ht][i]];
    std::memcpy(trackInfo.bit + len, trackInfo.bit, len);
    
    // Indicates where the sector headers blocks and the sectors data blocks start
    u8 sync[sizeof(trackInfo.bit)];
    std::memset(sync, 0, sizeof(sync));
    
    // Scan for SYNC sequences and decode the byte that follows
    isize noOfOnes = 0;
    long stop = (long)(2 * len - 10);
    for (long i = 0; i < stop; i++) {
        
        assert(trackInfo.bit[i] <= 1);
        if (trackInfo.bit[i] == 0 && noOfOnes >= 10) {
            
            // <--- SYNC ---><-- sync[i] -->
            // 11111 .... 1110
            //               ^ <- We are at offset i which is here
            sync[i] = disk->decodeGcr(trackInfo.bit + i);
            
            if (sync[i] == 0x08) {
                trace(GCR_DEBUG, "Sector header block found at offset %ld\n", i);
            } else if (sync[i] == 0x07) {
                trace(GCR_DEBUG, "Sector data block found at offset %ld\n", i);
            } else {
                log(i, 10, "Invalid sector ID %02X at index %d. Should be 0x07 or 0x08.", sync[i], i);
            }
        }
        noOfOnes = trackInfo.bit[i] ? (noOfOnes + 1) : 0;
    }
    
    // Lookup first sector header block
    isize startOffset;
    for (startOffset = 0; startOffset < len; startOffset++) {
        if (sync[startOffset] == 0x08) {
            break;
        }
    }
    if (startOffset == len) {
        
        log(0, len, "This track contains no sector header block.");
        return;

    } else {
    
        // Compute offsets for all sectors
        u8 sector = UINT8_MAX;
        for (isize i = startOffset; i < startOffset + len; i++) {
            
            if (sync[i] == 0x08) {
                
                sector = disk->decodeGcr(trackInfo.bit + i + 20);
                
                if (isSectorNumber(sector)) {
                    if (trackInfo.sectorInfo[sector].headerEnd != 0)
                        break; // We've seen this sector already, so we are done.
                    trackInfo.sectorInfo[sector].headerBegin = i;
                    trackInfo.sectorInfo[sector].headerEnd = i + headerBlockSize;
                } else {
                    log(i + 20, 10, "Header block at index %d contains an invalid sector number (%d).", i, sector);
                }
                
            } else if (sync[i] == 0x07) {
                
                if (isSectorNumber(sector)) {
                    trackInfo.sectorInfo[sector].dataBegin = i;
                    trackInfo.sectorInfo[sector].dataEnd = i + dataBlockSize;
                } else {
                    log(i + 20, 10, "Data block at index %d contains an invalid sector number (%d).", i, sector);
                }
            }
        }
    }

    analyzeSectorBlocks(ht, trackInfo);
}

void
DiskAnalyzer::analyzeSectorBlocks(Halftrack ht, TrackInfo &trackInfo)
{
    Track t = (ht + 1) / 2;
    
    for (Sector s = 0; s < Disk::trackDefaults[t].sectors; s++) {
        
        SectorInfo *info = &trackInfo.sectorInfo[s];
        bool hasHeader = info->headerBegin != info->headerEnd;
        bool hasData = info->dataBegin != info->dataEnd;

        if (!hasHeader && !hasData) {
            log(0, 0, "Sector %d is missing.\n", s);
            continue;
        }
        
        if (hasHeader) {
            analyzeSectorHeaderBlock(info->headerBegin, trackInfo);
        } else {
            log(0, 0, "Sector %d has no header block.\n", s);
        }
        
        if (hasData) {
            analyzeSectorDataBlock(info->dataBegin, trackInfo);
        } else {
            log(0, 0, "Sector %d has no data block.\n", s);
        }
    }
}

void
DiskAnalyzer::analyzeSectorHeaderBlock(isize offset, TrackInfo &trackInfo)
{
    // The first byte must be 0x08 (indicating a header block)
    assert(disk->decodeGcr(trackInfo.bit + offset) == 0x08);
    offset += 10;
    
    u8 s = disk->decodeGcr(trackInfo.bit + offset + 10);
    u8 t = disk->decodeGcr(trackInfo.bit + offset + 20);
    u8 id2 = disk->decodeGcr(trackInfo.bit + offset + 30);
    u8 id1 = disk->decodeGcr(trackInfo.bit + offset + 40);
    u8 checksum = id1 ^ id2 ^ t ^ s;

    if (checksum != disk->decodeGcr(trackInfo.bit + offset)) {
        log(offset, 10, "Header block at index %d contains an invalid checksum.\n", offset);
    }
}

void
DiskAnalyzer::analyzeSectorDataBlock(isize offset, TrackInfo &trackInfo)
{
    // The first byte must be 0x07 (indicating a header block)
    assert(disk->decodeGcr(trackInfo.bit + offset) == 0x07);
    offset += 10;
    
    u8 checksum = 0;
    for (isize i = 0; i < 256; i++, offset += 10) {
        checksum ^= disk->decodeGcr(trackInfo.bit + offset);
    }
    
    if (checksum != disk->decodeGcr(trackInfo.bit + offset)) {
        log(offset, 10, "Data block at index %d contains an invalid checksum.\n", offset);
    }
}

void
DiskAnalyzer::log(isize begin, isize length, const char *fmt, ...)
{
    char buf[256];
    
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);

    errorLog.push_back(string(buf));
    errorStartIndex.push_back(begin);
    errorEndIndex.push_back(begin + length);
}

const char *
DiskAnalyzer::diskNameAsString()
{
    analyzeTrack(18);
    
    isize i;
    isize offset = trackInfo.sectorInfo[0].dataBegin + (0x90 * 10);
    
    for (i = 0; i < 255; i++, offset += 10) {
        u8 value = disk->decodeGcr(trackInfo.bit + offset);
        if (value == 0xA0)
            break;
        text[i] = value;
    }
    text[i] = 0;
    return text;
}


const char *
DiskAnalyzer::trackBitsAsString()
{
    isize i;
    for (i = 0; i < trackInfo.length; i++) {
        if (trackInfo.bit[i]) {
            text[i] = '1';
        } else {
            text[i] = '0';
        }
    }
    text[i] = 0;
    return text;
}

const char *
DiskAnalyzer::sectorHeaderBytesAsString(Sector nr, bool hex)
{
    assert(isSectorNumber(nr));
    isize begin = trackInfo.sectorInfo[nr].headerBegin;
    isize end = trackInfo.sectorInfo[nr].headerEnd;
    return (begin == end) ? "" : sectorBytesAsString(trackInfo.bit + begin, 10, hex);
}

const char *
DiskAnalyzer::sectorDataBytesAsString(Sector nr, bool hex)
{
    assert(isSectorNumber(nr));
    isize begin = trackInfo.sectorInfo[nr].dataBegin;
    isize end = trackInfo.sectorInfo[nr].dataEnd;
    return (begin == end) ? "" : sectorBytesAsString(trackInfo.bit + begin, 256, hex);
}

const char *
DiskAnalyzer::sectorBytesAsString(u8 *buffer, isize length, bool hex)
{
    isize gcrOffset = 0;
    isize strOffset = 0;
    
    for (isize i = 0; i < length; i++, gcrOffset += 10) {

        u8 value = disk->decodeGcr(buffer + gcrOffset);

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
