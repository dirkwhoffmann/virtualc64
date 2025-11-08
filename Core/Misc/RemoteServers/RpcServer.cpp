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
#include "RpcServer.h"
#include "Emulator.h"
#include "httplib.h"
#include <thread>

namespace vc64 {

void
RpcServer::_dump(Category category, std::ostream &os) const
{
    using namespace util;

    RemoteServer::_dump(category, os);
}

void
RpcServer::didStart()
{
    if (config.verbose) {

        *this << "Remote server is listening at port " << config.port << "\n";
    }
}

string
RpcServer::doReceive()
{
    string payload = connection.recv();

    // Remove LF and CR (if present)
    payload = util::rtrim(payload, "\n\r");

    if (config.verbose) {

        retroShell << "R: " << util::makePrintable(payload) << "\n";
        printf("R: %s\n", util::makePrintable(payload).c_str());
    }

    return payload;
}

void
RpcServer::doSend(const string &payload)
{
    connection.send(payload);

    if (config.verbose) {

        retroShell << "T: " << util::makePrintable(payload) << "\n";
        printf("T: %s\n", util::makePrintable(payload).c_str());
    }
}

void
RpcServer::doProcess(const string &payload)
{
    // TODO
    printf("TODO: doProcess(%s)\n", payload.c_str());
    retroShell.asyncExec(QueuedCmd { .type = QueuedCmd::Type::RPC, .id = 0, .cmd = payload });
}

void
RpcServer::reply(const string &payload, isize id)
{
    send("reply: TODO");
}

}
