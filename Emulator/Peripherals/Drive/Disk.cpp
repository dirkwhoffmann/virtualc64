// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "C64.h"
#include "IO.h"
#include "Checksum.h"

#include <stdarg.h>

const Disk::TrackDefaults Disk::trackDefaults[43] = {
    
    { 0, 0, 0, 0, 0, 0 }, // Padding
    
    // Speedzone 3 (outer tracks)
    { 21, 3, 7693, 7693 * 8,   0, 0.268956 }, // Track 1
    { 21, 3, 7693, 7693 * 8,  21, 0.724382 }, // Track 2
    { 21, 3, 7693, 7693 * 8,  42, 0.177191 }, // Track 3
    { 21, 3, 7693, 7693 * 8,  63, 0.632698 }, // Track 4
    { 21, 3, 7693, 7693 * 8,  84, 0.088173 }, // Track 5
    { 21, 3, 7693, 7693 * 8, 105, 0.543583 }, // Track 6
    { 21, 3, 7693, 7693 * 8, 126, 0.996409 }, // Track 7
    { 21, 3, 7693, 7693 * 8, 147, 0.451883 }, // Track 8
    { 21, 3, 7693, 7693 * 8, 168, 0.907342 }, // Track 9
    { 21, 3, 7693, 7693 * 8, 289, 0.362768 }, // Track 10
    { 21, 3, 7693, 7693 * 8, 210, 0.815512 }, // Track 11
    { 21, 3, 7693, 7693 * 8, 231, 0.268338 }, // Track 12
    { 21, 3, 7693, 7693 * 8, 252, 0.723813 }, // Track 13
    { 21, 3, 7693, 7693 * 8, 273, 0.179288 }, // Track 14
    { 21, 3, 7693, 7693 * 8, 294, 0.634779 }, // Track 15
    { 21, 3, 7693, 7693 * 8, 315, 0.090253 }, // Track 16
    { 21, 3, 7693, 7693 * 8, 336, 0.545712 }, // Track 17
    
    // Speedzone 2
    { 19, 2, 7143, 7143 * 8, 357, 0.945418 }, // Track 18
    { 19, 2, 7143, 7143 * 8, 376, 0.506081 }, // Track 19
    { 19, 2, 7143, 7143 * 8, 395, 0.066622 }, // Track 20
    { 19, 2, 7143, 7143 * 8, 414, 0.627303 }, // Track 21
    { 19, 2, 7143, 7143 * 8, 433, 0.187862 }, // Track 22
    { 19, 2, 7143, 7143 * 8, 452, 0.748403 }, // Track 23
    { 19, 2, 7143, 7143 * 8, 471, 0.308962 }, // Track 24
    
    // Speedzone 1
    { 18, 1, 6667, 6667 * 8, 490, 0.116926 }, // Track 25
    { 18, 1, 6667, 6667 * 8, 508, 0.788086 }, // Track 26
    { 18, 1, 6667, 6667 * 8, 526, 0.459190 }, // Track 27
    { 18, 1, 6667, 6667 * 8, 544, 0.130238 }, // Track 28
    { 18, 1, 6667, 6667 * 8, 562, 0.801286 }, // Track 29
    { 18, 1, 6667, 6667 * 8, 580, 0.472353 }, // Track 30
    
    // Speedzone 0 (inner tracks)
    { 17, 0, 6250, 6250 * 8, 598, 0.834120 }, // Track 31
    { 17, 0, 6250, 6250 * 8, 615, 0.614880 }, // Track 32
    { 17, 0, 6250, 6250 * 8, 632, 0.395480 }, // Track 33
    { 17, 0, 6250, 6250 * 8, 649, 0.176140 }, // Track 34
    { 17, 0, 6250, 6250 * 8, 666, 0.956800 }, // Track 35
    
    // Speedzone 0 (usually unused tracks)
    { 17, 0, 6250, 6250 * 8, 683, 0.300 }, // Track 36
    { 17, 0, 6250, 6250 * 8, 700, 0.820 }, // Track 37
    { 17, 0, 6250, 6250 * 8, 717, 0.420 }, // Track 38
    { 17, 0, 6250, 6250 * 8, 734, 0.940 }, // Track 39
    { 17, 0, 6250, 6250 * 8, 751, 0.540 }, // Track 40
    { 17, 0, 6250, 6250 * 8, 768, 0.130 }, // Track 41
    { 17, 0, 6250, 6250 * 8, 785, 0.830 }  // Track 42
};

