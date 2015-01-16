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
#define VC1541_CYCLES_PER_BYTE 26 //40 // 30

// Forward declarations
class IEC;
class C64;

//! Virtual VC 1541 drive
class VC1541 : public VirtualComponent { 

public:
	//! Reference to the virtual IEC bus
	C64 *c64;
	
	//! Reference to the virtual IEC bus
	IEC *iec;

	//! Reference to the virtual 6502 CPU
	CPU *cpu;
	
	//! Reference to the virtual drive memory
	VC1541Memory *mem;

	//! The 1541 drive contains two via chips for disk and peripheral I/O
	VIA1 *via1;
	VIA2 *via2;

private:
	
    //! GCR encoding table
    /*! Data on a VC 1541 disk ist GCR encoded. Four data bytes will be expanded to five GCR encoded bytes. */
    const uint16_t gcr[16] = {
        0x0a, 0x0b, 0x12, 0x13, 0x0e, 0x0f, 0x16, 0x17, 0x09, 0x19, 0x1a, 0x1b, 0x0d, 0x1d, 0x1e, 0x15
    };

    //! Number of tracks
    uint8_t numTracks;

	//! Disk data
	/*! Each disk consists of 42 halftracks with a maximum of 7928 bytes */
	uint8_t data[84][7928];

	//! Real length of each track
	uint16_t length[84];

	//! Indicates whether disk is rotating or not
	bool rotating;
    
	//! Indicates whether red LED is on or off
	bool redLED;

    //! Indicates whether a disk is inserted
	bool diskInserted;

public:
	
	//! Timer
	int byteReadyTimer;
	
	//! Read write head
	int track, offset;
	
	//! Counts the number of consecutively found 0xFF mark 
	int noOfFFBytes;
			
	//! Write protection mark
	bool writeProtection;
	
public:
	
	//! Constructor
	VC1541(C64 *c64);
	
	//! Destructor
	~VC1541();

	//! Restore initial state
	void reset();
	
    //! Dump current configuration into message queue
    void ping();

	//! Load state
	void loadFromBuffer(uint8_t **buffer);
	
	//! Save state
	void saveToBuffer(uint8_t **buffer);	
	
	//! Dump current state into logfile
	void dumpState();
	
    //! Getter and setter
    void activateRedLED();
    void deactivateRedLED();
    inline bool hasRedLED() { return redLED; };
    
    void startRotating();
    void stopRotating();
    inline bool isRotating() { return rotating; };
    
    bool isWriteProtected() { return writeProtection; };
    void setWriteProtection(bool b);

    //! Read data from haftrack
	inline uint8_t getData(unsigned halftrack, unsigned offset) 
		{ assert(track < 84); assert (offset < 7928); return data[halftrack][offset]; }

    //! Write data to haftrack
	inline void setData(unsigned halftrack, unsigned offset, uint8_t value)
		{ assert(track < 84); assert (offset < 7928); data[halftrack][offset] = value; }
	
	//! Pass control to the virtual drive
	/*! The drive will be executed for the specified number of clock cycles. */
	bool executeOneCycle();	

	//! ATN interrupt
	/*! Simulates an interrupt. Will be called when the ATN signal is going high */
	void simulateAtnInterrupt();
	
	void moveHead(int distance);

	void signalByteReady() { if (via2->overflowEnabled()) cpu->setV(1); }

	inline void writeByteToDisk(uint8_t val) { setData(track, offset, val); }
	inline void writeOraToDisk() { writeByteToDisk(via2->ora); }
	
	//! Insert a virtual disc
	void insertDisc(Archive *a);
	void insertDisc(D64Archive *a);
    inline bool hasDisk() { return diskInserted; }

	//! Eject virtual disc
	void ejectDisc();

	//! Rotate disk
	/*! Moves the RW head to next byte on the current track */
	void rotateDisk();
    
	uint8_t readHead() { return data[track][offset]; }
	uint8_t readHeadLookAhead() { return data[track][(offset + 1) % length[track]]; }
	uint8_t readHeadLookBehind() { return data[track][(offset + length[track] - 1) % length[track]]; }
	bool isSyncByte() { return (readHead() == 0xFF && (readHeadLookAhead() == 0xFF || readHeadLookBehind() == 0xFF)); }	

	// uint8_t readHead() { return readHeadLookBehind(); }
	void writeHead(uint8_t value) { data[track][offset] = value; }
	
	//! Dump state to debug console
	void dumpTrack(int track = -1);
	void dumpFullTrack(int track = -1);
	
	//! Clear half track
	/*! All bytes of the specified half tracked are zeroes out. */
	void clearHalftrack(int nr);

	//! Clear all tracks
	/*! All tracks of the disk image are zeroed out. */
	void clearDisk();

	//! Translate 4 data bytes into 5 GCR encodes bytes
	void encodeGcr(uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4, uint8_t *dest);

    //! Translate 5 GCR encoded bytes into 4 data bytes
    void decodeGcr(uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4, uint8_t b5, uint8_t *dest);

	//! Encode sector in real VC1541 format
	/*! This function converts the input data to a native VCR1541 byte stream including sync
		marks, GCR encodings, etc. Returns the number of  bytes written */
	int encodeSector(D64Archive *a, uint8_t track, uint8_t sector, uint8_t *dest, int gap);

    //! Decode sector from real VC1541 format
    void decodeSector(uint8_t halftrack, uint8_t sector, uint8_t *dest);

    //! Convert D64 format to VC1541 format
    /*! Invoke this function in insert disk */
    void encodeDisk(D64Archive *a);

    //! Decode VC1541 format to D64 format
    void decodeDisk(FILE *file);

	//! Check file type
	/*! Returns true, iff the specifies file is a valid G64 image file. */
	static bool isG64Image(const char *filename);

	//! Insert a G64 disk image
	bool readG64Image(const char *filename);
    
    //! Export currently inserted disc to D64 file
    bool exportToD64(const char *filename); 
};

#endif
