// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "C64Object.h"

class RegressionTester : public C64Object {

public:
        
    // Filename of the test image
    string dumpTexturePath = "texture";
    
    // Texture cutout
    isize x1 = 104;
    isize y1 = 16;
    isize x2 = 488;
    isize y2 = 290;

private:
    
    // When the emulator exits, this value is returned to the test script
    u8 retValue = 0;

    
    //
    // Methods from C64Object
    //
    
private:
    
    const char *getDescription() const override { return "RegressionTester"; }
    
    
    //
    // Taking screenshots
    //

public:
    
    // Creates the test image and exits the emulator
    void dumpTexture(const class C64 &c64) const;
    void dumpTexture(const class C64 &c64, const string &filename) const;
    void dumpTexture(const class C64 &c64, std::ostream& os) const;

    
    //
    // Handling errors
    //

public:
    
    // Emulates the debugcart feature (used by VICE tests)
    void debugcart(u8 value);
};
