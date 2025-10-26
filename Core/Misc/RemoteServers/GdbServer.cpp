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
GdbServer::_dump(Category category, std::ostream &os) const
{
    using namespace util;

    RemoteServer::_dump(category, os);
}

bool
GdbServer::shouldRun()
{
    return true;
}

string
GdbServer::doReceive()
{
    auto cmd = connection.recv();

    // Remove LF and CR (if present)
    cmd = util::rtrim(cmd, "\n\r");

    if (config.verbose) {
        retroShell << "R: " << util::makePrintable(cmd) << "\n";
    }

    latestCmd = cmd;
    return latestCmd;
}

void
GdbServer::doSend(const string &payload)
{
    connection.send(payload);

    if (config.verbose) {
        retroShell << "T: " << util::makePrintable(payload) << "\n";
    }
}

void
GdbServer::doProcess(const string &payload)
{
    try {

        process(latestCmd);

    } catch (AppError &err) {

        auto msg = "GDB server error: " + string(err.what());
        debug(SRV_DEBUG, "%s\n", msg.c_str());

        // Display the error message in RetroShell
        retroShell << msg << '\n';

        // Disconnect the client
        disconnect();
    }
}

void
GdbServer::didStart()
{
    // emulator.pause();
}

void
GdbServer::didStop()
{

}

void
GdbServer::didConnect()
{
    ackMode = true;
}

void
GdbServer::reply(const string &payload)
{
    string packet = "$";

    packet += payload;
    packet += "#";
    packet += computeChecksum(payload);

    send(packet);
}

string
GdbServer::computeChecksum(const string &s)
{
    u8 chk = 0;
    for(auto &c : s) U8_INC(chk, c);

    return util::hexstr <2> (chk);
}

bool
GdbServer::verifyChecksum(const string &s, const string &chk)
{
    return chk == computeChecksum(s);
}

string
GdbServer::readRegister(isize nr)
{
    return util::hexstr <8> (0x42);

//    return "xxxxxxxx";
}

string
GdbServer::readMemory(isize addr)
{
    auto byte = mem.spypeek((u16)addr);
    return util::hexstr <2> (byte);
}

void
GdbServer::breakpointReached()
{
    debug(GDB_DEBUG, "breakpointReached()\n");
    process <'?'> ("");
}

}
