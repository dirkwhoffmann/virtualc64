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


//! @brief    A virtual 5,25" floppy disk
class Disk : public VirtualComponent {
    
public:
    
    Disk();
    ~Disk();
    
    //! @brief    Method from VirtualComponent
    void dumpState();
    
    
private:
    
    /*! @brief    GCR encoding table
     * @details   Maps 4 data bits to 5 GCR bits.
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
    
public:
    
    /*! @brief    Length of each halftrack in bits
     *  @details  length.halftack[i] is the length of halftrack i,
     *            length.track[i][0] is the length of track i,
     *            length.track[i][1] is the length of halftrack above track i
     */
    union {
        struct {
            size_t _pad;
            size_t halftrack[85];
        };
        size_t track[43][2];
    } length;

    //! @brief    Track layout as determined by analyzeTrack
    TrackInfo trackInfo;
    
    //! @brief    Textual representation of track data
    char text[maxBitsOnTrack + 1];
        
    /*! @brief       Total number of tracks on this disk
     *  @deprecated  Add method bool emptyTrack(Track nr) as a replacement
     */
    // uint8_t numTracks;

    /*! @brief    Returns true if track/offset indicates a valid disk position on disk
     */
    bool isValidDiskPositon(Halftrack ht, uint16_t bitoffset) {
        return isHalftrackNumber(ht) && bitoffset < length.halftrack[ht]; }
 
private:

    /*! @brief   Write protection mark
     */
    bool writeProtected;

    /*! @brief   Indicates whether data has been written
     *  @details According to this flag, the GUI shows a data loss warning dialog before a disk gets ejected.
     */
    bool modified;

    
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

    //! @brief   Converts a 5 bit GC codeword to a 4 bit binary value
    uint4_t gcr2bin(uint5_t value) { assert(is_uint5_t(value)); return invgcr[value]; }

    //! @brief   Encodes a byte as a GCR bitstream
    //! @details The created bitstream consists of 10 bytes (either 0 or 1)
    void encodeGcr(uint8_t value, uint8_t *gcrBits);
    
    //! @brief   Decodes a previously encoded GCR bitstream
    uint8_t decodeGcr(uint8_t *gcrBits);

    
    //
    //! @functiongroup Reading data from disk
    //
    
    /*! @brief   Reads a single bit from disk
     *  @param   data    Pointer to the first data byte of a track
     *  @param   offset  Number of bit to read (first bit has offset 0)
     *  @result  0 or 1
     */
    //uint8_t readBit(uint8_t *data, unsigned offset) {
    //     return (data[offset / 8] & (0x80 >> (offset % 8))) ? 1 : 0; }

    /*! @brief   Reads a single bit from disk
     *  @param   ht      Halftrack to read from
     *  @param   offset  Bit position (starting with 0)
     *  @result	 0 or 1
     */
    uint8_t readBitFromHalftrack(Halftrack ht, size_t offset) {
        assert(isHalftrackNumber(ht));
        // return readBit(data.halftrack[ht], offset % length.halftrack[ht]);
        offset = offset % length.halftrack[ht];
        return (data.halftrack[ht][offset / 8] & (0x80 >> (offset % 8))) ? 1 : 0;
    }

    /*! @brief   Reads a single bit from disk
     *  @param   t      Track to read from
     *  @param   offset  Bit position (starting with 0)
     *  @result     0 or 1
     */
    uint8_t readBitFromTrack(Track t, size_t offset) {
        assert(isTrackNumber(t));
        offset = offset % length.track[t][0];
        return (data.track[t][offset / 8] & (0x80 >> (offset % 8))) ? 1 : 0;
    }

    
    
    /*! @brief   Reads a single byte from disk
     *  @param   data    Pointer to the first data byte of a track
     *  @param   offset  Position of first bit to read (first bit has offset 0)
     *  @result	 0 .. 255
     */
    /*
    uint8_t readByte(uint8_t *data, unsigned offset) {
        uint8_t result = 0;
        
        for (uint8_t i = 0, mask = 0x80; i < 8; i++, mask >>= 1)
            if (readBit(data, offset + i)) result |= mask;
        return result;
    }
     */
    
    /*! @brief   Reads a single byte from disk
     *  @param   ht      Number of halftrack to read from
     *  @param   offset  Position of first bit to read (first bit has offset 0)
     *  @result	 returns 0 or 1
     */
    uint8_t readByteFromHalftrack(Halftrack ht, size_t offset) {
        uint8_t result = 0;
        for (uint8_t i = 0, mask = 0x80; i < 8; i++, mask >>= 1)
            if (readBitFromHalftrack(ht, offset + i)) result |= mask;
        return result;
    }

    
    //
    //! @functiongroup Writing data to disk
    //
    
 
    /*! @brief  Writes a single bit to disk
     *  @param  ht     Halftrack to write to
     *  @param  offset Bit position (0 ... length.halftrack[ht] - 1)
     *  @param  bit    Bit value (true = 1, false = 0)
     */
    void writeBitToHalftrack(Halftrack ht, size_t offset, bool bit) {
        assert(isHalftrackNumber(ht));
        offset = offset % length.halftrack[ht];
        if (bit) {
            data.halftrack[ht][offset / 8] |= (0x80 >> (offset % 8));
        } else {
            data.halftrack[ht][offset / 8] &= ~(0x80 >> (offset % 8));
        }
    }

