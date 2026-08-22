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

#include "RemoteServerTypes.h"

namespace vc64 {

//
// Enumerations
//

enum class ServerType : long
{
    RSH,
    RPC,
    DAP,
    PROM
};

struct ServerTypeEnum : Reflectable<ServerTypeEnum, ServerType>
{
    static constexpr long minVal = 0;
    static constexpr long maxVal = long(ServerType::PROM);

    static const char *_key(ServerType value)
    {
        switch (value) {
                
            case ServerType::RSH:    return "RSH";
            case ServerType::RPC:    return "RPC";
            case ServerType::DAP:    return "DAP";
            case ServerType::PROM:   return "PROM";
        }
        return "???";
    }
    static const char *help(ServerType value)
    {
        switch (value) {
                
            case ServerType::RSH:    return "Remote shell server";
            case ServerType::RPC:    return "JSON RPC server";
            case ServerType::DAP:    return "Debug adapter";
            case ServerType::PROM:   return "Prometheus server";
        }
        return "???";
    }
};


enum class TrafficDirection : long
{
    RECEIVED,       // The packet was received from a client
    SENT            // The packet was sent to a client
};

struct TrafficDirectionEnum : Reflectable<TrafficDirectionEnum, TrafficDirection>
{
    static constexpr long minVal = 0;
    static constexpr long maxVal = long(TrafficDirection::SENT);

    static const char *_key(TrafficDirection value)
    {
        switch (value) {

            case TrafficDirection::RECEIVED:    return "RECEIVED";
            case TrafficDirection::SENT:        return "SENT";
        }
        return "???";
    }
    static const char *help(TrafficDirection value)
    {
        switch (value) {

            case TrafficDirection::RECEIVED:    return "Received from a client";
            case TrafficDirection::SENT:        return "Sent to a client";
        }
        return "???";
    }
};


//
// Structures
//

typedef struct
{
    RemoteServerInfo rshInfo;
    RemoteServerInfo rpcInfo;
    RemoteServerInfo dapInfo;
    RemoteServerInfo promInfo;
}
RemoteManagerInfo;

/* A single entry of the traffic log. The remote servers record all transmitted
 * packets in a bounded log inside the RemoteManager. Each recorded packet is
 * assigned a monotonically increasing sequence number which is passed to the
 * GUI in the Msg::SRV_RECEIVE and Msg::SRV_SEND notification messages. The GUI
 * reads the log incrementally by passing the sequence number of the last seen
 * entry to RemoteManagerAPI::getTraffic().
 */
typedef struct
{
    // Sequence number (unique, monotonically increasing)
    isize nr;

    // The server that received or sent the packet
    ServerType server;

    // Tells whether the packet was received or sent
    TrafficDirection direction;

    // Host time stamp (milliseconds since epoch)
    i64 time;

    // Packet contents
    string payload;
}
TrafficEntry;

}
