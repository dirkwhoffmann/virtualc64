// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "C64Component.h"

C64Component::~C64Component()
{
}

void
C64Component::initialize()
{
    // Initialize all subcomponents
    for (C64Component *c : subComponents) c->initialize();
    
    // Initialize this component
    _initialize();
}

void
C64Component::reset(bool hard)
{
    // Reset all subcomponents
    for (C64Component *c : subComponents) c->reset(hard);

    // Reset this component
    _reset(hard);    
}

void
C64Component::configure(Option option, i64 value)
{
    // Configure all subcomponents
    for (C64Component *c : subComponents) {
        c->configure(option, value);
    }
    
    // Configure this component
    setConfigItem(option, value);
}

void
C64Component::configure(Option option, long id, i64 value)
{
    // Configure all subcomponents
    for (C64Component *c : subComponents) {
        c->configure(option, id, value);
    }
    
    // Configure this component
    setConfigItem(option, id, value);
}

isize
C64Component::size()
{
    isize result = _size();

    for (C64Component *c : subComponents) {
        result += c->size();
    }

    return result;
}

isize
C64Component::load(const u8 *buffer)
{    
    const u8 *ptr = buffer;

    // Call delegation method
    ptr += willLoadFromBuffer(ptr);

    // Load internal state of all subcomponents
    for (C64Component *c : subComponents) {
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
C64Component::save(u8 *buffer)
{
    u8 *ptr = buffer;

    // Call delegation method
    ptr += willSaveToBuffer(ptr);

    // Save internal state of all subcomponents
    for (C64Component *c : subComponents) {
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
C64Component::powerOn()
{
    for (auto c : subComponents) { c->powerOn(); }
    _powerOn();
}

void
C64Component::powerOff()
{
    _powerOff();
    for (auto c : subComponents) { c->powerOff(); }
}

void
C64Component::run()
{
    for (auto c : subComponents) { c->run(); }
    _run();
}

void
C64Component::pause()
{
    _pause();
    for (auto c : subComponents) { c->pause(); }
}

void
C64Component::warpOn()
{
    for (auto c : subComponents) { c->warpOn(); }
    _warpOn();
}

void
C64Component::warpOff()
{
    _warpOff();
    for (auto c : subComponents) { c->warpOff(); }
}

void
C64Component::debugOn()
{
    debugMode = true;
    for (auto c : subComponents) { c->debugOn(); }
    _debugOn();
}

void
C64Component::debugOff()
{
    debugMode = false;
    _debugOff();
    for (auto c : subComponents) { c->debugOff(); }
}

void
C64Component::inspect()
{
    // Inspect all subcomponents
    for (C64Component *c : subComponents) {
        c->inspect();
    }
    
    // Inspect this component
    _inspect();
}

void C64Component::dump(dump::Category category, std::ostream& ss) const
{
    _dump(category, ss);
}

void
C64Component::dump(dump::Category category) const
{
    dump(category, std::cout);
}

void
C64Component::dump(std::ostream& ss) const
{
    dump((dump::Category)(-1), ss);
}

void
C64Component::dump() const
{
    dump((dump::Category)(-1));
}
