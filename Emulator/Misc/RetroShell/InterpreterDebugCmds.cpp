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
#include "IOUtils.h"
#include "Parser.h"

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

                c64.setDebugVariable(DebugFlag(value), util::parseBool(argv[0]));

            }, i);
        }
    }


    //
    // Program execution
    //

    Command::currentGroup = "Program execution";

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

    root.add({"watch"},     "Manage CPU watchpoints");

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


    //
    // Monitoring
    //

    Command::currentGroup = "Monitoring";

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
    });

    root.add({"e"}, { Arg::address, Arg::count }, { Arg::value },
             std::pair<string, string>("e", "Erase memory"),
             [this](Arguments& argv, long value) {

        auto addr = parseAddr(argv[0]);
        auto cnt = parseNum(argv[1]);
        auto val = u8(parseNum(argv, 2, 0));
        
        debugger.write(addr, val, cnt);
    });

    root.add({"i"},
             "Inspect a component");

    Command::currentGroup = "Components";

    root.add({"i", "thread"},       "Emulator thread");

    root.add({"i", "thread", ""},        "Displays the thread state",
             [this](Arguments& argv, long value) {

        retroShell.dump(emulator, Category::State);
    });

    root.add({"i", "thread", "runahead"},    "Inspects the run-ahead instance",
             [this](Arguments& argv, long value) {

        retroShell.dump(emulator, Category::RunAhead);
    });

    auto cmd = c64.shellName();
    auto description = c64.description();
    root.add({"i", cmd}, description, [this](Arguments& argv, long value) {

        retroShell.dump(c64, { Category::Config, Category::State });
    });

    cmd = mem.shellName();
    description = mem.description();
    root.add({"i", cmd}, description, [this](Arguments& argv, long value) {

        retroShell.dump(mem, { Category::Config, Category::State });
    });

    cmd = cia1.shellName();
    description = cia1.description();
    root.add({"i", cmd}, description, [this](Arguments& argv, long value) {

        retroShell.dump(cia1, { Category::Config, Category::State });
    });

    cmd = cia2.shellName();
    description = cia2.description();
    root.add({"i", cmd}, description, [this](Arguments& argv, long value) {

        retroShell.dump(cia2, { Category::Config, Category::State });
    });

    cmd = vic.shellName();
    description = vic.description();
    root.add({"i", cmd}, description, [this](Arguments& argv, long value) {

        retroShell.dump(vic, { Category::Config, Category::State });
    });

    root.add({"i", "sid"}, { }, { Arg::value }, "Primary SID",
             [this](Arguments& argv, long value) {

        isize nr = parseNum(argv, 0, 0);
        if (nr < 0 || nr > 3) throw Error(VC64ERROR_OPT_INV_ARG, "0 ... 3");

        retroShell.dump(sidBridge.sid[nr], { Category::Config, Category::State });
    });

    cmd = sidBridge.shellName();
    description = sidBridge.description();
    root.add({"i", cmd}, description, [this](Arguments& argv, long value) {

        retroShell.dump(sidBridge, { Category::Config, Category::State });
    });

    cmd = expansionPort.shellName();
    description = expansionPort.description();
    root.add({"i", cmd}, description, [this](Arguments& argv, long value) {

        retroShell.dump(expansionPort, { Category::Config, Category::State });
    });

    Command::currentGroup = "Peripherals";

    cmd = keyboard.shellName();
    description = keyboard.description();
    root.add({"i", cmd}, description, [this](Arguments& argv, long value) {

        retroShell.dump(keyboard, { Category::Config, Category::State });
    });

    cmd = port1.shellName();
    description = port1.description();
    root.add({"i", cmd}, description, [this](Arguments& argv, long value) {

        retroShell.dump(port1, { Category::Config, Category::State });
    });

    cmd = port2.shellName();
    description = port2.description();
    root.add({"i", cmd}, description, [this](Arguments& argv, long value) {

        retroShell.dump(port2, { Category::Config, Category::State });
    });

    cmd = port1.joystick.shellName();
    description = port1.joystick.description();
    root.add({"i", cmd}, description, [this](Arguments& argv, long value) {

        retroShell.dump(port1.joystick, { Category::Config, Category::State });
    });

    cmd = port2.joystick.shellName();
    description = port2.joystick.description();
    root.add({"i", cmd}, description, [this](Arguments& argv, long value) {

        retroShell.dump(port2.joystick, { Category::Config, Category::State });
    });

    cmd = port1.mouse.shellName();
    description = port1.mouse.description();
    root.add({"i", cmd}, description, [this](Arguments& argv, long value) {

        retroShell.dump(port1.mouse, { Category::Config, Category::State });
    });

    cmd = port2.mouse.shellName();
    description = port2.mouse.description();
    root.add({"i", cmd}, description, [this](Arguments& argv, long value) {

        retroShell.dump(port2.mouse, { Category::Config, Category::State });
    });

    for (isize i = 0; i < 2; i++) {

        auto &drive = i == 0 ? c64.drive8 : c64.drive9;

        cmd = drive.shellName();
        description = drive.description();
        root.add({cmd}, description);

        root.add({"i", cmd}, description);

        root.add({"i", cmd, ""},
                 "Inspects the internal state",
                 [this](Arguments& argv, long value) {

            auto &drive = value ? drive9 : drive8;
            retroShell.dump(drive, { Category::Config, Category::State });
        }, i);

        root.add({"i", cmd, "bankmap"},
                 "Displays the memory layout",
                 [this](Arguments& argv, long value) {

            auto &drive = value ? drive9 : drive8;
            retroShell.dump(drive, Category::BankMap);
        }, i);

        root.add({"i", cmd, "disk"},
                 "Inspects the current disk",
                 [this](Arguments& argv, long value) {

            auto &drive = value ? drive9 : drive8;
            retroShell.dump(drive, Category::Disk);
        }, i);

        root.add({"i", cmd, "layout"},
                 "Displays the disk layout",
                 [this](Arguments& argv, long value) {

            auto &drive = value ? drive9 : drive8;
            retroShell.dump(drive, Category::Layout);
        }, i);
    }

    cmd = serialPort.shellName();
    description = serialPort.description();
    root.add({"i", cmd}, description, [this](Arguments& argv, long value) {

        retroShell.dump(serialPort, { Category::Config, Category::State });
    });

    cmd = datasette.shellName();
    description = datasette.description();
    root.add({"i", cmd}, description, [this](Arguments& argv, long value) {

        retroShell.dump(datasette, { Category::Config, Category::State });
    });

    cmd = audioPort.shellName();
    description = audioPort.description();
    root.add({"i", cmd}, description, [this](Arguments& argv, long value) {

        retroShell.dump(audioPort, { Category::Config, Category::State });
    });

    Command::currentGroup = "Miscellaneous";

    cmd = host.shellName();
    description = host.description();
    root.add({"i", cmd}, description, [this](Arguments& argv, long value) {

        retroShell.dump(host, { Category::Config, Category::State });
    });

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

        retroShell.dump(sidBridge.sid[0], Category::Registers);
    });


    //
    // Miscellaneous (Recorder)
    //

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
}

}
