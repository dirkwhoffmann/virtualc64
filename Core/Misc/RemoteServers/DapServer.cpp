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
DapServer::_pause()
{
    printf("DapServer::_pause()\n");
}

Transport &
DapServer::transport()
{
    switch (config.transport) {

        case TransportProtocol::STDIO: return stdio;
        case TransportProtocol::TCP:   return tcp;

        default:
            fatalError;
    }
}

const Transport &
DapServer::transport() const
{
    return const_cast<DapServer *>(this)->transport();
}

bool
DapServer::isSupported(TransportProtocol protocol) const
{
    switch (config.transport) {

        case TransportProtocol::STDIO:  return true;
        case TransportProtocol::TCP:    return true;

        default:
            return false;
    }
}

void
DapServer::didSwitch(SrvState from, SrvState to)
{
    if (from != to) msgQueue.put(Msg::SRV_STATE, (i64)to);
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

void
DapServer::reply(const string &payload)
{
    std::stringstream ss;

    ss << "Content-Length: " << payload.size() << "\r\n\r\n" << payload;
    printf("T: %s\n", util::makePrintable(payload).c_str());

    *this << ss.str();
}

}
