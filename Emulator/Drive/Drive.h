// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

/*
 * This implementation is based on the following two documents written
 * by Ruud Baltissen. Ruud, thank you for this excellent work!
 *
 * Description: http://www.baltissen.org/newhtm/1541a.htm
 * Schematics:  http://www.baltissen.org/images/1540.gif
 */
 
#ifndef _DRIVE_H
#define _DRIVE_H

#include "VIA.h"
#include "Disk.h"

class Drive : public C64Component {
    
    //
    // Constants
    //

    // Time between two carry pulses of UE7 in 1/10 nano seconds. The VC1541
    // drive is clocked by 16 Mhz. The base frequency is divided by N where N
    // ranges from 13 (density bits = 11) to 16 (density bits = 00). On the
    // logic board, this is done with a 4-bit counter of type 74SL193 whose
    // reset value bits are connected to the two density bits (PB5 and PB6
    // of VIA2). It follows that a single bit is ready after approx. 3.25 CPU
    // cycles in the fastest zone and approx. 4 CPU cycles in the slowest zone.

    const u64 delayBetweenTwoCarryPulses[4] = {
        
        10000, // Density bits = 00: Carry pulse every 16/16 * 10^4 1/10 nsec
        9375,  // Density bits = 01: Carry pulse every 15/16 * 10^4 1/10 nsec
        8750,  // Density bits = 10: Carry pulse every 14/16 * 10^4 1/10 nsec
        8125   // Density bits = 11: Carry pulse every 13/16 * 10^4 1/10 nsec
    };
    
    //
    // Sub components
    //
    
    // Current configuration
    DriveConfig config;
    
public:
    
	DriveMemory mem = DriveMemory(c64, *this);
    CPU cpu = CPU(MOS_6502, c64, mem);
    VIA1 via1 = VIA1(this, c64);
    VIA2 via2 = VIA2(this, c64);
    Disk disk = Disk(c64);
    
    
    
    //
    // Drive status
    //
    
private:
    
    // Device number of this disk drive (8 = first drive, 9 = second drive)
    DriveID deviceNr;
    
    //! @brief    Indicates whether the disk is rotating.
    bool spinning;
    
    //! @brief    Indicates whether the red LED is on.
    bool redLED;
    
    //! @brief    Indicates if or how a disk is inserted.
    InsertionStatus insertionStatus;
        
    
    //
    // Clocking logic
    //
    
    //! @brief    Elapsed time since power up in 1/10 nano seconds
    u64 elapsedTime;
    
    //! @brief    Duration of a single CPU clock cycle in 1/10 nano seconds
    u64 durationOfOneCpuCycle;
    
    /*! @brief    Indicates when the next drive clock cycle occurs.
     *  @details  The VC1541 drive is clocked by 16 MHz. The clock signal is
     *            fed into a counter which serves as a frequency divider. It's
     *            output is used to clock the drive's CPU and the two VIA chips.
     */
    i64 nextClock;
     
    /*! @brief    Indicates when the next carry output pulse occurs on UE7.
     *  @details  The 16 MHz signal is also fed into UE7, a 74SL193 4-bit
     *            couter, which generates a carry output signal on overflow.
     *            The pre-load inputs of this counter are connected to PB5 and
     *            PB6 of VIA2 (the 'density bits'). This means that a carry
     *            signal is generated every 13th cycle (from the 16 Mhz clock)
     *            when both density bits are 0 and every 16th cycle when both
     *            density bits are 1. The carry signal drives uf4, a counter of
     *            the same type.
     */
    i64 nextCarry;
    
public:
    
    /*! @brief    Counts the number of carry pulses from UE7.
     *  @details  In a perfect setting, a new bit is read from or written to the
     *            drive after four carry pulses.
     */
    i64 carryCounter;
    
    /*! @brief    The second 74SL193 4-bit counter on the logic board.
     *  @details  This counter is driven by the carry output of UE7. It has
     *            four outputs QA, QB, QC, and QD. QA and QB are used to clock
     *            most of the other components. QC and QD are fed into a
     *            NOR gate whose output is connected to the serial input pin of
     *            the input shift register.
     */
    uint4_t counterUF4;
    
    
    //
    // Read/Write logic
    //
    
    //! @brief    The next bit will be ready after this number of cycles.
    i16 bitReadyTimer;
    
