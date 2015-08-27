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
    reset(NULL);
}

Disk525::~Disk525()
{
}

void
Disk525::reset(C64 *c64)
{
    // Disk properties
    clearDisk();
}

uint32_t
Disk525::stateSize()
{
    return sizeof(data) + sizeof(length) + 1;
}

void
Disk525::loadFromBuffer(uint8_t **buffer)
{
    uint8_t *old = *buffer;
    
    // Disk data storage
    readBlock(buffer, data[0], sizeof(data));
    readBlock16(buffer, length, sizeof(length));
    numTracks = read8(buffer);
    
    assert(*buffer - old == stateSize());
}

void
Disk525::saveToBuffer(uint8_t **buffer)
{
    uint8_t *old = *buffer;
    
    // Disk data storage
    writeBlock(buffer, data[0], sizeof(data));
    writeBlock16(buffer, length, sizeof(length));
    write8(buffer, numTracks);
    
    assert(*buffer - old == stateSize());
}

void
Disk525::clearHalftrack(Halftrack ht)
{
    assert(isHalftrackNumber(ht));
    
    // TODO:
    // debug(2, "Clearing halftrack %d with %d 0x55 bytes\n");
    debug(2, "Clearing halftrack %d\n");
    memset(startOfHalftrack(ht), 0x55, 7928);
}

void
Disk525::clearDisk()
{
    for (unsigned i = 1; i <= 84; i++) {
        clearHalftrack(i);
        setLengthOfHalftrack(i, 7928);
    }
}


// ---------------------------------------------------------------------------------------------
//                               Data encoding and decoding
// ---------------------------------------------------------------------------------------------


void
Disk525::encodeDisk(D64Archive *a)
{
    // Interleave patterns (no interleave for now)
    int zone1[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, -1 };
    int zone2[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, -1 };
    int zone3[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, -1 };
    int zone4[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, -1 };
    
    unsigned track, encodedBytes;
    
    assert(a != NULL);
    
    clearDisk();
    numTracks = a->numberOfTracks();
    
    debug(2, "Encoding D64 archive with %d tracks", numTracks);
    
    // Zone 1: Tracks 1 - 17 (21 sectors, tailgap 9/9
    for (track = 1; track <= 17; track++) {
        assert(21 == a->numberOfSectors(trackToHalftrack(track)));
        encodedBytes = encodeTrack(a, track, zone1, 9, 9);
    }
    
    // Zone 2: Tracks 18 - 24 (19 sectors, tailgap 9/19 (even/odd sectors))
    for (track = 18; track <= 24; track++) {
        encodedBytes = encodeTrack(a, track, zone2, 9, 19);
    }
    
    // Zone 3: Tracks 25 - 30 (18 sectors, tailgap 9/13 (even/odd sectors))
    for (track = 25; track <= 30; track++) {
        encodedBytes = encodeTrack(a, track, zone3, 9, 13);
    }
    
    // Zone 4: Tracks 31 - 35..42 (17 sectors, tailgap 9/10 (even/odd sectors))
    for (track = 31; track <= a->numberOfTracks(); track++) {
        encodedBytes = encodeTrack(a, track, zone4, 9, 10);
    }
    
    // Clear remaining tracks (if any)
    for (track = numTracks + 1; track <= 42; track++) {
        unsigned halftrack = trackToHalftrack(track);
        setLengthOfHalftrack(halftrack, encodedBytes);
        setLengthOfHalftrack(halftrack + 1, encodedBytes);
    }
    
    for (unsigned i = 1; i <= 84; i++) {
        assert(length[i-1] <= 7928);
    }
}

