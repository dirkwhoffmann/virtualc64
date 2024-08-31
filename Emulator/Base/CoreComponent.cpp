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

bool
CoreComponent::operator== (CoreComponent &other)
{
    return checksum(true) == other.checksum(true);
}

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

const char *
CoreComponent::shellName() const
{
    assert(isize(getDescriptions().size()) > objid);
    return getDescriptions().at(objid).shell;
}

bool
CoreComponent::isInitialized() const
{
    return emulator.isInitialized();
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
CoreComponent::isReady() const
{
    for (auto c : subComponents) { c->isReady(); }
    _isReady();
}

u64
CoreComponent::checksum(bool recursive)
{
    SerChecker checker;

    // Compute a checksum for the members of this component
    *this << checker;

    // Incoorporate subcomponents if requested
    if (recursive) for (auto &c : subComponents) checker << c->checksum(recursive);

    return checker.hash;
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
CoreComponent::resetConfig()
{
    for (CoreComponent *c : subComponents) { c->resetConfig(); }
    Configurable::resetConfig(emulator.defaults, objid);
}

i64
CoreComponent::getFallback(Option opt) const
{
    return emulator.defaults.get(opt);
}

Configurable *
CoreComponent::routeOption(Option opt, isize objid)
{
    if (this->objid == objid) {
        for (auto &o : getOptions()) if (o == opt) return this;
    }
    for (auto &c : subComponents) {
        if (auto result = c->routeOption(opt, objid); result) return result;
    }

    return nullptr;
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
CoreComponent::reset(bool hard)
{
    SerResetter resetter(hard);

    {   SUSPENDED

        // Call the pre-reset delegate
        postorderWalk([hard](CoreComponent *c) { c->_willReset(hard); });

        // Revert to a clean state
        postorderWalk([&resetter](CoreComponent *c) { *c << resetter; });

        // Call the post-reset delegate
        postorderWalk([hard](CoreComponent *c) { c->_didReset(hard); });
    }
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

isize
CoreComponent::size(bool recursive)
{
    SerCounter counter;
    *this << counter;
    isize result = counter.count;

    // Add 8 bytes for the checksum
    result += 8;

    // Add size of subcomponents if requested
    if (recursive) for (CoreComponent *c : subComponents) { result += c->size(); }

    return result;
}

isize
CoreComponent::load(const u8 *buffer)
{
    assert(!isRunning());

    isize result = 0;

    postorderWalk([this, buffer, &result](CoreComponent *c) {

        const u8 *ptr = buffer + result;

        // Load the checksum for this component
        auto hash = read64(ptr);

        // Load the internal state of this component
        SerReader reader(ptr); *c << reader;

        // Determine the number of loaded bytes
        isize count = (isize)(reader.ptr - (buffer + result));

        // Check integrity
        if (hash != c->checksum(false) || FORCE_SNAP_CORRUPTED) {
            if (SNP_DEBUG) { fatalError; } else { throw Error(VC64ERROR_SNAP_CORRUPTED); }
        }

        debug(SNP_DEBUG, "Loaded %ld bytes (expected %ld)\n", count, c->size(false));
        result += count;
    });

    postorderWalk([](CoreComponent *c) { c->_didLoad(); });

    return result;
}

isize
CoreComponent::save(u8 *buffer)
{
    isize result = 0;

    postorderWalk([this, buffer, &result](CoreComponent *c) {

        u8 *ptr = buffer + result;

        // Save the checksum for this component
        write64(ptr, c->checksum(false));

        // Save the internal state of this component
        SerWriter writer(ptr); *c << writer;

        // Determine the number of written bytes
        isize count = (isize)(writer.ptr - (buffer + result));

        // Check integrity
        if (count != c->size(false) || FORCE_SNAP_CORRUPTED) {
            if (SNP_DEBUG) { fatalError; } else { throw Error(VC64ERROR_SNAP_CORRUPTED); }
        }

        debug(SNP_DEBUG, "Saved %ld bytes (expected %ld)\n", count, c->size(false));
        result += count;
    });

    postorderWalk([](CoreComponent *c) { c->_didSave(); });

    return result;
}

std::vector<CoreComponent *>
CoreComponent::collectComponents()
{
    std::vector<CoreComponent *> result;
    collectComponents(result);
    return result;
}

void
CoreComponent::collectComponents(std::vector<CoreComponent *> &result)
{
    result.push_back(this);
    for (auto &c : subComponents) c->collectComponents(result);
}

void
CoreComponent::preoderWalk(std::function<void(CoreComponent *)> func)
{
    func(this);
    for (auto &c : subComponents) c->preoderWalk(func);
}

void
CoreComponent::postorderWalk(std::function<void(CoreComponent *)> func)
{
    for (auto &c : subComponents) c->postorderWalk(func);
    func(this);
}

bool
CoreComponent::isEmulatorThread() const
{
    return emulator.isEmulatorThread();
}

void
CoreComponent::diff(CoreComponent &other)
{
    auto num = subComponents.size();
    assert(num == other.subComponents.size());

    // Compare all subcomponents
    for (usize i = 0; i < num; i++) {
        subComponents[i]->diff(*other.subComponents[i]);
    }

    // Compare this component
    if (auto check1 = checksum(false), check2 = other.checksum(false); check1 != check2) {
        msg("Checksum mismatch: %llx != %llx\n", check1, check2);
    }
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

            auto cmd = "try " + string(shellName());
            auto currentStr = OptionParser::asPlainString(opt, current);
            auto fallbackStr = OptionParser::asPlainString(opt, fallback);

            string line = cmd + " set " + OptionEnum::key(opt) + " " + currentStr;
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
