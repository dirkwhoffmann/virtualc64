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
#include "Socket.h"

namespace vc64 {

class TcpTransport : public Transport {

    // Sockets
    Socket listener;
    Socket connection;

    // Number of transmitted packages
    // isize numSent = 0;
    // isize numReceived = 0;

    using Transport::Transport;

    TcpTransport& operator=(const TcpTransport& other) {

        Transport::operator=(other);
        return *this;
    }

    //
    // Methods from Transport
    //

public:

    virtual void disconnect() override;
    void main(u16 port, const string &endpoint = "") override;

private:
    
    // Inner loops (called from main)
    void mainLoop(u16 port);
    void sessionLoop();


    //
    // Sending
    //

public:

    // Sends a packet
    void send(const string &payload) override;
};

}
