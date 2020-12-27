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

/*
#include "FSObjects.h"
#include "FSBlock.h"
#include "FSEmptyBlock.h"
#include "FSBootBlock.h"
#include "FSRootBlock.h"
#include "FSBitmapBlock.h"
#include "FSBitmapExtBlock.h"
#include "FSUserDirBlock.h"
#include "FSFileHeaderBlock.h"
#include "FSFileListBlock.h"
#include "FSDataBlock.h"
*/

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
    FSVolumeType dos;
    
    // Number of cylinders
    u32 numCyls = 0;
    
    // Number of heads
    u32 numHeads = 0;
        
    
    //
    // Initializing
    //
    
    FSDeviceDescriptor() { }

    // Creates a device descriptor for a standard disk
    FSDeviceDescriptor(DiskType type, FSVolumeType dos = FS_CBM_DOS);

    // Creates a device descriptor compatible with the provided D64 file
    // FSDeviceDescriptor(class D64File *d64);

    // Creates a device descriptor compatible with the provided disk
    // FSDeviceDescriptor(class Disk *disk);

    
    const char *getDescription() override { return "FSLayout"; }
    
    
    //
    // Performing integrity checks
    //
    
    bool isCylinderNr(Cylinder c) { return 1 <= c && c <= numCyls; }
    bool isHeadNr(Head h) { return h == 0 || h == 1; }
    bool isTrackNr(Track t) { return 1 <= t && t <= numCyls * numHeads; }
    bool isTrackSectorPair(Track t, Sector s);
    bool isValidRef(BlockRef ref);

    
    //
    // Querying device properties
    //
    
    u32 numTracks() { return numCyls * numHeads; }
    u32 speedZone(Track track);
    u32 numSectors(Track track);
    u32 numBlocks();

    
    //
    // Translating block numbers
    //
        
    Cylinder cylNr(Track t);

    Head headNr(Track t);

    Track trackNr(Cylinder c, Head h);
    Track trackNr(Block b);

    Sector sectorNr(Block b);

    Block blockNr(Cylinder c, Head h, Sector s);
    Block blockNr(Track t, Sector s);
    Block blockNr(BlockRef ts);
    
    void translateBlockNr(Block b, Track *t, Sector *s);
    void translateBlockNr(Block b, Cylinder *c, Head *h, Sector *s);
    
    void translateBlockNr(Block *b, Track t, Sector s);
    void translateBlockNr(Block *b, Cylinder c, Head h, Sector s);
    
    
    //
    // Ordering blocks
    //
    
public:
    
    bool nextBlock(Block b, Block *nb);
    BlockRef nextBlockRef(BlockRef b);
    bool nextTrackAndSector(Track t, Sector s, Track *nt, Sector *ns);
};

#endif
