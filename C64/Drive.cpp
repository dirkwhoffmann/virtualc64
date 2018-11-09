/*!
 * @file        Drive.cpp
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   Dirk W. Hoffmann. All rights reserved.
 */
/*
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

#include "C64.h"

VC1541::VC1541(unsigned nr)
{
    assert(nr == 1 || nr == 2);
    
    deviceNr = nr;
    setDescription(deviceNr == 1 ? "Drive1" : "Drive2");
    debug(3, "Creating %s at address %p\n", getDescription());
	
	// Configure CPU
	cpu.setDescription(deviceNr == 1 ? "Drive1CPU" : "Drive2CPU");
    cpu.chipModel = MOS_6502;
    
    // Register sub components
    VirtualComponent *subcomponents[] = { &mem, &cpu, &via1, &via2, &disk, NULL };
    registerSubComponents(subcomponents, sizeof(subcomponents));
    
    // Setup references
    cpu.mem = &mem;
    mem.drive = this;
    via1.drive = this;
    via2.drive = this;
    
    // Register snapshot items
    SnapshotItem items[] = {

        // Life-time items
        { &sendSoundMessages,       sizeof(sendSoundMessages),      KEEP_ON_RESET },
        { &durationOfOneCpuCycle,   sizeof(durationOfOneCpuCycle),  KEEP_ON_RESET },
        { &poweredOn,               sizeof(poweredOn),              KEEP_ON_RESET },

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
    sendSoundMessages = true;
    resetDisk();
}

VC1541::~VC1541()
{
	debug(3, "Releasing VC1541...\n");
}

void
VC1541::reset()
{
    VirtualComponent::reset();
    
    cpu.PC = 0xEAA0;
    halftrack = 41;
}

void
VC1541::resetDisk()
{
    debug (3, "Resetting disk in VC1541...\n");
    
    disk.clearDisk();
}

void
VC1541::ping()
{
    VirtualComponent::ping();
    
    c64->putMessage(poweredOn ? MSG_VC1541_ATTACHED : MSG_VC1541_DETACHED, deviceNr);
    c64->putMessage(redLED ? MSG_VC1541_RED_LED_ON : MSG_VC1541_RED_LED_OFF, deviceNr);
    c64->putMessage(spinning ? MSG_VC1541_MOTOR_ON : MSG_VC1541_MOTOR_OFF, deviceNr);
    c64->putMessage(hasDisk() ? MSG_VC1541_DISK : MSG_VC1541_NO_DISK, deviceNr);
    c64->putMessage(hasModifiedDisk() ? MSG_DISK_UNSAVED : MSG_DISK_SAVED, deviceNr);

}

void
VC1541::setClockFrequency(uint32_t frequency)
{
    durationOfOneCpuCycle = 10000000000 / frequency;
    debug("Duration a CPU cycle is %lld 1/10 nsec.\n", durationOfOneCpuCycle);
}

void 
VC1541::dump()
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
VC1541::powerUp()
{
    suspend();
    reset();
    resume();
}

bool
VC1541::execute(uint64_t duration)
{
    uint8_t result = true;
    
    elapsedTime += duration;
    while (nextClock < elapsedTime || nextCarry < elapsedTime) {

        if (nextClock <= nextCarry) {
            
            // Execute CPU and VIAs
            uint64_t cycle = ++cpu.cycle;
            result = cpu.executeOneCycle();
            if (cycle >= via1.wakeUpCycle) via1.execute(); else via1.idleCounter++;
            if (cycle >= via2.wakeUpCycle) via2.execute(); else via2.idleCounter++;
            updateByteReady();
            if (c64->iec.isDirtyDriveSide) c64->iec.updateIecLinesDriveSide();

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

/*
bool
VC1541::execute(uint64_t duration)
{
    uint8_t result = true;
    
    elapsedTime += duration;
    
    if (nextCarry < elapsedTime && nextCarry < nextClock) {
        // Execute read/write logic
        if (spinning) executeUF4();
        nextCarry += delayBetweenTwoCarryPulses[zone];
    }
    
    if (nextClock < elapsedTime) {
        // Execute CPU and VIAs
        uint64_t cycle = ++cpu.cycle;
        if (cycle >= via1.wakeUpCycle) via1.execute(); else via1.idleCounter++;
        if (cycle >= via2.wakeUpCycle) via2.execute(); else via2.idleCounter++;
        result = cpu.executeOneCycle();
        nextClock += 10000;
    }
    
    if (nextCarry < elapsedTime) {
        // Execute read/write logic
        if (spinning) executeUF4();
        nextCarry += delayBetweenTwoCarryPulses[zone];
    }
    
    assert(nextClock >= elapsedTime && nextCarry >= elapsedTime);
    return result;
}
*/

void
VC1541::executeUF4()
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

bool
VC1541::computeByteReady()
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
    // debug("%d %d (%d) %d (%d)\n", ca2, qb, counterUF4, ue3, byteReadyCounter);
    return !(ca2 && !qb && ue3);
}

void
VC1541::updateByteReady()
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
VC1541::raiseByteReady()
{
    if (!byteReady) {
        byteReady = true;
        via2.CA1action(true);
    }
}

