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

namespace vc64 {

class Stdio {

#ifdef _WIN32
    // Manual-reset event used to interrupt a blocking get() call. Stored as a
    // void* so this header does not need to pull in <windows.h>.
    void *wakeEvent = nullptr;
#else
    // Self-pipe used to interrupt the blocking select() inside get()
    int term[2];
#endif

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

    // Stops the server thread before this object's members (in particular
    // stdio, whose pipe the thread may still be blocked on) are torn down.
    // Without this, the implicitly-generated destructor would destroy those
    // members -- and Transport's serverThread -- while the thread could
    // still be running, which makes serverThread's destructor call
    // std::terminate() on a still-joinable thread.
    ~StdioTransport();

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
    void send(const string &payload) override;
};

}
