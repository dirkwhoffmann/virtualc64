/*
 * Written 2015 by Dirk W. Hoffmann
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

Datasette::Datasette()
{
    name = "Datasette";
    debug(2, "Creating virtual datasette at address %p\n", this);
        
    // Register snapshot items
    SnapshotItem items[] = {
        
        // Tape properties (will survive reset)
        { &size,            sizeof(size),                   KEEP_ON_RESET },
        { &type,            sizeof(type),                   KEEP_ON_RESET },
        { &duration,        sizeof(duration),               KEEP_ON_RESET },
        
        // Internal state (will be cleared on reset)
        { &middleOfPulse,   sizeof(middleOfPulse),          CLEAR_ON_RESET },
        { &pulseLength,     sizeof(pulseLength),            CLEAR_ON_RESET },

        { &playKey,         sizeof(playKey),                CLEAR_ON_RESET },
        { &motor,           sizeof(motor),                  CLEAR_ON_RESET },
        { &nextPulse,       sizeof(nextPulse),              CLEAR_ON_RESET },
        { &head,            sizeof(head),                   CLEAR_ON_RESET },
        { NULL,             0,                              0 }};
    
    registerSnapshotItems(items, sizeof(items));
    
    // Initialize all values that are not initialized in reset()
    data = NULL;
    size = 0;
    type = 0;
    duration = 0;
    
    head = -1;
}

Datasette::~Datasette()
{
    debug(2, "Releasing Datasette...\n");

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
    debug(2, "Pinging Datasette...\n");
#if 0
    c64->putMessage(MSG_VC1541_LED, redLED ? 1 : 0);
    c64->putMessage(MSG_VC1541_MOTOR, rotating ? 1 : 0);
    c64->putMessage(MSG_VC1541_DISK, diskInserted ? 1 : 0);
#endif
}

uint32_t
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
    msg(" Bit ready timer : %d\n", bitReadyTimer);
    msg("   Head position : Track %d, Bit offset %d\n", halftrack, bitoffset);
    msg("            SYNC : %d\n", sync);
    msg("       Read mode : %s\n", readMode() ? "YES" : "NO");
    msg("\n");
#endif
}

uint32_t
Datasette::getHeadPosition()
{
    return head;
}

uint32_t
Datasette::getHeadPositionInSeconds()
{
    if (head == -1)
        return 0;

    unsigned percentage = (100 * head) / size;
    return elapsedTime[percentage];
}

void
Datasette::setHeadPosition(uint32_t value)
{
    head = value;
}

void
Datasette::setHeadPositionInSeconds(uint32_t value)
{
    unsigned percentage = (100 * value) / duration;
    head = (percentage < 100) ? headPosition[percentage] : headPosition[100];
}

#if 0
unsigned
Datasette::durationUntil(int headpos)
{
    int oldhead = head;
    
    // Determine tape length (in clock cycles)
    int64_t pulse, cycles = 0;
    rewind();
    for (unsigned i = 0; i < headpos; i++) {
        if ((pulse = nextPulseLength()) == -1)
            break;
        cycles += pulse;
    }
    
    // Determine tape length (in seconds)
    head = oldhead;
    return cycles / (PAL_CYCLES_PER_FRAME * PAL_REFRESH_RATE);
}

unsigned
Datasette::durationUntilPercentage(int percentage)
{
    int headpos = (percentage * duration) / 100;
    return durationUntil(headpos);
}
#endif

void
Datasette::insertTape(TAPArchive *a)
{
    size = a->getSize();
    type = a->TAPversion();
    
    debug(2, "Inserting tape (size = %d, type = %d)...\n", size, type);
    
    // Copy data
    data = (uint8_t *)malloc(size);
    memcpy(data, a->getData(), size);
    rewind();
    
    for (unsigned i = 16; i > 0; i--) {
        fprintf(stderr, "%02X ", data[size - i]);
    }
    fprintf(stderr, "\n");

    // Determine total tape length (in clock cycles)
    int skip;
    uint64_t totalCycles, cycles, i;
    for (i = totalCycles = 0; i < size; i += skip) {
        totalCycles += computePulseLength(i, &skip);
    }
    duration = totalCycles / PAL_CYCLES_PER_SECOND;

    // Prepare mapping arrays
    for (i = 0; i <= 100; i++)
        headPosition[i] = elapsedTime[i] = -1;
    
    // Fill up arrays
    for (i = cycles = 0; i < size; i += skip) {
        
        unsigned spacePercentage = ((100 * i) / size);
        unsigned timePercentage = ((100 * cycles) / totalCycles);

        if (headPosition[timePercentage] == -1)
            headPosition[timePercentage] = i;
        
        if (elapsedTime[spacePercentage] == -1)
            elapsedTime[spacePercentage] = cycles / PAL_CYCLES_PER_SECOND;

        cycles += computePulseLength(i, &skip);
    }
    headPosition[100] = size - 1;
    elapsedTime[100] = totalCycles / PAL_CYCLES_PER_SECOND;
    
    /*
    for (i = 0; i < 101; i++) {
        debug(2, "headPosition[%d] = %d\n", i, headPosition[i]);
        debug(2, "elapsedTime[%d] = %d\n", i, elapsedTime[i]);
    }
    */
}

