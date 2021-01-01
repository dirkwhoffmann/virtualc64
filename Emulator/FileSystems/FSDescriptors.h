// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _FS_DESCRIPTORS_H
#define _FS_DESCRIPTORS_H

#include "C64Object.h"
#include "FSTypes.h"

/* To create a FSDevice, the layout parameters of the represendet device have
 * to be provided. This is done by passing a structure of type FSDeviceLayout
 * which contains physical properties such as the number of cylinders and heads
 * and logical parameters such as the number of sectors per track.
 *
 * FSDeviceDescriptors can be obtained in several ways. If a descriptor for
 * diskette is needed, it can be created by specifiying the disk type. If a
 * D64 file is given, a suitable device contructor can be created from this
 * file.
 */

struct FSDeviceDescriptor : C64Object {
    
    // DOS type
    DOSType dos = DOSType_NODOS;
    
    // Number of cylinders
    u32 numCyls = 0;
    
    // Number of heads
    u32 numHeads = 0;
        
    
    //
    // Initializing
    //
    
    FSDeviceDescriptor() { }

    // Creates a device descriptor for a standard disk
    FSDeviceDescriptor(DiskType type, DOSType dos = DOSType_CBM);

    // Creates a device descriptor from a D64 file
    FSDeviceDescriptor(class D64File *d64);

    const char *getDescription() override { return "FSLayout"; }
    
    
    //
    // Performing integrity checks
    //
    
    bool isCylinderNr(Cylinder c) { return 1 <= c && c <= numCyls; }
    bool isHeadNr(Head h) { return h == 0 || h == 1; }
    bool isTrackNr(Track t) { return 1 <= t && t <= numCyls * numHeads; }
    bool isValidLink(TSLink ref);

    
    //
    // Querying device properties
    //
    
    u32 numTracks() { return numCyls * numHeads; }
    u32 speedZone(Track track);
    u32 numSectors(Track track);
    u32 numBlocks();

    
    //
    // Translating blocks, tracks, sectors, and heads
    //

    Cylinder cylNr(Track t) { return t <= numCyls ? t : t - numCyls; }
    Head headNr(Track t) { return t <= numCyls ? 0 : 1; }
    Track trackNr(Cylinder c, Head h) { return c + h * numCyls; }

    TSLink tsLink(Block b);
    Track trackNr(Block b) { return tsLink(b).t; }
    Sector sectorNr(Block b) { return tsLink(b).s; }
    
    Block blockNr(TSLink ts);
    Block blockNr(Track t, Sector s) { return blockNr(TSLink{t,s}); }
    Block blockNr(Cylinder c, Head h, Sector s) { return blockNr(trackNr(c,h), s); }

    
    //
    // Ordering blocks
    //
    
public:
    
    bool nextBlock(Block b, Block *nb);
    TSLink nextBlockRef(TSLink b);
};

#endif
