// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// This FILE is dual-licensed. You are free to choose between:
//
//     - The GNU General Public License v3 (or any later version)
//     - The Mozilla Public License v2
//
// SPDX-License-Identifier: GPL-3.0-or-later OR MPL-2.0
// -----------------------------------------------------------------------------

#pragma once

#include "CoreObject.h"
#include "DiskTypes.h"
#include "FSTypes.h"

namespace vc64 {

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

struct FSDeviceDescriptor : CoreObject {
    
    // DOS type
    DOSType dos = DOS_TYPE_NODOS;
    
    // Number of cylinders
    isize numCyls = 0;
    
    // Number of heads
    isize numHeads = 0;

    
    //
    // Initializing
    //
    
    FSDeviceDescriptor() { }

    // Creates a device descriptor for a standard disk
    FSDeviceDescriptor(DiskType type, DOSType dos = DOS_TYPE_CBM);

    // Creates a device descriptor from a D64 file
    FSDeviceDescriptor(const class D64File &d64);

    const char *objectName() const override { return "FSLayout"; }
    
    
    //
    // Performing integrity checks
    //
    
    bool isCylinderNr(Cylinder c) const { return 1 <= c && c <= numCyls; }
    bool isHeadNr(Head h) const { return h == 0 || h == 1; }
    bool isTrackNr(Track t) const { return 1 <= t && t <= numCyls * numHeads; }
    bool isValidLink(TSLink ref) const;

    
    //
    // Querying device properties
    //
    
    isize numTracks() const { return numCyls * numHeads; }
    isize speedZone(Cylinder track) const;
    isize numSectors(Cylinder track) const;
    isize numBlocks() const;

    
    //
    // Translating blocks, tracks, sectors, and heads
    //

    Cylinder cylNr(Track t) const { return t <= numCyls ? t : t - numCyls; }
    Head headNr(Track t) const { return t <= numCyls ? 0 : 1; }
    Track trackNr(Cylinder c, Head h) const { return c + h * numCyls; }

    TSLink tsLink(Block b) const;
    Track trackNr(Block b) const { return tsLink(b).t; }
    Sector sectorNr(Block b) const { return tsLink(b).s; }
    
    Block blockNr(TSLink ts) const;
    Block blockNr(Track t, Sector s) const { return blockNr(TSLink{t,s}); }
    Block blockNr(Cylinder c, Head h, Sector s) const { return blockNr(trackNr(c,h), s); }

    
    //
    // Ordering blocks
    //
    
public:
    
    bool nextBlock(Block b, Block *nb) const;
    TSLink nextBlockRef(TSLink b) const;
};

}
