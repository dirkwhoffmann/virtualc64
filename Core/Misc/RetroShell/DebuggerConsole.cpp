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
DebuggerConsole::_enter()
{
    msgQueue.put(Msg::RSH_DEBUGGER, true);

    // If the console is entered the first time...
    if (isEmpty()) {

        // Print the welcome message
        exec("welcome");
        *this << getPrompt();
    }
}

void
DebuggerConsole::_pause()
{
    *this << '\n' << '\n';
    exec("state");
    *this << getPrompt();
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
    printHelp();
    *this << '\n';
}

void
DebuggerConsole::printHelp()
{
    storage << "Type 'help' or press 'TAB' twice for help.\n";
    storage << "Type '.' or press 'SHIFT+RETURN' to exit debug mode.";

    remoteManager.rshServer << "Type 'help' for help.\n";
    remoteManager.rshServer << "Type '.' to exit debug mode.";

    *this << '\n';
}

void
DebuggerConsole::pressReturn(bool shift)
{
    if (!shift && input.empty()) {

        emulator.isRunning() ? emulator.put(Cmd::PAUSE) : emulator.stepInto();

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

    root.add({
        
        .tokens = { "goto" },
        .extra  = { Arg::value },
        .help   = { "Goto address", "g[oto]" },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            argv.empty() ? emulator.run() : cpu.jump(parseAddr(argv[0]));
        }
    });
    
    root.clone({ "goto" }, "g");

    root.add({
        
        .tokens = { "step" },
        .help   = { "Step into the next instruction", "s[tep]" },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            emulator.stepInto();
        }
    });
    
    root.clone({ "step" }, "s");

    root.add({
        
        .tokens = { "next" },
        .help   = { "Step over the next instruction", "n[next]" },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            emulator.stepOver();
        }
    });
    
    root.clone({ "next" }, "n");

    root.add({
        
        .tokens = { "eol" },
        .help   = { "Complete the current line" },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            emulator.finishLine();
        }
    });

    root.add({
        
        .tokens = { "eof" },
        .help   = { "Complete the current frame" },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            emulator.finishFrame();
        }
    });
    
    //
    // Breakpoints
    //
    
    root.add({
        
        .tokens = { "break" },
        .help   = { "Manage CPU breakpoints" }
    });
    
    root.add({
        
        .tokens = { "break", "" },
        .help   = { "List all breakpoints" },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            dump(cpu, Category::Breakpoints);
        }
    });
    
    root.add({
        
        .tokens = { "break", "at" },
        .args   = { Arg::address },
        .extra  = { Arg::ignores },
        .help   = { "Set a breakpoint" },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            cpu.setBreakpoint(parseAddr(argv[0]), parseNum(argv, 1, 0));
        }
    });
 
    root.add({
        
        .tokens = { "break", "delete" },
        .args   = { Arg::nr },
        .help   = { "Delete breakpoints" },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            cpu.deleteBreakpoint(parseNum(argv[0]));
        }
    });
    
    root.add({
        
        .tokens = { "break", "toggle" },
        .args   = { Arg::nr },
        .help   = { "Enable or disable breakpoints" },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            cpu.toggleBreakpoint(parseNum(argv[0]));
        }
    });
    
    //
    // Watchpoints
    //
    
    root.add({
        
        .tokens = { "watch" },
        .help   = { "Manage CPU watchpoints" }
    });
    
    root.add({
        
        .tokens = { "watch", "" },
        .help   = { "Lists all watchpoints" },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            dump(cpu, Category::Watchpoints);
        }
    });
 
    root.add({
        
        .tokens = { "watch", "at" },
        .args   = { Arg::address },
        .extra  = { Arg::ignores },
        .help   = { "Set a watchpoint at the specified address" },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            cpu.setWatchpoint(parseAddr(argv[0]), parseNum(argv, 1, 0));
        }
    });
    
    root.add({
        
        .tokens = { "watch", "delete" },
        .args   = { Arg::address },
        .help   = { "Delete a watchpoint" },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            cpu.deleteWatchpoint(parseNum(argv[0]));
        }
    });
    
    root.add({
        
        .tokens = { "watch", "toggle" },
        .args   = { Arg::address },
        .help   = { "Enable or disable a watchpoint" },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            cpu.toggleWatchpoint(parseNum(argv[0]));
        }
    });
    
    //
    // Monitoring
    //

    RSCommand::currentGroup = "Monitoring";

    root.add({
        
        .tokens = { "d" },
        .extra  = { Arg::address },
        .help   = { "Disassemble instructions" },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            std::stringstream ss;
            cpu.disassembler.disassembleRange(ss, parseAddr(argv, 0, cpu.getPC0()), 16);
            retroShell << '\n' << ss << '\n';
        }
    });
    
    root.add({
        
        .tokens = { "a" },
        .extra  = { Arg::address },
        .help   = { "Dump memory in ASCII" },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {

            std::stringstream ss;
            mem.debugger.ascDump(ss, parseAddr(argv, 0, mem.debugger.current), 16);
            retroShell << '\n' << ss << '\n';
        }
    });

    root.add({
        
        .tokens = {"m"},
        .extra  = { Arg::address },
        .help   = { "Dump memory" },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
                        
            std::stringstream ss;
            mem.debugger.memDump(ss, parseAddr(argv, 0, mem.debugger.current), 16);
            retroShell << '\n' << ss << '\n';
        }
    });

    root.add({
        
        .tokens = { "w" },
        .args   = { Arg::value },
        .extra  = { Arg::address },
        .help   = { "Write into memory" },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            u16 addr = mem.debugger.current;
            if (argv.size() > 1) { addr = parseAddr(argv[1]); }
            mem.debugger.write(addr, u8(parseNum(argv[0])));
        }
    });
    
    root.add({
        
        .tokens = { "c" },
        .args   = { Arg::src, Arg::dst, Arg::count },
        .help   = { "Copy a chunk of memory" },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            mem.debugger.copy(u16(parseNum(argv[0])), u16(parseNum(argv[1])), parseNum(argv[2]));
        }
    });
    
    root.add({
        
        .tokens = { "f" },
        .args   = { Arg::sequence },
        .extra  = { Arg::address },
        .help   = { "Find a sequence in memory" },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            auto pattern = parseSeq(argv[0]);
            auto addr = parseAddr(argv, 1, mem.debugger.current);
            auto found = mem.debugger.memSearch(pattern, addr);

            if (found >= 0) {

                std::stringstream ss;
                mem.debugger.memDump(ss, u16(found), 16);
                retroShell << ss;

            } else {

                std::stringstream ss;
                ss << "Not found";
                retroShell << ss;
            }
        }
    });
    
    root.add({
        
        .tokens = { "e" },
        .args   = { Arg::address, Arg::count },
        .extra  = { Arg::value },
        .help   = { "Erase memory" },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            auto addr = parseAddr(argv[0]);
            auto cnt = parseNum(argv[1]);
            auto val = u8(parseNum(argv, 2, 0));
            
            mem.debugger.write(addr, val, cnt);
        }
    });

    root.add({
        
        .tokens = { "r" },
        .help   = { "Show registers" }
    });
 
    root.add({
        
        .tokens = { "r", "cia1" },
        .help   = { "CIA1" },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            dump(cia1, Category::Registers);
        }
    });

    root.add({
        
        .tokens = { "r", "cia2" },
        .help   = { "CIA2" },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            dump(cia2, Category::Registers);
        }
    });

    root.add({
        
        .tokens = { "r", "vicii" },
        .help   = { "VICII" },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            dump(vic, Category::Registers);
        }
    });
 
    root.add({
        
        .tokens = { "r", "sid" },
        .help   = { "Primary SID" },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            dump(sidBridge.sid[0], Category::Registers);
        }
    });
    
    //
    // Components
    //
    
    RSCommand::currentGroup = "Components";

    root.add({
        
        .tokens = { "?" },
        .help   = { "Inspect a component" }
    });
    
    root.add({
        
        .tokens = { "?", "thread" },
        .help   = { "Emulator thread" }
    });
    
    root.add({
        
        .tokens = { "?", "thread", "" },
        .help   = { "Emulator thread" },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            dump(emulator, Category::State);
        }
    });

    root.add({
        
        .tokens = { "?", "thread", "runahead" },
        .help   = { "Run-ahead instance" },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            dump(emulator, Category::RunAhead);
        }
    });

    root.add({
        
        .tokens = { "?", c64.shellName() },
        .help   = { c64.description() },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            dump(c64, { Category::Config, Category::State });
        }
    });

    root.add({
        
        .tokens = { "?", cpu.shellName() },
        .help   = { cpu.description() },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            dump(cpu, { Category::Config, Category::State });
        }
    });

    root.add({
        
        .tokens = { "?", mem.shellName() },
        .help   = { mem.description() },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            dump(mem, { Category::Config, Category::State });
        }
    });

    root.add({
        
        .tokens = { "?", cia1.shellName() },
        .help   = { cia1.description() },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            dump(cia1, { Category::Config, Category::State });
        }
    });
    
    root.add({
        
        .tokens = { "?", cia2.shellName() },
        .help   = { cia2.description() },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            dump(cia2, { Category::Config, Category::State });
        }
    });
    
    root.add({
        
        .tokens = { "?", vic.shellName() },
        .help   = { vic.description() },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            dump(vic, { Category::Config, Category::State });
        }
    });
    
    root.add({
        
        .tokens = { "?", "sid" },
        .extra  = { "Arg::value" },
        .help   = { "SID" },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            isize nr = parseNum(argv, 0, 0);
            if (nr < 0 || nr > 3) throw AppError(Fault::OPT_INV_ARG, "0 ... 3");

            dump(sidBridge.sid[nr], { Category::Config, Category::State });
        }
    });
    
    root.add({
        
        .tokens = { "?", sidBridge.shellName() },
        .help   = { sidBridge.description() },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            dump(sidBridge, { Category::Config, Category::State });
        }
    });

    root.add({
        
        .tokens = { "?", expansionPort.shellName() },
        .help   = { expansionPort.description() },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            dump(expansionPort, { Category::Config, Category::State });
        }
    });
    
    //
    // Peripherals
    //
    
    RSCommand::currentGroup = "Peripherals";

    root.add({
        
        .tokens = { "?", keyboard.shellName() },
        .help   = { keyboard.description() },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            dump(keyboard, { Category::Config, Category::State });
        }
    });

    root.add({
        
        .tokens = { "?", port1.shellName() },
        .help   = { port1.description() },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            dump(port1, { Category::Config, Category::State });
        }
    });

    root.add({
        
        .tokens = { "?", port2.shellName() },
        .help   = { port2.description() },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            dump(port2, { Category::Config, Category::State });
        }
    });
    
    root.add({
        
        .tokens = { "?", port1.joystick.shellName() },
        .help   = { port1.joystick.description() },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            dump(port1.joystick, { Category::Config, Category::State });
        }
    });
    
    root.add({
        
        .tokens = { "?", port2.joystick.shellName() },
        .help   = { port2.joystick.description() },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            dump(port2.joystick, { Category::Config, Category::State });
        }
    });
    
    root.add({
        
        .tokens = { "?", port1.mouse.shellName() },
        .help   = { port1.mouse.description() },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            dump(port1.mouse, { Category::Config, Category::State });
        }
    });
    
    root.add({
        
        .tokens = { "?", port2.mouse.shellName() },
        .help   = { port2.mouse.description() },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            dump(port2.mouse, { Category::Config, Category::State });
        }
    });
    
    for (isize i = 0; i < 2; i++) {
        
        auto &drive = i == 0 ? c64.drive8 : c64.drive9;
        
        root.add({
            
            .tokens = { "?", drive.shellName() },
            .help   = { drive.description() }
        });
        
        root.add({
            
            .tokens = { "?", drive.shellName(), "" },
            .help   = { "Inspects the internal state" },
            .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
                
                auto &drive = values.front() ? drive9 : drive8;
                dump(drive, { Category::Config, Category::State });
                
            }, .values = {i}
        });
        
        root.add({
            
            .tokens = { "?", drive.shellName(), "bankmap" },
            .help   = { "Displays the memory layout" },
            .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
                
                auto &drive = values.front() ? drive9 : drive8;
                dump(drive, Category::BankMap);
                
            }, .values = {i}
        });
        
        root.add({
            
            .tokens = { "?", drive.shellName(), "disk" },
            .help   = { "Inspects the current disk" },
            .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
                
                auto &drive = values.front() ? drive9 : drive8;
                dump(drive, Category::Disk);
                
            }, .values = {i}
        });
        
        root.add({
            
            .tokens = { "?", drive.shellName(), "layout" },
            .help   = { "Displays the disk layout" },
            .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
                
                auto &drive = values.front() ? drive9 : drive8;
                dump(drive, Category::Layout);
                
            }, .values = {i}
        });
    }
        
    root.add({
        
        .tokens = { "?", serialPort.shellName() },
        .help   = { serialPort.description() },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            dump(serialPort, { Category::Config, Category::State });
        }
    });
    
    root.add({
        
        .tokens = { "?", datasette.shellName() },
        .help   = { datasette.description() },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            dump(datasette, { Category::Config, Category::State });
        }
    });

    root.add({
        
        .tokens = { "?", audioPort.shellName() },
        .help   = { audioPort.description() },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            dump(audioPort, { Category::Config, Category::State });
        }
    });

    root.add({
        
        .tokens = { "?", host.shellName() },
        .help   = { host.description() },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            dump(host, { Category::Config, Category::State });
        }
    });

    
    //
    // Miscellaneous
    //

    RSCommand::currentGroup = "Miscellaneous";

    root.add({
        
        .tokens = { "checksums" },
        .help   = { "Displays checksum of various components" },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            dump(c64, Category::Checksums);
        }
    });
    
    root.add({
        
        .tokens = { "debug" },
        .help   = { "Debug variables" },
    });
    
    root.add({
        
        .tokens = { "debug", "" },
        .help   = { "Display all debug variables" },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            dump(emulator, Category::Debug);
        }
    });

    if (debugBuild) {

        for (auto i : DebugFlagEnum::elements()) {
            
            root.add({
                
                .tokens = { "debug", DebugFlagEnum::key(i) },
                .args   = { Arg::boolean },
                .help   = { DebugFlagEnum::help(i) },
                .func   = [] (Arguments& argv, const std::vector<isize> &values) {
                    
                    Emulator::setDebugVariable(DebugFlag(values[0]), int(util::parseNum(argv[0])));
                    
                }, .values = { isize(i) }
            });
        }
            
        root.add({
            
            .tokens = { "debug", "verbosity" },
            .args   = { Arg::value },
            .help   = { "Set the verbosity level for generated debug output" },
            .func   = [] (Arguments& argv, const std::vector<isize> &values) {
                
                CoreObject::verbosity = isize(util::parseNum(argv[0]));
            }
        });
    }

    root.add({
        
        .tokens = {"%"},
        .args   = { Arg::value },
        .help   = { "Convert a value into different formats" },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            std::stringstream ss;
            
            if (isNum(argv[0])) {
                mem.debugger.convertNumeric(ss, (u32)parseNum(argv[0]));
            } else {
                mem.debugger.convertNumeric(ss, argv.front());
            }
            
            retroShell << '\n' << ss << '\n';
        }
    });
}

}
