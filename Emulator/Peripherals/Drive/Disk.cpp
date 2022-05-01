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
#include "IOUtils.h"
#include "Checksum.h"

#include <stdarg.h>

const TrackDefaults Disk::trackDefaults[43] = {
    
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
    
    auto fs = FileSystem(path);
    init(fs, wp);
}

void
Disk::init(DOSType type, PETName<16> name, bool wp)
{
    assert_enum(DOSType, type);
    
    if (type == DOS_TYPE_CBM) {
        
        auto fs = FileSystem(DISK_TYPE_SS_SD, DOS_TYPE_CBM);
        fs.setName(name);
        init(fs, wp);

    } else {

        clearDisk();
    }
}

void
Disk::init(const FileSystem &fs, bool wp)
{
    clearDisk();
    encode(fs);
    setWriteProtection(wp);
}

void
Disk::init(const G64File &g64, bool wp)
{
    clearDisk();
    encodeG64(g64);
    setWriteProtection(wp);
}

void
Disk::init(const D64File &d64, bool wp)
{
    auto fs = FileSystem(d64);
    init(fs, wp);
}

void
Disk::init(AnyCollection &collection, bool wp)
{
    auto fs = FileSystem(collection);
    init(fs, wp);
}

void
Disk::init(util::SerReader &reader)
{
    applyToPersistentItems(reader);
}

void
Disk::_dump(Category category, std::ostream& os) const
{
    using namespace util;
    
    if (category == Category::State) {

        auto checksum = util::fnv32((const u8 *)data.track, sizeof(data.track));

        os << tab("Write protected") << bol(writeProtected) << std::endl;
        os << tab("Modified") << bol(modified) << std::endl;
        os << tab("Checksum") << hex(checksum) << std::endl;
    }

    if (category == Category::Layout) {

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

bool
Disk::isValidHeadPos(Halftrack ht, HeadPos pos) const
{
    return isHalftrackNumber(ht) && pos >= 0 && pos < length.halftrack[ht];
}

HeadPos
Disk::wrap(Halftrack ht, HeadPos pos) const
{
    auto len = length.halftrack[ht];
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

isize
Disk::lengthOfTrack(Track t) const
{
    assert(isTrackNumber(t));
    return length.track[t][0];
}

isize
Disk::lengthOfHalftrack(Halftrack ht) const
{
    assert(isHalftrackNumber(ht));
    return length.halftrack[ht];
}


//
// Decoding disk data
//

isize
Disk::decodeDisk(u8 *dest)
{
    // Analyze the GCR bit stream
    DiskAnalyzer analyzer(*this);
    
    // Determine highest non-empty track
    Track t = 42;
    while (t > 0 && trackIsEmpty(t)) t--;
    
    // Decode disk 
    if (t <= 35)
        return decodeDisk(dest, 35, analyzer);
        
    if (t <= 40)
        return decodeDisk(dest, 40, analyzer);
    
    return decodeDisk(dest, 42, analyzer);
}

isize
Disk::decodeDisk(u8 *dest, isize numTracks, DiskAnalyzer &analyzer)
{
    isize numBytes = 0;

    assert(numTracks == 35 || numTracks == 40 || numTracks == 42);

    // For each full track ...
   for (Track t = 1; t <= numTracks; t++) {
        
        if (trackIsEmpty(t))
            break;
        
       trace(GCR_DEBUG, "Decoding track %ld %s\n", t, dest ? "" : "(test run)");
        numBytes += decodeTrack(t, dest + (dest ? numBytes : 0), analyzer);
    }
    
    return numBytes;
}

isize
Disk::decodeTrack(Track t, u8 *dest, DiskAnalyzer &analyzer)
{
    assert(isTrackNumber(t));
    
    return decodeHalfrack(2 * t - 1, dest, analyzer);
}

isize
Disk::decodeHalfrack(Halftrack ht, u8 *dest, DiskAnalyzer &analyzer)
{
    assert(isHalftrackNumber(ht));
    
    isize numBytes = 0;
    isize numSectors = numberOfSectorsInHalftrack(ht);

    // For each sector ...
    for (Sector s = 0; s < numSectors; s++) {
        
        trace(GCR_DEBUG, "   Decoding sector %ld\n", s);
        SectorInfo info = analyzer.sectorLayout(ht, s);
        if (info.dataBegin != info.dataEnd) {
            numBytes += decodeSector(ht, info.dataBegin, dest + (dest ? numBytes : 0), analyzer);
        } else {

            // The decoder failed to decode this sector.
            break;
        }
    }
    
    return numBytes;
}

isize
Disk::decodeSector(Halftrack ht, isize offset, u8 *dest, DiskAnalyzer &analyzer)
{
    // The first byte must be 0x07 (indicating a data block)
    assert(analyzer.decodeGcr(ht, offset) == 0x07);
    offset += 10;
    
    if (dest) {
        for (isize i = 0; i < 256; i++) {
            dest[i] = analyzer.decodeGcr(ht, offset);
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
            warn("Halftrack %ld has %ld bytes. Must be less than 7928\n", ht, size);
            continue;
        }
        trace(GCR_DEBUG, "  Encoding halftrack %ld (%ld bytes)\n", ht, size);
        length.halftrack[ht] = (u16)(8 * size);
        
        a.copyHalftrack(ht, data.halftrack[ht]);
    }
}

void
Disk::encode(const FileSystem &fs, bool alignTracks)
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

    trace(GCR_DEBUG, "Encoding disk with %ld tracks\n", numTracks);

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
        assert(length.halftrack[ht] <= isizeof(data.halftrack[ht]) * 8);
    }
}

isize
Disk::encodeTrack(const FileSystem &fs, Track t, isize gap, HeadPos start)
{
    assert(isTrackNumber(t));
    trace(GCR_DEBUG, "Encoding track %ld\n", t);

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
Disk::encodeSector(const FileSystem &fs, Track t, Sector s, HeadPos start, isize tailGap)
{
    assert(isValidTrackSectorPair(t, s));
    
    TSLink ts = TSLink{t,s};
    
    HeadPos offset = start;
    u8 errorCode = fs.getErrorCode(ts);
        
    trace(GCR_DEBUG, "  Encoding track/sector %ld/%ld\n", t, s);
    
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
