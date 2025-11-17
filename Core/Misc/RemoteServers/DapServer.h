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
// #include "Console.h"
#include "TcpTransport.h"

namespace vc64 {

class DapServer final : public RemoteServer, public TransportDelegate {

    class DapAdapter *adapter = nullptr;
    TcpTransport tcp = TcpTransport(*this);

public:

    using RemoteServer::RemoteServer;


    //
    // Initializing
    //

public:

    DapServer(C64& ref, isize id);
    ~DapServer();

    DapServer& operator= (const DapServer& other) {

        RemoteServer::operator = (other);
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
    // Methods from Configurable
    //

private:

    void checkOption(Opt opt, i64 value) override;
    

    //
    // Methods from RemoteServer
    //

    virtual SrvState getState() const override { return tcp.getState(); }
    virtual void switchState(SrvState newState) override;
    virtual bool isOff() const override { return tcp.isOff(); }
    // virtual bool isWaiting() const override { return tcp.isWaiting(); }
    virtual bool isStarting() const override { return tcp.isStarting(); }
    virtual bool isListening() const override { return tcp.isListening(); }
    virtual bool isConnected() const override { return tcp.isConnected(); }
    virtual bool isStopping() const override { return tcp.isStopping(); }
    virtual bool isErroneous() const override { return tcp.isErroneous(); }

    virtual void start() override;
    virtual void stop() override;
    virtual void disconnect() override;
    virtual void main() override;


    //
    // Methods from TransportDelegate
    //

    virtual void didStart() override;
    virtual void didStop() override;
    virtual void didConnect() override;
    virtual void didDisconnect() override;
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
