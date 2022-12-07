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

namespace vc64 {

class Script : public AnyFile {
    
public:
    
    //
    // Class methods
    //
    
    static bool isCompatible(const string &path);
    static bool isCompatible(std::istream &stream);
    
    
    //
    // Initializing
    //
    
public:
    
    // Script();
    Script(const string &path) throws { init(path); }
    Script(const u8 *buf, isize len) throws { init(buf, len); }
    
    
    //
    // Methods from C64Object
    //

    const char *getDescription() const override { return "Script"; }
    
    
    //
    // Methods from AnyFile
    //
    
public:
    
    bool isCompatiblePath(const string &path) override { return isCompatible(path); }
    bool isCompatibleStream(std::istream &stream) override { return isCompatible(stream); }
    FileType type() const override { return FILETYPE_SCRIPT; }
    
    
    //
    // Processing
    //
    
    // Executes the script
    void execute(class C64 &c64);
};

}
