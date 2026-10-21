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
#include "Console.h"
#include "StdioTransport.h"
#include "TcpTransport.h"
#include "HttpTransport.h"

namespace vc64 {

class RshServer final : public RemoteServer, public ConsoleDelegate {

    StdioTransport stdio = StdioTransport(*this);
    TcpTransport tcp = TcpTransport(*this);
    HttpTransport http = HttpTransport(*this);

public:

    using RemoteServer::RemoteServer;

    RshServer& operator= (const RshServer& other) {

        RemoteServer::operator = (other);
        return *this;
    }


    //
    // Methods from CoreObject
    //

private:

    void _initialize() override;
    void _dump(Category category, std::ostream &os) const override;

    //
    // Methods from RemoteServer
    //

    bool canRun() override { return true; }
    string endpoint() const override { return "/rsh"; }

    Transport &transport() override;
    const Transport &transport() const override;
    bool isSupported(TransportProtocol protocol) const override;

public:

    // Sends a packet, mapping control characters for terminal display
    void send(const string &payload) override;

private:


    //
    // Methods from TransportDelegate
    //

    void didStart() override;
    void didConnect() override;
    void didDisconnect() override;
    void didReceive(const string &payload) override;
    void didReceive(const httplib::Request &req, httplib::Response &res) override;


    //
    // Methods from ConsoleDelegate
    //

    void didActivate() override;
    void didDeactivate() override;
    void willExecute(const InputLine &input) override;
    void didExecute(const InputLine &input, std::stringstream &ss) override;
    void didExecute(const InputLine &input, std::stringstream &ss, std::exception &e) override;
};

}
