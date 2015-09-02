/*
 * Written 2006 - 2015 by Dirk W. Hoffmann
 *
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

VC1541::VC1541()
{
	name = "1541";
    debug(2, "Creating virtual VC1541 at address %p\n", this);
	
	// Create sub components
	mem = new VC1541Memory();
	cpu = new CPU();
	cpu->setName("1541CPU");
    cpu->chipModel = CPU::MOS6502;
    
    // Register snapshot items
    SnapshotItem items[] = {
        
        // Configuration
        { &bitAccuracy, sizeof(bitAccuracy) },
        
        // Internal state
        { &bitReadyTimer, sizeof(bitAccuracy) },
        { &byteReadyCounter, sizeof(byteReadyCounter) },
        { &rotating, sizeof(rotating) },
        { &redLED, sizeof(redLED) },
        { &diskInserted, sizeof(diskInserted) },
        { &writeProtected, sizeof(writeProtected) },
        { &sendSoundMessages, sizeof(sendSoundMessages) },
        
        // Read/Write logic
        { &halftrack, sizeof(halftrack) },
        { &bitoffset, sizeof(bitoffset) },
        { &zone, sizeof(zone) },
        { &read_shiftreg, sizeof(read_shiftreg) },
        { &write_shiftreg, sizeof(write_shiftreg) },
        { &sync, sizeof(sync) },
        { NULL, 0 }
    };
    
    registerSnapshotItems(items, sizeof(items));
    
    sendSoundMessages = true; 
    resetDisk();
}

VC1541::~VC1541()
{
	debug(2, "Releasing VC1541...\n");
	
	delete cpu;	
	delete mem;
}

void
VC1541::resetDrive(C64 *c64)
{
    VirtualComponent::reset(c64);
    
    // Establish bindings
    iec = c64->iec;
    
    // Reset subcomponents
    mem->reset(c64);
    cpu->reset(c64, mem);
    cpu->setPC(0xEAA0);
    via1.reset(c64);
    via2.reset(c64);
    
    // VC1541 properties
    rotating = false;
    redLED = false;
    bitReadyTimer = 0;
    byteReadyCounter = 0;
    halftrack = 41;
    bitoffset = 0;
    zone = 0;
    read_shiftreg = 0;
    write_shiftreg = 0;
    sync = false;
}

void
VC1541::resetDisk()
{
    debug (2, "Resetting disk in VC1541...\n");
    
    // Disk properties
    disk.reset(c64);
    diskInserted = false;
    writeProtected = false;
}

void
VC1541::ping()
{
    debug(2, "Pinging VC1541...\n");
    c64->putMessage(MSG_VC1541_LED, redLED ? 1 : 0);
    c64->putMessage(MSG_VC1541_MOTOR, rotating ? 1 : 0);
    c64->putMessage(MSG_VC1541_DISK, diskInserted ? 1 : 0);

    cpu->ping();
    mem->ping();
    via1.ping();
    via2.ping();

}

uint32_t
VC1541::stateSize()
{
    uint32_t result = VirtualComponent::stateSize();
    
    result += disk.stateSize();
    result += cpu->stateSize();
    result += via1.stateSize();
    result += via2.stateSize();
    result += mem->stateSize();
    
    return result;
}

void
VC1541::loadFromBuffer(uint8_t **buffer)
{	
    uint8_t *old = *buffer;
    
    VirtualComponent::loadFromBuffer(buffer);
    disk.loadFromBuffer(buffer);
	cpu->loadFromBuffer(buffer);
    via1.loadFromBuffer(buffer);
    via2.loadFromBuffer(buffer);
    mem->loadFromBuffer(buffer);
    
    assert(*buffer - old == stateSize());
}

void 
VC1541::saveToBuffer(uint8_t **buffer)
{	
    uint8_t *old = *buffer;
    
    VirtualComponent::saveToBuffer(buffer);
    disk.saveToBuffer(buffer);
    cpu->saveToBuffer(buffer);
    via1.saveToBuffer(buffer);
    via2.saveToBuffer(buffer);
	mem->saveToBuffer(buffer);
    
    assert(*buffer - old == stateSize());
}

void 
VC1541::dumpState()
{
	msg("VC1541\n");
	msg("------\n\n");
	msg(" Bit ready timer : %d\n", bitReadyTimer);
	msg("   Head position : Track %d, Bit offset %d\n", halftrack, bitoffset);
	msg("            SYNC : %d\n", sync);
    msg("       Read mode : %s\n", readMode() ? "YES" : "NO");
	msg("\n");
    disk.dumpState();
}

void
VC1541::executeBitReady()
{
    read_shiftreg <<= 1;

    if (readMode()) {
        
        // Read mode
        read_shiftreg |= readBitFromHead();

        // Set SYNC signal
        if ((read_shiftreg & 0x3FF) == 0x3FF) {

            sync = true;
            
        } else {

            if (sync)
                byteReadyCounter = 0; // Cleared on falling edge of SYNC
            sync = false;
        }
        
    } else {
        
        // Write mode
        writeBitToHead(write_shiftreg & 0x80);
        sync = false;
    }
    write_shiftreg <<= 1;
    
    rotateDisk();
    
    // Perform action if byte is complete
    if (byteReadyCounter++ == 7) {
        executeByteReady();
        byteReadyCounter = 0;
    }
    
    bitReadyTimer += cyclesPerBit[zone];
}

void
VC1541::executeByteReady()
{
    assert(bitoffset % 8 == 0);
    
    if (readMode() && !sync) {
        byteReady(read_shiftreg);
    }
    if (writeMode()) {
        write_shiftreg = via2.ora;
        byteReady();
    }
}

inline void
VC1541::byteReady(uint8_t byte)
{
    // On the VC1541 logic board, the byte ready signal is computed by a NAND gate with three inputs.
    // Two of them are clock lines ensuring that a signal is generated every eigths bit.
    // The third signal is hard-wired to pin CA2 of VIA2. By pulling CA2 low, the CPU can silence the
    // the byte ready line. E.g., this is done when moving the drive head to a different track
    if (via2.CA2()) {
        via2.ira = byte;
        byteReady();
    }
}

inline void
VC1541::byteReady()
{
    if (via2.overflowEnabled()) cpu->setV(1);
}


void 
VC1541::simulateAtnInterrupt()
{
	if (via1.atnInterruptsEnabled()) {
		via1.indicateAtnInterrupt();
		cpu->setIRQLineATN();
		// debug("CPU is interrupted by ATN line.\n");
	} else {
		// debug("Sorry, want to interrupt, but CPU does not accept ATN line interrupts\n");
	}
}

void
VC1541::setZone(uint8_t z)
{
    assert (z <= 3);
    
    if (z != zone) {
        debug(3, "Switching from disk zone %d to disk zone %d\n", zone, z);
        zone = z;
    }
}

void
VC1541::setRedLED(bool b)
{
    if (!redLED && b) {
        redLED = true;
        c64->putMessage(MSG_VC1541_LED, 1);
    } else if (redLED && !b) {
        redLED = false;
        c64->putMessage(MSG_VC1541_LED, 0);
    }
}

void
VC1541::setRotating(bool b)
{
    if (!rotating && b) {
        rotating = true;
        c64->putMessage(MSG_VC1541_MOTOR, 1);
    } else if (rotating && !b) {
        rotating = false;
        c64->putMessage(MSG_VC1541_MOTOR, 0);
    }
}

void
VC1541::moveHeadUp()
{
    if (halftrack < 84) {

        float position = (float)bitoffset / (float)disk.length.halftrack[halftrack];
        halftrack++;
        bitoffset = position * disk.length.halftrack[halftrack];
         
        // Byte-align bitoffset (to keep the fast loader happy once implemented)
        bitoffset &= 0xFFF8; 
        byteReadyCounter = 0;
        
        debug(3, "Moving head up to halftrack %d (track %2.1f) bit accurate emulation: %s\n",
              halftrack, (halftrack + 1) / 2.0, bitAccuracy ? "YES" : "NO");
    }
   
    assert(disk.isValidDiskPositon(halftrack, bitoffset));
    
    c64->putMessage(MSG_VC1541_HEAD, 1);
    if (halftrack % 2 && sendSoundMessages)
        c64->putMessage(MSG_VC1541_HEAD_SOUND, 1); // play sound for full tracks, only
}

void
VC1541::moveHeadDown()
{
    if (halftrack > 1) {
        float position = (float)bitoffset / (float)disk.length.halftrack[halftrack];
        halftrack--;
        bitoffset = position * disk.length.halftrack[halftrack];

        // Byte-align bitoffset (to keep the fast loader happy once implemented)
        bitoffset &= 0xFFF8;
        byteReadyCounter = 0;
        
        debug(3, "Moving head down to halftrack %d (track %2.1f) bit accurate emulation: %s\n",
              halftrack, (halftrack + 1) / 2.0, bitAccuracy ? "YES" : "NO");
    }
    
    assert(disk.isValidDiskPositon(halftrack, bitoffset));
    
    c64->putMessage(MSG_VC1541_HEAD, 0);
    if (halftrack % 2 && sendSoundMessages)
        c64->putMessage(MSG_VC1541_HEAD_SOUND, 0); // play sound for full tracks, only
}

void
VC1541::insertDisk(D64Archive *a)
{
    assert(a != NULL);
    
    ejectDisk();
    disk.encodeArchive(a);
    
    diskInserted = true;
    setWriteProtection(false);
    c64->putMessage(MSG_VC1541_DISK, 1);
    if (sendSoundMessages)
        c64->putMessage(MSG_VC1541_DISK_SOUND, 1);
}

void
VC1541::insertDisk(Archive *a)
{
    warn("Can only mount D64 images.\n");
}



void 
VC1541::ejectDisk()
{
    if (!hasDisk())
        return;
    
	// Open lid (write protection light barrier will be blocked)
	setWriteProtection(true);

	// Drive will notice the change in its interrupt routine...
	sleepMicrosec((uint64_t)200000);
	
	// Remove disk (write protection light barrier is no longer blocked)
	setWriteProtection(false);
		
    resetDisk();
	c64->putMessage(MSG_VC1541_DISK, 0);
    if (sendSoundMessages)
        c64->putMessage(MSG_VC1541_DISK_SOUND, 0);
}

bool
VC1541::exportToD64(const char *filename)
{
    D64Archive *archive;
    
    assert(filename != NULL);
    
    // Create archive
    if ((archive = D64Archive::archiveFromDrive(this)) == NULL)
        return false;
    
    // Write archive to disk
    archive->writeToFile(filename);
    
    delete archive;
    return true;
}


//
// First implementation of fastLoaderRead() and fastLoaderSync()
// The implementation mimics Frodo. It is a little bit more complicated, because we store real SYNC marks on disk
// while Frodo is using a single 0xFF byte.
//

#if 0
void
VC1541::fastLoaderRead()
{
    uint8_t byteUnderHead = readByteFromHead();
    byteReady(byteUnderHead);
    rotateDiskByOneByte();
}

bool
VC1541::fastLoaderSync()
{
    uint8_t byteUnderHead = readByteFromHead();
    
    if (byteUnderHead == 0xFF) {
        // Move head to last 0xFF byte
        do { rotateDiskByOneByte(); } while (readByteFromHead() == 0xFF);
        rotateBackByOneByte();
        return true;
    } else {
        // Proceed to the next byte
        rotateDiskByOneByte();
        return false;
    }
}
#endif


//
// Second implementation. We try to do a bit better ...
//

void
VC1541::fastLoaderRead()
{
    uint8_t byteUnderHead = readByteFromHead();
    byteReady(byteUnderHead);
    
    if (byteUnderHead == 0xFF) {
        fastLoaderSkipSyncMark(); // If we're inside a SYNC mark, proceed to next data byte
    } else {
        rotateDiskByOneByte(); // If we're outside a SYNC mark, the next data byte is just one byte ahead
    }
}

bool
VC1541::fastLoaderSync()
{
    uint8_t byteUnderHead = readByteFromHead();
    rotateDiskByOneByte();
    return byteUnderHead == 0xFF;
}

#if 0

// THE CODE BELOW SEEMS TO WORK
uint8_t
VC1541::fastLoaderRead()
{
    uint8_t byteUnderHead = readByteFromHead();
    
    // printf("(%02X %04X) ", byteUnderHead, c64->floppy->cpu->getPC_at_cycle_0());
    
    byteReady(byteUnderHead);
    
    if (byteUnderHead == 0xFF) {
        // If we're inside a SYNC mark, proceed to next data byte
        fastLoaderSkipSyncMark();
    } else {
        rotateDiskByOneByte();
    }
    
    return byteUnderHead;
}


bool
VC1541::fastLoaderSync()
{
    bool result;
    uint8_t byteUnderHead = readByteFromHead();

    // printf("[%02X %04X] ", byteUnderHead, c64->floppy->cpu->getPC_at_cycle_0());

    if (byteUnderHead == 0xFF) {
        result = true;
        rotateDiskByOneByte();
    } else {
        result = false;
        rotateDiskByOneByte();
    }

    return result;
}
#endif

