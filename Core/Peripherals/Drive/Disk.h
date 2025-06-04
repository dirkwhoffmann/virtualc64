// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// This FILE is dual-licensed. You are free to choose between:
//
//     - The GNU General Public License v3 (or any later version)
//     - The Mozilla Public License v2
//
// SPDX-License-Identifier: GPL-3.0-or-later OR MPL-2.0
// -----------------------------------------------------------------------------

#pragma once

#include "DiskTypes.h"
#include "DiskAnalyzerTypes.h"
#include "FSTypes.h"
#include "SubComponent.h"
#include "PETName.h"


namespace vc64 {

class DiskAnalyzer;
class FileSystem;

class Disk final : public CoreObject {
    
    friend class Drive;
    
public:
    
    //
    // Constants and lookup tables
    //
    
    static const TrackDefaults trackDefaults[43];

    // GCR encoding table. Maps 4 data bits to 5 GCR bits.
    static constexpr u8 gcr[16] = {
        
        0x0a, 0x0b, 0x12, 0x13, /*  0 -  3 */
        0x0e, 0x0f, 0x16, 0x17, /*  4 -  7 */
        0x09, 0x19, 0x1a, 0x1b, /*  8 - 11 */
        0x0d, 0x1d, 0x1e, 0x15  /* 12 - 15 */
    };
    
    // Inverse GCR encoding table. Maps 5 GCR bits to 4 data bits.
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


    //
    // Disk properties
    //
    
private:
    
    // Write protection mark
    bool writeProtected = false;
    
    // Indicates whether data has been written (data would be lost on eject)
    bool modified = false;
    
    
    //
    // Disk data
    //

public:
    
    // Data information for each halftrack on this disk
    DiskData data = { };

    // Length information for each halftrack on this disk
    DiskLength length = { };

    
    //
    // Class functions
    //
    
public:
    
    // Returns the number of sectors stored in a certain track or halftrack
    static isize numberOfSectorsInTrack(Track t);
    static isize numberOfSectorsInHalftrack(Halftrack ht);

    // Returns the default speed zone of a track or halftrack
    static isize speedZoneOfTrack(Track t);
    static isize speedZoneOfHalftrack(Halftrack ht);

    // Checks if the given pair is a valid (half)track / sector combination
    static bool isValidTrackSectorPair(Track t, Sector s);
    static bool isValidHalftrackSectorPair(Halftrack ht, Sector s);
    
    
    //
    // Methods
    //
    
public:
    
    Disk();
    Disk(const fs::path &path, bool wp = false) throws { init(path, wp); }
    Disk(DOSType type, PETName<16> name, bool wp = false) { init(type, name, wp); }
    Disk(const FileSystem &device, bool wp = false) { init(device, wp); }
    Disk(const class G64File &g64, bool wp = false) { init(g64, wp); }
    Disk(const class D64File &d64, bool wp = false) throws { init(d64, wp); }
    Disk(class AnyCollection &archive, bool wp = false) throws { init(archive, wp); }
    Disk(SerReader &reader) throws { init(reader); }
    
private:
    
    void init(const fs::path &path, bool wp) throws;
    void init(DOSType type, PETName<16> name, bool wp);
    void init(const class FileSystem &device, bool wp);
    void init(const class G64File &g64, bool wp);
    void init(const class D64File &d64, bool wp) throws;
    void init(class AnyCollection &archive, bool wp) throws;
    void init(SerReader &reader) throws;

public:

    Disk& operator= (const Disk& other) {

        CLONE(writeProtected)
        CLONE(modified)
        CLONE(data)
        CLONE(length)

        return *this;
    }


    //
    // Methods from Serializable
    //

public:

    template <class T>
    void serialize(T& worker)
    {
        if (isResetter(worker)) return;

        worker

        << writeProtected
        << modified
        << data
        << length;
    }

    
    //
    // Methods from CoreObject and Dumpable
    //

private:

