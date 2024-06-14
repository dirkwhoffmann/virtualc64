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
#include "CoreComponent.h"
#include "Emulator.h"
#include "Checksum.h"
#include "Option.h"

namespace vc64 {

const char *
CoreComponent::objectName() const
{
    assert(isize(getDescriptions().size()) > objid);
    return getDescriptions().at(objid).name;
}

const char *
CoreComponent::description() const
{
    assert(isize(getDescriptions().size()) > objid);
    return getDescriptions().at(objid).description;
}

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

        warn("Initialization aborted: %s\n", e.what());
    }
}

void 
CoreComponent::hardReset()
{
    // Start over from a zeroed-out state
    Serializable::hardReset();

    // Let all components perform their specific actions
    reset(true);
}

void 
CoreComponent::softReset()
{
    // Start over from a zeroed-out state
    Serializable::softReset();

    // Let all components perform their specific actions
    reset(false);
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
    for (CoreComponent *c : subComponents) { c->resetConfig(); }
    Configurable::resetConfig(emulator.defaults, objid);
}

void
CoreComponent::routeOption(Option opt, std::vector<Configurable *> &result)
{
    for (auto &o : getOptions()) {
        if (o == opt) result.push_back(this);
    }
    for (auto &c : subComponents) {
        c->routeOption(opt, result);
    }
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

void
CoreComponent::focus()
{
    for (auto c : subComponents) { c->focus(); }
    _focus();
}

void
CoreComponent::unfocus()
{
    for (auto c : subComponents) { c->unfocus(); }
    _unfocus();
}

bool
CoreComponent::isEmulatorThread() const
{
    return emulator.isEmulatorThread();
}

void CoreComponent::exportConfig(std::ostream& ss, bool diff) const
{
    bool first = true;

    for (auto &opt: getOptions()) {

        auto current = getOption(opt);
        auto fallback = getFallback(opt);

        if (!diff || current != fallback) {

            if (first) {

                ss << "# " << description() << std::endl << std::endl;
                first = false;
            }

            auto cmd = "try " +Interpreter::shellName(*this);
            auto currentStr = OptionParser::asPlainString(opt, current);
            auto fallbackStr = OptionParser::asPlainString(opt, fallback);

            string line = cmd + " set " + OptionEnum::plainkey(opt) + " " + currentStr;
            string comment = diff ? fallbackStr : OptionEnum::help(opt);

            ss << std::setw(40) << std::left << line << " # " << comment << std::endl;
        }
    }

    if (!first) ss << std::endl;

    for (auto &sub: subComponents) {

        sub->exportConfig(ss, diff);
    }
}

}
