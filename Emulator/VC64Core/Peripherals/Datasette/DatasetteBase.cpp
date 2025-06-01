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

#include "VirtualC64Config.h"
#include "Datasette.h"
#include "MsgQueue.h"
#include "IOUtils.h"

namespace vc64 {

Datasette::~Datasette()
{
    dealloc();
}

void
Datasette::alloc(isize capacity)
{
    dealloc();

    if (capacity) {
        pulses = new (std::nothrow) Pulse[capacity];
        numPulses = capacity;
    }
}

void
Datasette::dealloc()
{
    if (pulses) {
        delete[] pulses;
        pulses = nullptr;
        numPulses = 0;
    }
}

void
Datasette::_dump(Category category, std::ostream& os) const
{
    using namespace util;

    if (category == Category::Config) {

        dumpConfig(os);
    }

    if (category == Category::State) {

        os << tab("TAP type");
        os << dec(type) << std::endl;
        os << tab("Pulse count");
        os << dec(numPulses) << std::endl;

        os << std::endl;

        os << tab("Head position");
        os << dec(head) << std::endl;
        os << tab("Play key");
        os << bol(playKey, "pressed", "released") << std::endl;
        os << tab("Motor");
        os << bol(motor, "on", "off") << std::endl;
        os << tab("nextRisingEdge");
        os << dec(nextRisingEdge) << std::endl;
        os << tab("nextFallingEdge");
        os << dec(nextFallingEdge) << std::endl;
    }
}

void
Datasette::cacheInfo(DatasetteInfo &result) const
{
    info.hasTape = hasTape();
    info.type = type;
    info.motor = motor;
    info.playKey = playKey;
    info.counter = (isize)counter.asSeconds();
}

Datasette&
Datasette::operator= (const Datasette& other) {

    CLONE(head)
    CLONE(counter.ticks)
    CLONE(playKey)
    CLONE(motor)
    CLONE(nextRisingEdge)
    CLONE(nextFallingEdge)

    CLONE(type)

    assert((pulses == nullptr) == (numPulses == 0));
    assert((other.pulses == nullptr) == (other.numPulses == 0));

    if (numPulses != other.numPulses) {

        // Create a new pulse buffer
        if (pulses) delete[] pulses;
        pulses = nullptr;
        if (other.numPulses) pulses = new Pulse[other.numPulses];
        numPulses = other.numPulses;
    }

    assert((pulses == nullptr) == (numPulses == 0));
    assert(numPulses == other.numPulses);

    // Clone the pulse buffer
    for (isize i = 0; i < numPulses; i++) pulses[i] = other.pulses[i];

    return *this;
}

void
Datasette::operator << (SerCounter &worker)
{
    serialize(worker);

    worker << numPulses;
    for (isize i = 0; i < numPulses; i++) worker << pulses[i].cycles;
}

void
Datasette::operator << (SerReader &worker)
{
    serialize(worker);

    // Free previously allocated memory
    dealloc();

    // Load size
    worker << numPulses;

    // Make sure a corrupted value won't steal all memory
    if (numPulses > 0x8FFFF) { numPulses = 0; }

    // Create a new pulse buffer
    alloc(numPulses);

    // Load pulses from buffer
    for (isize i = 0; i < numPulses; i++) worker << pulses[i].cycles;
}

void
Datasette::operator << (SerWriter &worker)
{
    serialize(worker);

    // Save size
    worker << numPulses;

    // Save pulses to buffer
    for (isize i = 0; i < numPulses; i++) worker << pulses[i].cycles;
}

i64
Datasette::getOption(Opt option) const
{
    switch (option) {

        case Opt::DAT_MODEL:     return (i64)config.model;
        case Opt::DAT_CONNECT:   return (i64)config.connected;

        default:
            fatalError;
    }
}

void
Datasette::checkOption(Opt opt, i64 value)
{
    switch (opt) {

        case Opt::DAT_MODEL:
        case Opt::DAT_CONNECT:

            return;

        default:
            throw AppError(Fault::OPT_UNSUPPORTED);
    }
}

void
Datasette::setOption(Opt opt, i64 value)
{
    checkOption(opt, value);

    switch (opt) {

        case Opt::DAT_MODEL:

            config.model = DatasetteModel(value);
            return;

        case Opt::DAT_CONNECT:

            if (config.connected != bool(value)) {

                config.connected = bool(value);
                updateDatEvent();
                msgQueue.put(Msg::VC1530_CONNECT, value);
            }
            return;

        default:
            return;
    }
}

}
