// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "IEC.h"
#include "C64.h"

void 
IEC::_reset(bool hard)
{
    RESET_SNAPSHOT_ITEMS(hard)
    
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
IEC::_dump(Category category, std::ostream& os) const
{
    using namespace util;
    
    if (category == Category::State) {
        
        os << tab("VIA1::DDRB (Drive8)");
        os << hex(drive8.via1.getDDRB()) << std::endl;
        os << tab("VIA1::DDRB (Drive9)");
        os << hex(drive9.via1.getDDRB()) << std::endl;
        os << tab("Idle");
        os << dec(idle) << " frames" << std::endl;
    }
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
IEC::updateIecLines()
{
    bool wasIdle = idle;

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
        idle = 0;
        
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
    if (++idle == 32) {
        updateTransferStatus();
    }    
}

void
IEC::updateTransferStatus()
{
    bool rotating = drive8.isRotating() || drive9.isRotating();
    bool newValue = rotating && idle < 32;
    
    if (transferring != newValue) {
        transferring = newValue;
        msgQueue.put(newValue ? MSG_IEC_BUS_BUSY : MSG_IEC_BUS_IDLE);
    }
}
