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

// Last review:

#ifndef _IEC_INC
#define _IEC_INC

#include "VirtualComponent.h"

#define IEC_READY 0
#define IEC_ATTENTION 1
#define IEC_READY_TO_SEND 2
#define IEC_READY_FOR_DATA 3
#define IEC_READY_FOR_FINAL_BYTE 4
#define IEC_BYTE_RECEIVED 5
#define IEC_EOI 6
#define IEC_READY_TO_ANSWER 7
#define IEC_SENDING_ANSWER 8
#define IEC_ACKNOWLEDGE_TALKER 9
#define IEC_ACK_TALKER2 10

#define IEC_RECEIVING 1
#define IEC_SENDING 2

// Forward declarations
class CIA2;
class VC1541;

class IEC : public VirtualComponent {

public:
	
	//! Reference to the virtual disc drive
	VC1541 *drive;

private:

	//! True, iff drive is connected to the IEC bus
	bool driveConnected;
	
	//! Current value of the IEC bus atn line	
	bool atnLine;

	//! Previous value of the IEC bus atn line	
	bool oldAtnLine;

	//! Current value of the IEC bus clock line	
	bool clockLine;

	//! Previous value of the IEC bus clock line	
	bool oldClockLine;

	//! Current value of the IEC bus data line
	bool dataLine;

	//! Previous value of the IEC bus data line
	bool oldDataLine;
	 	
	//! Current value of the atn pin of the connected external device
	bool deviceAtnPin;

	//! True, iff the device atn pin is configured as output
	bool deviceAtnIsOutput;

	//! Current value of the data pin of the connected external device
	bool deviceDataPin;

	//! True, iff the device data pin is configured as output
	bool deviceDataIsOutput;
		
	//! Current value of the clock pin of the connected external device
	bool deviceClockPin;

	//! True, iff the device clock pin is configured as output
	bool deviceClockIsOutput;
	
	//! Current value of the data pin of the connected CIA chip
	bool ciaDataPin;
	
	//! True, iff the CIA data pin is configured as output
	bool ciaDataIsOutput;
	
	//! Current value of the clock pin of the connected CIA chip
	bool ciaClockPin;

	//! True, iff the CIA clock pin is configured as output
	bool ciaClockIsOutput;
	
	//! Current value of the ATN pin of the connected CIA chip
	bool ciaAtnPin;

	//! True, iff the CIA ATN pin is configured as output
	bool ciaAtnIsOutput;

	//! Used to determine if the bus is idle or if data is transferred 
	uint32_t busActivity;
	
	//! Update IEC bus lines depending on the CIA and device pins
	bool _updateIecLines(bool *atnedge = NULL);

public:

	//! Constructor
	IEC();
	
	//! Destructor
	~IEC();
			
	//! Bring the component back to its initial state
	void reset(C64 *c64);

    //! Dump current configuration into message queue
    void ping();

    //! Size of internal state
    uint32_t stateSize();

	//! Load state
	void loadFromBuffer(uint8_t **buffer);
	
	//! Save state
	void saveToBuffer(uint8_t **buffer);	
	
	//! Dump internal state to console
	void dumpState();
	
	//! Write trace output to console
	void dumpTrace();
	
	//! Connect the virtual diesc drive to the IEC bus
	void setDrive(VC1541 *d) { assert(drive == NULL); drive = d; }
	
	//! Connect drive to the IEC bus
	void connectDrive();
	
	//! Disconnect the drive from the IEC bus
	void disconnectDrive();

	//! Returns true, iff a virtual disk drive is connected
	bool driveIsConnected() { return driveConnected; }
		
	//! Change/Update the value of all three bus lines 
	void updateIecLines();
	
    //! Updates the values of the CIA pin variables
	//* This function is to be invoked by the cia chip, only.
	void updateCiaPins(uint8_t cia_data, uint8_t cia_direction);	

    //! Updates the values of the device pin variables
	//* This function is to be invoked by the VC1541 drive, only.
	void updateDevicePins(uint8_t device_data, uint8_t device_direction);	
	
	// Deprecated (won't drive them manually when the drive is constantly connected)
	void setDeviceClockPin(bool value) { deviceClockPin = value; _updateIecLines(); } 
	void setDeviceDataPin(bool value) { deviceDataPin = value; _updateIecLines(); }
			
	bool getAtnLine() { return atnLine; }
	//bool getOldAtnLine() { return oldAtnLine; }
	bool getClockLine() { return clockLine; }
	//bool getOldClockLine() { return oldClockLine; }
	bool getDataLine() { return dataLine; }
	//bool getOldDataLine() { return oldDataLine; }

	bool atnPositiveEdge() { return oldAtnLine == 0 && atnLine == 1; }
	bool atnNegativeEdge() { return oldAtnLine == 1 && atnLine == 0; }
	bool clockPositiveEdge() { return oldClockLine == 0 && clockLine == 1; }
	bool clockNegativeEdge() { return oldClockLine == 1 && clockLine == 0; }
	bool dataPositiveEdge() { return oldDataLine == 0 && dataLine == 1; }
	bool dataNegativeEdge() { return oldDataLine == 1 && dataLine == 0; }
			
	//! Is invoked periodically by the run thread
	void execute();
};
	
#endif
