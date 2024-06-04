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

#include "Reflection.h"
#include "Serializable.h"
#include "DriveTypes.h"

namespace vc64 {

//
// Constants
//

/* The VC1541 can move the drive head to 84 distinct positions (1 .. 84). Odd
 * numbers correspond to "full tracks" and even numbers to "half tracks". The
 * full tracks between 1 and 70 correspond to the 35 tracks that are used by
 * VC1541 DOS. The Rom code always moves the drive head by two positions. When
 * programmed manually, the head can also be moved to even positions or
 * positions beyond 70 which correspoonds to the innermost full track used by
 * DOS.
 *
 * VirtualC64 provides two data types for specifying head positions: Track
 * and Halftrack. They are related as follows:
 *
 *            -----------------------------------------------------------------
 * Layout:    |  1  | 1.5 |  2  | 2.5 | ... |  35  | 35.5 | ... |  42  | 42.5 |
 *            -----------------------------------------------------------------
 *            -----------------------------------------------------------------
 * Halftrack: |  1  |  2  |  3  |  4  |     |  69  |  70  |     |  83  |  84  |
 * Track:     |  1  |     |  2  |     |     |  35  |      |     |  42  |      |
 *            -----------------------------------------------------------------
 */

// Highest track and halftrack number (numbering starts with 1)
static const isize highestTrack = 42;
static const isize highestHalftrack = 84;

// Highest sector number (numbering starts with 0)
static const isize highestSector = 20;

static inline bool isTrackNumber(isize nr) { return 1 <= nr && nr <= highestTrack; }
static inline bool isHalftrackNumber(isize nr) { return 1 <= nr && nr <= highestHalftrack; }
static inline bool isSectorNumber(isize nr) { return nr >= 0 && nr <= highestSector; }

/* Maximum number of bits and bytes stored on a single track. Each track can
 * store a maximum of 7928 bytes (63424 bits). The exact number depends on the
 * track number (inner tracks contain fewer bytes) and the actual write speed
 * of a drive.
 */
static const isize maxBytesOnTrack = 7928;
static const isize maxBitsOnTrack = maxBytesOnTrack * 8;

/* Returns the average duration of a single bit in 1/10 nano seconds. The
 * returned value is the time span the drive head resists over a single bit.
 * The exact value depends on the speed zone and the disk rotation speed. We
 * assume a rotation speed of 300 rpm.
 */
static const i64 averageBitTimeSpan[] =
{
    4 * 10000,        // 4 * 16/16 * 10^4 1/10 nsec
    4 * 9375,         // 4 * 15/16 * 10^4 1/10 nsec
    4 * 8750,         // 4 * 14/16 * 10^4 1/10 nsec
    4 * 8125          // 4 * 13/16 * 10^4 1/10 nsec
};

/* Average number of bits stored on a single track. The values are based on a
 * drive with 300 rotations per minute which means that a full rotation lasts
 * 2.000.000.000 1/10 nsec.
 */
static const isize averageBitsOnTrack[4] =
{
    50000,             // 200.000.000.000 / averageBitTimeSpan[0]
    53333,             // 200.000.000.000 / averageBitTimeSpan[1]
    57142,             // 200.000.000.000 / averageBitTimeSpan[2]
    61528              // 200.000.000.000 / averageBitTimeSpan[3]
};

/* Average number of bytes stored on a single track. The values are based on a
 * drive with 300 rotations per minute.
 */
static const isize averageBytesOnTrack[4] =
{
    6250,              // averageBitsOnTrack[0] / 8
    6666,              // averageBitsOnTrack[1] / 8
    7142,              // averageBitsOnTrack[2] / 8
    7692               // averageBitsOnTrack[3] / 8
};

/* Size of a sector header or data block in bits. Each data block consists of
 * 325 GCR bytes (coding 260 real bytes).
 */
static const isize headerBlockSize = 10 * 8;
static const isize dataBlockSize = 325 * 8;


//
// Enumerations
//

/// Disk type
enum_long(DISK_TYPE)
{
    DISK_TYPE_SS_SD,   ///< Single-sided, single density (VC1541)
    DISK_TYPE_DS_SD    ///< Double-sided, single density (VC1571) (unsupported)
};
typedef DISK_TYPE DiskType;

struct DiskTypeEnum : util::Reflection<DiskTypeEnum, DiskType> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = DISK_TYPE_DS_SD;
    static bool isValid(auto value) { return value >= minVal && value <= maxVal; }

    static const char *prefix() { return "DISK_TYPE"; }
    static const char *key(long value)
    {
        switch (value) {

            case DISK_TYPE_SS_SD:  return "SS_SD";
            case DISK_TYPE_DS_SD:  return "DS_SD";
        }
        return "???";
    }
};