    /*! @brief    Byte ready counter (UE3)
     *  @details  The VC1540 logic board contains a 4-bit-counter of type
     *            72LS191 which is advanced whenever a bit is ready. By reaching
     *            7, the counter signals that a byte is ready. In that case,
     *            the write shift register is loaded with new data and pin CA1
     *            of VIA2 changes state. This state change causes the current
     *            contents of the read shift register to be latched into the
     *            input register of VIA2.
     */
    u8 byteReadyCounter;
    
    //! @brief    Halftrack position of the read/write head
    Halftrack halftrack;
    
    //! @brief    Position of the drive head inside the current track
    HeadPosition offset;
    
    /*! @brief    Current disk zone
     *  @details  Each track belongs to one of four zones. Whenever the drive
     *            moves the r/w head, it computes the new number and writes into
     *            PB5 and PB6 of via2. These bits are hard-wired to a 74LS193
     *            counter on the logic board that breaks down the 16 Mhz base
     *            frequency. This mechanism is used to slow down the read/write
     *            process on inner tracks.
     */
    u8 zone;
    
    /*! @brief    The 74LS164 serial to parallel shift register
     *  @details  In read mode, this register is fed by the drive head with data.
     */
    u16 readShiftreg;
    
    /*! @brief    The 74LS165 parallel to serial shift register
     *  @details  In write mode, this register feeds the drive head with data.
     */
    u8 writeShiftreg;
    
    /*! @brief    Current value of the SYNC line
     *  @details  The SYNC signal plays an important role for timing
     *            synchronization. It becomes true when the beginning of a SYNC
     *            is detected. On the logic board, the SYNC signal is computed
     *            by a NAND gate that combines the 10 previously read bits from
     *            the input shift register and VIA2::CB2 (the r/w mode pin).
     *            Connecting CB2 to the NAND gates ensures that SYNC can only be
     *            true in read mode. When SYNC becomes false (meaning that a 0
     *            was pushed into the shift register), the byteReadyCounter is
     *            reset.
     */
    bool sync;
    
    /*! @brief    Current value of the ByteReady line
     *  @details  This signal goes low when a byte has been processed.
     */
    bool byteReady;
    
    
    //
    // Constructing and serializing
    //
    
public:
    
    Drive(DriveID id, C64 &ref);
    
    
    //
    // Configuring
    //
    
    DriveConfig getConfig() { return config; }
    
    DriveType getType() { return config.type; }
    void setType(DriveType type) { config.type = type; }

    bool isConnectable();
    bool isConnected() { return config.connected; }
    bool isDisconnected() { return !config.connected; }
    
    void setConnected(bool value);
    void connect() { setConnected(true); }
    void disconnect() { setConnected(false); }
    void toggleConnection() { isConnected() ? disconnect() : connect(); }

    //
    // Methods from HardwareComponent
    //
    
public:

    void _initialize() override;
    void _reset() override;

private:
    
    void _ping() override;
    void _dump() override;
    void _setClockFrequency(u32 value) override;
    
public:
    
    /*! @brief    Resets all disk related properties
     *  @note     This method is needed, because reset() keeps the disk alive.
     */
    void resetDisk();
    

    //
    //! @functiongroup Working with the drive
    //

    // Returns the device number
    DriveID getDeviceNr() { return deviceNr; }
        
    //! @brief    Returns true iff the red drive LED is on.
    bool getRedLED() { return redLED; };

    //! @brief    Turns red drive LED on or off.
    void setRedLED(bool b);

    //! @brief    Returns true iff the drive engine is on.
    bool isRotating() { return spinning; };

    //! @brief    Turns the drive engine on or off.
    void setRotating(bool b);
    
    
    //
    // Handling disks
    //

    // Checks if a disk is present
    bool hasDisk() { return insertionStatus == FULLY_INSERTED; }
    bool hasPartiallyInsertedDisk() { return insertionStatus == PARTIALLY_INSERTED; }
    bool hasWriteProtectedDisk() { return hasDisk() && disk.isWriteProtected(); }

    // Get or sets the modification status
    bool hasModifiedDisk() { return hasDisk() && disk.isModified(); }
    void setModifiedDisk(bool value);

    /* Prepares to insert a disk. This functions puts a disk partially into
     * the drive. As a result, the light barrier is blocked.
     * Warning: Only call this functions if no disk is inserted.
     */
    void prepareToInsert();
    
    /* Inserts an archive as a virtual disk.
     * Warning: Make sure to eject a previously inserted disk before calling
     *          this function.
     * Note:    Inserting an archive as a disk is a time consuming task
     *          because variouls conversion have to take place. E.g., if you
     *          provide a T64 archive, it is first converted to a D64 archive.
     *          After that, all tracks will be GCR-encoded and written to a
     *          new disk.
     */
    void insertDisk(AnyArchive *a);