unsigned
Disk525::encodeTrack(D64Archive *a, uint8_t track, int *sector, uint8_t tailGapEven, uint8_t tailGapOdd)
{
    unsigned encodedBytes, totalEncodedBytes = 0;
    
    assert(1 <= track && track <= 42);
    assert(a != NULL);
    
    debug(4, "Encoding track %d\n", track);
    
    uint8_t *dest = startOfTrack(track);
    
    // Scan the interleave pattern and encode each sector
    for (unsigned i = 0; sector[i] != -1; i++) {
        
        encodedBytes = encodeSector(a, track, sector[i], dest, (i % 2) ? tailGapOdd : tailGapEven);
        dest += encodedBytes;
        totalEncodedBytes += encodedBytes;
    }
    
    setLengthOfTrack(track, totalEncodedBytes);
    setLengthOfHalftrack(trackToHalftrack(track)+1, totalEncodedBytes);
    return totalEncodedBytes;
}

unsigned
Disk525::encodeSector(D64Archive *a, uint8_t track, uint8_t sector, uint8_t *dest, int gap)
{
    uint8_t *source, *ptr = dest;
    uint8_t halftrack = trackToHalftrack(track);
    
    assert(1 <= track && track <= 42);
    assert(a != NULL);
    assert(ptr != NULL);
    
    // Get source address from archive
    if ((source = a->findSector(halftrack, sector)) == 0) {
        warn("Can't find halftrack data in archive\n");
        return 0;
    }
    
    debug(4, "  Encoding sector %d\n", track, sector);
    
    // Get disk id and compute checksum
    uint8_t id_lo = a->diskIdLow();
    uint8_t id_hi = a->diskIdHi();
    uint8_t checksum = id_lo ^ id_hi ^ track ^ sector; // Header checksum byte
    
    encodeSync(ptr); // 0xFF 0xFF 0xFF 0xFF 0xFF
    ptr += 5;
    encodeGcr(0x08, checksum, sector, track, ptr); // header block ID, checksum, sector and track
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
    unsigned track, halftrack, numBytes = 0;
    uint8_t tmpbuf[2 * 7928];
    
    if (error) *error = 0; // We assume the best
    
    // For each full track ...
    for (track = 1, halftrack = 0; track <= numTracks; track++, halftrack += 2) {
        
        debug(3, "Decoding track %d %s\n", track, dest == NULL ? "(test run)" : "");
        
        // Copy the track into temporary buffer
        // Buffer is double sized, so we can read safely beyond the array bounds
        
        assert(length[halftrack] < 7928);
        memcpy(tmpbuf, data[halftrack], length[halftrack]);
        memcpy(tmpbuf + length[halftrack], data[halftrack], length[halftrack]);
        
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
    
    // Create inverse lookup table
    uint8_t lookup[32];
    memset(lookup, 0, sizeof(lookup));
    for (unsigned i = 0; i < 16; i++)
        lookup[gcr[i]] = i;
    
    // Shift in
    shift_reg = b1;
    shift_reg = (shift_reg << 8) | b2;
    shift_reg = (shift_reg << 8) | b3;
    shift_reg = (shift_reg << 8) | b4;
    shift_reg = (shift_reg << 8) | b5;
    
    // Shift out
    dest[3] = lookup[shift_reg & 0x1F]; shift_reg >>= 5;
    dest[3] |= (lookup[shift_reg & 0x1F] << 4); shift_reg >>= 5;
    dest[2] = lookup[shift_reg & 0x1F]; shift_reg >>= 5;
    dest[2] |= (lookup[shift_reg & 0x1F] << 4); shift_reg >>= 5;
    dest[1] = lookup[shift_reg & 0x1F]; shift_reg >>= 5;
    dest[1] |= (lookup[shift_reg & 0x1F] << 4); shift_reg >>= 5;
    dest[0] = lookup[shift_reg & 0x1F]; shift_reg >>= 5;
    dest[0] |= (lookup[shift_reg & 0x1F] << 4);
}

void
Disk525::dumpTrack(Halftrack ht, unsigned min, unsigned max, unsigned highlight)
{
    assert(isHalftrackNumber(ht));
    
    if (max > lengthOfHalftrack(ht)) max = lengthOfHalftrack(ht);
    
    msg("Dumping track %d (length = %d)\n", ht, lengthOfHalftrack(ht));
    for (unsigned i = min; i < max; i++) {
        msg(i == highlight ? "(%02X) " : "%02X ", data[ht][i]);
    }
    msg("\n");
}



