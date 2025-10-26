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

enum class GdbCmd : long
{
    Attached,
    C,
    ContQ,
    Cont,
    CtrlC,
    EnableErrorStrings,
    fThreadInfo,
    HostInfo,
    ListThreadsInStopReply,
    MustReplyEmpty,
    Offset,
    ProcessInfo,
    RegisterInfo,
    ShlibInfoAddr,
    StartNoAckMode,
    StructuredDataPlugins,
    sThreadInfo,
    Supported,
    Symbol,
    TfV,
    TfP,
    ThreadExtendedInfo,
    ThreadsInfo,
    ThreadSuffixSupported,
    TStatus,
    VAttachOrWaitSupported,
    Xfer
};

class GdbServer final : public SocketServer {

    // The most recently processed command string
    string latestCmd;

    // Indicates whether received packets should be acknowledged
    bool ackMode = true;


    //
    // Initializing
    //

public:

    using SocketServer::SocketServer;

    GdbServer& operator= (const GdbServer& other) {

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
    // Managing checksums
    //

    // Computes a checksum for a given string
    string computeChecksum(const string &s);

    // Verifies the checksum for a given string
    bool verifyChecksum(const string &s, const string &chk);


    //
    // Handling packets
    //

public:

    // Processes a packet in the format used by GDB
    void process(string packet) throws;

    // Processes a checksum-free packet with the first letter stripped off
    void process(char letter, string packet) throws;

private:

    // Processes a single command (GdbServerCmds.cpp)
    template <char letter> void process(string arg) throws;
    template <char letter, GdbCmd cmd> void process(string arg) throws;

    // Sends a packet with control characters and a checksum attached
    void reply(const string &payload);

    // Sends a JSON packet with control characters and a checksum attached
    void replyJSON(const string &payload);

    // Sends a T response
    void replyT();


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
