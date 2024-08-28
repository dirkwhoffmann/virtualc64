// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// This FILE is dual-licensed. You are free to choose between:
//
//     - The GNU General Public License v3 (or any later version)
//     - The Mozilla Public License v2
//
// SPDX-License-Identifier: GPL-3.0-or-later OR MPL-2.0
// -----------------------------------------------------------------------------

#pragma once

#include "DriveTypes.h"
#include "C64Types.h"
#include "CmdQueueTypes.h"
#include "SubComponent.h"
#include "CPU.h"
#include "Disk.h"
#include "DiskAnalyzer.h"
#include "DriveMemory.h"
#include "VIA.h"
#include "PIA.h"

namespace vc64 {

/*
 * This implementation is based on the following two documents written
 * by Ruud Baltissen. Ruud, thank you for this excellent work!
 *
 * Description: http://www.baltissen.org/newhtm/1541a.htm
 * Schematics:  http://www.baltissen.org/images/1540.gif
 */

class Drive final : public SubComponent, public Inspectable<DriveInfo> {

    Descriptions descriptions = {
        {
            .type           = DriveClass,
            .name           = "Drive8",
            .description    = "First Floppy Drive",
            .shell          = "drive8"
        },
        {
            .type           = DriveClass,
            .name           = "Drive9",
            .description    = "Second Floppy Drive",
            .shell          = "drive9"
        }
    };

    Options options = {

        OPT_DRV_AUTO_CONFIG,
        OPT_DRV_TYPE,
        OPT_DRV_RAM,
        OPT_DRV_SAVE_ROMS,
        OPT_DRV_PARCABLE,
        OPT_DRV_CONNECT,
        OPT_DRV_POWER_SWITCH,
        OPT_DRV_POWER_SAVE,
        OPT_DRV_EJECT_DELAY,
        OPT_DRV_SWAP_DELAY,
        OPT_DRV_INSERT_DELAY,
        OPT_DRV_PAN,
        OPT_DRV_POWER_VOL,
        OPT_DRV_STEP_VOL,
        OPT_DRV_INSERT_VOL,
        OPT_DRV_EJECT_VOL
    };

    friend class DriveMemory;
    friend class VIA1;
    friend class VIA2;

    //
    // Constants
    //

    /* Power-safe threshold measured in frames. If the drive was inactive for
     * the specified number of frames, it is put into power-safe mode (if this
     * option is enabled). In this mode, executing the drive is skipped inside
     * the run loop. As a effect, the current drive state is frozen until the
     * drive is woken up.
     */
    static constexpr i64 powerSafeThreshold = 100;
    
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

    // Current configuration
    DriveConfig config = { };

    
    //
    // Subcomponents
    //
    
public:
    
    DriveMemory mem = DriveMemory(c64, *this);
    CPU cpu = CPU(MOS_6502, c64);
    VIA1 via1 = VIA1(c64, *this);
    VIA2 via2 = VIA2(c64, *this);
    PiaDolphin pia = PiaDolphin(c64, *this);
    
    // The currently inserted disk (if any)
    std::unique_ptr<Disk> disk;
    

    //
    // Disk change logic
    //
    
    // A disk waiting to be inserted
    std::unique_ptr<Disk> diskToInsert;

    
    //
    // Drive state
    //
    
private:

    // Indicates whether the disk is rotating
    bool spinning = false;
    
    // Indicates whether the red LED is on
    bool redLED = false;
    
    // Indicates if or how a disk is inserted
    InsertionStatus insertionStatus = DISK_FULLY_EJECTED;

    
    //
    // Clocking logic
    //
    
    // Elapsed time since power up in 1/10 nano seconds
    u64 elapsedTime = 0;
    
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
    isize zone = 0;
    
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
    // Speed logic (power-save)
    //
    
    // Idle counter
    i64 watchdog = INT64_MAX;

    // Indicates whether execute() should be called inside the run loop
    bool needsEmulation = false;
    
    
    //
    // Methods
    //
    
public:
    
    Drive(C64 &ref, isize id);

