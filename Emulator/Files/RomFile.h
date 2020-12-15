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

#include "AnyFile.h"

class RomFile : public AnyFile {
    
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
    FileType romtype;
        
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
    static RomIdentifier identifier(u64 fnv);

    // Classifies a ROM identifier by type
    static bool isCommodoreRom(RomIdentifier rev);
    static bool isMega65Rom(RomIdentifier rev);
    static bool isPatchedRom(RomIdentifier rev);

    // Provides information about known ROMs
    static const char *title(RomIdentifier rev);
    static const char *subTitle(RomIdentifier rev);
    static const char *revision(RomIdentifier rev);

    
    //
    // Creating and destructing
    //
    
    RomFile();
    const char *getDescriptionNew() override { return "RomFile"; }

    // Factory methods
    static RomFile *makeWithBuffer(const u8 *buffer, size_t length);
    static RomFile *makeWithFile(const char *filename);
    
    
    //
    // Methods from AnyC64File
    //
    
    FileType type() override { return romtype; }
    bool hasSameType(const char *filename) override { return isRomFile(filename); }
    bool readFromBuffer(const u8 *buffer, size_t length) override;
    
};
#endif
