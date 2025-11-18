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

#pragma once

#include "RemoteServer.h"
#include "HttpTransport.h"

namespace vc64 {

class PromServer final : public RemoteServer, public TransportDelegate {

    HttpTransport http = HttpTransport(*this);


    //
    // Methods
    //

    using RemoteServer::RemoteServer;

    PromServer& operator=(const PromServer& other) {

        RemoteServer::operator=(other);
        return *this;
    }


    //
    // Methods from CoreObject
    //

protected:

    void _initialize() override;
    void _dump(Category category, std::ostream &os) const override;


    //
    // Methods from RemoteServer
    //

    Transport &transport() override;
    const Transport &transport() const override;
    bool isSupported(TransportProtocol protocol) const override;
    void start() override { transport().start(config.port, "/metrics"); }

    
    //
    // Methods from TransportDelegate
    //

    void didSwitch(SrvState from, SrvState to) override;
    void didStart() override { }
    void didStop() override { }
    void didConnect() override { }
    void didDisconnect() override { }
    void didReceive(const httplib::Request &req, httplib::Response &res) override;
};

}