    /*! @brief  Writes a single bit to disk
     *  @param  t      Track number
     *  @param  offset Bit position (0 ... length.track[t] - 1)
     *  @param  bit    Bit value (true = 1, false = 0)
     */
    void writeBitToTrack(Track t, size_t offset, bool bit) {
        assert(isTrackNumber(t));
        offset = offset % length.track[t][0];
        if (bit) {
            data.track[t][offset / 8] |= (0x80 >> (offset % 8));
        } else {
            data.track[t][offset / 8] &= ~(0x80 >> (offset % 8));
        }
    }
    
    /*! @brief  Writes a single byte to disk
     *  @param  data   Pointer to the first data byte of a track
     *  @param  offset Number of fist bit to write
     *  @param  byte   Byte to write
     */
    /*
    void writeByte(uint8_t *data, unsigned offset, uint8_t byte) {
        for (uint8_t i = 0, mask = 0x80; i < 8; i++, mask >>= 1)
            writeBit(data, offset + i, byte & mask);
    }
    */
    
    /*! @brief  Writes a single byte to disk
     *  @param  ht     Halftrack number
     *  @param  offset Position of first bit to write
     *  @param  byte   Byte to write
     */
    void writeByteToHalftrack(Halftrack ht, size_t offset, uint8_t byte) {
        assert(isHalftrackNumber(ht));
        for (uint8_t i = 0, mask = 0x80; i < 8; i++, mask >>= 1)
            writeBitToHalftrack(ht, offset + i, byte & mask);
    }

    /*! @brief  Writes a single byte to disk
     *  @param  ht     Number of track to write to
     *  @param  offset Position of first bit to write
     *  @param  byte   Byte to write
     */
    void writeByteToTrack(Track t, size_t offset, uint8_t byte) {
        assert(isTrackNumber(t));
        for (uint8_t i = 0, mask = 0x80; i < 8; i++, mask >>= 1)
            writeBitToTrack(t, offset + i, byte & mask);
    }

    /*! @brief   Writes a certain number of SYNC bits
     *  @param   dest Start address of track.
     *  @param   offset Beginning of SYNC sequence relative to track start in bits
     *  @param   length Number of SYNC bits to write
     */
    // void writeSyncBits(uint8_t *dest, unsigned offset, unsigned length) {
    //     for (unsigned i = 0; i < length; i++) writeBit(dest, offset + i, 1); }
    void writeSyncBitsToTrack(Track t, size_t offset, size_t length) {
        assert(isTrackNumber(t));
        for (unsigned i = 0; i < length; i++) writeBitToTrack(t, offset + i, 1);
    }
    
    /*! @brief   Write interblock gap
     */
    //void writeGap(uint8_t *dest, unsigned offset, unsigned length) {
    //     for (unsigned i = 0; i < length; i++) writeByte(dest, offset + i * 8, 0x55); }
    void writeGapToTrack(Track t, size_t offset, size_t length) {
        assert(isTrackNumber(t));
        for (unsigned i = 0; i < length; i++) writeByteToTrack(t, offset + i * 8, 0x55);
    }
    
    
    //
    //! @functiongroup Erasing data
    //
    
    /*! @brief    Clears the whole disk
     */
    void clearDisk();

    /*! @brief    Clears a single track
     */
    void clearTrack(Track t);


    /*! @brief    Clears a single halftrack
     */
    void clearHalftrack(Halftrack ht);


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
    //! @functiongroup Debugging disk data
    //
    
    /*! @brief   Returns a textual represention of halftrack data
     *  @details The starting position of the first bit is specified as an absolute position.
     *  @todo    Clean this up, it's ugly 
     */
    // const char *dataAbs(Halftrack ht, size_t start, size_t n);

    /*! @brief   Returns a textual represention of halftrack data
     *  @details The starting position of the first bit is specified as an absolute position.
     */
    /*
    const char *dataAbs(Halftrack ht, size_t start) {
        assert(isHalftrackNumber(ht));
        return dataAbs(ht, start, length.halftrack[ht]);
    }
 */
    
    /*! @brief Prints some track data 
     */
    // void dumpHalftrack(Halftrack ht, unsigned min = 0, unsigned max = UINT_MAX, unsigned highlight = UINT_MAX);
    
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
    
    
    //
    //! @functiongroup Decoding disk data
    //

public:
    
    //! @brief    Analyzes a track
    /*! @details  The start and end offsets of all sectors are determined and writes
     *            into variable trackLayout.
     */
    void analyzeHalftrack(Halftrack ht);
    void analyzeTrack(Track t);
    
private:
    
    void _analyzeTrack(uint8_t *data, uint16_t length);
    
public:
    
    //! @brief    Returns a sector layout from variable trackInfo
    SectorInfo sectorLayout(unsigned nr) {
        assert(nr < 22);
        return trackInfo.sectorInfo[nr];
    }
    
    //! @brief    Returns a textual representation of the data stored in trackInfo
    const char *trackDataAsString();

    //! @brief    Returns a textual representation of the data stored in trackInfo
    const char *sectorHeaderAsString(Sector nr);

    //! @brief    Returns a textual representation of the data stored in trackInfo
    const char *sectorDataAsString(Sector nr);

private:
    
    //! @brief    Returns a textual representation
    const char *sectorBytesAsString(uint8_t *buffer, size_t length);
    
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

};
    
#endif
