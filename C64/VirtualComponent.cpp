/*
 * Author: Dirk W. Hoffmann. All rights reserved.
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
#include <algorithm>    // std::copy CAN WE GET RID OF THIS?

VirtualComponent::VirtualComponent()
{
    snapshotItems = NULL;
    subComponents = NULL;
    snapshotSize = 0;
}

VirtualComponent::~VirtualComponent()
{
	debug(3, "Terminated\n");
    
    if (subComponents)
        delete [] subComponents;

    if (snapshotItems)
        delete [] snapshotItems;
}

void
VirtualComponent::setC64(C64 *c64)
{
    this->c64 = c64;
    if (subComponents != NULL)
        for (unsigned i = 0; subComponents[i] != NULL; i++)
            subComponents[i]->setC64(c64);
}

void
VirtualComponent::reset()
{
    // Reset all sub components
    if (subComponents != NULL)
        for (unsigned i = 0; subComponents[i] != NULL; i++)
            subComponents[i]->reset();
    
    // Clear snapshot items marked with 'CLEAR_ON_RESET'
    if (snapshotItems != NULL)
        for (unsigned i = 0; snapshotItems[i].data != NULL; i++)
            if (snapshotItems[i].flags & CLEAR_ON_RESET)
                memset(snapshotItems[i].data, 0, snapshotItems[i].size);
    
    debug(3, "Resetting...\n");
    
}

void
VirtualComponent::ping()
{
    // Ping all sub components
    if (subComponents != NULL)
        for (unsigned i = 0; subComponents[i] != NULL; i++)
            subComponents[i]->ping();
}

void
VirtualComponent::setClockFrequency(uint32_t frequency)
{
    assert(frequency == PAL_CLOCK_FREQUENCY || frequency == NTSC_CLOCK_FREQUENCY);
    
    // Call method for all sub components
    if (subComponents != NULL)
        for (unsigned i = 0; subComponents[i] != NULL; i++)
            subComponents[i]->setClockFrequency(frequency);
}

void 
VirtualComponent::dumpState()
{
}


//
// Snapshots
// 

void
VirtualComponent::registerSnapshotItems(SnapshotItem *items, unsigned length) {
    
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

void
VirtualComponent::registerSubComponents(VirtualComponent **components, unsigned length) {
    
    assert(components != NULL);
    assert(length % sizeof(VirtualComponent *) == 0);
    
    unsigned numItems = length / sizeof(VirtualComponent *);
    
    // Allocate new array on heap and copy array data
    subComponents = new VirtualComponent*[numItems];
    std::copy(components, components + numItems, &subComponents[0]);    
}

size_t
VirtualComponent::stateSize()
{
    uint32_t result = snapshotSize;
    
    if (subComponents != NULL)
        for (unsigned i = 0; subComponents[i] != NULL; i++)
            result += subComponents[i]->stateSize();

    return result;
}

void
VirtualComponent::loadFromBuffer(uint8_t **buffer)
{
    uint8_t *old = *buffer;
    
    debug(3, "    Loading internal state (%d bytes) ...\n", VirtualComponent::stateSize());
    
    // Load internal state of sub components
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
                case 1:  *(uint8_t *)data  = read8(buffer); break;
                case 2:  *(uint16_t *)data = read16(buffer); break;
                case 4:  *(uint32_t *)data = read32(buffer); break;
                case 8:  *(uint64_t *)data = read64(buffer); break;
                default: readBlock(buffer, (uint8_t *)data, size);
            }

        } else { // Format is specified manually
            
            switch (flags) {
                case BYTE_ARRAY: readBlock(buffer, (uint8_t *)data, size); break;
                case WORD_ARRAY: readBlock16(buffer, (uint16_t *)data, size); break;
                case DWORD_ARRAY: readBlock32(buffer, (uint32_t *)data, size); break;
                case QWORD_ARRAY: readBlock64(buffer, (uint64_t *)data, size); break;
                default: assert(0);
            }
        }
    }
    
    if (*buffer - old != VirtualComponent::stateSize()) {
        panic("loadFromBuffer: Snapshot size is wrong.\n");
        assert(false);
    }
}

void
VirtualComponent::saveToBuffer(uint8_t **buffer)
{
    uint8_t *old = *buffer;

    debug(3, "    Saving internal state (%d bytes) ...\n", VirtualComponent::stateSize());

    // Save internal state of sub components
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
                case 1:  write8(buffer, *(uint8_t *)data); break;
                case 2:  write16(buffer, *(uint16_t *)data); break;
                case 4:  write32(buffer, *(uint32_t *)data); break;
                case 8:  write64(buffer, *(uint64_t *)data); break;
                default: writeBlock(buffer, (uint8_t *)data, size);
            }
            
        } else { // Format is specified manually
            
            switch (flags) {
                case BYTE_ARRAY: writeBlock(buffer, (uint8_t *)data, size); break;
                case WORD_ARRAY: writeBlock16(buffer, (uint16_t *)data, size); break;
                case DWORD_ARRAY: writeBlock32(buffer, (uint32_t *)data, size); break;
                case QWORD_ARRAY: writeBlock64(buffer, (uint64_t *)data, size); break;
                default: assert(0);
            }
        }
    }
    
    if (*buffer - old != VirtualComponent::stateSize()) {
        panic("saveToBuffer: Snapshot size is wrong.");
        assert(false);
    }
}

