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
    "vicii set GLUE_LOGIC DISCRETE",
    "vicii set GLUE_LOGIC IC",
    "vicii set SS_COLLISIONS true",
    "vicii set SS_COLLISIONS false",
    "vicii set SB_COLLISIONS true",
    "vicii set SB_COLLISIONS false",
    "vicii set PALETTE COLOR",
    "vicii set PALETTE BLACK_WHITE",
    "vicii set PALETTE PAPER_WHITE",
    "vicii set PALETTE GREEN",
    "vicii set PALETTE AMBER",
    "vicii set PALETTE SEPIA",
    "vicii set BRIGHTNESS 50",
    "vicii set CONTRAST 50",
    "vicii set SATURATION 50",

    "dmadebugger open",
    "dmadebugger close",
    "dmadebugger set DMA_DEBUG_CHANNEL0 true",
    "dmadebugger set DMA_DEBUG_CHANNEL0 false",
    "dmadebugger set DMA_DEBUG_CHANNEL1 true",
    "dmadebugger set DMA_DEBUG_CHANNEL1 false",
    "dmadebugger set DMA_DEBUG_CHANNEL2 true",
    "dmadebugger set DMA_DEBUG_CHANNEL2 false",
    "dmadebugger set DMA_DEBUG_CHANNEL3 true",
    "dmadebugger set DMA_DEBUG_CHANNEL3 false",
    "dmadebugger set DMA_DEBUG_CHANNEL4 true",
    "dmadebugger set DMA_DEBUG_CHANNEL4 false",
    "dmadebugger set DMA_DEBUG_CHANNEL5 true",
    "dmadebugger set DMA_DEBUG_CHANNEL5 false",

    "sid 1",
    "sid 1 set SID_ENGINE FASTSID",
    "sid 1 set SID_ENGINE RESID",
    "sid 1 set SID_REVISION MOS_6581",
    "sid 1 set SID_REVISION MOS_8580",
    "sid 1 set SID_SAMPLING FAST",
    "sid 1 set SID_SAMPLING INTERPOLATE",
    "sid 1 set SID_SAMPLING RESAMPLE",
    "sid 1 set SID_SAMPLING RESAMPLE_FASTMEM",
    "sid 1 set SID_FILTER true",
    "sid 1 set SID_FILTER false",
    /*
    "sid 1 set volume channel0 50",
    "sid 1 set volume channel1 50",
    "sid 1 set volume channel2 50",
    "sid 1 set volume channel3 50",
    "sid 1 set volume left 50",
    "sid 1 set volume right 50",
    "sid 1 set pan channel0 50",
    "sid 1 set pan channel1 50",
    "sid 1 set pan channel2 50",
    "sid 1 set pan channel3 50",
    */

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
    "joystick 1 set AUTOFIRE true",
    "joystick 1 set AUTOFIRE false",
    "joystick 1 set AUTOFIRE_BURSTS true",
    "joystick 1 set AUTOFIRE_BURSTS false",
    "joystick 1 set AUTOFIRE_BULLETS 10",
    "joystick 1 set AUTOFIRE_DELAY 10",
    "joystick 1 press",
    "joystick 1 unpress",
    "joystick 1 pull left",
    "joystick 1 pull right",
    "joystick 1 pull up",
    "joystick 1 pull down",
    "joystick 1 release x",
    "joystick 1 release y",

    "joystick 2",
    "joystick 2 set AUTOFIRE true",
    "joystick 2 set AUTOFIRE false",
    "joystick 2 set AUTOFIRE_BURSTS true",
    "joystick 2 set AUTOFIRE_BURSTS false",
    "joystick 2 set AUTOFIRE_BULLETS 10",
    "joystick 2 set AUTOFIRE_DELAY 10",
    "joystick 2 press",
    "joystick 2 unpress",
    "joystick 2 pull left",
    "joystick 2 pull right",
    "joystick 2 pull up",
    "joystick 2 pull down",
    "joystick 2 release x",
    "joystick 2 release y",

    "mouse 1",
    "mouse 1 set MOUSE_MODEL C1350",
    "mouse 1 set MOUSE_MODEL C1351",
    "mouse 1 set MOUSE_MODEL NEOS",
    "mouse 1 set MOUSE_VELOCITY 50",
    "mouse 1 set SHAKE_DETECTION true",
    "mouse 1 set SHAKE_DETECTION false",

    "mouse 2",
    "mouse 2 set MOUSE_MODEL C1350",
    "mouse 2 set MOUSE_MODEL C1351",
    "mouse 2 set MOUSE_MODEL NEOS",
    "mouse 2 set MOUSE_VELOCITY 50",
    "mouse 2 set SHAKE_DETECTION true",
    "mouse 2 set SHAKE_DETECTION false",

    "parcable"
};

}
