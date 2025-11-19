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
#include "DapServerTypes.h"
#include "StdioTransport.h"
#include "TcpTransport.h"

namespace vc64 {

class DapServer final : public RemoteServer, public TransportDelegate {

    StdioTransport stdio = StdioTransport(*this);
    TcpTransport tcp = TcpTransport(*this);

    class DapAdapter *adapter = nullptr;


    //
    // Methods
    //

public:

    // using RemoteServer::RemoteServer;

    DapServer(C64& ref, isize id);
    ~DapServer();

    DapServer& operator=(const DapServer& other) {

        RemoteServer::operator=(other);
        return *this;
    }


    //
    // Methods from CoreObject
    //

private:

    void _dump(Category category, std::ostream &os) const override;
    void _pause() override;


    //
    // Methods from CoreComponent
    //

    void _halt() override { try { stop(); } catch(...) { } };

  

    //
    // Methods from RemoteServer
    //

    Transport &transport() override;
    const Transport &transport() const override;
    bool isSupported(TransportProtocol protocol) const override;


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
    // Handling packets
    //

public:

    // Processes a single command (DapServerCmds.cpp)
    // template <dap::Command> void process(isize seq, const string &packet) throws;
    // void process(dap::Command cmd, isize seq, const string &packet) throws;

    // Sends a packet to the connected client
    void reply(const string &payload);
};

}
