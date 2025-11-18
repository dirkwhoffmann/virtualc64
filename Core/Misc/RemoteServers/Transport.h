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

#include "CoreObject.h"
#include "TransportTypes.h"
#include <thread>

namespace vc64 {

class Transport : public CoreObject {

    // The server thread
    std::thread serverThread;

protected:

    // Transport delegate
    TransportDelegate &delegate;

    // The current server state
    SrvState state = SrvState::OFF;


    //
    // Initializing
    //

public:

    Transport(TransportDelegate &delegate) : delegate(delegate) { }
    ~Transport();

    virtual const char *objectName() const override { return "Transport"; }

    Transport& operator= (const Transport& other) {

        return *this;
    }


    //
    // Examining state
    //

public:

    SrvState getState() const { return state; }
    bool isOff() const { return state == SrvState::OFF; }
    bool isStarting() const { return state == SrvState::STARTING; }
    bool isListening() const { return state == SrvState::LISTENING; }
    bool isConnected() const { return state == SrvState::CONNECTED; }
    bool isStopping() const { return state == SrvState::STOPPING; }
    bool isErroneous() const { return state == SrvState::INVALID; }


    //
    // Starting and stopping the server
    //

public:

    // Launch the remote server
    virtual void start(u16 port, const string &endpoint = "") throws;

    // Shuts down the remote server
    virtual void stop() throws;

    // Disconnects the client
    virtual void disconnect() throws = 0;

    // Switches the internal state
    void switchState(SrvState newState);

    // The main thread function
    virtual void main(u16 port, const string &endpoint = "") throws { };


    //
    // Sending packets
    //

public:

    // Sends a packet
    virtual void send(const string &payload) = 0;
    /*
    void send(char payload);
    void send(int payload);
    void send(long payload);
    void send(std::stringstream &payload);
    */

    // Operator overloads
    Transport &operator<<(const string &payload) { send(payload); return *this; }
    /*
    Transport &operator<<(char payload) { send(payload); return *this; }
    Transport &operator<<(int payload) { send(payload); return *this; }
    Transport &operator<<(long payload) { send(payload); return *this; }
    Transport &operator<<(std::stringstream &payload) { send(payload); return *this; }
    */
};

}
