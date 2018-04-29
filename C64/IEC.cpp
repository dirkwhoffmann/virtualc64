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
        
        { &driveConnected,      sizeof(driveConnected),         CLEAR_ON_RESET },
        { &atnLine,             sizeof(atnLine),                CLEAR_ON_RESET },
        { &oldAtnLine,          sizeof(oldAtnLine),             CLEAR_ON_RESET },
        { &clockLine,           sizeof(clockLine),              CLEAR_ON_RESET },
        { &oldClockLine,        sizeof(oldClockLine),           CLEAR_ON_RESET },
        { &dataLine,            sizeof(dataLine),               CLEAR_ON_RESET },
        { &oldDataLine,         sizeof(oldDataLine),            CLEAR_ON_RESET },
        { &deviceAtnPin,        sizeof(deviceAtnPin),           CLEAR_ON_RESET },
        { &deviceAtnIsOutput,   sizeof(deviceAtnIsOutput),      CLEAR_ON_RESET },
        { &deviceDataPin,       sizeof(deviceDataPin),          CLEAR_ON_RESET },
        { &deviceDataIsOutput,  sizeof(deviceDataIsOutput),     CLEAR_ON_RESET },
        { &deviceClockPin,      sizeof(deviceClockPin),         CLEAR_ON_RESET },
        { &deviceClockIsOutput, sizeof(deviceClockIsOutput),    CLEAR_ON_RESET },
        { &ciaDataPin,          sizeof(ciaDataPin),             CLEAR_ON_RESET },
        { &ciaDataIsOutput,     sizeof(ciaDataIsOutput),        CLEAR_ON_RESET },
        { &ciaClockPin,         sizeof(ciaClockPin),            CLEAR_ON_RESET },
        { &ciaClockIsOutput,    sizeof(ciaClockIsOutput),       CLEAR_ON_RESET },
        { &ciaAtnPin,           sizeof(ciaAtnPin),              CLEAR_ON_RESET },
        { &ciaAtnIsOutput,      sizeof(ciaAtnIsOutput),         CLEAR_ON_RESET },
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
    
    driveConnected = 1;
	atnLine = 1;
	oldAtnLine = 1;
	clockLine = 1;
	oldClockLine = 1;
	dataLine = 1;
	oldDataLine = 1;
	deviceDataPin = 1;
	deviceClockPin = 1;
	ciaDataPin = 1;
	ciaDataIsOutput = 1;
	ciaClockPin = 1;
	ciaClockIsOutput = 1;
	ciaAtnPin = 1;
	ciaAtnIsOutput = 1;
	
    _updateIecLines();
}

void
IEC::ping()
{
    c64->putMessage(driveConnected ? MSG_VC1541_ATTACHED : MSG_VC1541_DETACHED);
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
	msg("Drive connected : %s\n", driveConnected ? "yes" : "no");
	msg("        old ATN : %d\n", oldAtnLine);
	msg("        old CLK : %d\n", oldClockLine);
	msg("       old DATA : %d\n", oldDataLine);
	msg("\n");	
}

void 
IEC::dumpTrace()
{
	debug(1, "ATN: %s[%s%s%s%s] CLK: %s[%s%s%s%s] DATA: %s[%s%s%s%s]\n", 
		  atnLine ? "1 F" : "0 T", 
		  deviceAtnPin ? "1" : "0",
		  deviceAtnIsOutput ? "<-" : "->", 
		  ciaAtnPin ? "1" : "0",
		  ciaAtnIsOutput ? "<-" : "->",
		  clockLine ? "1 F" : "0 T", 
		  deviceClockPin ? "1" : "0",
		  deviceClockIsOutput ? "<-" : "->", 
		  ciaClockPin ? "1" : "0",
		  ciaClockIsOutput ? "<-" : "->",
		  dataLine ? "1 F" : "0 T",
		  deviceDataPin ? "1" : "0",
		  deviceDataIsOutput ? "<-" : "->",
		  ciaDataPin ? "1" : "0",
		  ciaDataIsOutput ? "<-" : "->"); 
}

