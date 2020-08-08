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
    
    if (snapshotItems)
        delete [] snapshotItems;
}

void
HardwareComponent::initialize()
{
    // Initialize all subcomponents
    for (HardwareComponent *c : subComponents) c->initialize();
    
    // Initialize this component
    _initialize();
}

void
HardwareComponent::powerOn()
{
    if (isPoweredOn()) return;
    assert(!isRunning());
    
    // Power all subcomponents on
    for (HardwareComponent *c : subComponents)  c->powerOn();
    
    // Reset all non-persistant snapshot items
    _reset();
    
    // Power this component on
    _powerOn();

    state = STATE_PAUSED;
}

void
HardwareComponent::powerOff()
{
    if (isPoweredOff()) return;
    
    // Pause if needed
    pause();
    
    // Power off this component
    _powerOff();
    
    // Power all subcomponents off
    for (HardwareComponent *c : subComponents) c->powerOff();

    state = STATE_OFF;
}

void
HardwareComponent::run()
{
    if (isRunning()) return;
    
    // Power on if needed
    powerOn();
    
    // Start all subcomponents
    for (HardwareComponent *c : subComponents) {
        c->run();
    }
    
    // Start this component
    _run();
    
    state = STATE_RUNNING;
}

void
HardwareComponent::pause()
{
    if (!isRunning()) return;
    
    // Pause this component
    _pause();

    // Pause all subcomponents
    for (HardwareComponent *c : subComponents) c->pause();

    state = STATE_PAUSED;
}

void
HardwareComponent::reset()
{
    // Reset all subcomponents
    for (HardwareComponent *c : subComponents) c->reset();

    // Reset this component
    _reset();
}

void
HardwareComponent::ping()
{
    // Ping all subcomponents
    for (HardwareComponent *c : subComponents) c->ping();
    
    // Ping this component
    _ping();
}

void
HardwareComponent::inspect()
{
    // Inspect all subcomponents
    for (HardwareComponent *c : subComponents) {
        c->inspect();
    }
    
    // Inspect this component
    _inspect();
}

void
HardwareComponent::dumpConfig()
{
    msg("%s (memory location: %p)\n\n", getDescription(), this);
    _dumpConfig();
}

void
HardwareComponent::dump()
{
    msg("%s (memory location: %p)\n\n", getDescription(), this);
    _dump();
}

void
HardwareComponent::setWarp(bool enable)
{
    if (warpMode == enable) return;
    
    warpMode = enable;

     // Enable or disable warp mode for all subcomponents
     for (HardwareComponent *c : subComponents) {
         c->setWarp(enable);
     }

     // Enable warp mode for this component
     _setWarp(enable);
}

void
HardwareComponent::setDebug(bool enable)
{
    if (debugMode == enable) return;
    
    debugMode = enable;

     // Enable or disable debug mode for all subcomponents
     for (HardwareComponent *c : subComponents) {
         c->setDebug(enable);
     }

     // Enable debug mode for this component
     _setDebug(enable);
}

void
HardwareComponent::setClockFrequency(u32 value)
{
    assert(value == PAL_CLOCK_FREQUENCY || value == NTSC_CLOCK_FREQUENCY);
    
    for (HardwareComponent *c : subComponents) {
        c->setClockFrequency(value);
    }
    _setClockFrequency(value);
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
    
    for (HardwareComponent *c : subComponents) {
        result += c->stateSize();
    }
        
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
    for (HardwareComponent *c : subComponents) {
        c->loadFromBuffer(buffer);
    }
    
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
    for (HardwareComponent *c : subComponents) {
        c->saveToBuffer(buffer);
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