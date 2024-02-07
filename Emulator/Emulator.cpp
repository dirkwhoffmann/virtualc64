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

namespace vc64 {

Emulator::Emulator()
{
    // subComponents = std::vector<CoreComponent *> { &c64 };

    // Initialize the sync timer
    targetTime = util::Time::now();

    // trace(RUN_DEBUG, "Creating emulator\n");
}

Emulator::~Emulator()
{
    debug(RUN_DEBUG, "Destroying emulator\n");
    if (thread.joinable()) { halt(); }
}

void
Emulator::launch(const void *listener, Callback *func)
{
    c64.msgQueue.setListener(listener, func);

    launch();
}

void
Emulator::launch()
{
    // Make sure to call this function only once
    assert(!thread.joinable());

    // Start the thread and enter the main function
    thread = std::thread(&Thread::main, this);
}

void 
Emulator::readyToGo()
{
    c64.isReady();
}

SyncMode
Emulator::getSyncMode() const
{
    return c64.getSyncMode();
}

void 
Emulator::execute()
{
    c64.execute();
}

double 
Emulator::refreshRate() const
{
    return c64.refreshRate();
}

isize 
Emulator::slicesPerFrame() const
{
    return c64.slicesPerFrame();
}

util::Time 
Emulator::wakeupPeriod() const
{
    return c64.wakeupPeriod();
}

void Emulator::powerOnDelegate() { c64.powerOn(); }
void Emulator::powerOffDelegate() { c64.powerOff(); }
void Emulator::runDelegate() { c64.run(); }
void Emulator::pauseDelegate() { c64.pause(); }
void Emulator::haltDelegate() { c64.halt(); }
void Emulator::warpOnDelegate() { c64.warpOn(); }
void Emulator::warpOffDelegate() { c64.warpOff(); }
void Emulator::trackOnDelegate() { c64.trackOn(); }
void Emulator::trackOffDelegate() { c64.trackOff(); }


}
