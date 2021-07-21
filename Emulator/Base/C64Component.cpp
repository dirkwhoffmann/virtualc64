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
#include "C64.h"

C64Component::C64Component(C64& ref) :

c64(ref),
cia1(ref.cia1),
cia2(ref.cia2),
port1(ref.port1),
port2(ref.port2),
cpu(ref.cpu),
datasette(ref.datasette),
drive8(ref.drive8),
drive9(ref.drive9),
expansionport(ref.expansionport),
iec(ref.iec),
keyboard(ref.keyboard),
mem(ref.mem),
messageQueue(ref.msgQueue),
oscillator(ref.oscillator),
parCable(ref.parCable),
recorder(ref.recorder),
regressionTester(ref.regressionTester),
retroShell(ref.retroShell),
sid(ref.sid),
vic(ref.vic)
{
};

bool
C64Component::isPoweredOff() const
{
    return c64.isPoweredOff();
}

bool
C64Component::isPoweredOn() const
{
    return c64.isPoweredOn();
}

bool
C64Component::isPaused() const
{
    return c64.isPaused();
}

bool
C64Component::isRunning() const
{
    return c64.isRunning();
}

void
C64Component::suspend()
{
    c64.suspend();
}

void
C64Component::resume()
{
    c64.resume();
}

void
C64Component::prefix() const
{
    c64.prefix();
}
