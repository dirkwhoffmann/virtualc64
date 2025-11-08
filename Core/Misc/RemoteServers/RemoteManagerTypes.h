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

namespace vc64 {

//
// Enumerations
//

enum class ServerType : long
{
    RPC,
    DAP,
    GDB,
    RSH
};

struct ServerTypeEnum : Reflection<ServerTypeEnum, ServerType>
{
    static constexpr long minVal = 0;
    static constexpr long maxVal = long(ServerType::RSH);

    static const char *_key(ServerType value)
    {
        switch (value) {
                
            case ServerType::RPC:    return "RPC";
            case ServerType::DAP:    return "DAP";
            case ServerType::GDB:    return "GDB";
            case ServerType::RSH:    return "RSH";
        }
        return "???";
    }
    static const char *help(ServerType value)
    {
        switch (value) {
                
            case ServerType::RPC:    return "JSON RPC server";
            case ServerType::DAP:    return "Debug adapter";
            case ServerType::GDB:    return "Debug server";
            case ServerType::RSH:    return "Remote shell server";
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
}
RemoteManagerInfo;

}
