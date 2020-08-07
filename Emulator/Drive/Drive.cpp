// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "C64.h"

Drive::Drive(DriveID id, C64 &ref) : C64Component(ref)
{
    assert(id == DRIVE8 || id == DRIVE9);
    
    deviceNr = id;
    setDescription(id == DRIVE8 ? "Drive8" : "Drive9");
    cpu.setDescription(id == DRIVE8 ? "Drive8CPU" : "Drive9CPU");
	
    subComponents = vector <HardwareComponent *> {
        
        &mem,
        &cpu,
        &via1,
        &via2,
        &disk
    };
     
    // Register snapshot items
    SnapshotItem items[] = {

        // Life-time items
        { &durationOfOneCpuCycle,   sizeof(durationOfOneCpuCycle),  KEEP_ON_RESET },
        { &config.type,             sizeof(config.type),            KEEP_ON_RESET },
        { &config.connected,        sizeof(config.connected),       KEEP_ON_RESET },

        // Internal state
        { &spinning,                sizeof(spinning),               CLEAR_ON_RESET },
        { &redLED,                  sizeof(redLED),                 CLEAR_ON_RESET },
        { &elapsedTime,             sizeof(elapsedTime),            CLEAR_ON_RESET },
        { &nextClock,               sizeof(nextClock),              CLEAR_ON_RESET },
        { &nextCarry,               sizeof(nextCarry),              CLEAR_ON_RESET },
        { &carryCounter,            sizeof(carryCounter),           CLEAR_ON_RESET },
        { &counterUF4,              sizeof(counterUF4),             CLEAR_ON_RESET },
        { &bitReadyTimer,           sizeof(bitReadyTimer),          CLEAR_ON_RESET },
        { &byteReadyCounter,        sizeof(byteReadyCounter),       CLEAR_ON_RESET },
        { &halftrack,               sizeof(halftrack),              CLEAR_ON_RESET },
        { &offset,                  sizeof(offset),                 CLEAR_ON_RESET },
        { &zone,                    sizeof(zone),                   CLEAR_ON_RESET },
        { &readShiftreg,            sizeof(readShiftreg),           CLEAR_ON_RESET },
        { &writeShiftreg,           sizeof(writeShiftreg),          CLEAR_ON_RESET },
        { &sync,                    sizeof(sync),                   CLEAR_ON_RESET },
        { &byteReady,               sizeof(byteReady),              CLEAR_ON_RESET },

        // Disk properties (will survive reset)
        { &insertionStatus,         sizeof(insertionStatus),        KEEP_ON_RESET },
        { NULL,                     0,                              0 }};
    
    registerSnapshotItems(items, sizeof(items));
    
    insertionStatus = NOT_INSERTED;
    resetDisk();
}

bool
Drive::isConnectable()
{
    return c64.hasVC1541Rom();
}

void
Drive::setConnected(bool value)
{
    // Only proceed if the connection status will change
    if (config.connected == value) return;
        
    // Only drives with a Rom can be connected
    if (value && !isConnectable()) return;
    
    suspend();

    config.connected = value;
    _reset();
    ping();

    resume();
}

void
Drive::_initialize()
{
    debug("VC1541 initialize");

    // Start with a single drive powered on
    deviceNr == DRIVE8 ? connect() : disconnect();
}

void
Drive::_reset()
{
    // Clear snapshot items marked with 'CLEAR_ON_RESET'
     if (snapshotItems != NULL)
         for (unsigned i = 0; snapshotItems[i].data != NULL; i++)
             if (snapshotItems[i].flags & CLEAR_ON_RESET)
                 memset(snapshotItems[i].data, 0, snapshotItems[i].size);

    cpu.regPC = 0xEAA0;
    halftrack = 41;
}

void
Drive::resetDisk()
{    
    disk.clearDisk();
}

void
Drive::_ping()
{    
    c64.putMessage(config.connected ? MSG_DRIVE_CONNECT : MSG_DRIVE_DISCONNECT, deviceNr);
    c64.putMessage(redLED ? MSG_DRIVE_LED_ON : MSG_DRIVE_LED_OFF, deviceNr);
    c64.putMessage(spinning ? MSG_DRIVE_MOTOR_ON : MSG_DRIVE_MOTOR_OFF, deviceNr);
    c64.putMessage(hasDisk() ? MSG_DRIVE_DISK : MSG_DRIVE_NO_DISK, deviceNr);
    c64.putMessage(hasModifiedDisk() ? MSG_DISK_UNSAVED : MSG_DISK_SAVED, deviceNr);

}

