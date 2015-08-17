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

	//! VIA6522 connecting the drive CPU with the IEC bus
    VIA1 *via1;

    //! VIA6522 connecting the drive CPU with the drives read/write head
    VIA2 *via2;

    //! Constructor
    VC1541(C64 *c64);
    
    //! Destructor
    ~VC1541();
    
    //! Restore initial state
    void reset();
    
    //! Dump current configuration into message queue
    void ping();
    
    //! Size of internal state
    uint32_t stateSize();
    
    //! Load state
    void loadFromBuffer(uint8_t **buffer);
    
    //! Save state
    void saveToBuffer(uint8_t **buffer);
    
    //! Dump current state into logfile
    void dumpState();


    // -----------------------------------------------------------------------------------------------
    //                                     Constant definitions
    // -----------------------------------------------------------------------------------------------

private:
    
    //! GCR encoding table
    /*! VC1541 uses GCR encoding. Four data bytes will be expanded to five GCR encoded bytes. */
    
    const uint16_t gcr[16] = {
        0x0a, 0x0b, 0x12, 0x13,
        0x0e, 0x0f, 0x16, 0x17,
        0x09, 0x19, 0x1a, 0x1b,
        0x0d, 0x1d, 0x1e, 0x15
    };

    // ---------------------------------------------------------------------------------------------
    //                                     Disk data storage
    // ---------------------------------------------------------------------------------------------

private:
    
    //! Disk data storage
    /*! Each disk consists of 42 tracks (84 halftracks) with a maximum of 7928 bytes.
     As the number of tracks and the number of byter per track vary, the array is usually filled
     partially. The number of tracks of the currently inserted disk is stored in variable numTracks
     and the real length of a given halftrack in array length[]. */
    uint8_t data[84][7928];
    
    //! Total number of halftracks
    uint8_t numTracks;
    
    //! Length of halftrack
    uint16_t length[84];
    
    // ---------------------------------------------------------------------------------------------
    //                                   Drive properties
    // ---------------------------------------------------------------------------------------------
   
private:
    
    //! Indicates whether disk is rotating or not
    bool rotating;
    
    //! Indicates whether red LED is on or off
    bool redLED;
    
    //! Indicates whether a disk is inserted
    bool diskInserted;
    
    //! Write protection mark
    bool writeProtected;
    
public:
    
    inline bool isWriteProtected() { return writeProtected; }
    inline void setWriteProtection(bool b) { writeProtected = b; }
    
    inline bool getSyncMark() { return syncMark; }
    inline void setSyncMark(bool b) { syncMark = b; }
    
    inline bool hasRedLED() { return redLED; };
    void activateRedLED();
    void deactivateRedLED();
    
    inline bool isRotating() { return rotating; };
    void startRotating();
    void stopRotating();

    //! Insert a virtual disc
    void insertDisc(Archive *a);
    void insertDisc(D64Archive *a);
    
    //! Returns true if a disk is currently inserted
    inline bool hasDisk() { return diskInserted; }
    
    //! Eject virtual disc
    void ejectDisc();

    // ---------------------------------------------------------------------------------------------
    //                                  Read/Write logic
    // ---------------------------------------------------------------------------------------------

private:
    
    //! Position of read write head
    /*! The position marks the byte that is currently read in. When the
     byteReadyTimer times out, the byte is copied to ora in via2 */
    int track, offset;

    //! Temporarily set to true when the VC1541 reads over a sync mark
    bool syncMark;
	
	//! Timer
	int byteReadyTimer;
	
	//! Counts the number of consecutively found 0xFF mark
    /*! This variable is used to detect ynchronization marks on disk.
        In a real VC1514, a synchronization sequence is detected when at least
        then 10 consecutive 1-bits are read. As the emulator is byte based, a
        sync mark is detected when two consecutive 0xFF bytes are found. */
    int noOfFFBytes;
				
    //! Indicates whether the next byte on disk will be read or written
    /*! When the read/write head moves to the next byte, the emulator determines
        the operation mode (read or write) for the next byte. If the operation
        mode is 'write', the data byte in 'latched_ora' will be written to disk. */
    bool latched_readmode;
    
    //! Next byte to be written on disk
    uint8_t latched_ora;
    
