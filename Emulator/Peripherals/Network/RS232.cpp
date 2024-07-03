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
#include "C64.h"

namespace vc64 {

u8 
RS232::getPB() const
{
    return pb;
}

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
    if (txdCnt == 0) {

        if (value == 1) {

            trace(USR_DEBUG, "Ignoring invalid start bit (%d)\n", value);

        } else {

            trace(USR_DEBUG, "Start bit received (%d)\n", value);
            txdCnt++;
        }

    } else if (txdCnt < 9) {

        trace(USR_DEBUG, "Data bit %ld reveived (%d)\n", txdCnt, value);
        value ? SET_BIT(txdShr, txdCnt - 1) : CLR_BIT(txdShr, txdCnt - 1);
        txdCnt++;

    } else if (txdCnt == 9) {

        trace(USR_DEBUG, "Stop bit reveived (%d)\n", value);
        recordOutgoingPacket(txdShr);
        txdCnt = 0;
        txdShr = 0;
    }
}

void
RS232::operator<<(char c)
{
    *this << string{c};
}

void
RS232::operator<<(const string &s)
{
    {   SYNCHRONIZED

        // Add the text
        for (auto &c : s) {

            input += c;
            if (c == '\n') input += '\r';
        }

        // Start the reception process on the C64 side if needed
        if (!c64.hasEvent<SLOT_RXD>()) c64.scheduleImm<SLOT_RXD>(RXD_BIT, 0);
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

void 
RS232::processTxdEvent()
{
    trace(USR_DEBUG, "processTxdEvent\n");
}

void 
RS232::processRxdEvent()
{
    assert(c64.eventid[SLOT_RXD] == RXD_BIT);
    auto payload = c64.data[SLOT_RXD];

    auto send = [&](bool bit) {
        pb = bit ? 0xFF : 0xFE;
        trace(USR_DEBUG, "Sending %d\n", pb);
        // cia2.triggerFallingEdgeOnFlagPin();
    };

    trace(USR_DEBUG, "processRxdEvent(%lld)\n", payload);

    switch (payload) {

        case 0: send(0); cia2.triggerFallingEdgeOnFlagPin(); break;
        case 1: send(0); break;
        case 2: send(1); cia2.triggerFallingEdgeOnFlagPin(); break;
        case 3: send(1); break;
        case 4: send(0); break;
        case 5: send(0); break;
        case 6: send(0); break;
        case 7: send(0); break;
        case 8: send(1); cia2.triggerFallingEdgeOnFlagPin(); break;
        case 9: send(1); break;
        // case 10: send(1); break;
    }

    if (payload == 9) {

        c64.cancel<SLOT_RXD>();

    } else {

        // Schedule the next reception event
        c64.scheduleRel<SLOT_RXD>(982800 / config.baud, RXD_BIT, payload + 1);
    }
}

}
