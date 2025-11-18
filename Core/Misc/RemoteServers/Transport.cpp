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

#include "config.h"
#include "Transport.h"

namespace vc64 {

Transport::~Transport() {

    debug(SRV_DEBUG, "Shutting down\n");
}

void
Transport::start(u16 port, const string &endpoint)
{
    if (!isOff()) return;

    debug(SRV_DEBUG, "Starting server...\n");
    switchState(SrvState::STARTING);

    // Make sure we continue with a terminated server thread
    if (serverThread.joinable()) serverThread.join();

    // Spawn a new thread
    serverThread = std::thread(&Transport::main, this, port, endpoint);
}

void
Transport::stop()
{
    if (isOff() || isStopping()) return;

    debug(SRV_DEBUG, "Stopping server...\n");
    switchState(SrvState::STOPPING);

    // Interrupt the server thread
    disconnect();

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

        debug(SRV_DEBUG, "Switching state: %s -> %s\n",
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
