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
#include "TcpTransport.h"

namespace vc64 {

void
TcpTransport::disconnect()
{
    debug(SRV_DEBUG, "Disconnecting...\n");

    // Trigger an exception inside the server thread
    connection.close();
    listener.close();
}

void
TcpTransport::main(u16 port, const string &endpoint)
{
    try {

        mainLoop(port);

    } catch (std::exception &err) {

        debug(SRV_DEBUG, "Server thread interrupted\n");
        delegate.didTerminate(err.what());
        // handleError(err.what());
    }
}

void
TcpTransport::mainLoop(u16 port)
{
    switchState(SrvState::LISTENING);

    while (isListening()) {

        try {

            try {

                // Try to be a client by connecting to an existing server
                connection.connect(port);
                debug(SRV_DEBUG, "Acting as a client\n");

            } catch (...) {

                // If there is no existing server, be the server
                debug(SRV_DEBUG, "Acting as a server\n");

                // Create a port listener
                listener.bind(port);
                listener.listen();

                // Wait for a client to connect
                connection = listener.accept();
            }

            // Handle the session
            sessionLoop();

            // Close the port listener
            listener.close();

        } catch (std::exception &err) {

            debug(SRV_DEBUG, "Main loop interrupted\n");

            // Handle error if we haven't been interrupted purposely
            if (!isStopping()) delegate.didTerminate(err.what()); // handleError(err.what());
        }
    }

    switchState(SrvState::OFF);
}

void
TcpTransport::sessionLoop()
{
    switchState(SrvState::CONNECTED);

    numReceived = 0;
    numSent = 0;

    try {

        // Receive and process packets
        while (1) { delegate.didReceive(connection.recv()); }

    } catch (std::exception &err) {

        debug(SRV_DEBUG, "Session loop interrupted\n");

        // Handle error if we haven't been interrupted purposely
        if (!isStopping()) {

            delegate.didTerminate(err.what());
            switchState(SrvState::LISTENING);
        }
    }

    numReceived = 0;
    numSent = 0;

    connection.close();
}

/*
string
TcpTransport::receive()
{
    string payload;

    if (isConnected()) {

        payload = connection.recv();
        delegate.didReceive(payload);
    }

    return payload;
}
*/

void
TcpTransport::send(const string &payload)
{
    if (isConnected()) {

        connection.send(payload);

        // doSend(packet);
        // msgQueue.put(Msg::SRV_SEND, ++numSent);
    }
}

void
TcpTransport::send(char payload)
{
    send(string(1, payload));
}

void
TcpTransport::send(int payload)
{
    send(std::to_string(payload));
}

void
TcpTransport::send(long payload)
{
    send(std::to_string(payload));
}


void
TcpTransport::send(std::stringstream &payload)
{
    string line;
    while(std::getline(payload, line)) {
        send(line + "\n");
    }
}

}
