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

    /* Time between two carry pulses of UE7 in 1/10 nano seconds. The VC1541
     * drive is clocked by 16 Mhz. The base frequency is divided by N where N
     * ranges from 13 (density bits = 11) to 16 (density bits = 00). On the
     * logic board, this is done with a 4-bit counter of type 74SL193 whose
     * reset value bits are connected to the two density bits (PB5 and PB6
     * of VIA2). It follows that a single bit is ready after approx. 3.25 CPU
     * cycles in the fastest zone and approx. 4 CPU cycles in the slowest zone.
     */
    const u64 delayBetweenTwoCarryPulses[4] = {
        
        10000, // Density bits = 00: Carry pulse every 16/16 * 10^4 1/10 nsec
        9375,  // Density bits = 01: Carry pulse every 15/16 * 10^4 1/10 nsec
        8750,  // Density bits = 10: Carry pulse every 14/16 * 10^4 1/10 nsec
        8125   // Density bits = 11: Carry pulse every 13/16 * 10^4 1/10 nsec
    };

    // Device number of this disk drive (8 = first drive, 9 = second drive)
    DriveID deviceNr;

    // Current configuration
    DriveConfig config;

    
    //
    // Sub components
    //
    
public:
    
	DriveMemory mem = DriveMemory(c64, *this);
    DriveCPU cpu = DriveCPU(c64, mem);
    VIA1 via1 = VIA1(c64, *this);
    VIA2 via2 = VIA2(c64, *this);
    Disk disk = Disk(c64);
    

    //
    // Drive state
    //
    
private:
    
    // Indicates whether the drive is active (connected and switched on)
    bool active = false;
    
    // Indicates whether the disk is rotating
    bool spinning = false;
    
    // Indicates whether the red LED is on
    bool redLED = false;
    
    // Indicates if or how a disk is inserted
    InsertionStatus insertionStatus = NOT_INSERTED;
        
    
    //
    // Clocking logic
    //
    
    // Elapsed time since power up in 1/10 nano seconds
    u64 elapsedTime = 0;
    
    // Duration of a single CPU clock cycle in 1/10 nano seconds
    u64 durationOfOneCpuCycle = 0;
    
    /* Indicates when the next drive clock cycle occurs. The VC1541 drive is
     * clocked by 16 MHz. The clock signal is fed into a counter which serves
     * as a frequency divider. It's output is used to clock the drive's CPU and
     * the two VIA chips.
     */
    i64 nextClock = 0;
     
    /* Indicates when the next carry output pulse occurs on UE7. The 16 MHz
     * signal is also fed into UE7, a 74SL193 4-bit couter, which generates a
     * carry output signal on overflow. The pre-load inputs of this counter are
     * connected to PB5 and PB6 of VIA2 (the 'density bits'). This means that a
     * carry signal is generated every 13th cycle (from the 16 Mhz clock) when
     * both density bits are 0 and every 16th cycle when both density bits are
     * 1. The carry signal drives uf4, a counter of the same type.
     */
    i64 nextCarry = 0;
    
