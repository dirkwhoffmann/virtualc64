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
/// @file

#include "config.h"
#include "Headless.h"
#include "C64.h"
#include "Script.h"
#include <chrono>

int main(int argc, char *argv[])
{
    try {

        return vc64::Headless().main(argc, argv);

    } catch (vc64::SyntaxError &e) {

        std::cout << "Usage: VirtualC64Headless [-fsdvm] [<script>]" << std::endl;
        std::cout << std::endl;
        std::cout << "       -f or --footprint   Report the size of objects" << std::endl;
        std::cout << "       -s or --smoke       Run smoke tests to test the build" << std::endl;
        std::cout << "       -d or --diagnose    Launch the emulator thread" << std::endl;
        std::cout << "       -v or --verbose     Print the executed script lines" << std::endl;
        std::cout << "       -m or --messages    Observe the message queue" << std::endl;
        std::cout << "       <script>            Execute a custom script" << std::endl;
        std::cout << std::endl;

        if (auto what = std::string(e.what()); !what.empty()) {
            std::cout << what << std::endl;
        }

    } catch (vc64::AppError &e) {

        std::cout << "Emulator Error: " << e.what() << std::endl;

    } catch (std::exception &e) {

        std::cout << "System Error: " << e.what() << std::endl;

    } catch (...) {

        std::cout << "Error" << std::endl;
    }

    return 1;
}

