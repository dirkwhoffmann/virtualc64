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

void
DapServer::_dump(Category category, std::ostream &os) const
{
    using namespace util;

    RemoteServer::_dump(category, os);
}

void
DapServer::_pause()
{
    printf("DapServer::_pause()\n");
}

bool
DapServer::shouldRun()
{
    return true;
}

string
DapServer::doReceive()
{
    auto cmd = connection.recv();

    // Remove LF and CR (if present)
    // cmd = util::rtrim(cmd, "\n\r");

    if (config.verbose) {
        retroShell << "R: " << util::makePrintable(cmd) << "\n";
        printf("R: %s\n", util::makePrintable(cmd).c_str());
    }

    // latestCmd = cmd;
    return cmd;
}

void
DapServer::doSend(const string &payload)
{
    connection.send(payload);

    if (config.verbose) {
        retroShell << "T: " << util::makePrintable(payload) << "\n";
        printf("T: %s\n", util::makePrintable(payload).c_str());
    }
}

void
DapServer::doProcess(const string &payload)
{
    try {

        process(payload);

    } catch (AppError &err) {

        auto msg = "DAP server error: " + string(err.what());
        debug(SRV_DEBUG, "%s\n", msg.c_str());

        // Display the error message in RetroShell
        retroShell << msg << '\n';

        // Disconnect the client
        disconnect();
    }
}

void
DapServer::didStart()
{
    emulator.put(Cmd::PAUSE);
}

void
DapServer::didStop()
{

}

void
DapServer::didConnect()
{

}

void
DapServer::reply(const string &payload)
{
    string packet = "$";

    packet += payload;

    send(packet);
}

string
DapServer::readRegister(isize nr)
{
    switch (nr) {

        case 0: return util::hexstr<2>(c64.cpu.reg.a);
        case 1: return util::hexstr<2>(c64.cpu.reg.x);
        case 2: return util::hexstr<2>(c64.cpu.reg.y);
        case 3: return util::hexstr<2>(c64.cpu.reg.sp);
        case 4: return util::hexstr<2>(c64.cpu.getP());
        case 5: return util::hexstr<4>(c64.cpu.getPC0());
        default: return "00";
    }
}

string
DapServer::readMemory(isize addr)
{
    auto byte = mem.spypeek((u16)addr);
    return util::hexstr <2> (byte);
}

void
DapServer::breakpointReached()
{
    debug(DAP_DEBUG, "breakpointReached()\n");
}

}
