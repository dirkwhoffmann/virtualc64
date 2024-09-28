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
    return "vc64% ";
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

        retroShell.asyncExec("printhelp");

    } else {

        Console::pressReturn(shift);
    }
}

void
CommandConsole::initCommands(Command &root)
{
    Console::initCommands(root);

    //
    // Regression tester
    //

    Command::currentGroup = "Regression testing";

    auto cmd = registerComponent(regressionTester);
    root.seek("regression")->hidden = releaseBuild;

    // root.add({"regression"}, debugBuild ? "Runs the regression tester" : "");

    root.add({"regression", "setup"}, { C64ModelEnum::argList() },
             "Initialize the test environment",
             [this](Arguments& argv, long value) {

        auto model = parseEnum <C64Model, C64ModelEnum> (argv[0]);
        regressionTester.prepare(c64, model);

        // Pause the script to give the C64 some time to boot
        c64.scheduleRel<SLOT_RSH>(3 * vic.getFrequency(), RSH_WAKEUP);
        throw ScriptInterruption("");
    });

    root.add({"regression", "run"}, { Arg::path },
             "Launch a regression test",
             [this](Arguments& argv, long value) {

        regressionTester.run(argv.front());
    });

    root.add({"screenshot"}, debugBuild ? "Manages screenshots" : "");

    root.add({"screenshot", "set"},
             "Configure the screenshot");

    root.add({"screenshot", "set", "path"}, { Arg::path },
             "Assign the save directory",
             [this](Arguments& argv, long value) {

        regressionTester.screenshotPath = argv.front() == "\"\"" ? "" : argv.front();
    });

    root.add({"screenshot", "set", "cutout"}, { Arg::value, Arg::value, Arg::value, Arg::value },
             "Adjust the texture cutout",
             [this](Arguments& argv, long value) {

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
             [this](Arguments& argv, long value) {

        regressionTester.dumpTexture(c64, argv.front());
    });


    //
    // Components
    //

    Command::currentGroup = "Components";

    //
    // Components (C64)
    //

    cmd = registerComponent(c64);

    root.add({cmd, "defaults"},
             "Display the user defaults storage",
             [this](Arguments& argv, long value) {

        dump(emulator, Category::Defaults);
    });

    root.add({cmd, "power"}, { Arg::onoff },
             "Switch the C64 on or off",
             [this](Arguments& argv, long value) {

        parseOnOff(argv[0]) ? c64.emulator.run() : c64.emulator.powerOff();
    });

    root.add({cmd, "reset"},
             "Perform a hard reset",
             [this](Arguments& argv, long value) {

        c64.hardReset();
    });

    root.add({cmd, "init"}, { C64ModelEnum::argList() },
             "Initialize the emulator with factory defaults",
             [this](Arguments& argv, long value) {

        emulator.set(parseEnum<C64Model, C64ModelEnum>(argv[0]));
    });

    root.add({cmd, "diff"},
             "Reports all differences to the default configuration",
             [this](Arguments& argv, long value) {

        std::stringstream ss;
        c64.exportDiff(ss);
        printf("ss = %s\n", ss.str().c_str());
        retroShell << ss << '\n';
    });


    //
    // Components (Memory)
    //

    cmd = registerComponent(mem);

    root.add({cmd, "flash"}, { Arg::path },
             "Flash a file into memory",
             [this](Arguments& argv, long value) {

        auto path = argv.front();
        if (!util::fileExists(path)) throw Error(VC64ERROR_FILE_NOT_FOUND, path);

        auto file = PRGFile(path);
        c64.flash(file, 0);
    });

    root.add({cmd, "load"},
             "Load memory contents from a file");

    root.add({cmd, "load", "rom"}, { Arg::path },
             "Load a Rom image from disk",
             [this](Arguments& argv, long value) {

        c64.loadRom(argv.front());
    });

    root.add({cmd, "load", "ram"}, { Arg::path, Arg::address },
             "Load a chunk of RAM",
             [this](Arguments& argv, long value) {

        fs::path path(argv[0]);
        mem.debugger.load(path, parseAddr(argv[1]));
    });

    root.add({cmd, "load", "openroms"},
             "Install MEGA65 OpenROMs",
             [this](Arguments& argv, long value) {

        c64.installOpenRoms();
    });

    root.add({cmd, "save"},
             "Save memory contents to a file");

    root.add({cmd, "save", "ram"}, { Arg::path, Arg::address, Arg::count },
             "Save a chunk of RAM",
             [this](Arguments& argv, long value) {

        fs::path path(argv[0]);
        mem.debugger.save(path, parseAddr(argv[1]), parseNum(argv[2]));
    });


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

    root.add({cmd, "open"},
             "Opens the DMA debugger",
             [this](Arguments& argv, long value) {

        emulator.set(OPT_DMA_DEBUG_ENABLE, true);
    });

    root.add({cmd, "close"},
             "Closes the DMA debugger",
             [this](Arguments& argv, long value) {

        emulator.set(OPT_DMA_DEBUG_ENABLE, false);
    });


    //
    // Components (SID)
    //

    cmd = registerComponent(sid0);
    cmd = registerComponent(sid1);
    cmd = registerComponent(sid2);
    cmd = registerComponent(sid3);


    //
    // Components (SIDBridge)
    //

    // cmd = registerComponent(sidBridge);


    //
    // Ports
    //

    Command::currentGroup = "Ports";


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

    root.add({cmd, "attach"},
             "Attaches a cartridge");

    root.add({cmd, "attach", "cartridge"}, { Arg::path },
             "Attaches a cartridge from a CRT file",
             [this](Arguments& argv, long value) {

        auto path = argv.front();
        if (!util::fileExists(path)) throw Error(VC64ERROR_FILE_NOT_FOUND, path);
        expansionPort.attachCartridge(path);
    });

    root.add({cmd, "attach", "reu"}, { "<KB>" },
             "Attaches a REU expansion cartridge",
             [this](Arguments& argv, long value) {

        expansionPort.attachReu(parseNum(argv[0]));
    });

    root.add({cmd, "attach", "georam"}, { "<KB>" },
             "Attaches a GeoRAM expansion cartridge",
             [this](Arguments& argv, long value) {

        expansionPort.attachGeoRam(parseNum(argv[0]));
    });


    //
    // Peripherals
    //

    Command::currentGroup = "Peripherals";


    //
    // Peripherals (Monitor)
    //

    registerComponent(monitor);


    //
    // Peripherals (Keyboard)
    //

    cmd = registerComponent(keyboard);

    root.add({cmd, "press"}, { Arg::value },
             "Presses a key",
             [this](Arguments& argv, long value) {

        keyboard.press(C64Key(parseNum(argv[0])));
    });

    root.add({cmd, "release"}, { Arg::value },
             "Presses a key",
             [this](Arguments& argv, long value) {

        keyboard.release(C64Key(parseNum(argv[0])));
    });

    root.add({cmd, "type"},
             "Types text on the keyboard");

    root.add({cmd, "type", "text"}, { Arg::string },
             "Types arbitrary text",
             [this](Arguments& argv, long value) {

        keyboard.autoType(argv.front());
    });

    root.add({cmd, "type", "load"},
             "Types \"LOAD\"*\",8,1",
             [this](Arguments& argv, long value) {

        keyboard.autoType("load \"*\",8,1\n");
    });

    root.add({cmd, "type", "run"},
             "Types RUN",
             [this](Arguments& argv, long value) {

        keyboard.autoType("run\n");
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

        root.add({cmd, "press"},
                 "Presses the joystick button",
                 [this](Arguments& argv, long value) {

            auto &port = (value == PORT_1) ? c64.port1 : c64.port2;
            port.joystick.trigger(PRESS_FIRE);

        }, i);

        root.add({cmd, "unpress"},
                 "Releases a joystick button",
                 [this](Arguments& argv, long value) {

            auto &port = (value == PORT_1) ? c64.port1 : c64.port2;
            port.joystick.trigger(RELEASE_FIRE);

        }, i);

        root.add({cmd, "pull"},
                 "Pulls the joystick");

        root.add({cmd, "pull", "left"},
                 "Pulls the joystick left",
                 [this](Arguments& argv, long value) {

            auto &port = (value == PORT_1) ? c64.port1 : c64.port2;
            port.joystick.trigger(PULL_LEFT);

        }, i);

        root.add({cmd, "pull", "right"},
                 "Pulls the joystick right",
                 [this](Arguments& argv, long value) {

            auto &port = (value == PORT_1) ? c64.port1 : c64.port2;
            port.joystick.trigger(PULL_RIGHT);

        }, i);

        root.add({cmd, "pull", "up"},
                 "Pulls the joystick up",
                 [this](Arguments& argv, long value) {

            auto &port = (value == PORT_1) ? c64.port1 : c64.port2;
            port.joystick.trigger(PULL_UP);

        }, i);

        root.add({cmd, "pull", "down"},
                 "Pulls the joystick down",
                 [this](Arguments& argv, long value) {

            auto &port = (value == PORT_1) ? c64.port1 : c64.port2;
            port.joystick.trigger(PULL_DOWN);

        }, i);

        root.add({cmd, "release"},
                 "Release a joystick axis");

        root.add({cmd, "release", "x"},
                 "Releases the x-axis",
                 [this](Arguments& argv, long value) {

            auto &port = (value == PORT_1) ? c64.port1 : c64.port2;
            port.joystick.trigger(RELEASE_X);

        }, i);

        root.add({cmd, "release", "y"},
                 "Releases the y-axis",
                 [this](Arguments& argv, long value) {

            auto &port = (value == PORT_1) ? c64.port1 : c64.port2;
            port.joystick.trigger(RELEASE_Y);

        }, i);
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

    root.add({cmd, "connect"},
             "Connects the datasette",
             [this](Arguments& argv, long value) {

        emulator.set(OPT_DAT_CONNECT, true);
    });

    root.add({cmd, "disconnect"},
             "Disconnects the datasette",
             [this](Arguments& argv, long value) {

        emulator.set(OPT_DAT_CONNECT, false);
    });

    root.add({cmd, "rewind"},
             "Rewinds the tape",
             [this](Arguments& argv, long value) {

        datasette.rewind();
    });

    root.add({cmd, "rewind", "to"}, { Arg::value },
             "Rewinds the tape to a specific position",
             [this](Arguments& argv, long value) {

        datasette.rewind(parseNum(argv[0]));
    });


    //
    // Peripherals (Drives)
    //

    for (isize i = 0; i < 2; i++) {

        if (i == 0) cmd = registerComponent(drive8);
        if (i == 1) cmd = registerComponent(drive9);

        root.add({cmd, "bankmap"},
                 "Displays the memory layout",
                 [this](Arguments& argv, long value) {

            auto &drive = value ? drive9 : drive8;
            dump(drive, Category::BankMap);

        }, i);

        root.add({cmd, "connect"},
                 "Connects the drive",
                 [this](Arguments& argv, long value) {

            auto id = value ? DRIVE9 : DRIVE8;
            emulator.set(OPT_DRV_CONNECT, true, { id });

        }, i);

        root.add({cmd, "disconnect"},
                 "Disconnects the drive",
                 [this](Arguments& argv, long value) {

            auto id = value ? DRIVE9 : DRIVE8;
            emulator.set(OPT_DRV_CONNECT, false, { id });

        }, i);

        root.add({cmd, "eject"},
                 "Ejects a floppy disk",
                 [this](Arguments& argv, long value) {

            auto &drive = value ? drive9 : drive8;
            drive.ejectDisk();

        }, i);

        root.add({cmd, "insert"}, { Arg::path },
                 "Inserts a floppy disk",
                 [this](Arguments& argv, long value) {

            auto path = argv.front();
            if (!util::fileExists(path)) throw Error(VC64ERROR_FILE_NOT_FOUND, path);

            auto &drive = value ? drive9 : drive8;
            drive.insertDisk(path, false);

        }, i);

        root.add({cmd, "newdisk"}, { DOSTypeEnum::argList() },
                 "Inserts a new blank disk",
                 [this](Arguments& argv, long value) {

            auto type = util::parseEnum <DOSType, DOSTypeEnum> (argv.front());
            auto &drive = value ? drive9 : drive8;
            drive.insertNewDisk(type, "NEW DISK");

        }, i);
    }


    //
    // Peripherals (RS232)
    //

    cmd = registerComponent(userPort.rs232);

    root.add({cmd, "send"}, {Arg::string},
             "Feeds text into the RS232 adapter",
             [this](Arguments& argv, long value) {

        userPort.rs232 << argv[0];
    });


    //
    // Miscellaneous
    //

    Command::currentGroup = "Miscellaneous";

    //
    // Miscellaneous (Host)
    //

    cmd = registerComponent(host);


    //
    // Miscellaneous (Remote server)
    //

    root.add({"server"}, "Remote connections");

    root.add({"server", ""},
             "Displays a server status summary",
             [this](Arguments& argv, long value) {

        dump(remoteManager, Category::State);
    });

    cmd = registerComponent(remoteManager.rshServer, root / "server");

    root.add({"server", cmd, "start"},
             "Starts the retro shell server",
             [this](Arguments& argv, long value) {

        remoteManager.rshServer.start();
    });

    root.add({"server", cmd, "stop"},
             "Stops the retro shell server",
             [this](Arguments& argv, long value) {

        remoteManager.rshServer.stop();
    });

    root.add({"server", cmd, "disconnect"},
             "Disconnects a client",
             [this](Arguments& argv, long value) {

        remoteManager.rshServer.disconnect();
    });


    //
    // Miscellaneous (Recorder)
    //

    cmd = registerComponent(recorder);
}

}
