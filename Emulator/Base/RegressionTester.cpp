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
#include "IO.h"

#include <fstream>

void
RegressionTester::prepare(C64 &c64, C64Model model)
{
    // Revert to factory settings
    c64.initialize(model);
    
    // Select the default texture cutout
    x1 = 104; y1 = 17; x2 = 488; y2 = c64.vic.isPAL() ? 291 : 241;
    
    // Run as fast as possible
    c64.setWarp(true);
    
    // Prevent the GUI from disabling warp mode
    c64.lockWarpMode();
    
    // Launch the emulator
    c64.run();
}

void
RegressionTester::dumpTexture(C64 &c64) const
{
    dumpTexture(c64, dumpTexturePath);
}

void
RegressionTester::dumpTexture(C64 &c64, const string &filename) const
{
    /* This function is used for automatic regression testing. It generates a
     * TIFF image of the current emulator texture in the /tmp directory and
     * exits the application. The regression testing script will pick up the
     * texture and compare it against a previously recorded reference image.
     */
    std::ofstream file;
        
    // Assemble the target file names
    string rawFile = "/tmp/" + filename + ".raw";
    string tiffFile = "/tmp/" + filename + ".tiff";

    // Open an output stream
    file.open(rawFile.c_str());
    
    // Dump texture
    dumpTexture(c64, file);
    file.close();
    
    // Convert raw data into a TIFF file
    string cmd = "/usr/local/bin/raw2tiff";
    cmd += " -p rgb -b 3";
    cmd += " -w " + std::to_string(x2 - x1);
    cmd += " -l " + std::to_string(y2 - y1);
    cmd += " " + rawFile + " " + tiffFile;
    
    if (system(cmd.c_str()) == -1) {
        warn("Error executing %s\n", cmd.c_str());
    }
    
    // Exit the emulator
    exit(retValue);
}

void
RegressionTester::dumpTexture(C64 &c64, std::ostream& os) const
{
    c64.suspend();
    
    auto buffer = (u32 *) c64.vic.stableEmuTexture();

    for (isize y = y1; y < y2; y++) {
        
        for (isize x = x1; x < x2; x++) {
            
            char *cptr = (char *)(buffer + y * TEX_WIDTH + x);
            os.write(cptr + 0, 1);
            os.write(cptr + 1, 1);
            os.write(cptr + 2, 1);
        }
    }
    
    c64.resume();
}

void
RegressionTester::debugcart(u8 value)
{
    retValue = value;
}
