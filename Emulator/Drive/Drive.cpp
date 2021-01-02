// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "C64.h"

Drive::Drive(DriveID id, C64 &ref) : C64Component(ref), deviceNr(id)
{
    assert(deviceNr == DRIVE8 || deviceNr == DRIVE9);
	
    subComponents = vector <HardwareComponent *> {
        
        &mem,
        &cpu,
        &via1,
        &via2,
        &disk
    };
    
    config.connected = false;
    config.switchedOn = true;
    config.type = DRIVE_VC1541II;
    
    insertionStatus = DISK_FULLY_EJECTED;
    disk.clearDisk();
}

const char *
Drive::getDescription()
{
    assert(deviceNr == DRIVE8 || deviceNr == DRIVE9);
    return deviceNr == DRIVE8 ? "Drive8" : "Drive9";
}

void
Drive::_reset()
{
    RESET_SNAPSHOT_ITEMS

    cpu.reg.pc = 0xEAA0;
    halftrack = 41;
}

long
Drive::getConfigItem(Option option)
{
    switch (option) {
            
        case OPT_DRIVE_TYPE:          return config.type;
        case OPT_DRIVE_CONNECT:       return config.connected;
        case OPT_DRIVE_POWER_SWITCH:  return config.switchedOn;
            
        default:
            assert(false);
            return 0;
    }
}

bool
Drive::setConfigItem(Option option, long value)
{
    switch (option) {
            
        case OPT_VIC_REVISION:
        {
            u64 duration = 10000000000 / VICII::getFrequency((VICRevision)value);
            
            if (durationOfOneCpuCycle == duration) {
                return false;
            }
            
            durationOfOneCpuCycle = duration;
            return true;
        }
        default:
            return false;
    }
}

bool
Drive::setConfigItem(Option option, long id, long value)
{
    if (id != deviceNr) return false;
    
    switch (option) {
            
        case OPT_DRIVE_TYPE:
        {
            if (!isDriveType(value)) {
                warn("Invalid drive type: %ld\n", value);
                return false;
            }
            if (config.type == value) {
                return false;
            }
            
            config.type = (DriveType)value;
            return true;
        }
        case OPT_DRIVE_CONNECT:
        {
            if (config.connected == value) {
                return false;
            }
            if (value && !c64.hasRom(ROM_TYPE_VC1541)) {
                warn("Can't connect drive (ROM missing).\n");
                return false;
            }
            
            suspend();
            config.connected = value;
            bool wasActive = active;
            active = config.connected && config.switchedOn;
            reset();
            resume();
            messageQueue.put(value ? MSG_DRIVE_CONNECT : MSG_DRIVE_DISCONNECT, deviceNr);
            if (wasActive != active)
                messageQueue.put(active ? MSG_DRIVE_ACTIVE : MSG_DRIVE_INACTIVE, deviceNr);
            return true;
        }
        case OPT_DRIVE_POWER_SWITCH:
        {
            if (config.switchedOn == value) {
                return false;
            }
            
            suspend();
            config.switchedOn = value;
            bool wasActive = active;
            active = config.connected && config.switchedOn;
            reset();
            resume();
            messageQueue.put(value ? MSG_DRIVE_POWER_ON : MSG_DRIVE_POWER_OFF, deviceNr);
            if (wasActive != active)
                messageQueue.put(active ? MSG_DRIVE_ACTIVE : MSG_DRIVE_INACTIVE, deviceNr);
            return true;
        }
        default:
            return false;
    }
}

void 
Drive::_dump()
{
	msg("VC1541\n");
	msg("------\n\n");
	msg(" Bit ready timer : %d\n", bitReadyTimer);
	msg("   Head position : Track %d, Bit offset %d\n", halftrack, offset);
	msg("            SYNC : %d\n", sync);
    msg("       Read mode : %s\n", readMode() ? "YES" : "NO");
	msg("\n");
    mem.dump();
}

