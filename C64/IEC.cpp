/*
 * (C) 2006 - 2018 Dirk W. Hoffmann. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "C64.h"

IEC::IEC()
{
  	setDescription("IEC");
    debug(3, "  Creating IEC bus at address %p...\n", this);
    
    // Register snapshot items
    SnapshotItem items[] = {
        
        { &driveIsConnected,    sizeof(driveIsConnected),       CLEAR_ON_RESET },
        { &atnLine,             sizeof(atnLine),                CLEAR_ON_RESET },
        { &clockLine,           sizeof(clockLine),              CLEAR_ON_RESET },
        { &dataLine,            sizeof(dataLine),               CLEAR_ON_RESET },
        { &isDirty,             sizeof(isDirty),                CLEAR_ON_RESET },
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
    
    driveIsConnected = 1;
    atnLine = 1;
    clockLine = 1;
    dataLine = 1;
}

void
IEC::ping()
{
    VirtualComponent::ping();
    c64->putMessage(driveIsConnected ? MSG_VC1541_ATTACHED : MSG_VC1541_DETACHED);
    c64->putMessage(busActivity > 0 ? MSG_VC1541_DATA_ON : MSG_VC1541_DATA_OFF );
}

void 
IEC::dumpState()
{
	msg("IEC bus\n");
	msg("-------\n");
	msg("\n");
	dumpTrace();
	msg("\n");
	msg("Drive connected : %s\n", driveIsConnected ? "yes" : "no");
    msg("    DDRB (VIA1) : %02X\n", c64->floppy.via1.getDDRB());
    msg("    DDRA (CIA2) : %02X\n", c64->cia2.DDRA);

    msg("\n");
}

void 
IEC::dumpTrace()
{
    debug(1, "ATN: %d CLK: %d DATA: %d\n", atnLine, clockLine, dataLine);
}

void 
IEC::connectDrive() 
{ 
	driveIsConnected = true;
	c64->putMessage(MSG_VC1541_ATTACHED);
    if (c64->floppy.soundMessagesEnabled())
        c64->putMessage(MSG_VC1541_ATTACHED_SOUND);
}
	
void 
IEC::disconnectDrive()
{
    // Disconnect drive from bus
	driveIsConnected = false;
	c64->putMessage(MSG_VC1541_DETACHED);
    if (c64->floppy.soundMessagesEnabled())
        c64->putMessage(MSG_VC1541_DETACHED_SOUND);

    // Switch drive off and on
    c64->floppy.powerUp();
}

bool IEC::_updateIecLines()
{
    // Save current values
    bool oldAtnLine = atnLine;
    bool oldClockLine = clockLine;
    bool oldDataLine = dataLine;
    
    // Get bus signals from device side
    uint8_t deviceBits = c64->floppy.via1.getPB();
    bool deviceAtn = !!(deviceBits & 0x10);
    bool deviceClock = !!(deviceBits & 0x08);
    bool deviceData = !!(deviceBits & 0x02);

    // Get bus signals from c64 side
    uint8_t ciaBits = c64->cia2.PA;
    bool ciaAtn = !!(ciaBits & 0x08);
    bool ciaClock = !!(ciaBits & 0x10);
    bool ciaData = !!(ciaBits & 0x20);
    
    // Compute bus signals (inverted and "wired AND")
    atnLine = !ciaAtn;
    clockLine = !deviceClock && !ciaClock;
    dataLine = !deviceData && !ciaData;
    
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
    dataLine &= !driveIsConnected || (atnLine ^ deviceAtn);
    
    isDirty = false;
    return (oldAtnLine != atnLine ||
            oldClockLine != clockLine ||
            oldDataLine != dataLine);
}

void IEC::updateIecLines()
{
	bool signals_changed;
			
	// Update port lines
	signals_changed = _updateIecLines();	
    
    // ATN signal is connected to CA1 pin of VIA 1
    c64->floppy.via1.setCA1late(!atnLine);
    
	if (signals_changed) {
        
        if (tracingEnabled()) {
            dumpTrace();
        }
        
		if (busActivity == 0) {
            
			// Bus has just been activated
			c64->putMessage(MSG_VC1541_DATA_ON);
			c64->setWarp(c64->getAlwaysWarp() || c64->getWarpLoad());
		}
        
        // Reset watchdog counter
		busActivity = 30;
	}
}

void IEC::execute()
{
	if (busActivity > 0) {
		if (--busActivity == 0) {
			// Bus is idle 
			c64->putMessage(MSG_VC1541_DATA_OFF);
			c64->setWarp(c64->getAlwaysWarp());
		}
	}
}

