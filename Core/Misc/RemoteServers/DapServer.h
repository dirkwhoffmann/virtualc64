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

#include "SocketServer.h"

namespace vc64 {

enum class DapCmd
{
    Init
};

class DapServer final : public SocketServer {

    // The most recently processed command string
    // string latestCmd;


    //
    // Initializing
    //

public:

    using SocketServer::SocketServer;

    DapServer& operator= (const DapServer& other) {

        SocketServer::operator = (other);
        return *this;
    }


    //
    // Methods from CoreObject
    //

private:

    void _dump(Category category, std::ostream &os) const override;
    void _pause() override;


    //
    // Methods from RemoteServer
    //

public:

    bool shouldRun() override;
    string doReceive() throws override;
    void doSend(const string &payload) throws override;
    void doProcess(const string &payload) throws override;
    void didStart() override;
    void didStop() override;
    void didConnect() override;


    //
    // Handling packets
    //

public:

    // Processes an incoming DAP packet
    void process(string packet) throws;

    // Processes a checksum-free packet with the first letter stripped off
    // void process(char letter, string packet) throws;

private:

    // Processes a single command (DapServerCmds.cpp)
    template <DapCmd cmd> void process(string arg) throws;

    // Sends a packet to the connected client
    void reply(const string &payload);


    //
    // Reading the emulator state
    //

    // Reads a register value
    string readRegister(isize nr);

    // Reads a byte from memory
    string readMemory(isize addr);


    //
    // Delegation methods
    //

public:

    void breakpointReached();
};

}
