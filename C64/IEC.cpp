/*
 * (C) 2006 Dirk W. Hoffmann. All rights reserved.
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

IEC::IEC(C64 *c64)
{
	name = "IEC";
	
	debug(2, "  Creating IEC bus at address %p...\n", this);

    this->c64 = c64;
}

IEC::~IEC()
{
	debug(2, "  Releasing IEC bus...\n");
}

void 
IEC::reset()
{
	debug(2, "  Resetting IEC bus...\n");

    drive = c64->floppy;
    
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
	
	connectDrive();
	setDeviceClockPin(1);
	setDeviceDataPin(1);
	busActivity = 0;
}
 
void
IEC::loadFromBuffer(uint8_t **buffer)
{
	debug(2, "  Loading IEC state...\n");
	driveConnected = (bool)read8(buffer);
	atnLine = (bool)read8(buffer);
	oldAtnLine = (bool)read8(buffer);
	clockLine = (bool)read8(buffer);
	oldClockLine = (bool)read8(buffer);
	dataLine = (bool)read8(buffer);
	oldDataLine = (bool)read8(buffer);
	deviceDataPin = (bool)read8(buffer);
	deviceClockPin = (bool)read8(buffer);
	ciaDataPin = (bool)read8(buffer);
	ciaDataIsOutput = (bool)read8(buffer);
	ciaClockPin = (bool)read8(buffer);
	ciaClockIsOutput = (bool)read8(buffer);
	ciaAtnPin = (bool)read8(buffer);
	ciaAtnIsOutput = (bool)read8(buffer);
}

void
IEC::saveToBuffer(uint8_t **buffer)
{
	debug(2, "  Saving IEC state...\n");
	write8(buffer, (uint8_t)driveConnected);
	write8(buffer, (uint8_t)atnLine);
	write8(buffer, (uint8_t)oldAtnLine);
	write8(buffer, (uint8_t)clockLine);
	write8(buffer, (uint8_t)oldClockLine);
	write8(buffer, (uint8_t)dataLine);
	write8(buffer, (uint8_t)oldDataLine);
	write8(buffer, (uint8_t)deviceDataPin);
	write8(buffer, (uint8_t)deviceClockPin);
	write8(buffer, (uint8_t)ciaDataPin);
	write8(buffer, (uint8_t)ciaDataIsOutput);
	write8(buffer, (uint8_t)ciaClockPin);
	write8(buffer, (uint8_t)ciaClockIsOutput);
	write8(buffer, (uint8_t)ciaAtnPin);
	write8(buffer, (uint8_t)ciaAtnIsOutput);
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
	drive->c64->putMessage(MSG_VC1541_ATTACHED, 1);
}
	
void 
IEC::disconnectDrive()
{ 
	driveConnected = false; 
	drive->c64->putMessage(MSG_VC1541_ATTACHED, 0);
}

bool IEC::_updateIecLines(bool *atnedge)
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
	
	// Check atn line for a negative edge
	if (atnedge != NULL) 
		*atnedge = (oldAtnLine == 1 && atnLine == 0);
	
	// did any signal change its value?
	return (oldAtnLine != atnLine || oldClockLine != clockLine || oldDataLine != dataLine);	
}

void IEC::updateIecLines()
{
	bool signals_changed;
	bool atn_edge;
			
	// Update port lines
	signals_changed = _updateIecLines(&atn_edge);	

	// Check if ATN edge occurred
	if (atn_edge) {
		drive->simulateAtnInterrupt();
	}

	if (signals_changed) {
		if (busActivity == 0) {
			// Bus activity detected
			drive->c64->putMessage(MSG_VC1541_DATA, 1);
			drive->c64->setWarp(drive->c64->getAlwaysWarp() || drive->c64->getWarpLoad());
		}
		busActivity = 10;
	}

	if (signals_changed && tracingEnabled()) {
		dumpTrace();
	}
}
	
void IEC::updateCiaPins(uint8_t cia_data, uint8_t cia_direction)
{
	// Note: On the pyhsical pins, 0 is dominant. 
	// I.e., a single 0-source will bring the signal down to 0
	
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
	// Note: On the pyhsical pins, 0 is dominant. 
	// I.e., a single 0-source will bring the signal down to 0
	
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
			drive->c64->putMessage(MSG_VC1541_DATA, 0);
			drive->c64->setWarp(drive->c64->getAlwaysWarp());
		}
	}
}
