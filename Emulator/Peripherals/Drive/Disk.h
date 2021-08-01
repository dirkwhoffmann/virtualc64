// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "DiskTypes.h"
#include "FSTypes.h"
#include "SubComponent.h"
#include "PETName.h"

class Disk : public SubComponent {
    
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
    static constexpr u8 gcr[16] = {
        
        0x0a, 0x0b, 0x12, 0x13, /*  0 -  3 */
        0x0e, 0x0f, 0x16, 0x17, /*  4 -  7 */
        0x09, 0x19, 0x1a, 0x1b, /*  8 - 11 */
        0x0d, 0x1d, 0x1e, 0x15  /* 12 - 15 */
    };
    
    /* Inverse GCR encoding table. Maps 5 GCR bits to 4 data bits. Invalid
     * patterns are marked with 255.
     */
    static constexpr u8 invgcr[32] = {
        
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
    bool writeProtected = false;
    
    /* Indicates whether data has been written. Depending on this flag, the GUI
     * shows a warning dialog before a disk gets ejected.
     */
    bool modified = false;
    
    
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
    std::vector<usize> errorStartIndex;

    // Stores the end offset of the erroneous bit sequence
    std::vector<usize> errorEndIndex;

    // Textual representation of track data
    char text[maxBitsOnTrack + 1];
    
    
    //
    // Class functions
    //
    
public:
    
    // Returns the number of sectors stored in a certain track or halftrack
    static unsigned numberOfSectorsInTrack(Track t);
    static unsigned numberOfSectorsInHalftrack(Halftrack ht);

    // Returns the default speed zone of a track or halftrack
    static unsigned speedZoneOfTrack(Track t);
    static unsigned speedZoneOfHalftrack(Halftrack ht);

    // Checks if the given pair is a valid (half)track / sector combination
    static bool isValidTrackSectorPair(Track t, Sector s);
    static bool isValidHalftrackSectorPair(Halftrack ht, Sector s);
    
    
    //
    // Creating
    //
    
public:
    
    static Disk *make(C64 &ref, const string &path) throws;
    static Disk *make(C64 &ref, DOSType type, PETName<16> name);
    static Disk *makeWithFileSystem(C64 &ref, class FSDevice &device);
    static Disk *makeWithG64(C64 &ref, G64File *g64);
    static Disk *makeWithD64(C64 &ref, D64File *d64);
    static Disk *makeWithCollection(C64 &ref, AnyCollection &archive);


    //
    // Initializing
    //
    
public:
    
    Disk(C64 &ref);
    const char *getDescription() const override { return "Disk"; }
    
private:
    
    void _reset(bool hard) override;

    
    //
    // Analyzing
    //
    
private:
    
    void _dump(dump::Category category, std::ostream& os) const override;

    
    //
    // Serializing
    //
    
private:
    
    template <class T>
    void applyToPersistentItems(T& worker)
    {
        worker
        
        << writeProtected
        << modified
        >> data
        >> length;
    }
    
    template <class T>
    void applyToResetItems(T& worker, bool hard = true)
    {
    }
    
    isize _size() override { COMPUTE_SNAPSHOT_SIZE }
    isize _load(const u8 *buffer) override { LOAD_SNAPSHOT_ITEMS }
    isize _save(u8 *buffer) override { SAVE_SNAPSHOT_ITEMS }
    
    
    //
    // Accessing
    //

public:
    
    bool isWriteProtected() const { return writeProtected; }
    void setWriteProtection(bool b) { writeProtected = b; }
    void toggleWriteProtection() { writeProtected = !writeProtected; }

    bool isModified() const { return modified; }
    void setModified(bool b);
    
    
    //
    // Handling GCR encoded data
    //
    
public:
    
    // Converts a 4 bit binary value to a 5 bit GCR codeword or vice versa
    static u8 bin2gcr(u8 value) { assert(value < 16); return gcr[value]; }
    static u8 gcr2bin(u8 value) { assert(value < 32); return invgcr[value]; }

    // Returns true if the provided 5 bit codeword is a valid GCR codeword
    static bool isGcr(u8 value) { assert(value < 32); return invgcr[value] != 0xFF; }
    
    /* Encodes a byte stream as a GCR bit stream. The first function encodes
     * a single byte and the second functions encodes multiple bytes. For each
     * byte, 10 bits are written to the specified disk position.
     */
    void encodeGcr(u8 value, Track t, HeadPos offset);
    void encodeGcr(u8 *values, usize length, Track t, HeadPos offset);
    
    
    /* Decodes a nibble (4 bit) from a previously encoded GCR bitstream.
     * Returns 0xFF, if no valid GCR sequence is found.
     */
    u8 decodeGcrNibble(u8 *gcrBits);

    /* Decodes a byte (8 bit) form a previously encoded GCR bitstream. Returns
     * an unpredictable result if invalid GCR sequences are found.
     */
    u8 decodeGcr(u8 *gcrBits);

    
    //
    // Accessing disk data
    //
    
    // Returns true if the provided drive head position is valid
    bool isValidHeadPos(Halftrack ht, HeadPos pos) const;
    
    // Fixes a wrapped over head position
    HeadPos wrap(Halftrack ht, HeadPos pos) const;
    
    /* Returns the duration of a single bit in 1/10 nano seconds. The returned
     * value is the time span the drive head resists over the specified bit.
     * The value is determined by the the density bits at the time the bit was
     * written to disk. Function "_bitDelay" expects the head position to be
     * inside the halftrack bounds.
     */
    u64 _bitDelay(Halftrack ht, HeadPos pos) const;
    u64 bitDelay(Halftrack ht, HeadPos pos) const { return _bitDelay(ht, wrap(ht, pos)); }
    
    /* Reads or writes a single bit. The functions come in two variants. The
     * first variants expect the provided head position inside the valid
     * halftrack bounds. The other variants wrap over the head position first.
     */
    u8 _readBitFromHalftrack(Halftrack ht, HeadPos pos) const {
        assert(isValidHeadPos(ht, pos));
        return (data.halftrack[ht][pos / 8] & (0x80 >> (pos % 8))) != 0;
    }
    u8 readBitFromHalftrack(Halftrack ht, HeadPos pos) const {
        return _readBitFromHalftrack(ht, wrap(ht, pos));
    }
    void _writeBitToHalftrack(Halftrack ht, HeadPos pos, bool bit) {
        assert(isValidHeadPos(ht, pos));
        if (bit) {
            data.halftrack[ht][pos / 8] |= (0x0080 >> (pos % 8));
        } else {
            data.halftrack[ht][pos / 8] &= (0xFF7F >> (pos % 8));
        }
    }
    void _writeBitToTrack(Track t, HeadPos pos, bool bit) {
        _writeBitToHalftrack(2 * t - 1, pos, bit);
    }
    void writeBitToHalftrack(Halftrack ht, HeadPos pos, bool bit) {
        _writeBitToHalftrack(ht, wrap(ht, pos), bit);
    }
    void writeBitToTrack(Track t, HeadPos pos, bool bit) {
        _writeBitToHalftrack(2 * t - 1, pos, bit);
    }
    
    // Writes a bit multiple times
    void writeBitToHalftrack(Halftrack ht, HeadPos pos, bool bit, usize count) {
        for (usize i = 0; i < count; i++)
            writeBitToHalftrack(ht, pos++, bit);
    }
    void writeBitToTrack(Track t, HeadPos pos, bool bit, usize count) {
            writeBitToHalftrack(2 * t - 1, pos, bit, count);
    }

    // Writes a single byte
    void writeByteToHalftrack(Halftrack ht, HeadPos pos, u8 byte) {
        for (u8 mask = 0x80; mask != 0; mask >>= 1)
            writeBitToHalftrack(ht, pos++, byte & mask);
    }
    void writeByteToTrack(Track t, HeadPos pos, u8 byte) {
        writeByteToHalftrack(2 * t - 1, pos, byte);
    }
    
    // Writes a certain number of interblock bytes to disk
    void writeGapToHalftrack(Halftrack ht, HeadPos pos, usize length) {
        for (usize i = 0; i < length; i++, pos += 8)
            writeByteToHalftrack(ht, pos, 0x55);
    }
    void writeGapToTrack(Track t, HeadPos pos, usize length) {
        writeGapToHalftrack(2 * t - 1, pos, length);
    }

    // Clears a single halftrack
    void clearHalftrack(Halftrack ht); 

    /* Reverts to a factory-new disk. All disk data gets erased and the copy
     * protection mark removed.
     */
    void clearDisk();
    
    /* Checks whether a track or halftrack is cleared. Avoid calling these
     * methods frequently, because they scan the whole track.
     */
    bool trackIsEmpty(Track t) const;
    bool halftrackIsEmpty(Halftrack ht) const;
    unsigned nonemptyHalftracks() const;

    
    //
    // Analyzing the disk
    //

public:
    
    // Returns the length of a halftrack in bits
    u16 lengthOfHalftrack(Halftrack ht) const;
    u16 lengthOfTrack(Track t) const;
    
    /* Analyzes the sector layout. The functions determines the start and end
     * offsets of all sectors and writes them into variable trackLayout.
     */
    void analyzeHalftrack(Halftrack ht);
    void analyzeTrack(Track t);
    
private:
    
    // Checks the integrity of a sector header or sector data block
    void analyzeSectorHeaderBlock(usize offset);
    void analyzeSectorDataBlock(usize offset);

    // Writes an error message into the error log
    void log(usize begin, usize length, const char *fmt, ...);
    
public:
    
    // Returns a sector layout from variable trackInfo
    SectorInfo sectorLayout(Sector nr) {
        assert(isSectorNumber(nr)); return trackInfo.sectorInfo[nr]; }
    
    // Returns the number of entries in the error log
    usize numErrors() { return errorLog.size(); }
    
    // Reads an error message from the error log
    std::string errorMessage(unsigned nr) const { return errorLog.at(nr); }
    
    // Reads the error begin index from the error log
    usize firstErroneousBit(unsigned nr) const { return errorStartIndex.at(nr); }
    
    // Reads the error end index from the error log
    usize lastErroneousBit(unsigned nr) const { return errorEndIndex.at(nr); }
    
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
    const char *sectorBytesAsString(u8 *buffer, usize length, bool hex);
    
    
    //
    // Decoding disk data
    //
    
public:
    
    /* Converts the disk into a byte stream and returns the number of bytes
     * written. The byte stream is compatible with the D64 file format. By
     * passing a null pointer, a test run is performed. Test runs are used to
     * determine how many bytes will be written.
     */
    usize decodeDisk(u8 *dest);
 
private:
    
    usize decodeDisk(u8 *dest, unsigned numTracks);
    usize decodeTrack(Track t, u8 *dest);
    usize decodeSector(usize offset, u8 *dest);


    //
    // Encoding disk data
    //
    
public:
    
    // Encodes a G64 file
    void encodeG64(G64File *a);
    
    /* Encodes a file system. The method creates sync marks, GRC encoded header
     * and data blocks, checksums and gaps. If alignTracks is true, the first
     * sector always starts at the beginning of a track.
     */
    void encode(FSDevice &fs, bool alignTracks = false);
    
private:
    
    /* Encode a single track. This function translates the logical byte
     * sequence of a single track into the native VC1541 byte representation.
     * The native representation includes sync marks, GCR data etc.
     * 'tailGapEven' specifies the number of tail bytes follwowing sectors with
     * even sector numbers. 'tailGapOdd' specifies the number of tail bytes
     * follwowing sectors with odd sector numbers. The number of written bits
     * is returned.
     */
    usize encodeTrack(FSDevice &fs, Track t, u8 tailGap, HeadPos start);
    
    /* Encode a single sector. This function translates the logical byte
     * sequence of a single sector into the native VC1541 byte representation.
     * The sector is closed by 'gap' tail gap bytes. The number of written bits
     * is returned.
     */
    usize encodeSector(FSDevice &fs, Track t, Sector sector, HeadPos start, int gap);
};
 
