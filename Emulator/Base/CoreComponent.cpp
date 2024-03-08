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
#include "Option.h"

namespace vc64 {

bool 
CoreComponent::operator== (CoreComponent &other)
{
    return checksum() == other.checksum();
}

void
CoreComponent::initialize()
{
    assert(!isRunning());

    try {

        for (CoreComponent *c : subComponents) { c->initialize(); }
        _initialize();

    } catch (std::exception &e) {

        fatal("Failed to initialize: %s\n", e.what());
    }
}

void 
CoreComponent::reset(bool hard)
{
    try {

        for (CoreComponent *c : subComponents) { c->reset(hard); }
        _reset(hard);

    } catch (std::exception &e) {

        fatal("Failed to reset: %s\n", e.what());
    }
}

i64 
CoreComponent::getFallback(Option opt) const
{
    return emulator.defaults.get(opt);
}

void
CoreComponent::resetConfig()
{
    Configurable::resetConfig(emulator.defaults);
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

bool 
CoreComponent::isEmulatorThread() const
{
    return emulator.isEmulatorThread();
}

void
CoreComponent::dumpDiff(std::ostream& ss) const
{
    using namespace util;

    bool first = true;

    for (auto &opt: getOptions()) {

        auto current = getOption(opt);
        auto fallback = getFallback(opt);

        if (current != fallback) {

            if (first) {
            
                ss << std::endl << getDescription() << ":" << std::endl << std::endl;
                first = false;
            }

            auto arg1 = OptionParser::create(opt, current)->asString();
            auto arg2 = OptionParser::create(opt, fallback)->asString();
            ss << tab(OptionEnum::key(opt)) << arg1 << " [" << arg2 << "]" << std::endl;
        }
    }

    for (auto &sub: subComponents) {

        sub->dumpDiff(ss);
    }
}

}
