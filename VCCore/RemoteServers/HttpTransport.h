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

#include "Transport.h"

namespace httplib { class Server; struct Request; struct Response; }

namespace vc64 {

class HttpTransport : public Transport {

    using Transport::Transport;

protected:

    HttpTransport& operator= (const HttpTransport& other) {

        Transport::operator = (other);
        return *this;
    }

    // A simple third-party HTTP server
    httplib::Server *srv = nullptr;


    //
    // Methods from Transport
    //

public:

    // See StdioTransport::~StdioTransport() -- stops the server thread
    // before this object's srv pointer and Transport's serverThread are
    // torn down.
    ~HttpTransport();

    void disconnect() override;
    void main(u16 port, const string &endpoint) override;
    void send(const string &payload) override { }

private:

    // Records a request/response pair and forwards it to the delegate
    void deliver(const httplib::Request &req, httplib::Response &res);
};

}