u64 Disk::bitExpansion[256];

isize
Disk::numberOfSectorsInTrack(Track t)
{
    return (t < 1) ? 0 : (t < 18) ? 21 : (t < 25) ? 19 : (t < 31) ? 18 : (t < 43) ? 17 : 0;
}
isize
Disk::numberOfSectorsInHalftrack(Halftrack ht)
{
    return numberOfSectorsInTrack((ht + 1) / 2);
}

isize
Disk::speedZoneOfTrack(Track t)
{
    return (t < 18) ? 3 : (t < 25) ? 2 : (t < 31) ? 1 : 0;
}

isize
Disk::speedZoneOfHalftrack(Halftrack ht)
{
    return (ht < 35) ? 3 : (ht < 49) ? 2 : (ht < 61) ? 1 : 0;
}

bool
Disk::isValidTrackSectorPair(Track t, Sector s)
{
    return s < numberOfSectorsInTrack(t);
}

bool
Disk::isValidHalftrackSectorPair(Halftrack ht, Sector s)
{
    return s < numberOfSectorsInHalftrack(ht);
}

Disk::Disk()
{    
    /* Create the bit expansion table. Note that this table expects a Little
     * Endian architecture to work. If you compile the emulator on a Big Endian
     * architecture, the byte order needs to be reversed.
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
    
    clearDisk();
}

void
Disk::init(const string &path, bool wp)
{
    if (G64File::isCompatible(path)) {
    
        auto file = G64File(path);
        init(file, wp);
        return;
    }
    
    auto fs = FSDevice(path);
    init(fs, wp);
}

void
Disk::init(DOSType type, PETName<16> name, bool wp)
{
    assert_enum(DOSType, type);
    
    if (type == DOS_TYPE_CBM) {
        
        auto fs = FSDevice(DISK_TYPE_SS_SD, DOS_TYPE_CBM);
        fs.setName(name);
        init(fs, wp);
    }
}

void
Disk::init(const FSDevice &fs, bool wp)
{
    encode(fs);
    setWriteProtection(wp);
}

void
Disk::init(const G64File &g64, bool wp)
{
    encodeG64(g64);
    setWriteProtection(wp);
}

void
Disk::init(const D64File &d64, bool wp)
{
    auto fs = FSDevice(d64);
    init(fs, wp);
}

void
Disk::init(AnyCollection &collection, bool wp)
{
    auto fs = FSDevice(collection);
    init(fs, wp);
}

void
Disk::init(util::SerReader &reader)
{
    applyToPersistentItems(reader);
}

void
Disk::_dump(dump::Category category, std::ostream& os) const
{
    using namespace util;
    
    if (category & dump::State) {

        auto checksum = util::fnv_1a_32((const u8 *)data.track, sizeof(data.track));

        os << tab("Write protected") << bol(writeProtected) << std::endl;
        os << tab("Modified") << bol(modified) << std::endl;
        os << tab("Checksum") << hex(checksum) << std::endl;
    }

    if (category & dump::Layout) {

        for (Halftrack ht = 1; ht <= highestHalftrack; ht++) {
            
            auto length = lengthOfHalftrack(ht);
            
            os << tab("Halftrack " + std::to_string(ht));
            os << dec(length) << " Bits (" << dec(length / 8) << " Bytes)";
            os << std::endl;
        }
    }
}

void
Disk::setModified(bool b)
{
    if (b != modified) {
        modified = b;
        // messageQueue.put(MSG_DISK_PROTECT);
    }
}

void
Disk::encodeGcr(u8 value, Track t, HeadPos offset)
{
    assert(isTrackNumber(t));
    
    u8 nibble1 = bin2gcr(value >> 4);
    u8 nibble2 = bin2gcr(value & 0xF);
    
    writeBitToTrack(t, offset++, nibble1 & 0x10);
    writeBitToTrack(t, offset++, nibble1 & 0x08);
    writeBitToTrack(t, offset++, nibble1 & 0x04);
    writeBitToTrack(t, offset++, nibble1 & 0x02);
    writeBitToTrack(t, offset++, nibble1 & 0x01);

    writeBitToTrack(t, offset++, nibble2 & 0x10);
    writeBitToTrack(t, offset++, nibble2 & 0x08);
    writeBitToTrack(t, offset++, nibble2 & 0x04);
    writeBitToTrack(t, offset++, nibble2 & 0x02);
    writeBitToTrack(t, offset++, nibble2 & 0x01);
}

void
Disk::encodeGcr(u8 *values, isize length, Track t, HeadPos offset)
{
    for (isize i = 0; i < length; i++, values++, offset += 10) {
        encodeGcr(*values, t, offset);
    }
}

u8
Disk::decodeGcrNibble(u8 *gcr)
{
    assert(gcr);
    
    auto codeword = gcr[0] << 4 | gcr[1] << 3 | gcr[2] << 2 | gcr[3] << 1 | gcr[4];
    assert(codeword < 32);
    
    return invgcr[codeword];
}

u8
Disk::decodeGcr(u8 *gcr)
{
    assert(gcr);
    
    u8 nibble1 = decodeGcrNibble(gcr);
    u8 nibble2 = decodeGcrNibble(gcr + 5);

    return (u8)(nibble1 << 4 | nibble2);
}

bool
Disk::isValidHeadPos(Halftrack ht, HeadPos pos) const
{
    return isHalftrackNumber(ht) && pos < length.halftrack[ht];
}

HeadPos
Disk::wrap(Halftrack ht, HeadPos pos) const
{
    u16 len = length.halftrack[ht];
    return pos < 0 ? pos + len : pos >= len ? pos - len : pos;
}

u64
Disk::_bitDelay(Halftrack ht, HeadPos pos) const {
    
    assert(isValidHeadPos(ht, pos));

    // In the current implementation, we assume that the density bits were
    // set to their correct values when a bit was written to disk. According
    // to this assumption, the returned value is determined solely by the
    // track position of the drive head.
    
    if (ht <= 33)
        return 4 * 10000; // Density bits = 00: 4 * 16/16 * 10^4 1/10 nsec
    if (ht <= 47)
        return 4 * 9375;  // Density bits = 01: 4 * 15/16 * 10^4 1/10 nsec
    if (ht <= 59)
        return 4 * 8750;  // Density bits = 10: 4 * 14/16 * 10^4 1/10 nsec
    
     return 4 * 8125;     // Density bits = 11: 4 * 13/16 * 10^4 1/10 nsec
}

void
Disk::clearHalftrack(Halftrack ht)
{
    memset(&data.halftrack[ht], 0x55, sizeof(data.halftrack[ht]));
    length.halftrack[ht] = sizeof(data.halftrack[ht]) * 8;
}

void
Disk::clearDisk()
{
    // memset(&data, 0x55, sizeof(data));
    for (Halftrack ht = 1; ht <= highestHalftrack; ht++) {
        // length.halftrack[ht] = sizeof(data.halftrack[ht]) * 8;
        clearHalftrack(ht);
    }
    writeProtected = false;
    modified = false; 
}

bool
Disk::halftrackIsEmpty(Halftrack ht) const
{
    assert(isHalftrackNumber(ht));
    for (isize i = 0; i < isizeof(data.halftrack[ht]); i++)
        if (data.halftrack[ht][i] != 0x55) return false;
    return true;
}

bool
Disk::trackIsEmpty(Track t) const
{
    assert(isTrackNumber(t));
    return halftrackIsEmpty(2 * t - 1);
}

isize
Disk::nonemptyHalftracks() const
{
    isize result = 0;
    
    for (Halftrack ht = 1; ht < 85; ht++) {
        if (!halftrackIsEmpty(ht))
            result++;
    }
    
    return result;
}

u16
Disk::lengthOfTrack(Track t) const
{
    assert(isTrackNumber(t));
    return length.track[t][0];
}

u16
Disk::lengthOfHalftrack(Halftrack ht) const
{
    assert(isHalftrackNumber(ht));
    return length.halftrack[ht];
}

void
Disk::analyzeTrack(Track t)
{
    assert(isTrackNumber(t));
    analyzeHalftrack(2 * t - 1);
}

void
Disk::analyzeHalftrack(Halftrack ht)
{
    assert(isHalftrackNumber(ht));
    
    // TrackInfo trackInfo = { };
    
    u16 len = length.halftrack[ht];

    errorLog.clear();
    errorStartIndex.clear();
    errorEndIndex.clear();
    
    // The result of the analysis is stored in variable trackInfo
    // memset(&trackInfo, 0, sizeof(trackInfo));
    trackInfo.length = len;
    
    // Setup working buffer (two copies of the track, each bit represented by one byte)
    for (isize i = 0; i < maxBytesOnTrack; i++)
        trackInfo.byte[i] = bitExpansion[data.halftrack[ht][i]];
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
            sync[i] = decodeGcr(trackInfo.bit + i);
            
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
    }
    
    // Compute offsets for all sectors
    u8 sector = UINT8_MAX;
    for (isize i = startOffset; i < startOffset + len; i++) {
        
        if (sync[i] == 0x08) {
            
            sector = decodeGcr(trackInfo.bit + i + 20);
            
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
    
    analyzeSectorBlocks(ht, trackInfo);
}

void
Disk::analyzeSectorBlocks(Halftrack ht, TrackInfo &trackInfo)
{
    Track t = (ht + 1) / 2;
    
    for (Sector s = 0; s < trackDefaults[t].sectors; s++) {
        
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
Disk::analyzeSectorHeaderBlock(isize offset, TrackInfo &trackInfo)
{
    // The first byte must be 0x08 (indicating a header block)
    assert(decodeGcr(trackInfo.bit + offset) == 0x08);
    offset += 10;
    
    u8 s = decodeGcr(trackInfo.bit + offset + 10);
    u8 t = decodeGcr(trackInfo.bit + offset + 20);
    u8 id2 = decodeGcr(trackInfo.bit + offset + 30);
    u8 id1 = decodeGcr(trackInfo.bit + offset + 40);
    u8 checksum = id1 ^ id2 ^ t ^ s;

    if (checksum != decodeGcr(trackInfo.bit + offset)) {
        log(offset, 10, "Header block at index %d contains an invalid checksum.\n", offset);
    }
}

void
Disk::analyzeSectorDataBlock(isize offset, TrackInfo &trackInfo)
{
    // The first byte must be 0x07 (indicating a header block)
    assert(decodeGcr(trackInfo.bit + offset) == 0x07);
    offset += 10;
    
    u8 checksum = 0;
    for (isize i = 0; i < 256; i++, offset += 10) {
        checksum ^= decodeGcr(trackInfo.bit + offset);
    }
    
    if (checksum != decodeGcr(trackInfo.bit + offset)) {
        log(offset, 10, "Data block at index %d contains an invalid checksum.\n", offset);
    }
}

void
Disk::log(isize begin, isize length, const char *fmt, ...)
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
Disk::diskNameAsString()
{
    analyzeTrack(18);
    
    isize i;
    isize offset = trackInfo.sectorInfo[0].dataBegin + (0x90 * 10);
    
    for (i = 0; i < 255; i++, offset += 10) {
        u8 value = decodeGcr(trackInfo.bit + offset);
        if (value == 0xA0)
            break;
        text[i] = value;
    }
    text[i] = 0;
    return text;
}


const char *
Disk::trackBitsAsString()
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
Disk::sectorHeaderBytesAsString(Sector nr, bool hex)
{
    assert(isSectorNumber(nr));
    isize begin = trackInfo.sectorInfo[nr].headerBegin;
    isize end = trackInfo.sectorInfo[nr].headerEnd;
    return (begin == end) ? "" : sectorBytesAsString(trackInfo.bit + begin, 10, hex);
}

const char *
Disk::sectorDataBytesAsString(Sector nr, bool hex)
{
    assert(isSectorNumber(nr));
    isize begin = trackInfo.sectorInfo[nr].dataBegin;
    isize end = trackInfo.sectorInfo[nr].dataEnd;
    return (begin == end) ? "" : sectorBytesAsString(trackInfo.bit + begin, 256, hex);
}

const char *
Disk::sectorBytesAsString(u8 *buffer, isize length, bool hex)
{
    isize gcrOffset = 0;
    isize strOffset = 0;
    
    for (isize i = 0; i < length; i++, gcrOffset += 10) {

        u8 value = decodeGcr(buffer + gcrOffset);

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


//
// Decoding disk data
//

isize
Disk::decodeDisk(u8 *dest)
{
    // Determine highest non-empty track
    Track t = 42;
    while (t > 0 && trackIsEmpty(t)) t--;
    
    // Decode disk 
    if (t <= 35)
        return decodeDisk(dest, 35);
        
    if (t <= 40)
        return decodeDisk(dest, 40);
    
    return decodeDisk(dest, 42);
}

isize
Disk::decodeDisk(u8 *dest, isize numTracks)
{
    isize numBytes = 0;

    assert(numTracks == 35 || numTracks == 40 || numTracks == 42);

    // For each full track ...
   for (Track t = 1; t <= numTracks; t++) {
        
        if (trackIsEmpty(t))
            break;
        
       trace(GCR_DEBUG, "Decoding track %zd %s\n", t, dest ? "" : "(test run)");
        numBytes += decodeTrack(t, dest + (dest ? numBytes : 0));
    }
    
    return numBytes;
}

isize
Disk::decodeTrack(Track t, u8 *dest)
{
    isize numBytes = 0;
    isize numSectors = numberOfSectorsInTrack(t);

    // Gather sector information
    analyzeTrack(t);

    // For each sector ...
    for (Sector s = 0; s < numSectors; s++) {
        
        trace(GCR_DEBUG, "   Decoding sector %zd\n", s);
        SectorInfo info = sectorLayout(s);
        if (info.dataBegin != info.dataEnd) {
            numBytes += decodeSector(info.dataBegin, dest + (dest ? numBytes : 0));
        } else {

            // The decoder failed to decode this sector.
            break;
        }
    }
    
    return numBytes;
}

isize
Disk::decodeSector(isize offset, u8 *dest)
{
    // The first byte must be 0x07 (indicating a data block)
    assert(decodeGcr(trackInfo.bit + offset) == 0x07);
    offset += 10;
    
    if (dest) {
        for (isize i = 0; i < 256; i++) {
            dest[i] = decodeGcr(trackInfo.bit + offset);
            offset += 10;
        }
    }
    
    return 256;
}

//
// Encoding disk data
//

void
Disk::encodeG64(const G64File &a)
{
    trace(GCR_DEBUG, "Encoding G64 archive\n");
        
    clearDisk();
    for (Halftrack ht = 1; ht <= 84; ht++) {
        
        isize size = a.getSizeOfHalftrack(ht);
        
        if (size == 0) {
            if (ht > 1) {
                // Make this halftrack as long as the previous halftrack
                length.halftrack[ht] = length.halftrack[ht - 1];
            }
            continue;
        }
        
        if (size > 7928) {
            warn("Halftrack %zd has %zd bytes. Must be less than 7928\n", ht, size);
            continue;
        }
        trace(GCR_DEBUG, "  Encoding halftrack %zd (%zd bytes)\n", ht, size);
        length.halftrack[ht] = (u16)(8 * size);
        
        a.copyHalftrack(ht, data.halftrack[ht]);
    }
}

void
Disk::encode(const FSDevice &fs, bool alignTracks)
{    
    // 64COPY (fails on VICE test drive/skew)
    /*
    int tailGap[4] = { 9, 9, 9, 9 };
    u16 trackLength[4] =
    {
        6250 * 8, // Tracks 31 - 35..42 (inner tracks)
        6666 * 8, // Tracks 25 - 30
        7142 * 8, // Tracks 18 - 24
        7692 * 8  // Tracks  1 - 17     (outer tracks)
    };
    */
    
    // Hoxs64 (passes VICE test drive/skew)
    int tailGap[4] = { 9, 12, 17, 8 };
    u16 trackLength[4] =
    {
        6250 * 8, // Tracks 31 - 35..42 (inner tracks)
        6667 * 8, // Tracks 25 - 30
        7143 * 8, // Tracks 18 - 24
        7693 * 8  // Tracks  1 - 17     (outer tracks)
    };
    
    // VirtualC64 2.4
    /*
    const int tailGap[4] = { 13, 16, 21, 12 };
    const u16 trackLength[4] =
    {
        (u16)(8 * 17 * (354 + tailGap[0])), // Tracks 31 - 35..42 (inner tracks)
        (u16)(8 * 18 * (354 + tailGap[1])), // Tracks 25 - 30
        (u16)(8 * 19 * (354 + tailGap[2])), // Tracks 18 - 24
        (u16)(8 * 21 * (354 + tailGap[3]))  // Tracks  1 - 17     (outer tracks)
    };
    */
    
    isize numTracks = fs.getNumTracks();

    trace(GCR_DEBUG, "Encoding disk with %zd tracks\n", numTracks);

    // Wipe out track data
    clearDisk();

    // Assign track length
     for (Halftrack ht = 1; ht <= highestHalftrack; ht++)
         length.halftrack[ht] = trackLength[speedZoneOfHalftrack(ht)];
    
    // Encode tracks
    HeadPos start;
    for (Track t = 1; t <= numTracks; t++) {
        
        auto zone = speedZoneOfTrack(t);
        if (alignTracks) {
            start = (HeadPos)(length.track[t][0] * trackDefaults[t].stagger);
        } else {
            start = 0;
        }
        encodeTrack(fs, t, tailGap[zone], start);
    }

    // Do some consistency checking
    for (Halftrack ht = 1; ht <= highestHalftrack; ht++) {
        assert(length.halftrack[ht] >= 0);
        assert(length.halftrack[ht] <= sizeof(data.halftrack[ht]) * 8);
    }
}

