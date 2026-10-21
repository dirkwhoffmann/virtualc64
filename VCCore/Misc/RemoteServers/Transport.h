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
#include "RemoteManagerTypes.h"
#include <atomic>
#include <thread>

namespace vc64 {

class RemoteServer;

class Transport : public CoreObject {

    // The server thread
    std::thread serverThread;

protected:

    // The remote server owning this transport (used for traffic recording)
    RemoteServer &server;

    // Transport delegate (the owning server)
    TransportDelegate &delegate;

    // The current server state
    SrvState state = SrvState::OFF;

    // Sticky "go away" flag for the server thread
    std::atomic<bool> stopRequested { false };


    //
    // Initializing
    //

public:

    Transport(RemoteServer &server);
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
    bool isWaiting() const { return state == SrvState::WAITING; }
    bool isStarting() const { return state == SrvState::STARTING; }
    bool isListening() const { return state == SrvState::LISTENING; }
    bool isConnected() const { return state == SrvState::CONNECTED; }
    bool isStopping() const { return state == SrvState::STOPPING; }
    bool isErroneous() const { return state == SrvState::INVALID; }

    // Whether the server thread should wind down. Server threads must test
    // this rather than isStopping() alone -- see stopRequested.
    bool terminating() const { return stopRequested || isStopping(); }


    //
    // Starting and stopping the server
    //

public:

    // Arms the server: it is enabled, but waits for its launch condition
    void wait();

    // Launch the remote server
    virtual void start(u16 port, const string &endpoint = "");

    // Shuts down the remote server
    virtual void stop();

    // Disconnects the client
    virtual void disconnect() = 0;

    // Switches the internal state
    void switchState(SrvState newState);

    // The main thread function
    virtual void main(u16 port, const string &endpoint = "") { };


    //
    // Sending and receiving packets
    //

public:

    // Sends a packet
    virtual void send(const string &payload) = 0;

    // Operator overloads
    Transport &operator<<(const string &payload) { send(payload); return *this; }

protected:

    /* Traffic recording. Every transport is expected to route received
     * packets through deliver() and to call record() for every packet it
     * actually transmits. Both functions record the packet in the traffic
     * log of the RemoteManager, which informs the GUI via Msg::SRV_RECEIVE
     * and Msg::SRV_SEND.
     */

    // Records a packet in the traffic log
    void record(TrafficDirection direction, const string &payload);

    // Records a received packet and forwards it to the delegate
    void deliver(const string &payload);
};

}
