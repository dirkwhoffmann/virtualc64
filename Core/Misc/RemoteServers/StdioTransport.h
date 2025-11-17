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
#include <unistd.h>
#include <string>
#include <sstream>

namespace vc64 {

class Stdio {

    // Pipe for terminating the blocking stdio read call
    int term[2];

public:

    Stdio();
    ~Stdio();

    // Terminates a blocking read
    void terminate();

    // Blocking read from stdin
    string get();

    // Write to stdout
    void put(const string &str);

    // Overloads
    Stdio& operator<<(const std::string &s) { put(s); return *this; }
    Stdio& operator>>(std::string &s) { s = get(); return *this; }
};

class StdioTransport : public Transport {

    Stdio stdio;

    using Transport::Transport;

    StdioTransport& operator=(const StdioTransport &other) {

        Transport::operator=(other);
        return *this;
    }


    //
    // Methods from Transport
    //

public:

    virtual void disconnect() override;
    void main(u16 port = 0, const string &endpoint = "") override;

private:

    // Inner loop (called from main)
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
    StdioTransport &operator<<(char payload) { send(payload); return *this; }
    StdioTransport &operator<<(const string &payload) { send(payload); return *this; }
    StdioTransport &operator<<(int payload) { send(payload); return *this; }
    StdioTransport &operator<<(long payload) { send(payload); return *this; }
    StdioTransport &operator<<(std::stringstream &payload) { send(payload); return *this; }
};

}
