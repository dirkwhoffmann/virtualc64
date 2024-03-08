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

        auto model = parseEnum <C64ModelEnum> (argv[0]);
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

        isize x1 = parseNum(argv[0], 0);
        isize y1 = parseNum(argv[0], 1);
        isize x2 = parseNum(argv[0], 2);
        isize y2 = parseNum(argv[0], 3);

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

    root.pushGroup("Components");

    //
    // Components (Emulator)
    //

    root.add({"c64"}, "Commodore 64");

    root.add({"c64", ""},
             "Display the current configuration",
             [this](Arguments& argv, long value) {

        retroShell.dump(emulator, Category::Config);
    });

    root.add({"c64", "set"}, "Configure the component");
    for (auto &opt : emulator.getOptions()) {

        root.add({"c64", "set", OptionEnum::key(opt)},
                 {OptionParser::create(opt)->argList()},
                 OptionEnum::help(opt),
                 [this](Arguments& argv, long opt) {

            emulator.set(Option(opt), argv[0]);

        }, opt);
    }

    root.add({"c64", "defaults"},
             "Display the user defaults storage",
             [this](Arguments& argv, long value) {

        retroShell.dump(emulator, Category::Defaults);
    });

    root.add({"c64", "power"}, { Arg::onoff },
             "Switch the C64 on or off",
             [this](Arguments& argv, long value) {

        parseOnOff(argv[0]) ? c64.emulator.run() : c64.emulator.powerOff();
    });

    root.add({"c64", "reset"},
             "Perform a hard reset",
             [this](Arguments& argv, long value) {

        c64.hardReset();
    });

    root.add({"c64", "init"}, { C64ModelEnum::argList() },
             "Initialize the emulator with factory defaults",
             [this](Arguments& argv, long value) {

        emulator.set(parseEnum<C64ModelEnum>(argv[0]));
    });

    root.add({"c64", "diff"},
             "Reports all differences to the default configuration",
             [this](Arguments& argv, long value) {

        std::stringstream ss;
        c64.dumpDiff(ss);
        retroShell << ss << '\n';
    });


    //
    // Components (Memory)
    //

    root.add({"memory"}, "Ram and Rom");

    root.add({"memory", ""},
             "Displays the current configuration",
             [this](Arguments& argv, long value) {

        retroShell.dump(mem, Category::Config);
    });

    root.add({"memory", "set"}, "Configures the component");
    for (auto &opt : mem.getOptions()) {

        root.add({"memory", "set", OptionEnum::key(opt)},
                 {OptionParser::create(opt)->argList()},
                 OptionEnum::help(opt),
                 [this](Arguments& argv, long opt) {

            emulator.set(Option(opt), argv[0]);

        }, opt);
    }

    root.add({"memory", "load"}, { Arg::path },
             "Installs a Rom image",
             [this](Arguments& argv, long value) {

        c64.loadRom(argv.front());
    });

    root.add({"memory", "flash"}, { Arg::path },
             "Flashes a file into memory",
             [this](Arguments& argv, long value) {

        auto path = argv.front();
        if (!util::fileExists(path)) throw VC64Error(ERROR_FILE_NOT_FOUND, path);

        auto file = PRGFile(path);
        c64.flash(file, 0);
    });


    //
    // Components (CIA)
    //

    for (isize i = 0; i < 2; i++) {

        string cia = (i == 0) ? "cia1" : "cia2";

        root.add({cia}, "Complex Interface Adapter " + std::to_string(i));

        root.add({cia, ""},
                 "Displays the current configuration",
                 [this](Arguments& argv, long value) {

            value == 0 ?
            retroShell.dump(cia1, Category::Config) :
            retroShell.dump(cia2, Category::Config) ;

        }, i);

        root.add({cia, "set"}, "Configures the component");

        for (auto &opt : cia1.getOptions()) {

            root.add({cia, "set", OptionEnum::key(opt)},
                     {OptionParser::create(opt)->argList()},
                     OptionEnum::help(opt),
                     [this](Arguments& argv, long value) {

                emulator.set(Option(HI_WORD(value)), LO_WORD(value), argv[0]);

            }, HI_W_LO_W(opt, i));
        }
    }

    //
    // Components (VICII)
    //

    root.add({"vicii"}, "Video Interface Controller");

    root.add({"vicii", ""},
             "Displays the current configuration",
             [this](Arguments& argv, long value) {

        retroShell.dump(vic, Category::Config);
    });

    root.add({"vicii", "set"}, "Configures the component");
    for (auto &opt : vic.getOptions()) {

        root.add({"vicii", "set", OptionEnum::plainkey(opt)},
                 {OptionParser::create(opt)->argList()},
                 OptionEnum::help(opt),
                 [this](Arguments& argv, long opt) {

            emulator.set(Option(opt), argv[0]);

        }, opt);
    }


    //
    // Components (DMA Debugger)
    //

    root.add({"dmadebugger"},   "DMA Debugger");

    root.add({"dmadebugger", ""},
             "Displays the current configuration",
             [this](Arguments& argv, long value) {

        retroShell.dump(vic.dmaDebugger, Category::Config);
    });

    root.add({"dmadebugger", "open"},
             "Opens the DMA debugger",
             [this](Arguments& argv, long value) {

        configure(OPT_DMA_DEBUG_ENABLE, true);
    });

    root.add({"dmadebugger", "close"},
             "Closes the DMA debugger",
             [this](Arguments& argv, long value) {

        configure(OPT_DMA_DEBUG_ENABLE, false);
    });

    root.add({"dmadebugger", "set"}, "Configures the component");

    for (auto &opt : vic.dmaDebugger.getOptions()) {

        root.add({"dmadebugger", "set", OptionEnum::key(opt)},
                 {OptionParser::create(opt)->argList()},
                 OptionEnum::help(opt),
                 [this](Arguments& argv, long opt) {

            emulator.set(Option(opt), argv[0]);

        }, opt);
    }

    //
    // Components (SID)
    //

    root.add({"sid"}, "Sound Interface Device");

    for (isize i = 0; i < 4; i++) {

        string nr = std::to_string(i);

        root.add({"sid", nr},
                 "SID " + nr + (i == 0 ? " (primary SID)" : ""));

        root.add({"sid", nr, ""},
                 "Displays the current configuration",
                 [this](Arguments& argv, long value) {

            retroShell.dump(muxer.sid[value], Category::Config);
        }, i);

        root.add({"sid", nr, "set"}, "Configures the component");

        for (auto &opt : muxer.sid[i].getOptions()) {

            root.add({"sid", nr, "set", OptionEnum::key(opt)},
                     {OptionParser::create(opt)->argList()},
                     OptionEnum::help(opt),
                     [this](Arguments& argv, long value) {

                emulator.set(Option(HI_WORD(value)), LO_WORD(value), argv[0]);

            }, HI_W_LO_W(opt, i));
        }
    }

    //
    // Components (Muxer)
    //

    root.add({"muxer"}, "Audio backend");

    root.add({"muxer", ""},
             "Displays the current configuration",
             [this](Arguments& argv, long value) {

        retroShell.dump(muxer, Category::Config);
    });

    root.add({"muxer", "set"}, "Configures the component");

    for (auto &opt : muxer.getOptions()) {

        root.add({"muxer", "set", OptionEnum::key(opt)},
                 {OptionParser::create(opt)->argList()},
                 OptionEnum::help(opt),
                 [this](Arguments& argv, long opt) {

            emulator.set(Option(opt), argv[0]);

        }, opt);
    }

    //
    // Components (Expansion port)
    //

    root.add({"expansion"}, "Expansion port");

    root.add({"expansion", "attach"},
             "Attaches a cartridge");

    root.add({"expansion", "attach", "cartridge"}, { Arg::path },
             "Attaches a cartridge from a CRT file",
             [this](Arguments& argv, long value) {

        auto path = argv.front();
        if (!util::fileExists(path)) throw VC64Error(ERROR_FILE_NOT_FOUND, path);
        expansionport.attachCartridge(path);
    });

    root.add({"expansion", "attach", "reu"}, { "<KB>" },
             "Attaches a REU expansion cartridge",
             [this](Arguments& argv, long value) {

        expansionport.attachReu(parseNum(argv[0]));
    });

    root.add({"expansion", "attach", "georam"}, { "<KB>" },
             "Attaches a GeoRAM expansion cartridge",
             [this](Arguments& argv, long value) {

        expansionport.attachGeoRam(parseNum(argv[0]));
    });


    //
    // Components (Power supply)
    //

    root.add({"powersupply"},   "Power supply");

    root.add({"powersupply", ""},
             "Displays the current configuration",
             [this](Arguments& argv, long value) {

        retroShell.dump(powerSupply, Category::Config);
    });

    root.add({"powersupply", "set"}, "Configures the component");

    for (auto &opt : powerSupply.getOptions()) {

        root.add({"powersupply", "set", OptionEnum::key(opt)},
                 {OptionParser::create(opt)->argList()},
                 OptionEnum::help(opt),
                 [this](Arguments& argv, long opt) {

            emulator.set(Option(opt), argv[0]);

        }, opt);
    }


    //
    // Components (Host)
    //

    root.add({"host"},          "Host computer");

    root.add({"host", ""},
             "Displays the current configuration",
             [this](Arguments& argv, long value) {

        retroShell.dump(host, Category::Config);
    });

    root.add({"host", "set"}, "Configures the component");

    for (auto &opt : powerSupply.getOptions()) {

        root.add({"host", "set", OptionEnum::key(opt)},
                 {OptionParser::create(opt)->argList()},
                 OptionEnum::help(opt),
                 [this](Arguments& argv, long opt) {

            emulator.set(Option(opt), argv[0]);

        }, opt);
    }


    //
    // Peripherals
    //

    root.pushGroup("Peripherals");

    root.add({"monitor"},       "C64 monitor");

    //
    // Peripherals (Keyboard)
    //
    root.add({"keyboard"},      "Keyboard");

    root.add({"keyboard", "press"}, { Arg::value },
             "Presses a key",
             [this](Arguments& argv, long value) {

        keyboard.press(C64Key(parseNum(argv[0])));
    });

    root.add({"keyboard", "release"}, { Arg::value },
             "Presses a key",
             [this](Arguments& argv, long value) {

        keyboard.release(C64Key(parseNum(argv[0])));
    });

    root.add({"keyboard", "type"},
             "Types text on the keyboard");

    root.add({"keyboard", "type", "text"}, { Arg::string },
             "Types arbitrary text",
             [this](Arguments& argv, long value) {

        keyboard.autoType(argv.front());
    });

    root.add({"keyboard", "type", "load"},
             "Types \"LOAD\"*\",8,1",
             [this](Arguments& argv, long value) {

        keyboard.autoType("load \"*\",8,1\n");
    });

    root.add({"keyboard", "type", "run"},
             "Types RUN",
             [this](Arguments& argv, long value) {

        keyboard.autoType("run\n");
    });

    //
    // Peripherals (Mouse)
    //

    root.pushGroup("");
    root.add({"mouse"}, "Mouse");

    for (isize i = PORT_1; i <= PORT_2; i++) {

        string nr = (i == 1) ? "1" : "2";

        root.add({"mouse", nr},
                 "Mouse in port " + nr);

        root.add({"mouse", nr, ""},
                 "Displays the current configuration",
                 [this](Arguments& argv, long value) {

            auto &port = (value == PORT_1) ? c64.port1 : c64.port2;
            retroShell.dump(port.mouse, Category::Config);

        }, i);

        root.add({"mouse", nr, "set"}, "Configures the component");

        for (auto &opt : c64.port1.mouse.getOptions()) {

            root.add({"mouse", nr, "set", OptionEnum::key(opt)},
                     {OptionParser::create(opt)->argList()},
                     OptionEnum::help(opt),
                     [this](Arguments& argv, long value) {

                emulator.set(Option(HI_WORD(value)), LO_WORD(value), argv[0]);

            }, HI_W_LO_W(opt, i));
        }
    }


    //
    // Peripherals (Joystick)
    //

    root.add({"joystick"}, "Joystick");
    root.pushGroup("");

    for (isize i = PORT_1; i <= PORT_2; i++) {

        string nr = (i == 1) ? "1" : "2";

        root.add({"joystick", nr},
                 "Joystick in port " + nr);

        root.add({"joystick", nr, ""},
                 "Displays the current configuration",
                 [this](Arguments& argv, long value) {

            auto &port = (value == PORT_1) ? c64.port1 : c64.port2;
            retroShell.dump(port.joystick, Category::Config);

        }, i);

        root.add({"joystick", nr, "set"}, "Configures the component");

        for (auto &opt : c64.port1.joystick.getOptions()) {

            root.add({"joystick", nr, "set", OptionEnum::key(opt)},
                     {OptionParser::create(opt)->argList()},
                     OptionEnum::help(opt),
                     [this](Arguments& argv, long value) {

                emulator.set(Option(HI_WORD(value)), LO_WORD(value), argv[0]);

            }, HI_W_LO_W(opt, i));
        }

        root.add({"joystick", nr, "press"},
                 "Presses the joystick button",
                 [this](Arguments& argv, long value) {

            auto &port = (value == PORT_1) ? c64.port1 : c64.port2;
            port.joystick.trigger(PRESS_FIRE);

        }, i);

        root.add({"joystick", nr, "unpress"},
                 "Releases a joystick button",
                 [this](Arguments& argv, long value) {

            auto &port = (value == PORT_1) ? c64.port1 : c64.port2;
            port.joystick.trigger(RELEASE_FIRE);

        }, i);

        root.add({"joystick", nr, "pull"},
                 "Pulls the joystick");

        root.add({"joystick", nr, "pull", "left"},
                 "Pulls the joystick left",
                 [this](Arguments& argv, long value) {

            auto &port = (value == PORT_1) ? c64.port1 : c64.port2;
            port.joystick.trigger(PULL_LEFT);

        }, i);

        root.add({"joystick", nr, "pull", "right"},
                 "Pulls the joystick right",
                 [this](Arguments& argv, long value) {

            auto &port = (value == PORT_1) ? c64.port1 : c64.port2;
            port.joystick.trigger(PULL_RIGHT);

        }, i);

        root.add({"joystick", nr, "pull", "up"},
                 "Pulls the joystick up",
                 [this](Arguments& argv, long value) {

            auto &port = (value == PORT_1) ? c64.port1 : c64.port2;
            port.joystick.trigger(PULL_UP);

        }, i);

        root.add({"joystick", nr, "pull", "down"},
                 "Pulls the joystick down",
                 [this](Arguments& argv, long value) {

            auto &port = (value == PORT_1) ? c64.port1 : c64.port2;
            port.joystick.trigger(PULL_DOWN);

        }, i);

        root.add({"joystick", nr, "release"},
                 "Release a joystick axis");

        root.add({"joystick", nr, "release", "x"},
                 "Releases the x-axis",
                 [this](Arguments& argv, long value) {

            auto &port = (value == PORT_1) ? c64.port1 : c64.port2;
            port.joystick.trigger(RELEASE_X);

        }, i);

        root.add({"joystick", nr, "release", "y"},
                 "Releases the y-axis",
                 [this](Arguments& argv, long value) {

            auto &port = (value == PORT_1) ? c64.port1 : c64.port2;
            port.joystick.trigger(RELEASE_Y);

        }, i);
    }

    //
    // Peripherals (Datasette)
    //

    root.add({"datasette"}, "Commodore tape drive");

    root.add({"datasette", ""},
             "Displays the current configuration",
             [this](Arguments& argv, long value) {

        retroShell.dump(datasette, Category::Config);
    });

    root.add({"datasette", "connect"},
             "Connects the datasette",
             [this](Arguments& argv, long value) {

        configure(OPT_DAT_CONNECT, true);
    });

    root.add({"datasette", "disconnect"},
             "Disconnects the datasette",
             [this](Arguments& argv, long value) {

        configure(OPT_DAT_CONNECT, false);
    });

    root.add({"datasette", "rewind"},
             "Rewinds the tape",
             [this](Arguments& argv, long value) {

        datasette.rewind();
    });

    root.add({"datasette", "rewind", "to"}, { Arg::value },
             "Rewinds the tape to a specific position",
             [this](Arguments& argv, long value) {

        datasette.rewind(parseNum(argv[0]));
    });

    root.add({"datasette", "set"}, "Configures the component");

    for (auto &opt : datasette.getOptions()) {

        root.add({"datasette", "set", OptionEnum::key(opt)},
                 {OptionParser::create(opt)->argList()},
                 OptionEnum::help(opt),
                 [this](Arguments& argv, long opt) {

            emulator.set(Option(opt), argv[0]);

        }, opt);
    }


    //
    // Peripherals (Drives)
    //

    for (isize i = 0; i < 2; i++) {

        string drive = (i == 0) ? "drive8" : "drive9";

        root.add({drive}, "Floppy drive " + std::to_string(i + 8));

        root.add({drive, ""},
                 "Displays the current configuration",
                 [this](Arguments& argv, long value) {

            auto &drive = value ? drive9 : drive8;
            retroShell.dump(drive, Category::Config);

        }, i);

        root.add({drive, "connect"},
                 "Connects the drive",
                 [this](Arguments& argv, long value) {

            auto id = value ? DRIVE9 : DRIVE8;
            configure(OPT_DRV_CONNECT, id, true);

        }, i);

        root.add({drive, "disconnect"},
                 "Disconnects the drive",
                 [this](Arguments& argv, long value) {

            auto id = value ? DRIVE9 : DRIVE8;
            configure(OPT_DRV_CONNECT, id, false);

        }, i);

        root.add({drive, "eject"},
                 "Ejects a floppy disk",
                 [this](Arguments& argv, long value) {

            auto &drive = value ? drive9 : drive8;
            drive.ejectDisk();

        }, i);

        root.add({drive, "insert"}, { Arg::path },
                 "Inserts a floppy disk",
                 [this](Arguments& argv, long value) {

            auto path = argv.front();
            if (!util::fileExists(path)) throw VC64Error(ERROR_FILE_NOT_FOUND, path);

            auto &drive = value ? drive9 : drive8;
            drive.insertDisk(path, false);

        }, i);

        root.add({drive, "newdisk"}, { DOSTypeEnum::argList() },
                 "Inserts a new blank disk",
                 [this](Arguments& argv, long value) {

            auto type = util::parseEnum <DOSTypeEnum> (argv.front());
            auto &drive = value ? drive9 : drive8;
            drive.insertNewDisk(type, PETName<16>("NEW DISK"));

        }, i);

        root.add({drive, "set"}, "Configures the component");

        for (auto &opt : drive8.getOptions()) {

            root.add({drive, "set", OptionEnum::key(opt)},
                     {OptionParser::create(opt)->argList()},
                     OptionEnum::help(opt),
                     [this](Arguments& argv, long value) {

                emulator.set(Option(HI_WORD(value)), LO_WORD(value), argv[0]);

            }, HI_W_LO_W(opt, i));
        }
    }

    //
    // Peripherals (Parallel cable)
    //

    root.add({"parcable"},      "Parallel drive cable");


    //
    // Regression testing (hidden commands)
    //

 
    















    //
    // Joystick
    //



    //
    // Mouse
    //



    //
    // Parallel cable
    //

    root.add({"parcable", ""},
             "Displays the current configuration",
             [this](Arguments& argv, long value) {

        retroShell.dump(parCable, Category::Config);
    });
}

}
