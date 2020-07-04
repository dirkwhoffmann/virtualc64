// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "C64.h"
#include <algorithm>

HardwareComponent::~HardwareComponent()
{
	debug(RUN_DEBUG, "Terminated\n");
    
    if (subComponents)
        delete [] subComponents;

    if (snapshotItems)
        delete [] snapshotItems;
}

void
HardwareComponent::setC64(C64 *c64)
{
    assert(this->c64 == NULL);
    assert(c64 != NULL);
    
    this->c64 = c64;
    if (subComponents != NULL)
        for (unsigned i = 0; subComponents[i] != NULL; i++)
            subComponents[i]->setC64(c64);
}

void
HardwareComponent::reset()
{
    // Reset all sub components
    if (subComponents != NULL)
        for (unsigned i = 0; subComponents[i] != NULL; i++) {
            subComponents[i]->reset();
        }
    
    // Clear snapshot items marked with 'CLEAR_ON_RESET'
    if (snapshotItems != NULL)
        for (unsigned i = 0; snapshotItems[i].data != NULL; i++)
            if (snapshotItems[i].flags & CLEAR_ON_RESET)
                memset(snapshotItems[i].data, 0, snapshotItems[i].size);
    
    debug(RUN_DEBUG, "Resetting...\n");
}

void
HardwareComponent::ping()
{
    // Ping all sub components
    if (subComponents != NULL)
        for (unsigned i = 0; subComponents[i] != NULL; i++)
            subComponents[i]->ping();
}

void
HardwareComponent::setClockFrequency(u32 frequency)
{
    assert(frequency == PAL_CLOCK_FREQUENCY || frequency == NTSC_CLOCK_FREQUENCY);
    
    // Call method for all sub components
    if (subComponents != NULL)
        for (unsigned i = 0; subComponents[i] != NULL; i++)
            subComponents[i]->setClockFrequency(frequency);
}

void
HardwareComponent::suspend()
{
    c64->suspend();
}

void
HardwareComponent::resume()
{
    c64->resume();
}

void
HardwareComponent::registerSubComponents(HardwareComponent **components, unsigned length) {
    
    assert(components != NULL);
    assert(length % sizeof(HardwareComponent *) == 0);
    
    unsigned numItems = length / sizeof(HardwareComponent *);
    
    // Allocate new array on heap and copy array data
    subComponents = new HardwareComponent*[numItems];
    std::copy(components, components + numItems, &subComponents[0]);
}

void
HardwareComponent::registerSnapshotItems(SnapshotItem *items, unsigned length) {
    
    assert(items != NULL);
    assert(length % sizeof(SnapshotItem) == 0);
    
    unsigned i, numItems = length / sizeof(SnapshotItem);
    
    // Allocate new array on heap and copy array data
    snapshotItems = new SnapshotItem[numItems];
    std::copy(items, items + numItems, &snapshotItems[0]);
    
    // Determine size of snapshot on disk
    for (i = snapshotSize = 0; snapshotItems[i].data != NULL; i++)
        snapshotSize += snapshotItems[i].size;
}

size_t
HardwareComponent::stateSize()
{
    u32 result = snapshotSize;
    
    if (subComponents != NULL)
        for (unsigned i = 0; subComponents[i] != NULL; i++)
            result += subComponents[i]->stateSize();

    return result;
}

void
HardwareComponent::loadFromBuffer(u8 **buffer)
{
    u8 *old = *buffer;
    
    debug(SNP_DEBUG, "    Loading internal state ...\n");
    
    // Call delegation method
    willLoadFromBuffer(buffer);
    
    // Load internal state of all sub components
    if (subComponents != NULL)
        for (unsigned i = 0; subComponents[i] != NULL; i++)
            subComponents[i]->loadFromBuffer(buffer);

    // Load own internal state
    void *data; size_t size; int flags;
    for (unsigned i = 0; snapshotItems != NULL && snapshotItems[i].data != NULL; i++) {
        
        data  = snapshotItems[i].data;
        flags = snapshotItems[i].flags & 0x0F;
        size  = snapshotItems[i].size;
        
        if (flags == 0) { // Auto detect size

            switch (snapshotItems[i].size) {
                case 1:  *(u8 *)data  = read8(buffer); break;
                case 2:  *(u16 *)data = read16(buffer); break;
                case 4:  *(u32 *)data = read32(buffer); break;
                case 8:  *(u64 *)data = read64(buffer); break;
                default: readBlock(buffer, (u8 *)data, size);
            }

        } else { // Format is specified manually
            
            switch (flags) {
                case BYTE_ARRAY: readBlock(buffer, (u8 *)data, size); break;
                case WORD_ARRAY: readBlock16(buffer, (u16 *)data, size); break;
                case DWORD_ARRAY: readBlock32(buffer, (u32 *)data, size); break;
                case QWORD_ARRAY: readBlock64(buffer, (u64 *)data, size); break;
                default: assert(0);
            }
        }
    }
    
    // Call delegation method
    didLoadFromBuffer(buffer);
    
    // Verify that the number of read bytes matches the state size
    if (*buffer - old != stateSize()) {
        panic("loadFromBuffer: Snapshot size is wrong. Got %d, expected %d.",
              *buffer - old, stateSize());
        assert(false);
    }
}

void
HardwareComponent::saveToBuffer(u8 **buffer)
{
    u8 *old = *buffer;
    
    debug(SNP_DEBUG, "    Saving internal state ...\n");

    // Call delegation method
    willSaveToBuffer(buffer);
    
    // Save internal state of all sub components
    if (subComponents != NULL) {
        for (unsigned i = 0; subComponents[i] != NULL; i++)
            subComponents[i]->saveToBuffer(buffer);
    }
    
    // Save own internal state
    void *data; size_t size; int flags;
    for (unsigned i = 0; snapshotItems != NULL && snapshotItems[i].data != NULL; i++) {
        
        data  = snapshotItems[i].data;
        flags = snapshotItems[i].flags & 0x0F;
        size  = snapshotItems[i].size;

        if (flags == 0) { // Auto detect size
            
            switch (snapshotItems[i].size) {
                case 1:  write8(buffer, *(u8 *)data); break;
                case 2:  write16(buffer, *(u16 *)data); break;
                case 4:  write32(buffer, *(u32 *)data); break;
                case 8:  write64(buffer, *(u64 *)data); break;
                default: writeBlock(buffer, (u8 *)data, size);
            }
            
        } else { // Format is specified manually
            
            switch (flags) {
                case BYTE_ARRAY: writeBlock(buffer, (u8 *)data, size); break;
                case WORD_ARRAY: writeBlock16(buffer, (u16 *)data, size); break;
                case DWORD_ARRAY: writeBlock32(buffer, (u32 *)data, size); break;
                case QWORD_ARRAY: writeBlock64(buffer, (u64 *)data, size); break;
                default: assert(0);
            }
        }
    }
    
    // Call delegation method
    didSaveToBuffer(buffer);
    
    // Verify that the number of written bytes matches the state size
    if (*buffer - old != stateSize()) {
        panic("saveToBuffer: Snapshot size is wrong. Got %d, expected %d.",
              *buffer - old, stateSize());
        assert(false);
    }
}
