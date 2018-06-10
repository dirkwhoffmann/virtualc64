/*!
 * @file        Disk.cpp
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   2015 - 2018 Dirk W. Hoffmann
 */
/* This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "basic.h"
#include "Disk.h"
#include "D64Archive.h"
#include "G64Archive.h"
#include "NIBArchive.h"

Disk::Disk()
{
    setDescription("Disk525");

    // Register snapshot items
    SnapshotItem items[] = {        
        { data.track[0],    sizeof(data.track),     KEEP_ON_RESET },
        { length.track[0],  sizeof(length.track),   KEEP_ON_RESET | WORD_FORMAT },
        // { &numTracks,       sizeof(numTracks),      KEEP_ON_RESET },
        { &writeProtected,  sizeof(writeProtected), KEEP_ON_RESET },
        { &modified,        sizeof(modified),       KEEP_ON_RESET },
        { NULL,             0,                      0 }};
    
    registerSnapshotItems(items, sizeof(items));

    // Create bit expansion table
    // Note that this table expects a LITTLE ENDIAN architecture to work. If you compile
    // the emulator on a BIG ENDIAN architecture, the byte order needs to be reversed.
    for (unsigned i = 0; i < 256; i++) {
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

Disk::~Disk()
{
}

void
Disk::dumpState()
{
    unsigned noOfOneBits, alignedSyncs, unalignedSyncs;
    uint8_t bit;
    
    msg("Floppy disk\n");
    msg("-----------\n\n");

    /*
    for (unsigned track = 1; track <= 42; track++) {
        assert(isTrackNumber(track));
        noOfOneBits = alignedSyncs = unalignedSyncs = 0;
        for (unsigned offset = 0; offset < length.track[track][0]; offset++) {
            if ((bit = readBitFromTrack(track, offset))) {
                noOfOneBits++;
            } else {
                if (noOfOneBits >= 10) { // SYNC FOUND
                    if (offset % 8 == 0) {
                        alignedSyncs++;
                    } else {
                        unalignedSyncs++;
                    }
                }
                noOfOneBits = 0;
            }
        }
        
        // Note: If a sync marks wraps the array bound, it is not detected
        msg("Track %2d: Length: %d bits %d SYNC sequences found (%d are byte aligned)\n",
            track, length.track[track][0], alignedSyncs + unalignedSyncs, alignedSyncs);
    }
    msg("\n");
    */
    for (unsigned i = 0; i < maxNumberOfSectors; i++) {
        debug("Sector %d: header: %d - %d, data: %d - %d",
              i,
              trackInfo.sectorInfo[i].headerBegin,
              trackInfo.sectorInfo[i].headerEnd,
              trackInfo.sectorInfo[i].dataBegin,
              trackInfo.sectorInfo[i].dataEnd);
    }
}

/*
void
Disk::dumpHalftrack(Halftrack ht, unsigned min, unsigned max, unsigned highlight)
{
    assert(isHalftrackNumber(ht));
    
    uint16_t bytesOnTrack = length.halftrack[ht] / 8;
    
    if (max > bytesOnTrack) max = bytesOnTrack;
    
    msg("Dumping track %d (length = %d)\n", ht, bytesOnTrack);
    for (unsigned i = min; i < max; i++) {
        msg(i == highlight ? "(%02X) " : "%02X ", data.halftrack[ht][i]);
    }
    msg("\n");
}
*/

void
Disk::encodeGcr(uint8_t value, uint8_t *gcrBits)
{
    assert(gcrBits != NULL);

    uint8_t nibble1 = bin2gcr(value >> 4);
    uint8_t nibble2 = bin2gcr(value & 0xF);
    
    gcrBits[0] = (nibble1 & 0x10) ? 1 : 0;
    gcrBits[1] = (nibble1 & 0x08) ? 1 : 0;
    gcrBits[2] = (nibble1 & 0x04) ? 1 : 0;
    gcrBits[3] = (nibble1 & 0x02) ? 1 : 0;
    gcrBits[4] = (nibble1 & 0x01) ? 1 : 0;

    gcrBits[5] = (nibble2 & 0x10) ? 1 : 0;
    gcrBits[6] = (nibble2 & 0x08) ? 1 : 0;
    gcrBits[7] = (nibble2 & 0x04) ? 1 : 0;
    gcrBits[8] = (nibble2 & 0x02) ? 1 : 0;
    gcrBits[9] = (nibble2 & 0x01) ? 1 : 0;
}

