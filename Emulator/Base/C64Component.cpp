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

void
C64Component::initialize()
{
    for (C64Component *c : subComponents) { c->initialize(); }
    _initialize();
}

void
C64Component::reset(bool hard)
{
    for (C64Component *c : subComponents) { c->reset(hard); }
    _reset(hard);
}

void
C64Component::inspect() const
{
    for (C64Component *c : subComponents) { c->inspect(); }
    _inspect();
}

isize
C64Component::size()
{
    isize result = _size();
    for (C64Component *c : subComponents) { result += c->size(); }
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
    isize result = (isize)(ptr - buffer);
    
    // Verify that the number of written bytes matches the snapshot size
    trace(SNP_DEBUG, "Loaded %zd bytes (expected %zd)\n", result, size());
    assert(result == size());

    return result;
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
    isize result = (isize)(ptr - buffer);
    
    // Verify that the number of written bytes matches the snapshot size
    trace(SNP_DEBUG, "Saved %zd bytes (expected %zd)\n", result, size());
    assert(result == size());

    return result;
}

void
C64Component::isReady() const
{
    for (auto c : subComponents) { c->isReady(); }
    _isReady();
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
    for (auto c : subComponents) { c->powerOff(); }
    _powerOff();
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
    for (auto c : subComponents) { c->pause(); }
    _pause();
}

void
C64Component::halt()
{
    for (auto c : subComponents) { c->halt(); }
    _halt();
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
    for (auto c : subComponents) { c->warpOff(); }
    _warpOff();
}

void
C64Component::debugOn()
{
    for (auto c : subComponents) { c->debugOn(); }
    _debugOn();
}

void
C64Component::debugOff()
{
    for (auto c : subComponents) { c->debugOff(); }
    _debugOff();
}
