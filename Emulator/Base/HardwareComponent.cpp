// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "HardwareComponent.h"

HardwareComponent::~HardwareComponent()
{
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
HardwareComponent::reset(bool hard)
{
    // Reset all subcomponents
    for (HardwareComponent *c : subComponents) c->reset(hard);

    // Reset this component
    _reset(hard);    
}

bool
HardwareComponent::configure(Option option, i64 value)
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
HardwareComponent::configure(Option option, long id, i64 value)
{
    bool result = false;
    
    // Configure all subcomponents
    for (HardwareComponent *c : subComponents) {
        result |= c->configure(option, id, value);
    }
    
    // Configure this component
    result |= setConfigItem(option, id, value);

    return result;
}

isize
HardwareComponent::size()
{
    isize result = _size();

    for (HardwareComponent *c : subComponents) {
        result += c->size();
    }

    return result;
}

isize
HardwareComponent::load(const u8 *buffer)
{    
    const u8 *ptr = buffer;

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
    trace(SNP_DEBUG, "Loaded %ld bytes (expected %zu)\n", ptr - buffer, size());
    assert(ptr - buffer == (long)size());

    return ptr - buffer;
}

isize
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
    trace(SNP_DEBUG, "Saved %ld bytes (expected %zu)\n", ptr - buffer, size());
    assert(ptr - buffer == (long)size());

    return ptr - buffer;
}

void
HardwareComponent::powerOn()
{
    for (auto c : subComponents) { c->powerOn(); }
    _powerOn();
}

void
HardwareComponent::powerOff()
{
    _powerOff();
    for (auto c : subComponents) { c->powerOff(); }
}

void
HardwareComponent::run()
{
    for (auto c : subComponents) { c->run(); }
    _run();
}

void
HardwareComponent::pause()
{
    _pause();
    for (auto c : subComponents) { c->pause(); }
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

void HardwareComponent::dump(dump::Category category, std::ostream& ss) const
{
    _dump(category, ss);
}

void
HardwareComponent::dump(dump::Category category) const
{
    dump(category, std::cout);
}

void
HardwareComponent::dump(std::ostream& ss) const
{
    dump((dump::Category)(-1), ss);
}

void
HardwareComponent::dump() const
{
    dump((dump::Category)(-1));
}

void
HardwareComponent::setWarp(bool enable)
{
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
