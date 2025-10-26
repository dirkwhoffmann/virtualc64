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
#include "GdbServer.h"
#include "Emulator.h"
#include "CPU.h"
#include "IOUtils.h"
#include "Memory.h"
#include "MemUtils.h"
#include "MsgQueue.h"
#include "RetroShell.h"

namespace vc64 {

template <> void
GdbServer::process <' ', GdbCmd::CtrlC> (string arg)
{
    debug(SRV_DEBUG, "Ctrl+C\n");

    c64.signalStop();
    reply("OK");
}

template <> void
GdbServer::process <'q', GdbCmd::Supported> (string arg)
{
    reply("PacketSize=1ff;"
          "multiprocess-;"
          "swbreak+;"
          "QStartNoAckMode+;"
          "vContSupported+");
}

template <> void
GdbServer::process <'q', GdbCmd::Symbol> (string arg)
{
    reply("OK");
}

template <> void
GdbServer::process <'q', GdbCmd::Offset> (string arg)
{
    string result = "??";

    reply(result);
}

template <> void
GdbServer::process <'q', GdbCmd::TStatus> (string arg)
{
    reply("");
}

template <> void
GdbServer::process <'q', GdbCmd::TfV> (string arg)
{
    reply("l");
}

template <> void
GdbServer::process <'q', GdbCmd::TfP> (string arg)
{
    reply("l");
}

template <> void
GdbServer::process <'q', GdbCmd::fThreadInfo> (string arg)
{
    reply("m1");
}

template <> void
GdbServer::process <'q', GdbCmd::sThreadInfo> (string arg)
{
    reply("l");
}

template <> void
GdbServer::process <'q', GdbCmd::Attached> (string arg)
{
    reply("0");
}

template <> void
GdbServer::process <'q', GdbCmd::C> (string arg)
{
    reply("");
}

template <> void
GdbServer::process <'Q', GdbCmd::StartNoAckMode> (string arg)
{
    ackMode = false;
    reply("OK");
}

template <> void
GdbServer::process <'v', GdbCmd::MustReplyEmpty> (string arg)
{
    reply("");
}

template <> void
GdbServer::process <'v', GdbCmd::ContQ> (string arg)
{
    reply("vCont;cst");
}

template <> void
GdbServer::process <'v', GdbCmd::Cont> (string arg)
{
    if (arg == "c") {

        emulator.run();
        return;
    }
    if (arg == "s") {

        emulator.stepInto();
        return;
    }

    throw AppError(Fault::GDB_INVALID_FORMAT);
}

template <> void
GdbServer::process <'v'> (string cmd)
{
    auto command = cmd;

    if (command == "MustReplyEmpty") {

        process <'v', GdbCmd::MustReplyEmpty> ("");
        return;
    }
    if (command == "Cont?") {

        process <'v', GdbCmd::ContQ> ("");
        return;
    }
    if (command == "Cont;c") {

        process <'v', GdbCmd::Cont> ("c");
        return;
    }

    if (command == "Cont;s") {

        process <'v', GdbCmd::Cont> ("s");
        return;
    }

    throw AppError(Fault::GDB_UNSUPPORTED_CMD, "v");
}

template <> void
GdbServer::process <'q'> (string cmd)
{
    auto command = cmd.substr(0, cmd.find(":"));

    if (command == "Supported") {

        process <'q', GdbCmd::Supported> ("");
        return;
    }
    if (cmd == "Symbol::") {

        process <'q', GdbCmd::Symbol> ("");
        return;
    }
    if (cmd == "Offsets") {

        process <'q', GdbCmd::Offset> ("");
        return;
    }
    if (cmd == "TStatus") {

        process <'q', GdbCmd::TStatus> ("");
        return;
    }
    if (cmd == "TfV") {

        process <'q', GdbCmd::TfV> ("");
        return;
    }
    if (cmd == "TfP") {

        process <'q', GdbCmd::TfP> ("");
        return;
    }
    if (cmd == "fThreadInfo") {

        process <'q', GdbCmd::fThreadInfo> ("");
        return;
    }
    if (cmd == "sThreadInfo") {

        process <'q', GdbCmd::sThreadInfo> ("");
        return;
    }
    if (command == "Attached") {

        process <'q', GdbCmd::Attached> ("");
        return;
    }
    if (command == "C") {

        process <'q', GdbCmd::C> ("");
        return;
    }

    throw AppError(Fault::GDB_UNSUPPORTED_CMD, "q");
}

template <> void
GdbServer::process <'Q'> (string cmd)
{
    auto tokens = util::split(cmd, ':');

    if (tokens[0] == "StartNoAckMode") {

        process <'Q', GdbCmd::StartNoAckMode> ("");
        return;
    }

    throw AppError(Fault::GDB_UNSUPPORTED_CMD, "Q");
}

template <> void
GdbServer::process <'g'> (string cmd)
{
    string result;
    for (int i = 0; i < 18; i++) result += readRegister(i);
    reply(result);
}

template <> void
GdbServer::process <'s'> (string cmd)
{
    process <'v', GdbCmd::Cont> ("s");
}

template <> void
GdbServer::process <'n'> (string cmd)
{
    throw AppError(Fault::GDB_UNSUPPORTED_CMD, "n");
}

template <> void
GdbServer::process <'H'> (string cmd)
{
    reply("OK");
}

template <> void
GdbServer::process <'G'> (string cmd)
{
    throw AppError(Fault::GDB_UNSUPPORTED_CMD, "G");
}

template <> void
GdbServer::process <'?'> (string cmd)
{
    auto pc = util::hexstr <8> (cpu.getPC0());
    reply("T051:" + pc + ";");

    /*
     if (amiga.isRunning()) {

     reply("OK");

     } else {

     auto pc = util::hexstr <8> (cpu.getPC0());
     reply("T051:" + pc + ";");
     }
     */
}

template <> void
GdbServer::process <'!'> (string cmd)
{
    throw AppError(Fault::GDB_UNSUPPORTED_CMD, "!");
}

template <> void
GdbServer::process <'k'> (string cmd)
{
    throw AppError(Fault::GDB_UNSUPPORTED_CMD, "k");
}

template <> void
GdbServer::process <'m'> (string cmd)
{
    auto tokens = util::split(cmd, ',');

    if (tokens.size() == 2) {

        string result;

        isize addr;
        util::parseHex(tokens[0], &addr);
        isize size;
        util::parseHex(tokens[1], &size);

        for (isize i = 0; i < size; i++) {
            result += readMemory(addr + i);
        }

        reply(result);

    } else {

        throw AppError(Fault::GDB_UNSUPPORTED_CMD, "m");
    }
}

template <> void
GdbServer::process <'M'> (string cmd)
{
    throw AppError(Fault::GDB_UNSUPPORTED_CMD, "M");
}

template <> void
GdbServer::process <'p'> (string cmd)
{
    isize nr;
    util::parseHex(cmd, &nr);
    reply(readRegister(nr));
}

template <> void
GdbServer::process <'P'> (string cmd)
{
    throw AppError(Fault::GDB_UNSUPPORTED_CMD, "P");
}

template <> void
GdbServer::process <'c'> (string cmd)
{
    process <'v', GdbCmd::Cont> ("c");
}

template <> void
GdbServer::process <'D'> (string cmd)
{
    throw AppError(Fault::GDB_UNSUPPORTED_CMD, "D");
}

template <> void
GdbServer::process <'Z'> (string cmd)
{
    auto tokens = util::split(cmd, ',');

    if (tokens.size() == 3) {

        auto type = std::stol(tokens[0]);
        auto addr = std::stol(tokens[1], 0, 16);

        if (type == 0) {

            cpu.debugger.breakpoints.setAt((u32)addr);
        }

        reply("OK");

    } else {

        throw AppError(Fault::GDB_INVALID_FORMAT, "Z");
    }
}

template <> void
GdbServer::process <'z'> (string cmd)
{
    auto tokens = util::split(cmd, ',');

    if (tokens.size() == 3) {

        auto type = std::stol(tokens[0]);
        auto addr = std::stol(tokens[1], 0, 16);

        if (type == 0) {

            cpu.debugger.breakpoints.removeAt((u32)addr);
        }

        reply("OK");

    } else {

        throw AppError(Fault::GDB_INVALID_FORMAT, "z");
    }
}

void
GdbServer::process(string package)
{
    debug(SRV_DEBUG, "process(%s)\n", package.c_str());

    // Check if the previous package has been rejected
    if (package[0] == '-') throw AppError(Fault::GDB_NO_ACK);

    // Strip off the acknowledgment symbol if present
    if (package[0] == '+') package.erase(0,1);

    if (auto len = package.length()) {

        // Check for Ctrl+C
        if (package[0] == 0x03) {
            process <' ', GdbCmd::CtrlC> ("");
            return;
        }

        // Check for '$x[...]#xx'
        if (package[0] == '$' && len >= 5 && package[len - 3] == '#') {

            auto cmd = package[1];
            auto arg = package.substr(2, len - 5);
            auto chk = package.substr(len - 2, 2);

            if (verifyChecksum(package.substr(1, len - 4), chk)) {

                latestCmd = package;

                if (ackMode) send("+");
                process(cmd, arg);

            } else {

                if (ackMode) connection.send("-");
                throw AppError(Fault::GDB_INVALID_CHECKSUM);
            }

            return;
        }

        throw AppError(Fault::GDB_INVALID_FORMAT);
    }
}

void
GdbServer::process(char cmd, string package)
{
    switch (cmd) {

        case 'v' : process <'v'> (package); break;
        case 'q' : process <'q'> (package); break;
        case 'Q' : process <'Q'> (package); break;
        case 'g' : process <'g'> (package); break;
        case 's' : process <'s'> (package); break;
        case 'n' : process <'n'> (package); break;
        case 'H' : process <'H'> (package); break;
        case 'G' : process <'G'> (package); break;
        case '?' : process <'?'> (package); break;
        case '!' : process <'!'> (package); break;
        case 'k' : process <'k'> (package); break;
        case 'm' : process <'m'> (package); break;
        case 'M' : process <'M'> (package); break;
        case 'p' : process <'p'> (package); break;
        case 'P' : process <'P'> (package); break;
        case 'c' : process <'c'> (package); break;
        case 'D' : process <'D'> (package); break;
        case 'Z' : process <'Z'> (package); break;
        case 'z' : process <'z'> (package); break;

        default:
            throw AppError(Fault::GDB_UNRECOGNIZED_CMD, string(1, cmd));
    }
}

}
