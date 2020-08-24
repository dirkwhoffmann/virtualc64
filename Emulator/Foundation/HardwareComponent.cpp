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
HardwareComponent::reset()
{
    // Reset all subcomponents
    for (HardwareComponent *c : subComponents) c->reset();

    // Reset this component
    _reset();
}

bool
HardwareComponent::configure(ConfigOption option, long value)
{
    bool result = false;
    
    // Configure all subcomponents
    for (HardwareComponent *c : subComponents) {
        result |= c->configure(option, value);
    }
    
    // Configure this component
    result |= setConfigItem(option, value);

    return result;
}

bool
HardwareComponent::configure(unsigned dfn, ConfigOption option, long value)
{
    bool result = false;
    
    // Configure all subcomponents
    for (HardwareComponent *c : subComponents) {
        result |= c->configure(dfn, option, value);
    }
    
    // Configure this component
    result |= setConfigItem(dfn, option, value);

    return result;
}

size_t
HardwareComponent::size()
{
    size_t result = _size();

    for (HardwareComponent *c : subComponents) {
        result += c->size();
    }

    return result;
}

size_t
HardwareComponent::load(u8 *buffer)
{
    u8 *ptr = buffer;

    // Call delegation method
    ptr += willLoadFromBuffer(ptr);

    // Load internal state of all subcomponents
    for (HardwareComponent *c : subComponents) {
        ptr += c->load(ptr);
    }

    // Load internal state of this component
    ptr += _load(ptr);

    // Call delegation method
    ptr += didLoadFromBuffer(ptr);

    // Verify that the number of written bytes matches the snapshot size
    debug(SNP_DEBUG, "Loaded %d bytes (expected %d)\n", ptr - buffer, size());
    assert(ptr - buffer == size());

    return ptr - buffer;
}

size_t
HardwareComponent::save(u8 *buffer)
{
    u8 *ptr = buffer;

    // Call delegation method
    ptr += willSaveToBuffer(ptr);

    // Save internal state of all subcomponents
    for (HardwareComponent *c : subComponents) {
        ptr += c->save(ptr);
    }

    // Save internal state of this component
    ptr += _save(ptr);

    // Call delegation method
    ptr += didSaveToBuffer(ptr);

    // Verify that the number of written bytes matches the snapshot size
    debug(SNP_DEBUG, "Saved %d bytes (expected %d)\n", ptr - buffer, size());
    assert(ptr - buffer == size());

    return ptr - buffer;
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
