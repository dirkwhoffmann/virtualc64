/*
 * Written 2015 by Dirk W. Hoffmann
 *
 * This program is free software; you can redistribute it and/or modify
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
#include "Disk525.h"

Disk525::Disk525()
{
    // Create inverse GCR lookup table
    memset(invgcr, 0, sizeof(invgcr));
    for (unsigned i = 0; i < 16; i++)
        invgcr[gcr[i]] = i;

    clearDisk();
}

Disk525::~Disk525()
{
}

void
Disk525::reset(C64 *c64)
{
    clearDisk();
}

uint32_t
Disk525::stateSize()
{
    return sizeof(length.track) + sizeof(data.track) + 1;
}

void
Disk525::loadFromBuffer(uint8_t **buffer)
{
    uint8_t *old = *buffer;
    
    // Disk data storage
    readBlock(buffer, data.track[0], sizeof(data.track));
    readBlock16(buffer, length.track[0], sizeof(length.track));
    numTracks = read8(buffer);
    
    assert(*buffer - old == stateSize());
}

void
Disk525::saveToBuffer(uint8_t **buffer)
{
    uint8_t *old = *buffer;
    
    // Disk data storage
    writeBlock(buffer, data.track[0], sizeof(data.track));
    writeBlock16(buffer, length.track[0], sizeof(length.track));
    write8(buffer, numTracks);
    
    assert(*buffer - old == stateSize());
}

void
Disk525::dumpState()
{
    unsigned noOfOneBits, alignedSyncs, unalignedSyncs;
    
    msg("5,25\" floppy disk\n");
    msg("-----------------\n\n");

    for (unsigned track = 1; track <= 42; track++) {
        assert(isTrackNumber(track));
        noOfOneBits = alignedSyncs = unalignedSyncs = 0;
        for (unsigned offset = 0; offset < length.track[track][0]; offset++) {
            for (uint8_t bit = 0x80; bit != 0x00; bit >>= 1) {
                if (data.track[track][offset] & bit) {
                    noOfOneBits++;
                } else {
                    if (noOfOneBits >= 10) { // SYNC FOUND
                        if (bit == 0x80) {
                            alignedSyncs++;
                        } else {
                            unalignedSyncs++;
                        }
                    }
                    noOfOneBits = 0;
                }
            }
        }
        // Note: If a sync marks wraps the array bound, it is not detected
        msg("Track %2d: Length: %d bytes %d SYNC sequences found (%d are byte aligned)\n",
            track, length.track[track][0], alignedSyncs + unalignedSyncs, alignedSyncs);
    }
    msg("\n");
}

void
Disk525::dumpHalftrack(Halftrack ht, unsigned min, unsigned max, unsigned highlight)
{
    assert(isHalftrackNumber(ht));
    
    if (max > length.halftrack[ht]) max = length.halftrack[ht];
    
    msg("Dumping track %d (length = %d)\n", ht, length.halftrack[ht]);
    for (unsigned i = min; i < max; i++) {
        msg(i == highlight ? "(%02X) " : "%02X ", data.halftrack[ht][i]);
    }
    msg("\n");
}

void
Disk525::clearDisk()
{
    for (Halftrack ht = 1; ht <= 84; ht++) {
        clearHalftrack(ht);
        length.halftrack[ht] = sizeof(data.halftrack[ht]);
    }
}

void
Disk525::clearHalftrack(Halftrack ht)
{
    assert(isHalftrackNumber(ht));
    memset(data.halftrack[ht], 0x55, sizeof(data.halftrack[ht]));
}


// ---------------------------------------------------------------------------------------------
//                               Data encoding and decoding
// ---------------------------------------------------------------------------------------------


void
Disk525::encodeArchive(D64Archive *a)
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

    unsigned track, encodedBytes;
    
    assert(a != NULL);
    
    clearDisk();
    numTracks = a->numberOfTracks();
    
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
        encodedBytes = encodeTrack(a, track, zone4, 9, 10);
    }
    
    // Clear remaining tracks (if any)
    for (track = numTracks + 1; track <= 42; track++) {
        length.track[track][0] = encodedBytes; // Track t
        length.track[track][1] = encodedBytes; // Half track above
    }
    
    for (Halftrack ht = 1; ht <= 84; ht++) {
        assert(length.halftrack[ht] <= sizeof(data.halftrack[ht]));
    }
}

unsigned
Disk525::encodeTrack(D64Archive *a, Track t, int *sectorList, uint8_t tailGapEven, uint8_t tailGapOdd)
{
    assert(isTrackNumber(t));

    unsigned encodedBytes, totalEncodedBytes = 0;
    uint8_t *dest = data.track[t];
    
    debug(3, "Encoding track %d\n", t);
    
    // Scan the interleave pattern and encode each sector
    for (unsigned i = 0; sectorList[i] != -1; i++) {
        
        encodedBytes = encodeSector(a, t, sectorList[i], dest, (i % 2) ? tailGapOdd : tailGapEven);
        dest += encodedBytes;
        totalEncodedBytes += encodedBytes;
    }
    
    length.track[t][0] = totalEncodedBytes; // Track t
    length.track[t][1] = totalEncodedBytes; // Half track above
    
    return totalEncodedBytes;
}

unsigned
Disk525::encodeSector(D64Archive *a, Track t, uint8_t sector, uint8_t *dest, int gap)
{
    uint8_t *source, *ptr = dest;
    
    assert(isTrackNumber(t));
    assert(a != NULL);
    assert(ptr != NULL);
    
    // Get source address from archive
    if ((source = a->findSector(t, sector)) == 0) {
        warn("Can't find halftrack data in archive\n");
        return 0;
    }
    
    debug(4, "  Encoding track/sector %d/%d\n", t, sector);
    
    // Get disk id and compute checksum
    uint8_t id_lo = a->diskIdLow();
    uint8_t id_hi = a->diskIdHi();
    uint8_t checksum = id_lo ^ id_hi ^ t ^ sector; // Header checksum byte
    
    encodeSync(ptr); // 0xFF 0xFF 0xFF 0xFF 0xFF
    ptr += 5;
    encodeGcr(0x08, checksum, sector, t, ptr); // header block ID, checksum, sector and track
    ptr += 5;
    encodeGcr(id_lo, id_hi, 0x0F, 0x0F, ptr); // Sector ID (LO/HI), 0x0F, 0x0F
    ptr += 5;
    encodeGap(ptr, 9); // 0x55 0x55 0x55 0x55 0x55 0x55 0x55 0x55 0x55
    ptr += 9;
    encodeSync(ptr); // 0xFF 0xFF 0xFF 0xFF 0xFF
    ptr += 5;
    
    checksum = source[0];
    for (unsigned i = 1; i < 256; i++) // Data checksum byte
        checksum ^= source[i];
    
    encodeGcr(0x07, source[0], source[1], source[2], ptr); // data block ID, first three data bytes
    ptr += 5;
    for (unsigned i = 3; i < 255; i += 4, ptr += 5)
        encodeGcr(source[i], source[i+1], source[i+2], source[i+3], ptr); // Data chunks
    encodeGcr(source[255], checksum, 0, 0, ptr); // Last byte, checksum, 0x00, 0x00
    ptr += 5;
    
    assert(ptr - dest == 354);
    
    encodeGap(ptr, gap); // 0x55 0x55 ... 0x55 (tail gap)
    ptr += gap;
    
    // Return number of encoded bytes
    return ptr - dest;
}

void
Disk525::encodeGcr(uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4, uint8_t *dest)
{
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
    dest[4] = shift_reg & 0xFF; shift_reg >>= 8;
    dest[3] = shift_reg & 0xFF; shift_reg >>= 8;
    dest[2] = shift_reg & 0xFF; shift_reg >>= 8;
    dest[1] = shift_reg & 0xFF; shift_reg >>= 8;
    dest[0] = shift_reg & 0xFF;
}

unsigned
Disk525::decodeDisk(uint8_t *dest, int *error)
{
    Track t;
    Halftrack halftrack;
    unsigned numBytes = 0;
    uint8_t tmpbuf[2 * 7928];
    
    if (error) *error = 0; // We assume the best
    
    // For each full track ...
    for (t = 1, halftrack = 0; t <= numTracks; t++, halftrack += 2) {
        
        debug(3, "Decoding track %d %s\n", t, dest == NULL ? "(test run)" : "");
        
        // Copy track into temporary buffer
        // Buffer is double sized, so we can read safely beyond the array bounds
        assert(2 * length.track[t][0] < sizeof(tmpbuf));
        memcpy(tmpbuf, data.track[t], length.track[t][0]);
        memcpy(tmpbuf + length.track[t][0], data.track[t], length.track[t][0]);
        
        if (dest)
            numBytes += decodeTrack(tmpbuf, dest + numBytes, error);
        else
            numBytes += decodeTrack(tmpbuf, NULL, error);
        
    }
    return numBytes;
}

unsigned
Disk525::decodeTrack(uint8_t *source, uint8_t *dest, int *error)
{
    
    unsigned numBytes = 0;
    int sectorID = -1, sectorStart[21]; // A track can contain up to 21 sectors
    
    // Initialize offset table
    for (unsigned i = 0; i < 21; sectorStart[i++] = -1);
    
    // Collect start addresses of all sectors in this track
    for (unsigned i = 2; i < 7928 + 512; i++) {
        
        if (source[i-2] != 0xFF || source[i-1] != 0xFF || source[i] == 0xFF)
            continue;
        
        uint8_t data[4];
        decodeGcr(source[i], source[i+1], source[i+2], source[i+3], source[i+4], data);
        
        // We found: |  0xFF  |  0xFF  | !0xFF   |
        //                             | data[0] | data[1] | data[2] | data[3] |
        //                                $08 => Header block
        //                                $07 => Data block
        
        
        if (data[0] == 0x08) { // Header block
            
            // databyte[1] = header block checksum
            // databyte[2] = sector number
            // databyte[3] = track number
            
            sectorID = data[2];
            debug(2, "Found header block (%d/%d)\n", data[3], data[2], data[1]);
            
        } else if (data[0] == 0x07) { // Data block
            
            if (sectorID == -1) {
                // we need to see the header block, first
                continue;
            }
            
            if (sectorID < 0 || sectorID > 20) {
                warn("Skipping sector %d. Sector number of range (0 - 20).\n", sectorID);
                if (error) *error = 1;
                continue;
            }
            
            debug("Found data block for sector %d at offset %d\n", sectorID, i);
            sectorStart[sectorID] = i;
            
        } else {
            warn("Skipping sector %d. Unknown header ID (expected $07 or $08, found $%02X).\n", sectorID, sectorID);
            if (error) *error = 1;
        }
    }
    
    // Decode all sectors that have been found
    for (unsigned i = 0; i < 21 && sectorStart[i] != -1; i++, numBytes += 256) {
        debug(2, "   Decoding sector %d\n", i);
        if (dest) {
            decodeSector(source + sectorStart[i], dest);
            dest += 256;
        }
    }
    
    return numBytes;
}

void
Disk525::decodeSector(uint8_t *source, uint8_t *dest)
{
    uint8_t databytes[4];
    uint8_t *ptr = dest;
    
    if (dest == NULL) return;
    
    // Decode the first three data bytes
    decodeGcr(source[0], source[1], source[2], source[3], source[4], databytes);
    assert(databytes[0] == 0x07);
    
    *(ptr++) = databytes[1];
    *(ptr++) = databytes[2];
    *(ptr++) = databytes[3];
    source += 5;
    
    // Keep on going
    for (unsigned i = 3; i < 255; i += 4, source += 5, ptr += 4) {
        decodeGcr(source[0], source[1], source[2], source[3], source[4], ptr);
    }
    
    // Decode the last byte
    decodeGcr(source[0], source[1], source[2], source[3], source[4], databytes);
    *(ptr++) = databytes[0];
    
    debug(2, "%d bytes written.\n", ptr-dest);
    assert(ptr - dest == 256);
}

void
Disk525::decodeGcr(uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4, uint8_t b5, uint8_t *dest)
{
    uint64_t shift_reg;
    
    // Shift in
    shift_reg = b1;
    shift_reg = (shift_reg << 8) | b2;
    shift_reg = (shift_reg << 8) | b3;
    shift_reg = (shift_reg << 8) | b4;
    shift_reg = (shift_reg << 8) | b5;
    
    // Shift out
    dest[3] = invgcr[shift_reg & 0x1F]; shift_reg >>= 5;
    dest[3] |= (invgcr[shift_reg & 0x1F] << 4); shift_reg >>= 5;
    dest[2] = invgcr[shift_reg & 0x1F]; shift_reg >>= 5;
    dest[2] |= (invgcr[shift_reg & 0x1F] << 4); shift_reg >>= 5;
    dest[1] = invgcr[shift_reg & 0x1F]; shift_reg >>= 5;
    dest[1] |= (invgcr[shift_reg & 0x1F] << 4); shift_reg >>= 5;
    dest[0] = invgcr[shift_reg & 0x1F]; shift_reg >>= 5;
    dest[0] |= (invgcr[shift_reg & 0x1F] << 4);
}





