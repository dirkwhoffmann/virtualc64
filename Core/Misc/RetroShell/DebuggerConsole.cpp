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

namespace vc64 {

void
DebuggerConsole::_pause()
{
    if (retroShell.inDebugShell()) {
        
        *this << '\n';
    exec("state");
    *this << getPrompt();
    }
}

string
DebuggerConsole::getPrompt()
{
    std::stringstream ss;
    
    ss << "(";
    ss << std::right << std::setw(0) << std::dec << isize(c64.scanline);
    ss << ",";
    ss << std::right << std::setw(0) << std::dec << isize(c64.rasterCycle);
    ss << ") $";
    ss << std::right << std::setw(4) << std::hex << isize(cpu.getPC0());
    ss << ": ";
    
    return ss.str();
}

void
DebuggerConsole::welcome()
{
    Console::welcome();
}

void
DebuggerConsole::summary()
{
    std::stringstream ss;

    // ss << "RetroShell Debugger" << std::endl << std::endl;
    c64.dump(Category::Current, ss);

    *this << vspace{1};
    string line;
    while(std::getline(ss, line)) { *this << "    " << line << '\n'; }
    // *this << ss;
    *this << vspace{1};
}

void
DebuggerConsole::printHelp(isize tab)
{
    Console::printHelp(tab);
}

void
DebuggerConsole::pressReturn(bool shift)
{
    if (emulator.isPaused() && !shift && input.empty()) {
        
        emulator.stepInto();
        
    } else {
        
        Console::pressReturn(shift);
    }
}

void
DebuggerConsole::initCommands(RSCommand &root)
{
    Console::initCommands(root);
    
    //
    // Program execution
    //
    
    RSCommand::currentGroup = "Program execution";

    root.add({ .tokens = { "p[ause]" }, .ghelp  = { "Pause emulation" }, .chelp  = { "p or pause" } });

    root.add({

        .tokens = { "pause" },
        .chelp  = { "Pause emulation" },
        .flags  = rs::shadowed,
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            if (emulator.isRunning()) emulator.put(Cmd::PAUSE);
        }
    });

    root.clone({ "pause" }, "p");

