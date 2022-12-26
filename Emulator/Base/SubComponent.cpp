// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "SubComponent.h"
#include "C64.h"

namespace vc64 {

SubComponent::SubComponent(C64& ref) :

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
host(ref.host),
iec(ref.iec),
keyboard(ref.keyboard),
mem(ref.mem),
msgQueue(ref.msgQueue),
oscillator(ref.supply),
parCable(ref.parCable),
recorder(ref.recorder),
regressionTester(ref.regressionTester),
retroShell(ref.retroShell),
muxer(ref.muxer),
vic(ref.vic)
{
};

bool
SubComponent::isPoweredOff() const
{
    return c64.isPoweredOff();
}

bool
SubComponent::isPoweredOn() const
{
    return c64.isPoweredOn();
}

bool
SubComponent::isPaused() const
{
    return c64.isPaused();
}

bool
SubComponent::isRunning() const
{
    return c64.isRunning();
}

bool
SubComponent::isSuspended() const
{
    return c64.isSuspended();
}

bool
SubComponent::isHalted() const
{
    return c64.isHalted();
}

void
SubComponent::suspend()
{
    c64.suspend();
}

void
SubComponent::resume()
{
    c64.resume();
}

void
SubComponent::prefix() const
{
    c64.prefix();
}

}