    const char *objectName() const override { return "Disk"; }
    void _dump(Category category, std::ostream& os) const override;


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
    void encodeGcr(u8 *values, isize length, Track t, HeadPos offset);

    
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
        return (data.halftrack[ht][pos >> 3] & (0x80 >> (pos & 7))) != 0;
    }
    u8 readBitFromHalftrack(Halftrack ht, HeadPos pos) const {
        return _readBitFromHalftrack(ht, wrap(ht, pos));
    }
    void _writeBitToHalftrack(Halftrack ht, HeadPos pos, bool bit) {
        assert(isValidHeadPos(ht, pos));
        if (bit) {
            data.halftrack[ht][pos >> 3] |= (0x0080 >> (pos & 7));
        } else {
            data.halftrack[ht][pos >> 3] &= (0xFF7F >> (pos & 7));
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
    void writeBitToHalftrack(Halftrack ht, HeadPos pos, bool bit, isize count) {
        for (isize i = 0; i < count; i++)
            writeBitToHalftrack(ht, pos++, bit);
    }
    void writeBitToTrack(Track t, HeadPos pos, bool bit, isize count) {
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
    void writeGapToHalftrack(Halftrack ht, HeadPos pos, isize length) {
        for (isize i = 0; i < length; i++, pos += 8)
            writeByteToHalftrack(ht, pos, 0x55);
    }
    void writeGapToTrack(Track t, HeadPos pos, isize length) {
        writeGapToHalftrack(2 * t - 1, pos, length);
    }

    // Clears a single halftrack
    void clearHalftrack(Halftrack ht);

    // Reverts to a factory-fresh disk
    void clearDisk();
    
    
    //
    // Analyzing the disk
    //

public:

    /* Checks whether a track or halftrack is cleared. Avoid calling these
     * methods frequently, because they scan the whole track.
     */
    bool trackIsEmpty(Track t) const;
    bool halftrackIsEmpty(Halftrack ht) const;
    isize nonemptyHalftracks() const;

    // Returns the length of a halftrack in bits
    isize lengthOfTrack(Track t) const;
    isize lengthOfHalftrack(Halftrack ht) const;

    
    //
    // Decoding disk data
    //
    
public:
    
    /* Converts the disk into a byte stream and returns the number of bytes
     * written. The byte stream is compatible with the D64 file format. By
     * passing a null pointer, a test run is performed. Test runs are used to
     * determine how many bytes will be written.
     */
    isize decodeDisk(u8 *dest);

private:
    
    isize decodeDisk(u8 *dest, isize numTracks, DiskAnalyzer &analyzer);
    isize decodeTrack(Track t, u8 *dest, DiskAnalyzer &analyzer);
    isize decodeHalfrack(Halftrack ht, u8 *dest, DiskAnalyzer &analyzer);
    isize decodeSector(Halftrack ht, isize offset, u8 *dest, DiskAnalyzer &analyzer);


    //
    // Encoding disk data
    //
    
public:
    
    // Encodes a G64 file
    void encodeG64(const G64File &a);
    
    /* Encodes a file system. The method creates sync marks, GRC encoded header
     * and data blocks, checksums and gaps. If alignTracks is true, the first
     * sector always starts at the beginning of a track.
     */
    void encode(const FileSystem &fs, bool alignTracks = false);
    
private:
    
    /* Encode a single track. This function translates the logical byte
     * sequence of a single track into the native VC1541 byte representation.
     * The native representation includes sync marks, GCR data etc.
     * 'tailGapEven' specifies the number of tail bytes follwowing sectors with
     * even sector numbers. 'tailGapOdd' specifies the number of tail bytes
     * follwowing sectors with odd sector numbers. The number of written bits
     * is returned.
     */
    isize encodeTrack(const FileSystem &fs, Track t, isize gap, HeadPos start);
    
    /* Encode a single sector. This function translates the logical byte
     * sequence of a single sector into the native VC1541 byte representation.
     * The sector is closed by 'gap' tail gap bytes. The number of written bits
     * is returned.
     */
    isize encodeSector(const FileSystem &fs, Track t, Sector sector, HeadPos start, isize gap);
};

}
 