public:
    
    /* Counts the number of carry pulses from UE7. In a perfect setting, a new
     * bit is read from or written to the drive after four carry pulses.
     */
    i64 carryCounter = 0;
    
    /* The second 74SL193 4-bit counter on the logic board. This counter is
     * driven by the carry output of UE7. It has four outputs QA, QB, QC, and
     * QD. QA and QB are used to clock most of the other components. QC and QD
     * are fed into a NOR gate whose output is connected to the serial input
     * pin of the input shift register.
     */
    u8 counterUF4 = 0;
    
    
    //
    // Read/Write logic
    //
    
    // The next bit will be ready after this number of cycles
    i16 bitReadyTimer = 0;
    
    /* Byte ready counter (UE3). The VC1540 logic board contains a 4 bit
     * counter of type 72LS191 which is advanced whenever a bit is ready. By
     * reaching 7, the counter signals that a byte is ready. In that case, the
     * write shift register is loaded with new data and pin CA1 of VIA2 changes
     * state. This state change causes the current contents of the read shift
     * register to be latched into the input register of VIA2.
     */
    u8 byteReadyCounter = 0;
    
    // Halftrack position of the drive head
    Halftrack halftrack = 0;
    
    // Position of the drive head inside the current track
    HeadPos offset = 0;
    
    /* Current disk zone. Each track belongs to one of four zones. Whenever the
     * drive moves the r/w head, it computes the new number and writes into PB5
     * and PB6 of via2. These bits are hard-wired to a 74LS193 counter on the
     * logic board that breaks down the 16 Mhz base frequency. This mechanism
     * is used to slow down the read/write process on inner tracks.
     */
    u8 zone = 0;
    
    /* The 74LS164 serial to parallel shift register. In read mode, this
     * register is fed by the drive head with data.
     */
    u16 readShiftreg = 0;
    
    /* The 74LS165 parallel to serial shift register. In write mode, this
     * register feeds the drive head with data.
     */
    u8 writeShiftreg = 0;
    
    /* Current value of the SYNC line. The SYNC signal plays an important role
     * for timing synchronization. It becomes true when the beginning of a SYNC
     * is detected. On the logic board, the SYNC signal is computed by a NAND
     * gate that combines the 10 previously read bits from the input shift
     * register and VIA2::CB2 (the r/w mode pin). Connecting CB2 to the NAND
     * gates ensures that SYNC can only be true in read mode. When SYNC becomes
     * false (meaning that a 0 was pushed into the shift register), the
     * byteReadyCounter is reset.
     */
    bool sync = false;
    
    /* Current value of the ByteReady line. This signal goes low when a byte
     * has been processed.
     */
    bool byteReady = false;
    
    
    //
    // Initializing
    //
    
public:
    
    Drive(DriveID id, C64 &ref);
    
private:

    void _reset() override;

    
    //
    // Configuring
    //
    
public:
    
    DriveConfig getConfig() { return config; }
    
    long getConfigItem(ConfigOption option);
    bool setConfigItem(ConfigOption option, long value) override;
    bool setConfigItem(DriveID id, ConfigOption option, long value) override;
        
    
    //
    // Analyzing
    //
    
private:
    
    void _dump() override;

    
    //
    // Serializing
    //
    
private:
    
    template <class T>
    void applyToPersistentItems(T& worker)
    {
        worker
        
        & durationOfOneCpuCycle
        & config.type
        & config.connected
        & insertionStatus;
    }
    
    template <class T>
    void applyToResetItems(T& worker)
    {
        worker
        
        & spinning
        & redLED
        & elapsedTime
        & nextClock
        & nextCarry
        & carryCounter
        & counterUF4
        & bitReadyTimer
        & byteReadyCounter
        & halftrack
        & offset
        & zone
        & readShiftreg
        & writeShiftreg
        & sync
        & byteReady;
    }
    
    size_t _size() override { COMPUTE_SNAPSHOT_SIZE }
    size_t _load(u8 *buffer) override { LOAD_SNAPSHOT_ITEMS }
    size_t _save(u8 *buffer) override { SAVE_SNAPSHOT_ITEMS }
    
    
    //
    // Working with the drive
    //

public:

    // Checks whether the drive is active (connected and switched on)
    bool isActive() { return active; }
    
    // Returns the device number
    DriveID getDeviceNr() { return deviceNr; }
        
    // Returns true iff the red drive LED is on
    bool getRedLED() { return redLED; };

    // Turns the red drive LED on or off
    void setRedLED(bool b);

    // Returns true iff the drive engine is on
    bool isRotating() { return spinning; };

    // Turns the drive engine on or off
    void setRotating(bool b);
    
    
    //
    // Handling disks
    //

