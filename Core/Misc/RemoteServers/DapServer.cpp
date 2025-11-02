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
#include "DapServer.h"
#include "DapServerCmds.h"
#include "Emulator.h"
#include "CPU.h"
#include "IOUtils.h"
#include "Memory.h"
#include "MemUtils.h"
#include "MsgQueue.h"
#include "RetroShell.h"
#include "json.h"

using json = nlohmann::json;

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

    // Try to find the header terminator
    auto headerEnd = cmd.find("\r\n\r\n");
    if (headerEnd == std::string::npos) {
        throw AppError(Fault::DAP_INVALID_FORMAT, cmd);
    }

    // Extract the header portion
    auto header = cmd.substr(0, headerEnd);
    auto contentLengthPos = header.find("Content-Length:");
    if (contentLengthPos == std::string::npos) {
        throw AppError(Fault::DAP_INVALID_FORMAT, cmd);
    }

    // Parse content length
    auto lenStart = contentLengthPos + strlen("Content-Length:");
    auto lenEnd = header.find_first_of("\r\n", lenStart);
    auto lenStr = header.substr(lenStart, lenEnd - lenStart);
    auto contentLength = std::stoul(lenStr);

    // Check if we have the full message
    auto messageStart = headerEnd + 4;
    if (cmd.size() < messageStart + contentLength) {
        throw AppError(Fault::DAP_INVALID_FORMAT, cmd);
    }

    // Extract JSON payload
    string jsonStr = cmd.substr(messageStart, contentLength);

    if (config.verbose) {
        retroShell << "R: " << util::makePrintable(jsonStr) << "\n";
        printf("R: %s\n", util::makePrintable(jsonStr).c_str());
    }

    return jsonStr;
}

void
DapServer::doSend(const string &payload)
{
    connection.send(payload);

    if (config.verbose) {
        retroShell << "T: " << util::makePrintable(payload) << "\n";
    }
}

void
DapServer::doProcess(const string &payload)
{
    try {

        process(payload);
        /*
        if (auto msg = dap::Message::parse(payload); msg) {
            if (auto str = msg->process(c64); !str.empty()) {
                for (auto &s : str) { reply(s); }
            }
        } else {
            throw AppError(Fault::DAP_INVALID_FORMAT, payload);
        }
        */

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
DapServer::process(const string &packet)
{
    json j = json::parse(packet);
    isize s = j.value("seq", 0);
    string t = j.value("type", "");
    string c = j.value("command", "");

    try {

        if (t == "request") {

            if (c == "breakpointLocations") { process<dap::Command::BreakpointLocations>(s, packet); }
            if (c == "configurationDone") { process<dap::Command::ConfigurationDone>(s, packet); }
            if (c == "initialize") { process<dap::Command::Initialize>(s, packet); }
            if (c == "launch") { process<dap::Command::Launch>(s, packet); }
            if (c == "setBreakpoints") { process<dap::Command::SetBreakpoints>(s, packet); }

        } else if (t == "response") {

        } else if (t == "event") {

        } else {

            throw AppError(Fault::DAP_UNRECOGNIZED_CMD, packet);
        }

    } catch (...) {

        throw AppError(Fault::DAP_UNRECOGNIZED_CMD, packet);
    }
}

void
DapServer::reply(const string &payload)
{
    std::stringstream ss;
    ss << "Content-Length: " << payload.size() << "\r\n\r\n" << payload;
    printf("T: %s\n", util::makePrintable(payload).c_str());
    send(ss.str());
}

void
DapServer::replySuccess(isize seq, const string &command)
{
    json response = {

        {"type", "response"},
        {"seq", 0},
        {"request_seq", seq},
        {"command", command},
        {"success", true}
    };

    reply(response.dump());
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
