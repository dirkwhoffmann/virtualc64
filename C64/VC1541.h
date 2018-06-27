/*! 
 * @header      VC1541.h
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   2008 - 2018 Dirk W. Hoffmann
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

/*
 * This implementation is based on the following two documents written
 * by Ruud Baltissen. Ruud, thank you for this excellent work!
 *
 * Description: http://www.baltissen.org/newhtm/1541a.htm
 * Schematics:  http://www.baltissen.org/images/1540.gif
 */
 
#ifndef _VC1541_INC
#define _VC1541_INC

#include "VIA6522.h"
#include "Disk.h"

/*!
 * @brief    Virtual floppy drive
 * @details  Bit-accurate emulation of a VC1541
 */
class VC1541 : public VirtualComponent {

public:
    
	//! @brief    CPU of the virtual drive (6502)
	CPU cpu;
	
	//! @brief    Memory of the virtual drive
	VC1541Memory mem;

	//! @brief    VIA6522 connecting the drive CPU with the IEC bus
    VIA1 via1;

    //! @brief    VIA6522 connecting the drive CPU with the drives read/write head
    VIA2 via2;

    //! @brief    Disk in this drive (single sided 5,25" floppy disk)
    Disk disk;
    
    //
    // Drive status
    //
    
private:
    
    //! @brief    Indicates whether disk is rotating or not
    bool spinning;
    
    //! @brief    Indicates whether red LED is on or off
    bool redLED;
    
    /*! @brief    Indicates whether a disk is inserted.
     *  @note     A fully inserted disk blocks the write protection barrier
     *            if it is write protected.
     */
    bool diskInserted;
    
    /*! @brief    Indicates whether a disk is inserted only partially.
     *  @note     A partially inserted disk blocks always blocks the write
     *            protection barrier
     */
    bool diskPartiallyInserted;
    
    //! @brief    Indicates whether the drive shall seld sound notifications
    bool sendSoundMessages;
    
    
    //
    // Clocking logic
    //
    
    //! @brief    Duration of a single CPU clock cycle in pico seconds
    uint64_t durationOfOneCpuCycle;
    
    /*! @brief    Indicates when the next carry output pulse occurs on UE7.
     *  @details  The VC1541 drive is clocked by 16 Mhz. The clock signal is
     *            fed into UE7, a 74SL193 4-bit couter, which generates a
     *            carry output signal on overflow. The pre-load inputs of this
     *            counter are connected to PB5 and PB6 of VIA2 (the 'density
     *            bits'). This means that a carry signal is generated every
     *            13th cycle (from the 16 Mhz clock) when both density bits are
     *            0 and every 16th cycle when both density bits are 1. The carry
     *            signal drives uf4, a counter of the same type.
     */
    int64_t nextCarry;
    
    /*! @brief    The second 74SL193 4-bit counter on the logic board.
     *  @details  This counter is driven by the carry output of UE7. It has
     *            four outputs QA, QB, QC, and QD. QA and QB are used to clock
     *            most of the other components. QC and QD are fed into a
     *            NOR gate whose output is connected to the serial input pin of
     *            the input shift register.
     */
    uint4_t counterUF4;
    
    /*! @brief    A 74SL191 4-bit counter on the logic board.
     *  @details  This counter is driven by the output pin QB of UF4.
     *            Whenever the three lower output bits equal 1, it generates
     *            a pulse which is used to compute the write register's load
     *            signal and the byte ready signal.
     */
    uint4_t counterUE3;
    
    //
    // Read/Write logic
    //
    
    //! @brief    The next bit will be ready after this number of cycles.
    int16_t bitReadyTimer;
    
    /*! @brief    Serial load signal
     *  @details  The VC1541 logic board contains a 4-bit-counter of type 72LS191
     *            which is advanced whenever a bit is ready. By reaching 7, the
     *            counter signals that a byte is ready. In that case, the write
     *            shift register is loaded with new data and the byte ready signal,
     *            which is connected to CA1 of VIA2, changes state. In read mode,
     *            this state change will feed the input latch of VIA2 with the
     *            current contents of the read shift register.
     */
    uint8_t byteReadyCounter;
    
    //! @brief    Halftrack position of the read/write head
    Halftrack halftrack;
    
    //! @brief    Position of the drive head inside the current track
    HeadPosition offset;
    
    /*! @brief    Current disk zone
     *  @details  Each track belongs to one of four zones. Whenever the drive moves
     *            the r/w head, it computes the new number and writes into PB5 and
     *            PB6 of via2. These bits are hard-wired to a 74LS193 counter on
     *            the logic board that breaks down the 16 Mhz base frequency.
     *            This mechanism is used to slow down the read/write process on
     *            inner tracks.
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
     *  @details  This signal plays an important role for timing synchronization.
     *            It becomes true when the beginning of a SYNC is detected. On the
     *            logic board, the SYNC signal is computed by a NAND gate that combines
     *            the 10 previously read bits rom the input shift register and CB2
     *            of VIA2 (the r/w mode pin). Connecting CB2 to the NAND gates ensures
     *            that SYNC can only be true in read mode. When SYNC becomes false
     *            (meaning that a 0 was pushed into the shift register), the
     *            byteReadyCounter is reset.
     */
    bool sync;
    
public:

