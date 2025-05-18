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
#include "Console.h"
#include "Emulator.h"
#include "Option.h"

namespace vc64 {

void
CommandConsole::_enter()
{
    msgQueue.put(MSG_RSH_DEBUGGER, false);

    // If the console is entered the first time...
    if (isEmpty()) {

        // Print the welcome message
        exec("welcome");
        *this << getPrompt();
    }
}

void
CommandConsole::_pause()
{

}

string
CommandConsole::getPrompt()
{
    return "C64% ";
}

void
CommandConsole::welcome()
{
    storage << "VirtualC64 RetroShell ";
    remoteManager.rshServer << "VirtualC64 RetroShell Remote Server ";
    *this << C64::build() << '\n';
    *this << '\n';
    *this << "Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de" << '\n';
    *this << "https://github.com/dirkwhoffmann/virtualc64" << '\n';
    *this << '\n';

    printHelp();
    *this << '\n';
}

void
CommandConsole::printHelp()
{
    storage << "Type 'help' or press 'TAB' twice for help.\n";
    storage << "Type '.' or press 'SHIFT+RETURN' to enter debug mode.";

    remoteManager.rshServer << "Type 'help' for help.\n";
    remoteManager.rshServer << "Type '.' to enter debug mode.";

    *this << '\n';
}

void
CommandConsole::pressReturn(bool shift)
{
    if (!shift && input.empty()) {

        printHelp();

    } else {

        Console::pressReturn(shift);
    }
}

void
CommandConsole::initCommands(RetroShellCmd &root)
{
    Console::initCommands(root);

    //
    // Workspace management
    //
    
    root.add({
        
        .tokens = { "workspace" },
        .hidden = releaseBuild,
        .help   = { "Workspace management" }
    });
    
    root.add({
        
        .tokens = { "workspace init" },
        .hidden = releaseBuild,
        .help   = { "First command of a workspace script" },
        .func   = [] (Arguments& argv, const std::vector<isize> &values) {
            
            // c64.initWorkspace();
        }
    });

    root.add({
        
        .tokens = { "workspace activate" },
        .hidden = releaseBuild,
        .help   = { "Last command of a workspace script" },
        .func   = [] (Arguments& argv, const std::vector<isize> &values) {
            
            // c64.activateWorkspace();
        }
    });

    
    //
    // Regression tester
    //

    RetroShellCmd::currentGroup = "Regression testing";

    auto cmd = registerComponent(regressionTester);
    
    root.add({
        
        .tokens = { "regression" },
        .hidden = releaseBuild,
        .help   = { "Runs the regression tester" }
    });
    
    root.add({
        
        .tokens = { "regression", "setup" },
        .args   = { C64ModelEnum::argList() },
        .extra  = { Arg::path, Arg::path },
        .help   = { "Initializes the test environment" },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            auto model = parseEnum <C64Model, C64ModelEnum> (argv[0]);
            regressionTester.prepare(c64, model);

            // Pause the script to give the C64 some time to boot
            c64.scheduleRel<SLOT_RSH>(3 * vic.getFrequency(), RSH_WAKEUP);
            throw ScriptInterruption("");
        }
    });
    
    /*
    root.add({"regression", "setup"}, { C64ModelEnum::argList() },
             "Initialize the test environment",
             [this](Arguments& argv, const std::vector<isize> &values) {

        auto model = parseEnum <C64Model, C64ModelEnum> (argv[0]);
        regressionTester.prepare(c64, model);

        // Pause the script to give the C64 some time to boot
        c64.scheduleRel<SLOT_RSH>(3 * vic.getFrequency(), RSH_WAKEUP);
        throw ScriptInterruption("");
    });
    */
    