    /* Returns the current state of the write protection barrier. If the light
     * barrier is blocked, the drive head is unable to modify bits on disk.
     * Note:   We block the write barrier on power up for about 1.5 sec, because
     *         the drive enters write mode during the power up phase. I'm unsure
     *         if this is normal drive behavior or an emulator bug. Any hint on
     *         this is very welcome!
     */
    bool getLightBarrier() {
        return
        (cpu.cycle < 1500000)
        || hasPartiallyInsertedDisk()
        || disk.isWriteProtected();
    }

    /* Prepares to eject a disk. This functions opens the drive lid and
     * partially removes the disk. As a result, no data can be read any more
     * and the light barrier is blocked.
     *  Warning: Only call this functions if a disk is inserted.
     */
    void prepareToEject();
    
    /* Finishes the ejection of a disk. This function assumes that the drive
     * lid is already open. It fully removes the disk and frees the light
     * barrier.
     * Note: To eject a disk in the right way, make sure that some time
     *       elapsed between the two calls to prepareToEject() and
     *       ejectDisk(). Otherwise, the VC1541 DOS does not recognize
     *       the ejection.
     */
    void ejectDisk();
   
    
    //
    //! @functiongroup Running the device
    //
    
    //! @brief    Cold starts the floppy drive
    /*! @details  Mimics the effect of switching the drive off and on again.
     */
    void powerUp();

    //! @brief    Executes all pending cycles of the virtual drive
    /*! @details  The number of cycles is determined by the target time
     *            which is elapsedTime + duration.
     */
    void execute(u64 duration);

private:
    
    //! @brief   Emulates a trigger event on the carry output pin of UE7.
    void executeUF4();
    
public:

    // Returns the current access mode of this drive (read or write)
    bool readMode() { return via2.getCB2(); }
    bool writeMode() { return !readMode(); }
    
    // Returns the current halftrack or track number
    Halftrack getHalftrack() { return halftrack; }
    Track getTrack() { return (halftrack + 1) / 2; }
    
    // Moves the drive head to the specified track
    void setHalftrack(Halftrack ht) { assert(isHalftrackNumber(ht)); halftrack = ht; }
    void setTrack(Track t) { assert(isTrackNumber(t)); halftrack = 2 * t - 1; }
    
    //! @brief    Returns the number of bits in the current halftrack
    u16 sizeOfCurrentHalftrack() {
        return hasDisk() ? disk.lengthOfHalftrack(halftrack) : 0; }

    //! @brief    Returns the position of the drive head inside the current track
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
    
    //! @brief    Updates the value on the byte ready line
    /*! @note     The byte ready line is connected to the CA1 pin of VIA2.
     *            Pulling this signal low causes important side effects.
     *            Firstly, the contents of the read shift register is latched
     *            into the VIA chip. Secondly, the V flag is set inside the CPU.
     *  @seealso  CA1action()
     */
    void updateByteReady();
    
    //! @brief    Raises the byte ready line
    void raiseByteReady();
    
    //! @brief    Returns the current track zone (0 to 3)
    bool getZone() { return zone; }

    /*! @brief    Sets the current track zone
     *  @param    z drive zone (0 to 3)
     */
    void setZone(uint2_t value);

    /*! @brief    Reads a single bit from the disk head
     *  @result   0 or 1
     */
    u8 readBitFromHead() { return disk.readBitFromHalftrack(halftrack, offset); }
    
    //! @brief Writes a single bit to the disk head
    void writeBitToHead(u8 bit) {
        disk.writeBitToHalftrack(halftrack, offset, bit); }
    
    //! @brief  Advances drive head position by one bit
    void rotateDisk() { if (++offset >= disk.lengthOfHalftrack(halftrack)) offset = 0; }

    //! @brief  Moves drive head position back by one bit
    void rotateBack() { if (--offset < 0) offset = disk.lengthOfHalftrack(halftrack) - 1; }

private:
    
    //! @brief    Advances drive head position by eight bits
    void rotateDiskByOneByte() { for (unsigned i = 0; i < 8; i++) rotateDisk(); }

    //! @brief    Moves drive head position back by eight bits
    void rotateBackByOneByte() { for (unsigned i = 0; i < 8; i++) rotateBack(); }
};

#endif
