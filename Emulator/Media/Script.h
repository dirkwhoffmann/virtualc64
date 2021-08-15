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

class Script : public AnyFile {
    
public:
    
    //
    // Class methods
    //
    
    static bool isCompatiblePath(const string &path);
    static bool isCompatibleStream(std::istream &stream);
    
    
    //
    // Initializing
    //
    
public:
    
    Script();
    
    //
    // Methods from C64Object
    //
        
    const char *getDescription() const override { return "Script"; }
    
    
    //
    // Methods from AnyFile
    //
    
public:
    
    FileType type() const override { return FILETYPE_SCRIPT; }
    
    
    //
    // Processing
    //
    
    // Executes the script
    void execute(class C64 &c64);
};
