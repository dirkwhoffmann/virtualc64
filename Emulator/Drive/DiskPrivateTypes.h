// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef DISK_PRIVATE_TYPES_H
#define DISK_PRIVATE_TYPES_H

/* Disk data
 *
 *    - The first valid track and halftrack number is 1
 *    - data.halftack[i] points to the first byte of halftrack i
 *    - data.track[i] points to the first byte of track i
 */
union DiskData
{
    struct
    {
        u8 _pad[maxBytesOnTrack];
        u8 halftrack[85][maxBytesOnTrack];
    };
    
    u8 track[43][2 * maxBytesOnTrack];

    template <class T>
    void applyToItems(T& worker)
    {
        worker
        
        & track;
    }
};


/* Length of each halftrack in bits
 *
 *     - length.halftack[i] is the length of halftrack i
 *     - length.track[i][0] is the length of track i
 *     - length.track[i][1] is the length of halftrack above track i
 */

union DiskLength
{
    struct
    {
        u16 _pad;
        u16 halftrack[85];
    };
    
    u16 track[43][2];

    template <class T>
    void applyToItems(T& worker)
    {
        worker
        
        & track;
    }
};

#endif
