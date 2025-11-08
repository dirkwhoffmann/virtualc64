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

namespace vc64 {

class RpcServer final : public SocketServer {

public:

    using SocketServer::SocketServer;

protected:

    RpcServer& operator= (const RpcServer& other) {

        SocketServer::operator = (other);
        return *this;
    }


    //
    // Methods from CoreObject
    //

protected:

    void _dump(Category category, std::ostream &os) const override;


    //
    // Methods from RemoteServer
    //

    string doReceive() throws override;
    void doProcess(const string &packet) throws override;
    void doSend(const string &packet) throws  override;
    void didStart() override;
    // void didConnect() override;

    //
    //
    //

public:

    // Sends a response packet
    void reply(const string &payload, isize id);
};

}
