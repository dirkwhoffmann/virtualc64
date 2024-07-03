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
#include "RS232.h"
#include "RetroShell.h"
#include "MsgQueue.h"

namespace vc64 {

void
RS232::setPA2(bool value)
{
    // PA2 is used as the TXD line
    // Shortcut for testing...
    // TODO: Start a receive event via the event scheduler
    sendBit(value);
}

void
RS232::sendBit(bool value)
{
    if (counter == 0) {

        if (value == 1) {

            trace(USR_DEBUG, "Ignoring invalid start bit (%d)\n", value);

        } else {

            trace(USR_DEBUG, "Start bit received (%d)\n", value);
            counter++;
        }

    } else if (counter < 9) {

        trace(USR_DEBUG, "Data bit %ld reveived (%d)\n", counter, value);
        value ? SET_BIT(shrOut, counter - 1) : CLR_BIT(shrOut, counter - 1);
        counter++;

    } else if (counter == 9) {

        trace(USR_DEBUG, "Stop bit reveived (%d)\n", value);
        recordOutgoingPacket(shrOut);
        counter = 0;
        shrOut = 0;
    }
}

void
RS232::recordOutgoingPacket(u16 packet)
{
    {   SYNCHRONIZED

        if (isprint((char(packet)))) {
            trace(USR_DEBUG, "Outgoing: '%c'\n", char(packet));
        } else {
            trace(USR_DEBUG, "Outgoing: %02X\n", packet);
        }

        // Record the incoming packet
        outgoing += packet;

        // Inform the GUI if the record buffer had been empty
        if (outgoing.length() == 1) msgQueue.put(MSG_RS232_OUT);

        // Inform RetroShell
        dumpPacket(packet);
    }
}

void 
RS232::dumpPacket(u16 packet)
{
    char c = char(packet);

    if (config.device == COMDEV_RETROSHELL) {

        if (isprint(c) || c == '\n') {
            retroShell << c;
        } else if (c == '\r') {
            retroShell << '\n';
        } else {
            retroShell << "[" + std::to_string(packet) + "]";
        }
    }

    if (config.device == COMDEV_COMMANDER) {

        switch (c) {

            case '\n':
            case '\r':

                retroShell.press(RSKEY_RETURN);
                break;

            default:
                
                if (isprint(c)) retroShell.press(c);
                break;
        }
    }
}

}
