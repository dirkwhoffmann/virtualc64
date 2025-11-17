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
#include "TcpTransport.h"
#include "HttpTransport.h"

namespace vc64 {

namespace RPC {

const long PARSE_ERROR      = -32700; // Invalid JSON was received by the server
const long INVALID_REQUEST  = -32600; // The JSON sent is not a valid Request object
const long METHOD_NOT_FOUND = -32601; // The method does not exist / is not available
const long INVALID_PARAMS   = -32602; // Invalid method parameter(s)
const long INTERNAL_ERROR   = -32603; // Internal JSON-RPC error
const long SERVER_ERROR     = -32000; // Reserved for implementation-defined server-errors

}

class RpcServer final : public RemoteServer, public ConsoleDelegate, public TransportDelegate {

    TcpTransport tcp = TcpTransport(*this);
    // HttpTransport http = HttpTransport(*this);

public:

    using RemoteServer::RemoteServer;
    
protected:

    RpcServer& operator= (const RpcServer& other) {

        RemoteServer::operator = (other);
        return *this;
    }


    //
    // Methods from CoreObject
    //

protected:

    void _initialize() override;
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

    virtual SrvState getState() const override { return tcp.getState(); }

    virtual void start() override;
    virtual void stop() override;
    virtual void disconnect() override;

    
    //
    // Methods from TransportDelegate
    //

    virtual void didSwitch(SrvState from, SrvState to) override;
    virtual void didStart() override { }
    virtual void didStop() override { }
    virtual void didConnect() override { }
    virtual void didDisconnect() override { }
    virtual void didReceive(const string &payload) override;

    
    //
    // Methods from ConsoleDelegate
    //

    void willExecute(const InputLine &input) override;
    void didExecute(const InputLine &input, std::stringstream &ss) override;
    void didExecute(const InputLine &input, std::stringstream &ss, std::exception &e) override;
};

}
