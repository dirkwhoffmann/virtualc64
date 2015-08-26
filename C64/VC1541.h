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
    VIA1 via1;

    //! VIA6522 connecting the drive CPU with the drives read/write head
    VIA2 via2;

    //! Constructor
    VC1541();
    
    //! Destructor
    ~VC1541();
    
    //! Reset VC1541 drive
    /*! A reset will only affect the VC1541s internal state. It keeps the disk alive */
    void reset(C64 *c64) { resetDrive(c64); }

    //! Reset drive properties
    /*! Resets all VC1541 related properties. Disk related properties remain in place. */
    void resetDrive(C64 *c64);

    //! Reset disk properties
    /*! Resets all disk related properties.  */
    void resetDisk();
    
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

public:
    
    //! Halftrack number (1 .. 85)
    /*! VC1541 DOS stores data on the halftracks with odd number, simply called "tracks".
        Halftrack 85 (sometimes called track 42.5) is the highest accessable halftrack. */
    typedef uint8_t Halftrack;

    bool isHalftrackNumber(uint8_t nr) { return 1 <= nr && nr <= 85; }

    //! Track number (1 .. 42)
    /*! Track 1 equals halftrack 1, track 2 equals haltrack 3, ... track 42 equals halftrack 84. */
    typedef uint8_t Track;

    bool isTrackNumber(uint8_t nr) { return 1 <= nr && nr <= 42; }

    //! Maximum number of files that can be stored on a single disk
    /*! VC1541 DOS allows up to 18 directory sectors, each containing 8 files. */
    const static unsigned MAX_FILES_ON_DISK = 144;

private:
    
    //! GCR encoding table
    /*! VC1541 uses GCR encoding. Four data bytes will be expanded to five GCR encoded bytes. */
    
    const uint16_t gcr[16] = {
        0x0a, 0x0b, 0x12, 0x13,
        0x0e, 0x0f, 0x16, 0x17,
        0x09, 0x19, 0x1a, 0x1b,
        0x0d, 0x1d, 0x1e, 0x15
    };

    //! The stored numbers indicate how many clock cycles are needed for reading or writing a single byte
    /*! Background: The VC1541 drive is clocked by 16 Mhz. The base frequency is divided by N where N ranges
        from 13 (zone 0) to 16 (zone 4). On the logic board, this is done with a 4-bit counter of type 74SL193 
        whose reset value bits are directly connected to the two "zone" bits (PB5 and PB6) coming from via 2.
        A second 74SL193 divides the signal by 4. The result serves as the clock signal for all units operating 
        on the bit level (i.e., the two shift registers that transfer bits from and to the head).
        This means that a single bit is ready after 3,25 CPU cycles in zone 0 and 4 CPU cycles in zone 4.
        The resulting signal is fed into a third counter (of type 74LS191). It divides the signal by 8 and its 
        output is fed into a three input NAND-gate computing the important BYTE-READY signal. */
     
    const uint16_t cyclesPerByte[4] = {
        13 * 4 * 8, // Zone 0: One byte each (16 * 26) base clock cycles (= 26 CPU cycles)
        14 * 4 * 8, // Zone 1: One byte each (16 * 28) base clock cycles (= 28 CPU cycles)
        15 * 4 * 8, // Zone 2: One byte each (16 * 30) base clock cycles (30 CPU cycles)
        16 * 4 * 8, // Zone 3: One byte each (16 * 32) base clock cycles (32 CPU cycles)
    };
    
    
    // ---------------------------------------------------------------------------------------------
    //                                    Main entry points
    // ---------------------------------------------------------------------------------------------
    
public:
    
    // Configuration
    
    inline bool soundMessagesEnabled() { return sendSoundMessages; }
    inline void setSendSoundMessages(bool b) { sendSoundMessages = b; }
    

    // Disk handling
    
    inline bool isWriteProtected() { return writeProtected; }
    inline void setWriteProtection(bool b) { writeProtected = b; }
    
    inline bool getRedLED() { return redLED; };
    void setRedLED(bool b);
    
    inline bool isRotating() { return rotating; };
    void setRotating(bool b);
    
    //! Inserts an archive as a virtual disk
    /*! Before inserting, the archive data is converted to VC1541s GCR-encoded track/sector format */
    void insertDisk(Archive *a);
    void insertDisk(D64Archive *a);

    // Returns true if a disk is inserted
    inline bool hasDisk() { return diskInserted; }
    
    //! Eject the virtual disk. Does nothing, if no disk is present.
    /*! Beware that this function causes a considerable time delay, because it is necessary to
        block the write protection light barrier for a while. Otherwise, the VC1541 DOS would not recognize 
        the ejection. */
    void ejectDisk();

    
    // Execution
    
    //! Execute virtual drive for one cycle (fast execution wrapper)
    inline bool executeOneCycle() {

        via1.execute();
        via2.execute();
        uint8_t result = cpu->executeOneCycle();
        
        // Exit if drive in inactive
        if (!rotating)
            return result;
        
        // Wait until next byte is ready
        if (byteReadyTimer) {
            byteReadyTimer -= 16;
            return result;
        }
        
        // Byte is ready. Reset counter and process byte
        assert (zone < 4);
        byteReadyTimer = cyclesPerByte[zone];
        executeByteReady();
        return true;
    }

    //! Main execution code of executeOneCycle()
    /*! The main action takes place when all bits of a serially read or written byte byte are processed. */
    void executeByteReady();
        
    
    // ---------------------------------------------------------------------------------------------
    //                                     Disk data storage
    // ---------------------------------------------------------------------------------------------