void
Drive::_setClockFrequency(u32 value)
{
    durationOfOneCpuCycle = 10000000000 / value;
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
    startTracing();
}

void
Drive::powerUp()
{
    suspend();
    _reset();
    resume();
}

bool
Drive::execute(u64 duration)
{
    u8 result = true;
    
    elapsedTime += duration;
    while (nextClock < elapsedTime || nextCarry < elapsedTime) {

        if (nextClock <= nextCarry) {
            
            // Execute CPU and VIAs
            u64 cycle = ++cpu.cycle;
            result = cpu.executeOneCycle();
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
    assert(nextClock >= elapsedTime && nextCarry >= elapsedTime);
    
    return result;
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
Drive::setZone(uint2_t value)
{
    assert(is_uint2_t(value));
    
    if (value != zone) {
        debug(DRV_DEBUG, "Switching from disk zone %d to disk zone %d\n", zone, value);
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
        offset = (HeadPosition)(position * disk.lengthOfHalftrack(halftrack));
        
        debug(DRV_DEBUG, "Moving head up to halftrack %d (track %2.1f) (offset %d)\n",
              halftrack, (halftrack + 1) / 2.0, offset);
        debug(DRV_DEBUG, "Halftrack %d has %d bits.\n", halftrack, disk.lengthOfHalftrack(halftrack));
    }
   
    assert(disk.isValidHeadPositon(halftrack, offset));
    
    c64.putMessage(MSG_DRIVE_HEAD, deviceNr);
}

void
Drive::moveHeadDown()
{
    if (halftrack > 1) {
        float position = (float)offset / (float)disk.lengthOfHalftrack(halftrack);
        halftrack--;
        offset = (HeadPosition)(position * disk.lengthOfHalftrack(halftrack));
        
        debug(DRV_DEBUG, "Moving head down to halftrack %d (track %2.1f)\n",
              halftrack, (halftrack + 1) / 2.0);
        debug(DRV_DEBUG, "Halftrack %d has %d bits.\n", halftrack, disk.lengthOfHalftrack(halftrack));
    }
    
    assert(disk.isValidHeadPositon(halftrack, offset));
    
    c64.putMessage(MSG_DRIVE_HEAD, deviceNr);
}

void
Drive::setModifiedDisk(bool value)
{
    disk.setModified(value);
    c64.putMessage(value ? MSG_DISK_UNSAVED : MSG_DISK_SAVED, deviceNr);
}

void
Drive::prepareToInsert()
{
    suspend();
    
    debug(DRV_DEBUG, "prepareToInsert\n");
    assert(insertionStatus == NOT_INSERTED);
    
    // Block the light barrier by taking the disk half out
    insertionStatus = PARTIALLY_INSERTED;
    
    resume();
}

void
Drive::insertDisk(AnyArchive *a)
{
    suspend();

    debug(DRV_DEBUG, "insertDisk\n");
    assert(a != NULL);
    assert(insertionStatus == PARTIALLY_INSERTED);
    
    switch (a->type()) {
            
        case D64_FILE:
            disk.clearDisk();
            disk.encodeArchive((D64File *)a);
            break;
            
        case G64_FILE:
            disk.clearDisk();
            disk.encodeArchive((G64File *)a);
            break;
            
        default: {

            // All other archives cannot be encoded directly.
            // We convert them to a D64 archive first.
            
            D64File *converted = D64File::makeWithAnyArchive(a);
            disk.clearDisk();
            disk.encodeArchive(converted);
            break;
        }
    }
    
    insertionStatus = FULLY_INSERTED;
    
    c64.putMessage(MSG_DRIVE_DISK, deviceNr);
    c64.putMessage(MSG_DISK_SAVED, deviceNr);
    
    resume();
}

void
Drive::prepareToEject()
{
    suspend();
    
    debug(DRV_DEBUG, "prepareToEject\n");
    assert(insertionStatus == FULLY_INSERTED);
    
    // Block the light barrier by taking the disk half out
    insertionStatus = PARTIALLY_INSERTED;
    
    // Make sure the drive can no longer read from this disk
    disk.clearDisk();
    
    resume();
}

void 
Drive::ejectDisk()
{
    suspend();
 
    debug(DRV_DEBUG, "ejectDisk\n");
    assert(insertionStatus == PARTIALLY_INSERTED);
    
    // Unblock the light barrier by taking the disk out
    insertionStatus = NOT_INSERTED;
    
    // Notify listener
    c64.putMessage(MSG_DRIVE_NO_DISK, deviceNr);
    
    resume();
}

