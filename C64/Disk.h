/*!
 * @header      Disk.h
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

#ifndef _DISK_INC
#define _DISK_INC

#include "VirtualComponent.h"
#include "Disk_types.h"

// Forward declarations
class C64;
class D64Archive;
class G64Archive;
class NIBArchive;


//! @brief    A virtual floppy disk
class Disk : public VirtualComponent {
    
public:
    
    Disk();
    ~Disk();
    
    //! @brief   Method from VirtualComponent
    void dumpState();
    
    //
    // Constants and lookup tables
    //
    
private:
    
    /*! @brief   GCR encoding table
     * @details  Maps 4 data bits to 5 GCR bits.
     */
    const uint5_t gcr[16] = {
        
        0x0a, 0x0b, 0x12, 0x13, /*  0 -  3 */
        0x0e, 0x0f, 0x16, 0x17, /*  4 -  7 */
        0x09, 0x19, 0x1a, 0x1b, /*  8 - 11 */
        0x0d, 0x1d, 0x1e, 0x15  /* 12 - 15 */
    };
    
    /*! @brief    Inverse GCR encoding table
     *  @details  Maps 5 GCR bits to 4 data bits.
     */
    const uint4_t invgcr[32] = {
        
        0,  0,  0,  0, /* 0x00 - 0x03 */
        0,  0,  0,  0, /* 0x04 - 0x07 */
        0,  8,  0,  1, /* 0x08 - 0x0B */
        0, 12,  4,  5, /* 0x0C - 0x0F */
        0,  0,  2,  3, /* 0x10 - 0x13 */
        0, 15,  6,  7, /* 0x14 - 0x17 */
        0,  9, 10, 11, /* 0x18 - 0x1B */
        0, 13, 14,  0  /* 0x1C - 0x1F */
    };

    /*! @brief    Indicates which bit sequences are valid GCR bit patterns
     */
    const bool validGcrPattern[32] = {
        
        0,  0,  0,  0, /* 0x00 - 0x03 */
        0,  0,  0,  0, /* 0x04 - 0x07 */
        0,  1,  1,  1, /* 0x08 - 0x0B */
        0,  1,  1,  1, /* 0x0C - 0x0F */
        0,  0,  1,  1, /* 0x10 - 0x13 */
        0,  1,  1,  1, /* 0x14 - 0x17 */
        0,  1,  1,  1, /* 0x18 - 0x1B */
        0,  1,  1,  0  /* 0x1C - 0x1F */
    };

    /*! @brief    Maps a byte to an expanded 64 bit representation
     *  @details  Example: 0110 ... -> 00000000 00000001 0000001 00000000 ...
     *            This method is used to quickly inflate a bit stream into a
     *            byte stream.
     */
    uint64_t bitExpansion[256];
    
    
    //
    // Disk properties
    //
    
    /*! @brief   Write protection mark
     */
    bool writeProtected;
    
    /*! @brief   Indicates whether data has been written
     *  @details Depending on this flag, the GUI shows a warning dialog
     *           before a disk gets ejected.
     */
    bool modified;
    
    
    //
    // Disk data
    //

private:
    
    /*! @brief    Disk data
     *  @details  The first valid track and halftrack number is 1.
     *            data.halftack[i] points to the first byte of halftrack i,
     *            data.track[i] points to the first byte of track i
     */
    union {
        struct {
            uint8_t _pad[maxBytesOnTrack];
            uint8_t halftrack[85][maxBytesOnTrack];
        };
        uint8_t track[43][2 * maxBytesOnTrack];
    } data;
    
    /*! @brief    Length of each halftrack in bits
     *  @details  length.halftack[i] is the length of halftrack i,
     *            length.track[i][0] is the length of track i,
     *            length.track[i][1] is the length of halftrack above track i
     */
    union {
        struct {
            uint16_t _pad;
            uint16_t halftrack[85];
        };
        uint16_t track[43][2];
    } length;

    
    //
    // Debug information
    //
    
    //! @brief    Track layout as determined by analyzeTrack
    TrackInfo trackInfo;
    
    //! @brief    Textual representation of track data
    char text[maxBitsOnTrack + 1];
    


public:
    
    /*! @brief Returns write protection flag 
     */
    bool isWriteProtected() { return writeProtected; }

    /*! @brief Sets write protection flag 
     */
    void setWriteProtection(bool b) { writeProtected = b; }

    /*! @brief Returns modified flag
     */
    bool isModified() { return modified; }
    
    /*! @brief Sets modified flag
     */
    void setModified(bool b) { modified = b; }

    
