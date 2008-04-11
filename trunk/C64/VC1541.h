/*
 * (C) 2008 Dirk W. Hoffmann. All rights reserved.
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

#ifndef _VC1541_INC
#define _VC1541_INC

#include "VIA6522.h"

// = 26; // due to my own calculations, a new byte should arrive after 26 CPU cycles
#define VC1541_CYCLES_PER_BYTE 30

// Forward declarations
class IEC;
class C64;

//! Virtual VC 1541 drive
class VC1541 : public VirtualComponent { 

public:
	//! The 1541 drive contains two via chips for disk and peripheral I/O
	VIA1 *via1;
	VIA2 *via2;

	//! Reference to the virtual IEC bus
	C64 *c64;
		
	//! Reference to the virtual IEC bus
	IEC *iec;

	//! Reference to the virtual 6502 CPU
	CPU *cpu;
	
	//! Reference to the virtual drive memory
	VC1541Memory *mem;

	//! Disk data
	uint8_t data[84][7928];  // 42 halftracks with max. 7928 bytes
	uint16_t length[84];     // real length of each track
	
	//! Timer
	int byteReadyTimer;
	
	//! Read write head
	int track, offset;
	
	//! Counts the number of consecutively found 0xFF mark 
	int noOfFFBytes;
			
	//! Write protection
	bool writeProtection;
	
public:
	//! Read flag
	/*! Indicates that a byte has been read from the RW head */
	bool readFlag;

	//! Write flag
	/*! Indicates that a byte has been passed to the RW head */
	bool writeFlag;
	bool syncFlag;
	
public:
	//! Constructor
	VC1541();
	
	//! Destructor
	~VC1541();

	inline uint8_t getData(unsigned halftrack, unsigned offset) 
		{ assert(track < 84); assert (offset < 7928); return data[halftrack][offset]; }
	inline void setData(unsigned halftrack, unsigned offset, uint8_t value) 
		{ assert(track < 84); assert (offset < 7928); data[halftrack][offset] = value; }

	// Bring component back to its initial state
	void reset();

	//! Bind disc drive to a virtual IEC bus
	void setIEC(IEC *bus) { assert(iec == NULL); iec = bus; mem->setIEC(bus); }

	//! Bind disc drive to a virtual C64
	void setC64(C64 *c) { assert(c64 == NULL); c64 = c; }
	
	//! Pass control to the virtual drive
	/*! The drive will be executed for the specified number of clock cycles. */
	// bool execute(int cycles); 
	void executeOneCycle();	

	//! ATN interrupt
	/*! Simulates an interrupt. Will be called when the ATN signal is going high */
	void simulateAtnInterrupt();
	
	//! Load internal state from a file
	bool load(FILE *file);

	//! Save internal state into a file
	bool save(FILE *file);

	void moveHead(int distance);

	void activateRedLED() { getListener()->driveLEDAction(true); }
	void deactivateRedLED() { getListener()->driveLEDAction(false); }
	void startRotating();
	void stopRotating();
	void signalByteReady() { if (via2->overflowEnabled()) cpu->setV(1); }
	void writeOraToDisk() { setData(track, offset, via2->ora); debug(" (%02X)", via2->ora); }
	// void writeByteToDisk() { assert(writeBuf != -1); setData(track, offset, (uint8_t)writeBuf); debug(" (%02X)", via2->ora); }


	//! Insert a virtual disc
	void insertDisc(Archive *a);
	void insertDisc(D64Archive *a);

	//! Eject virtual disc
	void ejectDisc();

	//! Rotate disk
	/*! Moves the RW head to next byte on the current track */
	void rotateDisk(); 
	uint8_t readHead() { return data[track][offset]; }
	uint8_t readHeadLookAhead() { return data[track][(offset + 1) % length[track]]; }
	uint8_t readHeadLookBehind() { return data[track][(offset + length[track] - 1) % length[track]]; }
	bool isSyncByte() { return (readHead() == 0xFF && (readHeadLookAhead() == 0xFF || readHeadLookBehind() == 0xFF)); }	

	void writeHead(uint8_t value) { data[track][offset] = value; }
	
	//! Dump state to debug console
	void dumpState();

	//! Clear half track
	/*! All bytes of the specified half tracked are zeroes out. */
	void clearHalftrack(int nr);

	//! Clear all tracks
	/*! All tracks of the disk image are zeroed out. */
	void clearDisk();

	//! Encode four bytes in GCR format
	/*! Data on a VC 1541 disk ist GCR encoded. Four data bytes will result in five bytes
	    on the floppy disk. */
	void encodeGcr(uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4, uint8_t *dest);

	//! Encode sector in real VC1541 format
	/*! source points to the input data, dest to the target location od the encoded data. 
		This function converts the input data to a native VCR1541 byte stream including sync 
		marks, GCR encodings, etc. Returns the number of  bytes written */
	int encodeSector(D64Archive *a, uint8_t track, uint8_t sector, uint8_t *dest);

	//! Check file type
	/*! Returns true, iff the specifies file is a valid G64 image file. */
	static bool isG64Image(const char *filename);

	//! Insert a G64 disk image
	bool readG64Image(const char *filename);
};

#endif