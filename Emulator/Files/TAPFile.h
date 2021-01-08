// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
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
    
    const char *getDescription() override { return "TAPFile"; }
    
    
    //
    // Methods from AnyFile
    //
    
    FileType type() const override { return FILETYPE_TAP; }
    PETName<16> getName() const override;
    
    
    //
    // Retrieving tape information
    //
    
    // Returns the TAP version (0 = original layout, 1 = updated layout)
    TAPVersion version() { return (TAPVersion)data[0x000C]; }
    
    // Returns the beginning of the data area
    u8 *getData() { return data + 0x14; }
    
    // Returns the size of the data area in bytes
    usize getDataSize() { return size - 0x14; }
    
    
    //
    // Repairing
    //
    
    void repair() override;
};