enum_long(CBM_FILE_TYPE)
{
    CBM_FILE_PRG,
    CBM_FILE_SEQ,
    CBM_FILE_USR,
    CBM_FILE_REL
};
typedef CBM_FILE_TYPE CBMFileType;

struct CBMFileTypeEnum : util::Reflection<CBMFileTypeEnum, CBMFileType> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = CBM_FILE_REL;
    static bool isValid(auto value) { return value >= minVal && value <= maxVal; }

    static const char *prefix() { return "CBM"; }
    static const char *key(long value)
    {
        switch (value) {

            case CBM_FILE_PRG:    return "PRG";
            case CBM_FILE_SEQ:    return "SEQ";
            case CBM_FILE_USR:    return "USR";
            case CBM_FILE_REL:    return "REL";
        }
        return "???";
    }
};

enum_long(DISK_ERROR_CODE)
{
    DISK_OK                            = 0x1,
    HEADER_BLOCK_NOT_FOUND_ERROR       = 0x2,
    NO_SYNC_SEQUENCE_ERROR             = 0x3,
    DATA_BLOCK_NOT_FOUND_ERROR         = 0x4,
    DATA_BLOCK_CHECKSUM_ERROR          = 0x5,
    WRITE_VERIFY_ERROR_ON_FORMAT_ERROR = 0x6,
    WRITE_VERIFY_ERROR                 = 0x7,
    WRITE_PROTECT_ON_ERROR             = 0x8,
    HEADER_BLOCK_CHECKSUM_ERROR        = 0x9,
    WRITE_ERROR                        = 0xA,
    DISK_ID_MISMATCH_ERROR             = 0xB,
    DRIVE_NOT_READY_ERRROR             = 0xF
};
typedef DISK_ERROR_CODE DiskErrorCode;

struct DiskErrorCodeEnum : util::Reflection<DiskErrorCodeEnum, DiskErrorCode> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = DRIVE_NOT_READY_ERRROR;
    static bool isValid(auto value) { return value >= minVal && value <= maxVal; }

    static const char *prefix() { return ""; }
    static const char *key(long value)
    {
        switch (value) {

            case DISK_OK:                            return "DISK_OK";
            case HEADER_BLOCK_NOT_FOUND_ERROR:       return "HEADER_BLOCK_NOT_FOUND_ERROR";
            case NO_SYNC_SEQUENCE_ERROR:             return "NO_SYNC_SEQUENCE_ERROR";
            case DATA_BLOCK_NOT_FOUND_ERROR:         return "DATA_BLOCK_NOT_FOUND_ERROR";
            case DATA_BLOCK_CHECKSUM_ERROR:          return "DATA_BLOCK_CHECKSUM_ERROR";
            case WRITE_VERIFY_ERROR_ON_FORMAT_ERROR: return "WRITE_VERIFY_ERROR_ON_FORMAT_ERROR";
            case WRITE_VERIFY_ERROR:                 return "WRITE_VERIFY_ERROR";
            case WRITE_PROTECT_ON_ERROR:             return "WRITE_PROTECT_ON_ERROR";
            case HEADER_BLOCK_CHECKSUM_ERROR:        return "HEADER_BLOCK_CHECKSUM_ERROR";
            case WRITE_ERROR:                        return "WRITE_ERROR";
            case DISK_ID_MISMATCH_ERROR:             return "DISK_ID_MISMATCH_ERROR";
            case DRIVE_NOT_READY_ERRROR:             return "DRIVE_NOT_READY_ERRROR";
        }
        return "???";
    }
};


//
// Structures
//

/* Disk data
 *
 *    - The first valid track and halftrack number is 1
 *    - data.halftack[i] points to the first byte of halftrack i
 *    - data.track[i] points to the first byte of track i
 */
struct DiskData : public Serializable
{
    union {

        struct
        {
            u8 _pad[maxBytesOnTrack];
            u8 halftrack[85][maxBytesOnTrack];
        };

        u8 track[43][2 * maxBytesOnTrack];
    };

    template <class W>
    void serialize(W& worker)
    {
        worker

        << track;

    } SERIALIZERS(serialize);

};

/* Length of each halftrack in bits
 *
 *     - length.halftack[i] is the length of halftrack i
 *     - length.track[i][0] is the length of track i
 *     - length.track[i][1] is the length of halftrack above track i
 */

struct DiskLength : public Serializable
{
    union {

        struct
        {
            isize _pad;
            isize halftrack[85];
        };

        isize track[43][2];
    };

    template <class W>
    void serialize(W& worker)
    {
        worker

        << track;

    } SERIALIZERS(serialize);
};

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

}
