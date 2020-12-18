// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "C64.h"

Datasette::~Datasette()
{
    if (data) delete[] data;
}

void
Datasette::_reset()
{
    RESET_SNAPSHOT_ITEMS
    
    rewind();
}

void
Datasette::setHeadInCycles(u64 value)
{
    printf("Fast forwarding to cycle %lld (duration %lld)\n", value, durationInCycles);

    rewind();
    while (headInCycles <= value && head < size) advanceHead(true);

    printf("Head is %llu (max %llu)\n", head, size);
}

bool
Datasette::insertTape(TAPFile *a)
{
    suspend();
    
    size = a->getDataSize();
    type = a->TAPversion();
    
    debug(TAP_DEBUG, "Inserting tape (size = %d, type = %d)...\n", size, type);
    
    // Copy data
    data = (u8 *)malloc(size);
    memcpy(data, a->getData(), size);

    // Determine tape length (by fast forwarding)
    rewind();
    while (head < size) advanceHead(true /* Don't send progress messages */);

    durationInCycles = headInCycles;
    rewind();
    
    c64.putMessage(MSG_VC1530_TAPE);
    resume();
    
    return true;
}

void
Datasette::ejectTape()
{
    suspend();
    
    debug(TAP_DEBUG, "Ejecting tape\n");

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

    c64.putMessage(MSG_VC1530_NO_TAPE);
    resume();
}

void
Datasette::advanceHead(bool silent)
{
    assert(head < size);
    
    // Update head and headInCycles
    int length, skip;
    length = pulseLength(&skip);
    head += skip;
    headInCycles += length;
    
    // Send message if the tapeCounter (in seconds) changes
    u32 newHeadInSeconds = (u32)(headInCycles / c64.frequency);
    if (newHeadInSeconds != headInSeconds && !silent)
        c64.putMessage(MSG_VC1530_PROGRESS);
    
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
        if (head + 3 < size) {
            return  LO_LO_HI_HI(data[head+1], data[head+2], data[head+3], 0);
        } else {
            warn("TAP file ended unexpectedly (%d, %d)\n", size, head + 3);
            assert(false);
            return 8 * 256;
        }
    }
}

void
Datasette::pressPlay()
{
    if (!hasTape())
        return;
    
    debug(TAP_DEBUG, "pressPlay\n");
    playKey = true;

    // Schedule first pulse
    uint64_t length = pulseLength();
    nextRisingEdge = length / 2;
    nextFallingEdge = length;
    advanceHead();
}

void
Datasette::pressStop()
{
    debug(TAP_DEBUG, "pressStop\n");
    motor = false;
    playKey = false;
}

void
Datasette::_execute()
{
    // Only proceed if the datasette is active
    if (!hasTape() || !playKey || !motor) return;
        
    if (--nextRisingEdge == 0) {
        
        cia1.triggerRisingEdgeOnFlagPin();
    }

    if (--nextFallingEdge == 0) {
        
        cia1.triggerFallingEdgeOnFlagPin();

        if (head < size) {

            // Schedule the next pulse
            uint64_t length = pulseLength();
            nextRisingEdge = length / 2;
            nextFallingEdge = length;
            advanceHead();
            
        } else {
            
            // Press the stop key
            pressStop();
        }
    }
}
