// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "Interpreter.h"
#include "C64.h"

namespace vc64 {

void
Interpreter::initCommons(Command &root)
{
    //
    // Common commands
    //

    root.newGroup("Controlling the shell");

    root.add({"."},
             "Enters or exists the debugger",
             &RetroShell::exec <Token::debug>);

    root.add({"clear"},
             "Clears the console window",
             &RetroShell::exec <Token::clear>);

    root.add({"close"},
             "Hides the console window",
             &RetroShell::exec <Token::close>);

    root.add({"help"}, { }, {Arg::command},
             "Prints usage information",
             &RetroShell::exec <Token::help>);

    root.add({"joshua"},
             "",
             &RetroShell::exec <Token::easteregg>);

    root.add({"source"}, {Arg::path},
             "Processes a command script",
             &RetroShell::exec <Token::source>);

    root.add({"wait"}, {Arg::value, Arg::seconds},
             "Pauses the execution of a command script",
             &RetroShell::exec <Token::wait>);
}

void
Interpreter::initCommandShell(Command &root)
{
    initCommons(root);

    //
    // Top-level commands
    //

    root.newGroup("Regression testing");

    root.add({"regression"},    "Runs the regression tester");
    root.add({"screenshot"},    "Manages screenshots");

    root.newGroup("Controlling components");

    root.add({"c64"},           "The virtual Commodore 64");
    root.add({"memory"},        "Ram and Rom");
    root.add({"cpu"},           "MOS 6510 CPU");
    root.add({"cia1"},          "Complex Interface Adapter 1");
    root.add({"cia2"},          "Complex Interface Adapter 2");
    root.add({"vicii"},         "Video Interface Controller");
    root.add({"dmadebugger"},   "DMA Debugger");
    root.add({"sid"},           "Sound Interface Device");

    root.newGroup("Controlling ports");

    root.add({"controlport1"},  "Control port 1");
    root.add({"controlport2"},  "Control port 2");
    root.add({"expansion"},     "Expansion port");

    root.newGroup("Controlling peripherals");

    root.add({"monitor"},       "C64 monitor");
    root.add({"keyboard"},      "Keyboard");
    root.add({"mouse"},         "mouse");
    root.add({"joystick"},      "Joystick");
    root.add({"datasette"},     "Commodore tape drive");
    root.add({"drive8"},        "Floppy drive 8");
    root.add({"drive9"},        "Floppy drive 9");
    root.add({"parcable"},      "Parallel drive cable");


    //
    // Regression testing
    //

    root.add({"regression", "setup"}, { C64ModelEnum::argList() },
             "Initializes the test environment",
             &RetroShell::exec <Token::regression, Token::setup>);


    root.add({"regression", "run"}, { Arg::path },
             "Launches a regression test",
             &RetroShell::exec <Token::regression, Token::run>);

    root.add({"screenshot", "set"},
             "Configures the screenshot");

    root.add({"screenshot", "set", "filename"}, { Arg::path },
             "Assigns the screen shot filename",
             &RetroShell::exec <Token::screenshot, Token::set, Token::filename>);

    root.add({"screenshot", "set", "cutout"}, { Arg::value, Arg::value, Arg::value, Arg::value },
             "Adjusts the texture cutout",
             &RetroShell::exec <Token::screenshot, Token::set, Token::cutout>);

    root.add({"screenshot", "save"}, { Arg::path },
             "Saves a screenshot and exits the emulator",
             &RetroShell::exec <Token::screenshot, Token::save>);

    
    //
    // C64
    //

    root.add({"c64", "config"},
             "Displays the current configuration",
             &RetroShell::exec <Token::c64, Token::config>);

    root.add({"c64", "set"},
             "Configures the component");

    root.add({"c64", "set", "fpsmode"}, { FpsModeEnum::argList() },
             "Selects the frame mode",
             &RetroShell::exec <Token::c64, Token::set, Token::fpsmode>);

    root.add({"c64", "set", "fps"}, { Arg::value },
             "Sets the frames per seconds",
             &RetroShell::exec <Token::c64, Token::set, Token::fps>);

    root.add({"c64", "power"}, { Arg::onoff },
             "Switches the C64 on or off",
             &RetroShell::exec <Token::c64, Token::power>);
    
    root.add({"c64", "reset"},
             "Performs a hard reset",
             &RetroShell::exec <Token::c64, Token::reset>);

    root.add({"c64", "init"}, { C64ModelEnum::argList() },
             "Initializes the emulator with factory settings",
             &RetroShell::exec <Token::c64, Token::init>);

    root.add({"c64", "defaults"},
             "Displays the user defaults storage",
             &RetroShell::exec <Token::c64, Token::defaults>);

    
    //
    // Memory
    //

    root.add({"memory", "config"},
             "Displays the current configuration",
             &RetroShell::exec <Token::memory, Token::config>);

    root.add({"memory", "set"},
             "Configures the component");

    root.add({"memory", "set", "raminit" }, { RamPatternEnum::argList() },
             "Determines how Ram is initialized on startup",
             &RetroShell::exec <Token::memory, Token::set, Token::raminitpattern>);

    root.add({"memory", "set", "saveroms"}, { Arg::onoff },
             "Save Roms to snapshot files",
             &RetroShell::exec <Token::memory, Token::set, Token::saveroms>);

    root.add({"memory", "load"}, { Arg::path },
             "Installs a Rom image",
             &RetroShell::exec <Token::memory, Token::load>);

    root.add({"memory", "flash"}, { Arg::path },
             "Flashes a file into memory",
             &RetroShell::exec <Token::memory, Token::flash>);

    
    //
    // Drive
    //

    for (isize i = 0; i < 2; i++) {

        string drive = (i == 0) ? "drive8" : "drive9";

        root.add({drive, "config"},
                 "Displays the current configuration",
                 &RetroShell::exec <Token::drive, Token::config>);
        
        root.add({drive, "connect"},
                 "Connects the drive",
                 &RetroShell::exec <Token::drive, Token::connect>);
        
        root.add({drive, "disconnect"},
                 "Disconnects the drive",
                 &RetroShell::exec <Token::drive, Token::disconnect>);
        
        root.add({drive, "eject"},
                 "Ejects a floppy disk",
                 &RetroShell::exec <Token::drive, Token::eject>);
        
        root.add({drive, "insert"}, { Arg::path },
                 "Inserts a floppy disk",
                 &RetroShell::exec <Token::drive, Token::insert>, 1);
        
        root.add({drive, "newdisk"}, { DOSTypeEnum::argList() },
                 "Inserts a new blank disk",
                 &RetroShell::exec <Token::drive, Token::insert, Token::newdisk>);
    }
    
    
    //
    // Datasette
    //

    root.add({"datasette", "config"},
             "Displays the current configuration",
             &RetroShell::exec <Token::datasette, Token::config>);

    root.add({"datasette", "connect"},
             "Connects the datasette",
             &RetroShell::exec <Token::datasette, Token::connect>);

    root.add({"datasette", "disconnect"},
             "Disconnects the datasette",
             &RetroShell::exec <Token::datasette, Token::disconnect>);

    root.add({"datasette", "rewind"},
             "Rewinds the tape",
             &RetroShell::exec <Token::datasette, Token::rewind>);

    root.add({"datasette", "rewind", "to"}, { Arg::value },
             "Rewinds the tape to a specific position",
             &RetroShell::exec <Token::datasette, Token::rewind, Token::to>);

    
    //
    // CPU
    //

    
    //
    // CIA
    //

    for (isize i = 0; i < 2; i++) {

        string cia = (i == 0) ? "cia1" : "cia1";

        root.add({cia, "config"},
                 "Displays the current configuration",
                 &RetroShell::exec <Token::cia, Token::config>);
        
        root.add({cia, "set"},
                 "Configures the component");
        
        root.add({cia, "set", "revision"}, { CIARevisionEnum::argList() },
                 "Selects the emulated chip model",
                 &RetroShell::exec <Token::cia, Token::set, Token::revision>);
        
        root.add({cia, "set", "timerbbug"}, { Arg::onoff },
                 "Enables or disables the timer B hardware bug",
                 &RetroShell::exec <Token::cia, Token::set, Token::timerbbug>);
    }


    //
    // VICII
    //

    root.add({"vicii", "config"},
             "Displays the current configuration",
             &RetroShell::exec <Token::vicii, Token::config>);

    root.add({"vicii", "set"},
             "Configures the component");

    root.add({"vicii", "set", "revision"}, { VICIIRevisionEnum::argList() },
             "Selects the emulated chip model",
             &RetroShell::exec <Token::vicii, Token::set, Token::revision>);

    root.add({"vicii", "set", "graydotbug"}, { Arg::onoff },
             "Enables or disables the gray dot bug",
             &RetroShell::exec <Token::vicii, Token::set, Token::graydotbug>);

    root.add({"vicii", "set", "gluelogic"}, { Arg::onoff },
             "Configures the logic board",
             &RetroShell::exec <Token::vicii, Token::set, Token::gluelogic>);

    root.add({"vicii", "set", "sscollisions"}, { Arg::onoff },
             "Enables or disables sprite-sprite collision detection",
             &RetroShell::exec <Token::vicii, Token::set, Token::sscollisions>);

    root.add({"vicii", "set", "sbcollisions"}, { Arg::onoff },
             "Enables or disables sprite-background collision detection",
             &RetroShell::exec <Token::vicii, Token::set, Token::sbcollisions>);

    
    //
    // DMA Debugger
    //

    root.add({"dmadebugger", "config"},
             "Displays the current configuration",
             &RetroShell::exec <Token::dmadebugger, Token::config>);

    root.add({"dmadebugger", "open"},
             "Opens the DMA debugger",
             &RetroShell::exec <Token::dmadebugger, Token::open>);

    root.add({"dmadebugger", "close"},
             "Closes the DMA debugger",
             &RetroShell::exec <Token::dmadebugger, Token::close>);

    root.add({"dmadebugger", "show"},
             "Enables the debugger for a certain DMA channel");

    root.add({"dmadebugger", "show", "raccesses"},
             "Visualizes refresh cycles",
             &RetroShell::exec <Token::dmadebugger, Token::show, Token::raccesses>);

    root.add({"dmadebugger", "show", "iaccesses"},
             "Visualizes idle accesses",
             &RetroShell::exec <Token::dmadebugger, Token::show, Token::iaccesses>);

    root.add({"dmadebugger", "show", "caccesses"},
             "Visualizes character accesses",
             &RetroShell::exec <Token::dmadebugger, Token::show, Token::caccesses>);

    root.add({"dmadebugger", "show", "gaccesses"},
             "Visualizes graphics accesses",
             &RetroShell::exec <Token::dmadebugger, Token::show, Token::gaccesses>);

    root.add({"dmadebugger", "show", "paccesses"},
             "Visualizes sprite pointer accesses",
             &RetroShell::exec <Token::dmadebugger, Token::show, Token::paccesses>);

    root.add({"dmadebugger", "show", "saccesses"},
             "Visualizes sprite accesses",
             &RetroShell::exec <Token::dmadebugger, Token::show, Token::saccesses>);

    root.add({"dmadebugger", "hide"},
             "Disables the debugger for a certain DMA channel");

    root.add({"dmadebugger", "hide", "raccesses"},
             "Hides refresh cycles",
             &RetroShell::exec <Token::dmadebugger, Token::hide, Token::raccesses>);

    root.add({"dmadebugger", "hide", "iaccesses"},
             "Hides idle accesses",
             &RetroShell::exec <Token::dmadebugger, Token::hide, Token::iaccesses>);

    root.add({"dmadebugger", "hide", "caccesses"},
             "Hides character accesses",
             &RetroShell::exec <Token::dmadebugger, Token::hide, Token::caccesses>);

    root.add({"dmadebugger", "hide", "gaccesses"},
             "Hides graphics accesses",
             &RetroShell::exec <Token::dmadebugger, Token::hide, Token::gaccesses>);

    root.add({"dmadebugger", "hide", "paccesses"},
             "Hides sprite pointer accesses",
             &RetroShell::exec <Token::dmadebugger, Token::hide, Token::paccesses>);

    root.add({"dmadebugger", "hide", "saccesses"},
             "Hides sprite accesses",
             &RetroShell::exec <Token::dmadebugger, Token::hide, Token::saccesses>);

    
    //
    // Monitor
    //

    root.add({"monitor", "set"},
             "Configures the component");

    root.add({"monitor", "set", "palette"}, { PaletteEnum::argList() },
             "Selects the color palette",
             &RetroShell::exec <Token::monitor, Token::set, Token::palette>);

    root.add({"monitor", "set", "brightness"}, { Arg::value },
             "Adjusts the monitor brightness",
             &RetroShell::exec <Token::monitor, Token::set, Token::brightness>);

    root.add({"monitor", "set", "contrast"}, { Arg::value },
             "Adjusts the monitor contrast",
             &RetroShell::exec <Token::monitor, Token::set, Token::contrast>);

    root.add({"monitor", "set", "saturation"}, { Arg::value },
             "Adjusts the color saturation",
             &RetroShell::exec <Token::monitor, Token::set, Token::saturation>);

    
    //
    // SID
    //

    root.add({"sid", "config"},
             "Displays the current configuration",
             &RetroShell::exec <Token::sid, Token::config>);

    root.add({"sid", "set"},
             "Configures the component");

    root.add({"sid", "set", "engine"}, { SIDEngineEnum::argList() },
             "Selects the SID backend",
             &RetroShell::exec <Token::sid, Token::set, Token::engine>);

    root.add({"sid", "set", "revision"}, { SIDRevisionEnum::argList() },
             "Selects the emulated chip model",
             &RetroShell::exec <Token::sid, Token::set, Token::revision>);

    root.add({"sid", "set", "sampling"}, { SamplingMethodEnum::argList() },
             "Selects the sampling method",
             &RetroShell::exec <Token::sid, Token::set, Token::sampling>);

    root.add({"sid", "set", "filter"}, { Arg::onoff },
             "Configures the audio filter",
             &RetroShell::exec <Token::sid, Token::set, Token::filter>);

    root.add({"sid", "set", "volume"},
             "Sets the volume");

    root.add({"sid", "set", "volume", "channel0"}, { Arg::volume },
             "Sets the volume for the first SID",
             &RetroShell::exec <Token::sid, Token::set, Token::volume>, 0);

    root.add({"sid", "set", "volume", "channel1"}, { Arg::volume },
             "Sets the volume for the second SID",
             &RetroShell::exec <Token::sid, Token::set, Token::volume>, 1);
    
    root.add({"sid", "set", "volume", "channel2"}, { Arg::volume },
             "Sets the volume for the third SID",
             &RetroShell::exec <Token::sid, Token::set, Token::volume>, 2);
    
    root.add({"sid", "set", "volume", "channel3"}, { Arg::volume },
             "Sets the volume for the fourth SID",
             &RetroShell::exec <Token::sid, Token::set, Token::volume>, 3);

    root.add({"sid", "set", "volume", "left"}, { Arg::volume },
             "Sets the master volume for the left speaker",
             &RetroShell::exec <Token::sid, Token::set, Token::volume>, 4);
    
    root.add({"sid", "set", "volume", "right"}, { Arg::volume },
             "Sets the master volume for the right speaker",
             &RetroShell::exec <Token::sid, Token::set, Token::volume>, 5);

    root.add({"sid", "set", "pan"},
             "Sets the pan for one of the four SIDs");
    
    root.add({"sid", "set", "pan", "channel0"}, { Arg::value },
             "Sets the pan for the first SID",
             &RetroShell::exec <Token::sid, Token::set, Token::pan>, 0);
    
    root.add({"sid", "set", "pan", "channel1"}, { Arg::value },
             "Sets the pan for the second SID",
             &RetroShell::exec <Token::sid, Token::set, Token::pan>, 1);
    
    root.add({"sid", "set", "pan", "channel2"}, { Arg::value },
             "Sets the pan for the third SID",
             &RetroShell::exec <Token::sid, Token::set, Token::pan>, 2);
    
    root.add({"sid", "set", "pan", "channel3"}, { Arg::value },
             "Sets the pan for the fourth SID",
             &RetroShell::exec <Token::sid, Token::set, Token::pan>, 3);

    
    //
    // Control port
    //


    //
    // Expansion port
    //

    root.add({"expansion", "attach"}, { Arg::path },
             "Attaches a cartridge",
             &RetroShell::exec <Token::expansion, Token::attach>);

    
    //
    // Keyboard
    //

    root.add({"keyboard", "type"},
             "Types a command");

    root.add({"keyboard", "type", "load"},
             "Types LOAD \"*\",8,1",
             &RetroShell::exec <Token::keyboard, Token::type, Token::load>);

    root.add({"keyboard", "type", "run"},
             "Types RUN",
             &RetroShell::exec <Token::keyboard, Token::type, Token::run>);

    root.add({"keyboard", "press"}, { Arg::value },
             "Presses a key",
             &RetroShell::exec <Token::keyboard, Token::press>);

    /*
    root.add({"keyboard", "press", "shiftlock"},
             "command", "Presses the shift lock key",
             &RetroShell::exec <Token::keyboard, Token::press, Token::shiftlock>);
    */

    root.add({"keyboard", "release"}, { Arg::value },
             "Presses a key",
             &RetroShell::exec <Token::keyboard, Token::release>);

    /*
    root.add({"keyboard", "release", "shiftlock"},
             "command", "Releases the shift lock key",
             &RetroShell::exec <Token::keyboard, Token::release, Token::shiftlock>);
    */
    
    //
    // Joystick
    //

    root.add({"joystick", "config"},
             "Displays the current configuration",
             &RetroShell::exec <Token::joystick, Token::config>);

    root.add({"joystick", "set"},
             "Configures the component");

    root.add({"joystick", "set", "autofire"}, { Arg::onoff },
             "Enables or disables auto fire mode",
             &RetroShell::exec <Token::joystick, Token::set, Token::autofire>);

    root.add({"joystick", "set", "bullets"}, { Arg::value },
             "Sets the number of bullets per auto fire shot",
             &RetroShell::exec <Token::joystick, Token::set, Token::bullets>);

    root.add({"joystick", "set", "delay"}, { Arg::value },
             "Sets the auto fire delay in frames",
             &RetroShell::exec <Token::joystick, Token::set, Token::delay>);

    
    //
    // Mouse
    //

    root.add({"mouse", "config"},
             "Displays the current configuration",
             &RetroShell::exec <Token::mouse, Token::config>);

    root.add({"mouse", "set"},
             "Configures the component");

    root.add({"mouse", "set", "model"}, { MouseModelEnum::argList() },
             "Selects the mouse model",
             &RetroShell::exec <Token::mouse, Token::set, Token::model>);

    root.add({"mouse", "set", "velocity"}, { Arg::value },
             "Sets the horizontal and vertical mouse velocity",
             &RetroShell::exec <Token::mouse, Token::set, Token::velocity>);

    root.add({"mouse", "set", "shakedetector"}, { Arg::onoff },
             "Enables or disables the shake detector",
             &RetroShell::exec <Token::mouse, Token::set, Token::shakedetector>);

    
    //
    // Parallel cable
    //

    root.add({"parcable", "config"},
             "Displays the current configuration",
             &RetroShell::exec <Token::parcable, Token::config>);
}

}
