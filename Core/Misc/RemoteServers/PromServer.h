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

public:

    using RemoteServer::RemoteServer;

protected:

    PromServer& operator= (const PromServer& other) {

        RemoteServer::operator = (other);
        return *this;
    }


    //
    // Methods from CoreObject
    //

protected:

    void _dump(Category category, std::ostream &os) const override;


    //
    // Methods from CoreComponent
    //

    void _halt() override { try { stop(); } catch(...) { } };


    //
    // Methods from Configurable
    //

private:

    void checkOption(Opt opt, i64 value) override;
    

    //
    // Methods from RemoteServer
    //

    virtual SrvState getState() const override { return http.getState(); }
    virtual void start() override { http.start(config.port, "/metrics"); }
    virtual void stop() override { http.stop(); }
    virtual void disconnect() override { http.disconnect(); }


    //
    // Methods from TransportDelegate
    //

    // virtual void didStart() override;
    // virtual void didStop() override;
    // virtual void didConnect() override;
    // virtual void didDisconnect() override;
    virtual void didReceive(const httplib::Request &req, httplib::Response &res) override;


    //
    // Handling requests
    //

    // Generate a response
    // string respond(const httplib::Request& request);
};

}
