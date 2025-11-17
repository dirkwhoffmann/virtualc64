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

    virtual void didStart() override;
    virtual void didStop() override;
    virtual void didConnect() override;
    virtual void didDisconnect() override;
    virtual void didReceive(const string &payload) override;


    //
    // Methods from ConsoleDelegate
    //

    void willExecute(const InputLine &input) override;
    void didExecute(const InputLine &input, std::stringstream &ss) override;
    void didExecute(const InputLine &input, std::stringstream &ss, std::exception &e) override;
};

}
