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

    HttpServer::_dump(category, os);
}

string
RpcServer::respond(const httplib::Request& request)
{
    std::ostringstream output;

    printf("Holla, die Waldfee\n");
    output << "Holla, die Waldfee";

    return output.str();
}

void
RpcServer::main()
{
    try {

        // Create the HTTP server
        if (!srv) srv = new httplib::Server();

        // Define the endpoint
        srv->Post("/rpc", [this](const httplib::Request& req, httplib::Response& res) {

            switchState(SrvState::CONNECTED);
            res.set_content(respond(req), "text/plain");
        });

        // Start the server to listen on localhost
        debug(SRV_DEBUG, "Starting RPC server\n");
        srv->listen("localhost", (int)config.port);

    } catch (std::exception &err) {

        debug(SRV_DEBUG, "Server thread interrupted\n");
        handleError(err.what());
    }
}

}