    root.add({
        
        .tokens = { "regression", "run" },
        .args   = { Arg::path },
        .help   = { "Launches a regression test" },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            regressionTester.run(argv.front());
        }
    });

    /*
    root.add({"regression", "run"}, { Arg::path },
             "Launch a regression test",
             [this](Arguments& argv, const std::vector<isize> &values) {

        regressionTester.run(argv.front());
    });
    */
    
    root.add({
        
        .tokens = { "screenshot" },
        .hidden = releaseBuild,
        .help   = { "Manages screenshots" }
    });
    
    root.add({
        
        .tokens = { "screenshot", "set" },
        .help   = { "Configure the screenshot" }
    });
    
    root.add({
        
        .tokens = { "screenshot", "set", "path" },
        .args   = { Arg::path },
        .help   = { "Assign the save directory" },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            regressionTester.screenshotPath = argv.front() == "\"\"" ? "" : argv.front();
        }
    });
    
    root.add({
        
        .tokens = { "screenshot", "set", "cutout" },
        .args   = { Arg::value, Arg::value, Arg::value, Arg::value },
        .help   = { "Adjust the texture cutout" },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            isize x1 = parseNum(argv[0]);
            isize y1 = parseNum(argv[1]);
            isize x2 = parseNum(argv[2]);
            isize y2 = parseNum(argv[3]);
            
            regressionTester.x1 = x1;
            regressionTester.y1 = y1;
            regressionTester.x2 = x2;
            regressionTester.y2 = y2;
        }
    });
    
    root.add({
        
        .tokens = { "screenshot", "save" },
        .args   = { Arg::path },
        .help   = { "Saves a screenshot and exits the emulator" },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            regressionTester.dumpTexture(c64, argv.front());
        }
    });
    
    /*
    root.add({"screenshot"}, debugBuild ? "Manages screenshots" : "");

    root.add({"screenshot", "set"},
             "Configure the screenshot");

    root.add({"screenshot", "set", "path"}, { Arg::path },
             "Assign the save directory",
             [this](Arguments& argv, const std::vector<isize> &values) {

        regressionTester.screenshotPath = argv.front() == "\"\"" ? "" : argv.front();
    });

    root.add({"screenshot", "set", "cutout"}, { Arg::value, Arg::value, Arg::value, Arg::value },
             "Adjust the texture cutout",
             [this](Arguments& argv, const std::vector<isize> &values) {

        isize x1 = parseNum(argv[0]);
        isize y1 = parseNum(argv[1]);
        isize x2 = parseNum(argv[2]);
        isize y2 = parseNum(argv[3]);

        regressionTester.x1 = x1;
        regressionTester.y1 = y1;
        regressionTester.x2 = x2;
        regressionTester.y2 = y2;
    });

    root.add({"screenshot", "save"}, { Arg::path },
             "Save a screenshot and exits the emulator",
             [this](Arguments& argv, const std::vector<isize> &values) {

        regressionTester.dumpTexture(c64, argv.front());
    });
    */

    //
    // Components
    //

    RetroShellCmd::currentGroup = "Components";

    //
    // Components (C64)
    //

    cmd = registerComponent(c64);

    root.add({
        
        .tokens = { cmd, "defaults" },
        .help   = { "Displays the user defaults storage" },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            dump(emulator, Category::Defaults);
        }
    });
        
    root.add({
        
        .tokens = { cmd, "power" },
        .args   = { Arg::onoff },
        .help   = { "Switches the C64 on or off" },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            parseOnOff(argv[0]) ? emulator.run() : emulator.powerOff();
        }
    });
       
    root.add({
        
        .tokens = { cmd, "reset" },
        .help   = { "Performs a hard reset" },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {

            c64.hardReset();
        }
    });
    
    root.add({
        
        .tokens = { cmd, "init" },
        .args   = { C64ModelEnum::argList() },
        .help   = { "Initializes the C64 with a predefined scheme" },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            emulator.set(parseEnum<C64Model, C64ModelEnum>(argv[0]));
        }
    });
    
    //
    // Components (Memory)
    //

    cmd = registerComponent(mem);
    
    root.add({
        
        .tokens = { cmd, "flash" },
        .args   = { Arg::path },
        .help   = { "Flashes a file into memory" },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            auto path = argv.front();
            if (!util::fileExists(path)) throw Error(VC64ERROR_FILE_NOT_FOUND, path);

            auto file = PRGFile(path);
            c64.flash(file, 0);
        }
    });

    root.add({
        
        .tokens = { cmd, "load" },
        .help   = { "Loads memory contents from a file" }
    });

    root.add({
        
        .tokens = { cmd, "load", "rom" },
        .args   = { Arg::path },
        .help   = { "Loads a Rom image" },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            c64.loadRom(argv.front());
        }
    });
    
    root.add({
        
        .tokens = { cmd, "load", "ram" },
        .args   = { Arg::path, Arg::address },
        .help   = { "Loads a chunk of RAM" },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            fs::path path(argv[0]);
            mem.debugger.load(path, parseAddr(argv[1]));
        }
    });

    root.add({
        
        .tokens = { cmd, "load", "openroms" },
        .args   = { Arg::path, Arg::address },
        .help   = { "Installs the MEGA65 OpenROMs" },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            c64.installOpenRoms();
        }
    });
    
    root.add({
        
        .tokens = { cmd, "save" },
        .help   = { "Save memory contents to a file" }
    });
    
    root.add({
        
        .tokens = { cmd, "save", "ram" },
        .args   = { Arg::path, Arg::address, Arg::count },
        .help   = { "Saves a chunk of RAM" },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            fs::path path(argv[0]);
            mem.debugger.save(path, parseAddr(argv[1]), parseNum(argv[2]));
        }
    });
    
    //
    // Components (CPU)
    //
    
    cmd = registerComponent(cpu);
    
    
    //
    // Components (CIA)
    //

    cmd = registerComponent(cia1);
    cmd = registerComponent(cia2);


    //
    // Components (VICII)
    //

    cmd = registerComponent(vic);


    //
    // Components (DMA Debugger)
    //

    cmd = registerComponent(vic.dmaDebugger);

    /*
    root.add({cmd, "open"},
             "Opens the DMA debugger",
             [this](Arguments& argv, const std::vector<isize> &values) {

        emulator.set(OPT_DMA_DEBUG_ENABLE, true);
    });

    root.add({cmd, "close"},
             "Closes the DMA debugger",
             [this](Arguments& argv, const std::vector<isize> &values) {

        emulator.set(OPT_DMA_DEBUG_ENABLE, false);
    });
    */
    

    //
    // Components (SID)
    //

    cmd = registerComponent(sid0);
    cmd = registerComponent(sid1);
    cmd = registerComponent(sid2);
    cmd = registerComponent(sid3);


    //
    // Ports
    //

    RetroShellCmd::currentGroup = "Ports";


    //
    // Ports (Power port)
    //

    cmd = registerComponent(powerSupply);


    //
    // Ports (Audio port)
    //

    cmd = registerComponent(audioPort);


    //
    // Ports (User port)
    //

    cmd = registerComponent(userPort);


    //
    // Ports (Video port)
    //

    cmd = registerComponent(videoPort);


    //
    // Ports (Expansion port)
    //

    cmd = registerComponent(expansionPort);

    root.add({
        
        .tokens = { cmd, "attach" },
        .help   = { "Attaches a cartridge" }
    });

    root.add({
        
        .tokens = { cmd, "attach", "cartridge" },
        .args   = { Arg::path },
        .help   = { "Attaches a cartridge from a CRT file" },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            auto path = argv.front();
            if (!util::fileExists(path)) throw Error(VC64ERROR_FILE_NOT_FOUND, path);
            expansionPort.attachCartridge(path);
        }
    });

    root.add({
        
        .tokens = { cmd, "attach", "reu" },
        .args   = { "<KB>" },
        .help   = { "Attaches a REU expansion cartridge" },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            expansionPort.attachReu(parseNum(argv[0]));
        }
    });

    root.add({
        
        .tokens = { cmd, "attach", "georam" },
        .args   = { "<KB>" },
        .help   = { "Attaches a GeoRAM expansion cartridge" },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            expansionPort.attachGeoRam(parseNum(argv[0]));
        }
    });
    

    //
    // Peripherals
    //

    RetroShellCmd::currentGroup = "Peripherals";


    //
    // Peripherals (Monitor)
    //

    registerComponent(monitor);


    //
    // Peripherals (Keyboard)
    //

    cmd = registerComponent(keyboard);

    root.add({
        
        .tokens = { cmd, "press" },
        .args   = { Arg::value },
        .help   = { "Presses a key" },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            keyboard.press(C64Key(parseNum(argv[0])));
        }
    });

    root.add({
        
        .tokens = { cmd, "release" },
        .args   = { Arg::value },
        .help   = { "Releases a key" },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            keyboard.release(C64Key(parseNum(argv[0])));
        }
    });
    
    root.add({
        
        .tokens = { cmd, "type" },
        .help   = { "Types text on the keyboard" }
    });
    
    root.add({
        
        .tokens = { cmd, "type", "text" },
        .args   = { Arg::string },
        .help   = { "Types text on the keyboard" },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            keyboard.autoType(argv.front());
        }
    });
    
    root.add({
        
        .tokens = { cmd, "type", "load" },
        .help   = { "Types \"LOAD\"*\",8,1" },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            keyboard.autoType("load \"*\",8,1\n");
        }
    });

    root.add({
        
        .tokens = { cmd, "type", "run" },
        .help   = { "Types RUN" },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            keyboard.autoType("run\n");
        }
    });


    //
    // Peripherals (Mouse)
    //

    registerComponent(c64.port1.mouse);
    registerComponent(c64.port2.mouse);


    //
    // Peripherals (Joystick)
    //

    for (isize i = 0; i <= 1; i++) {
        
        if (i == 0) cmd = registerComponent(port1.joystick);
        if (i == 1) cmd = registerComponent(port2.joystick);
        
        root.add({
            
            .tokens = { cmd, "press" },
            .help   = { "Presses a joystick button" },
            .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
                
                auto &port = (values.front() == PORT_1) ? c64.port1 : c64.port2;
                port.joystick.trigger(PRESS_FIRE);
                
            }, .values = {i}
        });
        
        root.add({
            
            .tokens = { cmd, "unpress" },
            .help   = { "Releases a joystick button" },
            .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
                
                auto &port = (values.front() == PORT_1) ? c64.port1 : c64.port2;
                port.joystick.trigger(RELEASE_FIRE);
                
            }, .values = {i}
        });
        
        root.add({
            
            .tokens = { cmd, "pull" },
            .help   = { "Pulls the joystick" }
        });
        
        root.add({
            
            .tokens = { cmd, "pull", "left" },
            .help   = { "Pulls the joystick left" },
            .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
                
                auto &port = (values.front() == PORT_1) ? c64.port1 : c64.port2;
                port.joystick.trigger(PULL_LEFT);
                
            }, .values = {i}
        });
        
        root.add({
            
            .tokens = { cmd, "pull", "right" },
            .help   = { "Pulls the joystick right" },
            .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
                
                auto &port = (values.front() == PORT_1) ? c64.port1 : c64.port2;
                port.joystick.trigger(PULL_RIGHT);
                
            }, .values = {i}
        });
        
        root.add({
            
            .tokens = { cmd, "pull", "up" },
            .help   = { "Pulls the joystick up" },
            .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
                
                auto &port = (values.front() == PORT_1) ? c64.port1 : c64.port2;
                port.joystick.trigger(PULL_UP);
                
            }, .values = {i}
        });
        
        root.add({
            
            .tokens = { cmd, "pull", "down" },
            .help   = { "Pulls the joystick down" },
            .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
                
                auto &port = (values.front() == PORT_1) ? c64.port1 : c64.port2;
                port.joystick.trigger(PULL_DOWN);
                
            }, .values = {i}
        });
        
        root.add({
            
            .tokens = { cmd, "release" },
            .help   = { "Release a joystick axis" }
        });
        
        root.add({
            
            .tokens = { cmd, "release", "x" },
            .help   = { "Releases the x-axis" },
            .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
                
                auto &port = (values.front() == PORT_1) ? c64.port1 : c64.port2;
                port.joystick.trigger(RELEASE_X);
                
            }, .values = {i}
        });
        
        root.add({
            
            .tokens = { cmd, "release", "y" },
            .help   = { "Releases the y-axis" },
            .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
                
                auto &port = (values.front() == PORT_1) ? c64.port1 : c64.port2;
                port.joystick.trigger(RELEASE_Y);
                
            }, .values = {i}
        });
    }
    

    //
    // Peripherals (Paddles)
    //

    cmd = registerComponent(port1.paddle);
    cmd = registerComponent(port2.paddle);


    //
    // Peripherals (Datasette)
    //

    cmd = registerComponent(datasette);

    root.add({
        
        .tokens = { cmd, "connect" },
        .help   = { "Connects the datasette" },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            emulator.set(OPT_DAT_CONNECT, true);
        }
    });

    root.add({
        
        .tokens = { cmd, "disconnect" },
        .help   = { "Disconnects the datasette" },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            emulator.set(OPT_DAT_CONNECT, false);
        }
    });
    
    root.add({
        
        .tokens = { cmd, "rewind" },
        .help   = { "Rewinds the tape" },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            datasette.rewind();
        }
    });

    root.add({
        
        .tokens = { cmd, "rewind", "to" },
        .args   = { Arg::value },
        .help   = { "Rewinds the tape to a specific position" },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            datasette.rewind(parseNum(argv[0]));
        }
    });
    

    //
    // Peripherals (Drives)
    //

    for (isize i = 0; i < 2; i++) {
        
        if (i == 0) cmd = registerComponent(drive8);
        if (i == 1) cmd = registerComponent(drive9);
        
        root.add({
            
            .tokens = { cmd, "bankmap" },
            .help   = { "Displays the memory layout" },
            .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
                
                datasette.rewind(parseNum(argv[0]));
                
            }, .values = {i}
        });
        
        root.add({
            
            .tokens = { cmd, "connect" },
            .help   = { "Connects the drive" },
            .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
                
                auto id = values.front() ? DRIVE9 : DRIVE8;
                emulator.set(OPT_DRV_CONNECT, true, { id });
                
            }, .values = {i}
        });
        
        root.add({
            
            .tokens = { cmd, "disconnect" },
            .help   = { "Disconnects the drive" },
            .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
                
                auto id = values.front() ? DRIVE9 : DRIVE8;
                emulator.set(OPT_DRV_CONNECT, false, { id });
                
            }, .values = {i}
        });
        
        root.add({
            
            .tokens = { cmd, "eject" },
            .help   = { "Ejects a floppy disk" },
            .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
                
                auto &drive = values.front() ? drive9 : drive8;
                drive.ejectDisk();
                
            }, .values = {i}
        });
        
        root.add({
            
            .tokens = { cmd, "insert" },
            .args   = { Arg::path },
            .help   = { "Inserts a floppy disk" },
            .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
                
                auto path = argv.front();
                if (!util::fileExists(path)) throw Error(VC64ERROR_FILE_NOT_FOUND, path);
                
                auto &drive = values.front() ? drive9 : drive8;
                drive.insertDisk(path, false);
                
            }, .values = {i}
        });
        
        root.add({
            
            .tokens = { cmd, "newdisk" },
            .args   = { DOSTypeEnum::argList() },
            .help   = { "Inserts a new blank disk" },
            .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
                
                auto type = util::parseEnum <DOSType, DOSTypeEnum> (argv.front());
                auto &drive = values.front() ? drive9 : drive8;
                drive.insertNewDisk(type, "NEW DISK");
                
            }, .values = {i}
        });
    }


    //
    // Peripherals (RS232)
    //

    cmd = registerComponent(userPort.rs232);

    root.add({
        
        .tokens = { cmd, "send" },
        .args   = { Arg::string },
        .help   = { "Feeds text into the RS232 adapter" },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            userPort.rs232 << argv[0];
        },
    });
    

    //
    // Miscellaneous
    //

    RetroShellCmd::currentGroup = "Miscellaneous";

    //
    // Miscellaneous (Host)
    //

    cmd = registerComponent(host);

    
    //
    // Miscellaneous (Config)
    //
    
    root.add({
        
        .tokens = { "config" },
        .help   = { "Virtual machine configuration" }
    });
    
    root.add({
        
        .tokens = { "config", "" },
        .help   = { "Displays the current configuration" },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            std::stringstream ss;
            c64.exportConfig(ss);
            *this << ss;
        },
    });
    
    root.add({
        
        .tokens = { "config", "diff" },
        .help   = { "Displays the difference to the default configuration" },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            std::stringstream ss;
            c64.exportDiff(ss);
            *this << ss;
        }
    });
    

    //
    // Miscellaneous (Remote server)
    //

    root.add({
        
        .tokens = { "server" },
        .help   = { "Remote connections" }
    });
    
    root.add({
        
        .tokens = { "server", "" },
        .help   = { "Displays a server status summary" },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            dump(remoteManager, Category::State);
        }
    });
    
    cmd = registerComponent(remoteManager.rshServer, root / "server");

    root.add({
        
        .tokens = { "server", cmd, "start" },
        .help   = { "Starts the retro shell server" },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            remoteManager.rshServer.start();
        }
    });
    
    root.add({
        
        .tokens = { "server", cmd, "stop" },
        .help   = { "Stops the retro shell server" },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            remoteManager.rshServer.stop();
        }
    });

    root.add({
        
        .tokens = { "server", cmd, "disconnect" },
        .help   = { "Disconnects a client" },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            remoteManager.rshServer.disconnect();
        }
    });
    

    //
    // Miscellaneous (Recorder)
    //

    cmd = registerComponent(recorder);
}

}