    //! @brief    Constructor
    VC1541();
    
    //! @brief    Destructor
    ~VC1541();
    
    //! @brief    Methods from VirtualComponent
    void reset();
    void ping();
    void dumpState();
    void setClockFrequency(uint32_t frequency);

    /*! @brief    Resets disk properties
     *  @details  Resets all disk related properties. reset() keeps the disk alive. 
     */
    void resetDisk();
    
    /*! @brief    Indicates how many clock cycles are needed for reading or writing a single bit
     *  @details  The VC1541 drive is clocked by 16 Mhz. The base frequency is divided by N where
     *            N ranges from 13 (zone 0) to 16 (zone 4). On the logic board, this is done with
     *            a 4-bit counter of type 74SL193 whose reset value bits are connected to the two
     *            "zone" bits (PB5 and PB6) coming from via 2. A second 74SL193 divides the signal
     *            by 4. The result serves as the clock signal for all units operating on bit level
     *            (i.e., the two shift registers that transfer bits from and to the head). It
     *            follows that a single bit is ready after 3,25 CPU cycles in zone 0 and 4 CPU
     *            cycles in zone 4. The resulting signal is fed into a third counter (of type
     *            74LS191). It divides the signal by 8 and its output is fed into a three input
     *            NAND-gate computing the important BYTE-READY signal.
     * @deprecated and most likely wrong
     */
    /*
    const uint16_t cyclesPerBit[4] = {
        
        13 * 4, // Zone 0: One bit each (16 * 3.25) base clock cycles (= 3.25 CPU cycles)
        14 * 4, // Zone 1: One bit each (16 * 3.5) base clock cycles (= 3.5 CPU cycles)
        15 * 4, // Zone 2: One bit each (16 * 3.75) base clock cycles (3.75 CPU cycles)
        16 * 4, // Zone 3: One bit each (16 * 4) base clock cycles (4 CPU cycles)
    };
    */
    const uint16_t cyclesPerBit[4] = {
        
        16 * 4, // Zone 0: One bit each (16 * 4) base clock cycles (= 4 CPU cycles)
        15 * 4, // Zone 1: One bit each (16 * 3.75) base clock cycles (= 3.75 CPU cycles)
        14 * 4, // Zone 2: One bit each (16 * 3.5) base clock cycles (3.5 CPU cycles)
        13 * 4, // Zone 3: One bit each (16 * 3.25) base clock cycles (3.25 CPU cycles)
    };
    
    //! @brief    Time between two carry pulses of counter UE7 in pico seconds
    
    const uint64_t delayBetweenTwoCarryPulses[4] = {
        1000000, // Density bits = 00: Carry pulse every 16/16 * 10^6 psec
        937500,  // Density bits = 01: Carry pulse every 15/16 * 10^6 psec
        875000,  // Density bits = 10: Carry pulse every 14/16 * 10^6 psec
        812500   // Density bits = 11: Carry pulse every 13/16 * 10^6 psec
    };
    
    // For now, we use the values below. If we substitute the correct ones,
    // we get a timing problem. Check VIA2 handling (which is most likely the
    // culprit).
    /*
    const uint64_t delayBetweenTwoCarryPulses[4] = {
        1000000, // Density bits = 00: Carry pulse every 16/16 * 10^6 psec
        1000000,  // Density bits = 01: Carry pulse every 15/16 * 10^6 psec
        1000000,  // Density bits = 10: Carry pulse every 14/16 * 10^6 psec
        1000000   // Density bits = 11: Carry pulse every 13/16 * 10^6 psec
    };
     */
    
    // DELETE!
    uint64_t oldCycle, oldCycle2, oldCycle3;
    
    //
    //! @functiongroup Configuring the device
    //
    
    //! @brief    Returns true if sound messages are sent to the GUI.
    bool soundMessagesEnabled() { return sendSoundMessages; }

    //! @brief    Enables or disables sending of sound messages.
    void setSendSoundMessages(bool b) { sendSoundMessages = b; }

    
    //
    //! @functiongroup Accessing drive properties
    //
    
    //! @brief    Returns true iff the red drive LED is on.
    bool getRedLED() { return redLED; };

    //! @brief    Turns red drive LED on or off.
    void setRedLED(bool b);

    //! @brief    Returns true iff the drive engine is on.
    bool isRotating() { return spinning; };

