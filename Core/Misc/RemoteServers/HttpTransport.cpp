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
#include "HttpTransport.h"
#include "httplib.h"

namespace vc64 {

void
HttpTransport::disconnect()
{
    if (srv) srv->stop();
    delegate.didDisconnect();
}

void
HttpTransport::main(u16 port, const string &endpoint)
{
    try {

        // Create the HTTP server
        if (!srv) srv = new httplib::Server();

        // Define the endpoint
        srv->Get(endpoint, [this](const httplib::Request& req, httplib::Response& res) {

            switchState(SrvState::CONNECTED);
            delegate.didReceive(req, res);
        });

        // Start the server to listen on localhost
        debug(SRV_DEBUG, "Starting HTTP server thread\n");
        srv->listen("localhost", port);

    } catch (std::exception &err) {

        debug(SRV_DEBUG, "HTTP server thread interrupted\n");
        delegate.didTerminate(err.what());
    }
}

}
