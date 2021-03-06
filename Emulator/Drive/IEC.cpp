// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "C64.h"

void 
IEC::_reset()
{
    RESET_SNAPSHOT_ITEMS
    
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

void 
IEC::_dump() const
{
	msg("IEC bus\n");
	msg("-------\n");
	msg("\n");
	// dumpTrace();
	msg("\n");
    msg("    DDRB (VIA1) : %02X (Drive 1)\n", drive8.via1.getDDRB());
    msg("    DDRB (VIA1) : %02X (Drive 2)\n", drive9.via1.getDDRB());
    msg("   Bus activity : %d\n", busActivity); 

    msg("\n");
}

void 
IEC::dumpTrace()
{
    trace(IEC_DEBUG, "ATN: %d CLK: %d DATA: %d\n", atnLine, clockLine, dataLine);
}

bool IEC::_updateIecLines()
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
    dataLine &= !drive8.isActive() || (atnLine ^ device1Atn);
    dataLine &= !drive9.isActive() || (atnLine ^ device2Atn);

    return (oldAtnLine != atnLine ||
            oldClockLine != clockLine ||
            oldDataLine != dataLine);
}

void
IEC::updateIecLines()
{
    bool wasIdle = !busActivity;

	// Update bus lines
	bool signalsChanged = _updateIecLines();

    if (signalsChanged) {
        
        cia2.updatePA();
        
        // ATN signal is connected to CA1 pin of VIA 1
        drive8.via1.CA1action(!atnLine);
        drive9.via1.CA1action(!atnLine);
        
        // dumpTrace();
        
        // Reset watchdog timer
        busActivity = 30;

        // Update the transfer status if the bus was idle
        if (wasIdle) updateTransferStatus();
	}
}

void
IEC::updateIecLinesC64Side()
{
    // Get bus signals from C64 side
    u8 ciaBits = cia2.getPA();
    ciaAtn = !!(ciaBits & 0x08);
    ciaClock = !!(ciaBits & 0x10);
    ciaData = !!(ciaBits & 0x20);
    
    updateIecLines();
    isDirtyC64Side = false;
}

void
IEC::updateIecLinesDriveSide()
{
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
    isDirtyDriveSide = false;
}

void
IEC::execute()
{
	if (busActivity > 0) {
        if (--busActivity == 0) updateTransferStatus();
	}
}

void
IEC::updateTransferStatus()
{
    bool rotating = drive8.isRotating() || drive9.isRotating();
    bool newValue = rotating && busActivity > 0;
    
    if (transferring != newValue) {
        transferring = newValue;
        c64.putMessage(newValue ? MSG_IEC_BUS_BUSY : MSG_IEC_BUS_IDLE);
    }
}
