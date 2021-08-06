// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "AnyFile.h"
#include "C64Types.h"

struct RomSignature { RomType type; isize size; isize offset; u8 magic[3]; };

class RomFile : public AnyFile {
    
private:

    // Accepted header signatures
    static const RomSignature signatures[];
    
    // Rom type (Basic, Character, Kernal, or VC1541)
    FileType romFileType = FILETYPE_UNKNOWN;
        
public:
    
    //
    // Class methods
    //
    
    static bool isCompatiblePath(const std::string &name);
    static bool isCompatibleStream(std::istream &stream);
    
    static bool isRomStream(RomType type, std::istream &stream);
    static bool isBasicRomStream(std::istream &stream);
    static bool isCharRomStream(std::istream &stream);
    static bool isKernalRomStream(std::istream &stream);
    static bool isVC1541RomStream(std::istream &stream);
    
    static bool isRomFile(RomType type, const char *path);
    static bool isBasicRomFile(const char *path);
    static bool isCharRomFile(const char *path);
    static bool isKernalRomFile(const char *path);
    static bool isVC1541RomFile(const char *path);

    static bool isRomBuffer(RomType type, const u8 *buf, isize len);
    static bool isBasicRomBuffer(const u8 *buf, isize len);
    static bool isCharRomBuffer(const u8 *buf, isize len);
    static bool isKernalRomBuffer(const u8 *buf, isize len);
    static bool isVC1541RomBuffer(const u8 *buf, isize len);

    // Translates a FNV1A checksum (64 bit) into a ROM identifier
    static RomIdentifier identifier(u64 fnv);

    // Classifies a ROM identifier by type
    static bool isCommodoreRom(RomIdentifier rev);
    static bool isPatchedRom(RomIdentifier rev);

    // Provides information about known ROMs
    static const char *title(RomIdentifier rev);
    static const char *subTitle(RomIdentifier rev);
    static const char *revision(RomIdentifier rev);

  
    //
    // Methods from C64Object
    //
    
    const char *getDescription() const override { return "RomFile"; }
    
    
    //
    // Methods from AnyFile
    //
    
    FileType type() const override { return romFileType; }
    isize readFromStream(std::istream &stream) override;
    void repair() override;
};
