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

constexpr u32 TXD_MASK = 1 << 2;
constexpr u32 RXD_MASK = 1 << 3;
constexpr u32 RTS_MASK = 1 << 4;
constexpr u32 CTS_MASK = 1 << 5;
constexpr u32 DSR_MASK = 1 << 6;
constexpr u32 CD_MASK  = 1 << 8;
constexpr u32 DTR_MASK = 1 << 20;
constexpr u32 RI_MASK  = 1 << 22;

bool
RS232::getPin(isize nr) const
{
    assert(nr >= 1 && nr <= 25);

    // trace(SER_DEBUG, "getPin(%d) = %d, port = %04X\n", nr, GET_BIT(port, nr), port);
    return GET_BIT(port, nr);
}

void
RS232::setPin(isize nr, bool value)
{
    assert(nr >= 1 && nr <= 25);

    // trace(SER_DEBUG, "setPin(%d,%d), port = %04X\n", nr, value, port);
    setPort(1 << nr, value);
}

void
RS232::setPort(u32 mask, bool value)
{
    u32 oldPort = port;

    // THIS IS FROM VAMIGA. ADAPT FOR THE C64 AT A LATER POINT
    
    /* Emulate the loopback cable (if connected)
     *
     *     Connected pins: A: 2 - 3       (TXD - RXD)
     *                     B: 4 - 5 - 6   (RTS - CTS - DSR)
     *                     C: 8 - 20 - 22 (CD - DTR - RI)
     */
    if (config.device == COMDEV_LOOPBACK) {

        u32 maskA = TXD_MASK | RXD_MASK;
        u32 maskB = RTS_MASK | CTS_MASK | DSR_MASK;
        u32 maskC = CD_MASK | DTR_MASK | RI_MASK;

        if (mask & maskA) mask |= maskA;
        if (mask & maskB) mask |= maskB;
        if (mask & maskC) mask |= maskC;
    }

    // Change the port pins
    if (value) port |= mask; else port &= ~mask;

    // RXD is also connected to the FLAG pin of CIA2
    if (FALLING_EDGE(oldPort & RXD_MASK, port & RXD_MASK)) cia2.triggerFallingEdgeOnFlagPin();
}

u8
RS232::getPB() const
{
    return (port & RXD_MASK) ? 0xFF : 0xFE;
}

void
RS232::setPA2(bool value)
{
    /* The current implementation simply calls the TXD_BIT handler whenever
     * the PA2 is written to. This will work for most terminal programs, but
     * does not emulate the hardware accurately.
     * TODO: Add an accuracy mode that schedules the TXD_BIT events at the
     * proper baud rate.
     */

    c64.scheduleImm<SLOT_TXD>(TXD_BIT, value);
}

Cycle 
RS232::pulseWidth() const
{
    assert(config.baud);
    return vic.getTraits().frequency / config.baud;
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

        for (auto &c : s) {

            // Add a single character
            input += c;

            // Add an additional carriage return to all newline characters
            if (c == '\n') input += '\r';
        }

        // Start the reception process on the C64 side if needed
        if (!c64.hasEvent<SLOT_RXD>()) c64.scheduleImm<SLOT_RXD>(RXD_BIT, 0);
    }
}

std::u16string
RS232::readIncoming()
{
    {   SYNCHRONIZED

        auto result = incoming;
        incoming.clear();
        return result;
    }
}

std::u16string
RS232::readOutgoing()
{
    {   SYNCHRONIZED

        auto result = outgoing;
        outgoing.clear();
        return result;
    }
}

int
RS232::readIncomingByte()
{
    {   SYNCHRONIZED

        if (incoming.empty()) return -1;

        int result = incoming[0];
        incoming.erase(incoming.begin());
        return result;
    }
}

int
RS232::readOutgoingByte()
{
    {   SYNCHRONIZED

        if (outgoing.empty()) return -1;

        int result = outgoing[0];
        outgoing.erase(outgoing.begin());
        return result;
    }
}

int
RS232::readIncomingPrintableByte()
{
    {   SYNCHRONIZED

        while (1) {

            auto byte = readIncomingByte();
            if (byte == -1 || isprint(byte) || byte == '\n' || byte == '\r') return byte;
        }
    }
}

int
RS232::readOutgoingPrintableByte()
{
    {   SYNCHRONIZED

        while (1) {

            auto byte = readOutgoingByte();
            if (byte == -1 || isprint(byte) || byte == '\n' || byte == '\r') return byte;
        }
    }
}

void
RS232::recordIncomingPacket(u16 packet)
{
    {   SYNCHRONIZED

        if (isprint((char(packet)))) {
            trace(USR_DEBUG, "Incoming: '%c'\n", char(packet));
        } else {
            trace(USR_DEBUG, "Incoming: %02X\n", packet);
        }

        // Record the incoming packet
        incoming += packet;

        // Inform the GUI if the record buffer had been empty
        if (incoming.length() == 1) msgQueue.put(MSG_RS232_IN);

        // Inform RetroShell
        dumpPacket(packet);
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
    trace(USR_DEBUG, "processTxdEvent()\n");
    assert(c64.eventid[SLOT_TXD] == TXD_BIT);

    bool value = c64.data[SLOT_TXD];

    if (txdCnt == 0) {

        if (value == 1) {

            trace(USR_DEBUG, "Ignoring invalid start bit (%d)\n", value);

        } else {

            trace(USR_DEBUG, "Start bit received (%d)\n", value);
            txdCnt++;
        }

    } else if (txdCnt < 9) {

        trace(USR_DEBUG, "Data bit %d reveived (%d)\n", txdCnt, value);
        value ? SET_BIT(txdShr, txdCnt - 1) : CLR_BIT(txdShr, txdCnt - 1);
        txdCnt++;

    } else if (txdCnt == 9) {

        trace(USR_DEBUG, "Stop bit reveived (%d)\n", value);
        recordOutgoingPacket(txdShr);
        txdCnt = 0;
        txdShr = 0;
    }

    c64.cancel<SLOT_TXD>();
}

void 
RS232::processRxdEvent()
{
    SYNCHRONIZED

    trace(USR_DEBUG, "processRxdEvent()\n");
    assert(c64.eventid[SLOT_RXD] == RXD_BIT);

    // Stop sending, if there are no characters left
    if (input.empty()) {

        trace(USR_DEBUG, "All characters sent.\n");
        c64.cancel<SLOT_RXD>();
        return;
    }

    // Put the next bit on the RXD line
    if (rxdCnt == 0) {

        trace(USR_DEBUG, "Sending start bit 0...\n");
        setRXD(0);
        rxdCnt++;
    }

    else if (rxdCnt >= 1 && rxdCnt <= 8) {

        bool bit = GET_BIT(input[0], rxdCnt - 1);
        trace(USR_DEBUG, "Sending data bit %d...\n", bit);
        REPLACE_BIT(rxdShr, rxdCnt - 1, bit);
        setRXD(bit);
        rxdCnt++;

    } else if (rxdCnt >= 9) {

        trace(USR_DEBUG, "Sending stop bit 1...\n");
        recordIncomingPacket(rxdShr);
        setRXD(1);
        input.erase(0, 1);
        rxdCnt = 0;
        rxdShr = 0;
    }

    // Schedule the next reception event
    c64.scheduleRel<SLOT_RXD>(pulseWidth(), RXD_BIT);
}

}
