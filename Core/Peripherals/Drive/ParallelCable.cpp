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
#include "ParallelCable.h"
#include "C64.h"
#include "IOUtils.h"

namespace vc64 {

ParCable::ParCable(C64& ref) : SubComponent(ref)
{
};

void
ParCable::_dump(Category category, std::ostream &os) const
{
    using namespace util;

    if (category == Category::State) {
        
        os << tab("Cable value");
        os << hex(getValue()) << std::endl;
    }
}

u8
ParCable::getValue() const
{
    u8 result = userPort.getPB(); //  0xFF;

    switch (drive8.getParCableType()) {
            
        case ParCableType::STANDARD: result &= getVIA(drive8); break;
        case ParCableType::DOLPHIN3: result &= getPIA(drive8); break;
        default: break;
    }
    switch (drive9.getParCableType()) {
            
        case ParCableType::STANDARD: result &= getVIA(drive9); break;
        case ParCableType::DOLPHIN3: result &= getPIA(drive9); break;
        default: break;
    }

    return getCIA(result);
}

void
ParCable::driveHandshake()
{
    trace(PAR_DEBUG, "driveHandshake()\n");
    cia2.triggerFallingEdgeOnFlagPin();
}

void
ParCable::c64Handshake()
{
    c64Handshake(drive8);
    c64Handshake(drive9);
}

void
ParCable::c64Handshake(Drive &drive)
{
    trace(PAR_DEBUG, "c64Handshake(%ld)\n", drive.getDeviceNr());
    
    switch (drive.getParCableType()) {
            
        case ParCableType::STANDARD:
            
            drive.via1.setInterruptFlag_CB1();
            break;
            
        case ParCableType::DOLPHIN3:
            
            drive.pia.pulseCA1External();
            break;
            
        default:
            break;
    }
}

u8
ParCable::getCIA(u8 cable) const
{
    u8 ciaprb = cia2.portBinternal();
    u8 ciaddr = cia2.getDDRB();

    return (ciaprb & ciaddr) | (cable & ~ciaddr);
}

u8
ParCable::getVIA(const Drive &drive) const
{
    u8 viapra = drive.via1.portAinternal();
    u8 viaddr = drive.via1.getDDRA();

    return (viapra & viaddr) | (0xFF & ~viaddr);
}

u8
ParCable::getPIA(const Drive &drive) const
{
    u8 viapra = drive.pia.ora;
    u8 viaddr = drive.pia.ddra;
    
    return (viapra & viaddr) | (0xFF & ~viaddr);
}

}
