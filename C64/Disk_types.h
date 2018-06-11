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
const unsigned maxNumberOfTracks = 42;

/*! @brief    Maximum number of halftracks on a single disk.
 *  @note     Tracks are indexed from 1 to 84. There is no halftrack 0!
 */
const unsigned maxNumberOfHalftracks = 84;

/*! @brief    Maximum number of sectors in a single track
 *  @details  Sectors are numbered from 0 to 20.
 */
const unsigned maxNumberOfSectors = 21;

/*! @brief    Maximum number of bits stored on a single track.
 *  @details  Each track can store a maximum of 7928 bytes. The exact number depends on
 *            the track number (inner tracks contain fewer bytes) and the actual write
 *            speed of a drive.
 */
const unsigned maxBytesOnTrack = 7928;

/*! @brief    Maximum number of bits stored on a single track.
 */
const unsigned maxBitsOnTrack = maxBytesOnTrack * 8;

/*! @brief    Size of a sector header block in bits.
 */
const unsigned headerBlockSize = 10 * 8;

//! @brief    Size of a sector data block in bits.
/*! @details  Each data block consists of 325 GCR bytes (coding 260 real bytes)
 */
const unsigned dataBlockSize = 325 * 8;

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
inline bool isTrackNumber(unsigned nr) { return 1 <= nr && nr <= maxNumberOfTracks; }

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
inline bool isHalftrackNumber(unsigned nr) { return 1 <= nr && nr <= maxNumberOfHalftracks; }

/*! @brief    Data type for addressing sectors inside a track
 */
typedef unsigned Sector;

/*! @brief    Checks if a given number is a valid sector number
 */
inline bool isSectorNumber(unsigned nr) { return nr < maxNumberOfSectors; }

/*! @brief    Data type for specifying the head position inside a track
 */
typedef int32_t HeadPosition;


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
    size_t length; 
    union {
        uint8_t bit[2 * maxBitsOnTrack];
        uint64_t byte[2 * maxBytesOnTrack];
    };
    SectorInfo sectorInfo[22];
} TrackInfo;


#endif
