/*! 
 * @header      VC1541.h
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   2008 - 2016 Dirk W. Hoffmann
 * @brief       Declares VC1541 class
 */
/* This program is free software; you can redistribute it and/or modify
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
#include "D64Archive.h"

// Forward declarations
class IEC;
class C64;

/*!
 * @brief    Virtual VC1541 drive
 * @details  Bit-accurate emulation of a VC1541
 */
class VC1541 : public VirtualComponent {

public:
    
	//! @brief    Reference to the virtual IEC bus
	IEC *iec;

	//! @brief    CPU of the virtual drive (6502)
	CPU cpu;
	
	//! @brief    Memory of the virtual drive
	VC1541Memory mem;

	//! @brief    VIA6522 connecting the drive CPU with the IEC bus
    VIA1 via1;

    //! @brief    VIA6522 connecting the drive CPU with the drives read/write head
    VIA2 via2;

    //! @brief    Disk in this drive (single sided 5,25" floppy disk)
    Disk525 disk;
    
    //! @brief    Constructor
    VC1541();
    
    //! @brief    Destructor
    ~VC1541();
    
    //! @brief    Resets the VC1541 drive.
    void reset();

    /*! @brief    Resets disk properties
     *  @details  Resets all disk related properties. reset() keeps the disk alive. 
     */
    void resetDisk();
    
    //! @brief    Dump current configuration into message queue
    void ping();
        
    //! @brief    Dump current state into logfile
    void dumpState();

    
private:
    
    /*! @brief    The stored numbers indicate how many clock cycles are needed for reading or writing a single bit
     *  @details  The VC1541 drive is clocked by 16 Mhz. The base frequency is divided by N where N ranges
     *            from 13 (zone 0) to 16 (zone 4). On the logic board, this is done with a 4-bit counter of type 74SL193
     *            whose reset value bits are connected to the two "zone" bits (PB5 and PB6) coming from via 2.
     *            A second 74SL193 divides the signal by 4. The result serves as the clock signal for all units operating
     *            on bit level (i.e., the two shift registers that transfer bits from and to the head).
     *            It follows that a single bit is ready after 3,25 CPU cycles in zone 0 and 4 CPU cycles in zone 4.
     *            The resulting signal is fed into a third counter (of type 74LS191). It divides the signal by 8 and its
     *            output is fed into a three input NAND-gate computing the important BYTE-READY signal. 
     */
    const uint16_t cyclesPerBit[4] = {
        13 * 4, // Zone 0: One bit each (16 * 3.25) base clock cycles (= 3.25 CPU cycles)
        14 * 4, // Zone 1: One bit each (16 * 3.5) base clock cycles (= 3.5 CPU cycles)
        15 * 4, // Zone 2: One bit each (16 * 3.75) base clock cycles (3.75 CPU cycles)
        16 * 4, // Zone 3: One bit each (16 * 4) base clock cycles (4 CPU cycles)
    };

    
    // ---------------------------------------------------------------------------------------------
    //                                    Main entry points
    // ---------------------------------------------------------------------------------------------
    
public:
    
    //
    //! @functiongroup Configuring the device
    //
    
    //! @brief    Returns true if sound messages are sent to the GUI.
    inline bool soundMessagesEnabled() { return sendSoundMessages; }

    //! @brief    Enables or disables sending of sound messages.
    inline void setSendSoundMessages(bool b) { sendSoundMessages = b; }

    //! @brief    Returns true if drive is emulated bit accurately.
    inline bool getBitAccuracy() { return bitAccuracy; }

    //! @brief    Enables or disables bit accurate drive emulation.
    void setBitAccuracy(bool b);

    
    //
    //! @functiongroup Accessing drive properties
    //
    
    //! @brief    Returns true iff the red drive LED is shining.
    inline bool getRedLED() { return redLED; };

    //! @brief    Turns red drive LED on or off.
    void setRedLED(bool b);

    //! @brief    Returns true iff the drive engine is on.
    inline bool isRotating() { return rotating; };

    //! @brief    Turns the drive engine on or off.
    void setRotating(bool b);
    
    
    //
    //! @functiongroup Handling virtual disks
    //

    //! @brief    Returns true if a disk is inserted.
    inline bool hasDisk() { return diskInserted; }

