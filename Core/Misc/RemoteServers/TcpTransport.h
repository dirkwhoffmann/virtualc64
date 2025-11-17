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

    using Transport::Transport;

protected:

    TcpTransport& operator= (const TcpTransport& other) {

        Transport::operator = (other);
        return *this;
    }

    // Sockets
    Socket listener;
    Socket connection;

    // Number of transmitted packages
    isize numSent = 0;
    isize numReceived = 0;


    //
    // Methods from Transport
    //

public:

    virtual void disconnect() override;
    void main(u16 port, const string &endpoint = "") override;

private:
    
    // Inner loops (called from main)
    void mainLoop(u16 port) throws;
    void sessionLoop();


    //
    // Sending
    //

public:

    // Sends a packet
    void send(const string &payload) throws;
    void send(char payload) throws;
    void send(int payload) throws;
    void send(long payload) throws;
    void send(std::stringstream &payload) throws;

    // Operator overloads
    TcpTransport &operator<<(char payload) { send(payload); return *this; }
    TcpTransport &operator<<(const string &payload) { send(payload); return *this; }
    TcpTransport &operator<<(int payload) { send(payload); return *this; }
    TcpTransport &operator<<(long payload) { send(payload); return *this; }
    TcpTransport &operator<<(std::stringstream &payload) { send(payload); return *this; }
};
}
