// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// This FILE is dual-licensed. You are free to choose between:
//
//     - The GNU General Public License v3 (or any later version)
//     - The Mozilla Public License v2
//
// SPDX-License-Identifier: GPL-3.0-or-later OR MPL-2.0
// -----------------------------------------------------------------------------

#include "config.h"
#include "Emulator.h"
#include "CoreComponent.h"
#include "Checksum.h"

namespace vc64 {

void
CoreComponent::initialize()
{
    assert(!isRunning());

    try {

        for (CoreComponent *c : subComponents) { c->initialize(); }
        _initialize();

    } catch (std::exception &e) {

        warn("Failed to initialize: %s\n", e.what());
        fatalError;
    }
}

void
CoreComponent::reset(bool hard)
{
    if (hard) assert(!isRunning());

    for (CoreComponent *c : subComponents) { c->reset(hard); }
    _reset(hard);
}

void
CoreComponent::inspect() const
{
    for (CoreComponent *c : subComponents) { c->inspect(); }
    _inspect();
}

isize
CoreComponent::size()
{
    isize result = _size();

    // Add 8 bytes for the checksum
    result += 8;

    for (CoreComponent *c : subComponents) { result += c->size(); }
    return result;
}

u64
CoreComponent::checksum()
{
    u64 result = _checksum();

    // Compute checksums for all subcomponents
    for (CoreComponent *c : subComponents) {
        result = util::fnvIt64(result, c->checksum());
    }

    return result;
}

isize
CoreComponent::load(const u8 *buffer)
{
    assert(!isRunning());

    const u8 *ptr = buffer;

    // Call the delegate
    ptr += willLoadFromBuffer(ptr);

    // Load internal state of all subcomponents
    for (CoreComponent *c : subComponents) {
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

        debug(SNP_DEBUG, "Corrupted snapshot detected\n");
        throw VC64Error(ERROR_SNAP_CORRUPTED);
    }
    
    debug(SNP_DEBUG, "Loaded %ld bytes (expected %ld)\n", result, size());
    return result;
}

void
CoreComponent::didLoad()
{
    assert(!isRunning());

    for (CoreComponent *c : subComponents) {
        c->didLoad();
    }

    _didLoad();
}

isize
CoreComponent::save(u8 *buffer)
{
    u8 *ptr = buffer;
    
    // Call the delegate
    ptr += willSaveToBuffer(ptr);

    // Save internal state of all subcomponents
    for (CoreComponent *c : subComponents) {
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
CoreComponent::didSave()
{
    for (CoreComponent *c : subComponents) {
        c->didSave();
    }

    _didSave();
}

void
CoreComponent::isReady() const
{
    for (auto c : subComponents) { c->isReady(); }
    _isReady();
}

bool
CoreComponent::isPoweredOff() const
{
    return emulator.isPoweredOff();
}

bool
CoreComponent::isPoweredOn() const
{
    return emulator.isPoweredOn();
}

bool
CoreComponent::isPaused() const
{
    return emulator.isPaused();
}

bool
CoreComponent::isRunning() const
{
    return emulator.isRunning();
}

bool
CoreComponent::isSuspended() const
{
    return emulator.isSuspended();
}

bool
CoreComponent::isHalted() const
{
    return emulator.isHalted();
}

void 
CoreComponent::suspend() 
{
    return emulator.suspend();
}

void
CoreComponent::resume() 
{
    return emulator.resume();
}

void
CoreComponent::powerOn()
{
    for (auto c : subComponents) { c->powerOn(); }
    _powerOn();
}

void
CoreComponent::powerOff()
{
    for (auto c : subComponents) { c->powerOff(); }
    _powerOff();
}

void
CoreComponent::run()
{
    for (auto c : subComponents) { c->run(); }
    _run();
}

void
CoreComponent::pause()
{
    for (auto c : subComponents) { c->pause(); }
    _pause();
}

void
CoreComponent::halt()
{
    for (auto c : subComponents) { c->halt(); }
    _halt();
}

void
CoreComponent::warpOn()
{
    for (auto c : subComponents) { c->warpOn(); }
    _warpOn();
}

void
CoreComponent::warpOff()
{
    for (auto c : subComponents) { c->warpOff(); }
    _warpOff();
}

void
CoreComponent::trackOn()
{    
    for (auto c : subComponents) { c->trackOn(); }
    _trackOn();
}

void
CoreComponent::trackOff()
{
    for (auto c : subComponents) { c->trackOff(); }
    _trackOff();
}

}
