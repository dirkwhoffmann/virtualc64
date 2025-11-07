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

// #include "Reflection.h"

namespace vc64::dap {

enum Type {

    Request,
    Response,
    Event
};

enum Command {

    BreakpointLocations,
    ConfigurationDone,
    Initialize,
    Launch,
    SetBreakpoints
};

}

#if 0

//
// Enumerations
//

enum class Type : long
{
    REQUEST,
    RESPONSE,
    EVENT
};

/*
static const std::unordered_map<std::string, Type> stringToType = {

    { "REQUEST",  Type::REQUEST },
    { "RESPONSE", Type::RESPONSE },
    { "EVENT",    Type::EVENT }
};
*/

struct TypeEnum : Reflection<TypeEnum, Type>
{
    static constexpr long minVal = 0;
    static constexpr long maxVal = long(Type::EVENT);

    static const char *_key(Type value)
    {
        switch (value) {

            case Type::REQUEST:  return "request";
            case Type::RESPONSE: return "response";
            case Type::EVENT:    return "event";
        }
        return "???";
    }
    static const char *help(Type value)
    {
        switch (value) {

            case Type::REQUEST:  return "A client or debug adapter initiated request";
            case Type::RESPONSE: return "Response for a request";
            case Type::EVENT:    return "A debug adapter initiated event";
        }
        return "???";
    }
};

enum class Command : long
{
    initialize,
    launch,
    setBreakpoints
};

struct CommandEnum : Reflection<CommandEnum, Command>
{
    static constexpr long minVal = 0;
    static constexpr long maxVal = long(Command::setBreakpoints);

    static const char *_key(Command value)
    {
        switch (value) {

            case Command::initialize:       return "initialize";
            case Command::launch:           return "launch";
            case Command::setBreakpoints:   return "setBreakpoints";
        }
        return "???";
    }
    static const char *help(Command value)
    {
        switch (value) {

            case Command::initialize:       return "DAP Request";
            case Command::launch:           return "DAP Request";
            case Command::setBreakpoints:   return "DAP Request";
        }
        return "???";
    }
};

}
#endif

