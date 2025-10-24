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
#include "RemoteServer.h"
#include "Emulator.h"
#include "CPU.h"
#include "IOUtils.h"
#include "Memory.h"
#include "MemUtils.h"
#include "MsgQueue.h"
#include "RetroShell.h"

namespace vc64 {

void
SocketServer::_dump(Category category, std::ostream &os) const
{
    using namespace util;

    if (category == Category::State) {

        RemoteServer::_dump(category, os);
        os << tab("Received packets");
        os << dec(numReceived) << std::endl;
        os << tab("Transmitted packets");
        os << dec(numSent) << std::endl;

    } else {

        RemoteServer::_dump(category, os);
    }
}

void
SocketServer::disconnect()
{
    debug(SRV_DEBUG, "Disconnecting...\n");

    // Trigger an exception inside the server thread
    connection.close();
    listener.close();
}

void
SocketServer::main()
{
    try {

        mainLoop();

    } catch (std::exception &err) {

        debug(SRV_DEBUG, "Server thread interrupted\n");
        handleError(err.what());
    }
}

void
SocketServer::mainLoop()
{
    switchState(SrvState::LISTENING);

    while (isListening()) {

        try {

            try {

                // Try to be a client by connecting to an existing server
                connection.connect(config.port);
                debug(SRV_DEBUG, "Acting as a client\n");

            } catch (...) {

                // If there is no existing server, be the server
                debug(SRV_DEBUG, "Acting as a server\n");

                // Create a port listener
                listener.bind(config.port);
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
            if (!isStopping()) handleError(err.what());
        }
    }

    switchState(SrvState::OFF);
}

void
SocketServer::sessionLoop()
{
    switchState(SrvState::CONNECTED);

    numReceived = 0;
    numSent = 0;

    try {

        // Receive and process packets
        while (1) { process(receive()); }

    } catch (std::exception &err) {

        debug(SRV_DEBUG, "Session loop interrupted\n");

        // Handle error if we haven't been interrupted purposely
        if (!isStopping()) {

            handleError(err.what());
            switchState(SrvState::LISTENING);
        }
    }

    numReceived = 0;
    numSent = 0;

    connection.close();
}

string
SocketServer::receive()
{
    string packet;

    if (isConnected()) {

        packet = doReceive();
        msgQueue.put(Msg::SRV_RECEIVE, ++numReceived);
    }

    return packet;
}

void
SocketServer::send(const string &packet)
{
    if (isConnected()) {

        doSend(packet);
        msgQueue.put(Msg::SRV_SEND, ++numSent);
    }
}

void
SocketServer::send(char payload)
{
    send(string(1, payload));
}

void
SocketServer::send(int payload)
{
    send(std::to_string(payload));
}

void
SocketServer::send(long payload)
{
    send(std::to_string(payload));
}


void
SocketServer::send(std::stringstream &payload)
{
    string line;
    while(std::getline(payload, line)) {
        send(line + "\n");
    }
}

void
SocketServer::process(const string &payload)
{
    doProcess(payload);
}

}
