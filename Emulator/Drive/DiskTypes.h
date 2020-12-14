// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef DISK_TYPES_H
#define DISK_TYPES_H

/* Overview:
 *
 * The VC1541 can move the drive head to 85 distinct positions (1 .. 85). Odd
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

//
// Enumerations
//

longenum(DiskType)
{
    DISK_SS_SD,   // Single-sided, single density (VC1541)
    DISK_DS_SD    // Double-sided, single density (VC1571) (not supported)
};

inline bool isDiskType(DiskType value)
{
    return value >= 0 && value <= DISK_DS_SD;
}

inline const char *sDiskType(DiskType value)
{
    assert(isDiskType(value));
    
    switch (value) {
        case DISK_SS_SD:  return "SS_SD";
        case DISK_DS_SD:  return "DS_SD";
        default:          return "???";
    }
}

longenum(CBMFileType)
{
    CBM_PRG,
    CBM_SEQ,
    CBM_USR,
    CBM_REL
};

inline bool isCBMFileType(long value)
{
    return value >= CBM_PRG && value <= CBM_REL;
}

inline const char *CBMFileTypeString(CBMFileType type)
{
    switch (type) {
            
        case CBM_PRG: return "PRG";
        case CBM_SEQ: return "SEQ";
        case CBM_USR: return "USR";
        case CBM_REL: return "REL";
        default: assert(false);
    }
}

typedef enum : long
{
    FS_NONE,
    FS_COMMODORE
}
FileSystemType;

inline bool isFileSystemType(long value)
{
    return value >= FS_NONE && value <= FS_COMMODORE;
}


//
// Constants
//

// Track and halftrack number (numbering starts with 1)
static const unsigned highestTrack = 42;
static const unsigned highestHalftrack = 84;

// Highest sector number (numbering starts with 0)
static const unsigned highestSector = 20;

/* Maximum number of bits and bytes stored on a single track. Each track can
 * store a maximum of 7928 bytes (63424 bits). The exact number depends on the
 * track number (inner tracks contain fewer bytes) and the actual write speed
 * of a drive.
 */
static const unsigned maxBytesOnTrack = 7928;
static const unsigned maxBitsOnTrack = maxBytesOnTrack * 8;

/* Returns the average duration of a single bit in 1/10 nano seconds. The
 * returned value is the time span the drive head resists over a single bit.
 * The exact value depends on the speed zone and the disk rotation speed. We
 * assume a rotation speed of 300 rpm.
 */
 static const u64 averageBitTimeSpan[] =
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
static const unsigned averageBitsOnTrack[4] =
{
    50000,             // 200.000.000.000 / averageBitTimeSpan[0]
    53333,             // 200.000.000.000 / averageBitTimeSpan[1]
    57142,             // 200.000.000.000 / averageBitTimeSpan[2]
    61528              // 200.000.000.000 / averageBitTimeSpan[3]
};

/* Average number of bytes stored on a single track. The values are based on a
 * drive with 300 rotations per minute.
 */
static const unsigned averageBytesOnTrack[4] =
{
    6250,              // averageBitsOnTrack[0] / 8
    6666,              // averageBitsOnTrack[1] / 8
    7142,              // averageBitsOnTrack[2] / 8
    7692               // averageBitsOnTrack[3] / 8
};

/* Size of a sector header or data block in bits. Each data block consists of
 * 325 GCR bytes (coding 260 real bytes).
 */
static const unsigned headerBlockSize = 10 * 8;
static const unsigned dataBlockSize = 325 * 8;


//
// Types
//

static inline bool isTrackNumber(unsigned nr) { return 1 <= nr && nr <= highestTrack; }
static inline bool isHalftrackNumber(unsigned nr) { return 1 <= nr && nr <= highestHalftrack; }
static inline bool isSectorNumber(unsigned nr) { return nr <= highestSector; }

typedef i32 HeadPos;


//
// Structures
//

/* Information about a single sector as gathered by analyzeSector()
 */
typedef struct
{
    size_t headerBegin;
    size_t headerEnd;
    size_t dataBegin;
    size_t dataEnd;
}
SectorInfo;

/* Information about a single track as gathered by analyzeTrack(). To provide a
 * fast access, the the track data is stored as a byte stream. Each byte
 * represents a single bit and is either 0 or 1. The stored sequence is
 * repeated twice to ease the handling of wrap arounds.
 */
typedef struct
{
    size_t length;                       // Length of the track in bits
    
    union {
        u8 bit[2 * maxBitsOnTrack];      // Track data (bit access)
        u64 byte[2 * maxBytesOnTrack];   // Track data (byte access)
    };
    
    SectorInfo sectorInfo[22];           // Sector layout data
}
TrackInfo;

#endif