public:
    
    //
    //! @functiongroup Handling Gcr encoded data
    //
    
    //! @brief   Converts a 4 bit binary value to a 5 bit GCR codeword
    uint5_t bin2gcr(uint4_t value) { assert(is_uint4_t(value)); return gcr[value]; }

    //! @brief   Converts a 5 bit GCR codeword to a 4 bit binary value
    uint4_t gcr2bin(uint5_t value) { assert(is_uint5_t(value)); return invgcr[value]; }

    //! @brief   Encodes a byte as a GCR bitstream
    //! @details The created bitstream consists of 10 bytes (either 0x00 or 0x01)
    void encodeGcr(uint8_t value, uint8_t *gcrBits);
    
    //! @brief   Decodes a previously encoded GCR bitstream
    uint8_t decodeGcr(uint8_t *gcrBits);

    
    //
    //! @functiongroup Accessing disk data
    //
    
    //! @brief    Returns true if the provided drive head position is valid.
    bool isValidHeadPositon(Halftrack ht, HeadPosition pos) {
        return isHalftrackNumber(ht) && pos < length.halftrack[ht]; }
    
    //! @brief    Fixes a wrapped over head position.
    HeadPosition fitToBounds(Halftrack ht, HeadPosition pos) {
        uint16_t len = length.halftrack[ht];
        return pos < 0 ? pos + len : pos >= len ? pos - len : pos; }
    
    /*! @brief   Reads a single bit from disk.
     *  @note    The head position is expected to be inside the halftrack bounds.
     *  @result  0x00 or 0x01
     */
    uint8_t _readBitFromHalftrack(Halftrack ht, HeadPosition pos) {
        assert(isValidHeadPositon(ht, pos));
        return (data.halftrack[ht][pos / 8] & (0x80 >> (pos % 8))) != 0;
    }
    
    /*! @brief   Reads a single bit from disk.
     *  @result	 0x00 or 0x01
     */
    uint8_t readBitFromHalftrack(Halftrack ht, HeadPosition pos) {
        return _readBitFromHalftrack(ht, fitToBounds(ht, pos));
    }
 
    /*! @brief  Writes a single bit to disk.
     *  @note   The head position is expected to be inside the halftrack bounds.
     */
    void _writeBitToHalftrack(Halftrack ht, HeadPosition pos, bool bit) {
        assert(isValidHeadPositon(ht, pos));
        if (bit) {
            data.halftrack[ht][pos / 8] |= (0x0080 >> (pos % 8));
        } else {
            data.halftrack[ht][pos / 8] &= (0xFF7F >> (pos % 8));
        }
    }
    
    void _writeBitToTrack(Track t, HeadPosition pos, bool bit) {
        _writeBitToHalftrack(2 * t - 1, pos, bit);
    }
    
    /*! @brief  Writes a single bit to disk.
     */
    void writeBitToHalftrack(Halftrack ht, HeadPosition pos, bool bit) {
        _writeBitToHalftrack(ht, fitToBounds(ht, pos), bit);
    }
    
    void writeBitToTrack(Track t, HeadPosition pos, bool bit) {
        writeBitToHalftrack(2 * t - 1, pos, bit);
    }
    
    /*! @brief  Writes a single byte to disk.
     */
    void writeByteToHalftrack(Halftrack ht, HeadPosition pos, uint8_t byte) {
        for (uint8_t mask = 0x80; mask != 0; mask >>= 1)
            writeBitToHalftrack(ht, pos++, byte & mask);
    }

    void writeByteToTrack(Track t, HeadPosition pos, uint8_t byte) {
        writeByteToHalftrack(2 * t - 1, pos, byte);
    }

    /*! @brief   Writes a certain number of SYNC bits to disk.
     */
    void writeSyncBitsToHalftrack(Halftrack ht, HeadPosition pos, size_t length) {
        for (size_t i = 0; i < length; i++)
            writeBitToHalftrack(ht, pos++, 1);
    }

    void writeSyncBitsToTrack(Track t, HeadPosition pos, size_t length) {
        writeSyncBitsToHalftrack(2 * t - 1, pos, length);
    }
    
    /*! @brief   Writes a certain number of interblock bytes to disk.
     */
    void writeGapToHalftrack(Halftrack ht, HeadPosition pos, size_t length) {
        for (size_t i = 0; i < length; i++, pos += 8)
            writeByteToHalftrack(ht, pos, 0x55);
    }
    
    void writeGapToTrack(Track t, HeadPosition pos, size_t length) {
        writeGapToHalftrack(2 * t - 1, pos, length);
    }
    
    /*! @brief    Reverts to a factory-new disk.
     *  @details  All disk data gets erased and the copy protection mark removed.
     */
    void clearDisk();
    
    /*! @brief    Returns true if a  if a track is cleared out.
     *  @warning  Don't call this method frequently, because it scans the whole track.
     */
    bool trackIsEmpty(Track t);

    /*! @brief    Checks if a halftrack is cleared out.
     *  @warning  Don't call this method frequently, because it scans the whole track.
     */
    bool halftrackIsEmpty(Halftrack ht);

    /*! @brief    Returns the number of halftracks containing data
     *  @warning  Don't call this method frequently, because it scans the whole disk.
     */
    unsigned nonemptyHalftracks();

    
    //
    //! @functiongroup Analyzing the disk
    //

