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

#include <string>
#include <vector>
#include <iostream>

#include "VirtualComponent.h"
#include "Disk_types.h"

class C64;
class VC1541;
class D64File;
class G64File;


//! @brief    A virtual floppy disk
class Disk : public VirtualComponent {
    
public:
    
    //
    // Constants and lookup tables
    //
    
    //! @brief   Disk parameters of a standard floppy disk
    typedef struct {
        
        uint8_t  sectors;       // Typical number of sectors in this track
        uint8_t  speedZone;     // Default speed zone for this track
        uint16_t lengthInBytes; // Typical track size in bits
        uint16_t lengthInBits;  // Typical track size in bits
        Sector   firstSectorNr; // Logical number of first sector in track
        double   stagger;       // Relative position of the first bit (taken from Hoxs64)

    } TrackDefaults;
    
    static const TrackDefaults trackDefaults[43];
 
    
    //! @brief   Disk error codes
    /*! @details Some D64 files contain an error code for each sector.
     *           If possible, these errors are reproduced during disk encoding.
     */
    typedef enum {
        DISK_OK = 0x1,
        HEADER_BLOCK_NOT_FOUND_ERROR = 0x2,
        NO_SYNC_SEQUENCE_ERROR = 0x3,
        DATA_BLOCK_NOT_FOUND_ERROR = 0x4,
        DATA_BLOCK_CHECKSUM_ERROR = 0x5,
        WRITE_VERIFY_ERROR_ON_FORMAT_ERROR = 0x6,
        WRITE_VERIFY_ERROR = 0x7,
        WRITE_PROTECT_ON_ERROR = 0x8,
        HEADER_BLOCK_CHECKSUM_ERROR = 0x9,
        WRITE_ERROR = 0xA,
        DISK_ID_MISMATCH_ERROR = 0xB,
        DRIVE_NOT_READY_ERRROR = 0xF
    } DiskErrorCode;

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
     *  @detaiels  Maps 5 GCR bits to 4 data bits. Invalid patterns are marked with 255.
     */
    const uint4_t invgcr[32] = {
        
        255, 255, 255, 255, /* 0x00 - 0x03 */
        255, 255, 255, 255, /* 0x04 - 0x07 */
        255,   8,   0,   1, /* 0x08 - 0x0B */
        255,  12,   4,   5, /* 0x0C - 0x0F */
        255, 255,   2,   3, /* 0x10 - 0x13 */
        255,  15,   6,   7, /* 0x14 - 0x17 */
        255,   9,  10,  11, /* 0x18 - 0x1B */
        255,  13,  14, 255  /* 0x1C - 0x1F */
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

public:
    
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
    
private:
    
    //! @brief    Information about a detected track error
    typedef struct {
        size_t begin;
        size_t end;
        std::string msg;
    } TrackInfoError;
    
    //! @brief    Track layout as determined by analyzeTrack
    TrackInfo trackInfo;

    //! @brief    Error log created by analyzeTrack
    std::vector<std::string> errorLog;

    //! @brief    Stores the start offset of the erroneous bit sequence
    std::vector<size_t> errorStartIndex;

    //! @brief    Stores the end offset of the erroneous bit sequence
    std::vector<size_t> errorEndIndex;

    //! @brief    Textual representation of track data
    char text[maxBitsOnTrack + 1];
    

public:
    
    //
    //! @functiongroup Constructing and destructing
    //
    
    //! @brief    Constructor
    Disk();
    
    //! @brief    Destructor
    ~Disk();
    
    
    //
    //! @functiongroup Methods from VirtualComponent
    //
    
    void dump();
    void ping();

    
    
    //! @brief Returns write protection flag
    bool isWriteProtected() { return writeProtected; }

    //! @brief Sets write protection flag
    void setWriteProtection(bool b) { writeProtected = b; }

    //! @brief Toggles the write protection flag
    void toggleWriteProtection() { writeProtected = !writeProtected; }

    //! @brief Returns modified flag
    bool isModified() { return modified; }
    
    //! @brief Sets modified flag
    void setModified(bool b);

    
    //
    //! @functiongroup Handling Gcr encoded data
    //
    
public:
    
    //! @brief   Converts a 4 bit binary value to a 5 bit GCR codeword
    uint5_t bin2gcr(uint4_t value) { assert(is_uint4_t(value)); return gcr[value]; }

    //! @brief   Converts a 5 bit GCR codeword to a 4 bit binary value
    uint4_t gcr2bin(uint5_t value) { assert(is_uint5_t(value)); return invgcr[value]; }

    //! @brief   Returns true if the provided 5 bit codeword is a valid GCR codeword
    bool isGcr(uint5_t value) { assert(is_uint5_t(value)); return invgcr[value] != 0xFF; }

    //! @brief   Encodes a single byte as a GCR bitstream.
    /*! @details Writes 10 bits to the specified position on disk.
     */
    void encodeGcr(uint8_t value, Track t, HeadPosition offset);

    //! @brief   Encodes multiple bytes as a GCR bitstream.
    /*! @details Writes length * 10 bits to the specified position on disk.
     */
    void encodeGcr(uint8_t *values, size_t length, Track t, HeadPosition offset);

    /*! @brief   Translates four data bytes into five GCR encodes bytes
     *! @deprecated
     */
    void encodeGcr(uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4, Track t, unsigned offset);
    
    //! @brief   Decodes a nibble (4 bit) from a previously encoded GCR bitstream.
    /*! @return  0xFF, if no valid GCR sequence is found.
     */
    uint8_t decodeGcrNibble(uint8_t *gcrBits);

    //! @brief   Decodes a byte (8 bit) form a previously encoded GCR bitstream.
    /*! @note    Returns an unpredictable result if invalid GCR sequences are found.
     */
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
    
    /*! @brief   Returns the duration of a single bit in 1/10 nano seconds.
     *  @details The returned value is the time span the drive head resists
     *           over the specified bit. The value is determined by the
     *           the density bits at the time the bit was written to disk.
     *  @note    The head position is expected to be inside the halftrack bounds.
     */
    uint64_t _bitDelay(Halftrack ht, HeadPosition pos);

    /*! @brief   Returns the duration of a single bit in 1/10 nano seconds.
     */
    uint64_t bitDelay(Halftrack ht, HeadPosition pos) {
        return _bitDelay(ht, fitToBounds(ht, pos));
    }
    
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
    
    //! @brief  Writes a single bit to disk.
    void writeBitToHalftrack(Halftrack ht, HeadPosition pos, bool bit) {
        _writeBitToHalftrack(ht, fitToBounds(ht, pos), bit);
    }
    
    void writeBitToTrack(Track t, HeadPosition pos, bool bit) {
        writeBitToHalftrack(2 * t - 1, pos, bit);
    }
    
    //! @brief  Writes a single bit to disk multiple times.
    void writeBitToHalftrack(Halftrack ht, HeadPosition pos, bool bit, size_t count) {
        for (size_t i = 0; i < count; i++)
            writeBitToHalftrack(ht, pos++, bit);
    }
    
    void writeBitToTrack(Track t, HeadPosition pos, bool bit, size_t count) {
            writeBitToHalftrack(2 * t - 1, pos, bit, count);
    }

    //! @brief  Writes a single byte to disk.
    void writeByteToHalftrack(Halftrack ht, HeadPosition pos, uint8_t byte) {
        for (uint8_t mask = 0x80; mask != 0; mask >>= 1)
            writeBitToHalftrack(ht, pos++, byte & mask);
    }

    void writeByteToTrack(Track t, HeadPosition pos, uint8_t byte) {
        writeByteToHalftrack(2 * t - 1, pos, byte);
    }
    
    //! @brief   Writes a certain number of interblock bytes to disk.
    void writeGapToHalftrack(Halftrack ht, HeadPosition pos, size_t length) {
        for (size_t i = 0; i < length; i++, pos += 8)
            writeByteToHalftrack(ht, pos, 0x55);
    }
    
    void writeGapToTrack(Track t, HeadPosition pos, size_t length) {
        writeGapToHalftrack(2 * t - 1, pos, length);
    }

    //! @brief    Clears a single half-track.
    void clearHalftrack(Halftrack ht); 

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
    
    void analyzeTrack(Track t) { assert(isTrackNumber(t)); analyzeHalftrack(2 * t - 1); }
    
private:
    
    //! @brief   Checks the integrity of a sector header block
    void analyzeSectorHeaderBlock(size_t offset);
    
    //! @brief   Checks the integrity of a sector data block
    void analyzeSectorDataBlock(size_t offset);

    //! @brief    Writes an error message into the error log
    void log(size_t begin, size_t length, const char *fmt, ...);
    
public:
    
    //! @brief    Returns a sector layout from variable trackInfo
    SectorInfo sectorLayout(Sector nr) {
        assert(isSectorNumber(nr)); return trackInfo.sectorInfo[nr]; }
    
    //! @brief    Returns the number of entries in the error log
    unsigned numErrors() { return (unsigned)errorLog.size(); }
    
    //! @brief    Reads an error message from the error log
    std::string errorMessage(unsigned nr) { return errorLog.at(nr); }
    
    //! @brief    Reads the error begin index from the error log
    size_t firstErroneousBit(unsigned nr) { return errorStartIndex.at(nr); }
    
    //! @brief    Reads the error end index from the error log
    size_t lastErroneousBit(unsigned nr) { return errorEndIndex.at(nr); }
    
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
    
    /*! @brief   Converts the disk into a byte stream.
     *  @details The byte stream is compatible with the D64 file format.
     *  @param   dest Target buffer. If parameter is NULL, a test run is
     *           performed. Test runs are used to determine upfront how many
     *           bytes will be written.
     *  @return  Number of bytes written.
     */
    size_t decodeDisk(uint8_t *dest);
 
private:
    
    /*! @brief   Work horse for decodeDisk(uint8_t *)
     *  @param   numTracks must be either 35, 40, or 42.
     */
    size_t decodeDisk(uint8_t *dest, unsigned numTracks);
    
    //! @brief   Decodes all sectors of a track
    size_t decodeTrack(Track t, uint8_t *dest);

    //! @brief   Decodes a single sector
    size_t decodeSector(size_t offset, uint8_t *dest);

     //! @brief   Decodes a single broken sector (results in all zeroes)
     // size_t decodeBrokenSector(uint8_t *dest);


    //
    //! @functiongroup Encoding disk data
    //
    
public:
    
    /*! @brief   Converts a G64 archive into a virtual floppy disk. */
    void encodeArchive(G64File *a);
        
    /*! @brief   Converts a D64 archive into a floppy disk.
     *  @details The method creates sync marks, GRC encoded header and data
     *           blocks, checksums and gaps.
     *  @param   alignTracks If true, the first sector always starts at the
     *           beginning of a track.
     */
    void encodeArchive(D64File *a, bool alignTracks);

    //! @brief   Converts a D64 archive into a floppy disk.
    void encodeArchive(D64File *a) { encodeArchive(a, false); }

private:
    
    /*! @brief   Encode a single track
     *  @details This function translates the logical byte sequence of a single track into
     *           the native VC1541 byte representation. The native representation includes
     *           sync marks, GCR data etc.
     *  @param   tailGapEven
     *           Number of tail bytes follwowing sectors with even sector numbers.
     *  @param   tailGapOdd
     *           Number of tail bytes follwowing sectors with odd sector numbers.
     *  @return  Number of written bits.
     */
    size_t encodeTrack(D64File *a, Track t, uint8_t tailGap, HeadPosition start);
    
    /*! @brief   Encode a single sector
     *  @details This function translates the logical byte sequence of a single sector
     *           into the native VC1541 byte representation. The sector is closed by
     *           'gap' tail gap bytes.
     *  @return  Number of written bits.
     */
    size_t encodeSector(D64File *a, Track t, Sector sector, HeadPosition start, int gap);
};
    
#endif
