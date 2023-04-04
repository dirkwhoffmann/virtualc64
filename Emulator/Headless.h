// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "C64.h"
#include <map>

using std::map;
using std::vector;

namespace vc64 {

struct SyntaxError : public std::runtime_error {
    using runtime_error::runtime_error;
};

void process(const void *listener, Message msg);

class Headless {

    // Parsed command line arguments
    map<string,string> keys;

    // The emulator instance
    C64 c64;

    // Barrier for syncing script execution
    util::Mutex barrier;

    // Return code
    std::optional<int> returnCode;

    
    //
    // Launching
    //
    
public:

    // Main entry point
    int main(int argc, char *argv[]);

private:

    // Parses the command line arguments
    void parseArguments(int argc, char *argv[]);

    // Checks all command line arguments for conistency
    void checkArguments() throws;

    // Returns the path to the self-test script
    string selfTestScript();


    //
    // Running
    //

public:
    
    // Processes an incoming message
    void process(Message msg);
};

//
// Self-test script
//

static const char *testScript[] = {

    "# Self-test script for VirtualC64",
    "# ",
    "# This script is executed in nightly-builts to check the integrity of ",
    "# the compiled application. It runs several shell commands to check if ",
    "# anything breaks.",
    "# ",
    "# Dirk W. Hoffmann, 2023",

    "",
    "help",
    "hel",          // Test auto-completion
    "joshua",       // Test a hidden command
    "try joshu",    // Catch an error with 'try'

    /*
     "",
     "regression setup PAL",
     "regression setup PAL_II",
     "regression setup PAL_OLD",
     "regression setup NTSC",
     "regression setup NTSC_II",
     "regression setup NTSC_OLD",
     */

    "",
    "c64",
    "c64 defaults",
    "c64 set fps 50",
    "c64 set fps 60",
    "c64 init PAL",
    "c64 init PAL_II",
    "c64 init PAL_OLD",
    "c64 init NTSC",
    "c64 init NTSC_II",
    "c64 init NTSC_OLD",
    "c64 reset",

    "",
    "memory",
    "memory set raminit VICE",
    "memory set raminit CCS",
    "memory set raminit ZEROES",
    "memory set raminit ONES",
    "memory set raminit RANDOM",
    "memory set saveroms true",
    "memory set saveroms false",

    "",
    "cia1",
    "cia1 set revision MOS_6526",
    "cia1 set revision MOS_8521",
    "cia1 set timerbbug true",
    "cia1 set timerbbug false",

    "",
    "cia2",
    "cia2 set revision MOS_6526",
    "cia2 set revision MOS_8521",
    "cia2 set timerbbug true",
    "cia2 set timerbbug false",

    "",
    "vicii",
    "vicii set revision PAL_6569_R1",
    "vicii set revision PAL_6569_R3",
    "vicii set revision PAL_8565",
    "vicii set revision NTSC_6567",
    "vicii set revision NTSC_6567_R56A",
    "vicii set revision NTSC_8562",
    "vicii set graydotbug true",
    "vicii set graydotbug false",
    "vicii set gluelogic true",
    "vicii set gluelogic false",
    "vicii set sscollisions true",
    "vicii set sscollisions false",
    "vicii set sbcollisions true",
    "vicii set sbcollisions false",

    "dmadebugger open",
    "dmadebugger close",
    "dmadebugger raccesses true",
    "dmadebugger raccesses false",
    "dmadebugger iaccesses true",
    "dmadebugger iaccesses false",
    "dmadebugger caccesses true",
    "dmadebugger caccesses false",
    "dmadebugger gaccesses true",
    "dmadebugger gaccesses false",
    "dmadebugger paccesses true",
    "dmadebugger paccesses false",
    "dmadebugger saccesses true",
    "dmadebugger saccesses false",

    "monitor",
    "monitor set palette COLOR",
    "monitor set palette BLACK_WHITE",
    "monitor set palette PAPER_WHITE",
    "monitor set palette GREEN",
    "monitor set palette AMBER",
    "monitor set palette SEPIA",
    "monitor set brightness 50",
    "monitor set contrast 50",
    "monitor set saturation 50",

    "sid",
    "sid set engine FASTSID",
    "sid set engine RESID",
    "sid set revision MOS_6581",
    "sid set revision MOS_8580",
    "sid set sampling FAST",
    "sid set sampling INTERPOLATE",
    "sid set sampling RESAMPLE",
    "sid set sampling RESAMPLE_FASTMEM",
    "sid set filter true",
    "sid set filter false",
    "sid set volume channel0 50",
    "sid set volume channel1 50",
    "sid set volume channel2 50",
    "sid set volume channel3 50",
    "sid set volume left 50",
    "sid set volume right 50",
    "sid set pan channel0 50",
    "sid set pan channel1 50",
    "sid set pan channel2 50",
    "sid set pan channel3 50",

    "expansion attach reu 128",
    "expansion attach reu 256",
    "expansion attach reu 512",
    "expansion attach reu 2048",
    "expansion attach georam 512",

    "keyboard press 32",
    "keyboard release 32",
    "keyboard type text \"text\"",
    "keyboard type load",
    "keyboard type run",

    "drive8",
    "drive8 disconnect",
    "drive8 eject",
    "drive8 newdisk NODOS",
    "drive8 newdisk CBM",

    "drive9",
    "drive9 disconnect",
    "drive9 eject",
    "drive9 newdisk NODOS",
    "drive9 newdisk CBM",

    "datasette",
    "datasette connect",
    "datasette disconnect",
    "datasette rewind",
    "datasette rewind to 10",

    "joystick 1",
    "joystick 1 set autofire true",
    "joystick 1 set autofire false",
    "joystick 1 set bullets 10",
    "joystick 1 set delay 10",
    "joystick 1 press",
    "joystick 1 unpress",
    "joystick 1 pull left",
    "joystick 1 pull right",
    "joystick 1 pull up",
    "joystick 1 pull down",
    "joystick 1 release x",
    "joystick 1 release y",

    "joystick 2",
    "joystick 2 set autofire true",
    "joystick 2 set autofire false",
    "joystick 2 set bullets 10",
    "joystick 2 set delay 10",
    "joystick 2 press",
    "joystick 2 unpress",
    "joystick 2 pull left",
    "joystick 2 pull right",
    "joystick 2 pull up",
    "joystick 2 pull down",
    "joystick 2 release x",
    "joystick 2 release y",

    "mouse 1",
    "mouse 1 set model C1350",
    "mouse 1 set model C1351",
    "mouse 1 set model NEOS",
    "mouse 1 set velocity 50",
    "mouse 1 set shakedetector true",
    "mouse 1 set shakedetector false",

    "mouse 2",
    "mouse 2 set model C1350",
    "mouse 2 set model C1351",
    "mouse 2 set model NEOS",
    "mouse 2 set velocity 50",
    "mouse 2 set shakedetector true",
    "mouse 2 set shakedetector false",

    "parcable"
};

}
