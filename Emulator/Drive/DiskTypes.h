//
//  Disk_types.h
//  V64
//
//  Created by Dirk Hoffmann on 04.06.18.
//

#ifndef DISK_TYPES_H
#define DISK_TYPES_H

#include <ctype.h>

/* Overview:
 *
 *                       -----------------------------------------------------------------
 * Track layout:         |  1  | 1.5 |  2  | 2.5 | ... |  35  | 35.5 | ... |  42  | 42.5 |
 *                       -----------------------------------------------------------------
 * Halftrack addressing: |  1  |  2  |  3  |  4  |     |  69  |  70  |     |  83  |  84  |
 * Track addressing:     |  1  |     |  2  |     |     |  35  |      |     |  42  |      |
 *                       -----------------------------------------------------------------
 */


//
// Constants
//

/*! @brief    Maximum number of tracks on a single disk.
 *  @note     Tracks are indexed from 1 to 42. There is no track 0!
 */
static const unsigned maxNumberOfTracks = 42;

/*! @brief    Maximum number of halftracks on a single disk.
 *  @note     Tracks are indexed from 1 to 84. There is no halftrack 0!
 */
static const unsigned maxNumberOfHalftracks = 84;

/*! @brief    Maximum number of sectors in a single track
 *  @details  Sectors are numbered from 0 to 20.
 */
static const unsigned maxNumberOfSectors = 21;

/*! @brief    Maximum number of bits stored on a single track.
 *  @details  Each track can store a maximum of 7928 bytes. The exact number depends on
 *            the track number (inner tracks contain fewer bytes) and the actual write
 *            speed of a drive.
 */
static const unsigned maxBytesOnTrack = 7928;

/*! @brief    Maximum number of bits stored on a single track.
 */
static const unsigned maxBitsOnTrack = maxBytesOnTrack * 8;

/*! @brief    Returns the average duration of a single bit in 1/10 nano seconds.
 *  @details  The returned value is the time span the drive head resists
 *            over a single bit.
 *  @note     The exact value depends on the speed zone and the drive's
 *            rotation speed. We assume a rotation speed of 300 rpm.
 */
 static const u64 averageBitTimeSpan[] = {
     4 * 10000, // 4 * 16/16 * 10^4 1/10 nsec
     4 * 9375,  // 4 * 15/16 * 10^4 1/10 nsec
     4 * 8750,  // 4 * 14/16 * 10^4 1/10 nsec
     4 * 8125   // 4 * 13/16 * 10^4 1/10 nsec
 };

/*! @brief    Average number of bits stored on a single track.
 *  @note     The values are based on a drive with 300 rotations per minute
 *            which means that a full rotation lasts 2.000.000.000 1/10 nsec.
 */
static const unsigned averageBitsOnTrack[4] = {
    50000, // 200.000.000.000 / averageBitTimeSpan[0]
    53333, // 200.000.000.000 / averageBitTimeSpan[1]
    57142, // 200.000.000.000 / averageBitTimeSpan[2]
    61528  // 200.000.000.000 / averageBitTimeSpan[3]
};

/*! @brief    Average number of bytes stored on a single track.
 *  @note     The values are based on a drive with 300 rotations per minute.
 */
static const unsigned averageBytesOnTrack[4] = {
    6250, // averageBitsOnTrack[0] / 8
    6666, // averageBitsOnTrack[1] / 8
    7142, // averageBitsOnTrack[2] / 8
    7692  // averageBitsOnTrack[3] / 8
};

/*! @brief    Size of a sector header block in bits.
 */
static const unsigned headerBlockSize = 10 * 8;

/*! @brief    Size of a sector data block in bits.
 *  @details  Each data block consists of 325 GCR bytes (coding 260 real bytes)
 */
static const unsigned dataBlockSize = 325 * 8;

/*! @brief    Maximum number of files that can be stored on a single disk
 *  @details  VC1541 DOS stores the directors on track 18 which contains 19 sectors.
 *            Sector 0 is reserved for the BAM. Each of the remaining sectors can
 *            hold up to 8 directory entries, summing um to a total of 144 items.
 */
// static const unsigned maxNumberOfFiles = 144;


//
// Types
//

/*! @brief    Data type for addressing full tracks on disk
 *  @see      Halftrack
 */
typedef unsigned Track;

/*! @brief    Checks if a given number is a valid track number
 */
static inline bool isTrackNumber(unsigned nr) { return 1 <= nr && nr <= maxNumberOfTracks; }

/*! @brief    Data type for addressing half and full tracks on disk
 *  @details  The VC1541 drive head can move between position 1 and 85.
 *            The odd numbers between 1 and 70 mark the 35 tracks that
 *            are used by VC1541 DOS. This means that DOS moves the
 *            drive head always two positions up or down. If programmed
 *            manually, the head can also be position on half tracks
 *            and on tracks beyond 35.
 *  @see      Track
 */
typedef unsigned Halftrack;

/*! @brief    Checks if a given number is a valid halftrack number
 */
static inline bool isHalftrackNumber(unsigned nr) { return 1 <= nr && nr <= maxNumberOfHalftracks; }

//! @brief    Data type for addressing sectors inside a track
typedef unsigned Sector;

//! @brief    Checks if a given number is a valid sector number
static inline bool isSectorNumber(unsigned nr) { return nr < maxNumberOfSectors; }

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

//! @brief    Data type for specifying the head position inside a track
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
