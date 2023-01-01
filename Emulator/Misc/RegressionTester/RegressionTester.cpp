// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "RegressionTester.h"
#include "C64.h"
#include "IOUtils.h"

#include <fstream>

namespace vc64 {

void
RegressionTester::prepare(C64 &c64, C64Model model)
{
    // Only proceed if the /tmp folder exisits
    if (!util::fileExists("/tmp")) throw VC64Error(ERROR_DIR_NOT_FOUND, "/tmp");

    // Check if we've got write permissions
    if (c64.tmp() != "/tmp") throw VC64Error(ERROR_DIR_ACCESS_DENIED, "/tmp");

    // Initialize the emulator according to the specified model
    c64.revertToFactorySettings();
    c64.configure(model);
    
    // Choose a warp source that prevents the GUI from disabling warp mode
    constexpr isize warpSource = 1;

    // Run as fast as possible
    c64.warpOn(warpSource);

    // Run the emulator
    c64.run();
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

    {   SUSPENDED

        auto buffer = (u32 *)c64.vic.stableEmuTexture();
        char *cptr;

        for (isize y = Y1; y < Y2; y++) {
            
            for (isize x = X1; x < X2; x++) {

                if (y >= y1 && y < y2 && x >= x1 && x < x2) {
                    cptr = (char *)(buffer + y * TEX_WIDTH + x);
                } else {
                    cptr = checkerboard(y, x);
                }

                os.write(cptr + 0, 1);
                os.write(cptr + 1, 1);
                os.write(cptr + 2, 1);
            }
        }
    }
}

void
RegressionTester::debugcart(u8 value)
{
    retValue = value;
}

}