public:
		
	//! Passes control to the virtual drive
	/*! The drive will be executed for the specified number of clock cycles. */
	bool executeOneCycle();	

    //! Moves head one halftrack up
    void moveHeadUp();
    
    //! Moves head one halftrack down
    void moveHeadDown();

private:
    
    //! Reads the currently processed byte
    /*! In a real VC1541, the drive head would currently process one out of these eight bits. */
    inline uint8_t readHead() { return data[track][offset]; }
    inline uint8_t readHeadLookAhead() { return (offset+1 < length[track]) ? data[track][offset+1] : data[track][0]; }
    inline uint8_t readHeadLookBehind() { return (offset > 0) ? data[track][offset-1] : data[track][length[track]-1]; }
        
    //! Writes byte to the current head position
    /*! In a real VC1541, the drive head would currently process one out of these eight bits. */
    inline void writeHead(uint8_t value) { data[track][offset] = value; }

    //! Rotate disk
    /*! Moves head to next byte on the current track */
    void rotateDisk();

    // Signals the CPU that a byte has been processed
    inline void signalByteReady() { if (via2->overflowEnabled()) cpu->setV(1); }

    //! Triggers an ATN interrupt
	/*! This function is called when the ATN signal is going high */
	void simulateAtnInterrupt();
	
    
    // ---------------------------------------------------------------------------------------------
    //                              Track and sector management
    // ---------------------------------------------------------------------------------------------

private:
    
    //! Return start address of a given halftrack (1...84)
    inline uint8_t *startOfHalftrack(unsigned halftrack) {
        assert(halftrack >= 1 && halftrack <= 84); return &data[halftrack - 1][0]; }
    
    //! Return start address of a given track (1...42)
    inline uint8_t *startOfTrack(unsigned track) {
        assert(track >= 1 && track <= 42); return startOfHalftrack(2 * track - 1); }
    
    //! Zero out a single halftrack
    void clearHalftrack(int nr);
    
    //! Zero out all halftracks on disk
    void clearDisk();

    //! For debugging
    void dumpTrack(int track = -1);
    
    //! For debugging
    void dumpFullTrack(int track = -1);

    
    // ---------------------------------------------------------------------------------------------
    //                                       Data encoding
    // ---------------------------------------------------------------------------------------------

public:
    
    //! Convert D64 format to VC1541 format
    /*! Invoke this function in insert disk */
    void encodeDisk(D64Archive *a);
    
    //! Decode VC1541 format to D64 format
    /*! Returns the number of bytes written. Pass a NULL pointer to perform a test run, i.e.,
     to see how many bytes will be written */
    unsigned decodeDisk(uint8_t *dest);
    
    //! Export currently inserted disc to D64 file
    bool exportToD64(const char *filename);

private:
    
	//! Translate 4 data bytes into 5 GCR encodes bytes
	void encodeGcr(uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4, uint8_t *dest);

    //! Translate 5 GCR encoded bytes into 4 data bytes
    void decodeGcr(uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4, uint8_t b5, uint8_t *dest);

	//! Encode a single sector
	/*! This function translates the logical byte sequence of a single sector into the native VC1541 
        byte representation. The native representation includes sync marks, GCR data etc. 
        Returns the number of bytes written */
	int encodeSector(D64Archive *a, uint8_t track, uint8_t sector, uint8_t *dest, int gap);

    //! Decode a single sector
    /*! This function translates the native byte representation of a single sector into a logical
        byte stream. */
    void decodeSector(uint8_t *source, uint8_t *dest);
    
public:

    // EXPERIMENTAL CODE FOR G64 format. TODO: Make it a container class
    
	//! Check file type
	/*! Returns true, iff the specifies file is a valid G64 image file. */
	static bool isG64Image(const char *filename);

	//! Insert a G64 disk image
	bool readG64Image(const char *filename);
    
};

#endif
