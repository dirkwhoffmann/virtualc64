// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "DiskAnalyzerTypes.h"
#include "C64Object.h"

class DiskAnalyzer: public C64Object {
  
    class Disk *disk;
    
    //
    // Initializing
    //
    
public:

    DiskAnalyzer(class Disk *disk);
    ~DiskAnalyzer();
    
    
    //
    // Methods from C64Object
    //

private:
    
    const char *getDescription() const override { return "DiskAnalyzer"; }

};
