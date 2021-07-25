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

ParCable::ParCable(C64& ref) : C64Component(ref)
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
        
        os << tab("Value on C64 side");
        os << hex(getValue()) << std::endl;
        os << tab("Value on Drive 8 side");
        os << hex(getValue(DRIVE8)) << std::endl;
        os << tab("Value on Drive 9 side");
        os << hex(getValue(DRIVE9)) << std::endl;
    }
}

u8
ParCable::getValue() const
{
    u8 result = getCIA();
        
    if (drive8.hasParCable()) result &= getVIA(DRIVE8);
    if (drive9.hasParCable()) result &= getVIA(DRIVE9);

    return result;
}

u8
ParCable::getValue(DriveID id) const
{
    if (id == DRIVE8 && !drive8.hasParCable()) return getVIA(id);
    if (id == DRIVE9 && !drive9.hasParCable()) return getVIA(id);

    u8 result = getCIA();
    
    if (drive8.hasParCable()) result &= getVIA(DRIVE8);
    if (drive9.hasParCable()) result &= getVIA(DRIVE9);

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
    trace(PAR_DEBUG, "c64Handshake()\n");
    drive8.via1.setInterruptFlag_CB1();
}

u8
ParCable::getCIA() const
{
    u8 ciaprb = cia2.portBinternal();
    u8 ciaddr = cia2.getDDRB();

    return (ciaprb & ciaddr) | (0xFF & ~ciaddr);
}

u8
ParCable::getVIA(DriveID id) const
{
    assert(id == DRIVE8 || id == DRIVE9);
    
    if (id == DRIVE8) {
        
        u8 viapra = drive8.via1.portAinternal();
        u8 viaddr = drive8.via1.getDDRA();
        
        return (viapra & viaddr) | (0xFF & ~viaddr);

    } else {
        
        u8 viapra = drive9.via1.portAinternal();
        u8 viaddr = drive9.via1.getDDRA();
        
        return (viapra & viaddr) | (0xFF & ~viaddr);
    }
}
