// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "FSDescriptors.h"
#include "C64.h"

FSDeviceDescriptor::FSDeviceDescriptor(DiskType type, DOSType dos)
{
    assert(dos == DOS_TYPE_CBM);
 
    this->dos = dos;
    
    switch (type) {
            
        case DISK_TYPE_SS_SD:
            numCyls = 35;
            numHeads = 1;
            break;
            
        case DISK_TYPE_DS_SD:
            numCyls = 35;
            numHeads = 2;
            break;
            
        default:
            assert(false);
    }
}

FSDeviceDescriptor::FSDeviceDescriptor(const D64File &d64)
{    
    this->dos = DOS_TYPE_CBM;
    numCyls = d64.numTracks();
    numHeads = 1;
}

bool
FSDeviceDescriptor::isValidLink(TSLink ref) const
{
    return isTrackNr(ref.t) && ref.s >= 0 && ref.s < numSectors(ref.t);
}

isize
FSDeviceDescriptor::speedZone(Cylinder t) const
{
    assert(isTrackNr(t));
    
    return (t <= 17) ? 3 : (t <= 24) ? 2 : (t <= 30) ? 1 : 0;
}

isize
FSDeviceDescriptor::numSectors(Track t) const
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

isize
FSDeviceDescriptor::numBlocks() const
{
    isize result = 0;
    
    for (Track t = 1; t <= numTracks(); t++) {
        result += numSectors(t);
    }
    
    return result;
}

TSLink
FSDeviceDescriptor::tsLink(Block b) const
{
    for (Track i = 1; i <= numTracks(); i++) {

        isize num = numSectors(i);
        if (b < num) return TSLink{i,b};
        b -= num;
    }
    
    return TSLink{0,0};
}

Block
FSDeviceDescriptor::blockNr(TSLink ts) const
{
    if (!isValidLink(ts)) return (Block)(-1);
    
    Block result = ts.s;
    for (Track i = 1; i < ts.t; i++) {
        result += numSectors(i);
    }
    
    return result;
}

TSLink
FSDeviceDescriptor::nextBlockRef(TSLink ref) const
{
    assert(isValidLink(ref));
    
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
    
    assert(isValidLink(TSLink{t,s}));
    return TSLink{t,s};
}