public:
    
    //! @brief    Returns the length of a halftrack in bits
    uint16_t lengthOfHalftrack(Halftrack ht) {
        assert(isHalftrackNumber(ht)); return length.halftrack[ht]; }

    //! @brief    Returns the length of a track in bits
    uint16_t lengthOfTrack(Track t) {
        assert(isTrackNumber(t)); return length.track[t][0]; }

    //! @brief    Analyzes the sector layout
    /*! @details  The start and end offsets of all sectors are determined and writes
     *            into variable trackLayout.
     */
    void analyzeHalftrack(Halftrack ht);
    void analyzeTrack(Track t);
    
private:
    
    void _analyzeTrack(uint8_t *data, uint16_t length);
    
public:
    
    //! @brief    Returns a sector layout from variable trackInfo
    SectorInfo sectorLayout(Sector nr) {
        assert(isSectorNumber(nr)); return trackInfo.sectorInfo[nr]; }
    
    //! @brief    Returns a textual representation of the disk name
    const char *diskNameAsString();
    
    //! @brief    Returns a textual representation of the data stored in trackInfo
    const char *trackDataAsString();

    //! @brief    Returns a textual representation of the data stored in trackInfo
    const char *sectorHeaderAsString(Sector nr);

    //! @brief    Returns a textual representation of the data stored in trackInfo
    const char *sectorDataAsString(Sector nr);

private:
    
    //! @brief    Returns a textual representation
    const char *sectorBytesAsString(uint8_t *buffer, size_t length);
    
    
    //
    //! @functiongroup Decoding disk data
    //
    
public:
    
    /*! @brief   Converts a disk into a byte stream compatible with the D64 format.
     *  @details Returns the number of bytes written. If dest is NULL, a test run is
     *           performed (used to determine how many bytes will be written). If
     *           something went wrong, an error code is written into 'error' (0 = success)
     */
    unsigned decodeDisk(uint8_t *dest, int *error = NULL);
    
private:
    
    //! @brief   Decodes all sectors of a track
    unsigned decodeTrack(Track t, uint8_t *dest, int *error = NULL);

    //! @brief   Decodes a single sector
    unsigned decodeSector(size_t offset, uint8_t *dest, int *error = NULL);

    
    //
    //! @functiongroup Encoding disk data
    //
    
public:
    
    /*! @brief   Converts a G64 archive into a virtual floppy disk */
    void encodeArchive(G64Archive *a);
    
    /*! @brief   Converts a NIB archive into a virtual floppy disk */
    void encodeArchive(NIBArchive *a);
    
    /*! @brief   Converts a D64 archive into a virtual floppy disk
     *  @details The method creates sync marks, GRC encoded header and data blocks,
     *           checksums and gaps.
     */
    void encodeArchive(D64Archive *a);
    
private:
    
    /*! @brief   Encode a single track
     *  @details This function translates the logical byte sequence of a single track into
     *           the native VC1541 byte representation. The native representation includes
     *           sync marks, GCR data etc.
     *  @param   tailGapEven
     *           Number of tail bytes follwowing sectors with even sector numbers.
     *  @param   tailGapOdd
     *           Number of tail bytes follwowing sectors with odd sector numbers.
     *  @return  Number of bytes written.
     */
    unsigned encodeTrack(D64Archive *a, Track t, int *sectorList, uint8_t tailGapEven, uint8_t tailGapOdd);
    
    /*! @brief   Encode a single sector
     *  @details This function translates the logical byte sequence of a single sector
     *           into the native VC1541 byte representation. The sector is closed by
     *           'gap' tail gap bytes.
     *  @return  Number of bytes written.
     */
    unsigned encodeSector(D64Archive *a, Track t, uint8_t sector, unsigned bitoffset, int gap);
    
    
    /*! @brief   Translates four data bytes into five GCR encodes bytes
     *! @deprecated
     */
    void encodeGcr(uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4, Track t, unsigned offset);
    
};
    
#endif
