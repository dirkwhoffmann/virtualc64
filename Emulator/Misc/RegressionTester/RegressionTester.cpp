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

#include "config.h"
#include "RegressionTester.h"
#include "Emulator.h"
#include "IOUtils.h"

#include <fstream>

namespace vc64 {

void
RegressionTester::prepare(C64 &c64, C64Model model)
{
    // Only proceed if the /tmp folder exisits
    if (!util::fileExists(fs::path("/tmp"))) throw Error(VC64ERROR_DIR_NOT_FOUND, "/tmp");

    // Check if we've got write permissions
    if (host.tmp() != "/tmp") throw Error(VC64ERROR_DIR_ACCESS_DENIED, "/tmp");

    // Initialize the emulator according to the specified model
    emulator.set(model);

    // Choose a warp source that prevents the GUI from disabling warp mode
    constexpr isize warpSource = 1;

    // Run as fast as possible
    emulator.warpOn(warpSource);

    // Run the emulator
    emulator.run();
}

void
RegressionTester::run(string path)
{
    c64.flash(PRGFile(path), 0);
    keyboard.autoType("run\n");
}

void
RegressionTester::dumpTexture(C64 &c64)
{
    dumpTexture(c64, dumpTexturePath);
}

void
RegressionTester::dumpTexture(C64 &c64, const string &filename)
{
    /* This function is used for automatic regression testing. It dumps the
     * visible portion of the texture into the /tmp directory and exits the
     * application. The regression test script picks up the texture and
     * compares it against a previously recorded reference image.
     */
    std::ofstream file;

    // Open an output stream
    file.open(("/tmp/" + filename + ".raw").c_str());

    // Dump texture
    dumpTexture(c64, file);
    file.close();

    // Ask the GUI to quit
    msgQueue.put(MSG_ABORT, retValue);
}

void
RegressionTester::dumpTexture(C64 &c64, std::ostream& os)
{
    char grey2[3] = { 0x22, 0x22, 0x22 };
    char grey4[3] = { 0x44, 0x44, 0x44 };

    auto checkerboard = [&](isize y, isize x) {
        return ((y >> 3) & 1) == ((x >> 3) & 1) ? grey2 : grey4;
    };

    auto buffer = (u32 *)c64.videoPort.getTexture();
    char *cptr;

    for (isize y = Y1; y < Y2; y++) {

        for (isize x = X1; x < X2; x++) {

            if (y >= y1 && y < y2 && x >= x1 && x < x2) {
                cptr = (char *)(buffer + y * Texture::width + x);
            } else {
                cptr = checkerboard(y, x);
            }

            os.write(cptr + 0, 1);
            os.write(cptr + 1, 1);
            os.write(cptr + 2, 1);
        }
    }
}

void
RegressionTester::debugcart(u8 value)
{
    retValue = value;
}

}