isize
Disk::encodeTrack(const FSDevice &fs, Track t, isize gap, HeadPos start)
{
    assert(isTrackNumber(t));
    trace(GCR_DEBUG, "Encoding track %zd\n", t);

    isize totalEncodedBits = 0;
    
    // For each sector in this track ...
    for (Sector s = 0; s < trackDefaults[t].sectors; s++) {
        
        isize encodedBits = encodeSector(fs, t, s, start, gap);
        start += (HeadPos)encodedBits;
        totalEncodedBits += encodedBits;
    }
    
    return totalEncodedBits;
}

isize
Disk::encodeSector(const FSDevice &fs, Track t, Sector s, HeadPos start, isize tailGap)
{
    assert(isValidTrackSectorPair(t, s));
    
    TSLink ts = TSLink{t,s};
    
    HeadPos offset = start;
    u8 errorCode = fs.getErrorCode(ts);
        
    trace(GCR_DEBUG, "  Encoding track/sector %zd/%zd\n", t, s);
    
    // Get disk id and compute checksum
    u8 id1 = fs.diskId1();
    u8 id2 = fs.diskId2();
    u8 checksum = (u8)(id1 ^ id2 ^ t ^ s); // Header checksum byte
    
    // SYNC (0xFF 0xFF 0xFF 0xFF 0xFF)
    if (errorCode == 0x3) {
        writeBitToTrack(t, offset, 0, 40); // NO_SYNC SEQUENCE_ERROR
    } else {
        writeBitToTrack(t, offset, 1, 40);
    }
    offset += 40;
    
    // Header ID
    if (errorCode == 0x2) {
        encodeGcr(0x00, t, offset); // HEADER_BLOCK_NOT_FOUND_ERROR
    } else {
        encodeGcr(0x08, t, offset);
    }
    offset += 10;
 
    // Checksum
    if (errorCode == 0x9) {
        encodeGcr(checksum ^ 0xFF, t, offset); // HEADER_BLOCK_CHECKSUM_ERROR
    } else {
        encodeGcr(checksum, t, offset);
    }
    offset += 10;
    
    // Sector and track number
    encodeGcr((u8)s, t, offset);
    offset += 10;
    encodeGcr((u8)t, t, offset);
    offset += 10;
    
    // Disk ID (two bytes)
     if (errorCode == 0xB) {
         encodeGcr(id2 ^ 0xFF, t, offset); // DISK_ID_MISMATCH_ERROR
         offset += 10;
         encodeGcr(id1 ^ 0xFF, t, offset); // DISK_ID_MISMATCH_ERROR
     } else {
         encodeGcr(id2, t, offset);
         offset += 10;
         encodeGcr(id1, t, offset);
     }
    offset += 10;
    
    // 0x0F, 0x0F
    encodeGcr(0x0F, t, offset);
    offset += 10;
    encodeGcr(0x0F, t, offset);
    offset += 10;

    
    // 0x55 0x55 0x55 0x55 0x55 0x55 0x55 0x55 0x55
    writeGapToTrack(t, offset, 9);
    offset += 9 * 8;
    
    // SYNC (0xFF 0xFF 0xFF 0xFF 0xFF)
    if (errorCode == 3) {
        writeBitToTrack(t, offset, 0, 40); // NO_SYNC_SEQUENCE_ERROR
    } else {
        writeBitToTrack(t, offset, 1, 40);
    }
    offset += 40;
    
    // Data ID
    if (errorCode == 0x4) {
        // The error value is important here:
        // (1) If the first GCR bit equals 0, the sector can still be read.
        // (2) If the first GCR bit equals 1, the SYNC sequence continues.
        //     In this case, the bit sequence gets out of sync and the data
        //     can't be read.
        // Hoxs64 and VICE 3.2 write 0x00 which results in option (1)
        encodeGcr(0x00, t, offset); // DATA_BLOCK_NOT_FOUND_ERROR
    } else {
        encodeGcr(0x07, t, offset);
    }
    offset += 10;
    
    // Data bytes
    checksum = 0;
    for (isize i = 0; i < 256; i++, offset += 10) {
        u8 byte = fs.readByte(ts, (u32)i);
        checksum ^= byte;
        encodeGcr(byte, t, offset);
    }
    
    // Checksum
    if (errorCode == 0x5) {
        encodeGcr(checksum ^ 0xFF, t, offset); // DATA_BLOCK_CHECKSUM_ERROR
    } else {
        encodeGcr(checksum, t, offset);
    }
    offset += 10;
    
    // 0x00, 0x00
    encodeGcr(0x00, t, offset);
    offset += 10;
    encodeGcr(0x00, t, offset);
    offset += 10;
    
    // Tail gap (0x55 0x55 ... 0x55)
    writeGapToTrack(t, offset, tailGap);
    offset += tailGap * 8;
    
    // Return the number of encoded bits
    return offset - start;
}