void
Datasette::ejectTape()
{
    debug(2, "Ejecting tape\n");

    if (!hasTape())
        return;

    assert(data != NULL);

    free(data);
    data = NULL;
    size = 0;
    type = 0;
    duration = 0;
    head = -1;
}

int
Datasette::getByte()
{
    int result;
    // static int debugcnt = 0;
    
    if (head < 0 || head >= size)
        return -1;
    
    // get byte
    result = (uint8_t)data[head];
    
    // check for end of file
    if (head == (size - 1)) {
        head = -1;
    } else {
        // advance head
        head++;
    }
    
    return result;
}

int
Datasette::computePulseLength(int headpos, int *skip)
{
    // Pulse lengths between 1 * 8 and 255 * 8
    if (data[headpos] != 0) {
        *skip = 1;
        return 8 * data[headpos];
    }
    
    // Pulse lengths greater than 8 * 255
    if (type == 0) {
        *skip = 1;
        return 8 * 256;
    }
    *skip = 4;
    return  LO_LO_HI_HI(data[headpos+1], data[headpos+2], data[headpos+3], 0);
}

int
Datasette::nextPulseLength()
{
    int byte = getByte();
    
    if (byte == -1)
        return -1;
    
    if (byte != 0)
        return 8 * byte;

    // Long pulse (encoding depends on the TAP type)
    return type ? LO_LO_HI_HI(getByte(), getByte(), getByte(), 0) : (8 * 256);
}

void
Datasette::pressPlay()
{
    debug("Datasette::pressPlay\n");
    nextPulse = c64->getCycles() + 0.5 * PAL_CYCLES_PER_FRAME * 60; /* wait approx. 0.5 seconds */
    playKey = true;
}

void
Datasette::pressStop()
{
    debug("Datasette::pressStop\n");
    playKey = false;
}

void
Datasette::setMotor(bool value)
{
    if (motor == value)
        return;
    
    motor = value;
    debug(2, "Motor %s\n", motor ? "on" : "off");
}

void
Datasette::_execute()
{
    if (!hasTape() || !playKey || !motor)
        return;
    
    if (c64->getCycles() < nextPulse)
        return;
    
    if (middleOfPulse)
        _executeMiddle();
    else
        _executeBeginning();
}

void
Datasette::_executeBeginning()
{
    static int percentage = -42;

    pulseLength = nextPulseLength();

    if (head == 0) {
        
        // Trigger first edge (transmission starts here)
        assert(pulseLength != -1);
        c64->cia1->triggerFallingEdgeOnFlagPin();

        // Schedule next pulse
        nextPulse = c64->getCycles() + (pulseLength / 2);
        middleOfPulse = 1;
    }
    
    else if (pulseLength == -1) {
        
        // Trigger last edge (transmission ends here)
        c64->cia1->triggerFallingEdgeOnFlagPin();        
    }
    
    else {
        
        // Trigger falling edge
        c64->cia1->triggerFallingEdgeOnFlagPin();

        // Schedule next pulse
        nextPulse = c64->getCycles() + (pulseLength / 2);
        middleOfPulse = 1;
        
        // Progress info
        assert(head != 0);
        assert(size != 0);
        if (percentage != progress()) {
            percentage = progress();
            debug("%d percent completed\n", percentage);
        }
    }
}

void
Datasette::_executeMiddle()
{
    // Trigger rising edge
    c64->cia1->triggerRisingEdgeOnFlagPin();
    
    // Schedule next pulse
    nextPulse = c64->getCycles() + (pulseLength / 2);
    middleOfPulse = 0;
}