    Drive& operator= (const Drive& other) {

        CLONE(mem)
        CLONE(cpu)
        CLONE(via1)
        CLONE(via2)
        CLONE(pia)

        CLONE(spinning)
        CLONE(redLED)
        CLONE(elapsedTime)
        CLONE(nextClock)
        CLONE(nextCarry)
        CLONE(carryCounter)
        CLONE(counterUF4)
        CLONE(bitReadyTimer)
        CLONE(byteReadyCounter)
        CLONE(halftrack)
        CLONE(offset)
        CLONE(zone)
        CLONE(readShiftreg)
        CLONE(writeShiftreg)
        CLONE(sync)
        CLONE(byteReady)
        CLONE(watchdog)
        CLONE(needsEmulation)

        CLONE(insertionStatus)

        CLONE(config)

        if (other.disk && !disk) disk = std::make_unique<Disk>();
        if (!other.disk) disk = nullptr;
        if (disk) *disk = *other.disk;

        if (other.diskToInsert && !diskToInsert) diskToInsert = std::make_unique<Disk>();
        if (!other.diskToInsert) diskToInsert = nullptr;
        if (diskToInsert) *diskToInsert = *other.diskToInsert;

        return *this;
    }


    //
    // Methods from Serializable
    //

public:

    template <class T>
    void serialize(T& worker)
    {
        worker
                
        << spinning
        << redLED
        << elapsedTime
        << nextClock
        << nextCarry
        << carryCounter
        << counterUF4
        << bitReadyTimer
        << byteReadyCounter
        << halftrack
        << offset
        << zone
        << readShiftreg
        << writeShiftreg
        << sync
        << byteReady
        << watchdog
        << needsEmulation;

        if (isResetter(worker)) return;

        worker

        << insertionStatus;

        worker

        << config.type
        << config.ram
        << config.parCable
        << config.powerSave
        << config.connected
        << config.switchedOn
        << config.ejectDelay
        << config.swapDelay
        << config.insertDelay
        << config.pan
        << config.powerVolume
        << config.stepVolume
        << config.insertVolume
        << config.ejectVolume
        << config.saveRoms;
    }
    
    void operator << (SerResetter &worker) override { serialize(worker); };
    void operator << (SerChecker &worker) override;
    void operator << (SerCounter &worker) override;
    void operator << (SerReader &worker) override;
    void operator << (SerWriter &worker) override;


    //
    // Methods from CoreComponent
    //

public:

    const Descriptions &getDescriptions() const override { return descriptions; }

private:

    void _initialize() override;
    void _dump(Category category, std::ostream& os) const override;
    void _didReset(bool hard) override;


    //
    // Methods from Inspectable
    //

public:

    void cacheInfo(DriveInfo &result) const override;


    //
    // Methods from Configurable
    //

public:

    const DriveConfig &getConfig() const { return config; }
    const Options &getOptions() const override { return options; }
    i64 getFallback(Option opt) const override;
    i64 getOption(Option opt) const override;
    void checkOption(Option opt, i64 value) override;
    void setOption(Option opt, i64 value) override;
    void resetConfig() override;

    // Updates the current configuration according to the installed ROM
    void autoConfigure();


    //
    // Working with the drive
    //

public:
    
    // Returns the device number
    isize getDeviceNr() const { return objid; }

    // Convenience wrappers
    bool isDrive8() { return objid == DRIVE8; }
    bool isDrive9() { return objid == DRIVE9; }
    bool hasParCable() { return config.parCable != PAR_CABLE_NONE; }
    ParCableType getParCableType() const { return config.parCable; }

    // Checks whether the drive is ready to be connected
    bool canConnect();

    // Checks whether the drive is connected and switched on
    bool connectedAndOn() { return config.connected && config.switchedOn; }

    // Checks whether the drive has been idle for a while
    bool isIdle() const { return watchdog < 0; }

    // Returns true iff the red drive LED is on
    bool getRedLED() const { return redLED; };

    // Turns the red drive LED on or off
    void setRedLED(bool b);

    // Returns true iff the drive engine is on
    bool isRotating() const { return spinning; };

    // Turns the drive engine on or off
    void setRotating(bool b);
    
    // Wakes up the drive (clears the idle state)
    void wakeUp(isize awakeness = powerSafeThreshold);

    
    //
    // Handling disks
    //

public:

