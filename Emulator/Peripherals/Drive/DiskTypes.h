// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "Aliases.h"
#include "Reflection.h"

//
// Constants
//

/* The VC1541 can move the drive head to 85 distinct positions (1 .. 85). Odd
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
static inline bool isSectorNumber(isize nr) { return nr <= highestSector; }

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

enum_long(DISK_TYPE)
{
    DISK_TYPE_SS_SD,   // Single-sided, single density (VC1541)
    DISK_TYPE_DS_SD    // Double-sided, single density (VC1571) (not supported)
};
typedef DISK_TYPE DiskType;

#ifdef __cplusplus
struct DiskTypeEnum : util::Reflection<DiskTypeEnum, DiskType> {
    
    static long min() { return 0; }
    static long max() { return DISK_TYPE_DS_SD; }
    static bool isValid(long value) { return value >= min() && value <= max(); }

    static const char *prefix() { return "DISK_TYPE"; }
    static const char *key(DiskType value)
    {
        switch (value) {
                
            case DISK_TYPE_SS_SD:  return "SS_SD";
            case DISK_TYPE_DS_SD:  return "DS_SD";
        }
        return "???";
    }
};
#endif

enum_long(CBM_FILE_TYPE)
{
    CBM_FILE_PRG,
    CBM_FILE_SEQ,
    CBM_FILE_USR,
    CBM_FILE_REL
};
typedef CBM_FILE_TYPE CBMFileType;

#ifdef __cplusplus
struct CBMFileTypeEnum : util::Reflection<CBMFileTypeEnum, CBMFileType> {
    
    static long min() { return 0; }
    static long max() { return CBM_FILE_REL; }
    static bool isValid(long value) { return value >= min() && value <= max(); }
    
    static const char *prefix() { return "CBM"; }
    static const char *key(CBMFileType value)
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
#endif


//
// Structures
//

/* Disk data
 *
 *    - The first valid track and halftrack number is 1
 *    - data.halftack[i] points to the first byte of halftrack i
 *    - data.track[i] points to the first byte of track i
 */

#ifdef __cplusplus
union DiskData
{
    struct
    {
        u8 _pad[maxBytesOnTrack];
        u8 halftrack[85][maxBytesOnTrack];
    };
    
    u8 track[43][2 * maxBytesOnTrack];
    
    template <class W>
    void operator<<(W& worker)
    {
        worker
        
        << track;
    }
};
#endif

/* Length of each halftrack in bits
 *
 *     - length.halftack[i] is the length of halftrack i
 *     - length.track[i][0] is the length of track i
 *     - length.track[i][1] is the length of halftrack above track i
 */

#ifdef __cplusplus
union DiskLength
{
    struct
    {
        u16 _pad;
        u16 halftrack[85];
    };
    
    u16 track[43][2];

    template <class W>
    void operator<<(W& worker)
    {
        worker
        
        << track;
    }
};
#endif