public:
    
    // Checks if a disk is present
    bool hasDisk() { return insertionStatus == FULLY_INSERTED; }
    bool hasPartiallyInsertedDisk() { return insertionStatus == PARTIALLY_INSERTED; }
    bool hasWriteProtectedDisk() { return hasDisk() && disk.isWriteProtected(); }

    // Gets or sets the modification status
    bool hasModifiedDisk() { return hasDisk() && disk.isModified(); }
    void setModifiedDisk(bool value);
 
    /* Prepares to insert a disk. This functions puts a disk partially into the
     * drive. As a result, the light barrier is blocked. You must not call this
     * functions if a disk is already inserted.
     */
    void prepareToInsert();
    
    /* Inserts an archive as a virtual disk. You have to eject a previously
     * inserted disk before calling this function. Note: Inserting an archive
     * as a disk is a time consuming task, because various conversion have to
     * take place. E.g., if you provide a T64 archive, it is first converted to
     * a D64 archive. After that, all tracks will be GCR-encoded and written to
     * a new disk.
     */
    void insertDisk(AnyArchive *a);

    /* Inserts a disk
     */
    void insertDisk(Disk *disk);
    
    /* Returns the current state of the write protection barrier. If the light
     * barrier is blocked, the drive head is unable to modify bits on disk.
     * Note: We block the write barrier on power up for about 1.5 sec, because
     * the drive enters write mode during the power up phase. I'm unsure if
     * this is normal drive behavior or an emulator bug. Any hint on this is
     * very welcome!
     */
    bool getLightBarrier() {
        return
        (cpu.cycle < 1500000)
        || hasPartiallyInsertedDisk()
        || disk.isWriteProtected();
    }

    /* Prepares to eject a disk. This functions opens the drive lid and
     * partially removes the disk. As a result, no data can be read any more
     * and the light barrier is blocked. Warning: Only call this functions if a
     * disk is inserted.
     */
    void prepareToEject();
    
    /* Finishes the ejection of a disk. This function assumes that the drive
     * lid is already open. It fully removes the disk and frees the light
     * barrier. Note: To eject a disk in the right way, make sure that some
     * time elapses between the two calls to prepareToEject() and ejectDisk().
     * Otherwise, the VC1541 DOS does not recognize the ejection.
     */
    void ejectDisk();
   
    
    //
    // Emulating
    //
    
public:
    
    /* Executes all pending cycles of the virtual drive. The number of cycles
     * is determined by the target time which is elapsedTime + duration.
     */
    void execute(u64 duration);

private:
    
    // Emulates a trigger event on the carry output pin of UE7.
    void executeUF4();
    
public:

    // Returns the current access mode of this drive (read or write)
    bool readMode() { return via2.getCB2(); }
    bool writeMode() { return !readMode(); }
    
    // Returns the current halftrack or track number
    Halftrack getHalftrack() { return halftrack; }
    Track getTrack() { return (halftrack + 1) / 2; }
        
    // Returns the number of bits in a halftrack
    u16 sizeOfHalftrack(Halftrack ht) {
        return hasDisk() ? disk.lengthOfHalftrack(ht) : 0; }
    u16 sizeOfCurrentHalftrack() { return sizeOfHalftrack(halftrack); }

    // Returns the position of the drive head inside the current track
    HeadPos getOffset() { return offset; }

    // Moves head one halftrack up
    void moveHeadUp();
    
    // Moves head one halftrack down
    void moveHeadDown();

    // Returns the current value of the sync signal
    bool getSync() { return sync; }
    
    /* Updates the byte ready line. The byte ready line is connected to pin CA1
     * of VIA2. Pulling this signal low causes important side effects. Firstly,
     * the contents of the read shift register is latched into the VIA chip.
     * Secondly, the V flag is set inside the CPU. See also CA1action().
     */
    void updateByteReady();
    
    // Raises the byte ready line
    void raiseByteReady();
    
    // Returns the current track zone (0 to 3)
    bool getZone() { return zone; }

    // Sets the current track zone (0 to 3)
    void setZone(u8 value);

    // Reads a single bit from the disk head (result is 0 or 1)
    u8 readBitFromHead() { return disk.readBitFromHalftrack(halftrack, offset); }
    
    // Writes a single bit to the disk head
    void writeBitToHead(u8 bit) { disk.writeBitToHalftrack(halftrack, offset, bit); }
    
    // Advances drive head position by one bit
    void rotateDisk() { if (++offset >= disk.lengthOfHalftrack(halftrack)) offset = 0; }
};

#endif