void
Drive::_run()
{
    // Make sure the emulator has been configured properly
    assert(durationOfOneCpuCycle > 0);
}

void
Drive::execute(u64 duration)
{
    elapsedTime += duration;
    while (nextClock < (i64)elapsedTime || nextCarry < (i64)elapsedTime) {

        if (nextClock <= nextCarry) {
            
            // Execute CPU and VIAs
            u64 cycle = ++cpu.cycle;
            cpu.executeOneCycle();
            if (cycle >= via1.wakeUpCycle) via1.execute(); else via1.idleCounter++;
            if (cycle >= via2.wakeUpCycle) via2.execute(); else via2.idleCounter++;
            updateByteReady();
            if (iec.isDirtyDriveSide) iec.updateIecLinesDriveSide();

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
        if (readMode() && readBitFromHead()) {
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
            byteReadyCounter = sync ? (byteReadyCounter + 1) % 8 : 0;
            
            // (4) Execute the write shift register
            if (writeMode() && !getLightBarrier()) {
                writeBitToHead(writeShiftreg & 0x80);
                disk.setModified(true);
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
Drive::setZone(u8 value)
{
    assert(value < 4);
    
    if (value != zone) {
        trace(DRV_DEBUG, "Switching from disk zone %d to disk zone %d\n", zone, value);
        zone = value;
    }
}

void
Drive::setRedLED(bool b)
{
    if (!redLED && b) {
        redLED = true;
        c64.putMessage(MSG_DRIVE_LED_ON, deviceNr);
    } else if (redLED && !b) {
        redLED = false;
        c64.putMessage(MSG_DRIVE_LED_OFF, deviceNr);
    }
}

void
Drive::setRotating(bool b)
{
    if (!spinning && b) {
        spinning = true;
        c64.putMessage(MSG_DRIVE_MOTOR_ON, deviceNr);
    } else if (spinning && !b) {
        spinning = false;
        c64.putMessage(MSG_DRIVE_MOTOR_OFF, deviceNr);
    }
}

void
Drive::moveHeadUp()
{
    if (halftrack < 84) {

        float position = (float)offset / (float)disk.lengthOfHalftrack(halftrack);
        halftrack++;
        offset = (HeadPos)(position * disk.lengthOfHalftrack(halftrack));
        
        trace(DRV_DEBUG, "Moving head up to halftrack %d (track %2.1f) (offset %d)\n",
              halftrack, (halftrack + 1) / 2.0, offset);
        trace(DRV_DEBUG, "Halftrack %d has %d bits.\n", halftrack, disk.lengthOfHalftrack(halftrack));
    }
   
    assert(disk.isValidHeadPos(halftrack, offset));
    
    c64.putMessage(MSG_DRIVE_HEAD, deviceNr);
}

void
Drive::moveHeadDown()
{
    if (halftrack > 1) {
        float position = (float)offset / (float)disk.lengthOfHalftrack(halftrack);
        halftrack--;
        offset = (HeadPos)(position * disk.lengthOfHalftrack(halftrack));
        
        trace(DRV_DEBUG, "Moving head down to halftrack %d (track %2.1f)\n",
              halftrack, (halftrack + 1) / 2.0);
        trace(DRV_DEBUG, "Halftrack %d has %d bits.\n", halftrack, disk.lengthOfHalftrack(halftrack));
    }
    
    assert(disk.isValidHeadPos(halftrack, offset));
    
    c64.putMessage(MSG_DRIVE_HEAD, deviceNr);
}

void
Drive::setModifiedDisk(bool value)
{
    disk.setModified(value);
    c64.putMessage(value ? MSG_DISK_UNSAVED : MSG_DISK_SAVED, deviceNr);
}

void
Drive::insertDisk(Disk *otherDisk)
{
    debug(DSKCHG_DEBUG, "insertDisk(otherDisk %p)\n", otherDisk);

    suspend();
    
    if (!diskToInsert) {
        
        // Initiate the disk change procedure
        diskToInsert = otherDisk;
        diskChangeCounter = 1;
    }
    
    resume();
}

void
Drive::insertNewDisk(DOSType fsType)
{
    PETName<16> name = PETName<16>("NEW DISK");
    insertNewDisk(fsType, name);
}

void
Drive::insertNewDisk(DOSType fsType, PETName<16> name)
{
    Disk *newDisk = Disk::make(c64, fsType, name);
    insertDisk(newDisk);
}

void
Drive::insertFileSystem(FSDevice *device)
{
    debug(DSKCHG_DEBUG, "insertFileSystem(%p)\n", device);
    insertDisk(Disk::makeWithFileSystem(c64, device));
}

void
Drive::insertD64(D64File *d64)
{
    debug(DSKCHG_DEBUG, "insertD64(%p)\n", d64);
    insertDisk(Disk::makeWithD64(c64, d64));
}

void
Drive::insertG64(G64File *g64)
{
    debug(DSKCHG_DEBUG, "insertG64(%p)\n", g64);
    insertDisk(Disk::makeWithG64(c64, g64));
}

void
Drive::insertDisk(AnyCollection *collection)
{
    debug(DSKCHG_DEBUG, "insertDisk(collection %p)\n", collection);
    insertDisk(Disk::makeWithCollection(c64, collection));
}

void 
Drive::ejectDisk()
{
    debug(DSKCHG_DEBUG, "ejectDisk()\n");

    suspend();
    
    if (insertionStatus == DISK_FULLY_INSERTED && !diskToInsert) {
        
        // Initiate the disk change procedure
        diskChangeCounter = 1;
    }
    
    resume();
}

void
Drive::vsyncHandler()
{
    // Only proceed if a disk change state transition is to be performed
    if (--diskChangeCounter) return;
    
    switch (insertionStatus) {
            
        case DISK_FULLY_INSERTED:
            
            trace(DSKCHG_DEBUG, "FULLY_INSERTED -> PARTIALLY_EJECTED\n");

            // Pull the disk half out (blocks the light barrier)
            insertionStatus = DISK_PARTIALLY_EJECTED;
            
            // Make sure the drive can no longer read from this disk
            disk.clearDisk();
            
            // Schedule the next transition
            diskChangeCounter = 17;
            return;
            
        case DISK_PARTIALLY_EJECTED:
            
            trace(DSKCHG_DEBUG, "PARTIALLY_EJECTED -> FULLY_EJECTED\n");

            // Take the disk out (unblocks the light barrier)
            insertionStatus = DISK_FULLY_EJECTED;
            
            // Inform listeners
            c64.putMessage(MSG_DISK_EJECTED, deviceNr);
            
            // Schedule the next transition
            diskChangeCounter = 17;
            return;
            
        case DISK_FULLY_EJECTED:
            
            trace(DSKCHG_DEBUG, "FULLY_EJECTED -> PARTIALLY_INSERTED\n");

            // Only proceed if a new disk is waiting for insertion
            if (!diskToInsert) return;
            
            // Push the new disk half in (blocks the light barrier)
            insertionStatus = DISK_PARTIALLY_INSERTED;
            
            // Schedule the next transition
            diskChangeCounter = 17;
            return;
            
        case DISK_PARTIALLY_INSERTED:
            
            trace(DSKCHG_DEBUG, "PARTIALLY_INSERTED -> FULLY_INSERTED\n");

            // Fully insert the disk (unblocks the light barrier)
            insertionStatus = DISK_FULLY_INSERTED;

            // Copy the disk contents
            size_t size = diskToInsert->size();
            u8 *buffer = new u8[size];
            diskToInsert->save(buffer);
            disk.load(buffer);
            delete[] buffer;
            diskToInsert = NULL;

            // Inform listeners
            c64.putMessage(MSG_DISK_INSERTED, deviceNr);
            return;
    }
}
