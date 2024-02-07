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
             [this](Arguments& argv, long value) {

        retroShell.clear();
        switchInterpreter();
        retroShell.welcome();
    });

    root.add({"clear"},
             "Clears the console window",
             [this](Arguments& argv, long value) {

        retroShell.clear();
    });

    root.add({"close"},
             "Hides the console window",
             [this](Arguments& argv, long value) {

        msgQueue.put(MSG_CONSOLE_CLOSE);
    });

    root.add({"help"}, { }, {Arg::command},
             "Prints usage information",
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
             "Processes a command script",
             [this](Arguments& argv, long value) {

        auto stream = std::ifstream(argv.front());
        if (!stream.is_open()) throw VC64Error(ERROR_FILE_NOT_FOUND, argv.front());
        retroShell.execScript(stream);
    });

    root.add({"wait"}, {Arg::value, Arg::seconds},
             "Pauses the execution of a command script",
             [this](Arguments& argv, long value) {

        auto cycles = parseNum(argv) * vic.getFrequency();
        c64.scheduleRel<SLOT_RSH>(cycles, RSH_WAKEUP);
        throw ScriptInterruption("");
    });
}

void
Interpreter::initCommandShell(Command &root)
{
    initCommons(root);

    //
    // Top-level commands
    //

    root.newGroup("* Regression testing");

    root.add({"regression"},    "Runs the regression tester");
    root.add({"screenshot"},    "Manages screenshots");

    root.newGroup("Controlling components");

    root.add({"emulator"},      "The emulator thread");
    root.add({"c64"},           "The virtual Commodore 64");
    root.add({"memory"},        "Ram and Rom");
    root.add({"cia1"},          "Complex Interface Adapter 1");
    root.add({"cia2"},          "Complex Interface Adapter 2");
    root.add({"vicii"},         "Video Interface Controller");
    root.add({"dmadebugger"},   "DMA Debugger");
    root.add({"sid"},           "Sound Interface Device");
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

    root.newGroup("");

    root.add({"regression", "setup"}, { C64ModelEnum::argList() },
             "Initializes the test environment",
             [this](Arguments& argv, long value) {

        auto model = parseEnum <C64ModelEnum> (argv);
        regressionTester.prepare(c64, model);

        // Pause the script to give the C64 some time to boot
        c64.scheduleRel<SLOT_RSH>(3 * vic.getFrequency(), RSH_WAKEUP);
        throw ScriptInterruption("");
    });

    root.add({"regression", "run"}, { Arg::path },
             "Launches a regression test",
             [this](Arguments& argv, long value) {

        regressionTester.run(argv.front());
    });

    root.add({"screenshot", "set"},
             "Configures the screenshot");

    root.add({"screenshot", "set", "filename"}, { Arg::path },
             "Assigns the screen shot filename",
             [this](Arguments& argv, long value) {

        regressionTester.dumpTexturePath = argv.front();
    });

    root.add({"screenshot", "set", "cutout"}, { Arg::value, Arg::value, Arg::value, Arg::value },
             "Adjusts the texture cutout",
             [this](Arguments& argv, long value) {

        isize x1 = parseNum(argv, 0);
        isize y1 = parseNum(argv, 1);
        isize x2 = parseNum(argv, 2);
        isize y2 = parseNum(argv, 3);

        regressionTester.x1 = x1;
        regressionTester.y1 = y1;
        regressionTester.x2 = x2;
        regressionTester.y2 = y2;
    });

    root.add({"screenshot", "save"}, { Arg::path },
             "Saves a screenshot and exits the emulator",
             [this](Arguments& argv, long value) {

        regressionTester.dumpTexture(c64, argv.front());
    });

    
    //
    // Emulator
    //

    root.add({"emulator", ""},
             "Displays the current configuration",
             [this](Arguments& argv, long value) {

        retroShell.dump(emulator, Category::Config);
    });


    //
    // C64
    //

    root.add({"c64", ""},
             "Displays the current configuration",
             [this](Arguments& argv, long value) {

        retroShell.dump(c64, Category::Config);
    });

    root.add({"c64", "defaults"},
             "Displays the user defaults storage",
             [this](Arguments& argv, long value) {

        retroShell.dump(c64, Category::Defaults);
    });

    root.add({"c64", "set"},
             "Configures the component");

    root.add({"c64", "set", "warpboot"}, { Arg::onoff },
             "Enables or disables warp mode while Kickstart initializes",
             [this](Arguments& argv, long value) {

        c64.configure(OPT_WARP_BOOT, parseBool(argv));
    });

    root.add({"c64", "set", "warpmode"}, { WarpModeEnum::argList() },
             "Selects the warp mode",
             [this](Arguments& argv, long value) {

        c64.configure(OPT_WARP_MODE, parseEnum <WarpModeEnum> (argv));
    });

    root.add({"c64", "set", "vsync"}, { Arg::onoff },
             "Enables or disables VSYNC",
             [this](Arguments& argv, long value) {

        c64.configure(OPT_VSYNC, parseBool(argv[0]));
    });

    root.add({"c64", "set", "timelapse"}, { Arg::value },
             "Increases or decreases the native frame rate",
             [this](Arguments& argv, long value) {

        c64.configure(OPT_TIME_LAPSE, parseNum(argv[0]));
    });

    root.add({"c64", "set", "runahead"}, { Arg::value },
             "Sets the number of run-ahead frames",
             [this](Arguments& argv, long value) {

        c64.configure(OPT_RUN_AHEAD, parseNum(argv));
    });

    root.add({"c64", "power"}, { Arg::onoff },
             "Switches the C64 on or off",
             [this](Arguments& argv, long value) {

        parseOnOff(argv) ? c64.emulator.run() : c64.emulator.powerOff();
    });

    root.add({"c64", "reset"},
             "Performs a hard reset",
             [this](Arguments& argv, long value) {

        c64.hardReset();
    });

    root.add({"c64", "init"}, { C64ModelEnum::argList() },
             "Initializes the emulator with factory defaults",
             [this](Arguments& argv, long value) {

        auto model = parseEnum<C64ModelEnum>(argv);

        c64.revertToFactorySettings();
        c64.configure(model);
    });

    
    //
    // Memory
    //

    root.add({"memory", ""},
             "Displays the current configuration",
             [this](Arguments& argv, long value) {

        retroShell.dump(mem, Category::Config);
    });

    root.add({"memory", "set"},
             "Configures the component");

    root.add({"memory", "set", "saveroms"}, { Arg::onoff },
             "Save Roms to snapshot files",
             [this](Arguments& argv, long value) {

        c64.configure(OPT_SAVE_ROMS, parseBool(argv));
    });

    root.add({"memory", "set", "raminit" }, { RamPatternEnum::argList() },
             "Determines how Ram is initialized on startup",
             [this](Arguments& argv, long value) {

        c64.configure(OPT_RAM_PATTERN, parseEnum<RamPatternEnum>(argv));
    });

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
    // CIA
    //

    for (isize i = 0; i < 2; i++) {

        string cia = (i == 0) ? "cia1" : "cia2";

        root.add({cia, ""},
                 "Displays the current configuration",
                 [this](Arguments& argv, long value) {

            if (value == 0) retroShell.dump(cia1, Category::Config);
            if (value == 1) retroShell.dump(cia2, Category::Config);

        }, i);

        root.add({cia, "set"},
                 "Configures the component");
        
        root.add({cia, "set", "revision"}, { CIARevisionEnum::argList() },
                 "Selects the emulated chip model",
                 [this](Arguments& argv, long value) {

            c64.configure(OPT_CIA_REVISION, value, parseEnum <CIARevisionEnum> (argv));

        }, i);

        root.add({cia, "set", "timerbbug"}, { Arg::onoff },
                 "Enables or disables the timer B hardware bug",
                 [this](Arguments& argv, long value) {

            c64.configure(OPT_TIMER_B_BUG, value, parseBool(argv));

        }, i);
    }


    //
    // VICII
    //

    root.add({"vicii", ""},
             "Displays the current configuration",
             [this](Arguments& argv, long value) {

        retroShell.dump(vic, Category::Config);
    });

    root.add({"vicii", "set"},
             "Configures the component");

    root.add({"vicii", "set", "revision"}, { VICIIRevisionEnum::argList() },
             "Selects the emulated chip model",
             [this](Arguments& argv, long value) {

        c64.configure(OPT_VIC_REVISION, parseEnum <VICIIRevisionEnum> (argv));

    });

    root.add({"vicii", "set", "graydotbug"}, { Arg::onoff },
             "Enables or disables the gray dot bug",
             [this](Arguments& argv, long value) {

        c64.configure(OPT_GRAY_DOT_BUG, parseBool(argv));
    });

    root.add({"vicii", "set", "gluelogic"}, { Arg::onoff },
             "Configures the logic board",
             [this](Arguments& argv, long value) {

        c64.configure(OPT_GLUE_LOGIC, parseBool(argv));
    });

    root.add({"vicii", "set", "sscollisions"}, { Arg::onoff },
             "Enables or disables sprite-sprite collision detection",
             [this](Arguments& argv, long value) {

        c64.configure(OPT_SS_COLLISIONS, parseBool(argv));
    });

    root.add({"vicii", "set", "sbcollisions"}, { Arg::onoff },
             "Enables or disables sprite-background collision detection",
             [this](Arguments& argv, long value) {

        c64.configure(OPT_SB_COLLISIONS, parseBool(argv));
    });

    
    //
    // DMA Debugger
    //

    root.add({"dmadebugger", ""},
             "Displays the current configuration",
             [this](Arguments& argv, long value) {

        retroShell.dump(vic.dmaDebugger, Category::Config);
    });

    root.add({"dmadebugger", "open"},
             "Opens the DMA debugger",
             [this](Arguments& argv, long value) {

        c64.configure(OPT_DMA_DEBUG_ENABLE, true);
    });

    root.add({"dmadebugger", "close"},
             "Closes the DMA debugger",
             [this](Arguments& argv, long value) {

        c64.configure(OPT_DMA_DEBUG_ENABLE, false);
    });

    root.add({"dmadebugger", "raccesses"}, { Arg::onoff },
             "Visualizes refresh cycles",
             [this](Arguments& argv, long value) {

        c64.configure(OPT_DMA_DEBUG_CHANNEL, 0, parseBool(argv));
    });

    root.add({"dmadebugger", "iaccesses"}, { Arg::onoff },
             "Visualizes idle accesses",
             [this](Arguments& argv, long value) {

        c64.configure(OPT_DMA_DEBUG_CHANNEL, 1, parseBool(argv));
    });

    root.add({"dmadebugger", "caccesses"}, { Arg::onoff },
             "Visualizes character accesses",
             [this](Arguments& argv, long value) {

        c64.configure(OPT_DMA_DEBUG_CHANNEL, 2, parseBool(argv));
    });

    root.add({"dmadebugger", "gaccesses"}, { Arg::onoff },
             "Visualizes graphics accesses",
             [this](Arguments& argv, long value) {

        c64.configure(OPT_DMA_DEBUG_CHANNEL, 3, parseBool(argv));
    });

    root.add({"dmadebugger", "paccesses"}, { Arg::onoff },
             "Visualizes sprite pointer accesses",
             [this](Arguments& argv, long value) {

        c64.configure(OPT_DMA_DEBUG_CHANNEL, 4, parseBool(argv));
    });

    root.add({"dmadebugger", "saccesses"}, { Arg::onoff },
             "Visualizes sprite accesses",
             [this](Arguments& argv, long value) {

        c64.configure(OPT_DMA_DEBUG_CHANNEL, 5, parseBool(argv));
    });


    //
    // Monitor
    //

    root.add({"monitor", ""},
             "Displays the current configuration",
             [this](Arguments& argv, long value) {

        retroShell.dump(vic.dmaDebugger, Category::Config);
    });

    root.add({"monitor", "set"},
             "Configures the component");

    root.add({"monitor", "set", "palette"}, { PaletteEnum::argList() },
             "Selects the color palette",
             [this](Arguments& argv, long value) {

        c64.configure(OPT_PALETTE, util::parseEnum <PaletteEnum> (argv.front()));
    });

    root.add({"monitor", "set", "brightness"}, { Arg::value },
             "Adjusts the monitor brightness",
             [this](Arguments& argv, long value) {

        c64.configure(OPT_BRIGHTNESS, util::parseNum(argv.front()));
    });

    root.add({"monitor", "set", "contrast"}, { Arg::value },
             "Adjusts the monitor contrast",
             [this](Arguments& argv, long value) {

        c64.configure(OPT_CONTRAST, util::parseNum(argv.front()));
    });

    root.add({"monitor", "set", "saturation"}, { Arg::value },
             "Adjusts the color saturation",
             [this](Arguments& argv, long value) {

        c64.configure(OPT_SATURATION, util::parseNum(argv.front()));
    });

    
    //
    // SID
    //

    root.add({"sid", ""},
             "Displays the current configuration",
             [this](Arguments& argv, long value) {

        retroShell.dump(muxer, Category::Config);
    });

    root.add({"sid", "set"},
             "Configures the component");

    root.add({"sid", "set", "engine"}, { SIDEngineEnum::argList() },
             "Selects the SID backend",
             [this](Arguments& argv, long value) {

        c64.configure(OPT_SID_ENGINE, parseEnum <SIDEngineEnum> (argv));

    });

    root.add({"sid", "set", "revision"}, { SIDRevisionEnum::argList() },
             "Selects the emulated chip model",
             [this](Arguments& argv, long value) {

        c64.configure(OPT_SID_REVISION, parseEnum <SIDRevisionEnum> (argv));

    });

    root.add({"sid", "set", "sampling"}, { SamplingMethodEnum::argList() },
             "Selects the sampling method",
             [this](Arguments& argv, long value) {

        c64.configure(OPT_SID_SAMPLING, parseEnum <SamplingMethodEnum> (argv));

    });

    root.add({"sid", "set", "filter"}, { Arg::onoff },
             "Configures the audio filter",
             [this](Arguments& argv, long value) {

        c64.configure(OPT_SID_FILTER, parseBool(argv));
    });

    root.add({"sid", "set", "volume"},
             "Sets the volume");

    root.add({"sid", "set", "volume", "channel0"}, { Arg::volume },
             "Sets the volume for the first SID",
             [this](Arguments& argv, long value) {

        c64.configure(OPT_AUDVOL, 0, parseNum(argv));
    });

    root.add({"sid", "set", "volume", "channel1"}, { Arg::volume },
             "Sets the volume for the second SID",
             [this](Arguments& argv, long value) {

        c64.configure(OPT_AUDVOL, 1, parseNum(argv));
    });

    root.add({"sid", "set", "volume", "channel2"}, { Arg::volume },
             "Sets the volume for the third SID",
             [this](Arguments& argv, long value) {

        c64.configure(OPT_AUDVOL, 2, parseNum(argv));
    });

    root.add({"sid", "set", "volume", "channel3"}, { Arg::volume },
             "Sets the volume for the fourth SID",
             [this](Arguments& argv, long value) {

        c64.configure(OPT_AUDVOL, 3, parseNum(argv));
    });

    root.add({"sid", "set", "volume", "left"}, { Arg::volume },
             "Sets the master volume for the left speaker",
             [this](Arguments& argv, long value) {

        c64.configure(OPT_AUDVOLL, parseNum(argv));
    });

    root.add({"sid", "set", "volume", "right"}, { Arg::volume },
             "Sets the master volume for the right speaker",
             [this](Arguments& argv, long value) {

        c64.configure(OPT_AUDVOLR, parseNum(argv));
    });

    root.add({"sid", "set", "pan"},
             "Sets the pan for one of the four SIDs");
    
    root.add({"sid", "set", "pan", "channel0"}, { Arg::value },
             "Sets the pan for the first SID",
             [this](Arguments& argv, long value) {

        c64.configure(OPT_AUDPAN, 0, parseNum(argv));
    });

    root.add({"sid", "set", "pan", "channel1"}, { Arg::value },
             "Sets the pan for the second SID",
             [this](Arguments& argv, long value) {

        c64.configure(OPT_AUDPAN, 1, parseNum(argv));
    });

    root.add({"sid", "set", "pan", "channel2"}, { Arg::value },
             "Sets the pan for the third SID",
             [this](Arguments& argv, long value) {

        c64.configure(OPT_AUDPAN, 2, parseNum(argv));
    });

    root.add({"sid", "set", "pan", "channel3"}, { Arg::value },
             "Sets the pan for the fourth SID",
             [this](Arguments& argv, long value) {

        c64.configure(OPT_AUDPAN, 3, parseNum(argv));
    });


    //
    // Expansion port
    //

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

        expansionport.attachReu(parseNum(argv));
    });

    root.add({"expansion", "attach", "georam"}, { "<KB>" },
             "Attaches a GeoRAM expansion cartridge",
             [this](Arguments& argv, long value) {

        expansionport.attachGeoRam(parseNum(argv));
    });

    
    //
    // Keyboard
    //

    root.add({"keyboard", "press"}, { Arg::value },
             "Presses a key",
             [this](Arguments& argv, long value) {

        keyboard.press(C64Key(parseNum(argv)));
    });

    root.add({"keyboard", "release"}, { Arg::value },
             "Presses a key",
             [this](Arguments& argv, long value) {

        keyboard.release(C64Key(parseNum(argv)));
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
    // Drive
    //

    for (isize i = 0; i < 2; i++) {

        string drive = (i == 0) ? "drive8" : "drive9";

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
            c64.configure(OPT_DRV_CONNECT, id, true);

        }, i);

        root.add({drive, "disconnect"},
                 "Disconnects the drive",
                 [this](Arguments& argv, long value) {

            auto id = value ? DRIVE9 : DRIVE8;
            c64.configure(OPT_DRV_CONNECT, id, false);

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
    }


    //
    // Datasette
    //

    root.add({"datasette", ""},
             "Displays the current configuration",
             [this](Arguments& argv, long value) {

        retroShell.dump(datasette, Category::Config);
    });

    root.add({"datasette", "connect"},
             "Connects the datasette",
             [this](Arguments& argv, long value) {

        c64.configure(OPT_DAT_CONNECT, true);
    });

    root.add({"datasette", "disconnect"},
             "Disconnects the datasette",
             [this](Arguments& argv, long value) {

        c64.configure(OPT_DAT_CONNECT, false);
    });

    root.add({"datasette", "rewind"},
             "Rewinds the tape",
             [this](Arguments& argv, long value) {

        datasette.rewind();
    });

    root.add({"datasette", "rewind", "to"}, { Arg::value },
             "Rewinds the tape to a specific position",
             [this](Arguments& argv, long value) {

        datasette.rewind(parseNum(argv));
    });


    //
    // Joystick
    //

    root.newGroup("");

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

        root.add({"joystick", nr, "set"},
                 "Configures the component");

        root.add({"joystick", nr, "set", "autofire"}, { Arg::onoff },
                 "Enables or disables auto fire mode",
                 [this](Arguments& argv, long value) {

            c64.configure(OPT_AUTOFIRE, value, parseBool(argv));

        }, i);

        root.add({"joystick", nr, "set", "bullets"}, { Arg::value },
                 "Sets the number of bullets per auto fire shot",
                 [this](Arguments& argv, long value) {

            c64.configure(OPT_AUTOFIRE_BULLETS, value, parseNum(argv));

        }, i);


        root.add({"joystick", nr, "set", "delay"}, { Arg::value },
                 "Sets the auto fire delay in frames",
                 [this](Arguments& argv, long value) {

            c64.configure(OPT_AUTOFIRE_DELAY, value, parseNum(argv));

        }, i);

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
    // Mouse
    //

    root.newGroup("");

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

        root.add({"mouse", nr, "set"},
                 "Configures the component");

        root.add({"mouse", nr, "set", "model"}, { MouseModelEnum::argList() },
                 "Selects the mouse model",
                 [this](Arguments& argv, long value) {

            c64.configure(OPT_MOUSE_MODEL, value, parseEnum <MouseModelEnum> (argv));

        }, i);

        root.add({"mouse", nr, "set", "velocity"}, { Arg::value },
                 "Sets the horizontal and vertical mouse velocity",
                 [this](Arguments& argv, long value) {

            c64.configure(OPT_MOUSE_VELOCITY, value, parseNum(argv));

        }, i);

        root.add({"mouse", nr, "set", "shakedetector"}, { Arg::onoff },
                 "Enables or disables the shake detector",
                 [this](Arguments& argv, long value) {

            c64.configure(OPT_SHAKE_DETECTION, value, parseBool(argv));

        }, i);

        /*
        root.add({"mouse", nr, "press"},
                 "Presses a mouse button");

        root.add({"mouse", nr, "press", "left"},
                 "Presses the left mouse button",
                 [this](Arguments& argv, long value) {

            auto &port = (value == PORT_1) ? c64.port1 : c64.port2;
            port.mouse.pressAndReleaseLeft();

        }, i);

        root.add({"mouse", nr, "press", "right"},
                 "Presses the right mouse button",
                 [this](Arguments& argv, long value) {

            auto &port = (value == PORT_1) ? c64.port1 : c64.port2;
            port.mouse.pressAndReleaseRight();

        }, i);
        */
    }


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