private:
    
    // TODO:
    // Remove direct accesses to length.
    // Check direct accesses to data
    // Lift up array index by 1
    // Switch to union style storage
    // Cleanup
    
    //! Disk data storage
    /*! Each disk consists of 42 tracks (84 halftracks) with a maximum of 7928 bytes.
     As the number of tracks and the number of byter per track vary, the array is usually filled
     partially. The number of tracks of the currently inserted disk is stored in variable numTracks
     and the real length of a given halftrack in array length[]. */
    uint8_t data[84][7928];
    
    // NEW CODE:
    // Couting of track and halftrack numbers begins with 1. Hence, the entries [0][x] are unused.

    union {
        uint8_t halftrack[86][7928];
        uint8_t track[43][2 * 7928];
    } newdata;
    
    //! Total number of tracks on this disk
    // DEPRECATED (ADD METHOD emptyTrack(Track nr);) (Scans track for data, returns true/false numTracks())
    uint8_t numTracks;
    
    //! Length of each halftrack in bytes
    uint16_t length[84];

    union {
        uint16_t halftrack[86];
        uint16_t track[43][2];
    } newlength;

    
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
    
    //! Indicates whether the VC1541 shall provide sound notification messages to the GUI
    bool sendSoundMessages;


    // ---------------------------------------------------------------------------------------------
    //                                  Read/Write logic
    // ---------------------------------------------------------------------------------------------

private:
    
    //! The next byte will be ready after this number of cycles
    uint16_t byteReadyTimer;

    //! Track position of the read/write head
    uint8_t track;
    
    //! Position of the read/write head inside the current track (byte granularity)
    uint16_t offset;

    //! Current zone
    /*! Each track belongs to one of four zones. Whenever the drive moves the r/w head, 
        it computed the new number and writes into PB5 and PB6 of via2. These bits are 
        hard-wired to a 74LS193 counter on the logic board that breaks down the 16 Mhz base
        frequency. This mechanism is used to slow down the read/write process on inner tracks. */
    uint8_t zone;
    
    //! The 74LS164 serial to parallel shift register
    /*! In read mode, this register is fed by the drive head with data. */
    uint8_t read_shiftreg;

    //! Previous value of read_shiftreg
    /*! We need this value to detect the SYNC signal */
    uint8_t read_shiftreg_pipe;

    //! The 74LS165 parallel to serial shift register
    /*! In write mode, this register feeds the drive head with data. */
    uint8_t write_shiftreg;
        
public:

    //! Returns true iff drive is currently in read mode
    bool readMode() { return (via2.io[0x0C] & 0x20); }

    //! Returns true iff drive is currently in write mode
    bool writeMode() { return !(via2.io[0x0C] & 0x20); }

    // Returns the current value of the SYNC signal
    /* In the logic board, the SYNC signal is computed by a NAND gate that combines the 10 previously read bits
     from the input shift register and CB2 of VIA2 (the r/w mode pin). Connecting CB2 to the NAND gates ensures
     that SYNC can only be true in read mode. */
    inline bool SYNC() { return (read_shiftreg == 0xFF && (read_shiftreg_pipe & 0x03) == 0x03 && readMode()); }
    
    //! Moves head one halftrack up
    void moveHeadUp();
    
    //! Moves head one halftrack down
    void moveHeadDown();

    //! Returns the current track zone (0 to 3)
    inline bool getZone() { return zone; };

    //! Sets the current track zone (0 to 3)
    void setZone(uint8_t z);

    //! Triggers an ATN interrupt
    /*! This function is called by the IEC bus when the ATN signal raises. */
    void simulateAtnInterrupt();

private:

    //! Reads the currently processed byte
    /*! In a real VC1541, the drive head would currently process one out of the returned eight bits. */
    inline uint8_t readHead() { return data[track][offset]; }    
    
    //! Writes byte to the current head position
    inline void writeHead(uint8_t value) { data[track][offset] = value; }

    //! Rotate disk
    /*! Moves head to next byte on the current track */
    inline void rotateDisk() { if (++offset >= length[track]) offset = 0; }
    
    // Signals the CPU that a byte has been processed
    inline void byteReady();

    // Signals the CPU that a byte has been processed and load byte into input latch A of via 2
    inline void byteReady(uint8_t byte);
   
    
    // ---------------------------------------------------------------------------------------------
    //                              Track and sector management
    // ---------------------------------------------------------------------------------------------

