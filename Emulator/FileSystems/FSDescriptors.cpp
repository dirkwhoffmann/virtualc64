// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

// #include "FSDescriptors.h"
#include "C64.h"

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

/*
FSDeviceDescriptor::FSDeviceDescriptor(class D64File *d64)
{
    dos = FS_CBM_DOS;
    numCyls = d64->numberOfTracks();
    numHeads = 1;
}

FSDeviceDescriptor::FSDeviceDescriptor(class Disk *disk)
{
    dos = FS_CBM_DOS;
    numCyls = (disk->nonemptyHalftracks() + 1) / 2;
    numCyls = MAX(35, MIN(numCyls, 42));
    numHeads = 1;
}
*/

bool
FSDeviceDescriptor::isTrackSectorPair(Track t, Sector s)
{
    return isTrackNr(t) && s >= 0 && s < numSectors(t);
}

bool
FSDeviceDescriptor::isValidRef(TSLink ref)
{
    return isTrackNr(ref.t) && ref.s >= 0 && ref.s < numSectors(ref.t);
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
    if (!isTrackNr(t)) return 0;
    
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

Cylinder
FSDeviceDescriptor::cylNr(Track t)
{
    return t <= numCyls ? t : t - numCyls;
}

Head
FSDeviceDescriptor::headNr(Track t)
{
    return t <= numCyls ? 0 : 1;
}

Track
FSDeviceDescriptor::trackNr(Cylinder c, Head h)
{
    return c + h * numTracks();
}

Track
FSDeviceDescriptor::trackNr(Block b)
{
    Track t; Sector s;
    translateBlockNr(b, &t, &s);
    return t;
}

Sector
FSDeviceDescriptor::sectorNr(Block b)
{
    Track t; Sector s;
    translateBlockNr(b, &t, &s);
    return s;
}

TSLink
FSDeviceDescriptor::tsLink(Block b)
{
    Track t; Sector s;
    translateBlockNr(b, &t, &s);
    return TSLink { t, s };
}

Block
FSDeviceDescriptor::blockNr(Track t, Sector s)
{
    Block b;
    translateBlockNr(&b, t, s);
    return b;
}

Block
FSDeviceDescriptor::blockNr(Cylinder c, Head h, Sector s)
{
    Block b;
    translateBlockNr(&b, c, h, s);
    return b;
}

Block
FSDeviceDescriptor::blockNr(TSLink ts)
{
    return blockNr(ts.t, ts.s);
}

void
FSDeviceDescriptor::translateBlockNr(Block b, Track *t, Sector *s)
{
    for (Track i = 1; i <= numTracks(); i++) {

        u32 num = numSectors(i);
        if (b < num) { *t = i; *s = b; return; }
        b -= num;
    }
    
    *t = *s = 0; // Invalid
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
    if (!isTrackSectorPair(t, s)) { *b = (Block)(-1); return; }
    
    u32 cnt = s;
    
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

TSLink
FSDeviceDescriptor::nextBlockRef(TSLink ref)
{
    assert(isValidRef(ref));
    
    Track t = ref.t;
    Sector s = ref.s;
    
    // Lookup table for the next sector (interleave patterns)
    Sector next[5][21] = {

        // Speed zone 0 - 3
        { 10,11,12,13,14,15,16, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 },
        { 10,11,12,13,14,15,16,17, 1, 0, 2, 3, 4, 5, 6, 7, 8, 9 },
        { 10,11,12,13,14,15,16,17,18, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9},
        { 10,11,12,13,14,15,16,17,18,19,20, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 },
        
        // Directory track
        {  3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18, 0, 1, 2 }
    };

    if (t == 18) {

        // Take care of the directory track
        s = next[4][s];

        // Return immediately if we've wrapped over (directory track is full)
        if (s == 0) return {0,0};
    
    } else {
            
        // Take care of all other tracks
        s = next[speedZone(t)][s];
        
        // Move to the next track if we've wrapped over
        if (s == 0) {
            
            if (t >= numTracks()) return {0,0};
            t = t == 17 ? 19 : t + 1;
        }
    }
    
    assert(isTrackSectorPair(t, s));
    return {t,s};
}

bool
FSDeviceDescriptor::nextTrackAndSector(Track t, Sector s, Track *nt, Sector *ns)
{
    assert(isTrackSectorPair(t, s));
    assert(nt);
    assert(ns);
    
    // Lookup table for the next sector (interleave patterns)
    Sector next[5][21] = {

        // Speed zone 0 - 3
        { 10,11,12,13,14,15,16, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 },
        { 10,11,12,13,14,15,16,17, 1, 0, 2, 3, 4, 5, 6, 7, 8, 9 },
        { 10,11,12,13,14,15,16,17,18, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9},
        { 10,11,12,13,14,15,16,17,18,19,20, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 },
        
        // Directory track
        {  3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18, 0, 1, 2 }
    };

    if (t == 18) {

        // Take care of the directory track
        s = next[4][s];

        // Return false if we've wrapped over (directory track is full)
        if (s == 0) return false;
    
    } else {
            
        // Take care of all other tracks
        s = next[speedZone(t)][s];
        
        // Move to the next track if we've wrapped over
        if (s == 0) {
            
            if (t >= numTracks()) return false;
            t = t == 17 ? 19 : t + 1;
        }
    }
    
    assert(isTrackSectorPair(t, s));
    *nt = t;
    *ns = s;
    return true;
}