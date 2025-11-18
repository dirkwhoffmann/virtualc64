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

#include "TransportTypes.h"

namespace vc64 {

//
// Structures
//

typedef struct
{
    // Enable status
    bool enable;

    // The transport protocol of this server
    TransportProtocol transport;

    // The socket port of this server
    u16 port;

    // If true, transmitted packets are shown in RetroShell
    bool verbose;
}
ServerConfig;

typedef struct
{
    SrvState state;
}
RemoteServerInfo;

}
