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

}