void 
IEC::connectDrive() 
{ 
	driveConnected = true; 
	c64->putMessage(MSG_VC1541_ATTACHED);
    if (c64->floppy.soundMessagesEnabled())
        c64->putMessage(MSG_VC1541_ATTACHED_SOUND);
}
	
void 
IEC::disconnectDrive()
{
    // Disconnect drive from bus
	driveConnected = false; 
	c64->putMessage(MSG_VC1541_DETACHED);
    if (c64->floppy.soundMessagesEnabled())
        c64->putMessage(MSG_VC1541_DETACHED_SOUND);

    // Switch drive off and on
    c64->floppy.powerUp();
}

bool IEC::_updateIecLines()
{
	// save current values
	oldAtnLine = atnLine;
	oldClockLine = clockLine;
	oldDataLine = dataLine;

	// determine new values
	atnLine = 1;
	if (ciaAtnIsOutput) atnLine &= ciaAtnPin;

	clockLine = 1;
	if (ciaClockIsOutput) clockLine &= ciaClockPin;
	if (driveConnected && deviceClockIsOutput) clockLine &= deviceClockPin;
	
	dataLine = 1;
	if (ciaDataIsOutput) dataLine &= ciaDataPin;
	if (driveConnected && deviceDataIsOutput) dataLine &= deviceDataPin;

	// Note: The device atn pin is not connected to the ATN line.
	// It implements an auto acknowledge feature. When set to 1, the ATN signal
	// is automatically acknowledged by the drive. This feature allows the C64
	// to detect a connected drive without any interaction by the drive itself.
	if (driveConnected && deviceAtnPin == 1)
		dataLine &= atnLine;
    
	// Did any signal change its value?
	return (oldAtnLine != atnLine || oldClockLine != clockLine || oldDataLine != dataLine);	
}

void IEC::updateIecLines()
{
	bool signals_changed;
			
	// Update port lines
	signals_changed = _updateIecLines();	

    // ATN signal is connected to CA1 pin of VIA 1
    c64->floppy.via1.setCA1(!getAtnLine());
    
	if (signals_changed) {
		if (busActivity == 0) {
			// Bus activity detected
			c64->putMessage(MSG_VC1541_DATA_ON);
			c64->setWarp(c64->getAlwaysWarp() || c64->getWarpLoad());
		}
		busActivity = 30;
	}

	if (signals_changed && tracingEnabled()) {
		dumpTrace();
	}
}
	
void IEC::updateCiaPins(uint8_t cia_data, uint8_t cia_direction)
{
	// 0 is dominant on the bus. A single 0-source brings the signal down
	
	ciaAtnIsOutput = (cia_direction & 0x08) ? 1 : 0;
	ciaClockIsOutput = (cia_direction & 0x10) ? 1 : 0;
	ciaDataIsOutput = (cia_direction & 0x20) ? 1 : 0;
	ciaAtnPin = (cia_data & 0x08) ? 0 : 1; // Pin and line are connected via an inverter
	ciaClockPin = (cia_data & 0x10) ? 0 : 1; // Pin and line are connected via an inverter
	ciaDataPin = (cia_data & 0x20) ? 0 : 1; // Pin and line are connected via an inverter
		
	updateIecLines(); 
}

void IEC::updateDevicePins(uint8_t device_data, uint8_t device_direction)
{
	// 0 is dominant on the bus. A single 0-source brings the signal down
	
	deviceAtnIsOutput = (device_direction & 0x10) ? 1 : 0;
	deviceClockIsOutput = (device_direction & 0x08) ? 1 : 0;
	deviceDataIsOutput = (device_direction & 0x02) ? 1 : 0;
	deviceAtnPin = (device_data & 0x10) ? 0 : 1; // Pin and line are connected via an inverter
	deviceClockPin = (device_data & 0x08) ? 0 : 1; // Pin and line are connected via an inverter
	deviceDataPin = (device_data & 0x02) ? 0 : 1; // Pin and line are connected via an inverter
				
	updateIecLines(); 
}

void IEC::execute()
{
	if (busActivity > 0) {

		busActivity--;
		if (busActivity == 0) {
			// Bus is idle 
			c64->putMessage(MSG_VC1541_DATA_OFF);
			c64->setWarp(c64->getAlwaysWarp());
		}
	}
}

