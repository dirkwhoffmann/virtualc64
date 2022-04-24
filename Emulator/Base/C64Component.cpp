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
#include "Checksum.h"

void
C64Component::initialize()
{
    try {

        for (C64Component *c : subComponents) { c->initialize(); }
        _initialize();

    } catch (std::exception &e) {

        warn("Failed to initialize: %s\n", e.what());
        fatalError;
    }
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

    // Add 8 bytes for the checksum
    result += 8;

    for (C64Component *c : subComponents) { result += c->size(); }
    return result;
}

u64
C64Component::checksum()
{
    u64 result = _checksum();

    // Compute checksums for all subcomponents
    for (C64Component *c : subComponents) {
        result = util::fnvIt64(result, c->checksum());
    }

    return result;
}

isize
C64Component::load(const u8 *buffer)
{
    assert(!isRunning());

    const u8 *ptr = buffer;

    // Call the delegate
    ptr += willLoadFromBuffer(ptr);

    // Load internal state of all subcomponents
    for (C64Component *c : subComponents) {
        ptr += c->load(ptr);
    }

    // Load the checksum for this component
    auto hash = util::read64(ptr);

    // Load internal state of this component
    ptr += _load(ptr);

    // Call the delegate
    ptr += didLoadFromBuffer(ptr);
    isize result = (isize)(ptr - buffer);

    // Check integrity
    if (hash != _checksum() || FORCE_SNAP_CORRUPTED) {
        throw VC64Error(ERROR_SNAP_CORRUPTED);
    }

    trace(SNP_DEBUG, "Loaded %ld bytes (expected %ld)\n", result, size());
    return result;
}

void
C64Component::didLoad()
{
    assert(!isRunning());

    for (C64Component *c : subComponents) {
        c->didLoad();
    }

    _didLoad();
}

isize
C64Component::save(u8 *buffer)
{
    u8 *ptr = buffer;
    
    // Call the delegate
    ptr += willSaveToBuffer(ptr);

    // Save internal state of all subcomponents
    for (C64Component *c : subComponents) {
        ptr += c->save(ptr);
    }

    // Save the checksum for this component
    util::write64(ptr, _checksum());
    
    // Save the internal state of this component
    ptr += _save(ptr);

    // Call the delegate
    ptr += didSaveToBuffer(ptr);
    isize result = (isize)(ptr - buffer);
    
    debug(SNP_DEBUG, "Saved %ld bytes (expected %ld)\n", result, size());
    assert(result == size());

    return result;
}

void
C64Component::didSave()
{
    for (C64Component *c : subComponents) {
        c->didSave();
    }

    _didSave();
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
