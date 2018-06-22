/*!
 * @header      Datasette.h
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   2015 - 2016 Dirk W. Hoffmann
 * @brief       Declares Datasette class
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

Datasette::Datasette()
{
    setDescription("Datasette");
    debug(3, "Creating virtual datasette at address %p\n", this);
        
    // Register snapshot items
    SnapshotItem items[] = {
        
        // Tape properties (will survive reset)
        { &size,                    sizeof(size),                   KEEP_ON_RESET },
        { &type,                    sizeof(type),                   KEEP_ON_RESET },
        { &durationInCycles,        sizeof(durationInCycles),       KEEP_ON_RESET },
        
        // Internal state (will be cleared on reset)
        { &head,                    sizeof(head),                   CLEAR_ON_RESET },
        { &headInCycles,            sizeof(headInCycles),           CLEAR_ON_RESET },
        { &headInSeconds,           sizeof(headInSeconds),          CLEAR_ON_RESET },
        { &nextRisingEdge,          sizeof(nextRisingEdge),         CLEAR_ON_RESET },
        { &nextFallingEdge,         sizeof(nextFallingEdge),        CLEAR_ON_RESET },
        { &playKey,                 sizeof(playKey),                CLEAR_ON_RESET },
        { &motor,                   sizeof(motor),                  CLEAR_ON_RESET },
        
        { NULL,                     0,                              0 }};
    
    registerSnapshotItems(items, sizeof(items));
    
    // Initialize all values that are not initialized in reset()
    data = NULL;
    size = 0;
    type = 0;
    durationInCycles = 0;
}

Datasette::~Datasette()
{
    debug(3, "Releasing Datasette...\n");

    if (data)
        delete data;
}

void
Datasette::reset()
{
    VirtualComponent::reset();
    rewind();
}

void
Datasette::ping()
{
    VirtualComponent::ping();
    c64->putMessage(hasTape() ? MSG_VC1530_TAPE : MSG_VC1530_NO_TAPE);
    c64->putMessage(MSG_VC1530_PROGRESS);
}

size_t
Datasette::stateSize()
{
    return VirtualComponent::stateSize() + size;
}

void
Datasette::loadFromBuffer(uint8_t **buffer)
{
    uint8_t *old = *buffer;
    
    VirtualComponent::loadFromBuffer(buffer);
    if (size) {
        if (data == NULL)
            data = (uint8_t *)malloc(size);
        readBlock(buffer, (uint8_t *)data, size);
    }
    
    if (*buffer - old != stateSize())
        assert(0);
}

void
Datasette::saveToBuffer(uint8_t **buffer)
{
    uint8_t *old = *buffer;
    
    VirtualComponent::saveToBuffer(buffer);
    if (size) {
        assert(data != NULL);
        writeBlock(buffer, (uint8_t *)data, size);
    }
    
    if (*buffer - old != stateSize())
        assert(0);
}

void
Datasette::dumpState()
{
#if 0
    msg("Datasette\n");
    msg("---------\n\n");
#endif
}

void
Datasette::setHeadInCycles(uint64_t value)
{
    printf("Fast forwarding to cycle %lld (duration %lld)\n", value, durationInCycles);
    rewind();
    while (headInCycles <= value && head < size)
        advanceHead(true);
    printf("Head is %llu (max %llu)\n", head, size);
}

void
Datasette::insertTape(TAPFile *a)
{
    size = a->getSize();
    type = a->TAPversion();
    
    debug(2, "Inserting tape (size = %d, type = %d)...\n", size, type);
    
    // Copy data
    data = (uint8_t *)malloc(size);
    memcpy(data, a->getData(), size);

    // Determine tape length (by fast forwarding)
    rewind();
    while (head < size)
        advanceHead(true /* Don't send tape progress messages */);

    durationInCycles = headInCycles;
    rewind();
    
    c64->putMessage(MSG_VC1530_TAPE);
}

void
Datasette::ejectTape()
{
    debug(2, "Ejecting tape\n");

    if (!hasTape())
        return;
    
    pressStop();
    
    assert(data != NULL);
    free(data);
    data = NULL;
    size = 0;
    type = 0;
    durationInCycles = 0;
    head = -1;

    c64->putMessage(MSG_VC1530_NO_TAPE);
}

void
Datasette::advanceHead(bool silent)
{
    // Return if end of tape is already reached
    if (head == size)
        return;
    
    // Update head and headInCycles
    int length, skip;
    length = pulseLength(&skip);
    head += skip;
    headInCycles += length;
    
    // Send message if the tapeCounter (in seconds) changes
    uint32_t newHeadInSeconds = (uint32_t)(headInCycles / PAL_CYCLES_PER_SECOND);
    if (newHeadInSeconds != headInSeconds && !silent)
        c64->putMessage(MSG_VC1530_PROGRESS);

    // Update headInSeconds
    headInSeconds = newHeadInSeconds;
}

int
Datasette::pulseLength(int *skip)
{
    assert(head < size);

    if (data[head] != 0) {
        // Pulse lengths between 1 * 8 and 255 * 8
        if (skip) *skip = 1;
        return 8 * data[head];
    }
    
    if (type == 0) {
        // Pulse lengths greater than 8 * 255 (TAP V0 files)
        if (skip) *skip = 1;
        return 8 * 256;
    } else {
        // Pulse lengths greater than 8 * 255 (TAP V1 files)
        if (skip) *skip = 4;
        return  LO_LO_HI_HI(data[head+1], data[head+2], data[head+3], 0);
    }
}

void
Datasette::pressPlay()
{
    if (!hasTape())
        return;
    
    debug("Datasette::pressPlay\n");
    playKey = true;

    // Schedule first pulse
    uint64_t length = pulseLength();
    nextRisingEdge = length / 2;
    nextFallingEdge = length;
}

void
Datasette::pressStop()
{
    debug("Datasette::pressStop\n");
    setMotor(false);
    playKey = false;
}

void
Datasette::setMotor(bool value)
{
    if (motor == value)
        return;
    
    motor = value;
}

void
Datasette::_execute()
{
    if (!hasTape() || !playKey || !motor)
        return;
    
    nextRisingEdge--;
    nextFallingEdge--;
    
    if (nextRisingEdge == 0) {
        _executeRising();
        return;
    }

    if (nextFallingEdge == 0 && head < size) {
        _executeFalling();
        return;
    }
    
    if (head >= size) {
        pressStop();
    }
}

void
Datasette::_executeRising()
{
    c64->cia1.triggerRisingEdgeOnFlagPin();
}

void
Datasette::_executeFalling()
{
    c64->cia1.triggerFallingEdgeOnFlagPin();
    
    // Schedule next pulse
    advanceHead();
    uint64_t length = pulseLength();
    nextRisingEdge = length / 2;
    nextFallingEdge = length;
}
