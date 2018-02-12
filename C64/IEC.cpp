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
    
    // Establish bindings
    drive = &c64->floppy;
    
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
	
	setDeviceClockPin(1);
	setDeviceDataPin(1);
}

void
IEC::ping()
{
    drive->c64->putMessage(driveConnected ? MSG_VC1541_ATTACHED : MSG_VC1541_DETACHED);
    drive->c64->putMessage(busActivity > 0 ? MSG_VC1541_DATA_ON : MSG_VC1541_DATA_OFF );
    
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
	drive->c64->putMessage(MSG_VC1541_ATTACHED);
    if (drive->soundMessagesEnabled())
        drive->c64->putMessage(MSG_VC1541_ATTACHED_SOUND);
}
	
void 
IEC::disconnectDrive()
{
    // Disconnect drive from bus
	driveConnected = false; 
	drive->c64->putMessage(MSG_VC1541_DETACHED);
    if (drive->soundMessagesEnabled())
        drive->c64->putMessage(MSG_VC1541_DETACHED_SOUND);

    // Switch drive off and on
    drive->powerUp();
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
			drive->c64->putMessage(MSG_VC1541_DATA_ON);
			drive->c64->setWarp(drive->c64->getAlwaysWarp() || drive->c64->getWarpLoad());
		}
		busActivity = 30;
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
			drive->c64->putMessage(MSG_VC1541_DATA_OFF);
			drive->c64->setWarp(drive->c64->getAlwaysWarp());
		}
	}
}

// -------------------------------------------------------------------
//                            Fast loader
// -------------------------------------------------------------------

#if 0

uint8_t IEC::IECOutATN(uint8_t byte)
{
    // The upper four bits contain the command
    // -01- : LISTEN
    // -10- : TALK
    // ---0 : on
    // ---1 : off
    // The lower four bits contain the device number
    
    switch (byte >> 4) {
            
        case 2: /* LISTEN */

            debug(2, "Device %d is now listening\n", byte & 0x0F);
            if ((byte & 0x0F) == 8) { // We only support device number 8
                listening = true;
                filename[0] = 0;
                return IEC_OK;
            } else {
                listening = false;
                return IEC_NOTPRESENT;
            }
            
        case 3: /* UNLISTEN */

            debug(2, "No longer listening\n");
            listening = false;
            return IEC_OK;
            
        case 4: /* TALK */

            debug(2, "Device %d is now listening\n", byte & 0x0F);
            if ((byte & 0x0F) == 8) { // We only support device number 8
                talking = true;
                return IEC_OK;
            } else {
                talking = false;
                return IEC_NOTPRESENT;
            }

        case 5: /* UNTALK */
            
            debug(2, "No longer talking\n");
            talking = false;
            return IEC_OK;
    }
    
    return IEC_TIMEOUT;
}

uint8_t IEC::IECOutSec(uint8_t byte)
{
    // byte: xxxx---- : Command to execute
    //       ----xxxx : Secondary address
    
    if (listening) {
        return IECOutSecWhileListening(byte);
    }
    
    if (talking) {
        return IECOutSecWhileTalking(byte);
    }
    
    return IEC_TIMEOUT;
}

uint8_t IEC::IECOutSecWhileListening(uint8_t byte)
{
    command = (byte >> 4);
    secondary = (byte & 0xF);
    
    switch (command) {
        case IEC_CMD_OPEN:
            debug(2, "Received command: OPEN\n");
            return IEC_OK;
        case IEC_CMD_CLOSE:
            debug(2, "Received command: CLOSE\n");
            // Turn on LED
            return IEC_OK;
    }
    return IEC_OK;
}

uint8_t IEC::IECOutSecWhileTalking(uint8_t byte)
{
    command = (byte >> 4);
    secondary = (byte & 0xF);
    return IEC_OK;

}

uint8_t IEC::IECOut(uint8_t byte, bool eoi)
{
    char tmp[2] = { byte, 0 };
    strncat(filename, tmp, 16);
    
    if (eoi) {
        printf("Filename: ");
        for (unsigned i = 0; i < strlen(filename); i++)
            printf("%c", pet2ascii(filename[i]));
        printf("\n");
    }

    return IEC_OK;
}

uint8_t IEC::IECIn(uint8_t *byte)
{
    *byte = 42;
    return IEC_OK;
}

void IEC::IECSetATN()
{
    
}

void IEC::IECRelATN()
{
    
}

void IEC::IECTurnaround()
{
    
}

void IEC::IECRelease()
{
    
}
#endif
