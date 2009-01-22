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

IEC::IEC()
{
	debug("  Creating IEC bus at address %p...\n", this);

	drive = NULL;
}

IEC::~IEC()
{
	debug("  Releasing IEC bus...\n");
}

void 
IEC::reset()
{
	debug("  Resetting IEC bus...\n");

	connectDrive();
	setDeviceClockPin(1);
	setDeviceDataPin(1);
	busActivity = 0;
}
 
bool
IEC::load(FILE *file)
{
	debug("  Loading IEC state...\n");
	atnLine = (bool)read8(file);
	oldAtnLine = (bool)read8(file);
	clockLine = (bool)read8(file);
	oldClockLine = (bool)read8(file);
	dataLine = (bool)read8(file);
	oldDataLine = (bool)read8(file);
	deviceDataPin = (bool)read8(file);
	deviceClockPin = (bool)read8(file);
	ciaDataPin = (bool)read8(file);
	ciaDataIsOutput = (bool)read8(file);
	ciaClockPin = (bool)read8(file);
	ciaClockIsOutput = (bool)read8(file);
	ciaAtnPin = (bool)read8(file);
	ciaAtnIsOutput = (bool)read8(file);
	return true;
}

bool
IEC::save(FILE *file)
{
	debug("  Saving IEC state...\n");
	write8(file, (int)atnLine);
	write8(file, (int)oldAtnLine);
	write8(file, (int)clockLine);
	write8(file, (int)oldClockLine);
	write8(file, (int)dataLine);
	write8(file, (int)oldDataLine);
	write8(file, (int)deviceDataPin);
	write8(file, (int)deviceClockPin);
	write8(file, (int)ciaDataPin);
	write8(file, (int)ciaDataIsOutput);
	write8(file, (int)ciaClockPin);
	write8(file, (int)ciaClockIsOutput);
	write8(file, (int)ciaAtnPin);
	write8(file, (int)ciaAtnIsOutput);
	return true;
}

void 
IEC::dumpState()
{
	debug("IEC bus\n");
	debug("-------\n");
	debug("\n");
	dumpTrace();
	debug("\n");
	debug("Drive connected : %s\n", driveConnected ? "yes" : "no");
	debug("        old ATN : %d\n", oldAtnLine);
	debug("        old CLK : %d\n", oldClockLine);
	debug("       old DATA : %d\n", oldDataLine);
	debug("\n");	
}

void 
IEC::dumpTrace()
{
	debug("ATN: %s[%s%s%s%s] CLK: %s[%s%s%s%s] DATA: %s[%s%s%s%s]\n", 
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
	getListener()->driveAttachedAction(driveConnected);
}
	
void 
IEC::disconnectDrive()
{ 
	driveConnected = false; 
	getListener()->driveAttachedAction(driveConnected);
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
		if (busActivity == 0)
			getListener()->driveDataAction(true);
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
		if (busActivity == 0)
			getListener()->driveDataAction(false);
	}
}
