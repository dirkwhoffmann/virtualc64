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
#include "RetroShell.h"
#include "Emulator.h"
#include "IOUtils.hpp"
#include "Parser.hpp"

#include <fstream>
#include <sstream>

namespace vc64 {

void
Interpreter::initDebugShell(Command &root)
{
    initCommons(root);

    //
    // Debug variables
    //

    root.add({"debug"}, "Debug variables");

    root.add({"debug", ""}, {},
             "Display all debug variables",
             [this](Arguments& argv, long value) {

        retroShell.dump(emulator, Category::Debug);
    });

    if (debugBuild) {

        for (isize i = DebugFlagEnum::minVal; i < DebugFlagEnum::maxVal; i++) {

            root.add({"debug", DebugFlagEnum::key(i)}, { Arg::boolean },
                     DebugFlagEnum::help(i),
                     [this](Arguments& argv, long value) {

                c64.setDebugVariable(value, util::parseBool(argv[0]));

            }, i);
        }
    }


    //
    // Program execution
    //

    root.pushGroup("Program execution");

    root.add({"goto"}, { }, { Arg::value },
             std::pair <string, string>("g[oto]", "Goto address"),
             [this](Arguments& argv, long value) {

        argv.empty() ? emulator.run() : cpu.jump(parseAddr(argv[0]));
    });
    root.clone("g", {"goto"});

    root.add({"step"}, { }, { },
             std::pair <string, string>("s[tep]", "Step into the next instruction"),
             [this](Arguments& argv, long value) {

        emulator.stepInto();
    });
    root.clone("s", {"step"});

    root.add({"next"}, { }, { },
             std::pair <string, string>("n[next]", "Step over the next instruction"),
             [this](Arguments& argv, long value) {

        emulator.stepOver();
    });
    root.clone("n", {"next"});

    root.add({"break"},     "Manage CPU breakpoints");
    root.pushGroup("");

    root.add({"break", ""},
             "List all breakpoints",
             [this](Arguments& argv, long value) {

        retroShell.dump(cpu, Category::Breakpoints);
    });

    root.add({"break", "at"}, { Arg::address }, { Arg::ignores },
             "Set a breakpoint",
             [this](Arguments& argv, long value) {

        cpu.setBreakpoint(parseAddr(argv[0]), parseNum(argv, 1, 0));
    });

    root.add({"break", "delete"}, { Arg::nr },
             "Delete breakpoints",
             [this](Arguments& argv, long value) {

        cpu.deleteBreakpoint(parseNum(argv[0]));
    });

    root.add({"break", "toggle"}, { Arg::nr },
             "Enable or disable breakpoints",
             [this](Arguments& argv, long value) {

        cpu.toggleBreakpoint(parseNum(argv[0]));
    });

    root.popGroup();

    root.add({"watch"},     "Manage CPU watchpoints");

    root.pushGroup("");

    root.add({"watch", ""},
             "List all watchpoints",
             [this](Arguments& argv, long value) {

        retroShell.dump(cpu, Category::Watchpoints);
    });

    root.add({"watch", "at"}, { Arg::address }, { Arg::ignores },
             "Set a watchpoint",
             [this](Arguments& argv, long value) {

        cpu.setWatchpoint(parseAddr(argv[0]), parseNum(argv, 1, 0));
    });

    root.add({"watch", "delete"}, { Arg::nr },
             "Delete watchpoints",
             [this](Arguments& argv, long value) {

        cpu.deleteWatchpoint(parseNum(argv[0]));
    });

    root.add({"watch", "toggle"}, { Arg::nr },
             "Enable or disable watchpoints",
             [this](Arguments& argv, long value) {

        cpu.toggleWatchpoint(parseNum(argv[0]));
    });

    root.popGroup();


    //
    // Monitoring
    //

    root.pushGroup("Monitoring");

    root.add({"d"}, { }, { Arg::address },
             "Disassemble instructions",
             [this](Arguments& argv, long value) {

        std::stringstream ss;
        cpu.disassembler.disassembleRange(ss, parseAddr(argv, 0, cpu.getPC0()), 16);
        retroShell << '\n' << ss << '\n';
    });

    root.add({"a"}, { }, { Arg::address },
             "Dump memory in ASCII",
             [this](Arguments& argv, long value) {

        std::stringstream ss;
        debugger.ascDump(ss, parseAddr(argv, 0, debugger.current), 16);
        retroShell << '\n' << ss << '\n';
    });

    root.add({"m"}, { }, { Arg::address },
             std::pair<string, string>("m", "Dump memory"),
             [this](Arguments& argv, long value) {

        std::stringstream ss;
        debugger.memDump(ss, parseAddr(argv, 0, debugger.current), 16);
        retroShell << '\n' << ss << '\n';
    });

    root.add({"w"}, { Arg::value }, { Arg::address },
             std::pair<string, string>("w", "Write into memory"),
             [this](Arguments& argv, long value) {

        u16 addr = debugger.current;
        if (argv.size() > 1) { addr = parseAddr(argv[1]); }
        debugger.write(addr, u8(parseNum(argv[0])));
    });

    root.add({"c"}, { Arg::src, Arg::dst, Arg::count },
             std::pair<string, string>("c", "Copy a chunk of memory"),
             [this](Arguments& argv, long value) {

        debugger.copy(u16(parseNum(argv[0])), u16(parseNum(argv[1])), parseNum(argv[2]));
    });

    root.add({"f"}, { Arg::sequence }, { Arg::address },
             std::pair<string, string>("f", "Find a sequence in memory"),
             [this](Arguments& argv, long value) {

        {   SUSPENDED

            auto pattern = parseSeq(argv[0]);
            auto addr = parseAddr(argv, 1, debugger.current);
            auto found = debugger.memSearch(pattern, addr);

            if (found >= 0) {

                std::stringstream ss;
                debugger.memDump(ss, u16(found), 16);
                retroShell << ss;

            } else {

                std::stringstream ss;
                ss << "Not found";
                retroShell << ss;
            }
        }
    });

    root.add({"e"}, { Arg::address, Arg::count }, { Arg::value },
             std::pair<string, string>("e", "Erase memory"),
             [this](Arguments& argv, long value) {

        {   SUSPENDED

            auto addr = parseAddr(argv[0]);
            auto cnt = parseNum(argv[1]);
            auto val = u8(parseNum(argv, 2, 0));

            debugger.write(addr, val, cnt);
        }
    });

    root.add({"i"},
             "Inspect a component");

    root.pushGroup("Components");

    root.add({"i", "thread"},       "Emulator thread");

    root.pushGroup("");

    root.add({"i", "thread", ""},        "Displays the thread state",
             [this](Arguments& argv, long value) {

        retroShell.dump(emulator, Category::State);
    });

    root.add({"i", "thread", "runahead"},    "Inspects the run-ahead instance",
             [this](Arguments& argv, long value) {

        retroShell.dump(emulator, Category::RunAhead);
    });

    root.popGroup();

    root.add({"i", "c64"},          "C64",
             [this](Arguments& argv, long value) {

        retroShell.dump(c64, { Category::Config, Category::State });
    });

    root.add({"i", "memory"},       "Memory",
             [this](Arguments& argv, long value) {

        retroShell.dump(mem, { Category::Config, Category::State });
    });

    root.add({"i", "cia1"},         "CIA1",
             [this](Arguments& argv, long value) {

        retroShell.dump(cia1, { Category::Config, Category::State });
    });

    root.add({"i", "cia2"},         "CIA2",
             [this](Arguments& argv, long value) {

        retroShell.dump(cia2, { Category::Config, Category::State });
    });

    root.add({"i", "vicii"},        "VICII",
             [this](Arguments& argv, long value) {

        retroShell.dump(cia2, { Category::Config, Category::State });
    });

    root.add({"i", "sid"},          "Primary SID",
             [this](Arguments& argv, long value) {

        retroShell.dump(muxer.sid[0], { Category::Config, Category::State });
    });

    root.add({"i", "muxer"},        "Audio backend",
             [this](Arguments& argv, long value) {

        retroShell.dump(muxer, { Category::Config, Category::State });
    });

    root.add({"i", "expansion"},    "Expansion port",
             [this](Arguments& argv, long value) {

        retroShell.dump(expansionport, { Category::Config, Category::State });
    });

    root.popGroup();

    root.pushGroup("Peripherals");

    root.add({"i", "keyboard"},     "Keyboard",
             [this](Arguments& argv, long value) {

        retroShell.dump(keyboard, { Category::Config, Category::State });
    });

    root.add({"i", "port1"},       "Control Port 1",
             [this](Arguments& argv, long value) {

        retroShell.dump(port1, { Category::Config, Category::State });
    });

    root.add({"i", "port2"},       "Control Port 2",
             [this](Arguments& argv, long value) {

        retroShell.dump(port2, { Category::Config, Category::State });
    });

    root.add({"i", "joystick1"},    "Joystick (port 1)",
             [this](Arguments& argv, long value) {

        retroShell.dump(port1.joystick, { Category::Config, Category::State });
    });

    root.add({"i", "joystick2"},    "Joystick (port 2)",
             [this](Arguments& argv, long value) {

        retroShell.dump(port2.joystick, { Category::Config, Category::State });
    });

    root.add({"i", "mouse1"},       "Mouse (port 1)",
             [this](Arguments& argv, long value) {

        retroShell.dump(port1.mouse, { Category::Config, Category::State });
    });

    root.add({"i", "mouse2"},       "Mouse (port 2)",
             [this](Arguments& argv, long value) {

        retroShell.dump(port2.mouse, { Category::Config, Category::State });
    });

    root.add({"i", "drive8"},       "Floppy Drive 8");
    root.add({"i", "drive9"},       "Floppy Drive 9");

    root.pushGroup("");

    for (isize i = 0; i < 2; i++) {

        string drive = (i == 0) ? "drive8" : "drive9";

        root.add({"i", drive, ""},
                 "Inspects the internal state",
                 [this](Arguments& argv, long value) {

            auto &drive = value ? drive9 : drive8;
            retroShell.dump(drive, { Category::Config, Category::State });
        });

        root.add({"i", drive, "bankmap"},
                 "Displays the memory layout",
                 [this](Arguments& argv, long value) {

            auto &drive = value ? drive9 : drive8;
            retroShell.dump(drive, Category::BankMap);
        });

        root.add({"i", drive, "disk"},
                 "Inspects the current disk",
                 [this](Arguments& argv, long value) {

            auto &drive = value ? drive9 : drive8;
            retroShell.dump(drive, Category::Disk);
        });

        root.add({"i", drive, "layout"},
                 "Displays the disk layout",
                 [this](Arguments& argv, long value) {

            auto &drive = value ? drive9 : drive8;
            retroShell.dump(drive, Category::Layout);
        });
    }

    root.popGroup();

    root.add({"i", "iec"},          "IEC bus",
             [this](Arguments& argv, long value) {

        retroShell.dump(iec, { Category::Config, Category::State });
    });

    root.add({"i", "datasette"},    "Datasette",
             [this](Arguments& argv, long value) {

        retroShell.dump(datasette, { Category::Config, Category::State });
    });

    root.popGroup();

    root.pushGroup("Miscellaneous");

    root.add({"i", "host"},         "Host computer",
             [this](Arguments& argv, long value) {

        retroShell.dump(host, { Category::Config, Category::State });
    });

    root.popGroup();

    root.add({"r"},
             "Show registers");

    root.add({"r", "cia1"},         "CIA1",
             [this](Arguments& argv, long value) {

        retroShell.dump(cia1, Category::Registers);
    });

    root.add({"r", "cia2"},         "CIA2",
             [this](Arguments& argv, long value) {

        retroShell.dump(cia2, Category::Registers);
    });

    root.add({"r", "vicii"},        "VICII",
             [this](Arguments& argv, long value) {

        retroShell.dump(cia2, Category::Registers);
    });

    root.add({"r", "sid"},          "Primary SID",
             [this](Arguments& argv, long value) {

        retroShell.dump(muxer.sid[0], Category::Registers);
    });

    root.popGroup();


    //
    // Miscellaneous (Recorder)
    //

    root.pushGroup("Miscellaneous");

    root.add({"checksums"},
             "Displays checksum of various components",
             [this](Arguments& argv, long value) {

        retroShell.dump(c64, Category::Checksums);
    });

    root.add({"sizeof"},
             "Displays static memory footprints of various components",
             [this](Arguments& argv, long value) {

        retroShell.dump(c64, Category::Sizeof);
    });

    root.popGroup();
}

}
