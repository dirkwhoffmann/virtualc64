// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "C64.h"

Datasette::Datasette()
{
    setDescription("Datasette");
    debug(3, "Creating virtual datasette at address %p\n", this);
        
    // Register snapshot items
    SnapshotItem items[] = {
        
        // Tape properties (will survive reset)
        { &size,               sizeof(size),              KEEP_ON_RESET },
        { &type,               sizeof(type),              KEEP_ON_RESET },
        { &durationInCycles,   sizeof(durationInCycles),  KEEP_ON_RESET },
        
        // Internal state (will be cleared on reset)
        { &head,               sizeof(head),              CLEAR_ON_RESET },
        { &headInCycles,       sizeof(headInCycles),      CLEAR_ON_RESET },
        { &headInSeconds,      sizeof(headInSeconds),     CLEAR_ON_RESET },
        { &nextRisingEdge,     sizeof(nextRisingEdge),    CLEAR_ON_RESET },
        { &nextFallingEdge,    sizeof(nextFallingEdge),   CLEAR_ON_RESET },
        { &playKey,            sizeof(playKey),           CLEAR_ON_RESET },
        { &motor,              sizeof(motor),             CLEAR_ON_RESET },
        
        { NULL,                0,                         0 }};
    
    registerSnapshotItems(items, sizeof(items));
}

Datasette::~Datasette()
{
    debug(3, "Releasing Datasette...\n");

    if (data)
        delete[] data;
}

void
Datasette::reset()
{
    HardwareComponent::reset();
    rewind();
}

void
Datasette::ping()
{
    HardwareComponent::ping();
    c64->putMessage(hasTape() ? MSG_VC1530_TAPE : MSG_VC1530_NO_TAPE);
    c64->putMessage(MSG_VC1530_PROGRESS);
}

size_t
Datasette::stateSize()
{
    return HardwareComponent::stateSize() + size;
}

void
Datasette::didLoadFromBuffer(uint8_t **buffer)
{
    if (data) delete[] data;
    
    if (size) {
        data = new uint8_t[size];
        readBlock(buffer, data, size);
    }
}

void
Datasette::didSaveToBuffer(uint8_t **buffer)
{
    if (size) {
        assert(data != NULL);
        writeBlock(buffer, data, size);
    }
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

bool
Datasette::insertTape(TAPFile *a)
{
    suspend();
    
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
    resume();
    
    return true;
}

void
Datasette::ejectTape()
{
    suspend();
    
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
    resume();
}

void
Datasette::advanceHead(bool silent)
{
    // Return if end of tape has been reached already
    if (head == size)
        return;
    
    // Update head and headInCycles
    int length, skip;
    length = pulseLength(&skip);
    head += skip;
    headInCycles += length;
    
    // Send message if the tapeCounter (in seconds) changes
    uint32_t newHeadInSeconds = (uint32_t)(headInCycles / c64->frequency);
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