/*
void
VC1541::setByteReadyLine(bool value)
{
    if (byteReady != value) {
        byteReady = value;
        via2.CA1action(value);
    }
}
*/

void
VC1541::setZone(uint2_t value)
{
    assert(is_uint2_t(value));
    
    if (value != zone) {
        debug(2, "Switching from disk zone %d to disk zone %d\n", zone, value);
        zone = value;
    }
}

void
VC1541::powerOn()
{
    if (poweredOn) return;
    
    suspend();
    
    poweredOn = true;
    if (soundMessagesEnabled())
        c64->putMessage(MSG_VC1541_ATTACHED_SOUND, deviceNr);
    ping();
    
    resume();
}

void
VC1541::powerOff()
{
    if (!poweredOn) return;

    suspend();
    
    reset();
    
    poweredOn = false;
    if (soundMessagesEnabled())
        c64->putMessage(MSG_VC1541_DETACHED_SOUND, deviceNr);
    ping();
    
    resume();
}

void
VC1541::setRedLED(bool b)
{
    if (!redLED && b) {
        redLED = true;
        c64->putMessage(MSG_VC1541_RED_LED_ON, deviceNr);
    } else if (redLED && !b) {
        redLED = false;
        c64->putMessage(MSG_VC1541_RED_LED_OFF, deviceNr);
    }
}

void
VC1541::setRotating(bool b)
{
    if (!spinning && b) {
        spinning = true;
        c64->putMessage(MSG_VC1541_MOTOR_ON, deviceNr);
    } else if (spinning && !b) {
        spinning = false;
        c64->putMessage(MSG_VC1541_MOTOR_OFF, deviceNr);
    }
}

void
VC1541::moveHeadUp()
{
    if (halftrack < 84) {

        float position = (float)offset / (float)disk.lengthOfHalftrack(halftrack);
        halftrack++;
        offset = position * disk.lengthOfHalftrack(halftrack);
        
        debug(2, "Moving head up to halftrack %d (track %2.1f) (offset %d)\n",
              halftrack, (halftrack + 1) / 2.0, offset);
        debug(2, "Halftrack %d has %d bits.\n", halftrack, disk.lengthOfHalftrack(halftrack));
    }
   
    assert(disk.isValidHeadPositon(halftrack, offset));
    
    c64->putMessage(MSG_VC1541_HEAD_UP, deviceNr);
    if (halftrack % 2 && sendSoundMessages) {
        // Play sound for full tracks, only
        c64->putMessage(MSG_VC1541_HEAD_UP_SOUND, deviceNr);
    }
}

void
VC1541::moveHeadDown()
{
    if (halftrack > 1) {
        float position = (float)offset / (float)disk.lengthOfHalftrack(halftrack);
        halftrack--;
        offset = position * disk.lengthOfHalftrack(halftrack);
        
        debug(2, "Moving head down to halftrack %d (track %2.1f)\n",
              halftrack, (halftrack + 1) / 2.0);
        debug(2, "Halftrack %d has %d bits.\n", halftrack, disk.lengthOfHalftrack(halftrack));
    }
    
    assert(disk.isValidHeadPositon(halftrack, offset));
    
    c64->putMessage(MSG_VC1541_HEAD_DOWN, deviceNr);
    if (halftrack % 2 && sendSoundMessages)
        // Play sound for full tracks, only
        c64->putMessage(MSG_VC1541_HEAD_DOWN_SOUND, deviceNr);
}

void
VC1541::setModifiedDisk(bool value)
{
    disk.setModified(value);
    c64->putMessage(value ? MSG_DISK_UNSAVED : MSG_DISK_SAVED, deviceNr);
}

void
VC1541::prepareToInsert()
{
    c64->resume();
    
    debug("prepareToInsert\n");
    assert(insertionStatus == NOT_INSERTED);
    
    // Block the light barrier by taking the disk half out
    insertionStatus = PARTIALLY_INSERTED;
    
    c64->resume();
}

void
VC1541::insertDisk(AnyArchive *a)
{
    suspend();

    debug("insertDisk\n");
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
    
    c64->putMessage(MSG_VC1541_DISK, deviceNr);
    c64->putMessage(MSG_DISK_SAVED, deviceNr);
    if (sendSoundMessages)
        c64->putMessage(MSG_VC1541_DISK_SOUND, deviceNr);
    
    resume();
}

void
VC1541::prepareToEject()
{
    suspend();
    
    debug("prepareToEject\n");
    assert(insertionStatus == FULLY_INSERTED);
    
    // Block the light barrier by taking the disk half out
    insertionStatus = PARTIALLY_INSERTED;
    
    // Make sure the drive can no longer read from this disk
    disk.clearDisk();
    
    resume();
}

void 
VC1541::ejectDisk()
{
    suspend();
 
    debug("ejectDisk\n");
    assert(insertionStatus == PARTIALLY_INSERTED);
    
    // Unblock the light barrier by taking the disk out
    insertionStatus = NOT_INSERTED;
    
    // Notify listener
    c64->putMessage(MSG_VC1541_NO_DISK, deviceNr);
    if (sendSoundMessages)
        c64->putMessage(MSG_VC1541_NO_DISK_SOUND, deviceNr);
    
    resume();
}

