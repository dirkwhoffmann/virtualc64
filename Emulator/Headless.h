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

#include "VirtualC64.h"
#include <map>

namespace vc64 {

struct SyntaxError : public std::runtime_error {
    using runtime_error::runtime_error;
};

// The message listener
void process(const void *listener, Message msg);

class Headless {

    // Parsed command line arguments
    std::map<string,string> keys;

    // The emulator instance
    VirtualC64 c64;

    // Barrier for syncing script execution
    util::Mutex barrier;

    // Return code
    int returnCode;

    
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
    "# This script checks the integrity of application. It runs several shell",
    "# commands to check if anything breaks.",
    "# ",
    "# Dirk W. Hoffmann, 2024",

    "",
    "help",
    "hel",          // Test auto-completion
    "joshua",       // Test a hidden command
    "try joshu",    // Catch an error with 'try'

    "",
    "c64",
    "c64 defaults",
    "c64 set WARP_BOOT 50",
    "c64 set WARP_MODE WARP_ALWAYS",
    "c64 set WARP_MODE WARP_AUTO",
    "c64 set WARP_MODE WARP_NEVER",
    "c64 set VSYNC yes",
    "c64 set TIME_LAPSE 75",
    "c64 set RUN_AHEAD 2",
    "c64 init PAL",
    "c64 init PAL_II",
    "c64 init PAL_OLD",
    "c64 init NTSC",
    "c64 init NTSC_II",
    "c64 init NTSC_OLD",
    "c64 reset",

    "",
    "memory",
    "memory set RAM_PATTERN VICE",
    "memory set RAM_PATTERN CCS",
    "memory set RAM_PATTERN ZEROES",
    "memory set RAM_PATTERN ONES",
    "memory set RAM_PATTERN RANDOM",
    "memory set SAVE_ROMS true",
    "memory set SAVE_ROMS false",

    "",
    "cia1",
    "cia1 set CIA_REVISION MOS_6526",
    "cia1 set CIA_REVISION MOS_8521",
    "cia1 set TIMER_B_BUG true",
    "cia1 set TIMER_B_BUG false",

    "",
    "cia2",
    "cia2 set CIA_REVISION MOS_6526",
    "cia2 set CIA_REVISION MOS_8521",
    "cia2 set TIMER_B_BUG true",
    "cia2 set TIMER_B_BUG false",

    "",
    "vicii",
    "vicii set VICII_REVISION PAL_6569_R1",
    "vicii set VICII_REVISION PAL_6569_R3",
    "vicii set VICII_REVISION PAL_8565",
    "vicii set VICII_REVISION NTSC_6567",
    "vicii set VICII_REVISION NTSC_6567_R56A",
    "vicii set VICII_REVISION NTSC_8562",
    "vicii set GRAY_DOT_BUG true",
    "vicii set GRAY_DOT_BUG false",
    "vicii set GLUE_LOGIC true",
    "vicii set GLUE_LOGIC false",
    "vicii set OPT_VICII_SS_COLLISIONS true",
    "vicii set OPT_VICII_SS_COLLISIONS false",
    "vicii set OPT_VICII_SB_COLLISIONS true",
    "vicii set OPT_VICII_SB_COLLISIONS false",

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