    //! @brief    Turns the drive engine on or off.
    void setRotating(bool b);
    
    
    //
    //! @functiongroup Handling virtual disks
    //

    //! @brief    Returns true if a disk is inserted.
    bool hasDisk() { return diskInserted; }

    //! @brief    Returns true if a modified disk is inserted.
    bool hasModifiedDisk() { return hasDisk() && disk.isModified(); }

    /*! @brief    Inserts an archive as a virtual disk.
     *  @details  This function consumes some time as it needs to perform various conversions.
     *            E.g., if you provide a T64 archive, it is first converted to an D64 archive.
     *            After that, all tracks will be GCR-encoded and written to a new disk.
     */
    bool insertDisk(Archive *a);
    
    //! @brief    Returns true if a disk is partially inserted.
    bool isDiskPartiallyInserted() { return diskPartiallyInserted; }

    //! @brief    Sets if a disk is partially inserted.
    void setDiskPartiallyInserted(bool b) { diskPartiallyInserted = b; }

    /*! @brief    Returns the current status of the write protection light barrier
     *  @details  If the light barrier is blocked, the drive head is unable to change data bits
     */
    bool getLightBarrier() { return isDiskPartiallyInserted() || disk.isWriteProtected(); }

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
     *  @seealso  executeUE7
     *  @seealso  executeBitReady
     *  @seealso  executeByteReady 
     */
    bool executeOneCycle();

private:
    
    //! @brief   Emulates a trigger event on the carry output pin of UE7.
    void executeUE7(); 
        
    /*! @brief    Helper method for executeOneCycle
     *  @details  Method is executed whenever a single bit is ready
     */
    void executeBitReady();

    /*! @brief    Helper method for executeBitReady
     *  @details  Method is executed whenever a single byte is ready
     */
    void executeByteReady();
    
            
public:

    //! @brief    Returns true if drive is in read mode
    bool readMode() { return (via2.getPcr() & 0x20); }

    //! @brief    Returns true if drive is in write mode
    bool writeMode() { return !(via2.getPcr() & 0x20); }

    //! @brief    Returns the halftrack under the drive head
    Halftrack getHalftrack() { return halftrack; }

    //! @brief    Moves the drive head to the specified track
    void setTrack(Track t) { assert(isTrackNumber(t)); halftrack = 2 * t - 1; }

    //! @brief    Moves the drive head to the specified halftrack
    void setHalftrack(Halftrack ht) { assert(isHalftrackNumber(ht)); halftrack = ht; }
    
    //! @brief    Returns the number of bits in the current halftrack
    uint16_t sizeOfCurrentHalftrack() {
        return hasDisk() ? disk.lengthOfHalftrack(halftrack) : 0; }

    //! @brief    Returns the position of the read/write head inside the current track
    HeadPosition getOffset() { return offset; }

    //! @brief    Sets the position of the drive head inside the current track
    void setOffset(HeadPosition pos) {
        if (hasDisk() && disk.isValidHeadPositon(halftrack, pos)) offset = pos;
    }

    //! @brief    Moves head one halftrack up
    void moveHeadUp();
    
    //! @brief    Moves head one halftrack down
    void moveHeadDown();

    //! @brief    Returns the current value of the sync signal
    bool getSync() { return sync; }

    //! @brief    Returns the current track zone (0 to 3)
    bool getZone() { return zone; }

    /*! @brief    Sets the current track zone
     *  @param    z drive zone (0 to 3)
     */
    void setZone(uint2_t value);

    /*! @brief    Reads a single bit from the disk head
     *  @result   0 or 1
     */
    uint8_t readBitFromHead() { return disk.readBitFromHalftrack(halftrack, offset); }
    
    //! @brief Writes a single bit to the disk head
    void writeBitToHead(uint8_t bit) { disk.writeBitToHalftrack(halftrack, offset, bit); }
    
    //! @brief  Advances drive head position by one bit
    void rotateDisk() { if (++offset >= disk.lengthOfHalftrack(halftrack)) offset = 0; }

    //! @brief  Moves drive head position back by one bit
    void rotateBack() { if (--offset < 0) offset = disk.lengthOfHalftrack(halftrack) - 1; }

private:
    
    //! @brief    Advances drive head position by eight bits
    void rotateDiskByOneByte() { for (unsigned i = 0; i < 8; i++) rotateDisk(); }

    //! @brief    Moves drive head position back by eight bits
    void rotateBackByOneByte() { for (unsigned i = 0; i < 8; i++) rotateBack(); }

    //! @brief    Signals the CPU that a byte has been processed.
    void byteReady();

    //! @brief    Signals the CPU that a byte has been processed.
    /*! @details  Additionally, the byte is loaded into input latch A of VIA 2
     */
    void byteReady(uint8_t byte);
};

#endif
