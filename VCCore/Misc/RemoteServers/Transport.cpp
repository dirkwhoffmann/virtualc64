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

#include "vcconfig.h"
#include "Transport.h"
#include "RemoteServer.h"

namespace vc64 {

Transport::Transport(RemoteServer &server) : server(server), delegate(server) {

}

Transport::~Transport() {

    logme(LOG_SRV, "Shutting down\n");
}

void
Transport::record(TrafficDirection direction, const string &payload)
{
    server.recordTraffic(direction, payload);
}

void
Transport::deliver(const string &payload)
{
    record(TrafficDirection::RECEIVED, payload);
    delegate.didReceive(payload);
}

void
Transport::start(u16 port, const string &endpoint)
{
    if (!isOff()) return;

    logme(LOG_SRV, "Starting server...\n");
    switchState(SrvState::STARTING);

    // Make sure we continue with a terminated server thread
    if (serverThread.joinable()) serverThread.join();

    // Clear any stop request left over from the previous run
    stopRequested = false;

    // Spawn a new thread
    serverThread = std::thread(&Transport::main, this, port, endpoint);
}

void
Transport::stop()
{
    // Guard against re-entrant calls while a stop is already in progress,
    // but not against a thread that has already exited on its own (e.g.,
    // after an EOF on stdin) and self-switched to OFF from inside main() --
    // a std::thread stays joinable until join() is actually called, and an
    // un-joined joinable thread makes serverThread's destructor call
    // std::terminate() when this object is torn down.
    if (isStopping()) return;

    /* Raise the stop request before anything else, and unconditionally --
     * even when the state still reads OFF. The server thread may be between
     * being spawned and having switched itself to LISTENING, in which case
     * neither the state nor disconnect() reaches it (see stopRequested).
     */
    stopRequested = true;

    if (!isOff()) {

        logme(LOG_SRV, "Stopping server...\n");
        switchState(SrvState::STOPPING);

        // Interrupt the server thread
        disconnect();
    }

    // Wait until the server thread has terminated
    if (serverThread.joinable()) serverThread.join();

    switchState(SrvState::OFF);
}

void
Transport::disconnect()
{

}

void
Transport::switchState(SrvState newState)
{
    auto oldState = state;

    if (oldState != newState) {

        logme(LOG_SRV, "Switching state: %s -> %s\n",
              SrvStateEnum::key(state), SrvStateEnum::key(newState));

        // Switch state
        state = newState;

        // Inform the delegate
        delegate.didSwitch(oldState, newState);

        if (oldState == SrvState::STARTING && newState == SrvState::LISTENING) {
            delegate.didStart();
        }
        if (newState == SrvState::OFF) {
            delegate.didStop();
        }
        if (newState == SrvState::CONNECTED) {
            delegate.didConnect();
        }
        if (oldState == SrvState::CONNECTED) {
            delegate.didDisconnect();
        }

        // Inform the GUI
        // msgQueue.put(Msg::SRV_STATE, (i64)newState); MOVE TO DELEGATE
    }
}

/*
void
Transport::send(char payload)
{
    send(string(1, payload));
}

void
Transport::send(int payload)
{
    send(std::to_string(payload));
}

void
Transport::send(long payload)
{
    send(std::to_string(payload));
}

void
Transport::send(std::stringstream &payload)
{
    string line;
    while(std::getline(payload, line)) {
        send(line + "\n");
    }
}
*/

}
