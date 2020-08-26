// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _DISK_H
#define _DISK_H

#include "C64Component.h"

class Disk : public C64Component {
    
public:
    
    //
    // Constants and lookup tables
    //
    
    // Disk parameters of a standard floppy disk
    typedef struct
    {
        
        u8  sectors;          // Typical number of sectors in this track
        u8  speedZone;        // Default speed zone for this track
        u16 lengthInBytes;    // Typical track size in bits
        u16 lengthInBits;     // Typical track size in bits
        Sector firstSectorNr; // Logical number of first sector in track
        double stagger;       // Relative position of first bit (from Hoxs64)
    }
    TrackDefaults;
    
    static const TrackDefaults trackDefaults[43];
 
    
    /* Disk error codes. Some D64 files contain an error code for each sector.
     * If possible, these errors are reproduced during disk encoding.
     */
    typedef enum
    {
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
    }
    DiskErrorCode;

private:
    
    /* GCR encoding table. Maps 4 data bits to 5 GCR bits.
     */
    const u8 gcr[16] = {
        
        0x0a, 0x0b, 0x12, 0x13, /*  0 -  3 */
        0x0e, 0x0f, 0x16, 0x17, /*  4 -  7 */
        0x09, 0x19, 0x1a, 0x1b, /*  8 - 11 */
        0x0d, 0x1d, 0x1e, 0x15  /* 12 - 15 */
    };
    
    /* Inverse GCR encoding table. Maps 5 GCR bits to 4 data bits. Invalid
     * patterns are marked with 255.
     */
    const u8 invgcr[32] = {
        
        255, 255, 255, 255, /* 0x00 - 0x03 */
        255, 255, 255, 255, /* 0x04 - 0x07 */
        255,   8,   0,   1, /* 0x08 - 0x0B */
        255,  12,   4,   5, /* 0x0C - 0x0F */
        255, 255,   2,   3, /* 0x10 - 0x13 */
        255,  15,   6,   7, /* 0x14 - 0x17 */
        255,   9,  10,  11, /* 0x18 - 0x1B */
        255,  13,  14, 255  /* 0x1C - 0x1F */
    };

    /* Maps a byte to an expanded 64 bit representation. This method is used to
     * quickly inflate a bit stream into a byte stream.
     *
     *     Example: 0110 ... -> 00000000 00000001 0000001 00000000 ...
     */
    u64 bitExpansion[256];
    
    
    //
    // Disk properties
    //
    
    // Write protection mark
    bool writeProtected;
    
    /* Indicates whether data has been written. Depending on this flag, the GUI
     * shows a warning dialog before a disk gets ejected.
     */
    bool modified;
    
    
    //
    // Disk data
    //

public:
    
    // Data information for each halftrack on this disk
    DiskData data;

    // Length information for each halftrack on this disk
    DiskLength length;

    
    //
    // Debug information
    //
    
private:
    
    // Track layout as determined by analyzeTrack
    TrackInfo trackInfo;

    // Error log created by analyzeTrack
    std::vector<std::string> errorLog;

    // Stores the start offset of the erroneous bit sequence
    std::vector<size_t> errorStartIndex;

    // Stores the end offset of the erroneous bit sequence
    std::vector<size_t> errorEndIndex;

    // Textual representation of track data
    char text[maxBitsOnTrack + 1];
    
    
    //
    // Initializing
    //
    
public:
    
    Disk(C64 &ref);
    
private:
    
    void _reset() override;

    
    //
    // Analyzing
    //
    
private:
    
    void _dump() override;

    
    //
    // Serializing
    //
    
private:
    
    template <class T>
    void applyToPersistentItems(T& worker)
    {
        worker
        
        & writeProtected
        & modified
        & data
        & length;
    }
    
    template <class T>
    void applyToResetItems(T& worker)
    {
    }
    
    size_t _size() override { COMPUTE_SNAPSHOT_SIZE }
    size_t _load(u8 *buffer) override { LOAD_SNAPSHOT_ITEMS }
    size_t _save(u8 *buffer) override { SAVE_SNAPSHOT_ITEMS }
    
    
    //
    // Accessing
    //

public:
    
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
    u8 bin2gcr(u8 value) { assert(value < 16); return gcr[value]; }

    //! @brief   Converts a 5 bit GCR codeword to a 4 bit binary value
    u8 gcr2bin(u8 value) { assert(value < 32); return invgcr[value]; }

    //! @brief   Returns true if the provided 5 bit codeword is a valid GCR codeword
    bool isGcr(u8 value) { assert(value < 32); return invgcr[value] != 0xFF; }

    //! @brief   Encodes a single byte as a GCR bitstream.
    /*! @details Writes 10 bits to the specified position on disk.
     */
    void encodeGcr(u8 value, Track t, HeadPosition offset);

    //! @brief   Encodes multiple bytes as a GCR bitstream.
    /*! @details Writes length * 10 bits to the specified position on disk.
     */
    void encodeGcr(u8 *values, size_t length, Track t, HeadPosition offset);

    /*! @brief   Translates four data bytes into five GCR encodes bytes
     *! @deprecated
     */
    void encodeGcr(u8 b1, u8 b2, u8 b3, u8 b4, Track t, unsigned offset);
    
    //! @brief   Decodes a nibble (4 bit) from a previously encoded GCR bitstream.
    /*! @return  0xFF, if no valid GCR sequence is found.
     */
    u8 decodeGcrNibble(u8 *gcrBits);

