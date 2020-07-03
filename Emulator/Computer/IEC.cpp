// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "C64.h"

IEC::IEC()
{
  	setDescription("IEC");
    debug(3, "  Creating IEC bus at address %p...\n", this);
    
    // Register snapshot items
    SnapshotItem items[] = {
        
        // { &driveIsConnected,    sizeof(driveIsConnected),       CLEAR_ON_RESET },
        { &atnLine,             sizeof(atnLine),                CLEAR_ON_RESET },
        { &clockLine,           sizeof(clockLine),              CLEAR_ON_RESET },
        { &dataLine,            sizeof(dataLine),               CLEAR_ON_RESET },
        { &isDirtyC64Side,      sizeof(isDirtyC64Side),         CLEAR_ON_RESET },
        { &isDirtyDriveSide,    sizeof(isDirtyDriveSide),       CLEAR_ON_RESET },

        { &device1Atn,          sizeof(device1Atn),             CLEAR_ON_RESET },
        { &device1Clock,        sizeof(device1Clock),           CLEAR_ON_RESET },
        { &device1Data,         sizeof(device1Data),            CLEAR_ON_RESET },

        { &device2Atn,          sizeof(device2Atn),             CLEAR_ON_RESET },
        { &device2Clock,        sizeof(device2Clock),           CLEAR_ON_RESET },
        { &device2Data,         sizeof(device2Data),            CLEAR_ON_RESET },

        { &ciaAtn,              sizeof(ciaAtn),                 CLEAR_ON_RESET },
        { &ciaClock,            sizeof(ciaClock),               CLEAR_ON_RESET },
        { &ciaData,             sizeof(ciaData),                CLEAR_ON_RESET },

        { &busActivity,         sizeof(busActivity),            CLEAR_ON_RESET },
        { NULL,                 0,                              0 }};
    
    registerSnapshotItems(items, sizeof(items));
}

IEC::~IEC()
{
	debug(3, "  Releasing IEC bus...\n");
}

void 
IEC::reset()
{
    VirtualComponent::reset();

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
IEC::ping()
{
    VirtualComponent::ping();
    
    c64->putMessage(busActivity > 0 ? MSG_IEC_BUS_BUSY : MSG_IEC_BUS_IDLE);
}

void 
IEC::dump()
{
	msg("IEC bus\n");
	msg("-------\n");
	msg("\n");
	dumpTrace();
	msg("\n");
    msg("    DDRB (VIA1) : %02X (Drive 1)\n", c64->drive1.via1.getDDRB());
    msg("    DDRB (VIA1) : %02X (Drive 2)\n", c64->drive2.via1.getDDRB());
    msg("    DDRA (CIA2) : %02X\n\n", c64->cia2.getDDRA());
    msg("   Bus activity : %d\n", busActivity); 

    msg("\n");
}

void 
IEC::dumpTrace()
{
    debug(1, "ATN: %d CLK: %d DATA: %d\n", atnLine, clockLine, dataLine);
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
    dataLine &= c64->drive1.isPoweredOff() || (atnLine ^ device1Atn);
    dataLine &= c64->drive2.isPoweredOff() || (atnLine ^ device2Atn);

    return (oldAtnLine != atnLine ||
            oldClockLine != clockLine ||
            oldDataLine != dataLine);
}

void
IEC::updateIecLines()
{
	bool signals_changed;

	// Update bus lines
	signals_changed = _updateIecLines();	

    if (signals_changed) {
        
        c64->cia2.updatePA();
        
        // ATN signal is connected to CA1 pin of VIA 1
        c64->drive1.via1.CA1action(!atnLine);
        c64->drive2.via1.CA1action(!atnLine);
        
        if (tracingEnabled()) {
            dumpTrace();
        }
        
		if (busActivity == 0) {
            
            // Reset watchdog counter
            busActivity = 30;
            
			// Bus has just been activated
			c64->putMessage(MSG_IEC_BUS_BUSY);

        } else {
            
            // Reset watchdog counter
            busActivity = 30;
        }
	}
}

void
IEC::updateIecLinesC64Side()
{
    // Get bus signals from C64 side
    uint8_t ciaBits = c64->cia2.getPA();
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
    uint8_t device1Bits = c64->drive1.via1.getPB();
    device1Atn = !!(device1Bits & 0x10);
    device1Clock = !!(device1Bits & 0x08);
    device1Data = !!(device1Bits & 0x02);

    // Get bus signals from drive 2
    uint8_t device2Bits = c64->drive2.via1.getPB();
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
        
		if (--busActivity == 0) {
            
			// Bus goes idle
			c64->putMessage(MSG_IEC_BUS_IDLE);
		}
	}
}