    root.add({ .tokens = { "g[oto]" }, .ghelp  = { "Goto address" }, .chelp  = { "g or goto" } });
    root.add({

        .tokens = { "goto" },
        .chelp  = { "Goto address" },
        .flags  = rs::shadowed,
        .args   = { { .name = { "address", "Memory address" }, .flags = rs::opt } },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            args.contains("address") ? cpu.jump(parseAddr(args.at("address"))) : emulator.run();
        }
    });
    root.clone({ "goto" }, "g");

    root.add({ .tokens = { "s[tep]" }, .ghelp  = { "Step into the next instruction" }, .chelp  = { "s or step" } });
    root.add({

        .tokens = { "step" },
        .chelp  = { "Step into the next instruction" },
        .flags  = rs::shadowed,
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            emulator.stepInto();
        }
    });
    root.clone({ "step" }, "s");

    root.add({ .tokens = { "n[next]" }, .ghelp  = { "Step over the next instruction" }, .chelp  = { "n or next" } });
    root.add({

        .tokens = { "next" },
        .flags  = rs::shadowed,
        .chelp  = { "Step over the next instruction" },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            emulator.stepOver();
        }
    });
    root.clone({ "next" }, "n");

    root.add({

        .tokens = { "eol" },
        .chelp  = { "Complete the current line" },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            emulator.finishLine();
        }
    });

    root.add({

        .tokens = { "eof" },
        .chelp  = { "Complete the current frame" },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            emulator.finishFrame();
        }
    });

    
    //
    // Breakpoints
    //

    root.add({

        .tokens = { "break" },
        .ghelp  = { "Manage CPU breakpoints" },
        .chelp  = { "List all breakpoints" },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            dump(os, cpu, Category::Breakpoints);
        }
    });

    root.add({

        .tokens = { "break", "at" },
        .chelp  = { "Set a breakpoint" },
        .args   = {
            { .name = { "address", "Memory address" } },
            { .name = { "ignores", "Ignore count" }, .flags = rs::opt }
        },
            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                auto addr = parseAddr(args.at("address"));
                cpu.setBreakpoint(addr, parseNum(args, "ignores", 0));
            }
    });

    root.add({

        .tokens = { "break", "delete" },
        .chelp  = { "Delete breakpoints" },
        .args   = { { .name = { "nr", "Breakpoint number" } } },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            cpu.deleteBreakpoint(parseNum(args.at("nr")));
        }
    });

    root.add({

        .tokens = { "break", "toggle" },
        .chelp  = { "Enable or disable breakpoints" },
        .args   = { { .name = { "nr", "Breakpoint number" } } },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            cpu.toggleBreakpoint(parseNum(args.at("nr")));
        }
    });


    //
    // Watchpoints
    //
    
    root.add({

        .tokens = { "watch" },
        .ghelp  = { "Manage CPU watchpoints" },
        .chelp  = { "Lists all watchpoints" },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            dump(os, cpu, Category::Watchpoints);
        }
    });

    root.add({

        .tokens = { "watch", "at" },
        .chelp  = { "Set a watchpoint at the specified address" },
        .args   = {
            { .name = { "address", "Memory address" } },
            { .name = { "ignores", "Ignore count" }, .flags = rs::opt }
        },
            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                auto addr = parseAddr(args, "address");
                cpu.setWatchpoint(addr, parseNum(args, "ignores", 0));
            }
    });

    root.add({

        .tokens = { "watch", "delete" },
        .chelp  = { "Delete a watchpoint" },
        .args   = { { .name = { "nr", "Watchpoint number" } } },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            cpu.deleteWatchpoint(parseNum(args, "nr"));
        }
    });

    root.add({

        .tokens = { "watch", "toggle" },
        .chelp  = { "Enable or disable a watchpoint" },
        .args   = { { .name = { "nr", "Watchpoint number" } } },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            cpu.toggleWatchpoint(parseNum(args, "nr"));
        }
    });


    //
    // Monitoring
    //
    
    RSCommand::currentGroup = "Monitoring";

    root.add({

        .tokens = { "d" },
        .chelp  = { "Disassemble instructions" },
        .args   = { { .name = { "address", "Memory address" }, .flags = rs::opt } },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            std::stringstream ss;
            cpu.disassembler.disassembleRange(ss, parseAddr(args, "address", cpu.getPC0()), 16);
            retroShell << '\n' << ss << '\n';
        }
    });

    root.add({

        .tokens = { "a" },
        .chelp  = { "Dump memory in ASCII" },
        .args   = { { .name = { "address", "Memory address" }, .flags = rs::opt } },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            if (args.contains("address")) { current = parseAddr(args, "address"); }

            std::stringstream ss;
            current += (u16)mem.debugger.ascDump(ss, current, 16);
            retroShell << '\n' << ss << '\n';
        }
    });

    root.add({

        .tokens = { "m" },
        .chelp  = { "Dump memory" },
        .args   = { { .name = { "address", "Memory address" }, .flags = rs::opt } },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            if (args.contains("address")) { current = parseAddr(args, "address"); }

            std::stringstream ss;
            current += (u16)mem.debugger.memDump(ss, current, 16);
            retroShell << '\n' << ss << '\n';
        }
    });

    root.add({

        .tokens = { "w" },
        .chelp  = { "Write into memory" },
        .args   = {
            { .name = { "value", "Payload" } },
            { .name = { "target", "Memory address" }, .flags = rs::opt } },

            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                auto addr = current;

                if (args.contains("target")) {
                    addr = parseAddr(args, "target");
                }

                mem.debugger.write(addr, u8(parseNum(args.at("value"))));
                current = addr + 1;
            }
    });

    root.add({

        .tokens = { "c" },
        .chelp  = { "Copy a chunk of memory" },
        .args   = {
            { .name = { "src", "Source address" }, .flags = rs::keyval },
            { .name = { "dest", "Destination address" }, .flags = rs::keyval },
            { .name = { "count", "Number of bytes" }, .flags = rs::keyval } },

            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                auto src = parseNum(args.at("src"));
                auto dst = parseNum(args.at("dest"));
                auto cnt = parseNum(args.at("count"));

                if (src < dst) {

                    for (isize i = cnt - 1; i >= 0; i--)
                        mem.poke(u16(dst + i), mem.spypeek(u16(src + i)));

                } else {

                    for (isize i = 0; i <= cnt - 1; i++)
                        mem.poke(u16(dst + i), mem.spypeek(u16(src + i)));
                }
            }
    });

    root.add({

        .tokens = { "f" },
        .chelp  = { "Find a sequence in memory" },
        .args   = {
            { .name = { "sequence", "Search string" } },
            { .name = { "address", "Start address" }, .flags = rs::opt } },

            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                auto pattern = parseSeq(args.at("sequence"));
                auto addr = u16(parseNum(args, "address", current));
                auto found = mem.debugger.memSearch(pattern, addr);

                if (found >= 0) {

                    std::stringstream ss;
                    mem.debugger.memDump(ss, u16(found), 1);
                    retroShell << ss;
                    current = u16(found);

                } else {

                    std::stringstream ss;
                    ss << "Not found";
                    retroShell << ss;
                }
            }
    });

    root.add({

        .tokens = { "e" },
        .chelp  = { "Erase memory" },
        .args   = {
            { .name = { "address", "Start address" } },
            { .name = { "count", "Number of bytes to erase" } },
            { .name = { "value", "Replacement value" }, .flags = rs::opt } },

            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                auto addr = parseAddr(args.at("address"));
                auto count = parseNum(args, "count");
                auto val = u8(parseNum(args, "value", 0));

                mem.debugger.write(addr, val, count);
            }
    });

    root.add({

        .tokens = { "r" },
        .ghelp  = { "Show registers" }
    });

    root.add({

        .tokens = { "r", "cia1" },
        .chelp  = { "Complex Interface Adapter 1" },

        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            dump(os, cia1, Category::Registers);
        }
    });

    root.add({

        .tokens = { "r", "cia2" },
        .chelp  = { "Complex Interface Adapter 2" },

        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            dump(os, cia2, Category::Registers);
        }
    });

    root.add({

        .tokens = { "r", "vicii" },
        .chelp  = { "Video Interface Controller" },

        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            dump(os, vic, Category::Registers);
        }
    });

    root.add({

        .tokens = { "r", "sid" },
        .chelp  = { "Sound Interface Device" },

        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            dump(os, sid0, Category::Registers);
        }
    });

    
    //
    // Components
    //

    root.add({

        .tokens = { "?" },
        .ghelp  = { "Inspect a component" }
    });

    RSCommand::currentGroup = "Components";

    root.add({

        .tokens = { "?", "c64" },
        .chelp  = { "Inspects the internal state" },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            dump(os, c64, Category::State);
        }
    });

    root.add({

        .tokens = { "?", "memory" },
        .ghelp  = { "Memory" },
        .chelp  = { "Inspects the internal state" },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            dump(os, mem, Category::State);
        }
    });

    root.add({

        .tokens = { "?", "cpu" },
        .ghelp  = { "Central Processing Unit" },
        .chelp  = { "Inspect the internal state" },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            dump(os, cpu, Category::State );
        }
    });

    for (isize i = 0; i < 2; i++) {

        string cia = (i == 0) ? "cia1" : "cia2";
        root.add({

            .tokens = { "?", cia },
            .ghelp  = { "Complex Interface Adapter" },
            .chelp  = { "Inspect the internal state" },
            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                if (values[0] == 0) {
                    dump(os, cia1, Category::State );
                } else {
                    dump(os, cia1, Category::State );
                }
            }, .payload = {i}
        });

        root.add({

            .tokens = { "?", cia, "tod" },
            .chelp  = { "Display the state of the time-of-day clock" },
            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                if (values[0] == 0) {
                    dump(os, cia1.tod, Category::State );
                } else {
                    dump(os, cia2.tod, Category::State );
                }
            }, .payload = {i}
        });
    }

    root.add({

        .tokens = { "?", "vic" },
        .ghelp  = { "Video Interface Controller" },
        .chelp  = { "Inspect the internal state" },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            dump(os, vic, Category::State );
        }
    });

    root.add({

        .tokens = { "?", "sid" },
        .ghelp  = { "Sound Interface Device" },
        .chelp  = { "Inspect the internal state" },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            dump(os, sid0, Category::State );
        }
    });

    root.add({

        .tokens = { "?", "sidbridge" },
        .ghelp  = { "SID bridge" },
        .chelp  = { "Inspect the internal state" },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            dump(os, sidBridge, Category::State );
        }
    });

    root.add({

        .tokens = { "?", "expansion" },
        .ghelp  = { "Expansion port" },
        .chelp  = { "Inspect the internal state" },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            dump(os, expansionPort, Category::State );
        }
    });

    RSCommand::currentGroup = "Miscellaneous";

    root.add({

        .tokens = { "?", "thread" },
        .ghelp  = { "Emulator thread" },
        .chelp  = { "Display information about the thread state" },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            dump(os, emulator, Category::State);
        }
    });

    root.add({

        .tokens = { "?", "thread", "runahead" },
        .chelp  = { "Run-ahead instance" },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            dump(os, emulator, Category::RunAhead);
        }
    });


    //
    // Peripherals
    //
    
    RSCommand::currentGroup = "Peripherals";

    root.add({

        .tokens = { "?", "keyboard" },
        .chelp  = { "Keyboard" },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            dump(os, keyboard, Category::RunAhead);
        }
    });

    root.add({

        .tokens = { "?", "port" },
        .chelp  = { "Control port" }
    });

    for (isize i = 1; i <= 2; i++) {

        string nr = (i == 1) ? "1" : "2";

        root.add({

            .tokens = { "?", "port", nr },
            .ghelp  = { "Control port " + nr },
            .chelp  = { "Inspect the internal state" },
            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                if (values[0] == 1) dump(os, port1, Category::State);
                if (values[0] == 2) dump(os, port2, Category::State);
            }, .payload = {i}
        });
    }

    root.add({

        .tokens = { "?", "mouse" },
        .ghelp  = { "Mouse" }
    });

    for (isize i = 1; i <= 2; i++) {

        string nr = (i == 1) ? "1" : "2";

        root.add({

            .tokens = { "?", "mouse", nr },
            .ghelp  = { "Mouse in port " + nr },
            .chelp  = { "Inspect the internal state" },
            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                if (values[0] == 1) dump(os, port1.mouse, Category::State );
                if (values[0] == 2) dump(os, port2.mouse, Category::State );
            }, .payload = {i}
        });
    }

    root.add({

        .tokens = { "?", "joystick" },
        .chelp  = { "Joystick" }
    });

    for (isize i = 1; i <= 2; i++) {

        string nr = (i == 1) ? "1" : "2";

        root.add({

            .tokens = { "?", "joystick", nr },
            .ghelp  = { "Joystick in port " + nr },
            .chelp  = { "Inspect the internal state" },
            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                if (values[0] == 1) dump(os, port1.joystick, Category::State);
                if (values[0] == 2) dump(os, port2.joystick, Category::State);
            }, .payload = {i}
        });
    }

    root.add({

        .tokens = { "?", "drive[n]" },
        .ghelp  = { "Floppy drive" },
        .chelp  = { "? drive8, ? drive9" }
    });

    for (isize i = 0; i < 2; i++) {

        string df = "drive" + std::to_string(i + 8);

        root.add({

            .tokens = { "?", df },
            .ghelp  = { "Floppy drive n" },
            .chelp  = { "Inspect the internal state" },
            .flags  = rs::shadowed,
            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                auto *df = drive[values[0]];
                dump(os, *df, Category::State);

            }, .payload = {i}
        });

        root.add({

            .tokens = { "?", df, "bankmap" },
            .chelp  = { "Displays the memory layout" },
            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                auto *df = drive[values[0]];
                dump(os, *df, Category::BankMap);

            }, .payload = {i}
        });

        root.add({

            .tokens = { "?", df, "disk" },
            .chelp  = { "Inspect the inserted disk" },
            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                auto *df = drive[values[0]];
                dump(os, *df, Category::Disk);

            }, .payload = {i}
        });

        root.add({

            .tokens = { "?", df, "layout" },
            .chelp  = { "Displays the disk layout" },
            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                auto *df = drive[values[0]];
                dump(os, *df, Category::Layout);

            }, .payload = {i}
        });
    }

    root.add({

        .tokens = { "?", "serial" },
        .chelp  = { "Serial Port" },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            dump(os, serialPort, Category::State);
        }
    });

    root.add({

        .tokens = { "?", "datasette" },
        .chelp  = { "Datasette" },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            dump(os, datasette, Category::State);
        }
    });

    root.add({

        .tokens = { "?", "audio" },
        .chelp  = { "Audio Port" },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            dump(os, audioPort, Category::State);
        }
    });

    root.add({

        .tokens = { "?", "host" },
        .chelp  = { "Host computer" },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            dump(os, host, Category::State);
        }
    });

    
    //
    // Miscellaneous
    //
    
    RSCommand::currentGroup = "Miscellaneous";
    
    root.add({
        
        .tokens = { "checksums" },
        .chelp  = { "Displays checksum of various components" },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            dump(os, c64, Category::Checksums);
        }
    });

    root.add({

        .tokens = { "debug" },
        .ghelp  = { "Debug variables" },
        .chelp  = { "Display all debug variables" },

        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            dump(os, emulator, Category::Debug);
        }
    });

    if (debugBuild) {

        for (auto i : DebugFlagEnum::elements()) {

            root.add({

                .tokens = { "debug", DebugFlagEnum::key(i) },
                .chelp  = { DebugFlagEnum::help(i) },
                .args   = {
                    { .name = { "level", "Debug level" } }
                },
                    .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                        Emulator::setDebugVariable(DebugFlag(values[0]), int(parseNum(args, "level")));

                    }, .payload = { isize(i) }
            });
        }

        root.add({

            .tokens = { "debug", "verbosity" },
            .chelp  = { "Set the verbosity level for generated debug output" },
            .args   = {
                { .name = { "level", "Verbosity level" } }
            },
                .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                    CoreObject::verbosity = isize(parseNum(args, "level"));
                }
        });
    }

    root.add({

        .tokens = {"%"},
        .chelp  = { "Convert a value into different formats" },
        .args   = {
            { .name = { "value", "Payload" } }
        },
            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                std::stringstream ss;
                auto value = args.at("value");

                if (isNum(value)) {
                    mem.debugger.convertNumeric(ss, (u32)parseNum(value));
                } else {
                    mem.debugger.convertNumeric(ss, value);
                }

                retroShell << '\n' << ss << '\n';
            }
    });
}

}
