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
#include "RetroShell.h"

namespace vc64 {

void
Interpreter::registerInstructions()
{
    //
    // Commands
    //
    
    root.add({"clear"},
             "command", "Clears the console window",
             &RetroShell::exec <Token::clear>);
    root.seek("clear")->hidden = true;

    root.add({"close"},
             "command", "Hides the debug console",
             &RetroShell::exec <Token::close>);
    root.seek("close")->hidden = true;
    
    root.add({"joshua"},
             "command", "",
             &RetroShell::exec <Token::easteregg>);
    root.seek("joshua")->hidden = true;

    root.add({"source"},
             "command", "Processes a command script",
             &RetroShell::exec <Token::source>, 1);
    
    root.add({"wait"},
             "command", "Pauses the execution of a command script",
             &RetroShell::exec <Token::wait>, 2);

    
    //
    // Regression testing
    //

    root.add({"regression"},
             "component", "");
    root.seek("regression")->hidden = true;

    root.add({"regression", "setup"},
             "command", "Initializes the test environment",
             &RetroShell::exec <Token::regression, Token::setup>, 1);

    root.add({"regression", "run"},
             "command", "Launches a regression test",
             &RetroShell::exec <Token::regression, Token::run>, 1);

    root.add({"screenshot"},
             "component", "");
    root.seek("screenshot")->hidden = true;
    
    root.add({"screenshot", "set"},
             "command", "Configures the regression test");

    root.add({"screenshot", "set", "filename"},
             "key", "Assigns the screen shot filename",
             &RetroShell::exec <Token::screenshot, Token::set, Token::filename>, 1);

    root.add({"screenshot", "set", "cutout"},
             "key", "Adjusts the texture cutout",
             &RetroShell::exec <Token::screenshot, Token::set, Token::cutout>, 4);

    root.add({"screenshot", "save"},
             "key", "Saves a screenshot and exits the emulator",
             &RetroShell::exec <Token::screenshot, Token::save>, 1);

    
    //
    // C64
    //
    
    root.add({"c64"},
             "component", "The virtual Commodore 64");

    root.add({"c64", "config"},
             "command", "Displays the current configuration",
             &RetroShell::exec <Token::c64, Token::config>);

    root.add({"c64", "power"},
             "command", "Switches the C64 on or off");
    
    root.add({"c64", "power", "on"},
             "state", "Switches the C64 on",
             &RetroShell::exec <Token::c64, Token::power, Token::on>);

    root.add({"c64", "power", "off"},
             "state", "Switches the C64 off",
             &RetroShell::exec <Token::c64, Token::power, Token::off>);

    root.add({"c64", "run"},
             "command", "Starts the emulator thread",
             &RetroShell::exec <Token::c64, Token::run>);

    root.add({"c64", "pause"},
             "command", "Halts the emulator thread",
             &RetroShell::exec <Token::c64, Token::pause>);
    
    root.add({"c64", "reset"},
             "command", "Performs a hard reset",
             &RetroShell::exec <Token::c64, Token::reset>);
    
    root.add({"c64", "inspect"},
             "command", "Displays the component state");

    root.add({"c64", "inspect", "state"},
             "command", "Displays the current state",
             &RetroShell::exec <Token::c64, Token::inspect, Token::state>);

    root.add({"c64", "inspect", "defaults"},
             "command", "Displays the user defaults storage",
             &RetroShell::exec <Token::c64, Token::inspect, Token::defaults>);

    root.add({"c64", "init"},
             "command", "Initializes the emulator with factory settings",
             &RetroShell::exec <Token::c64, Token::init>, 1);

    
    //
    // Memory
    //
    
    root.add({"memory"},
             "component", "Ram and Rom");
    
    root.add({"memory", "config"},
             "command", "Displays the current configuration",
             &RetroShell::exec <Token::memory, Token::config>);

    root.add({"memory", "set"},
             "command", "Configures the component");

    root.add({"memory", "set", "raminit"},
             "key", "Determines how Ram is initialized on startup",
             &RetroShell::exec <Token::memory, Token::set, Token::raminitpattern>, 1);

    root.add({"memory", "set", "saveroms"},
             "key", "Save Roms to snapshot files",
             &RetroShell::exec <Token::memory, Token::set, Token::saveroms>, 1);

    root.add({"memory", "load"},
             "command", "Installs a Rom image",
             &RetroShell::exec <Token::memory, Token::load>, 1);

    root.add({"memory", "flash"},
             "command", "Flashes a file into memory",
             &RetroShell::exec <Token::memory, Token::flash>, 1);

    root.add({"memory", "inspect"},
             "command", "Displays the component state",
             &RetroShell::exec <Token::memory, Token::inspect>);

    
    //
    // Drive
    //

    root.add({"drive8"},
             "component", "Floppy drive 8");

    root.add({"drive9"},
             "component", "Floppy drive 9");

    for (isize i = 0; i < 2; i++) {

        string drive = (i == 0) ? "drive8" : "drive9";

        root.add({drive, "config"},
                 "command", "Displays the current configuration",
                 &RetroShell::exec <Token::drive, Token::config>);
        
        root.add({drive, "connect"},
                 "command", "Connects the drive",
                 &RetroShell::exec <Token::drive, Token::connect>);
        
        root.add({drive, "disconnect"},
                 "command", "Disconnects the drive",
                 &RetroShell::exec <Token::drive, Token::disconnect>);
        
        root.add({drive, "eject"},
                 "command", "Ejects a floppy disk",
                 &RetroShell::exec <Token::drive, Token::eject>);
        
        root.add({drive, "insert"},
                 "command", "Inserts a floppy disk",
                 &RetroShell::exec <Token::drive, Token::insert>, 1);
        
        root.add({drive, "newdisk"},
                 "command", "Inserts a new blank disk",
                 &RetroShell::exec <Token::drive, Token::insert, Token::newdisk>, 1);
        
        root.add({drive, "inspect"},
                 "command", "Displays the component state");
        
        root.add({drive, "inspect", "state"},
                 "command", "Displays the drive state",
                 &RetroShell::exec <Token::drive, Token::inspect, Token::state>);
        
        root.add({drive, "inspect", "bankmap"},
                 "command", "Displays the memory layout",
                 &RetroShell::exec <Token::drive, Token::inspect, Token::bankmap>);
        
        root.add({drive, "inspect", "disk"},
                 "command", "Displays the disk state",
                 &RetroShell::exec <Token::drive, Token::inspect, Token::disk>);
    }
    
    
    //
    // Datasette
    //

    root.add({"datasette"},
             "component", "Commodore tape drive");

    root.add({"datasette", "config"},
             "command", "Displays the current configuration",
             &RetroShell::exec <Token::datasette, Token::config>);

    root.add({"datasette", "connect"},
             "command", "Connects the datasette",
             &RetroShell::exec <Token::datasette, Token::connect>);

    root.add({"datasette", "disconnect"},
             "command", "Disconnects the datasette",
             &RetroShell::exec <Token::datasette, Token::disconnect>);

    root.add({"datasette", "inspect"},
             "command", "Displays the component state",
             &RetroShell::exec <Token::datasette, Token::inspect>);

    root.add({"datasette", "rewind"},
             "command", "Rewinds the tape",
             &RetroShell::exec <Token::datasette, Token::rewind>);

    root.add({"datasette", "rewind", "to"},
             "command", "Rewinds the tape to a specific position",
             &RetroShell::exec <Token::datasette, Token::rewind, Token::to>, 1);

    
    //
    // CPU
    //
    
    root.add({"cpu"},
             "component", "Motorola 68k CPU");
    
    root.add({"cpu", "inspect"},
             "command", "Displays the component state");

    root.add({"cpu", "inspect", "state"},
             "command", "Displays the current state",
             &RetroShell::exec <Token::cpu, Token::inspect, Token::state>);

    root.add({"cpu", "inspect", "registers"},
             "command", "Displays the current register values",
             &RetroShell::exec <Token::cpu, Token::inspect, Token::registers>);

    
    //
    // CIA
    //
    
    root.add({"cia1"},
             "component", "Complex Interface Adapter 1");

    root.add({"cia2"},
             "component", "Complex Interface Adapter 2");

    for (isize i = 0; i < 2; i++) {

        string cia = (i == 0) ? "cia1" : "cia1";

        root.add({cia, "config"},
                 "command", "Displays the current configuration",
                 &RetroShell::exec <Token::cia, Token::config>);
        
        root.add({cia, "set"},
                 "command", "Configures the component");
        
        root.add({cia, "set", "revision"},
                 "key", "Selects the emulated chip model",
                 &RetroShell::exec <Token::cia, Token::set, Token::revision>, 1);
        
        root.add({cia, "set", "timerbbug"},
                 "key", "Enables or disables the timer B hardware bug",
                 &RetroShell::exec <Token::cia, Token::set, Token::timerbbug>, 1);
        
        root.add({cia, "inspect"},
                 "command", "Displays the component state");
        
        root.add({cia, "inspect", "state"},
                 "category", "Displays the current state",
                 &RetroShell::exec <Token::cia, Token::inspect, Token::state>);
        
        root.add({cia, "inspect", "registers"},
                 "category", "Displays the current register values",
                 &RetroShell::exec <Token::cia, Token::inspect, Token::registers>);
        
        root.add({cia, "inspect", "tod"},
                 "category", "Displays the state of the TOD clock",
                 &RetroShell::exec <Token::cia, Token::inspect, Token::tod>);
    }
    
    //
    // VICII
    //
    
    root.add({"vicii"},
             "component", "Video Interface Controller");
    
    root.add({"vicii", "config"},
             "command", "Displays the current configuration",
             &RetroShell::exec <Token::vicii, Token::config>);

    root.add({"vicii", "set"},
             "command", "Configures the component");

    root.add({"vicii", "set", "revision"},
             "key", "Selects the emulated chip model",
             &RetroShell::exec <Token::vicii, Token::set, Token::revision>, 1);

    root.add({"vicii", "set", "speed"},
             "key", "Sets the emulation speed",
             &RetroShell::exec <Token::vicii, Token::set, Token::speed>, 1);

    root.add({"vicii", "set", "graydotbug"},
             "key", "Enables or disables the gray dot bug",
             &RetroShell::exec <Token::vicii, Token::set, Token::graydotbug>, 1);

    root.add({"vicii", "set", "gluelogic"},
             "key", "Configures the logic board",
             &RetroShell::exec <Token::vicii, Token::set, Token::gluelogic>, 1);

    root.add({"vicii", "set", "sscollisions"},
             "key", "Enables or disables sprite-sprite collision detection",
             &RetroShell::exec <Token::vicii, Token::set, Token::sscollisions>, 1);

    root.add({"vicii", "set", "sbcollisions"},
             "key", "Enables or disables sprite-background collision detection",
             &RetroShell::exec <Token::vicii, Token::set, Token::sbcollisions>, 1);
    
    root.add({"vicii", "inspect"},
             "command", "Displays the internal state");

    root.add({"vicii", "inspect", "registers"},
             "category", "Displays the register contents",
             &RetroShell::exec <Token::vicii, Token::inspect, Token::registers>);

    root.add({"vicii", "inspect", "state"},
             "category", "Displays the current state",
             &RetroShell::exec <Token::vicii, Token::inspect, Token::state>);

    
    //
    // DMA Debugger
    //

    root.add({"dmadebugger"},
             "component", "DMA Debugger");

    root.add({"dmadebugger", "config"},
             "command", "Displays the current configuration",
             &RetroShell::exec <Token::dmadebugger, Token::config>);

    root.add({"dmadebugger", "open"},
             "command", "Opens the DMA debugger",
             &RetroShell::exec <Token::dmadebugger, Token::open>);

    root.add({"dmadebugger", "close"},
             "command", "Closes the DMA debugger",
             &RetroShell::exec <Token::dmadebugger, Token::close>);

    root.add({"dmadebugger", "show"},
             "command", "Enables the debugger for a certain DMA channel");

    root.add({"dmadebugger", "show", "raccesses"},
             "command", "Visualizes refresh cycles",
             &RetroShell::exec <Token::dmadebugger, Token::show, Token::raccesses>);

    root.add({"dmadebugger", "show", "iaccesses"},
             "command", "Visualizes idle accesses",
             &RetroShell::exec <Token::dmadebugger, Token::show, Token::iaccesses>);

    root.add({"dmadebugger", "show", "caccesses"},
             "command", "Visualizes character accesses",
             &RetroShell::exec <Token::dmadebugger, Token::show, Token::caccesses>);

    root.add({"dmadebugger", "show", "gaccesses"},
             "command", "Visualizes graphics accesses",
             &RetroShell::exec <Token::dmadebugger, Token::show, Token::gaccesses>);

    root.add({"dmadebugger", "show", "paccesses"},
             "command", "Visualizes sprite pointer accesses",
             &RetroShell::exec <Token::dmadebugger, Token::show, Token::paccesses>);

    root.add({"dmadebugger", "show", "saccesses"},
             "command", "Visualizes sprite accesses",
             &RetroShell::exec <Token::dmadebugger, Token::show, Token::saccesses>);

    root.add({"dmadebugger", "hide"},
             "command", "Disables the debugger for a certain DMA channel");

    root.add({"dmadebugger", "hide", "raccesses"},
             "command", "Hides refresh cycles",
             &RetroShell::exec <Token::dmadebugger, Token::hide, Token::raccesses>);

    root.add({"dmadebugger", "hide", "iaccesses"},
             "command", "Hides idle accesses",
             &RetroShell::exec <Token::dmadebugger, Token::hide, Token::iaccesses>);

    root.add({"dmadebugger", "hide", "caccesses"},
             "command", "Hides character accesses",
             &RetroShell::exec <Token::dmadebugger, Token::hide, Token::caccesses>);

    root.add({"dmadebugger", "hide", "gaccesses"},
             "command", "Hides graphics accesses",
             &RetroShell::exec <Token::dmadebugger, Token::hide, Token::gaccesses>);

    root.add({"dmadebugger", "hide", "paccesses"},
             "command", "Hides sprite pointer accesses",
             &RetroShell::exec <Token::dmadebugger, Token::hide, Token::paccesses>);

    root.add({"dmadebugger", "hide", "saccesses"},
             "command", "Hides sprite accesses",
             &RetroShell::exec <Token::dmadebugger, Token::hide, Token::saccesses>);

    /*
     root.add({"dmadebugger", "inspect"},
     "command", "Displays the internal state",
     &RetroShell::exec <Token::dmadebugger, Token::inspect>);
     */
    
    
    //
    // Monitor
    //

    root.add({"monitor"},
             "component", "C64 monitor");

    root.add({"monitor", "set"},
             "command", "Configures the component");

    root.add({"monitor", "set", "palette"},
             "key", "Selects the color palette",
             &RetroShell::exec <Token::monitor, Token::set, Token::palette>, 1);

    root.add({"monitor", "set", "brightness"},
             "key", "Adjusts the monitor brightness",
             &RetroShell::exec <Token::monitor, Token::set, Token::brightness>, 1);

    root.add({"monitor", "set", "contrast"},
             "key", "Adjusts the monitor contrast",
             &RetroShell::exec <Token::monitor, Token::set, Token::contrast>, 1);

    root.add({"monitor", "set", "saturation"},
             "key", "Adjusts the color saturation",
             &RetroShell::exec <Token::monitor, Token::set, Token::saturation>, 1);

    
    //
    // SID
    //

    root.add({"sid"},
             "component", "Sound Interface Device");

    root.add({"sid", "config"},
             "command", "Displays the current configuration",
             &RetroShell::exec <Token::sid, Token::config>);

    root.add({"sid", "set"},
             "command", "Configures the component");

    root.add({"sid", "set", "engine"},
             "key", "Selects the SID backend",
             &RetroShell::exec <Token::sid, Token::set, Token::engine>, 1);

    root.add({"sid", "set", "revision"},
             "key", "Selects the emulated chip model",
             &RetroShell::exec <Token::sid, Token::set, Token::revision>, 1);

    root.add({"sid", "set", "sampling"},
             "key", "Selects the sampling method",
             &RetroShell::exec <Token::sid, Token::set, Token::sampling>, 1);

    root.add({"sid", "set", "filter"},
             "key", "Configures the audio filter",
             &RetroShell::exec <Token::sid, Token::set, Token::filter>, 1);

    root.add({"sid", "set", "volume"},
             "key", "Sets the volume");

    root.add({"sid", "set", "volume", "channel0"},
             "key", "Sets the volume for the first SID",
             &RetroShell::exec <Token::sid, Token::set, Token::volume>, 1, 0);

    root.add({"sid", "set", "volume", "channel1"},
             "key", "Sets the volume for the second SID",
             &RetroShell::exec <Token::sid, Token::set, Token::volume>, 1, 1);
    
    root.add({"sid", "set", "volume", "channel2"},
             "key", "Sets the volume for the third SID",
             &RetroShell::exec <Token::sid, Token::set, Token::volume>, 1, 2);
    
    root.add({"sid", "set", "volume", "channel3"},
             "key", "Sets the volume for the fourth SID",
             &RetroShell::exec <Token::sid, Token::set, Token::volume>, 1, 3);

    root.add({"sid", "set", "volume", "left"},
             "key", "Sets the master volume for the left speaker",
             &RetroShell::exec <Token::sid, Token::set, Token::volume>, 1, 4);
    
    root.add({"sid", "set", "volume", "right"},
             "key", "Sets the master volume for the right speaker",
             &RetroShell::exec <Token::sid, Token::set, Token::volume>, 1, 5);

    root.add({"sid", "set", "pan"},
             "key", "Sets the pan for one of the four SIDs");
    
    root.add({"sid", "set", "pan", "channel0"},
             "key", "Sets the pan for the first SID",
             &RetroShell::exec <Token::sid, Token::set, Token::pan>, 1, 0);
    
    root.add({"sid", "set", "pan", "channel1"},
             "key", "Sets the pan for the second SID",
             &RetroShell::exec <Token::sid, Token::set, Token::pan>, 1, 1);
    
    root.add({"sid", "set", "pan", "channel2"},
             "key", "Sets the pan for the third SID",
             &RetroShell::exec <Token::sid, Token::set, Token::pan>, 1, 2);
    
    root.add({"sid", "set", "pan", "channel3"},
             "key", "Sets the pan for the fourth SID",
             &RetroShell::exec <Token::sid, Token::set, Token::pan>, 1, 3);
    
    root.add({"sid", "inspect"},
             "command", "Displays the internal state");

    root.add({"sid", "inspect", "state"},
             "category", "Displays the current state of a single SID instance",
             &RetroShell::exec <Token::sid, Token::inspect, Token::state>, 1);

    root.add({"sid", "inspect", "registers"},
             "category", "Displays the registers of a single SID instance",
             &RetroShell::exec <Token::sid, Token::inspect, Token::registers>, 1);
    
    
    //
    // Control port
    //
    
    root.add({"controlport1"},
             "component", "Control port 1");
    
    root.add({"controlport2"},
             "component", "Control port 2");

    for (isize i = 0; i < 2; i++) {

        string port = (i == 0) ? "controlport1" : "controlport2";

        root.add({port, "inspect"},
                 "command", "Displays the internal state",
                 &RetroShell::exec <Token::controlport, Token::inspect>);
    }
    
    
    //
    // Expansion port
    //
    
    root.add({"expansion"},
             "component", "Expansion port");
    
    root.add({"expansion", "inspect"},
             "command", "Displays the internal state",
             &RetroShell::exec <Token::expansion, Token::inspect>);

    root.add({"expansion", "attach"},
             "command", "Attaches a cartridge",
             &RetroShell::exec <Token::expansion, Token::attach>, 1);

    
    //
    // Keyboard
    //

    root.add({"keyboard"},
             "component", "Keyboard");

    root.add({"keyboard", "inspect"},
             "command", "Displays the internal state",
             &RetroShell::exec <Token::keyboard, Token::inspect>);

    root.add({"keyboard", "type"},
             "command", "Types a command",
             &RetroShell::exec <Token::keyboard, Token::type>, 1);

    root.add({"keyboard", "type", "load"},
             "command", "Types LOAD \"*\",8,1",
             &RetroShell::exec <Token::keyboard, Token::type, Token::load>);

    root.add({"keyboard", "type", "run"},
             "command", "Types RUN",
             &RetroShell::exec <Token::keyboard, Token::type, Token::run>);

    root.add({"keyboard", "press"},
             "command", "Presses a key",
             &RetroShell::exec <Token::keyboard, Token::press>, 1);

    root.add({"keyboard", "press", "shiftlock"},
             "command", "Presses the shift lock key",
             &RetroShell::exec <Token::keyboard, Token::press, Token::shiftlock>);

    root.add({"keyboard", "release"},
             "command", "Presses a key",
             &RetroShell::exec <Token::keyboard, Token::release>, 1);

    root.add({"keyboard", "release", "shiftlock"},
             "command", "Releases the shift lock key",
             &RetroShell::exec <Token::keyboard, Token::release, Token::shiftlock>);

    
    //
    // Joystick
    //

    root.add({"joystick"},
             "component", "Joystick");

    root.add({"joystick", "config"},
             "command", "Displays the current configuration",
             &RetroShell::exec <Token::joystick, Token::config>);

    root.add({"joystick", "inspect"},
             "command", "Displays the internal state",
             &RetroShell::exec <Token::joystick, Token::inspect>);
    
    root.add({"joystick", "set"},
             "command", "Configures the component");

    root.add({"joystick", "set", "autofire"},
             "key", "Enables or disables auto fire mode",
             &RetroShell::exec <Token::joystick, Token::set, Token::autofire>, 1);

    root.add({"joystick", "set", "bullets"},
             "key", "Sets the number of bullets per auto fire shot",
             &RetroShell::exec <Token::joystick, Token::set, Token::bullets>, 1);

    root.add({"joystick", "set", "delay"},
             "key", "Sets the auto fire delay in frames",
             &RetroShell::exec <Token::joystick, Token::set, Token::delay>, 1);

    
    //
    // Mouse
    //

    root.add({"mouse"},
             "component", "mouse");

    root.add({"mouse", "config"},
             "command", "Displays the current configuration",
             &RetroShell::exec <Token::mouse, Token::config>);

    root.add({"mouse", "inspect"},
             "command", "Displays the internal state",
             &RetroShell::exec <Token::mouse, Token::inspect>);
    
    root.add({"mouse", "set"},
             "command", "Configures the component");

    root.add({"mouse", "set", "model"},
             "key", "Selects the mouse model",
             &RetroShell::exec <Token::mouse, Token::set, Token::model>, 1);

    root.add({"mouse", "set", "velocity"},
             "key", "Sets the horizontal and vertical mouse velocity",
             &RetroShell::exec <Token::mouse, Token::set, Token::velocity>, 1);

    root.add({"mouse", "set", "shakedetector"},
             "key", "Enables or disables the shake detector",
             &RetroShell::exec <Token::mouse, Token::set, Token::shakedetector>, 1);

    
    //
    // Parallel cable
    //
    
    root.add({"parcable"},
             "component", "Parallel drive cable");

    root.add({"parcable", "config"},
             "command", "Displays the current configuration",
             &RetroShell::exec <Token::parcable, Token::config>);

    root.add({"parcable", "inspect"},
             "command", "Displays the internal state",
             &RetroShell::exec <Token::parcable, Token::inspect>);
}

}
