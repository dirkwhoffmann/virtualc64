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

#include "config.h"
#include "DapServer.h"
#include "Emulator.h"
#include "CPU.h"
#include "Memory.h"
#include "MsgQueue.h"

namespace vc64 {

void
DapServer::process(string package)
{
    debug(SRV_DEBUG, "process(%s)\n", package.c_str());

    /*
    // Check if the previous package has been rejected
    if (package[0] == '-') throw AppError(Fault::GDB_NO_ACK);

    // Strip off the acknowledgment symbol if present
    if (package[0] == '+') package.erase(0,1);

    if (auto len = package.length()) {

        // Check for Ctrl+C
        if (package[0] == 0x03) {
            process <' ', GdbCmd::CtrlC> ("");
            return;
        }

        // Check for '$x[...]#xx'
        if (package[0] == '$' && len >= 5 && package[len - 3] == '#') {

            auto cmd = package[1];
            auto arg = package.substr(2, len - 5);
            auto chk = package.substr(len - 2, 2);

            if (verifyChecksum(package.substr(1, len - 4), chk)) {

                latestCmd = package;

                if (ackMode) send("+");
                process(cmd, arg);

            } else {

                if (ackMode) connection.send("-");
                throw AppError(Fault::GDB_INVALID_CHECKSUM);
            }

            return;
        }

        throw AppError(Fault::GDB_INVALID_FORMAT);
    }
    */
    throw AppError(Fault::DAP_INVALID_FORMAT);
}

}