namespace vc64 {

int
Headless::main(int argc, char *argv[])
{
    std::cout << "VirtualC64 Headless v" << VirtualC64::version();
    std::cout << " - (C)opyright Dirk W. Hoffmann" << std::endl << std::endl;

    // Parse all command line arguments
    parseArguments(argc, argv);

    // Check options
    if (keys.find("footprint") != keys.end())   { reportSize(); }
    if (keys.find("smoke") != keys.end())       { runScript(smokeTestScript); }
    if (keys.find("diagnose") != keys.end())    { runScript(selfTestScript); }
    if (keys.find("arg1") != keys.end())        { runScript(keys["arg1"]); }

    return returnCode;
}

void
Headless::parseArguments(int argc, char *argv[])
{
    // Remember the execution path
    keys["exec"] = std::filesystem::absolute(std::filesystem::path(argv[0])).string();

    // Parse command line arguments
    for (isize i = 1, n = 1; i < argc; i++) {

        auto arg = string(argv[i]);

        if (arg[0] == '-') {

            if (arg == "-f" || arg == "--footprint") { keys["footprint"] = "1"; continue; }
            if (arg == "-s" || arg == "--smoke")     { keys["smoke"] = "1"; continue; }
            if (arg == "-d" || arg == "--diagnose")  { keys["diagnose"] = "1"; continue; }
            if (arg == "-v" || arg == "--verbose")   { keys["verbose"] = "1"; continue; }
            if (arg == "-m" || arg == "--messages")  { keys["messages"] = "1"; continue; }

            throw SyntaxError("Invalid option '" + arg + "'");
        }

        auto path = std::filesystem::path(arg);
        keys["arg" + std::to_string(n++)] = std::filesystem::absolute(path).string();
    }

    // Check for syntax errors
    checkArguments();
}

void
Headless::checkArguments()
{
    // At most one file must be specified
    if (keys.find("arg2") != keys.end()) {
        throw SyntaxError("More than one script file is given");
    }

    if (keys.find("arg1") != keys.end()) {

        // The input file must exist
        if (!util::fileExists(keys["arg1"])) {
            throw SyntaxError("File " + keys["arg1"] + " does not exist");
        }

    } else {

        // Either -f, -s, or -d needs to be specified
        if (!keys.contains("footprint") &&
            !keys.contains("smoke") &&
            !keys.contains("diagnose")) throw SyntaxError("");
    }
}

void
Headless::runScript(const char **script)
{
    auto path = std::filesystem::temp_directory_path() / "script.retrosh";
    auto file = std::ofstream(path, std::ios::binary);

    for (isize i = 0; script[i] != nullptr; i++) {
        file << script[i] << std::endl;
    }
    runScript(path);
}

void
Headless::runScript(const fs::path &path)
{
    // Read the input script
    Script script(path);

    // Create an emulator instance
    VirtualC64 c64;

    // Plug in the three MEGA65 OpenROMs
    c64.c64.installOpenRoms();
    c64.c64.deleteRom(RomType::VC1541);
    c64.set(Opt::DRV_CONNECT, false, 0);
    c64.set(Opt::DRV_CONNECT, false, 1);

    // Redirect shell output to the console in verbose mode
    if (keys.find("verbose") != keys.end()) c64.retroShell.setStream(std::cout);

    // Launch the emulator thread
    c64.launch(this, vc64::process);

    // Execute script
    const auto timeout = util::Time::seconds(500.0);
    c64.retroShell.execScript(script);
    waitForWakeUp(timeout);
}

void
process(const void *listener, Message msg)
{
    ((Headless *)listener)->process(msg);
}

void
Headless::process(Message msg)
{
    static bool messages = keys.find("messages") != keys.end();
    
    if (messages) {
        
        std::cout << MsgEnum::key(msg.type);
        std::cout << "(" << msg.value << ")";
        std::cout << std::endl;
    }

    switch (msg.type) {

        case Msg::RSH_ERROR:

            returnCode = 1;
            wakeUp();
            break;

        case Msg::ABORT:

            wakeUp();
            break;

        default:
            break;
    }
}

void
Headless::reportSize()
{
    msg("               C64 : %zu bytes\n", sizeof(C64));
    msg("            Memory : %zu bytes\n", sizeof(Memory));
    msg("       DriveMemory : %zu bytes\n", sizeof(DriveMemory));
    msg("               CPU : %zu bytes\n", sizeof(CPU));
    msg("               CIA : %zu bytes\n", sizeof(CIA));
    msg("             VICII : %zu bytes\n", sizeof(VICII));
    msg("         SIDBridge : %zu bytes\n", sizeof(SIDBridge));
    msg("         PowerPort : %zu bytes\n", sizeof(PowerPort));
    msg("       ControlPort : %zu bytes\n", sizeof(ControlPort));
    msg("     ExpansionPort : %zu bytes\n", sizeof(ExpansionPort));
    msg("        SerialPort : %zu bytes\n", sizeof(SerialPort));
    msg("          Keyboard : %zu bytes\n", sizeof(Keyboard));
    msg("             Drive : %zu bytes\n", sizeof(Drive));
    msg("          ParCable : %zu bytes\n", sizeof(ParCable));
    msg("         Datasette : %zu bytes\n", sizeof(Datasette));
    msg("        RetroShell : %zu bytes\n", sizeof(RetroShell));
    msg("  RegressionTester : %zu bytes\n", sizeof(RegressionTester));
    msg("          MsgQueue : %zu bytes\n", sizeof(MsgQueue));
    msg("          CmdQueue : %zu bytes\n", sizeof(CmdQueue));
    msg("\n");
}

const char *
Headless::selfTestScript[] = {

    "# VirtualC64 Self Check",
    "# ",
    "# Dirk W. Hoffmann, 2024",
    "",
    "# Power up the emulator",
    "c64 power on",
    "",
    "# Let the emulator run for some time to see if it crashes",
    "wait 2",
    "",
    "# Terminate the application",
    "shutdown",

    nullptr
};

const char *
Headless::smokeTestScript[] = {

    "# RetroShell Syntax Check",
    "# ",
    "# This script checks the integrity of RetroShell. It runs several shell",
    "# commands to check if the command is recognized and anything breaks.",
    "# ",
    "# Dirk W. Hoffmann, 2024",

    "# ",
    "# Shell commands",
    "# ",

    "clear",
    "help",

    // Test a hidden command
    "joshua",

    // Catch an error with 'try'
    "try joshu",

    "regression set DEBUGCART true",
    "regression set DEBUGCART false",
    "regression set WATCHDOG 1000000",
    "regression set WATCHDOG 0",

    "# ",
    "# Components",
    "# ",

    "c64",
    "c64 set WARP_BOOT 50",
    "c64 set WARP_MODE ALWAYS",
    "c64 set WARP_MODE AUTO",
    "c64 set WARP_MODE NEVER",
    "c64 set SPEED_BOOST 75",
    "c64 set VSYNC yes",
    "c64 set VSYNC no",
    "c64 set RUN_AHEAD 2",
    "c64 defaults",
    "c64 reset",
    "c64 init PAL",
    "c64 init PAL_II",
    "c64 init PAL_OLD",
    "c64 init NTSC",
    "c64 init NTSC_II",
    "c64 init NTSC_OLD",

    "mem",
    "mem set INIT_PATTERN VICE",
    "mem set INIT_PATTERN CCS",
    "mem set INIT_PATTERN ZEROES",
    "mem set INIT_PATTERN ONES",
    "mem set INIT_PATTERN RANDOM",
    "mem set SAVE_ROMS true",
    "mem set SAVE_ROMS false",

    "cia1",
    "cia1 set REVISION MOS_6526",
    "cia1 set REVISION MOS_8521",
    "cia1 set TIMER_B_BUG true",
    "cia1 set TIMER_B_BUG false",
    "cia1 set IDLE_SLEEP true",
    "cia1 set IDLE_SLEEP false",

    "cia2",
    "cia2 set REVISION MOS_6526",
    "cia2 set REVISION MOS_8521",
    "cia2 set TIMER_B_BUG true",
    "cia2 set TIMER_B_BUG false",
    "cia2 set IDLE_SLEEP true",
    "cia2 set IDLE_SLEEP false",

    "vic",
    "vic set REVISION PAL_6569_R1",
    "vic set REVISION PAL_6569_R3",
    "vic set REVISION PAL_8565",
    "vic set REVISION NTSC_6567",
    "vic set REVISION NTSC_6567_R56A",
    "vic set REVISION NTSC_8562",
    "vic set POWER_SAVE true",
    "vic set POWER_SAVE false",
    "vic set GRAY_DOT_BUG true",
    "vic set GRAY_DOT_BUG false",
    "vic set GLUE_LOGIC DISCRETE",
    "vic set GLUE_LOGIC IC",
    "vic set HIDE_SPRITES true",
    "vic set HIDE_SPRITES false",
    "vic set SS_COLLISIONS true",
    "vic set SS_COLLISIONS false",
    "vic set SB_COLLISIONS true",
    "vic set SB_COLLISIONS false",

    "dmadebugger",
    "dmadebugger set ENABLE true",
    "dmadebugger set ENABLE false",
    "dmadebugger set MODE BG_LAYER",
    "dmadebugger set MODE FG_LAYER",
    "dmadebugger set MODE ODD_EVEN_LAYERS",
    "dmadebugger set OPACITY 50",
    "dmadebugger set CUT_LAYERS 4",
    "dmadebugger set CUT_OPACITY 50",
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
    "dmadebugger set COLOR0 $F00",
    "dmadebugger set COLOR1 0xF00",
    "dmadebugger set COLOR2 255",
    "dmadebugger set COLOR3 $FF0",
    "dmadebugger set COLOR4 $FFF",
    "dmadebugger set COLOR5 $0FF",

    "sid",
    "sid set ENABLE true",
    "try sid set ENABLE false",     // Primary SID cannot be disabled
    "try sid set ADDRESS 0xD420",   // Primary SID cannot be remapped
    "sid set REVISION MOS_6581",
    "sid set REVISION MOS_8580",
    "sid set FILTER false",
    "sid set FILTER true",
    "sid set ENGINE RESID",
    "sid set SAMPLING FAST",
    "sid set SAMPLING INTERPOLATE",
    "sid set SAMPLING RESAMPLE",
    "sid set SAMPLING RESAMPLE_FASTMEM",
    "sid set POWER_SAVE true",

    "sid2",
    "sid2 set ENABLE true",
    "sid2 set ENABLE false",
    "sid2 set ADDRESS 0xD420",
    "sid2 set REVISION MOS_6581",
    "sid2 set REVISION MOS_8580",
    "sid2 set FILTER true",
    "sid2 set FILTER false",
    "sid2 set ENGINE RESID",
    "sid2 set SAMPLING FAST",
    "sid2 set SAMPLING INTERPOLATE",
    "sid2 set SAMPLING RESAMPLE",
    "sid2 set SAMPLING RESAMPLE_FASTMEM",

    "# ",
    "# Ports",
    "# ",

    "power",
    "power set POWER_GRID STABLE_50HZ",
    "power set POWER_GRID STABLE_60HZ",
    "power set POWER_GRID UNSTABLE_50HZ",

    "audio",
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

    "user",
    "user set DEVICE NONE",
    "user set DEVICE PARCABLE",
    "user set DEVICE PARCABLE_DOLPHIN",
    "user set DEVICE RS232",

    "video",
    "video set WHITE_NOISE true",
    "video set WHITE_NOISE false",

    "expansion",
    "expansion set REU_SPEED 1",
    "expansion attach reu 128",
    "expansion attach reu 256",
    "expansion attach reu 512",
    "expansion attach reu 2048",
    "expansion attach georam 512",
    "expansion attach georam 1024",
    "expansion attach georam 2048",
    "expansion attach georam 4096",

    "# ",
    "# Peripherals",
    "# ",

    "monitor set PALETTE COLOR",
    "monitor set PALETTE BLACK_WHITE",
    "monitor set PALETTE PAPER_WHITE",
    "monitor set PALETTE GREEN",
    "monitor set PALETTE AMBER",
    "monitor set PALETTE SEPIA",
    "monitor set BRIGHTNESS 50",
    "monitor set CONTRAST 50",
    "monitor set SATURATION 50",

    "keyboard press 32",
    "keyboard release 32",
    "keyboard type text \"text\"",
    "keyboard type load",
    "keyboard type run",

    "drive8",
    "drive8 set AUTO_CONFIG true",
    "drive8 set AUTO_CONFIG false",
    "drive8 set TYPE VC1541",
    "drive8 set TYPE VC1541C",
    "drive8 set TYPE VC1541II",
    "drive8 set RAM 6000_7FFF",
    "drive8 set RAM 8000_9FFF",
    "drive8 set RAM NONE",
    "drive8 set PARCABLE STANDARD",
    "drive8 set PARCABLE DOLPHIN3",
    "drive8 set PARCABLE NONE",
    "drive8 protect",
    "drive8 unprotect",
    "drive8 eject",
    "drive8 newdisk NODOS",
    "drive8 newdisk CBM",

    "drive9",
    "drive9 set AUTO_CONFIG true",
    "drive9 set AUTO_CONFIG false",
    "drive9 set TYPE VC1541",
    "drive9 set TYPE VC1541C",
    "drive9 set TYPE VC1541II",
    "drive9 set RAM 6000_7FFF",
    "drive9 set RAM 8000_9FFF",
    "drive9 set RAM NONE",
    "drive9 set PARCABLE STANDARD",
    "drive9 set PARCABLE DOLPHIN3",
    "drive9 set PARCABLE NONE",
    "drive9 protect",
    "drive9 unprotect",
    "drive9 eject",
    "drive9 newdisk NODOS",
    "drive9 newdisk CBM",

    "datasette",
    "datasette set MODEL C1530",
    "datasette rewind",
    "datasette rewind to 10",

    "joystick1",
    "joystick1 set ENABLE true",
    "joystick1 set ENABLE false",
    "joystick1 set BURSTS true",
    "joystick1 set BURSTS false",
    "joystick1 set BULLETS 10",
    "joystick1 set DELAY 10",
    "joystick1 press",
    "joystick1 unpress",
    "joystick1 pull left",
    "joystick1 pull right",
    "joystick1 pull up",
    "joystick1 pull down",
    "joystick1 release x",
    "joystick1 release y",

    "joystick2",
    "joystick2 set ENABLE true",
    "joystick2 set ENABLE false",
    "joystick2 set BURSTS true",
    "joystick2 set BURSTS false",
    "joystick2 set BULLETS 10",
    "joystick2 set DELAY 10",
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

    "paddle1",
    "paddle1 set ORIENTATION HORIZONTAL",
    "paddle1 set ORIENTATION HORIZONTAL_FLIPPED",
    "paddle1 set ORIENTATION VERTICAL",
    "paddle1 set ORIENTATION VERTICAL_FLIPPED",

    "paddle2",
    "paddle2 set ORIENTATION HORIZONTAL",
    "paddle2 set ORIENTATION HORIZONTAL_FLIPPED",
    "paddle2 set ORIENTATION VERTICAL",
    "paddle2 set ORIENTATION VERTICAL_FLIPPED",

    "rs232",
    "rs232 set DEVICE NONE",
    "rs232 set DEVICE COMMANDER",
    "rs232 set DEVICE LOOPBACK",
    "rs232 set DEVICE RETROSHELL",
    "rs232 set BAUD 300",

    "config",
    "config diff",

    "host",
    "host set REFRESH_RATE 60",
    "host set SAMPLE_RATE 44100",

    "server",

    "shutdown",

    nullptr
};

}
