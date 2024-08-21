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

#include "config.h"
#include "Drive.h"
#include "Emulator.h"
#include "IOUtils.h"

namespace vc64 {

Drive::Drive(C64 &ref, isize id) : SubComponent(ref, id)
{
    assert(id == DRIVE8 || id == DRIVE9);

    disk = std::make_unique<Disk>();
    
    subComponents = std::vector <CoreComponent *> {
        
        &mem,
        &cpu,
        &via1,
        &via2
    };

    // Assign a unique ID to the CPU
    cpu.setID(id == DRIVE8 ? 1 : 2);
}

void
Drive::_initialize()
{    
    insertionStatus = DISK_FULLY_EJECTED;
    if (disk) disk->clearDisk();
}

bool
Drive::canConnect()
{
    return c64.hasRom(ROM_TYPE_VC1541);
}

void
Drive::execute(u64 duration)
{
    elapsedTime += duration;
    
    while (nextClock < (i64)elapsedTime || nextCarry < (i64)elapsedTime) {

        if (nextClock <= nextCarry) {
            
            // Execute CPU and VIAs
            i64 cycle = ++cpu.clock;
            cpu.execute<MOS_6502>();
            if (cycle >= via1.wakeUpCycle) via1.execute(); else via1.idleCounter++;
            if (cycle >= via2.wakeUpCycle) via2.execute(); else via2.idleCounter++;
            updateByteReady();
            nextClock += 10000;

        } else {
            
            // Execute read/write logic
            if (spinning) executeUF4();
            nextCarry += delayBetweenTwoCarryPulses[zone];
        }
    }
    assert(nextClock >= (i64)elapsedTime && nextCarry >= (i64)elapsedTime);
}

void
Drive::executeUF4()
{
    // Increase counter
    counterUF4++;
    carryCounter++;
    
    // We assume that a new bit comes in every fourth cycle.
    // Later, we can decouple timing here to emulate asynchronicity.
    if (carryCounter % 4 == 0) {
        
        // When a bit comes in and ...
        //   ... it's value equals 0, nothing happens.
        //   ... it's value equals 1, counter UF4 is reset.
        if (readMode() && hasDisk() && readBitFromHead()) {
            counterUF4 = 0;
        }
        rotateDisk();
    }

    // Update SYNC signal
    sync = (readShiftreg & 0x3FF) != 0x3FF || writeMode();
    if (!sync) byteReadyCounter = 0;
    
    // The lower two bits of counter UF4 are used to clock the logic board:
    //
    //                        (6) Load the write shift register
    //                         |      if the byte ready counter equals 7.
    //                         v
    //         ---- ----           ---- ----
    // QBQA:  | 00   01 | 10   11 | 00   01 | 10   11 |
    //                   ---- ----           ---- ----
    //                   ^          ^    ^    ^    ^
    //                   |          |    |    |    |
    //                   |          |    |   (2) Byte ready is always 1 here.
    //                   |         (1)  (1) Byte ready may be 0 here.
    //                   |
    //                  (3) Execute UE3 (the byte ready counter)
    //                  (4) Execute write shift register
    //                  (5) Execute read shift register
    //
    
    switch (counterUF4 & 0x03) {
            
        case 0x00:
        case 0x01:
            
            // Computation of the Byte Ready and the Load signal
            //
            //           74LS191                             ---
            //           -------               VIA2::CA2 ---|   |
            //  SYNC --o| Load  |                UF4::QB --o| & |o-- Byte Ready
            //    QB ---| Clk   |                        ---|   |
            //          |    QD |   ---                  |   ---
            //          |    QC |--|   |    ---          |   ---
            //          |    QB |--| & |o--| 1 |o-----------|   |
            //          |    QA |--|   |    ---   UF4::QB --| & |o-- load UD3
            //           -------    ---           UF4::QA --|   |
            //             UE3                               ---
            
            // (1) Update value on Byte Ready line
            updateByteReady();
            break;
            
        case 0x02:
            
            // (2)
            raiseByteReady();
            
            // (3) Execute byte ready counter
            byteReadyCounter = sync ? (byteReadyCounter + 1) & 7 : 0;
            
            // (4) Execute the write shift register
            if (writeMode() && hasDisk() && !getLightBarrier()) {
                writeBitToHead(writeShiftreg & 0x80);
                disk->setModified(true);
            }
            writeShiftreg <<= 1;
            
            // (5) Execute read shift register
            readShiftreg <<= 1;
            readShiftreg |= ((counterUF4 & 0x0C) == 0);
            break;
            
        case 0x03:
            
            // (6)
            if (byteReadyCounter == 7) {
                writeShiftreg = via2.getPA();
            }
            break;
    }
}

void
Drive::updateByteReady()
{
    //
    //           74LS191                             ---
    //           -------               VIA2::CA2 ---|   |
    //  SYNC --o| Load  |                UF4::QB --o| & |o-- Byte Ready
    //    QB ---| Clk   |                        ---|   |
    //          |    QD |   ---                  |   ---
    //          |    QC |--|   |    ---          |
    //          |    QB |--| & |o--| 1 |o---------
    //          |    QA |--|   |    ---
    //           -------    ---
    //             UE3
    
    bool ca2 = via2.getCA2();
    bool qb = counterUF4 & 0x02;
    bool ue3 = (byteReadyCounter == 7);
    bool newByteReady = !(ca2 && !qb && ue3);
    
    if (byteReady != newByteReady) {
        byteReady = newByteReady;
        via2.CA1action(byteReady);
    }
}

void
Drive::raiseByteReady()
{
    if (!byteReady) {
        byteReady = true;
        via2.CA1action(true);
    }
}

void
Drive::setZone(isize value)
{
    assert(value < 4);
    
    if (value != zone) {
        trace(DRV_DEBUG, "Switching zone: %ld --> %ld\n", zone, value);
        zone = value;
    }
}

u8
Drive::readBitFromHead() const
{
    assert(hasDisk());
    return disk->readBitFromHalftrack(halftrack, offset);
}

void
Drive::writeBitToHead(u8 bit)
{
    assert(hasDisk());
    disk->writeBitToHalftrack(halftrack, offset, bit);
}

void
Drive::rotateDisk()
{
    if (hasDisk()) {
        if (++offset >= disk->lengthOfHalftrack(halftrack)) offset = 0;
    }
}

void
Drive::setRedLED(bool b)
{
    if (redLED != b) {

        redLED = b;
        wakeUp();
        msgQueue.put(MSG_DRIVE_LED, DriveMsg { .nr = i16(objid), .value = b } );
        return;
    }
}

void
Drive::setRotating(bool b)
{
    if (spinning != b) {

        spinning = b;
        msgQueue.put(MSG_DRIVE_MOTOR, DriveMsg { .nr = i16(objid), .value = b } );
        serialPort.updateTransferStatus();
    }
}

void
Drive::wakeUp(isize awakeness)
{
    if (isIdle()) {
        
        trace(DRV_DEBUG, "Exiting power-safe mode\n");
        msgQueue.put(MSG_DRIVE_POWER_SAVE, DriveMsg { .nr = i16(objid), .value = 0 } );
        needsEmulation = true;
    }

    watchdog = awakeness;
}

void
Drive::moveHeadUp()
{
    if (halftrack < 84) {

        if (hasDisk()) {

            assert(disk->lengthOfHalftrack(halftrack) != 0);
            float pos = (float)offset / (float)disk->lengthOfHalftrack(halftrack);
            halftrack++;
            offset = (HeadPos)(pos * disk->lengthOfHalftrack(halftrack));
            assert(disk->isValidHeadPos(halftrack, offset));
            
        } else {
            
            halftrack++;
            offset = 0;
        }
        
        trace(DRV_DEBUG, "Moving head up to halftrack %ld (track %2.1f) (offset %ld)\n",
              halftrack, (halftrack + 1) / 2.0, offset);
    }

    msgQueue.put(MSG_DRIVE_STEP, DriveMsg {
        i16(objid), i16(halftrack), config.stepVolume, config.pan
    });
}

void
Drive::moveHeadDown()
{
    if (halftrack > 1) {
        
        if (hasDisk()) {

            assert(disk->lengthOfHalftrack(halftrack) != 0);
            float pos = (float)offset / (float)disk->lengthOfHalftrack(halftrack);
            halftrack--;
            offset = (HeadPos)(pos * disk->lengthOfHalftrack(halftrack));
            assert(disk->isValidHeadPos(halftrack, offset));

        } else {
            
            halftrack--;
            offset = 0;
        }
        
        trace(DRV_DEBUG, "Moving head down to halftrack %ld (track %2.1f)\n",
              halftrack, (halftrack + 1) / 2.0);
    }

    msgQueue.put(MSG_DRIVE_STEP, DriveMsg {
        i16(objid), i16(halftrack), config.stepVolume, config.pan
    });
}

bool
Drive::hasDisk() const
{
    return insertionStatus == DISK_FULLY_INSERTED;
}

bool
Drive::hasPartiallyRemovedDisk() const
{
    return
    insertionStatus == DISK_PARTIALLY_INSERTED ||
    insertionStatus == DISK_PARTIALLY_EJECTED;
}

void
Drive::setModificationFlag(bool value)
{
    if (hasDisk() && value != disk->isModified()) {

        disk->setModified(value);
        msgQueue.put(MSG_DISK_MODIFIED);
    }
}

void 
Drive::setProtection(bool value)
{
    if (hasDisk() && value != disk->isWriteProtected()) {

        disk->setWriteProtection(value);
        msgQueue.put(MSG_DISK_PROTECTED);
    }
}

void 
Drive::toggleProtection()
{
    if (hasDisk()) setProtection(!disk->isWriteProtected());
}

void
Drive::insertDisk(const fs::path &path, bool wp)
{
    insertDisk(std::make_unique<Disk>(path, wp));
}

void
Drive::insertDisk(std::unique_ptr<Disk> disk)
{
    debug(DSKCHG_DEBUG, "insertDisk\n");

    {   SUSPENDED
        
        if (!diskToInsert) {

            diskToInsert = std::move(disk);

            // Initiate the disk change procedure
            scheduleFirstDiskChangeEvent(DCH_INSERT);
        }
    }
}

void
Drive::insertNewDisk(DOSType fsType, string name)
{
    insertDisk(std::make_unique<Disk>(fsType, name));
}

void
Drive::insertFileSystem(const FileSystem &device, bool wp)
{
    insertDisk(std::make_unique<Disk>(device, wp));
}

void 
Drive::insertMediaFile(class MediaFile &file, bool wp)
{
    try {

        const D64File &d64 = dynamic_cast<const D64File &>(file);
        insertDisk(std::make_unique<Disk>(d64, wp));

    } catch (...) { try {
        
        const G64File &g64 = dynamic_cast<const G64File &>(file);
        insertDisk(std::make_unique<Disk>(g64, wp));
        
    } catch (...) { try {
        
        AnyCollection &collection = dynamic_cast<AnyCollection &>(file);
        insertDisk(std::make_unique<Disk>(collection, wp));

    } catch (...) {
        
        throw Error(VC64ERROR_FILE_TYPE_MISMATCH);
    }}}
}

void
Drive::insertD64(const D64File &d64, bool wp)
{
    insertDisk(std::make_unique<Disk>(d64, wp));
}

void
Drive::insertG64(const G64File &g64, bool wp)
{
    insertDisk(std::make_unique<Disk>(g64, wp));
}

void
Drive::insertCollection(AnyCollection &collection, bool wp)
{
    insertDisk(std::make_unique<Disk>(collection, wp));
}

void 
Drive::ejectDisk()
{
    debug(DSKCHG_DEBUG, "ejectDisk()\n");

    {   SUSPENDED
        
        if (insertionStatus == DISK_FULLY_INSERTED && !diskToInsert) {

            // Initiate the disk change procedure
            scheduleFirstDiskChangeEvent(DCH_EJECT);
        }
    }
}

void
Drive::vsyncHandler()
{
    // Only proceed if the drive is connected and switched on
    if (!config.connected || !config.switchedOn) return;

    // Check if we should enter power-safe mode
    if (!spinning && config.powerSave) {

        if (--watchdog == 0) {

            trace(DRV_DEBUG, "Entering power-save mode\n");
            needsEmulation = false;
            msgQueue.put(MSG_DRIVE_POWER_SAVE, DriveMsg { .nr = i16(objid), .value = 1 } );
        }
    }
}

void
Drive::processCommand(const Cmd &cmd)
{
    switch (cmd.type) {

        case CMD_DSK_TOGGLE_WP:     toggleProtection(); break;
        case CMD_DSK_MODIFIED:      markDiskAsModified(); break;
        case CMD_DSK_UNMODIFIED:    markDiskAsUnmodified(); break;

        default:
            fatalError;
    }
}

void
Drive::scheduleFirstDiskChangeEvent(EventID id)
{
    // Exit power-safe mode and make sure the drive stays awake for a while
    wakeUp(200); // 200 frames

    // Schedule the first event
    if (isDrive8()) c64.scheduleImm<SLOT_DC8>(id);
    if (isDrive9()) c64.scheduleImm<SLOT_DC9>(id);
}

void
Drive::processDiskChangeEvent(EventID id)
{
    auto reschedule = [&](isize delay) {

        Cycle cycles = vic.getCyclesPerFrame() * delay;

        if (isDrive8()) c64.rescheduleInc<SLOT_DC8>(cycles);
        if (isDrive9()) c64.rescheduleInc<SLOT_DC9>(cycles);
    };

    auto terminate = [&]() {

        if (isDrive8()) c64.cancel<SLOT_DC8>();
        if (isDrive9()) c64.cancel<SLOT_DC9>();
    };

    switch (insertionStatus) {

        case DISK_FULLY_INSERTED:

            trace(DSKCHG_DEBUG, "FULLY_INSERTED -> PARTIALLY_EJECTED\n");

            // Pull the disk half out (blocks the light barrier)
            insertionStatus = DISK_PARTIALLY_EJECTED;

            // Make sure the drive can no longer read from this disk
            disk->clearDisk();

            // Schedule the next transition
            reschedule(config.ejectDelay);
            return;

        case DISK_PARTIALLY_EJECTED:

            trace(DSKCHG_DEBUG, "PARTIALLY_EJECTED -> FULLY_EJECTED\n");

            // Take the disk out (unblocks the light barrier)
            insertionStatus = DISK_FULLY_EJECTED;

            // Inform the GUI
            msgQueue.put(MSG_DISK_EJECT, DriveMsg {
                i16(objid), i16(halftrack), config.stepVolume, config.pan
            });

            // Schedule the next transition
            reschedule(config.swapDelay);
            return;

        case DISK_FULLY_EJECTED:

            trace(DSKCHG_DEBUG, "FULLY_EJECTED -> PARTIALLY_INSERTED\n");

            // Only proceed if a new disk is waiting for insertion
            if (!diskToInsert) break;

            // Push the new disk half in (blocks the light barrier)
            insertionStatus = DISK_PARTIALLY_INSERTED;

            // Schedule the next transition
            reschedule(config.insertDelay);
            return;

        case DISK_PARTIALLY_INSERTED:

            trace(DSKCHG_DEBUG, "PARTIALLY_INSERTED -> FULLY_INSERTED\n");

            // Fully insert the disk (unblocks the light barrier)
            insertionStatus = DISK_FULLY_INSERTED;
            disk = std::move(diskToInsert);

            // Inform the GUI
            msgQueue.put(MSG_DISK_INSERT, DriveMsg {
                i16(objid), i16(halftrack), config.stepVolume, config.pan
            });
            break;

        default:
            fatalError;
    }

    // Terminate the disk change procedure
    terminate();
}

}
