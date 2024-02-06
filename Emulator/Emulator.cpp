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
    subComponents = std::vector<CoreComponent *> { &c64 };

    // trace(RUN_DEBUG, "Creating emulator\n");
}

Emulator::~Emulator()
{
    // debug(RUN_DEBUG, "Destroying emulator\n");
    // if (thread.joinable()) { c64.halt(); }
}

bool Emulator::isPoweredOff() const { return c64.isPoweredOff(); }
bool Emulator::isPoweredOn() const { return c64.isPoweredOn(); }
bool Emulator::isPaused() const { return c64.isPaused(); }
bool Emulator::isRunning() const { return c64.isRunning(); }
bool Emulator::isSuspended() const { return c64.isSuspended(); }
bool Emulator::isHalted() const { return c64.isHalted(); }

void Emulator::suspend() { return c64.suspend(); }
void Emulator::resume() { return c64.resume(); }
}
