// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "RetroShell.h"
#include "C64.h"
#include "IOUtils.h"
#include "Parser.h"

#include <fstream>
#include <sstream>

namespace vc64 {

//
// Instruction Stream
//

template <> void
RetroShell::exec <Token::pause> (Arguments &argv, long param)
{
    c64.pause();
}

template <> void
RetroShell::exec <Token::cont> (Arguments &argv, long param)
{
    c64.run();
}

template <> void
RetroShell::exec <Token::step> (Arguments &argv, long param)
{
    c64.stepInto();
}

template <> void
RetroShell::exec <Token::next> (Arguments &argv, long param)
{
    c64.stepOver();
}

template <> void
RetroShell::exec <Token::jump> (Arguments &argv, long param)
{
    *this << "TODO\n";
}

template <> void
RetroShell::exec <Token::disassemble> (Arguments &argv, long param)
{
    *this << "TODO\n";
}


//
// Breakpoints
//

template <> void
RetroShell::exec <Token::bp> (Arguments& argv, long param)
{
    dump(c64.cpu, Category::Breakpoints);
}

template <> void
RetroShell::exec <Token::bp, Token::at> (Arguments& argv, long param)
{
    *this << "TODO\n";
    // cpu.debugger.breakpoints.addAt(u32(util::parseNum(argv.front())));
}

template <> void
RetroShell::exec <Token::bp, Token::del> (Arguments& argv, long param)
{
    *this << "TODO\n";
    // cpu.debugger.breakpoints.removeAt(u32(util::parseNum(argv.front())));
}

template <> void
RetroShell::exec <Token::bp, Token::enable> (Arguments& argv, long param)
{
    *this << "TODO\n";
    // cpu.debugger.breakpoints.enableAt(u32(util::parseNum(argv.front())));
}

template <> void
RetroShell::exec <Token::bp, Token::disable> (Arguments& argv, long param)
{
    *this << "TODO\n";
    // cpu.debugger.breakpoints.disableAt(u32(util::parseNum(argv.front())));
}

template <> void
RetroShell::exec <Token::bp, Token::ignore> (Arguments& argv, long param)
{
    *this << "TODO\n";
}


//
// Watchpoints
//

template <> void
RetroShell::exec <Token::wp> (Arguments& argv, long param)
{
    dump(c64.cpu, Category::Watchpoints);
}

template <> void
RetroShell::exec <Token::wp, Token::at> (Arguments& argv, long param)
{
    *this << "TODO\n";
    // cpu.debugger.watchpoints.addAt(u32(util::parseNum(argv.front())));}
}

template <> void
RetroShell::exec <Token::wp, Token::del> (Arguments& argv, long param)
{
    *this << "TODO\n";
    // cpu.debugger.watchpoints.removeAt(u32(util::parseNum(argv.front())));
}

template <> void
RetroShell::exec <Token::wp, Token::enable> (Arguments& argv, long param)
{
    *this << "TODO\n";
    // cpu.debugger.watchpoints.enableAt(u32(util::parseNum(argv.front())));
}

template <> void
RetroShell::exec <Token::wp, Token::disable> (Arguments& argv, long param)
{
    *this << "TODO\n";
    // cpu.debugger.watchpoints.disableAt(u32(util::parseNum(argv.front())));
}

template <> void
RetroShell::exec <Token::wp, Token::ignore> (Arguments& argv, long param)
{
    *this << "TODO\n";
}


//
// C64
//

template <> void
RetroShell::exec <Token::c64, Token::inspect> (Arguments &argv, long param)
{
    dump(c64, Category::Inspection);
}

template <> void
RetroShell::exec <Token::c64, Token::debug> (Arguments &argv, long param)
{
    dump(c64, Category::Debug);
}


//
// Memory
//

template <> void
RetroShell::exec <Token::memory, Token::inspect> (Arguments& argv, long param)
{
    dump(mem, Category::State);
}


//
// Drive
//

template <> void
RetroShell::exec <Token::drive, Token::inspect> (Arguments& argv, long param)
{
    auto &drive = param ? drive9 : drive8;
    dump(drive, Category::Inspection);
}

template <> void
RetroShell::exec <Token::drive, Token::debug> (Arguments& argv, long param)
{
    auto &drive = param ? drive9 : drive8;
    dump(drive, Category::Debug);
}

template <> void
RetroShell::exec <Token::drive, Token::bankmap> (Arguments& argv, long param)
{
    auto &drive = param ? drive9 : drive8;
    dump(drive, Category::BankMap);
}

template <> void
RetroShell::exec <Token::drive, Token::disk> (Arguments& argv, long param)
{
    auto &drive = param ? drive9 : drive8;
    dump(drive, Category::Disk);
}


//
// Datasette
//

template <> void
RetroShell::exec <Token::datasette, Token::inspect> (Arguments& argv, long param)
{
    dump(datasette, Category::State);
}


//
// CPU
//

template <> void
RetroShell::exec <Token::cpu, Token::inspect, Token::state> (Arguments& argv, long param)
{
    dump(cpu, Category::State);
}

template <> void
RetroShell::exec <Token::cpu, Token::inspect, Token::registers> (Arguments& argv, long param)
{
    dump(cpu, Category::Registers);
}


//
// CIA
//

template <> void
RetroShell::exec <Token::cia, Token::inspect, Token::state> (Arguments& argv, long param)
{
    if (param == 0) {
        dump(cia1, Category::State);
    } else {
        dump(cia2, Category::State);
    }
}

template <> void
RetroShell::exec <Token::cia, Token::inspect, Token::registers> (Arguments& argv, long param)
{
    if (param == 0) {
        dump(cia1, Category::Registers);
    } else {
        dump(cia2, Category::Registers);
    }
}

template <> void
RetroShell::exec <Token::cia, Token::inspect, Token::tod> (Arguments& argv, long param)
{
    if (param == 0) {
        dump(cia1.tod, Category::State);
    } else {
        dump(cia2.tod, Category::State);
    }
}


//
// VICII
//

template <> void
RetroShell::exec <Token::vicii, Token::inspect, Token::registers> (Arguments& argv, long param)
{
    dump(vic, Category::Registers);
}

template <> void
RetroShell::exec <Token::vicii, Token::inspect, Token::state> (Arguments& argv, long param)
{
    dump(vic, Category::State);
}


//
// SID
//

template <> void
RetroShell::exec <Token::sid, Token::inspect, Token::sid> (Arguments& argv, long param)
{
    dump(muxer, Category::State);
}

template <> void
RetroShell::exec <Token::sid, Token::inspect, Token::state> (Arguments& argv, long param)
{
    auto value = util::parseNum(argv.front());
    if (value < 0 || value > 3) throw VC64Error(ERROR_OPT_INVARG, "0, 1, 2, or 3");
    dump(muxer.getSID(value), Category::State);
}

template <> void
RetroShell::exec <Token::sid, Token::inspect, Token::registers> (Arguments& argv, long param)
{
    auto value = util::parseNum(argv.front());
    if (value < 0 || value > 3) throw VC64Error(ERROR_OPT_INVARG, "0, 1, 2, or 3");
    dump(muxer.getSID(value), Category::Registers);
}


//
// Control port
//

template <> void
RetroShell::exec <Token::controlport, Token::inspect> (Arguments& argv, long param)
{
    dump(param == 0 ? port1 : port2, Category::State);
}


//
// Expansion port
//

template <> void
RetroShell::exec <Token::expansion, Token::inspect> (Arguments& argv, long param)
{
    dump(expansionport, Category::State);
}


//
// Keyboard
//

template <> void
RetroShell::exec <Token::keyboard, Token::inspect> (Arguments& argv, long param)
{
    dump(keyboard, Category::State);
}


//
// Joystick
//

template <> void
RetroShell::exec <Token::joystick, Token::inspect> (Arguments& argv, long param)
{
    dump(port1.joystick, Category::State);
    *this << '\n';
    dump(port2.joystick, Category::State);
}


//
// Mouse
//

template <> void
RetroShell::exec <Token::mouse, Token::inspect> (Arguments& argv, long param)
{
    dump(port1.mouse, Category::State);
    *this << '\n';
    dump(port2.mouse, Category::State);
}


//
// Parallel cable (drive accelerator)
//

template <> void
RetroShell::exec <Token::parcable, Token::inspect> (Arguments& argv, long param)
{
    dump(parCable, Category::State);
}

}
