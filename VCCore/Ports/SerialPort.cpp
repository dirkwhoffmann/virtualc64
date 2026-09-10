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

#include "vcconfig.h"
#include "SerialPort.h"
#include "C64.h"

namespace vc64 {

void
SerialPort::_didReset(bool hard)
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

    // A disconnected or idle printer contributes nothing to the wired-AND
    // (a stored value of 1 here would mean the printer pulls CLK/DATA low).
    prtClock = 0;
    prtData = 0;
}

bool SerialPort::_updateIecLines()
{
    // Save current values
    bool oldAtnLine = atnLine;
    bool oldClockLine = clockLine;
    bool oldDataLine = dataLine;
    
    // Compute bus signals (inverted and "wired AND")
    atnLine = !ciaAtn;
    clockLine = !device1Clock && !device2Clock && !prtClock && !ciaClock;
    dataLine = !device1Data && !device2Data && !prtData && !ciaData;
    
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

        // Feed the new line values to the printer's IEC state machine. A
        // disconnected printer must not run the protocol state machine at
        // all (it should behave as if electrically absent from the bus),
        // not merely have its own line contribution masked at iecClock()/
        // iecData() time. It is safe to feed this call the composite bus
        // values unconditionally -- including recomputations caused
        // solely by this device's own prtClock/prtData echoing back
        // through the wired-AND -- because IecListener's EOI-detect arm
        // is edge-triggered (see IecListener.cpp): a call in which CLK
        // and ATN are unchanged from the previous call, which is exactly
        // what a self-echo looks like, cannot arm or otherwise perturb
        // its state machine.
        if (printer.isConnected()) {
            printer.iec.lineTransition(atnLine, clockLine, dataLine);
        }
        
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

    // Get bus signals from the printer
    prtClock = printer.iecClock();
    prtData = printer.iecData();

    // Recompute the bus lines before cancelling the event that brought us
    // here. updateIecLines() can itself call setNeedsUpdate() again (via
    // cia2.updatePA(), or via the printer's iecLinesChanged() callback when
    // its own line contribution changes as a side effect of the transition
    // it was just fed) to ask for a follow-up recomputation. That request
    // is redundant, not lost: this call already reflects the state it
    // would recompute, since cia2.updatePA() and the printer's line
    // feedback both run synchronously inside updateIecLines() itself. If
    // the cancel ran first instead, that redundant self-request would
    // survive and immediately reschedule another pass, which reschedules
    // another on its own signalsChanged, and so on -- a self-sustaining
    // loop with no external cause, observed to hang LOAD indefinitely with
    // a printer connected. Recomputing first and cancelling after removes
    // exactly that spurious follow-up while leaving genuinely new
    // requests -- e.g. the printer's own timer-driven wakeUp(), scheduled
    // via the separate SLOT_PRT slot -- unaffected, since those arrive as
    // later, distinct calls to this function rather than a nested one
    // within the current call.
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
        msgQueue.put(newValue ? Msg::SER_BUSY : Msg::SER_IDLE);
    }
}

}