uint8_t
Disk::decodeGcr(uint8_t *gcr)
{
    assert(gcr != NULL);
    
    uint5_t gcr1 = (gcr[0] << 4) | (gcr[1] << 3) | (gcr[2] << 2) | (gcr[3] << 1) | gcr[4];
    uint5_t gcr2 = (gcr[5] << 4) | (gcr[6] << 3) | (gcr[7] << 2) | (gcr[8] << 1) | gcr[9];

    assert(is_uint5_t(gcr1));
    assert(is_uint5_t(gcr2));
    
    return (invgcr[gcr1] << 4) | invgcr[gcr2];
}

void
Disk::clearDisk()
{
    for (Halftrack ht = 1; ht <= 84; ht++) {
        clearHalftrack(ht);
        length.halftrack[ht] = sizeof(data.halftrack[ht]) * 8;
    }
    writeProtected = false;
    modified = false; 
}

void
Disk::clearTrack(Track t)
{
    assert(isTrackNumber(t));
    memset(data.track[t], 0x55, sizeof(data.track[t]));
}

void
Disk::clearHalftrack(Halftrack ht)
{
    assert(isHalftrackNumber(ht));
    memset(data.halftrack[ht], 0x55, sizeof(data.halftrack[ht]));
}

bool
Disk::trackIsEmpty(Track t)
{
    assert(isTrackNumber(t));
    for (unsigned i = 0; i < sizeof(data.track[t]); i++)
        if (data.track[t][i] != 0x55) return false;
    return true;
}

bool
Disk::halftrackIsEmpty(Halftrack ht)
{
    assert(isHalftrackNumber(ht));
    for (unsigned i = 0; i < sizeof(data.halftrack[ht]); i++)
        if (data.halftrack[ht][i] != 0x55) return false;
    return true;
}

unsigned
Disk::nonemptyHalftracks()
{
    unsigned result = 0;
    
    for (unsigned ht = 1; ht < 85; ht++) {
        if (!halftrackIsEmpty(ht))
            result++;
    }
    
    return result;
}


/*
const char *
Disk::dataAbs(Halftrack ht, size_t start, size_t n)
{
    assert(isHalftrackNumber(ht));
    assert(n < sizeof(text));
    
    // We also accept negative values for 'start'
    start = (start + length.halftrack[ht]) % length.halftrack[ht];
    
    size_t i;
    for (i = 0; i < n; i++) {
        text[i] = readBitFromHalftrack(ht, start + i) ? '1' : '0';
    }
    text[i] = 0;
    return text;
}
*/

// ---------------------------------------------------------------------------------------------
//                               Data encoding and decoding
// ---------------------------------------------------------------------------------------------

void
Disk::encodeArchive(G64Archive *a)
{
    debug(2, "Encoding G64 archive\n");
    
    assert(a != NULL);

    clearDisk();
    for (Halftrack ht = 1; ht <= 84; ht++) {
        
        unsigned item = ht - 1;
        uint16_t size = a->getSizeOfItem(item);
        
        if (size == 0) {
            continue;
        }
        
        if (size > 7928) {
            debug(2, "Halftrack %d has %d bytes. Must be less than 7928\n", ht, size);
            continue;
        }
        debug(2, "  Encoding halftrack %d (%d bytes)\n", ht, size);
        length.halftrack[ht] = 8 * size;
        a->selectItem(item);
        for (unsigned i = 0; i < size; i++) {
            int b = a->getByte();
            assert(b != -1);
            data.halftrack[ht][i] = (uint8_t)b;
        }
        assert(a->getByte() == -1); /* check for EOF */
    }
}

void
Disk::encodeArchive(NIBArchive *a)
{
    debug(2, "Encoding NIB archive\n");
    
    assert(a != NULL);
    
    clearDisk();
    for (Halftrack ht = 1; ht <= 84; ht++) {
        
        size_t size = a->getSizeOfItem(ht - 1);

        if (size == 0) {
            continue;
        }
        
        if (size > 8 * 7928) {
            debug(2, "Halftrack %d has %d bits. Must be less than 8 * 7928\n", ht, size);
            size = 8 * 7928;
        }
        debug(2, "  Encoding halftrack %d (%d bits)\n", ht, size);
        length.halftrack[ht] = size;
        a->selectItem(ht - 1);
        size_t bytesTotal = (size + 7) / 8;
        for (unsigned i = 0; i < bytesTotal; i++) {
            int b = a->getByte();
            assert(b != -1);
            data.halftrack[ht][i] = (uint8_t)b;
        }
    }
}

