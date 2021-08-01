// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "ParCable.h"
#include "C64.h"
#include "IO.h"

ParCable::ParCable(C64& ref) : SubComponent(ref)
{
};

const char *
ParCable::getDescription() const
{
    return "ParCable";
}

void
ParCable::_reset(bool hard)
{
    RESET_SNAPSHOT_ITEMS(hard)
}
            
void
ParCable::_dump(dump::Category category, std::ostream& os) const
{
    using namespace util;

    if (category & dump::State) {
        
        os << tab("Cable value");
        os << hex(getValue()) << std::endl;
    }
}

u8
ParCable::getValue() const
{
    u8 result = getCIA();
    
    switch (drive8.getParCableType()) {
            
        case PAR_CABLE_STANDARD: result &= getVIA(drive8); break;
        case PAR_CABLE_DOLPHIN3: result &= getPIA(drive8); break;
        default: break;
    }
    switch (drive9.getParCableType()) {
            
        case PAR_CABLE_STANDARD: result &= getVIA(drive9); break;
        case PAR_CABLE_DOLPHIN3: result &= getPIA(drive9); break;
        default: break;
    }

    return result;
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
    trace(PAR_DEBUG, "c64Handshake(%s)\n", drive.getDescription());
    
    switch (drive.getParCableType()) {
            
        case PAR_CABLE_STANDARD:
            
            drive.via1.setInterruptFlag_CB1();
            break;
            
        case PAR_CABLE_DOLPHIN3:
            
            drive.pia.pulseCA1External();
            break;
            
        default:
            break;
    }
}

u8
ParCable::getCIA() const
{
    u8 ciaprb = cia2.portBinternal();
    u8 ciaddr = cia2.getDDRB();

    return (ciaprb & ciaddr) | (0xFF & ~ciaddr);
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
