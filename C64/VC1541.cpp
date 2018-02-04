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
	debug(3, "Releasing VC1541...\n");
}

void
VC1541::reset()
{
    VirtualComponent::reset();
    
    // Establish bindings
    iec = &c64->iec;
    
    cpu.mem = &mem;
    cpu.setPC(0xEAA0);
    halftrack = 41;
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
    debug(3, "Pinging VC1541...\n");
    c64->putMessage(MSG_VC1541_LED, redLED ? 1 : 0);
    c64->putMessage(MSG_VC1541_MOTOR, rotating ? 1 : 0);
    c64->putMessage(MSG_VC1541_DISK, diskInserted ? 1 : 0);

    // TODO: Replace manual pinging of sub components by a call to super::ping()
    cpu.ping();
    mem.ping();
    via1.ping();
    via2.ping();

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
VC1541::powerUp() {

    c64->suspend();
    reset();
    c64->resume();
}
    
bool
VC1541::executeOneCycle() {
    
    via1.execute();
    via2.execute();
    uint8_t result = cpu.executeOneCycle();
    
    // Only proceed if drive is active
    if (!rotating)
        return result;
    
    // If bit accurate emulation is enabled, we don't do anything here
    if (!bitAccuracy) {
        return result;
    }
    
    // Wait until next bit is ready
    if (bitReadyTimer > 0) {
        bitReadyTimer -= 16;
        return result;
    }
    
    // Bit is ready
    executeBitReady();
    
    return result;
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
    if (via2.overflowEnabled()) cpu.setV(1);
}


void 
VC1541::simulateAtnInterrupt()
{
	if (via1.atnInterruptsEnabled()) {
		via1.indicateAtnInterrupt();
		cpu.setIRQLineATN();
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

bool
VC1541::insertDisk(Archive *a)
{
    assert(a != NULL);

    D64Archive *converted;
    
    switch (a->type()) {
            
        case T64_CONTAINER:
        case PRG_CONTAINER:
        case P00_CONTAINER:
            
            // Archives of this type are first converted to D64 format
            if (!(converted = D64Archive::makeD64ArchiveWithAnyArchive(a)))
                return false;
            
            ejectDisk();
            disk.encodeArchive(converted);
            break;
    
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
            
            warn("Only D64, T64, PRG, P00, G64 and NIB archives can be mounted as disk.");
            return false;
    }
    
    diskInserted = true;
    c64->putMessage(MSG_VC1541_DISK, 1);
    if (sendSoundMessages)
        c64->putMessage(MSG_VC1541_DISK_SOUND, 1);

    // If bit accuracy is disabled, we write-protect the disk
    disk.setWriteProtection(!bitAccuracy);
    
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
	c64->putMessage(MSG_VC1541_DISK, 0);
    if (sendSoundMessages)
        c64->putMessage(MSG_VC1541_DISK_SOUND, 0);
}

D64Archive *
VC1541::convertToD64()
{
    D64Archive *archive = new D64Archive();
    debug(1, "Creating D64 archive from currently inserted diskette ...\n");
    
    // Perform test run
    int error;
    if (disk.decodeDisk(NULL, &error) > D64_802_SECTORS_ECC || error) {
        archive->warn("Cannot create archive (error code: %d)\n", error);
        delete archive;
        return NULL;
    }
    
    // Decode diskette
    archive->setNumberOfTracks(42);
    disk.decodeDisk(archive->getData());
    
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
