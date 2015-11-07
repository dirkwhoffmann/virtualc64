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
    
    // Register sub components
    VirtualComponent *subcomponents[] = { mem, cpu, &via1, &via2, &disk, NULL };
    registerSubComponents(subcomponents, sizeof(subcomponents)); 

    // Register snapshot items
    SnapshotItem items[] = {

        // Configuration items
        { &bitAccuracy,             sizeof(bitAccuracy),            KEEP_ON_RESET },
        { &sendSoundMessages,       sizeof(sendSoundMessages),      KEEP_ON_RESET },
        
        // Internal state
        { &bitReadyTimer,           sizeof(bitReadyTimer),          CLEAR_ON_RESET },
        { &byteReadyCounter,        sizeof(byteReadyCounter),       CLEAR_ON_RESET },
        { &rotating,                sizeof(rotating),               CLEAR_ON_RESET },
        { &redLED,                  sizeof(redLED),                 CLEAR_ON_RESET },
        { &diskPartiallyInserted,   sizeof(diskPartiallyInserted),  CLEAR_ON_RESET },
        { &halftrack,               sizeof(halftrack),              CLEAR_ON_RESET },
        { &bitoffset,               sizeof(bitoffset),              CLEAR_ON_RESET },
        { &zone,                    sizeof(zone),                   CLEAR_ON_RESET },
        { &read_shiftreg,           sizeof(read_shiftreg),          CLEAR_ON_RESET },
        { &write_shiftreg,          sizeof(write_shiftreg),         CLEAR_ON_RESET },
        { &sync,                    sizeof(sync),                   CLEAR_ON_RESET },
        
        // Disk properties (will survive reset)
        { &diskInserted,            sizeof(diskInserted),           KEEP_ON_RESET },
        { NULL,                     0,                              0 }};
    
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
VC1541::reset()
{
    VirtualComponent::reset();
    
    // Establish bindings
    iec = c64->iec;
    
    cpu->mem = mem;
    cpu->setPC(0xEAA0);
    halftrack = 41;
}

void
VC1541::resetDisk()
{
    debug (2, "Resetting disk in VC1541...\n");
    
    // Disk properties
    disk.clearDisk();
    diskInserted = false;
    diskPartiallyInserted = false;
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

#if 0
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
#endif 

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
        disk.setModified(true); 
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
    // assert(bitoffset % 8 == 0);
    
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
         
        // Make sure new bitoffset starts at the beginning of a new byte to keep fast loader happy
        alignHead();
        
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

        // Make sure new bitoffset starts at the beginning of a new byte to keep fast loader happy
        alignHead();
        
        debug(3, "Moving head down to halftrack %d (track %2.1f) bit accurate emulation: %s\n",
              halftrack, (halftrack + 1) / 2.0, bitAccuracy ? "YES" : "NO");
    }
    
    assert(disk.isValidDiskPositon(halftrack, bitoffset));
    
    c64->putMessage(MSG_VC1541_HEAD, 0);
    if (halftrack % 2 && sendSoundMessages)
        c64->putMessage(MSG_VC1541_HEAD_SOUND, 0); // play sound for full tracks, only
}

void
VC1541::setBitAccuracy(bool b)
{
    bitAccuracy = b;
    
    if (!b) { // If bit accuracy is disabled, ...
        
        // we align the drive head to the beginning of a byte
        alignHead();
        
        // and write-protect the disk.
        disk.setWriteProtection(true);
    }
}

void
VC1541::insertDisk(Archive *a)
{
    assert(a != NULL);
    
    D64Archive *d64 = (D64Archive *)a;
    G64Archive *g64 = (G64Archive *)a;
    
    switch (a->getType()) {
            
        case D64_CONTAINER:
            
            ejectDisk();
            disk.encodeArchive(d64);
            break;
            
        case G64_CONTAINER:
            
            ejectDisk();
            disk.encodeArchive(g64);
            break;
            
        default:
            
            warn("Only D64 or G64 archives can be mounted as virtual disk.");
            return;
    }
    
    diskInserted = true;
    c64->putMessage(MSG_VC1541_DISK, 1);
    if (sendSoundMessages)
        c64->putMessage(MSG_VC1541_DISK_SOUND, 1);

    // If bit accuracy is disabled, we write-protect the disk
    disk.setWriteProtection(true);
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
VC1541::getFastLoaderSync()
{
    uint8_t byteUnderHead = readByteFromHead();
    rotateDiskByOneByte();
    return byteUnderHead == 0xFF;
}
