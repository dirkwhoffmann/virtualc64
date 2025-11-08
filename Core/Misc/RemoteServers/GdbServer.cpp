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

void
GdbServer::_pause()
{
    printf("GdbServer::_pause()\n");

    replyT();
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
        printf("R: %s\n", util::makePrintable(cmd).c_str());
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
        printf("T: %s\n", util::makePrintable(payload).c_str());
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
    emulator.put(Cmd::PAUSE);
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

void
GdbServer::replyJSON(const string &payload)
{
    string encoded;

    for (char c: payload) {

        if (c == '#' || c == '$' || c == '}' || c == '{' || c == '"' || c == '%') {

            char buf[4];
            snprintf(buf, sizeof(buf), "%%%02X", (unsigned char)c);
            encoded += buf;

        } else {

            encoded += c;
        }
    }

    reply(encoded);
}

void
GdbServer::replyT()
{
    std::vector<string> result = { "T05" };

    for (isize i = 0; i <= 5; i++) {
        result.push_back("0" + std::to_string(i) + ":" + readRegister(i));
    }

    reply(util::concat(result, ";"));
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
