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

namespace vc64 {

class VirtualC64;

class Script : public AnyFile {
    
public:
    
    //
    // Class methods
    //
    
    static bool isCompatible(const fs::path &path);
    static bool isCompatible(std::istream &stream);
    
    
    //
    // Initializing
    //
    
public:
    
    // Script();
    Script(const fs::path &path) throws { init(path); }
    Script(const u8 *buf, isize len) throws { init(buf, len); }
    
    
    //
    // Methods from CoreObject
    //

    const char *objectName() const override { return "Script"; }
    
    
    //
    // Methods from AnyFile
    //
    
public:
    
    bool isCompatiblePath(const fs::path &path) override { return isCompatible(path); }
    bool isCompatibleStream(std::istream &stream) override { return isCompatible(stream); }
    FileType type() const override { return FILETYPE_SCRIPT; }
    
    
    //
    // Processing
    //
    
    // Executes the script
    // void execute(VirtualC64 &c64);
};

}
