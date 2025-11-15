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

#include "Reflection.h"

namespace httplib { struct Request; struct Response; }

namespace vc64 {

//
// Enumerations
//

enum class SrvState : long
{
    OFF,            // The server is inactive
    WAITING,        // The server is waiting for the launch condition to be met
    STARTING,       // The server is starting up
    LISTENING,      // The server is waiting for a client to connect
    CONNECTED,      // The server is connected to a client
    STOPPING,       // The server is shutting down
    INVALID         // The server is in an error state
};

struct SrvStateEnum : Reflection<SrvStateEnum, SrvState>
{
    static constexpr long minVal = 0;
    static constexpr long maxVal = long(SrvState::INVALID);

    static const char *_key(SrvState value)
    {
        switch (value) {

            case SrvState::OFF:         return "OFF";
            case SrvState::WAITING:     return "WAITING";
            case SrvState::STARTING:    return "STARTING";
            case SrvState::LISTENING:   return "LISTENING";
            case SrvState::CONNECTED:   return "CONNECTED";
            case SrvState::STOPPING:    return "STOPPING";
            case SrvState::INVALID:     return "INVALID";
        }
        return "???";
    }
    static const char *help(SrvState value)
    {
        return "";
    }
};

enum class TransportProtocol
{
    TCP,
    HTTP
};

struct TransportProtocolEnum : Reflection<TransportProtocolEnum, TransportProtocol>
{
    static constexpr long minVal = 0;
    static constexpr long maxVal = long(TransportProtocol::HTTP);

    static const char *_key(TransportProtocol value)
    {
        switch (value) {

            case TransportProtocol::TCP:    return "TCP";
            case TransportProtocol::HTTP:   return "HTTP";
        }
        return "???";
    }
    static const char *help(TransportProtocol value)
    {
        return "";
    }
};

//
// Classes
//

class TransportDelegate {

public:

    virtual ~TransportDelegate() { }

    // State notifications
    virtual void didStart() { };
    virtual void didStop() { };
    virtual void didConnect() { };
    virtual void didDisconnect() { };
    virtual void didSwitch(SrvState from, SrvState to) { };

    // Error notifications
    virtual void didTerminate(const string &error) { };

    // Reception callbacks
    virtual void didReceive(const string &payload) { };
    virtual void didReceive(const httplib::Request &req, httplib::Response &res) { };
};

}
