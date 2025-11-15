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

#include "SocketServer.h"
#include "Console.h"
#include "TcpTransport.h"
#include "HttpTransport.h"

namespace vc64 {

class RshServer final : public RemoteServer, public ConsoleDelegate, public TransportDelegate {

public:
    
    using RemoteServer::RemoteServer;
    TcpTransport tcp = TcpTransport(*this);

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
    virtual bool isWaiting() const override { return tcp.isWaiting(); }
    virtual bool isStarting() const override { return tcp.isStarting(); }
    virtual bool isListening() const override { return tcp.isListening(); }
    virtual bool isConnected() const override { return tcp.isConnected(); }
    virtual bool isStopping() const override { return tcp.isStopping(); }
    virtual bool isErroneous() const override { return tcp.isErroneous(); }

    virtual void start() override;
    virtual void stop() override;
    virtual void disconnect() override;
    virtual bool canRun() override { return true; }
    virtual void main() override;


    //
    // Methods from SocketServer
    //

    // string doReceive() throws override;
    // void doProcess(const string &packet) throws override;
    // void doSend(const string &packet)throws  override;
    // void didStart() override;
    // void didConnect() override;


    //
    // Methods from TransportDelegate
    //

    virtual void didStart() override;
    virtual void didStop() override;
    virtual void didConnect() override;
    virtual void didDisconnect() override;
    /*
    virtual void didSwitch(SrvState from, SrvState to) { };

    // Error notifications
    virtual void didTerminate(const string &error) { };
    */

    // Provides the output of an executed RetroShell command
    void didReceive(const string &payload) override;


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
