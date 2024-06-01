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
#include "SerialPort.h"
#include "C64.h"

namespace vc64 {

void
SerialPort::_reset(bool hard)
{
    atnLine = 1;
    clockLine = 1;
    dataLine = 1;
    
    device1Atn = 1;
    device1Clock = 1;
    device1Data = 1;

    device2Atn = 1;
    device2Clock = 1;
    device2Data = 1;
    
    ciaAtn = 1;
    ciaClock = 1;
    ciaData = 1;
}

bool SerialPort::_updateIecLines()
{
    // Save current values
    bool oldAtnLine = atnLine;
    bool oldClockLine = clockLine;
    bool oldDataLine = dataLine;
    
    // Compute bus signals (inverted and "wired AND")
    atnLine = !ciaAtn;
    clockLine = !device1Clock && !device2Clock && !ciaClock;
    dataLine = !device1Data && !device2Data && !ciaData;
    
    // Auto-acknowdlege logic
    
    /* From the SERVICE MANUAL MODEL 1540/1541 DISK DRIVE (PN-314002-01)
     *
     * "ATN (Attention) is an input on pin 3 of P2 and P3 that is sensed
     *  at PB7 and CA1 of UC3 after being inverted by UA1. ATNA (Attention
     *  Acknowledge) is an output from PB4 of UC3 which is sensed on the data
     *  line pin 5 of P2 and P3 after being exclusively "ored" by UD3 and
     *  inverted by UB1."
     *
     *                        ----
     * ATNA (VIA) -----------|    |    ---
     *               ---     | =1 |---| 1 |o---> & DATA (IEC)
     *  ATN (IEC) --| 1 |o---|    |    ---
     *               ---      ----     UB1
     *               UA1      UD3
     *
     * if (driveIsConnected()) {
     *    bool ua1 = !atnLine;
     *    bool ud3 = ua1 ^ deviceAtn;
     *    bool ub1 = !ud3;
     *    dataLine &= ub1;
     * }
     */
    /*
     dataLine &= !drive8.isPoweredOn() || (atnLine ^ device1Atn);
     dataLine &= !drive9.isPoweredOn() || (atnLine ^ device2Atn);
     */
    if (drive8.connectedAndOn()) dataLine &= (atnLine ^ device1Atn);
    if (drive9.connectedAndOn()) dataLine &= (atnLine ^ device2Atn);
    return (oldAtnLine != atnLine ||
            oldClockLine != clockLine ||
            oldDataLine != dataLine);
}

void
SerialPort::updateIecLines()
{
    bool wasIdle = stats.idle;

    // Update bus lines
    bool signalsChanged = _updateIecLines();

    if (signalsChanged) {
        
        cia2.updatePA();
        
        // Wake up drives
        drive8.wakeUp();
        drive9.wakeUp();
        
        // ATN signal is connected to CA1 pin of VIA 1
        drive8.via1.CA1action(!atnLine);
        drive9.via1.CA1action(!atnLine);

        // Reset the idle counter
        stats.idle = 0;

        // Update the transfer status if the bus was idle
        if (wasIdle) updateTransferStatus();
    }
}

void 
SerialPort::setNeedsUpdate()
{
    c64.scheduleImm<SLOT_SER>(SER_UPDATE);
}

void
SerialPort::update()
{
    // Get bus signals from C64 side
    u8 ciaBits = cia2.getPA();
    ciaAtn = !!(ciaBits & 0x08);
    ciaClock = !!(ciaBits & 0x10);
    ciaData = !!(ciaBits & 0x20);

    // Get bus signals from drive 1
    u8 device1Bits = drive8.via1.getPB();
    device1Atn = !!(device1Bits & 0x10);
    device1Clock = !!(device1Bits & 0x08);
    device1Data = !!(device1Bits & 0x02);

    // Get bus signals from drive 2
    u8 device2Bits = drive9.via1.getPB();
    device2Atn = !!(device2Bits & 0x10);
    device2Clock = !!(device2Bits & 0x08);
    device2Data = !!(device2Bits & 0x02);

    updateIecLines();

    c64.cancel<SLOT_SER>();
}

void
SerialPort::execute()
{
    if (++stats.idle == 32) {
        updateTransferStatus();
    }
}

void
SerialPort::updateTransferStatus()
{
    bool rotating = drive8.isRotating() || drive9.isRotating();
    bool newValue = rotating && stats.idle < 32;
    
    if (transferring != newValue) {

        transferring = newValue;
        msgQueue.put(newValue ? MSG_SER_BUSY : MSG_SER_IDLE);
    }
}

}
