// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "SubComponent.h"
#include "C64Types.h"

namespace vc64 {

class C64;

class RegressionTester : public SubComponent {

public:
    
    // Filename of the test image
    string dumpTexturePath = "texture";
    
    // Texture cutout
    isize x1 = 104;
    isize y1 = 17;
    isize x2 = 488;
    isize y2 = 291;

private:

    // When the emulator exits, this value is returned to the test script
    u8 retValue = 0;

    
    //
    // Constructing
    //
    
public:
    
    using SubComponent::SubComponent;
    const char *getDescription() const override { return "RegressionTester"; }
    
private:
    
    void _reset(bool hard) override { };
    
    
    //
    // Serializing
    //
    
    isize _size() override { return 0; }
    u64 _checksum() override { return 0; }
    isize _load(const u8 *buffer) override { return 0; }
    isize _save(u8 *buffer) override { return 0; }
    
    
    //
    // Running a regression test
    //

public:

    // Reverts everything to factory settings
    void prepare(C64 &c64, C64Model model);

    // Runs a test case
    void run(string path);

    // Creates the test image and exits the emulator
    void dumpTexture(C64 &c64);
    void dumpTexture(C64 &c64, const string &filename);
    void dumpTexture(C64 &c64, std::ostream& os);

    
    //
    // Handling errors
    //

public:
    
    // Emulates the debugcart feature (used by VICE tests)
    void debugcart(u8 value);
};

}
