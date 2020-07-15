// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _ROMFILE_H
#define _ROMFILE_H

#include "AnyC64File.h"

class C64RomFile : public AnyC64File {
    
private:

    // Accepted header signatures
    static const size_t basicRomSignatureCnt  = 3;
    static const size_t charRomSignatureCnt   = 10;
    static const size_t kernalRomSignatureCnt = 3;
    static const size_t vc1541RomSignatureCnt = 4;

    static const u8 magicBasicRomBytes[basicRomSignatureCnt][3];
    static const u8 magicCharRomBytes[charRomSignatureCnt][4];
    static const u8 magicKernalRomBytes[kernalRomSignatureCnt][3];
    static const u8 magicVC1541RomBytes[vc1541RomSignatureCnt][3];

    // Rom type (Basic, Character, Kernal, or VC1541)
    C64FileType romtype;
        
public:
    
    //
    // Class methods
    //
    
    // Returns true if buffer contains a ROM image
    static bool isRomBuffer(const u8 *buffer, size_t length);
    static bool isBasicRomBuffer(const u8 *buffer, size_t length);
    static bool isCharRomBuffer(const u8 *buffer, size_t length);
    static bool isKernalRomBuffer(const u8 *buffer, size_t length);
    static bool isVC1541RomBuffer(const u8 *buffer, size_t length);

    // Returns true if path points to a ROM image
    static bool isRomFile(const char *path);
    static bool isBasicRomFile(const char *path);
    static bool isCharRomFile(const char *path);
    static bool isKernalRomFile(const char *path);
    static bool isVC1541RomFile(const char *path);
    
    // Translates a FNV1A checksum (64 bit) into a ROM identifier
    static RomRevision revision(u64 fnv);

    // Provides information about known ROMs
    static const char *title(RomRevision rev);
    static const char *version(RomRevision rev);
    static const char *released(RomRevision rev);

    
    //
    // Creating and destructing
    //
    
    C64RomFile();
    
    // Factory methods
    static C64RomFile *makeWithBuffer(const u8 *buffer, size_t length);
    static C64RomFile *makeWithFile(const char *filename);
    
    
    //
    // Methods from AnyC64File
    //
    
    C64FileType type() { return romtype; }
    const char *typeAsString() { return "ROM"; }
    bool hasSameType(const char *filename) { return isRomFile(filename); }
    bool readFromBuffer(const u8 *buffer, size_t length);
    
};
#endif
