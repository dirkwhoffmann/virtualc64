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
    head = -1;
    type = 0;
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

void
Datasette::insertTape(TAPArchive *a)
{
    size = a->getSize();
    type = a->TAPversion();
    
    debug(2, "Inserting tape (size = %d, type = %d)...\n", size, type);
    
    data = (uint8_t *)malloc(size);
    memcpy(data, a->getData(), size);
    rewind();
    
    for (unsigned i = 0; i < 10; i++) {
        for (unsigned j = 0; j < 16; j++) {
            fprintf(stderr, "%02X ", data[head+i*16+j]);
        }
        fprintf(stderr, "\n");
    }
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
    if (c64->getCycles() >= nextPulse) {
        if (middleOfPulse)
            _executeMiddle();
        else
            _executeBeginning();
    }
}

void
Datasette::_executeBeginning()
{
    static int percentage = -42;
    static int pulsecnt = 0;
    static int eot = 0;

    // Get pulse length
    pulseLength = nextPulseLength(); 

    // Check for end of tape
    if (pulseLength == -1) {
        if (!eot) {
            debug(2, "End of tape reached.\n");
            eot = 1;
        }
        c64->cia1->triggerFallingEdgeOnFlagPin();
        // playKey = false;
        return;
    }
    
    // Pull signal low (possibly causing an interrupt) and schedule rising edge
    c64->cia1->triggerFallingEdgeOnFlagPin();
    nextPulse = c64->getCycles() + (pulseLength / 2);
    middleOfPulse = 1;

    // Debug Output
    if (pulsecnt++ < 300) {
        debug(2, "Next pulse in %d cycles\n", pulseLength);
    }
    if (percentage != headPositionInPercent()) {
        debug(" %d %% completed\n", percentage);
        percentage = headPositionInPercent();
    }
}

void
Datasette::_executeMiddle()
{
    // Pull signal high and schedule falling edge
    c64->cia1->triggerRisingEdgeOnFlagPin();
    nextPulse = c64->getCycles() + (pulseLength / 2);
    middleOfPulse = 0;
}

