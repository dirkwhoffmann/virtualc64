// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef DISK_PRIVATE_H
#define DISK_PRIVATE_H

//
// Reflection APIs
//

struct DiskTypeEnum : Reflection<DiskTypeEnum, DiskType> {
    
    static bool isValid(long value)
    {
        return (unsigned long)value <= DISK_TYPE_DS_SD;
    }
    
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

struct CBMFileTypeEnum : Reflection<CBMFileTypeEnum, CBMFileType> {
    
    static bool isValid(long value)
    {
        return (unsigned long)value <= CBM_REL;
    }
    
    static const char *prefix() { return "CBM"; }
    static const char *key(CBMFileType value)
    {
        switch (value) {
                
            case CBM_PRG:  return "PRG";
            case CBM_SEQ:  return "SEQ";
            case CBM_USR:  return "USR";
            case CBM_REL:  return "REL";
        }
        return "???";
    }
};

//
// Private types
//

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