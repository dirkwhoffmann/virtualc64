// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "C64.h"

C64Component::C64Component(C64& ref) :

vc64(ref),
cpu(ref.cpu),
pport(ref.processorPort),
vic(ref.vic),
cia1(ref.cia1),
cia2(ref.cia2),
sid(ref.sid),
keyboard(ref.keyboard),
port1(ref.port1),
port2(ref.port2),
expansionport(ref.expansionport),
iec(ref.iec),
drive1(ref.drive1),
drive2(ref.drive2),
datasette(ref.datasette),
mouse(ref.mouse)
{
};

void
C64Component::prefix()
{
    vc64.prefix();
}
