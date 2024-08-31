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

#include "AnyFile.h"
#include "C64Types.h"
#include "MemoryTypes.h"

namespace vc64 {

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

    static bool isCompatible(const fs::path &name);
    static bool isCompatible(const u8 *buf, isize len);
    static bool isCompatible(const Buffer<u8> &buffer);

    static bool isRomBuffer(RomType type, const u8 *buf, isize len);
    static bool isBasicRomBuffer(const u8 *buf, isize len);
    static bool isCharRomBuffer(const u8 *buf, isize len);
    static bool isKernalRomBuffer(const u8 *buf, isize len);
    static bool isVC1541RomBuffer(const u8 *buf, isize len);

    static bool isRomBuffer(RomType type, const Buffer<u8> &buf);
    static bool isBasicRomBuffer(const Buffer<u8> &buf);
    static bool isCharRomBuffer(const Buffer<u8> &buf);
    static bool isKernalRomBuffer(const Buffer<u8> &buf);
    static bool isVC1541RomBuffer(const Buffer<u8> &buf);


    //
    // Initializing
    //
    
    RomFile(const fs::path &path) throws { init(path); }
    // RomFile(const fs::path &path, std::istream &stream) throws { init(path, stream); }
    RomFile(const u8 *buf, isize len) throws { init(buf, len); }

    
    //
    // Methods from CoreObject
    //
    
    const char *objectName() const override { return "RomFile"; }
    
    
    //
    // Methods from AnyFile
    //
    
    bool isCompatiblePath(const fs::path &path) override { return isCompatible(path); }
    bool isCompatibleBuffer(const u8 *buf, isize len) override { return isCompatible(buf, len); }
    FileType type() const override { return romFileType; }
    void finalizeRead() override;
};

}