void
Disk::encodeArchive(D64Archive *a)
{
    // Interleave patterns (no interleave)
    /*
    int zone1[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, -1 };
    int track18[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, -1 };
    int zone2[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, -1 };
    int zone3[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, -1 };
    int zone4[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, -1 };
    */
    
    // Interleave patterns (minics real VC1541 sector layout)
    int zone1[] = { 0, 10, 20, 9, 19, 8, 18, 7, 17, 6, 16, 5, 15, 4, 14, 3, 13, 2, 12, 1, 11, -1 };
    int track18[] = { 0, 3, 6, 9, 12, 15, 18, 2, 5, 8, 11, 14, 17, 1, 4, 7, 10, 13, 16, -1 };
    int zone2[] = { 0, 10, 1, 11, 2, 12, 3, 13, 4, 14, 5, 15, 6, 16, 7, 17, 8, 18, 9, -1 };
    int zone3[] = { 0, 10, 2, 12, 4, 14, 6, 16, 8, 1, 11, 3, 13, 5, 15, 7, 17, 9, -1 };
    int zone4[] = { 0, 10, 3, 13, 6, 16, 9, 2, 12, 5, 15, 8, 1, 11, 4, 14, 7, -1 };

    unsigned track, encodedBits;
    
    assert(a != NULL);
    
    clearDisk();
    unsigned numTracks = a->numberOfTracks();
    
    debug(2, "Encoding D64 archive with %d tracks\n", numTracks);
    
    // Zone 1: Tracks 1 - 17 (21 sectors, tailgap 9/9 (even/odd sectors))
    for (track = 1; track <= 17; track++) {
        (void)encodeTrack(a, track, zone1, 9, 9);
    }
    
    // Zone 2: Tracks 18 - 24 (19 sectors, tailgap 9/19 (even/odd sectors))
    (void)encodeTrack(a, track, track18, 9, 19); // Directory track
    for (track = 19; track <= 24; track++) {
        (void)encodeTrack(a, track, zone2, 9, 19);
    }
    
    // Zone 3: Tracks 25 - 30 (18 sectors, tailgap 9/13 (even/odd sectors))
    for (track = 25; track <= 30; track++) {
        (void)encodeTrack(a, track, zone3, 9, 13);
    }
    
    // Zone 4: Tracks 31 - 35..42 (17 sectors, tailgap 9/10 (even/odd sectors))
    for (track = 31; track <= a->numberOfTracks(); track++) {
        encodedBits = encodeTrack(a, track, zone4, 9, 10);
    }
    
    // Clear remaining tracks (if any)
    for (track = numTracks + 1; track <= 42; track++) {
        length.track[track][0] = encodedBits;  // Track t
        length.track[track][1] = encodedBits;  // Half track above
    }
    
    for (Halftrack ht = 1; ht <= 84; ht++) {
        assert(length.halftrack[ht] <= sizeof(data.halftrack[ht]) * 8);
    }    
}

unsigned
Disk::encodeTrack(D64Archive *a, Track t, int *sectorList, uint8_t tailGapEven, uint8_t tailGapOdd)
{
    assert(isTrackNumber(t));

    unsigned encodedBits, totalEncodedBits = 0;
    
    debug(3, "Encoding track %d\n", t);
    
    // Scan the interleave pattern and encode each sector
    for (unsigned i = 0; sectorList[i] != -1; i++) {
        
        encodedBits = encodeSector(a, t, sectorList[i], totalEncodedBits, (i % 2) ? tailGapOdd : tailGapEven);
        // assert(encodedBits % 8 == 0);
        totalEncodedBits += encodedBits;
    }

    length.track[t][0] = totalEncodedBits;  // Track t
    length.track[t][1] = totalEncodedBits;  // Half track above
    
    return totalEncodedBits;
}

