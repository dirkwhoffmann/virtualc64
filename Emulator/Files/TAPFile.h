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

class TAPFile : public AnyFile {
 
public:

    //
    // Class methods
    //

    static bool isCompatibleName(const std::string &name);
    static bool isCompatibleStream(std::istream &stream);
    
    
    //
    // Methods from C64Object
    //
    
    const char *getDescription() const override { return "TAPFile"; }
    
    
    //
    // Methods from AnyFile
    //
    
    FileType type() const override { return FILETYPE_TAP; }
    PETName<16> getName() const override;
    
    
    //
    // Retrieving tape information
    //
    
    // Returns the TAP version (0 = original layout, 1 = updated layout)
    TAPVersion version() const { return (TAPVersion)data[0x000C]; }
    

    //
    // Repairing
    //
    
    void repair() override;
};
