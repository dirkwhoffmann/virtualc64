// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "FSDescriptors.h"

FSDeviceDescriptor::FSDeviceDescriptor(DiskType type, FSVolumeType dos)
{
    assert(dos == FS_CBM_DOS);
 
    this->dos = dos;
    
    switch (type) {
            
        case DISK_SS_SD:
            numCyls = 35;
            numHeads = 1;
            break;
            
        case DISK_DS_SD:
            numCyls = 35;
            numHeads = 2;
            break;
            
        default:
            assert(false);
    }
}

bool
FSDeviceDescriptor::isTrackSectorPair(Track t, Sector s)
{
    return isTrackNr(t) && s >= 0 && s < numSectors(t);
}

u32
FSDeviceDescriptor::speedZone(Track t)
{
    assert(isTrackNr(t));
    
    return
    (t <= 17) ? 3 :
    (t <= 24) ? 2 :
    (t <= 30) ? 1 :
    (t <= 35) ? 0 :
    (t <= 52) ? 3 :     // Second side (DS disks only)
    (t <= 59) ? 2 :     // Second side (DS disks only)
    (t <= 65) ? 1 : 0;  // Second side (DS disks only)
}

u32
FSDeviceDescriptor::numSectors(Track t)
{
    switch (speedZone(t)) {

        case 0: return 17;
        case 1: return 18;
        case 2: return 19;
        case 3: return 21;
    }
    
    assert(false);
    return 0;
}

u32
FSDeviceDescriptor::numBlocks()
{
    u32 result = 0;
    
    for (u32 i = 1; i <= numTracks(); i++) {
        result += numSectors(i);
    }
    
    return result;
}

void
FSDeviceDescriptor::translateBlockNr(Block b, Track *t, Sector *s)
{
    for (Track i = 1; i <= numTracks(); i++) {

        u32 num = numSectors(i);
        
        if (b < num) {
            *t = i;
            *s = b;
            return;
        }
        
        b -= num;
    }
    
    assert(false);
}

void
FSDeviceDescriptor::translateBlockNr(Block b, Cylinder *c, Head *h, Sector *s)
{
    Track t;
    
    translateBlockNr(b, &t, s);
    
    if (t <= numCyls) {
        *h = 0; *c = t;
    } else {
        *h = 1; *c = t - numCyls;
    }
}

void
FSDeviceDescriptor::translateBlockNr(Block *b, Track t, Sector s)
{
    u32 cnt = 0;
    
    for (Track i = 1; i < t; i++) {
        cnt += numSectors(i);
    }
    
    *b = cnt;
}

void
FSDeviceDescriptor::translateBlockNr(Block *b, Cylinder c, Head h, Sector s)
{
    translateBlockNr(b, c + h * numTracks(), s);
}
