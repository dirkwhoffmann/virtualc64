/*!
 * @file        VC1541.h
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   2006 - 2018 Dirk W. Hoffmann
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

#include "C64.h"

VC1541::VC1541()
{
	setDescription("1541");
    debug(3, "Creating virtual VC1541 at address %p\n", this);
	
	// Configure CPU
	cpu.setDescription("1541CPU");
    cpu.chipModel = MOS_6502;
    
    // Register sub components
    VirtualComponent *subcomponents[] = { &mem, &cpu, &via1, &via2, &disk, NULL };
    registerSubComponents(subcomponents, sizeof(subcomponents)); 

    // Register snapshot items
    SnapshotItem items[] = {

        // Configuration items
        { &sendSoundMessages,       sizeof(sendSoundMessages),      KEEP_ON_RESET },
        { &durationOfOneCpuCycle,   sizeof(durationOfOneCpuCycle),  KEEP_ON_RESET },

        // Internal state
        { &elapsedTime,             sizeof(elapsedTime),            CLEAR_ON_RESET },
        { &nextClock,               sizeof(nextClock),              CLEAR_ON_RESET },
        { &nextCarry,               sizeof(nextCarry),              CLEAR_ON_RESET },
        { &counterUF4,              sizeof(counterUF4),             CLEAR_ON_RESET },
        { &bitReadyTimer,           sizeof(bitReadyTimer),          CLEAR_ON_RESET },
        { &byteReadyCounter,        sizeof(byteReadyCounter),       CLEAR_ON_RESET },
        { &spinning,                sizeof(spinning),               CLEAR_ON_RESET },
        { &redLED,                  sizeof(redLED),                 CLEAR_ON_RESET },
        { &diskPartiallyInserted,   sizeof(diskPartiallyInserted),  CLEAR_ON_RESET },
        { &halftrack,               sizeof(halftrack),              CLEAR_ON_RESET },
        { &offset,                  sizeof(offset),                 CLEAR_ON_RESET },
        { &zone,                    sizeof(zone),                   CLEAR_ON_RESET },
        { &readShiftreg,            sizeof(readShiftreg),           CLEAR_ON_RESET },
        { &writeShiftreg,           sizeof(writeShiftreg),          CLEAR_ON_RESET },
        { &sync,                    sizeof(sync),                   CLEAR_ON_RESET },
        { &byteReady,               sizeof(byteReady),              CLEAR_ON_RESET },

        // Disk properties (will survive reset)
        { &diskInserted,            sizeof(diskInserted),           KEEP_ON_RESET },
        { NULL,                     0,                              0 }};
    
    registerSnapshotItems(items, sizeof(items));
    
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
    
    cpu.setPC(0xEAA0);
    halftrack = 41;
        
    // setDebugLevel(2);
    // via2.setDebugLevel(2);
}

void
VC1541::resetDisk()
{
    debug (3, "Resetting disk in VC1541...\n");
    
    // Disk properties
    disk.clearDisk();
    diskInserted = false;
    diskPartiallyInserted = false;
}

void
VC1541::ping()
{
    VirtualComponent::ping();
    c64->putMessage(redLED ? MSG_VC1541_RED_LED_ON : MSG_VC1541_RED_LED_OFF);
    c64->putMessage(spinning ? MSG_VC1541_MOTOR_ON : MSG_VC1541_MOTOR_OFF);
    c64->putMessage(diskInserted ? MSG_VC1541_DISK : MSG_VC1541_NO_DISK);
}

void
VC1541::setClockFrequency(uint32_t frequency)
{
    durationOfOneCpuCycle = 1000000000000 / frequency;
    debug("Duration a CPU cycle is %lld pico seconds.\n", durationOfOneCpuCycle);
}

void 
VC1541::dumpState()
{
	msg("VC1541\n");
	msg("------\n\n");
	msg(" Bit ready timer : %d\n", bitReadyTimer);
	msg("   Head position : Track %d, Bit offset %d\n", halftrack, offset);
	msg("            SYNC : %d\n", sync);
    msg("       Read mode : %s\n", readMode() ? "YES" : "NO");
	msg("\n");
    mem.dumpState();
    startTracing();
}

void
VC1541::powerUp()
{
    c64->suspend();
    reset();
    c64->resume();
}

/*
bool
VC1541::execute(uint64_t duration)
{
    uint8_t result = true;
    
    elapsedTime += duration;
    while (nextClock < elapsedTime || nextCarry < elapsedTime) {

        if (nextClock <= nextCarry) {
            
            // Execute CPU and VIAs
            uint64_t cycle = ++cpu.cycle;
            if (cycle >= via1.wakeUpCycle) via1.execute(); else via1.idleCounter++;
            if (cycle >= via2.wakeUpCycle) via2.execute(); else via2.idleCounter++;
            result = cpu.executeOneCycle();
            nextClock += 1000000;

        } else {
            
            // Execute read/write logic
            if (spinning) executeUF4();
            nextCarry += delayBetweenTwoCarryPulses[zone];
        }
    }
    assert(nextClock >= elapsedTime && nextCarry >= elapsedTime);
    
    return result;
}
*/

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
        nextClock += 1000000;
    }
    
    if (nextCarry < elapsedTime) {
        // Execute read/write logic
        if (spinning) executeUF4();
        nextCarry += delayBetweenTwoCarryPulses[zone];
    }
    
    assert(nextClock >= elapsedTime && nextCarry >= elapsedTime);
    return result;
}

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
            // case 0x01:
            
            // Computation of the Byte Ready and the Load signal
            //
            //           74LS191                             ---
            //           -------               VIA2::CA2 --o|   |
            //  SYNC --o| Load  |                UF4::QB --o| & |o-- Byte Ready
            //    QB ---| Clk   |                        ---|   |
            //          |    QD |   ---                  |   ---
            //          |    QC |--|   |    ---          |   ---
            //          |    QB |--| & |o--| 1 |o-----------|   |
            //          |    QA |--|   |    ---   UF4::QB --| & |o-- load UD3
            //           -------    ---           UF4::QA --|   |
            //             UE3                               ---
            
            // (1) Update value on Byte Ready line
            if (byteReadyCounter == 7 && via2.getCA2())
                clearByteReadyLine();
            break;
            
        case 0x01:
            break;
            
        case 0x02:
            
            // (2)
            raiseByteReadyLine();
            
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
VC1541::setByteReadyLine(bool value)
{
    if (byteReady != value) {
        byteReady = value;
        via2.CA1action(value);
    }
}

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
VC1541::setRedLED(bool b)
{
    if (!redLED && b) {
        redLED = true;
        c64->putMessage(MSG_VC1541_RED_LED_ON);
    } else if (redLED && !b) {
        redLED = false;
        c64->putMessage(MSG_VC1541_RED_LED_OFF);
    }
}