    /*! @brief    Inserts an archive as a virtual disk.
     *  @details  This function consumes some time as it needs to perform various conversions.
     *            E.g., if you provide a T64 archive, it is first converted to an D64 archive.
     *            After that, all tracks will be GCR-encoded and written to a new disk.
     */
    bool insertDisk(Archive *a);
    
    //! @brief    Returns true if a disk is partially inserted.
    inline bool isDiskPartiallyInserted() { return diskPartiallyInserted; }

    //! @brief    Sets if a disk is partially inserted.
    inline void setDiskPartiallyInserted(bool b) { diskPartiallyInserted = b; }

    /*! @brief    Returns the current status of the write protection light barrier
     *  @details  If the light barrier is blocked, the drive head is unable to change data bits
     */
    inline bool getLightBarrier() { return isDiskPartiallyInserted() || disk.isWriteProtected(); }

    /*! @brief    Ejects the virtual disk
     *  @details  Does nothing, if no disk is present. Beware that this function causes a considerable time delay,
     *            because it is necessary to block the write protection light barrier for a while. Otherwise,
     *            VC1541 DOS would not recognize the ejection.
     */
    void ejectDisk();

    /*! @brief    Converts the currently inserted disk into a D64 archive.
     *  @result   A D64 archive containing the same files as the currently inserted disk;
     *            NULL if no disk is inserted.
     */
    D64Archive *convertToD64();

    //! @brief    Exports the currently inserted disk to D64 file.
    bool exportToD64(const char *filename);

    
    //
    //! @functiongroup Running the device
    //
    
    //! @brief    Cold starts the floppy drive
    /*! @details  Mimics the effect of switching the drive off and on again.
     */
    void powerUp();

    /*! @brief    Executes the virtual drive for one clock cycle
     *  @seealso  executeBitReady
     *  @seealso  executeByteReady 
     */
    bool executeOneCycle();

private:
    
    /*! @brief    Helper method for executeOneCycle
     *  @details  Method is executed whenever a single bit is ready
     */
    void executeBitReady();

    /*! @brief    Helper method for executeBitReady
     *  @details  Method is executed whenever a single byte is ready
     */
    void executeByteReady();
        
    
    // ---------------------------------------------------------------------------------------------
    //                                   Drive properties
    // ---------------------------------------------------------------------------------------------
   
private:
    
    //! @brief    Indicates whether disk is rotating or not
    bool rotating;
    
    //! @brief    Indicates whether red LED is on or off
    bool redLED;
    
    /*! @brief    Indicates whether a disk is inserted
     *  @note     A fully inserted disk blocks the write protection barrier if it is write protected 
     */
    bool diskInserted;
    
    /*! @brief    Indicates whether a disk is inserted only partially
     *  @note     A partially inserted disk blocks always blocks the write protection barrier 
     */
    bool diskPartiallyInserted;
    
    /*! @brief    Indicates whether VC1541 is simulated on the bit level
     *  @details  Bit level simulation is the standard emulation mode. If it is disabled, the
     *            emulator uses a fast load mechanism to make disk data available whenever the
     *            VC1541 DOS waits for it. Right now, this is an experimental feature. Note, that
     *            writing to disk is only works when bit level emulation is enabled.
     */
    bool bitAccuracy;

    //! @brief    Indicates whether the VC1541 shall provide sound notification messages to the GUI
    bool sendSoundMessages;


    // ---------------------------------------------------------------------------------------------
    //                                  Read/Write logic
    // ---------------------------------------------------------------------------------------------

private:
    
    //! @brief    The next bit will be ready after this number of cycles.
    int16_t bitReadyTimer;

    /*! @brief    Serial load signal
     *  @details  The VC1541 logic board contains a 4-bit-counter of type 72LS191 which is advanced whenever
     *            a bit is ready. By reaching 7, the counter signals that a byte is ready. In that case,
     *            the write shift register is loaded with new data and the byte ready signal, which is connected
     *            to CA1 of VIA2, changes state. In read mode, this state change will feed the input latch of VIA2
     *            with the current contents of the read shift register.
     */
    uint8_t byteReadyCounter;
    
    
    //! @brief    Halftrack position of the read/write head
    Halftrack halftrack;

    //! @brief    Bit position of the read/write head inside the current track
    uint16_t bitoffset;
    
    /*! @brief    Current disk zone
     *  @details  Each track belongs to one of four zones. Whenever the drive moves the r/w head,
     *            it computed the new number and writes into PB5 and PB6 of via2. These bits are
     *            hard-wired to a 74LS193 counter on the logic board that breaks down the 16 Mhz base
     *            frequency. This mechanism is used to slow down the read/write process on inner tracks.
     */
    uint8_t zone;

