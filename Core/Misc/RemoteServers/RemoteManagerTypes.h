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

struct ServerTypeEnum : Reflection<ServerTypeEnum, ServerType>
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


//
// Structures
//

typedef struct
{
    isize numLaunching;
    isize numListening;
    isize numConnected;
    isize numErroneous;

    RemoteServerInfo rshInfo;
    RemoteServerInfo rpcInfo;
    RemoteServerInfo dapInfo;
    RemoteServerInfo promInfo;
}
RemoteManagerInfo;

}
