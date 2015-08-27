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
#include "Disk525.h"

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

    //! Disk in this drive (single sided 5,25" floppy disk)
    Disk525 disk;
    
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

    
private:
    
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
    
    // TODO: MOVE TO DISK CLASS
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

    //! Export currently inserted disk to D64 file
    bool exportToD64(const char *filename);
    
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

public:
    

private:
    
    // TODO:
    // Remove direct accesses to length.
    // Check direct accesses to data
    // Lift up array index by 1
    // Switch to union style storage
    // Cleanup
    
    
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

    //! Halftrack position of the read/write head
    Disk525::Halftrack halftrack;

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
    inline uint8_t readHead() { return disk.data.halftrack[halftrack][offset]; }
    
    //! Writes byte to the current head position
    inline void writeHead(uint8_t value) { disk.data.halftrack[halftrack][offset] = value; }

    //! Rotate disk
    /*! Moves head to next byte on the current track */
    inline void rotateDisk() { if (++offset >= disk.length.halftrack[halftrack]) offset = 0; }
    
    // Signals the CPU that a byte has been processed
    inline void byteReady();

    // Signals the CPU that a byte has been processed and load byte into input latch A of via 2
    inline void byteReady(uint8_t byte);
   

};

#endif