    /*! @brief    The 74LS164 serial to parallel shift register
     *  @details  In read mode, this register is fed by the drive head with data.
     */
    uint16_t read_shiftreg;
    
    /*! @brief    The 74LS165 parallel to serial shift register
     *  @details  In write mode, this register feeds the drive head with data.
     */
    uint8_t write_shiftreg;

    /*! @brief    Current value of the SYNC signal
     *  @details  This signal plays an important role for timing synchronization. It becomes true when the
     *            beginning of a SYNC is detected. On the logic board, the SYNC signal is computed by a NAND gate
     *            that combines the 10 previously read bits rom the input shift register and CB2 of VIA2 (the
     *            r/w mode pin). Connecting CB2 to the NAND gates ensures that SYNC can only be true in read mode.
     *            When SYNC becomes false (meaning that a 0 was pushed into the shift register), the byteReadyCounter
     *            is reset.
     */
    bool sync;
            
public:

    //! @brief    Returns true iff drive is currently in read mode
    bool readMode() { return (via2.io[0x0C] & 0x20); }

    //! @brief    Returns true iff drive is currently in write mode
    bool writeMode() { return !(via2.io[0x0C] & 0x20); }
   
    //! @brief    Moves head one halftrack up
    void moveHeadUp();
    
    //! @brief    Moves head one halftrack down
    void moveHeadDown();

    //! @brief    Returns the current value of the sync signal
    inline bool getSync() { return sync; }

    //! @brief    Returns the current track zone (0 to 3)
    inline bool getZone() { return zone; }

    /*! @brief    Sets the current track zone
     *  @param    z drive zone (0 to 3)
     */
    void setZone(uint8_t z);

    /*! @brief    Triggers an ATN interrupt
     *  @details  This function is called by the IEC bus when the ATN signal raises. 
     */
    void simulateAtnInterrupt();

private:

    /*! @brief    Reads a single bit from the disk head
     *  @result   0 or 1
     */
    inline uint8_t readBitFromHead() { return disk.readBitFromHalftrack(halftrack, bitoffset); }

    /*! @brief    Reads a single byte from the disk head
     *  @result   0 ... 255
     */
    inline uint8_t readByteFromHead() { return disk.readByteFromHalftrack(halftrack, bitoffset); }

    //! @brief Writes a single bit to the disk head
    inline void writeBitToHead(uint8_t bit) { disk.writeBitToHalftrack(halftrack, bitoffset, bit); }
    
    //! @brief Writes a single byte to the disk head
    inline void writeByteToHead(uint8_t byte) { disk.writeByteToHalftrack(halftrack, bitoffset, byte); }
    
    //! @brief  Advances drive head position by one bit
    inline void rotateDisk() { if (++bitoffset >= disk.length.halftrack[halftrack]) bitoffset = 0; }

    //! @brief  Moves drive head position back by one bit
    inline void rotateBack() { bitoffset = (bitoffset > 0) ? (bitoffset - 1) : (disk.length.halftrack[halftrack] - 1); }

    //! @brief  Advances drive head position by eight bits
    inline void rotateDiskByOneByte() { for (unsigned i = 0; i < 8; i++) rotateDisk(); }

    //! @brief  Moves drive head position back by eight bits
    inline void rotateBackByOneByte() { for (unsigned i = 0; i < 8; i++) rotateBack(); }

    //! @brief  Align drive head to the beginning of a byte
    inline void alignHead() { bitoffset &= 0xFFF8; byteReadyCounter = 0; }

    //! @brief Signals the CPU that a byte has been processed
    inline void byteReady();

    //! @brief Signals the CPU that a byte has been processed and load byte into input latch A of via 2
    inline void byteReady(uint8_t byte);
   
public:

    /*! @brief    Performs read access of the fast loader
     *  @details  This method is used to latch in a byte from disk when bit accurate emulation is disabled
     */
    void fastLoaderRead();
 
    /*! @brief    Fast loader sync detection
     *  @details  Returns true when the drive head is currently inside a SYNC mark
     */
    bool getFastLoaderSync();
    
    //! @brief  Skip sync mark
    inline void fastLoaderSkipSyncMark() { while (readByteFromHead() == 0xFF) rotateDiskByOneByte(); }
};

#endif
