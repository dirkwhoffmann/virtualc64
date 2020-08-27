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

// Highest track and halftrack number (numbering starts with 1)
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

// Data types for addressing full tracks, half tracks, and sectors
typedef unsigned Track;
static inline bool isTrackNumber(unsigned nr) { return 1 <= nr && nr <= highestTrack; }

typedef unsigned Halftrack;
static inline bool isHalftrackNumber(unsigned nr) { return 1 <= nr && nr <= highestHalftrack; }

typedef unsigned Sector;
static inline bool isSectorNumber(unsigned nr) { return nr <= highestSector; }

//! @brief    Returns the number of sectors stored in a track
static inline unsigned numberOfSectorsInTrack(Track t) {
    return (t < 1) ? 0 : (t < 18) ? 21 : (t < 25) ? 19 : (t < 31) ? 18 : (t < 43) ? 17 : 0; }

//! @brief    Returns the number of sectors stored in a halftrack
static inline unsigned numberOfSectorsInHalftrack(Halftrack ht) {
    return numberOfSectorsInTrack((ht + 1) / 2); }

//! @brief    Returns the default speed zone of a track
static inline unsigned speedZoneOfTrack(Track t) {
    return (t < 18) ? 3 : (t < 25) ? 2 : (t < 31) ? 1 : 0; }

//! @brief    Returns the default speed zone of a halftrack
static inline unsigned speedZoneOfHalftrack(Halftrack ht) {
    return (ht < 35) ? 3 : (ht < 49) ? 2 : (ht < 61) ? 1 : 0; }

//! @brief    Checks if the given pair is a valid track / sector combination
static inline bool isValidTrackSectorPair(Track t, Sector s) {
    return s < numberOfSectorsInTrack(t);
}

//! @brief    Checks if the given pair is a valid halftrack / sector combination
static inline bool isValidHalftrackSectorPair(Halftrack ht, Sector s) {
    return s < numberOfSectorsInHalftrack(ht);
}

// Data type for specifying the head position inside a track
typedef i32 HeadPosition;

//! @brief    Layout information of a single sector
typedef struct {
    size_t headerBegin;
    size_t headerEnd;
    size_t dataBegin;
    size_t dataEnd;
} SectorInfo;

//! @brief    Information about a single track as gathered by analyzeTrack()
/*! @note     To provide a fast access, the the track data is stored as a byte stream.
 *            Each byte represents a single bit and is either 0 or 1. The stored sequence
 *            is repeated twice to ease the handling of wrap arounds.
 */
typedef struct {
    
    // Length of the track in bits
    size_t length;
    
    // Track data
    union {
        u8 bit[2 * maxBitsOnTrack];
        u64 byte[2 * maxBytesOnTrack];
    };

    // Sector layout data
    SectorInfo sectorInfo[22];
    
} TrackInfo;

#endif