    // Checks whether the drive contains a disk of a certain kind
    bool hasDisk() const;
    bool hasPartiallyRemovedDisk() const;
    bool hasProtectedDisk() const { return hasDisk() && disk->isWriteProtected(); }
    bool hasModifiedDisk() const { return hasDisk() && disk->isModified(); }
    bool hasUnmodifiedDisk() const { return hasDisk() && !hasModifiedDisk(); }
    bool hasUnprotectedDisk() const { return hasDisk() && !hasProtectedDisk(); }

    // Changes the modification state
    void setModificationFlag(bool value);
    void markDiskAsModified() { setModificationFlag(true); }
    void markDiskAsUnmodified() { setModificationFlag(false); }

    // Changes the write-protection state
    void setProtection(bool value);
    void protectDisk() { setProtection(true); }
    void unprotectDisk() { setProtection(false); }
    void toggleProtection();

    /* Returns the current state of the write protection barrier. If the light
     * barrier is blocked, the drive head is unable to modify bits on disk.
     * Note: We block the write barrier on power up for about 1.5 sec, because
     * the drive enters write mode during the power up phase. I'm unsure if
     * this is normal drive behavior or an emulator bug. Any hint on this is
     * very welcome!
     */
    bool getLightBarrier() const {
        return
        cpu.clock < 1500000
        || hasPartiallyRemovedDisk()
        || hasProtectedDisk();
    }

    /* Requests the emulator to inserts or eject a disk. Background: Many C64
     * programs try to detect a disk change by checking the light barrier. This
     * means that proper physical delays have to be taken in account. For that
     * reason, these functions initiate a sequence of events that are processed
     * one after another with a proper time delay. The sequence includes pulling
     * the currently inserted disk halfway out before it is removed completely,
     * and pushing the new disk halfway in before it is inserted completely.
     */
    void insertDisk(const fs::path &path, bool wp) throws;
    void insertDisk(std::unique_ptr<Disk> disk);
    void insertNewDisk(DOSType fstype, string name);
    void insertMediaFile(class MediaFile &file, bool wp);
    void insertD64(const class D64File &d64, bool wp);
    void insertG64(const class G64File &g64, bool wp);
    void insertCollection(class AnyCollection &archive, bool wp) throws;
    void insertFileSystem(const class FileSystem &device, bool wp);
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
    bool readMode() const { return via2.getCB2(); }
    bool writeMode() const { return !readMode(); }
    
    // Returns the current halftrack or track number
    Halftrack getHalftrack() const { return halftrack; }
    Track getTrack() const { return (halftrack + 1) / 2; }

    // Returns the number of bits in a halftrack
    isize sizeOfHalftrack(Halftrack ht) {
        return hasDisk() ? disk->lengthOfHalftrack(ht) : 0; }
    isize sizeOfCurrentHalftrack() { return sizeOfHalftrack(halftrack); }

    // Returns the position of the drive head inside the current track
    HeadPos getOffset() const { return offset; }

    // Moves head one halftrack up
    void moveHeadUp();
    
    // Moves head one halftrack down
    void moveHeadDown();

    // Returns the current value of the sync signal
    bool getSync() const { return sync; }
    
    /* Updates the byte ready line. The byte ready line is connected to pin CA1
     * of VIA2. Pulling this signal low causes important side effects. Firstly,
     * the contents of the read shift register is latched into the VIA chip.
     * Secondly, the V flag is set inside the CPU. See also CA1action().
     */
    void updateByteReady();
    
    // Raises the byte ready line
    void raiseByteReady();
    
    // Returns the current track zone (0 to 3)
    bool getZone() const { return zone; }

    // Sets the current track zone (0 to 3)
    void setZone(isize value);

    // Reads a single bit from the disk head (result is 0 or 1)
    u8 readBitFromHead() const;
    
    // Writes a single bit to the disk head
    void writeBitToHead(u8 bit);
    
    // Advances drive head position by one bit
    void rotateDisk();

    // Performs periodic actions
    void vsyncHandler();
    

    //
    // Processing commands and events
    //

public:

    // Processes a datasette command
    void processCommand(const Cmd &cmd);

    // Initiates the disk change procedure
    void scheduleFirstDiskChangeEvent(EventID id);

    // Carries out the disk change procedure
    void processDiskChangeEvent(EventID id);
};

}
