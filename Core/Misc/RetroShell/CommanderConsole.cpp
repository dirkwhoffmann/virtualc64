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
CommanderConsole::_pause()
{
    
}

string
CommanderConsole::prompt()
{
    return "C64% ";
}

void
CommanderConsole::didActivate()
{
    if (!activated) {

        *this << "RetroShell " << C64::build() << '\n';
        *this << '\n';
        *this << "Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de" << '\n';
        *this << "https://github.com/dirkwhoffmann/virtualc64" << '\n';
        *this << '\n';
    }

    activated = true;
}

void
CommanderConsole::didDeactivate()
{
    
}

void
CommanderConsole::initCommands(RSCommand &root)
{
    Console::initCommands(root);


    //
    // Empty command
    //

    root.add({

        .tokens = { "return" },
        .chelp  = { "Print status information" },
        .flags  = rs::hidden,
        .func   = [] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            os << "\nRetroShell Commander\n";
        }
    });


    //
    // Workspace management
    //

    root.add({

        .tokens = { "workspace" },
        .ghelp  = { "Workspace management" },
        .flags  = releaseBuild ? rs::hidden : 0
    });

    root.add({

        .tokens = { "workspace init" },
        .chelp  = { "First command of a workspace script" },
        .flags  = releaseBuild ? rs::hidden : 0,
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            c64.initWorkspace();
        }
    });

    root.add({

        .tokens = { "workspace activate" },
        .chelp  = { "Last command of a workspace script" },
        .flags  = releaseBuild ? rs::hidden : 0,
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            c64.activateWorkspace();
        }
    });


    //
    // Regression tester
    //

    RSCommand::currentGroup = "Regression testing";

    auto cmd = registerComponent(regressionTester, releaseBuild ? rs::hidden : 0);

    root.add({

        .tokens = { "regression", "setup" },
        .ghelp  = { "Initializes the test environment" },
    });

    for (auto &it : ConfigSchemeEnum::elements()) {

        root.add({

            .tokens = { "regression", "setup", ConfigSchemeEnum::key(it) },
            .chelp  = { ConfigSchemeEnum::help(it) },
            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                regressionTester.prepare(c64, ConfigScheme(values[0]));

                // Pause the script to give the C64 some time to boot
                c64.scheduleRel<SLOT_RSH>(3 * vic.getFrequency(), RSH_WAKEUP);
                throw ScriptInterruption("");

            }, .payload = { isize(it) }
        });
    }

    root.add({

        .tokens = { "regression", "run" },
        .chelp  = { "Launches a regression test" },
        .args   = { { .name = { "path", "Regression test script" } } },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            auto path = host.makeAbsolute(args.at("path"));
            regressionTester.run(path);
        }
    });

    root.add({

        .tokens = { "screenshot" },
        .ghelp  = { "Manages screenshots" },
        .flags  = releaseBuild ? rs::hidden : 0
    });

    root.add({

        .tokens = { "screenshot", "set" },
        .ghelp  = { "Configure the screenshot" }
    });

    root.add({

        .tokens = { "screenshot", "set", "filename" },
        .chelp  = { "Assign the screenshot filename" },
        .args   = { { .name = { "path", "File path" } } },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            auto path = host.makeAbsolute(args.at("path"));
            regressionTester.screenshotPath = path;
        }
    });

    root.add({

        .tokens = { "screenshot", "set", "cutout" },
        .chelp  = { "Adjust the texture cutout" },
        .args   = {
            { .name = { "x1", "Left x coordinate" }, .flags = rs::keyval },
            { .name = { "x2", "Right x coordinate" }, .flags = rs::keyval },
            { .name = { "y1", "Lower y coordinate" }, .flags = rs::keyval },
            { .name = { "y2", "Upper y coordinate" }, .flags = rs::keyval }
        },
            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                isize x1 = parseNum(args.at("x1"));
                isize y1 = parseNum(args.at("y1"));
                isize x2 = parseNum(args.at("x2"));
                isize y2 = parseNum(args.at("y2"));

                regressionTester.x1 = x1;
                regressionTester.y1 = y1;
                regressionTester.x2 = x2;
                regressionTester.y2 = y2;
            }
    });

    root.add({

        .tokens = { "screenshot", "save" },
        .chelp  = { "Saves a screenshot and exits the emulator" },
        .args   = { { .name = { "path", "File path" } } },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            auto path = host.makeAbsolute(args.at("path"));
            regressionTester.dumpTexture(c64, path);
        }
    });


    //
    // Components
    //

    RSCommand::currentGroup = "Components";

    //
    // Components (C64)
    //

    cmd = registerComponent(c64);

    root.add({

        .tokens = { cmd, "defaults" },
        .chelp  = { "Displays the user defaults storage" },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            dump(os, emulator, Category::Defaults);
        }
    });

    root.add({

        .tokens = { cmd, "power" },
        .chelp  = { "Switches the C64 on or off" },
        .args   = { { .name = { "onoff", "Power switch state" }, .key = "{ on | off }" } },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            parseOnOff(args.at("onoff")) ? emulator.run() : emulator.powerOff();
        }
    });

    root.add({

        .tokens = { cmd, "reset" },
        .chelp  = { "Performs a hard reset" },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            c64.hardReset();
        }
    });

    root.add({

        .tokens = { cmd, "init" },
        .ghelp  = { "Initializes the C64 with a predefined scheme" },
    });

    for (auto &it : ConfigSchemeEnum::elements()) {

        root.add({

            .tokens = { cmd, "init", ConfigSchemeEnum::key(it) },
            .chelp  = { ConfigSchemeEnum::help(it) },
            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                emulator.set(ConfigScheme(values[0]));
            }, .payload = { isize(it) }
        });
    }


    //
    // Components (Memory)
    //

    cmd = registerComponent(mem);

    root.add({

        .tokens = { cmd, "flash" },
        .chelp  = { "Flashes a file into memory" },
        .args   = { { .name = { "path", "File path" } } },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            auto path = host.makeAbsolute(args.at("path"));
            if (!util::fileExists(path)) throw AppError(Fault::FILE_NOT_FOUND, path);

            auto file = PRGFile(path);
            c64.flash(file, 0);
        }
    });

    root.add({

        .tokens = { cmd, "load" },
        .ghelp  = { "Loads memory contents from a file" }
    });

    root.add({

        .tokens = { cmd, "load", "rom" },
        .chelp  = { "Installs a ROM image" },
        .args   = { { .name = { "path", "File path" } } },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            auto path = host.makeAbsolute(args.at("path"));
            printf("load rom: %s\n", path.string().c_str());
            c64.loadRom(path);
        }
    });

    root.add({

        .tokens = { cmd, "load", "bin" },
        .chelp  = { "Loads a chunk of memory" },
        .args   = {
            { .name = { "path", "File path" } },
            { .name = { "address", "Target memory address" } },
        },
            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                auto path = host.makeAbsolute(args.at("path"));
                mem.debugger.load(path, parseAddr(args.at("address")));
            }
    });

    root.add({

        .tokens = { cmd, "load", "openroms" },
        .chelp  = { "Installs the MEGA65 OpenROMs" },
        .args   = {
            { .name = { "path", "File path" } },
            { .name = { "address", "Target memory address" } },
        },
            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                c64.installOpenRoms();
            }
    });

    root.add({

        .tokens = { cmd, "save" },
        .chelp  = { "Save memory contents to a file" }
    });

    root.add({

        .tokens = { cmd, "save", "bin" },
        .chelp  = { "Saves a chunk of memory" },
        .args   = {
            { .name = { "path", "File path" } },
            { .name = { "address", "Memory address" } },
            { .name = { "count", "Number of bytes" } },
        },
            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                fs::path path(args.at("path"));
                mem.debugger.save(path, parseAddr(args.at("address")), parseNum(args.at("count")));
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
    // Components (SID)
    //

    cmd = registerComponent(sid0);
    cmd = registerComponent(sid1);
    cmd = registerComponent(sid2);
    cmd = registerComponent(sid3);


    //
    // Ports
    //

    RSCommand::currentGroup = "Ports";


    //
    // Ports (Power port)
    //

    cmd = registerComponent(powerSupply);


    //
    // Ports (Audio port)
    //

    cmd = registerComponent(audioPort);


    //
    // Ports (Video port)
    //

    cmd = registerComponent(videoPort);


    //
    // Ports (Expansion port)
    //

    cmd = registerComponent(expansionPort);

    /*
     root.add({

     .tokens = { cmd, "attach" },
     .chelp  = { "Attaches a cartridge" }
     });
     */

    root.add({

        .tokens = { cmd, "attach" },
        .ghelp  = { "Attaches a cartridge" },
        .chelp  = { "Attaches a cartridge from a CRT file" },
        .args   = { { .name = { "path", "File path" } } },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            auto path = host.makeAbsolute(args.at("path"));
            expansionPort.attachCartridge(path);
        }
    });

    root.add({

        .tokens = { cmd, "attach", "reu" },
        .chelp  = { "Attaches a REU expansion cartridge" },
        .args   = { { .name = { "KB", "REU size" } } },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            expansionPort.attachReu(parseNum(args.at("KB")));
        }
    });

    root.add({

        .tokens = { cmd, "attach", "georam" },
        .chelp  = { "Attaches a GeoRAM expansion cartridge" },
        .args   = { { .name = { "KB", "REU size" } } },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            expansionPort.attachGeoRam(parseNum(args.at("KB")));
        }
    });

    root.add({

        .tokens = { cmd, "detach" },
        .chelp  = { "Detaches the current cartridge" },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            expansionPort.detachCartridge();
        }
    });


    //
    // Ports (User port)
    //

    cmd = registerComponent(userPort);


    //
    // Peripherals
    //

    RSCommand::currentGroup = "Peripherals";


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
        .chelp  = { "Presses a key" },
        .args   = { { .name = { "keycode", "Numerical code of the C64 key" } } },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            keyboard.press(C64Key(parseNum(args.at("keycode"))));
        }
    });

    root.add({

        .tokens = { cmd, "release" },
        .chelp  = { "Releases a key" },
        .args   = { { .name = { "keycode", "Numerical code of the C64 key" } } },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            keyboard.release(C64Key(parseNum(args.at("keycode"))));
        }
    });

    root.add({

        .tokens = { cmd, "type" },
        .chelp  = { "Types text on the keyboard" }
    });

    root.add({

        .tokens = { cmd, "type", "text" },
        .chelp  = { "Types text on the keyboard" },
        .args   = { { .name = { "text", "Text to type" } } },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            keyboard.autoType(args.at("text"));
        }
    });

    root.add({

        .tokens = { cmd, "type", "load" },
        .chelp  = { "Types \"LOAD\"*\",8,1" },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            keyboard.autoType("load \"*\",8,1\n");
        }
    });

    root.add({

        .tokens = { cmd, "type", "run" },
        .chelp  = { "Types RUN" },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            keyboard.autoType("run\n");
        }
    });


    //
    // Peripherals (Drives)
    //

    root.add({

        .tokens = { "drive[n]" },
        .ghelp  = { "Drive n" },
        .chelp  = { "Commands: drive8, drive9" }
    });

    for (isize i = 0; i < 2; i++) {

        if (i == 0) cmd = registerComponent(drive8, rs::shadowed);
        if (i == 1) cmd = registerComponent(drive9, rs::shadowed);

        root.add({

            .tokens = { cmd, "eject" },
            .chelp  = { "Ejects a floppy disk" },
            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                auto &drive = values.front() ? drive9 : drive8;
                drive.ejectDisk();

            }, .payload = {i}
        });

        root.add({

            .tokens = { cmd, "insert" },
            .chelp  = { "Inserts a floppy disk" },
            .args   = {{ .name = { "path", "File path" } } },
            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                auto path = host.makeAbsolute(args.at("path"));
                if (!util::fileExists(path)) throw AppError(Fault::FILE_NOT_FOUND, path);

                auto &drive = values.front() ? drive9 : drive8;
                drive.insertDisk(path, false);

            }, .payload = {i}
        });

        root.add({

            .tokens = { cmd, "newdisk" },
            .chelp  = { "Inserts a new blank disk" },
            .args   = { { .name = { "dos", "DOS type" } } },
            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                auto &drive = values.front() ? drive9 : drive8;
                auto type = util::parseEnum <DOSType, DOSTypeEnum> (args.at("dos"));
                drive.insertNewDisk(type, "NEW DISK");

            }, .payload = {i}
        });

        root.add({

            .tokens = { cmd, "protect" },
            .chelp  = { "Enables write protection" },
            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                auto &drive = values.front() ? drive9 : drive8;
                drive.protectDisk();

            }, .payload = {i}
        });

        root.add({

            .tokens = { cmd, "unprotect" },
            .chelp  = { "Disables write protection" },
            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                auto &drive = values.front() ? drive9 : drive8;
                drive.unprotectDisk();

            }, .payload = {i}
        });
    }


    //
    // Peripherals (Datasette)
    //

    cmd = registerComponent(datasette);

    root.add({

        .tokens = { cmd, "insert" },
        .chelp  = { "Inserts a tape from a TAP file" },
        .args   = { { .name = { "path", "File path" } } },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            auto path = host.makeAbsolute(args.at("path"));
            datasette.insertTape(path);
        }
    });

    root.add({

        .tokens = { cmd, "rewind" },
        .chelp  = { "Rewinds the tape" },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            datasette.rewind();
        }
    });

    root.add({

        .tokens = { cmd, "rewind", "to" },
        .chelp  = { "Rewinds the tape to a specific position" },
        .args   = { { .name = { "pos", "Tape position" } } },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            datasette.rewind(parseNum(args.at("pos")));
        }
    });


    //
    // Peripherals (Mouse)
    //

    root.add({

        .tokens = { "mouse[n]" },
        .ghelp  = { "Mouse n" },
        .chelp  = { "Commands: mouse1, mouse2" }
    });

    registerComponent(c64.port1.mouse, rs::shadowed);
    registerComponent(c64.port2.mouse, rs::shadowed);


    //
    // Peripherals (Joystick)
    //

    root.add({

        .tokens = { "joystick[n]" },
        .ghelp  = { "Joystick n" },
        .chelp  = { "Commands: joystick1, joystick2" }
    });

    for (isize i = 0; i <= 1; i++) {

        if (i == 0) cmd = registerComponent(port1.joystick, rs::shadowed);
        if (i == 1) cmd = registerComponent(port2.joystick, rs::shadowed);

        root.add({

            .tokens = { cmd, "press" },
            .chelp  = { "Presses a joystick button" },
            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                auto &port = (values[0] == 0) ? c64.port1 : c64.port2;
                port.joystick.trigger(GamePadAction::PRESS_FIRE);

            }, .payload = {i}
        });

        root.add({

            .tokens = { cmd, "unpress" },
            .chelp  = { "Releases a joystick button" },
            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                auto &port = (values[0] == 0) ? c64.port1 : c64.port2;
                port.joystick.trigger(GamePadAction::RELEASE_FIRE);

            }, .payload = {i}
        });

        root.add({

            .tokens = { cmd, "pull" },
            .ghelp  = { "Pulls the joystick" }
        });

        root.add({

            .tokens = { cmd, "pull", "left" },
            .chelp  = { "Pulls the joystick left" },
            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                auto &port = (values.front() == PORT_1) ? c64.port1 : c64.port2;
                port.joystick.trigger(GamePadAction::PULL_LEFT);

            }, .payload = {i}
        });

        root.add({

            .tokens = { cmd, "pull", "right" },
            .chelp  = { "Pulls the joystick right" },
            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                auto &port = (values.front() == PORT_1) ? c64.port1 : c64.port2;
                port.joystick.trigger(GamePadAction::PULL_RIGHT);

            }, .payload = {i}
        });

        root.add({

            .tokens = { cmd, "pull", "up" },
            .chelp  = { "Pulls the joystick up" },
            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                auto &port = (values.front() == PORT_1) ? c64.port1 : c64.port2;
                port.joystick.trigger(GamePadAction::PULL_UP);

            }, .payload = {i}
        });

        root.add({

            .tokens = { cmd, "pull", "down" },
            .chelp  = { "Pulls the joystick down" },
            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                auto &port = (values.front() == PORT_1) ? c64.port1 : c64.port2;
                port.joystick.trigger(GamePadAction::PULL_DOWN);

            }, .payload = {i}
        });

        root.add({

            .tokens = { cmd, "release" },
            .ghelp  = { "Release a joystick axis" }
        });

        root.add({

            .tokens = { cmd, "release", "x" },
            .chelp  = { "Releases the x-axis" },
            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                auto &port = (values.front() == PORT_1) ? c64.port1 : c64.port2;
                port.joystick.trigger(GamePadAction::RELEASE_X);

            }, .payload = {i}
        });

        root.add({

            .tokens = { cmd, "release", "y" },
            .chelp  = { "Releases the y-axis" },
            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                auto &port = (values.front() == PORT_1) ? c64.port1 : c64.port2;
                port.joystick.trigger(GamePadAction::RELEASE_Y);

            }, .payload = {i}
        });
    }


    //
    // Peripherals (Paddles)
    //

    root.add({

        .tokens = { "paddle[n]" },
        .ghelp  = { "Paddle n" },
        .chelp  = { "Commands: paddle1, paddle2" }
    });

    cmd = registerComponent(port1.paddle, rs::shadowed);
    cmd = registerComponent(port2.paddle, rs::shadowed);


    //
    // Peripherals (RS232)
    //

    cmd = registerComponent(userPort.rs232);

    root.add({

        .tokens = { cmd, "send" },
        .chelp  = { "Feeds text into the RS232 adapter" },
        .args   = { { .name = { "text", "Text message" } } },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            userPort.rs232 << args.at("text");
        }
    });


    //
    // Miscellaneous
    //

    RSCommand::currentGroup = "Miscellaneous";

    //
    // Miscellaneous (Host)
    //

    cmd = registerComponent(host);

    root.add({

        .tokens = { cmd, "searchpath" },
        .chelp  = { "Sets the search path for media files" },
        .args   = { { .name = { "path", "File path" } } },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            auto path = fs::path(args.at("path"));
            host.setSearchPath(path);
        }
    });


    //
    // Miscellaneous (Config)
    //

    root.add({

        .tokens = { "config" },
        .ghelp  = { "Virtual machine configuration" },
        .chelp  = { "Displays the current configuration" },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            c64.exportConfig(os);
        }
    });

    root.add({

        .tokens = { "config", "diff" },
        .chelp  = { "Displays the difference to the default configuration" },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            c64.exportDiff(os);
        }
    });


    //
    // Components (DMA Debugger)
    //

    cmd = registerComponent(vic.dmaDebugger);


    //
    // Miscellaneous (Remote server)
    //

    root.add({

        .tokens = { "server" },
        .ghelp  = { "Remote connections" },
        .chelp  = { "Displays a server status summary" },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            dump(os, remoteManager, Category::State);
        }
    });

    // RSH Server

    cmd = registerComponent(remoteManager.rshServer);

    /*
    root.add({

        .tokens = { cmd, "start" },
        .chelp  = { "Starts the RetroShell server" },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            remoteManager.rshServer.start();
        }
    });

    root.add({

        .tokens = { cmd, "stop" },
        .chelp  = { "Stops the RetroShell server" },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            remoteManager.rshServer.stop();
        }
    });

    root.add({

        .tokens = { cmd, "disconnect" },
        .chelp  = { "Disconnects a client" },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            remoteManager.rshServer.disconnect();
        }
    });
    */

    // RPC Server

    cmd = registerComponent(remoteManager.rpcServer);

    /*
    root.add({

        .tokens = { cmd, "start" },
        .chelp  = { "Starts the RPC server" },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            remoteManager.rpcServer.start();
        }
    });

    root.add({

        .tokens = { cmd, "stop" },
        .chelp  = { "Stops the RetroShell server" },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            remoteManager.rpcServer.stop();
        }
    });

    root.add({

        .tokens = { cmd, "disconnect" },
        .chelp  = { "Disconnects a client" },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            remoteManager.rpcServer.disconnect();
        }
    });
    */

    // DAP Server

    cmd = registerComponent(remoteManager.dapServer, releaseBuild ? rs::hidden : 0);

    /*
    root.add({

        .tokens = { cmd, "start" },
        .chelp  = { "Starts the DAP server" },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            remoteManager.dapServer.start();
        }
    });

    root.add({

        .tokens = { cmd, "stop" },
        .chelp  = { "Stops the DAP server" },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            remoteManager.dapServer.stop();
        }
    });

    root.add({

        .tokens = { cmd, "disconnect" },
        .chelp  = { "Disconnects a client" },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            remoteManager.dapServer.disconnect();
        }
    });
    */

    // PROM Server

    cmd = registerComponent(remoteManager.promServer, releaseBuild ? rs::hidden : 0);

    /*
    root.add({

        .tokens = { cmd, "start" },
        .chelp  = { "Starts the Prometheus server" },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            remoteManager.promServer.start();
        }
    });

    root.add({

        .tokens = { cmd, "stop" },
        .chelp  = { "Stops the Prometheus server" },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            remoteManager.promServer.stop();
        }
    });

    root.add({

        .tokens = { cmd, "disconnect" },
        .chelp  = { "Disconnects a client" },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            remoteManager.promServer.disconnect();
        }
    });
    */
}

}
