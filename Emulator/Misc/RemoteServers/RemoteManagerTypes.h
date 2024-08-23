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

enum_long(SERVER_TYPE)
{
    SERVER_RSH
};
typedef SERVER_TYPE ServerType;

#ifdef __cplusplus
struct ServerTypeEnum : util::Reflection<ServerTypeEnum, ServerType>
{
    static constexpr long minVal = 0;
    static constexpr long maxVal = SERVER_RSH;

    static const char *prefix() { return "SERVER"; }
    static const char *_key(long value)
    {
        switch (value) {

            case SERVER_RSH:    return "RSH";
        }
        return "???";
    }
};
#endif


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
