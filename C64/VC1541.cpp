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
    debug (2, "Resetting VC1541...\n");
    
    // Establish bindings
    this->c64 = c64;
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
    offset = 0;
    bit = 0x80;
    zone = 0;
    read = true;
    read_shiftreg = 0;
    read_shiftreg_pipe = 0;
    write_shiftreg = 0;
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
    uint32_t result = 17;

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
    
    // Disk
    disk.loadFromBuffer(buffer);
    
    // Drive properties
    bitReadyTimer = (int16_t)read16(buffer);
    byteReadyCounter = (uint8_t)read8(buffer);
	rotating = (bool)read8(buffer);
    redLED = (bool)read8(buffer);
    diskInserted = (bool)read8(buffer);
    writeProtected = (bool)read8(buffer);
    sendSoundMessages = (bool)read8(buffer);
    
    // Read/Write logic
    halftrack = (Disk525::Halftrack)read8(buffer);
    offset = read16(buffer);
    bit = read8(buffer);
    zone = read8(buffer);
    read = (bool)read8(buffer);
    read_shiftreg = read8(buffer);
    read_shiftreg_pipe = read8(buffer);
    write_shiftreg = read8(buffer);
    
    // Subcomponents
	cpu->loadFromBuffer(buffer);
    via1.loadFromBuffer(buffer);
    via2.loadFromBuffer(buffer);
    mem->loadFromBuffer(buffer);
    
    debug(2, "  VC1541 state loaded (%d bytes)\n", *buffer - old);
    assert(*buffer - old == stateSize());
}

void 
VC1541::saveToBuffer(uint8_t **buffer)
{	
    uint8_t *old = *buffer;
    
    // Disk
    disk.saveToBuffer(buffer);
    
    // Drive properties
    write16(buffer, bitReadyTimer);
    write8(buffer, byteReadyCounter);
    write8(buffer, (uint8_t)rotating);
    write8(buffer, (uint8_t)redLED);
    write8(buffer, (uint8_t)diskInserted);
    write8(buffer, (uint8_t)writeProtected);
    write8(buffer, (uint8_t)sendSoundMessages);
    
    // Read/Write logic
    write8(buffer, (uint8_t)halftrack);
    write16(buffer, offset);
    write8(buffer, bit);
    write8(buffer, zone);
    write8(buffer, (uint8_t)read);
    write8(buffer, read_shiftreg);
    write8(buffer, read_shiftreg_pipe);
    write8(buffer, write_shiftreg);

    // Subcomponents
    cpu->saveToBuffer(buffer);
    via1.saveToBuffer(buffer);
    via2.saveToBuffer(buffer);
	mem->saveToBuffer(buffer);
    
    debug(4, "  VC1541 state saved (%d bytes)\n", *buffer - old);
    assert(*buffer - old == stateSize());
}

void 
VC1541::dumpState()
{
	msg("VC1541\n");
	msg("------\n\n");
	msg(" Bit ready timer : %d\n", bitReadyTimer);
	msg("   Head position : Track %d, Offset %d, Bit position mask %02X\n", halftrack, offset, bit);
	msg("            SYNC : %d\n", SYNC());
    msg("       Read mode : %s\n", readMode() ? "YES" : "NO");
	msg("\n");
}

void
VC1541::executeBitReady()
{
    bitReadyTimer += cyclesPerBit[zone];
    byteReadyCounter++;

    if (byteReadyCounter == 7) {
        byteReadyCounter = 0;
        executeByteReady();
    }
}

void
VC1541::executeByteReady()
{
    read_shiftreg_pipe = read_shiftreg;
    read_shiftreg = readByteFromDisk();

    if (readMode() && !SYNC()) {
        byteReady(read_shiftreg);
    }

    if (writeMode()) {
        writeByteToDisk(write_shiftreg);
        write_shiftreg = via2.ora;
        byteReady();
    }
    
    rotateDiskByOneByte();
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
        float position = (float)offset / (float)disk.length.halftrack[halftrack];
        halftrack++;
        offset = position * disk.length.halftrack[halftrack];

        debug(3, "Moving head up to halftrack %d (track %2.1f)\n", halftrack, (halftrack + 1) / 2.0);
    }
   
    assert(disk.isValidDiskPositon(halftrack, offset));
    
    c64->putMessage(MSG_VC1541_HEAD, 1);
    if (halftrack % 2 && sendSoundMessages)
        c64->putMessage(MSG_VC1541_HEAD_SOUND, 1); // play sound for full tracks, only
}

void
VC1541::moveHeadDown()
{
    if (halftrack > 1) {
        float position = (float)offset / (float)disk.length.halftrack[halftrack];
        halftrack--;
        offset = position * disk.length.halftrack[halftrack];
        debug(3, "Moving head down to halftrack %d (track %2.1f) %f %d %d\n", halftrack, (halftrack + 1) / 2.0, position, offset, disk.length.halftrack[halftrack]);
    }
    
    assert(disk.isValidDiskPositon(halftrack, offset));
    
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

