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

protected:

    virtual const char *objectName() const { return "Transport"; }

    // Transport delegate
    TransportDelegate &delegate;

private:

    // The server thread
    std::thread serverThread;

    // The current server state
    SrvState state = SrvState::OFF;


    //
    // Initializing
    //

public:

    Transport(TransportDelegate &delegate) : delegate(delegate) { }
    ~Transport();

    Transport& operator= (const Transport& other) {

        return *this;
    }


    //
    // Examining state
    //

public:

    SrvState getState() const { return state; }
    bool isOff() const { return state == SrvState::OFF; }
    bool isWaiting() const { return state == SrvState::WAITING; }
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

private:

    // Indicates if the server is ready to launch
    virtual bool canRun() { return true; }


    //
    // Running the server
    //

public:

    // The main thread function
    virtual void main(u16 port, const string &endpoint = "") throws { };
};

}
