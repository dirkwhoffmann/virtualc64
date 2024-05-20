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
#include "Interpreter.h"
#include "Emulator.h"
#include "Option.h"

namespace vc64 {

void
Interpreter::initCommons(Command &root)
{
    //
    // Common commands
    //

    root.pushGroup("Shell commands");

    root.add({"."},
             "Enter or exit the debugger",
             [this](Arguments& argv, long value) {

        switchInterpreter();
    });

    root.add({"clear"},
             "Clear the console window",
             [this](Arguments& argv, long value) {

        retroShell.clear();
    });

    root.add({"close"},
             "Hide the console window",
             [this](Arguments& argv, long value) {

        msgQueue.put(MSG_CONSOLE_CLOSE);
    });

    root.add({"help"}, { }, {Arg::command},
             "Print usage information",
             [this](Arguments& argv, long value) {

        retroShell.help(argv.empty() ? "" : argv.front());
    });

    root.add({"joshua"},
             "",
             [this](Arguments& argv, long value) {

        retroShell << "\nGREETINGS PROFESSOR HOFFMANN.\n";
        retroShell << "THE ONLY WINNING MOVE IS NOT TO PLAY.\n";
        retroShell << "HOW ABOUT A NICE GAME OF CHESS?\n\n";
    });

    root.add({"source"}, {Arg::path},
             "Process a command script",
             [this](Arguments& argv, long value) {

        auto stream = std::ifstream(argv.front());
        if (!stream.is_open()) throw VC64Error(ERROR_FILE_NOT_FOUND, argv.front());
        retroShell.execScript(stream);
    });

    root.add({"wait"}, {Arg::value, Arg::seconds},
             "", // "Pause the execution of a command script",
             [this](Arguments& argv, long value) {

        auto cycles = parseNum(argv[0]) * vic.getFrequency();
        c64.scheduleRel<SLOT_RSH>(cycles, RSH_WAKEUP);
        throw ScriptInterruption("");
    });
}

void
Interpreter::initCommandShell(Command &root)
{
    //
    // Shared commands
    //

    initCommons(root);


    //
    // Regression tester
    //

    root.pushGroup("Regression tester");
    root.pushGroup("");

    root.add({"regression"}, ""); // Run the regression tester

    root.add({"regression", "setup"}, { C64ModelEnum::argList() },
             "Initialize the test environment",
             [this](Arguments& argv, long value) {

        auto model = parseEnum <C64Model, C64ModelEnum> (argv[0]);
        regressionTester.prepare(c64, model);

        // Pause the script to give the C64 some time to boot
        c64.scheduleRel<SLOT_RSH>(3 * vic.getFrequency(), RSH_WAKEUP);
        throw ScriptInterruption("");
    });

    root.add({"screenshot"}, ""); // Take a screenshot and exit
    root.add({"regression", "run"}, { Arg::path },
             "Launch a regression test",
             [this](Arguments& argv, long value) {

        regressionTester.run(argv.front());
    });

    root.add({"screenshot", "set"},
             "Configure the screenshot");

    root.add({"screenshot", "set", "filename"}, { Arg::path },
             "Assign the screen shot filename",
             [this](Arguments& argv, long value) {

        regressionTester.dumpTexturePath = argv.front();
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

    root.popGroup();


    //
    // Components
    //

    root.pushGroup("Components");

    //
    // Components (C64)
    //

    auto cmd = c64.shellName();
    auto description = c64.description();
    root.add({cmd}, description);

    root.pushGroup("");

    root.add({cmd, ""},
             "Display the current configuration",
             [this](Arguments& argv, long value) {

        retroShell.dump(emulator, Category::Config);
    });

    root.add({cmd, "set"}, "Configure the component");
    for (auto &opt : emulator.getOptions()) {

        root.add({cmd, "set", OptionEnum::plainkey(opt)},
                 {OptionParser::create(opt)->argList()},
                 OptionEnum::help(opt),
                 [this](Arguments& argv, long opt) {

            emulator.set(Option(opt), argv[0]);

        }, opt);
    }

    root.add({cmd, "defaults"},
             "Display the user defaults storage",
             [this](Arguments& argv, long value) {

        retroShell.dump(emulator, Category::Defaults);
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
        retroShell << ss << '\n';

        c64.exportConfig("/tmp/test.ini");
    });
    
    root.popGroup();


    //
    // Components (Memory)
    //

    cmd = mem.shellName();
    description = mem.description();
    root.add({cmd}, description);

    root.pushGroup("");

    root.add({cmd, ""},
             "Displays the current configuration",
             [this](Arguments& argv, long value) {

        retroShell.dump(mem, Category::Config);
    });

    root.add({cmd, "set"}, "Configures the component");
    for (auto &opt : mem.getOptions()) {

        root.add({cmd, "set", OptionEnum::plainkey(opt)},
                 {OptionParser::create(opt)->argList()},
                 OptionEnum::help(opt),
                 [this](Arguments& argv, long opt) {

            emulator.set(Option(opt), argv[0]);

        }, opt);
    }

    root.add({cmd, "load"}, { Arg::path },
             "Installs a Rom image",
             [this](Arguments& argv, long value) {

        c64.loadRom(argv.front());
    });

    root.add({cmd, "flash"}, { Arg::path },
             "Flashes a file into memory",
             [this](Arguments& argv, long value) {

        auto path = argv.front();
        if (!util::fileExists(path)) throw VC64Error(ERROR_FILE_NOT_FOUND, path);

        auto file = PRGFile(path);
        c64.flash(file, 0);
    });

    root.popGroup();

    //
    // Components (CIA)
    //

    for (isize i = 0; i < 2; i++) {

        cmd = (i == 0) ? cia1.shellName() : cia2.shellName();
        description = (i == 0) ? cia1.description() : cia2.description();
        root.add({cmd}, description);

        root.pushGroup("");

        root.add({cmd, ""},
                 "Displays the current configuration",
                 [this](Arguments& argv, long value) {

            value == 0 ?
            retroShell.dump(cia1, Category::Config) :
            retroShell.dump(cia2, Category::Config) ;

        }, i);

        root.add({cmd, "set"}, "Configures the component");

        for (auto &opt : cia1.getOptions()) {

            root.add({cmd, "set", OptionEnum::plainkey(opt)},
                     {OptionParser::create(opt)->argList()},
                     OptionEnum::help(opt),
                     [this](Arguments& argv, long value) {

                emulator.set(Option(HI_WORD(value)), LO_WORD(value), argv[0]);

            }, HI_W_LO_W(opt, i));
        }

        root.popGroup();
    }

    //
    // Components (VICII)
    //

    cmd = vic.shellName();
    description = vic.description();
    root.add({cmd}, description);

    root.pushGroup("");

    root.add({cmd, ""},
             "Displays the current configuration",
             [this](Arguments& argv, long value) {

        retroShell.dump(vic, Category::Config);
    });

    root.add({cmd, "set"}, "Configures the component");
    for (auto &opt : vic.getOptions()) {

        root.add({cmd, "set", OptionEnum::plainkey(opt)},
                 {OptionParser::create(opt)->argList()},
                 OptionEnum::help(opt),
                 [this](Arguments& argv, long opt) {

            emulator.set(Option(opt), argv[0]);

        }, opt);
    }

    root.popGroup();


    //
    // Components (DMA Debugger)
    //

    cmd = vic.dmaDebugger.shellName();
    description = vic.dmaDebugger.description();
    root.add({cmd}, description);

    root.pushGroup("");

    root.add({cmd, ""},
             "Displays the current configuration",
             [this](Arguments& argv, long value) {

        retroShell.dump(vic.dmaDebugger, Category::Config);
    });

    root.add({cmd, "open"},
             "Opens the DMA debugger",
             [this](Arguments& argv, long value) {

        configure(OPT_DMA_DEBUG_ENABLE, true);
    });

    root.add({cmd, "close"},
             "Closes the DMA debugger",
             [this](Arguments& argv, long value) {

        configure(OPT_DMA_DEBUG_ENABLE, false);
    });

    root.add({cmd, "set"}, "Configures the component");

    for (auto &opt : vic.dmaDebugger.getOptions()) {

        root.add({cmd, "set", OptionEnum::plainkey(opt)},
                 {OptionParser::create(opt)->argList()},
                 OptionEnum::help(opt),
                 [this](Arguments& argv, long opt) {

            emulator.set(Option(opt), argv[0]);

        }, opt);
    }
    
    root.popGroup();


    //
    // Components (SID)
    //

    for (isize i = 0; i < 4; i++) {

        auto &sid = sidBridge.sid[i];

        cmd = sid.shellName();
        description = sid.description();
        root.add({cmd}, description);

        root.pushGroup("");
        root.add({cmd, ""},
                 "Displays the current configuration",
                 [this](Arguments& argv, long value) {

            retroShell.dump(sidBridge.sid[value], Category::Config);
        }, i);

        root.add({cmd, "set"}, "Configures the component");

        for (auto &opt : sidBridge.sid[i].getOptions()) {

            root.add({cmd, "set", OptionEnum::plainkey(opt)},
                     {OptionParser::create(opt)->argList()},
                     OptionEnum::help(opt),
                     [this](Arguments& argv, long value) {

                emulator.set(Option(HI_WORD(value)), LO_WORD(value), argv[0]);

            }, HI_W_LO_W(opt, i));
        }

        root.popGroup();
    }


    //
    // Components (SIDBridge)
    //

    cmd = sidBridge.shellName();
    description = sidBridge.description();
    root.add({cmd}, description);

    root.pushGroup("");

    root.add({cmd, ""},
             "Displays the current configuration",
             [this](Arguments& argv, long value) {

        retroShell.dump(sidBridge, Category::Config);
    });

    root.add({cmd, "set"}, "Configures the component");

    for (auto &opt : sidBridge.getOptions()) {

        root.add({cmd, "set", OptionEnum::plainkey(opt)},
                 {OptionParser::create(opt)->argList()},
                 OptionEnum::help(opt),
                 [this](Arguments& argv, long opt) {

            emulator.set(Option(opt), argv[0]);

        }, opt);
    }

    root.popGroup();


    //
    // Components (Expansion port)
    //

    cmd = expansionport.shellName();
    description = expansionport.description();
    root.add({cmd}, description);

    root.pushGroup("");

    root.add({cmd, "attach"},
             "Attaches a cartridge");

    root.add({cmd, "attach", "cartridge"}, { Arg::path },
             "Attaches a cartridge from a CRT file",
             [this](Arguments& argv, long value) {

        auto path = argv.front();
        if (!util::fileExists(path)) throw VC64Error(ERROR_FILE_NOT_FOUND, path);
        expansionport.attachCartridge(path);
    });

    root.add({cmd, "attach", "reu"}, { "<KB>" },
             "Attaches a REU expansion cartridge",
             [this](Arguments& argv, long value) {

        expansionport.attachReu(parseNum(argv[0]));
    });

    root.add({cmd, "attach", "georam"}, { "<KB>" },
             "Attaches a GeoRAM expansion cartridge",
             [this](Arguments& argv, long value) {

        expansionport.attachGeoRam(parseNum(argv[0]));
    });

    root.popGroup();


    //
    // Components (Power supply)
    //

    cmd = powerSupply.shellName();
    description = powerSupply.description();
    root.add({cmd}, description);

    root.pushGroup("");

    root.add({cmd, ""},
             "Displays the current configuration",
             [this](Arguments& argv, long value) {

        retroShell.dump(powerSupply, Category::Config);
    });

    root.add({cmd, "set"}, "Configures the component");

    for (auto &opt : powerSupply.getOptions()) {

        root.add({cmd, "set", OptionEnum::plainkey(opt)},
                 {OptionParser::create(opt)->argList()},
                 OptionEnum::help(opt),
                 [this](Arguments& argv, long opt) {

            emulator.set(Option(opt), argv[0]);

        }, opt);
    }

    root.popGroup();


    //
    // Components (Host)
    //

    cmd = host.shellName();
    description = host.description();
    root.add({cmd}, description);

    root.pushGroup("");

    root.add({cmd, ""},
             "Displays the current configuration",
             [this](Arguments& argv, long value) {

        retroShell.dump(host, Category::Config);
    });

    root.add({cmd, "set"}, "Configures the component");

    for (auto &opt : powerSupply.getOptions()) {

        root.add({cmd, "set", OptionEnum::plainkey(opt)},
                 {OptionParser::create(opt)->argList()},
                 OptionEnum::help(opt),
                 [this](Arguments& argv, long opt) {

            emulator.set(Option(opt), argv[0]);

        }, opt);
    }
    
    root.popGroup();


    //
    // Peripherals
    //

    root.pushGroup("Peripherals");


    //
    // Peripherals (Monitor)
    //

    cmd = monitor.shellName();
    description = monitor.description();
    root.add({cmd}, description);

    root.pushGroup("");

    root.add({cmd, ""},
             "Displays the current configuration",
             [this](Arguments& argv, long value) {

        retroShell.dump(monitor, Category::Config);

    });

    root.add({cmd, "set"}, "Configures the component");

    for (auto &opt : c64.monitor.getOptions()) {

        root.add({cmd, "set", OptionEnum::plainkey(opt)},
                 {OptionParser::create(opt)->argList()},
                 OptionEnum::help(opt),
                 [this](Arguments& argv, long value) {

            emulator.set(Option(value), argv[0]);

        }, opt);
    }

    root.popGroup();


    //
    // Peripherals (Keyboard)
    //

    cmd = keyboard.shellName();
    description = keyboard.description();
    root.add({cmd}, description);

    root.pushGroup("");

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

    root.popGroup();


    //
    // Peripherals (Mouse)
    //

    for (isize i = PORT_1; i <= PORT_2; i++) {

        auto &mouse = i == PORT_1 ? c64.port1.mouse : c64.port2.mouse;

        cmd = mouse.shellName();
        description = mouse.description();
        root.add({cmd}, description);

        root.pushGroup("");

        root.add({cmd, ""},
                 "Displays the current configuration",
                 [this](Arguments& argv, long value) {

            auto &port = (value == PORT_1) ? c64.port1 : c64.port2;
            retroShell.dump(port.mouse, Category::Config);

        }, i);

        root.add({cmd, "set"}, "Configures the component");

        for (auto &opt : c64.port1.mouse.getOptions()) {

            root.add({cmd, "set", OptionEnum::plainkey(opt)},
                     {OptionParser::create(opt)->argList()},
                     OptionEnum::help(opt),
                     [this](Arguments& argv, long value) {

                emulator.set(Option(HI_WORD(value)), LO_WORD(value), argv[0]);

            }, HI_W_LO_W(opt, i));
        }

        root.popGroup();
    }

    //
    // Peripherals (Joystick)
    //

    for (isize i = PORT_1; i <= PORT_2; i++) {

        auto &joystick = i == PORT_1 ? c64.port1.joystick : c64.port2.joystick;

        cmd =joystick.shellName();
        description = joystick.description();
        root.add({cmd}, description);

        root.pushGroup("");

        root.add({cmd, ""},
                 "Displays the current configuration",
                 [this](Arguments& argv, long value) {

            auto &port = (value == PORT_1) ? c64.port1 : c64.port2;
            retroShell.dump(port.joystick, Category::Config);

        }, i);

        root.add({cmd, "set"}, "Configures the component");

        for (auto &opt : c64.port1.joystick.getOptions()) {

            root.add({cmd, "set", OptionEnum::plainkey(opt)},
                     {OptionParser::create(opt)->argList()},
                     OptionEnum::help(opt),
                     [this](Arguments& argv, long value) {

                emulator.set(Option(HI_WORD(value)), LO_WORD(value), argv[0]);

            }, HI_W_LO_W(opt, i));
        }

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

        root.popGroup();
    }

    //
    // Peripherals (Datasette)
    //

    cmd = datasette.shellName();
    description = datasette.description();
    root.add({cmd}, description);

    root.pushGroup("");

    root.add({cmd, ""},
             "Displays the current configuration",
             [this](Arguments& argv, long value) {

        retroShell.dump(datasette, Category::Config);
    });

    root.add({cmd, "connect"},
             "Connects the datasette",
             [this](Arguments& argv, long value) {

        configure(OPT_DAT_CONNECT, true);
    });

    root.add({cmd, "disconnect"},
             "Disconnects the datasette",
             [this](Arguments& argv, long value) {

        configure(OPT_DAT_CONNECT, false);
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

    root.add({cmd, "set"}, "Configures the component");

    for (auto &opt : datasette.getOptions()) {

        root.add({cmd, "set", OptionEnum::plainkey(opt)},
                 {OptionParser::create(opt)->argList()},
                 OptionEnum::help(opt),
                 [this](Arguments& argv, long opt) {

            emulator.set(Option(opt), argv[0]);

        }, opt);
    }

    root.popGroup();


    //
    // Peripherals (Drives)
    //

    for (isize i = 0; i < 2; i++) {

        auto &drive = i == 0 ? c64.drive8 : c64.drive9;

        cmd = drive.shellName();
        description = drive.description();
        root.add({cmd}, description);

        root.pushGroup("");

        root.add({cmd, ""},
                 "Displays the current configuration",
                 [this](Arguments& argv, long value) {

            auto &drive = value ? drive9 : drive8;
            retroShell.dump(drive, Category::Config);

        }, i);

        root.add({cmd, "connect"},
                 "Connects the drive",
                 [this](Arguments& argv, long value) {

            auto id = value ? DRIVE9 : DRIVE8;
            configure(OPT_DRV_CONNECT, id, true);

        }, i);

        root.add({cmd, "disconnect"},
                 "Disconnects the drive",
                 [this](Arguments& argv, long value) {

            auto id = value ? DRIVE9 : DRIVE8;
            configure(OPT_DRV_CONNECT, id, false);

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
            if (!util::fileExists(path)) throw VC64Error(ERROR_FILE_NOT_FOUND, path);

            auto &drive = value ? drive9 : drive8;
            drive.insertDisk(path, false);

        }, i);

        root.add({cmd, "newdisk"}, { DOSTypeEnum::argList() },
                 "Inserts a new blank disk",
                 [this](Arguments& argv, long value) {

            auto type = util::parseEnum <DOSType, DOSTypeEnum> (argv.front());
            auto &drive = value ? drive9 : drive8;
            drive.insertNewDisk(type, PETName<16>("NEW DISK"));

        }, i);

        root.add({cmd, "set"}, "Configures the component");

        for (auto &opt : drive8.getOptions()) {

            root.add({cmd, "set", OptionEnum::plainkey(opt)},
                     {OptionParser::create(opt)->argList()},
                     OptionEnum::help(opt),
                     [this](Arguments& argv, long value) {

                emulator.set(Option(HI_WORD(value)), LO_WORD(value), argv[0]);

            }, HI_W_LO_W(opt, i));
        }

        root.popGroup();
    }

    //
    // Peripherals (Parallel cable)
    //

    cmd = parCable.shellName();
    description = parCable.description();
    root.add({cmd}, description);

    root.pushGroup("");

    root.add({cmd, ""},
             "Displays the current configuration",
             [this](Arguments& argv, long value) {

        retroShell.dump(parCable, Category::Config);
    });

    root.popGroup();

    //
    // Miscellaneous
    //

    root.pushGroup("Miscellaneous");

    root.add({"recorder"},       "Screen recorder");
    root.pushGroup("");

    root.add({"recorder", ""},
             "Displays the current configuration",
             [this](Arguments& argv, long value) {

        retroShell.dump(recorder, Category::Config);
    });

    root.add({"recorder", "set"}, "Configures the component");

    for (auto &opt : recorder.getOptions()) {

        root.add({"recorder", "set", OptionEnum::plainkey(opt)},
                 {OptionParser::create(opt)->argList()},
                 OptionEnum::help(opt),
                 [this](Arguments& argv, long value) {

            emulator.set(Option(value), argv[0]);

        }, opt);
    }

    root.popGroup();
    root.popGroup();
}

}