    //! @brief   Decodes a byte (8 bit) form a previously encoded GCR bitstream.
    /*! @note    Returns an unpredictable result if invalid GCR sequences are found.
     */
    u8 decodeGcr(u8 *gcrBits);

    
    //
    //! @functiongroup Accessing disk data
    //
    
    //! @brief    Returns true if the provided drive head position is valid.
    bool isValidHeadPositon(Halftrack ht, HeadPosition pos) {
        return isHalftrackNumber(ht) && pos < length.halftrack[ht]; }
    
    //! @brief    Fixes a wrapped over head position.
    HeadPosition fitToBounds(Halftrack ht, HeadPosition pos) {
        u16 len = length.halftrack[ht];
        return pos < 0 ? pos + len : pos >= len ? pos - len : pos; }
    
    /*! @brief   Returns the duration of a single bit in 1/10 nano seconds.
     *  @details The returned value is the time span the drive head resists
     *           over the specified bit. The value is determined by the
     *           the density bits at the time the bit was written to disk.
     *  @note    The head position is expected to be inside the halftrack bounds.
     */
    u64 _bitDelay(Halftrack ht, HeadPosition pos);

    /*! @brief   Returns the duration of a single bit in 1/10 nano seconds.
     */
    u64 bitDelay(Halftrack ht, HeadPosition pos) {
        return _bitDelay(ht, fitToBounds(ht, pos));
    }
    
    /*! @brief   Reads a single bit from disk.
     *  @note    The head position is expected to be inside the halftrack bounds.
     *  @result  0x00 or 0x01
     */
    u8 _readBitFromHalftrack(Halftrack ht, HeadPosition pos) {
        assert(isValidHeadPositon(ht, pos));
        return (data.halftrack[ht][pos / 8] & (0x80 >> (pos % 8))) != 0;
    }
    
    /*! @brief   Reads a single bit from disk.
     *  @result	 0x00 or 0x01
     */
    u8 readBitFromHalftrack(Halftrack ht, HeadPosition pos) {
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
    void writeByteToHalftrack(Halftrack ht, HeadPosition pos, u8 byte) {
        for (u8 mask = 0x80; mask != 0; mask >>= 1)
            writeBitToHalftrack(ht, pos++, byte & mask);
    }

    void writeByteToTrack(Track t, HeadPosition pos, u8 byte) {
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
    // Analyzing the disk
    //

public:
    
    // Returns the length of a halftrack in bits
    u16 lengthOfHalftrack(Halftrack ht);
    u16 lengthOfTrack(Track t);
    
    /* Analyzes the sector layout. The functions determines the start and end
     * offsets of all sectors and writes them into variable trackLayout.
     */
    void analyzeHalftrack(Halftrack ht);
    void analyzeTrack(Track t);
    
private:
    
    // Checks the integrity of a sector header or sector data block
    void analyzeSectorHeaderBlock(size_t offset);
    void analyzeSectorDataBlock(size_t offset);

    // Writes an error message into the error log
    void log(size_t begin, size_t length, const char *fmt, ...);
    
public:
    
    // Returns a sector layout from variable trackInfo
    SectorInfo sectorLayout(Sector nr) {
        assert(isSectorNumber(nr)); return trackInfo.sectorInfo[nr]; }
    
    // Returns the number of entries in the error log
    unsigned numErrors() { return (unsigned)errorLog.size(); }
    
    // Reads an error message from the error log
    std::string errorMessage(unsigned nr) { return errorLog.at(nr); }
    
    // Reads the error begin index from the error log
    size_t firstErroneousBit(unsigned nr) { return errorStartIndex.at(nr); }
    
    // Reads the error end index from the error log
    size_t lastErroneousBit(unsigned nr) { return errorEndIndex.at(nr); }
    
    // Returns a textual representation of the disk name
    const char *diskNameAsString();
    
    // Returns a textual representation of the data stored in trackInfo
    const char *trackBitsAsString();

    // Returns a textual representation of the data stored in trackInfo
    const char *sectorHeaderBytesAsString(Sector nr, bool hex);

    // Returns a textual representation of the data stored in trackInfo
    const char *sectorDataBytesAsString(Sector nr, bool hex);

private:
    
    // Returns a textual representation
    const char *sectorBytesAsString(u8 *buffer, size_t length, bool hex);
    
    
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
    size_t decodeDisk(u8 *dest);
 
private:
    
    /*! @brief   Work horse for decodeDisk(u8 *)
     *  @param   numTracks must be either 35, 40, or 42.
     */
    size_t decodeDisk(u8 *dest, unsigned numTracks);
    
    //! @brief   Decodes all sectors of a track
    size_t decodeTrack(Track t, u8 *dest);

    //! @brief   Decodes a single sector
    size_t decodeSector(size_t offset, u8 *dest);

     //! @brief   Decodes a single broken sector (results in all zeroes)
     // size_t decodeBrokenSector(u8 *dest);


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
    size_t encodeTrack(D64File *a, Track t, u8 tailGap, HeadPosition start);
    
    /*! @brief   Encode a single sector
     *  @details This function translates the logical byte sequence of a single sector
     *           into the native VC1541 byte representation. The sector is closed by
     *           'gap' tail gap bytes.
     *  @return  Number of written bits.
     */
    size_t encodeSector(D64File *a, Track t, Sector sector, HeadPosition start, int gap);
};
    
#endif