unsigned
Disk::encodeSector(D64Archive *a, Track t, uint8_t sector, unsigned bitoffset, int gap)
{
    uint8_t *source;
    // uint8_t *dest = data.track[t];
    unsigned bitptr = bitoffset;
    
    assert(a != NULL);
    assert(isTrackNumber(t));
    
    // Get source address from archive
    if ((source = a->findSector(t, sector)) == 0) {
        warn("Can't find halftrack data in archive\n");
        return 0;
    }
    
    debug(4, "  Encoding track/sector %d/%d\n", t, sector);
    
    // Get disk id and compute checksum
    uint8_t id1 = a->diskId1();
    uint8_t id2 = a->diskId2();
    uint8_t checksum = id1 ^ id2 ^ t ^ sector; // Header checksum byte
    
    // 0xFF 0xFF 0xFF 0xFF 0xFF
    // writeSyncBits(dest, bitptr, 5 * 8);
    writeSyncBitsToTrack(t, bitptr, 5 * 8);
    bitptr += 5 * 8;
    
    // Header ID, Checksum, Sector number, Track number
    encodeGcr(0x08, checksum, sector, t, t, bitptr);
    bitptr += 5 * 8;
    
    // Disk ID 2, Disk ID 1, 0x0F, 0x0F
    encodeGcr(id2, id1, 0x0F, 0x0F, t, bitptr);
    bitptr += 5 * 8;
    
    // 0x55 0x55 0x55 0x55 0x55 0x55 0x55 0x55 0x55
    writeGapToTrack(t, bitptr, 9);
    bitptr += 9 * 8;
    
    // 0xFF 0xFF 0xFF 0xFF 0xFF
    writeSyncBitsToTrack(t, bitptr, 5 * 8);
    bitptr += 5 * 8;
    
    // Data ID, First three data bytes
    encodeGcr(0x07, source[0], source[1], source[2], t, bitptr);
    bitptr += 5 * 8;
    
    // Rest of the data bytes
    for (unsigned i = 3; i < 255; i += 4, bitptr += 5 * 8)
        encodeGcr(source[i], source[i+1], source[i+2], source[i+3], t, bitptr);
  
    // Last data byte, Checksum, 0x00, 0x00
    checksum = source[0];
    for (unsigned i = 1; i < 256; i++) {
        checksum ^= source[i];
    }
    encodeGcr(source[255], checksum, 0, 0, t, bitptr);
    bitptr += 5 * 8;
    
    // 0x55 0x55 ... 0x55 (Tail gap)
    writeGapToTrack(t, bitptr, gap);
    bitptr += gap * 8;
    
    // Return number of encoded bytes
    return bitptr - bitoffset;
}

void
Disk::encodeGcr(uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4, Track t, unsigned offset)
{
    assert(isTrackNumber(t));
    
    uint64_t shift_reg = 0;
    
    // Shift in
    shift_reg = gcr[b1 >> 4];
    shift_reg = (shift_reg << 5) | gcr[b1 & 0x0F];
    shift_reg = (shift_reg << 5) | gcr[b2 >> 4];
    shift_reg = (shift_reg << 5) | gcr[b2 & 0x0F];
    shift_reg = (shift_reg << 5) | gcr[b3 >> 4];
    shift_reg = (shift_reg << 5) | gcr[b3 & 0x0F];
    shift_reg = (shift_reg << 5) | gcr[b4 >> 4];
    shift_reg = (shift_reg << 5) | gcr[b4 & 0x0F];
    
    // Shift out
    writeByteToTrack(t, offset + 4 * 8, shift_reg & 0xFF); shift_reg >>= 8;
    writeByteToTrack(t, offset + 3 * 8, shift_reg & 0xFF); shift_reg >>= 8;
    writeByteToTrack(t, offset + 2 * 8, shift_reg & 0xFF); shift_reg >>= 8;
    writeByteToTrack(t, offset + 1 * 8, shift_reg & 0xFF); shift_reg >>= 8;
    writeByteToTrack(t, offset, shift_reg & 0xFF);
}

void
Disk::analyzeTrack(Track t)
{
    assert(isTrackNumber(t));
    _analyzeTrack(data.track[t], length.track[t][0]);
}

void
Disk::analyzeHalftrack(Halftrack ht)
{
    assert(isHalftrackNumber(ht));
    _analyzeTrack(data.halftrack[ht], length.halftrack[ht]);
}