void
VC1541::setRotating(bool b)
{
    if (!spinning && b) {
        spinning = true;
        c64->putMessage(MSG_VC1541_MOTOR_ON);
    } else if (spinning && !b) {
        spinning = false;
        c64->putMessage(MSG_VC1541_MOTOR_OFF);
    }
}

void
VC1541::moveHeadUp()
{
    if (halftrack < 84) {

        float position = (float)offset / (float)disk.lengthOfHalftrack(halftrack);
        halftrack++;
        offset = position * disk.lengthOfHalftrack(halftrack);
        
        debug(2, "Moving head up to halftrack %d (track %2.1f)\n",
              halftrack, (halftrack + 1) / 2.0);
        debug(2, "Halftrack %d has %d bits.\n", halftrack, disk.lengthOfHalftrack(halftrack));
    }
   
    assert(disk.isValidHeadPositon(halftrack, offset));
    
    c64->putMessage(MSG_VC1541_HEAD_UP);
    if (halftrack % 2 && sendSoundMessages)
        c64->putMessage(MSG_VC1541_HEAD_UP_SOUND); // play sound for full tracks, only
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
    
    c64->putMessage(MSG_VC1541_HEAD_DOWN);
    if (halftrack % 2 && sendSoundMessages)
        c64->putMessage(MSG_VC1541_HEAD_DOWN_SOUND); // play sound for full tracks, only
}

bool
VC1541::insertDisk(Archive *a)
{
    assert(a != NULL);

    D64Archive *converted;
    
    switch (a->type()) {
            
        case D64_CONTAINER:
            ejectDisk();
            disk.encodeArchive((D64Archive *)a);
            break;
            
        case G64_CONTAINER:
            ejectDisk();
            disk.encodeArchive((G64Archive *)a);
            break;
            
        case NIB_CONTAINER:
            ejectDisk();
            disk.encodeArchive((NIBArchive *)a);
            break;
            
        default:
            
            // All other archives cannot be encoded directly. We convert them to D64 first.
            if (!(converted = D64Archive::makeD64ArchiveWithAnyArchive(a)))
                return false;

            ejectDisk();
            disk.encodeArchive(converted);
            break;
    }
    
    diskInserted = true;
    c64->putMessage(MSG_VC1541_DISK);
    if (sendSoundMessages)
        c64->putMessage(MSG_VC1541_DISK_SOUND);
    
    return true; 
}

void 
VC1541::ejectDisk()
{
    if (!hasDisk())
        return;
    
	// Open lid (this blocks the light barrier)
    setDiskPartiallyInserted(true);

	// Let the drive notice the blocked light barrier in its interrupt routine ...
	sleepMicrosec((uint64_t)200000);

    // Erase disk data and reset write protection flag
    resetDisk();

	// Remove disk (this unblocks the light barrier)
	setDiskPartiallyInserted(false);
		
    // Notify listener
	c64->putMessage(MSG_VC1541_NO_DISK);
    if (sendSoundMessages)
        c64->putMessage(MSG_VC1541_NO_DISK_SOUND);
}

D64Archive *
VC1541::convertToD64()
{
    int error;
    
    D64Archive *archive = new D64Archive();
    debug(1, "Creating D64 archive from currently inserted diskette ...\n");
    
    // Determine D64 format (35, 40, or 42 track format)
    Track t = 42;
    while (t > 0 && disk.trackIsEmpty(t)) t--;
    unsigned numTracks = (t <= 35) ? 35 : (t <= 40) ? 40 : 42;

    // Perform test run
    size_t numBytes = disk.decodeDisk(NULL, numTracks, &error);
    if (error) {
        archive->warn("Decoder failure (error code: %d)\n", error);
        delete archive;
        return NULL;
    }
    if ((t == 35 && numBytes != D64_683_SECTORS) ||
        (t == 40 && numBytes != D64_768_SECTORS) ||
        (t == 42 && numBytes != D64_802_SECTORS)) {
        archive->warn("Decoder failure (wrong byte count: %d)\n", numBytes);
        delete archive;
        return NULL;
    }

    // Decode disk
    archive->setNumberOfTracks(numTracks);
    disk.decodeDisk(archive->getData(), numTracks);
    
    archive->debug(2, "Archive has %d files\n", archive->getNumberOfItems());
    archive->debug(2, "Item %d has size: %d\n", 0, archive->getSizeOfItem(0));
    
    return archive;
}

bool
VC1541::exportToD64(const char *filename)
{
    assert(filename != NULL);

    D64Archive *archive = convertToD64();
    
    if (archive == NULL)
        return false;
    
    archive->writeToFile(filename);
    delete archive;
    return true;
}


