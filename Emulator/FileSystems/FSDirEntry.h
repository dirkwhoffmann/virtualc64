// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _FS_DIR_ENTRY_H
#define _FS_DIR_ENTRY_H

#include "FSObjects.h"

typedef struct
{
    u8 nextDirTrack;      // $00
    u8 nextDirSector;     // $01
    u8 fileType;          // $02
    u8 firstDataTrack;    // $03
    u8 firstDataSector;   // $04
    u8 fileName[16];      // $05 - $14
    u8 sideSecBlkTrack;   // $15          REL files only
    u8 sideSrcBlkSector;  // $16          REL files only
    u8 recLength;         // $17          REL files only
    u8 unused[6];         // $18 - $1D    Geos only
    u8 fileSizeLo;        // $1E
    u8 fileSizeHi;        // $1F

    // Initializes this entry
    void init(const char *name, Track t, Sector s, size_t filesize);

    // Checks if this directory entry if empty
    bool isEmpty(); 
    
    // Returns the file type as a string
    const char *typeString(); 
    
    // Returns the name of this file
    FSName getName() { return FSName(fileName); }
    
    // Returns true if this file does not appear in a regular directory listing
    bool isHidden();
}
FSDirEntry;

#endif