//private:
public:
    
    //! Converts a track number into a halftrack number
    /*! The mapping is: 1->1, 2->3, 3->5, 5->7, ..., 41->81, 42->83 */
    // DEPRECATED
    static unsigned trackToHalftrack(unsigned track) {
        assert(track >= 1 && track <= 42); return (2 * track) - 1; }
    
    //! Converts a halftrack number into a track number
    /*! The mapping is: 1->1, 2->1, 3->2, 4->2, ..., 83->42, 84->42 */
    // DEPRECATED
    static unsigned halftrackToTrack(unsigned halftrack) {
        assert(halftrack >= 1 && halftrack <= 84); return (halftrack + 1) / 2; }

    //! Return start address of a given halftrack (1...84)
    // DEPRECATED
    inline uint8_t *startOfHalftrack(unsigned halftrack) {
        assert(halftrack >= 1 && halftrack <= 84); return data[halftrack - 1]; }
    
    //! Return start address of a given track (1...42)
    // DEPRECATED
    inline uint8_t *startOfTrack(unsigned track) {
        assert(track >= 1 && track <= 42); return startOfHalftrack(2 * track - 1); }
    
    //! Returns the length of a halftrack
    // DEPRECATED
    inline uint16_t lengthOfHalftrack(unsigned halftrack) {
        assert(halftrack >= 1 && halftrack <= 84); return length[halftrack - 1];
    }

    //! Returns the length of a track
    // DEPRECATED
    inline uint16_t lengthOfTrack(unsigned track) {
        assert(track >= 1 && track <= 42); return lengthOfHalftrack(trackToHalftrack(track));
    }

    //! Returns the length of a halftrack
    // DEPRECATED
    inline void setLengthOfHalftrack(unsigned halftrack, unsigned len) {
        assert(halftrack >= 1 && halftrack <= 84); length[halftrack - 1] = len;
    }
    
    //! Returns the length of a track
    // DEPRECATED
    inline void setLengthOfTrack(unsigned track, unsigned len) {
        assert(track >= 1 && track <= 42); setLengthOfHalftrack(trackToHalftrack(track), len);
    }
    
    //! Zero out a single halftrack
    /*! If the additional track size parameter is provided, the track size is also adjusted.
        Othwerwise, the current track size is kept. */
    void clearHalftrack(unsigned halftrack);
    
    //! Zero out all tracks on a disk
    void clearDisk();

    //! For debugging
    void dumpTrack(int track = -1);
    
    //! For debugging
    void dumpFullTrack(int track = -1);

    
    // ---------------------------------------------------------------------------------------------
    //                               Data encoding and decoding
    // ---------------------------------------------------------------------------------------------

public:
    
    //! Converts a D64 archive to real disk data
    /*! The methods creates tracks and sectors, GCR encodes the archive data and creates a directory */
    void encodeDisk(D64Archive *a);
    
    //! Converts real disk data into a byte stream compatible with the D64 format
    /*! Returns the number of bytes written.
        If dest is NULL, a test run is performed (used to determine how many bytes will be written).
        If something went wrong, an error code is written to 'error' (0 = no error = success) */
    unsigned decodeDisk(uint8_t *dest, int *error = NULL);

    //! Export currently inserted disk to D64 file
    bool exportToD64(const char *filename);

private:

    //! Encode a single trach
    /*! This function translates the logical byte sequence of a single track into the native VC1541
        byte representation. The native representation includes sync marks, GCR data etc. 
        tailGapEven is the number of tail bytes follwowing sectors with even sector numbers.
        tailGapOdd is the number of tail bytes follwowing sectors with odd sector numbers.
        Returns the number of bytes written */
    unsigned encodeTrack(D64Archive *a, uint8_t track, int *sector, uint8_t tailGapEven, uint8_t tailGapOdd);

    //! Encode a single sector
    /*! This function translates the logical byte sequence of a single sector into the native VC1541
        byte representation. The sector is closed by 'gap' tail gap bytes.
        Returns the number of bytes written. */
    unsigned encodeSector(D64Archive *a, uint8_t track, uint8_t sector, uint8_t *dest, int gap);
    
    //! Write five SYNC bytes
    void encodeSync(uint8_t *dest) { for (unsigned i = 0; i < 5; i++) dest[i] = 0xFF; }

    //! Write interblock gap
    void encodeGap(uint8_t *dest, unsigned size) { for (unsigned i = 0; i < size; i++) dest[i] = 0x55; }

    //! Translate 4 data bytes into 5 GCR encodes bytes
	void encodeGcr(uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4, uint8_t *dest);

    //! Decodes all sectors of a single GCR encoded track
    unsigned decodeTrack(uint8_t *source, uint8_t *dest, int *error = NULL);
    
    //! Decodes a single GCR encoded sector and write out its 256 data bytes 
    void decodeSector(uint8_t *source, uint8_t *dest);
    
    //! Decodes 5 GCR bytes into its 4 corrrsponding data bytes
    void decodeGcr(uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4, uint8_t b5, uint8_t *dest);
    
};

#endif
