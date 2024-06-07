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
#include "Concurrency.h"
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
    "memory set INIT_PATTERN VICE",
    "memory set INIT_PATTERN CCS",
    "memory set INIT_PATTERN ZEROES",
    "memory set INIT_PATTERN ONES",
    "memory set INIT_PATTERN RANDOM",
    "memory set SAVE_ROMS true",
    "memory set SAVE_ROMS false",

    "",
    "cia1",
    "cia1 set REVISION MOS_6526",
    "cia1 set REVISION MOS_8521",
    "cia1 set TIMER_B_BUG true",
    "cia1 set TIMER_B_BUG false",

    "",
    "cia2",
    "cia2 set REVISION MOS_6526",
    "cia2 set REVISION MOS_8521",
    "cia2 set TIMER_B_BUG true",
    "cia2 set TIMER_B_BUG false",

    "",
    "vic",
    "vic set REVISION PAL_6569_R1",
    "vic set REVISION PAL_6569_R3",
    "vic set REVISION PAL_8565",
    "vic set REVISION NTSC_6567",
    "vic set REVISION NTSC_6567_R56A",
    "vic set REVISION NTSC_8562",
    "vic set GRAY_DOT_BUG true",
    "vic set GRAY_DOT_BUG false",
    "vic set GLUE_LOGIC DISCRETE",
    "vic set GLUE_LOGIC IC",
    "vic set SS_COLLISIONS true",
    "vic set SS_COLLISIONS false",
    "vic set SB_COLLISIONS true",
    "vic set SB_COLLISIONS false",

    "monitor set PALETTE COLOR",
    "monitor set PALETTE BLACK_WHITE",
    "monitor set PALETTE PAPER_WHITE",
    "monitor set PALETTE GREEN",
    "monitor set PALETTE AMBER",
    "monitor set PALETTE SEPIA",
    "monitor set BRIGHTNESS 50",
    "monitor set CONTRAST 50",
    "monitor set SATURATION 50",

    "dmadebugger open",
    "dmadebugger close",
    "dmadebugger set CHANNEL0 true",
    "dmadebugger set CHANNEL0 false",
    "dmadebugger set CHANNEL1 true",
    "dmadebugger set CHANNEL1 false",
    "dmadebugger set CHANNEL2 true",
    "dmadebugger set CHANNEL2 false",
    "dmadebugger set CHANNEL3 true",
    "dmadebugger set CHANNEL3 false",
    "dmadebugger set CHANNEL4 true",
    "dmadebugger set CHANNEL4 false",
    "dmadebugger set CHANNEL5 true",
    "dmadebugger set CHANNEL5 false",

    "sid",
    "sid set ENGINE RESID",
    "sid set REVISION MOS_6581",
    "sid set REVISION MOS_8580",
    "sid set SAMPLING FAST",
    "sid set SAMPLING INTERPOLATE",
    "sid set SAMPLING RESAMPLE",
    "sid set SAMPLING RESAMPLE_FASTMEM",
    "sid set FILTER true",
    "sid set FILTER false",

    "sid2",
    "sid2 set ENGINE RESID",
    "sid2 set REVISION MOS_6581",
    "sid2 set REVISION MOS_8580",
    "sid2 set SAMPLING FAST",
    "sid2 set SAMPLING INTERPOLATE",
    "sid2 set SAMPLING RESAMPLE",
    "sid2 set SAMPLING RESAMPLE_FASTMEM",
    "sid2 set FILTER true",
    "sid2 set FILTER false",

    "audio set VOL0 50",
    "audio set VOL1 50",
    "audio set VOL2 50",
    "audio set VOL3 50",
    "audio set VOLL 50",
    "audio set VOLR 50",
    "audio set PAN0 50",
    "audio set PAN1 50",
    "audio set PAN2 50",
    "audio set PAN3 50",

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

    "joystick1",
    "joystick1 set autofire ENABLE true",
    "joystick1 set autofire ENABLE false",
    "joystick1 set autofire BURSTS true",
    "joystick1 set autofire BURSTS false",
    "joystick1 set autofire BULLETS 10",
    "joystick1 set autofire DELAY 10",
    "joystick1 press",
    "joystick1 unpress",
    "joystick1 pull left",
    "joystick1 pull right",
    "joystick1 pull up",
    "joystick1 pull down",
    "joystick1 release x",
    "joystick1 release y",

    "joystick2",
    "joystick2 set autofire ENABLE true",
    "joystick2 set autofire ENABLE false",
    "joystick2 set autofire BURSTS true",
    "joystick2 set autofire BURSTS false",
    "joystick2 set autofire BULLETS 10",
    "joystick2 set autofire DELAY 10",
    "joystick2 press",
    "joystick2 unpress",
    "joystick2 pull left",
    "joystick2 pull right",
    "joystick2 pull up",
    "joystick2 pull down",
    "joystick2 release x",
    "joystick2 release y",

    "mouse1",
    "mouse1 set MODEL C1350",
    "mouse1 set MODEL C1351",
    "mouse1 set MODEL NEOS",
    "mouse1 set VELOCITY 50",
    "mouse1 set SHAKE_DETECTION true",
    "mouse1 set SHAKE_DETECTION false",

    "mouse2",
    "mouse2 set MODEL C1350",
    "mouse2 set MODEL C1351",
    "mouse2 set MODEL NEOS",
    "mouse2 set VELOCITY 50",
    "mouse2 set SHAKE_DETECTION true",
    "mouse2 set SHAKE_DETECTION false",

    "parcable"
};

}
