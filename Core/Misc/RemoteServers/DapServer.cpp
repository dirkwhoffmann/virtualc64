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
#include "DapAdapter.h"
#include "Emulator.h"
#include "CPU.h"
#include "IOUtils.h"
#include "Memory.h"
#include "MemUtils.h"
#include "MsgQueue.h"
#include "RetroShell.h"

namespace vc64 {

DapServer::DapServer(C64& ref, isize id) : RemoteServer(ref, id) {

    adapter = new DapAdapter(c64);
}

DapServer::~DapServer() {

    delete adapter;
}

void
DapServer::_dump(Category category, std::ostream &os) const
{
    using namespace util;

    RemoteServer::_dump(category, os);
}

void
DapServer::checkOption(Opt opt, i64 value)
{
    if (opt == Opt::SRV_TRANSPORT && value == i64(TransportProtocol::HTTP)) {
        throw AppError(Fault::OPT_UNSUPPORTED, "This server requires a raw TCP connection.");
    }

    RemoteServer::checkOption(opt, value);
}

void
DapServer::_pause()
{
    printf("DapServer::_pause()\n");
}

void
DapServer::start()
{
    tcp.start(config.port);
}

void
DapServer::stop()
{
    tcp.stop();
}

void
DapServer::disconnect()
{
    tcp.disconnect();
}

void
DapServer::didSwitch(SrvState from, SrvState to)
{
    if (from != to) msgQueue.put(Msg::SRV_STATE, (i64)to);
}

void
DapServer::didConnect()
{
    if (config.verbose) {

        try {

            tcp << "VirtualC64 RetroShell Remote Server ";
            tcp << C64::build() << '\n';
            tcp << '\n';

            tcp << "Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de" << '\n';
            tcp << "https://github.com/dirkwhoffmann/virtualc64" << '\n';
            tcp << '\n';

            tcp << "Type 'help' for help.\n";
            tcp << '\n';

            tcp << retroShell.prompt();

        } catch (...) { };
    }
}

void
DapServer::didReceive(const string &cmd)
{
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

    try {

        adapter->process(jsonStr);

    } catch (AppError &err) {

        auto msg = "DAP server error: " + string(err.what());
        debug(SRV_DEBUG, "%s\n", msg.c_str());

        // Display the error message in RetroShell
        retroShell << msg << '\n';

        // Disconnect the client
        disconnect();
    }
}

/*
void
DapServer::doSend(const string &payload)
{
    connection.send(payload);

    if (config.verbose) {
        retroShell << "T: " << util::makePrintable(payload) << "\n";
    }
}
*/

/*
void
DapServer::doProcess(const string &payload)
{
    try {

        adapter->process(payload);

    } catch (AppError &err) {

        auto msg = "DAP server error: " + string(err.what());
        debug(SRV_DEBUG, "%s\n", msg.c_str());

        // Display the error message in RetroShell
        retroShell << msg << '\n';

        // Disconnect the client
        disconnect();
    }
}
*/

/*
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
*/

void
DapServer::reply(const string &payload)
{
    std::stringstream ss;
    ss << "Content-Length: " << payload.size() << "\r\n\r\n" << payload;
    printf("T: %s\n", util::makePrintable(payload).c_str());
    tcp << ss.str();
}

/*
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
*/

}
