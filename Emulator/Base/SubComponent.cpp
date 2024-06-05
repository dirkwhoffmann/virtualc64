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
#include "SubComponent.h"
#include "Emulator.h"

namespace vc64 {

References::References(C64& ref) :

audioPort(ref.audioPort),
c64(ref),
cia1(ref.cia1),
cia2(ref.cia2),
port1(ref.port1),
port2(ref.port2),
cpu(ref.cpu),
datasette(ref.datasette),
debugger(ref.debugger),
drive8(ref.drive8),
drive9(ref.drive9),
expansionPort(ref.expansionport),
host(ref.emulator.host),
serialPort(ref.iec),
keyboard(ref.keyboard),
mem(ref.mem),
monitor(ref.monitor),
msgQueue(ref.msgQueue),
parCable(ref.parCable),
powerSupply(ref.supply),
recorder(ref.recorder),
regressionTester(ref.regressionTester),
retroShell(ref.retroShell),
sidBridge(ref.sidBridge),
sid0(ref.sidBridge.sid[0]),
sid1(ref.sidBridge.sid[1]),
sid2(ref.sidBridge.sid[2]),
sid3(ref.sidBridge.sid[3]),
vic(ref.vic),
videoPort(ref.videoPort)
{

};

SubComponent::SubComponent(C64& ref) : CoreComponent(ref.emulator), References(ref) { };
SubComponent::SubComponent(C64& ref, isize id) : CoreComponent(ref.emulator, id), References(ref) { };

void
SubComponent::prefix() const
{
    c64.prefix();
}

void 
SubComponent::markAsDirty() 
{
    c64.markAsDirty();
}

}