void
Disk::_analyzeTrack(uint8_t *data, uint16_t length)
{
    // The result of the analysis is stored in variable trackInfo.
    memset(&trackInfo, 0, sizeof(trackInfo));
    trackInfo.length = length;
    
    // Setup working buffer (two copies of the track, each bit represented by one byte).
    for (unsigned i = 0; i < maxBytesOnTrack; i++)
        trackInfo.byte[i] = bitExpansion[data[i]];
    memcpy(trackInfo.bit + length, trackInfo.bit, length);
    
    // Indicates where the sector headers blocks and the sectors data blocks start.
    uint8_t sync[sizeof(trackInfo.bit)];
    memset(sync, 0, sizeof(sync));
    
    // Scan for SYNC sequences and decode the byte that follows.
    unsigned noOfOnes = 0;
    for (unsigned i = 0; i < 2 * length - 10; i++) {
        
        assert(trackInfo.bit[i] <= 1);
        if (trackInfo.bit[i] == 0 && noOfOnes >= 10) {
            
            // <--- SYNC ---><-- sync[i] -->
            // 11111 .... 1110
            //               ^ <- We are at offset i which is here
            sync[i] = decodeGcr(trackInfo.bit + i);
            
            if (sync[i] == 0x08) {
                debug(1, "Sector header block found at offset %d\n", i);
            } else if (sync[i] == 0x07) {
                debug(1, "Sector data block found at offset %d\n", i);
            } else {
                warn("Unknown sector ID (%d) found at index %d", sync[i], i);
            }
        }
        noOfOnes = trackInfo.bit[i] ? (noOfOnes + 1) : 0;
    }
    
    // Lookup first sector header block
    unsigned startOffset;
    for (startOffset = 0; startOffset < length; startOffset++) {
        if (sync[startOffset] == 0x08) {
            break;
        }
    }
    if (startOffset == length) {
        debug(2, "Track contains no sector header block.");
        return;
    }
    
    // Compute offsets for all sectors
    uint8_t sector;
    for (unsigned i = startOffset; i < startOffset + length; i++) {
        
        if (sync[i] == 0x08) {
            
            sector = decodeGcr(trackInfo.bit + i + 20);
            if (isSectorNumber(sector)) {
                if (trackInfo.sectorInfo[sector].headerEnd != 0)
                    break; // We've seen this sector already, so we are done.
                trackInfo.sectorInfo[sector].headerBegin = i;
                trackInfo.sectorInfo[sector].headerEnd = i + headerBlockSize;
            } else {
                warn("Invalid sector number (%d) in sector header block.\n", sector);
            }
        
        } else if (sync[i] == 0x07) {
            
            if (isSectorNumber(sector)) {
                trackInfo.sectorInfo[sector].dataBegin = i;
                trackInfo.sectorInfo[sector].dataEnd = i + dataBlockSize;
            } else {
                warn("Invalid sector number (%d) in sector data block.\n", sector);
            }
        }
    }
}

const char *
Disk::trackDataAsString()
{
    size_t i;
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
Disk::sectorHeaderAsString(Sector nr)
{
    assert(isSectorNumber(nr));
    size_t begin = trackInfo.sectorInfo[nr].headerBegin;
    size_t end = trackInfo.sectorInfo[nr].headerEnd;
    return (begin == end) ? "" : sectorBytesAsString(trackInfo.bit + begin, 10);
}

const char *
Disk::sectorDataAsString(Sector nr)
{
    assert(isSectorNumber(nr));
    size_t begin = trackInfo.sectorInfo[nr].dataBegin;
    size_t end = trackInfo.sectorInfo[nr].dataEnd;
    return (begin == end) ? "" : sectorBytesAsString(trackInfo.bit + begin, 256);
}

const char *
Disk::sectorBytesAsString(uint8_t *buffer, size_t length)
{
    size_t gcr_offset = 0;
    size_t str_offset = 0;
    
    for (size_t i = 0; i < length; i++, gcr_offset += 10, str_offset += 3) {
        uint8_t value = decodeGcr(buffer + gcr_offset);
        sprint8x(text + str_offset, value);
        text[str_offset + 2] = ' ';
    }
    text[str_offset] = 0;
    return text;
}

unsigned
Disk::decodeDisk(uint8_t *dest, int *error)
{
    unsigned numBytes = 0;
     if (error) *error = 0;
    
    // For each full track ...
    for (Track t = 1; t <= maxNumberOfTracks; t++) {
        
        if (trackIsEmpty(t))
            break;
        
        debug(2, "Decoding track %d %s\n", t, dest ? "" : "(test run)");
        numBytes += decodeTrack(t, dest + (dest ? numBytes : 0), error);
    }
    
    return numBytes;
}

unsigned
Disk::decodeTrack(Track t, uint8_t *dest, int *error)
{
    unsigned numBytes = 0;
    
    // Find sectors
    analyzeTrack(t);
    
    // For each sector ...
    for (unsigned s = 0; s < 21; s++) {

        debug(2, "   Decoding sector %d\n", s);
        SectorInfo info = sectorLayout(s);
        if (info.dataBegin != info.dataEnd) {
            numBytes += decodeSector(info.dataBegin, dest + (dest ? numBytes : 0), error);
        }
    }
    
    return numBytes;
}

unsigned
Disk::decodeSector(size_t offset, uint8_t *dest, int *error)
{
    // The first byte must be 0x07 (indicating a data block)
    assert(decodeGcr(trackInfo.bit + offset) == 0x07);
    offset += 10;
    
    if (dest) {
        for (unsigned i = 0; i < 256; i++) {
            dest[i] = decodeGcr(trackInfo.bit + offset);
            offset += 10;
        }
    }
    
    return 256;
}






