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

#include "BasicTypes.h"

namespace vc64 {

struct ServerError : public Error {

    // Sockets
    static constexpr long SOCK_CANT_CREATE           = 1;
    static constexpr long SOCK_CANT_CONNECT          = 2;
    static constexpr long SOCK_CANT_BIND             = 3;
    static constexpr long SOCK_CANT_LISTEN           = 4;
    static constexpr long SOCK_CANT_ACCEPT           = 5;
    static constexpr long SOCK_CANT_RECEIVE          = 6;
    static constexpr long SOCK_CANT_SEND             = 7;
    static constexpr long SOCK_DISCONNECTED          = 8;

    // All servers
    static constexpr long SERVER_PORT_IN_USE         = 10;
    static constexpr long SERVER_ON                  = 11;
    static constexpr long SERVER_OFF                 = 12;
    static constexpr long SERVER_RUNNING             = 13;
    static constexpr long SERVER_NOT_RUNNING         = 14;
    static constexpr long SERVER_NO_CLIENT           = 15;

    // DAP server
    static constexpr long DAP_INVALID_FORMAT         = 20;
    static constexpr long DAP_UNRECOGNIZED_CMD       = 21;
    static constexpr long DAP_UNSUPPORTED_CMD        = 22;
    
    // GDB server
    static constexpr long GDB_NO_ACK                 = 30;
    static constexpr long GDB_INVALID_FORMAT         = 31;
    static constexpr long GDB_INVALID_CHECKSUM       = 32;
    static constexpr long GDB_UNRECOGNIZED_CMD       = 33;
    static constexpr long GDB_UNSUPPORTED_CMD        = 34;

    const char *errstr() const noexcept override {

        switch (payload) {

            case SOCK_CANT_CREATE:            return "SOCK_CANT_CREATE";
            case SOCK_CANT_CONNECT:           return "SOCK_CANT_CONNECT";
            case SOCK_CANT_BIND:              return "SOCK_CANT_BIND";
            case SOCK_CANT_LISTEN:            return "SOCK_CANT_LISTEN";
            case SOCK_CANT_ACCEPT:            return "SOCK_CANT_ACCEPT";
            case SOCK_CANT_RECEIVE:           return "SOCK_CANT_RECEIVE";
            case SOCK_CANT_SEND:              return "SOCK_CANT_SEND";
            case SOCK_DISCONNECTED:           return "SOCK_DISCONNECTED";

            case SERVER_PORT_IN_USE:          return "SERVER_PORT_IN_USE";
            case SERVER_ON:                   return "SERVER_ON";
            case SERVER_OFF:                  return "SERVER_OFF";
            case SERVER_RUNNING:              return "SERVER_RUNNING";
            case SERVER_NOT_RUNNING:          return "SERVER_NOT_RUNNING";
            case SERVER_NO_CLIENT:            return "SERVER_NO_CLIENT";

            case DAP_INVALID_FORMAT:          return "DAP_INVALID_FORMAT";
            case DAP_UNRECOGNIZED_CMD:        return "DAP_UNRECOGNIZED_CMD";
            case DAP_UNSUPPORTED_CMD:         return "DAP_UNSUPPORTED_CMD";

            case GDB_NO_ACK:                  return "GDB_NO_ACK";
            case GDB_INVALID_FORMAT:          return "GDB_INVALID_FORMAT";
            case GDB_INVALID_CHECKSUM:        return "GDB_INVALID_CHECKSUM";
            case GDB_UNRECOGNIZED_CMD:        return "GDB_UNRECOGNIZED_CMD";
            case GDB_UNSUPPORTED_CMD:         return "GDB_UNSUPPORTED_CMD";
        }
        return "???";
    }

    explicit ServerError(long fault, const string &s = "") : Error(fault, s) {

        set_msg(string("ServerError ") + std::to_string(fault) + " (" + errstr() + ").");
    }
};

}
