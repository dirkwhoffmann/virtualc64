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
    name = "Disk525";

    // Register snapshot items
    SnapshotItem items[] = {        
        { data.track[0],    sizeof(data.track),     KEEP_ON_RESET },
        { length.track[0],  sizeof(length.track),   KEEP_ON_RESET | WORD_FORMAT },
        { &numTracks,       sizeof(numTracks),      KEEP_ON_RESET },
        { &writeProtected,  sizeof(writeProtected), KEEP_ON_RESET },
        { NULL,             0,                      0 }};
    
    registerSnapshotItems(items, sizeof(items));

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
Disk525::dumpState()
{
    unsigned noOfOneBits, alignedSyncs, unalignedSyncs;
    uint8_t bit;
    
    msg("5,25\" floppy disk\n");
    msg("-----------------\n\n");

    for (unsigned track = 1; track <= 42; track++) {
        assert(isTrackNumber(track));
        noOfOneBits = alignedSyncs = unalignedSyncs = 0;
        for (unsigned offset = 0; offset < length.track[track][0]; offset++) {
            if ((bit = readBit(data.track[track], offset))) {
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
}

void
Disk525::debugSyncMarks(uint8_t *data, unsigned lengthInBits) {
    
    unsigned r, noOfOneBits, alignedSyncs = 0, unalignedSyncs = 0;

    for (r = noOfOneBits = 0; r < lengthInBits; r++) {
        if (readBit(data, r)) {
            noOfOneBits++;
        } else {
            if (noOfOneBits >= 10) { // SYNC FOUND
                if (r % 8 == 0) {
                    alignedSyncs++;
                } else {
                    warn("Unaligned SYNC mark found at offset %d\n", r);
                    unalignedSyncs++;
                }
            }
            noOfOneBits = 0;
        }
    }

    if (unalignedSyncs) {
        warn("%d out of %d SYNC marks are not byte aligned\n", unalignedSyncs, alignedSyncs + unalignedSyncs);
    }
        
}

void
Disk525::dumpHalftrack(Halftrack ht, unsigned min, unsigned max, unsigned highlight)
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

void
Disk525::clearDisk()
{
    for (Halftrack ht = 1; ht <= 84; ht++) {
        clearHalftrack(ht);
        length.halftrack[ht] = sizeof(data.halftrack[ht]) * 8;
    }
    writeProtected = false; 
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

    unsigned track, encodedBits;
    
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
        encodedBits = encodeTrack(a, track, zone4, 9, 10);
    }
    
    // Clear remaining tracks (if any)
    for (track = numTracks + 1; track <= 42; track++) {
        assert(encodedBits % 8 == 0);
        length.track[track][0] = encodedBits;  // Track t
        length.track[track][1] = encodedBits;  // Half track above
    }
    
    for (Halftrack ht = 1; ht <= 84; ht++) {
        assert(length.halftrack[ht] <= sizeof(data.halftrack[ht]) * 8);
    }
}

unsigned
Disk525::encodeTrack(D64Archive *a, Track t, int *sectorList, uint8_t tailGapEven, uint8_t tailGapOdd)
{
    assert(isTrackNumber(t));

    unsigned encodedBits, totalEncodedBits = 0;
    uint8_t *dest = data.track[t];
    
    debug(3, "Encoding track %d\n", t);
    
    // Scan the interleave pattern and encode each sector
    for (unsigned i = 0; sectorList[i] != -1; i++) {
        
        encodedBits = encodeSector(a, t, sectorList[i], dest, (i % 2) ? tailGapOdd : tailGapEven);
        assert(encodedBits % 8 == 0);
        dest += (encodedBits / 8);
        totalEncodedBits += encodedBits;
    }

    assert(totalEncodedBits % 8 == 0);
    length.track[t][0] = totalEncodedBits;  // Track t
    length.track[t][1] = totalEncodedBits;  // Half track above
    
    return totalEncodedBits;
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
    return (ptr - dest) * 8;
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
    uint8_t tmpbuf1[2 * 7928], tmpbuf2[2 * 7928];
    unsigned tmpbuf1length, tmpbuf2length;
    unsigned r, w, copies, noOfOneBits, bitsOnTrack = 0, numBytes = 0;
    int startOfFirstSyncMark = -1;
    
    if (error) *error = 0; // We assume the best
    
    
    // For each full track ...
    for (Track t = 1; t <= numTracks; t++) {
    
        bitsOnTrack = length.track[t][0];
        startOfFirstSyncMark = 0;
        
        debug(3, "Decoding track %d (%d bits) %s\n", t, bitsOnTrack, dest == NULL ? "(test run)" : "");
        
        // Step 1: Search for first SYNC mark (ten 1s in a row)
        debug(3, "    Searching for first SYNC mark\n", startOfFirstSyncMark);
        for (r = noOfOneBits = 0; r < bitsOnTrack; r++) {
            
            // Count '1' bits
            if (readBit(data.track[t], r)) { noOfOneBits++; } else { noOfOneBits = 0; }
            
            // Check if we have found the beginning of a SYNC mark (ten 1s in a row)
            if (noOfOneBits == 10) { startOfFirstSyncMark = r - 9; break; }
        }
        
        if (startOfFirstSyncMark < 0) {
            warn("Disk decoding aborted. No SYNC mark found on track %d\n", t);
            *error = 1;
            return 0;
        }
        

        // Step 2: Copy track data into first temporary buffer starting at the first SYNC mark
        // Track data is repeates twice, so we can read safely beyond the array bounds later
        debug(3, "    Setting up temporary buffer (alignment offset = %d)\n", startOfFirstSyncMark);
        for (copies = w = 0; copies < 2; copies++) {
            for (r = startOfFirstSyncMark; r < bitsOnTrack; r++) {
                assert((w / 8) < sizeof(tmpbuf1) - 1);
                writeBit(tmpbuf1, w++, readBit(data.track[t], r));
            }
            for (r = 0; r < startOfFirstSyncMark; r++) {
                assert((w / 8) < sizeof(tmpbuf1) - 1);
                writeBit(tmpbuf1, w++, readBit(data.track[t], r));
            }
        }
        assert(w % 8 == 0);

        tmpbuf1length = w;
        debug(3, "    Temporary buffer contains %d bits\n", tmpbuf1length);
        assert(tmpbuf1length == 2 * bitsOnTrack);

        
        // Step 3: Write a byte aligned copy of the first temporary buffer into the second buffer.
        debug(3, "    Aligning SYNC marks\n");
        uint8_t bit;
        for (r = w = noOfOneBits = 0; r < tmpbuf1length; r++) {
            
            // Count '1' bits
            if ((bit = readBit(tmpbuf1, r))) noOfOneBits++; else noOfOneBits = 0;
            
            // Copy bits if we are not inside a SYNC mark
            if (noOfOneBits < 10) { writeBit(tmpbuf2, w++, bit); }
            
            // Check if we have found the beginning of a SYNC mark (ten 1s in a row)
            if (noOfOneBits == 10) {
                
                // Write more 1s and make sure that data is byte aligned
                for (unsigned i = 0; i < 8 || (w % 8) != 0; i++) writeBit(tmpbuf2, w++, 1);
            }
        }
        tmpbuf2length = w;
        debug(3, "    Buffer contains %d bits after alignment\n", tmpbuf2length);

        // Report sync marks that are not byte aligned (there shouldn't be any)
        debugSyncMarks(tmpbuf2, tmpbuf2length);
        
        // Step 4: Decode track data
        if (dest)
            numBytes += decodeTrack(tmpbuf2, dest + numBytes, error);
        else
            numBytes += decodeTrack(tmpbuf2, NULL, error);
        
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
            debug(3, "Found header block (%d/%d)\n", data[3], data[2], data[1]);
            
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
            
            debug(3, "Found data block for sector %d at offset %d\n", sectorID, i);
            sectorStart[sectorID] = i;
            
        } else {
            warn("Skipping sector %d. Unknown header ID (expected $07 or $08, found $%02X).\n", sectorID, sectorID);
            if (error) *error = 1;
        }
    }
    
    // Decode all sectors that have been found
    for (unsigned i = 0; i < 21 && sectorStart[i] != -1; i++, numBytes += 256) {
        debug(3, "   Decoding sector %d\n", i);
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
    
    debug(3, "%d bytes written.\n", ptr-dest);
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





