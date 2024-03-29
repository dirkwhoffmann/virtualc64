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

#include "SubComponent.h"
#include "C64Types.h"

namespace vc64 {

class C64;

class RegressionTester : public SubComponent {

    // Pixel area ritten to the test image
    static constexpr isize X1 = 104;
    static constexpr isize Y1 = 17;
    static constexpr isize X2 = 488;
    static constexpr isize Y2 = 291;

public:
    
    // Filename of the test image
    string dumpTexturePath = "texture";
    
    // Pixel area that is written to the test image
    isize x1 = X1;
    isize y1 = Y1;
    isize x2 = X2;
    isize y2 = Y2;

private:

    // When the emulator exits, this value is returned to the test script
    u8 retValue = 0;

    
    //
    // Constructing
    //
    
public:
    
    using SubComponent::SubComponent;

    //
    // Methods from CoreObject
    //

private:

    const char *getDescription() const override { return "RegressionTester"; }
    void _dump(Category category, std::ostream& os) const override { }

    
    //
    // Methods from CoreComponent
    //

private:

    void _reset(bool hard) override { };
    isize _size() override { return 0; }
    u64 _checksum() override { return 0; }
    isize _load(const u8 *buffer) override { return 0; }
    isize _save(u8 *buffer) override { return 0; }
    
    
    //
    // Running a regression test
    //

public:

    // Reverts to factory settings
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
