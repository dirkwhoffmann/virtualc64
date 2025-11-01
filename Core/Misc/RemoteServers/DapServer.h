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
#include "DapServerCmds.h"
#include "DapServerTypes.h"

namespace vc64 {

class DapServer final : public SocketServer {

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

    // Processes an DAP command
    void process(const string &packet) throws;

private:

    // Processes a single command (DapServerCmds.cpp)
    template <dap::Command> void process(isize seq, const string &packet) throws;
    // void process(dap::Command cmd, isize seq, const string &packet) throws;

    // Sends a packet to the connected client
    void reply(const string &payload);

    void replySuccess(isize seq, const string &command);

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
