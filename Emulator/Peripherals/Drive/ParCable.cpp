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
ParCable::resetConfig()
{
    setConfigItem(OPT_PAR_CABLE_TYPE, PAR_CABLE_STANDARD);
    setConfigItem(OPT_PAR_CABLE_CONNECT, DRIVE8, true);
    setConfigItem(OPT_PAR_CABLE_CONNECT, DRIVE9, true);
}

i64
ParCable::getConfigItem(Option option) const
{
    switch (option) {
            
        case OPT_PAR_CABLE_TYPE:  return (i64)config.type;

        default:
            assert(false);
            return 0;
    }
}

i64
ParCable::getConfigItem(Option option, long id) const
{
    assert(id == DRIVE8 || id == DRIVE9);
    
    switch (option) {
            
        case OPT_PAR_CABLE_CONNECT:
            
            return id == DRIVE8 ? config.connect8 : config.connect9;

        default:
            assert(false);
            return 0;
    }
}

bool
ParCable::setConfigItem(Option option, i64 value)
{
    switch (option) {
            
        case OPT_PAR_CABLE_TYPE:
            
            if (!ParCableTypeEnum::isValid(value)) {
                throw VC64Error(ERROR_OPT_INV_ARG, ParCableTypeEnum::keyList());
            }
            
            config.type = (ParCableType)value;
            return true;

        default:
            return false;
    }
}

bool
ParCable::setConfigItem(Option option, long id, i64 value)
{
    switch (option) {
            
        case OPT_PAR_CABLE_CONNECT:
        {
            assert(id == DRIVE8 || id == DRIVE9);

            if (id == 8) {
                config.connect8 = value;
            } else {
                config.connect9 = value;
            }
            return true;
        }
        default:
            return false;
    }
}
            
void
ParCable::_dump(dump::Category category, std::ostream& os) const
{
    using namespace util;

    if (category & dump::Config) {

        os << tab("Type");
        os << ParCableTypeEnum::key(config.type) << std::endl;
    }

    if (category & dump::State) {
        
    }
}

u8
ParCable::getValue()
{
    u8 result = getCIA();
    
    if (config.connect8) result &= getVIA(DRIVE8);
    if (config.connect9) result &= getVIA(DRIVE9);

    return result;
}

u8
ParCable::getValue(DriveID id)
{
    if (id == DRIVE8 && !config.connect8) return getVIA(id);
    if (id == DRIVE9 && !config.connect9) return getVIA(id);

    u8 result = getCIA();
    
    if (config.connect8) result &= getVIA(DRIVE8);
    if (config.connect9) result &= getVIA(DRIVE9);

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
ParCable::getCIA()
{
    u8 ciaprb = cia2.portBinternal();
    u8 ciaddr = cia2.getDDRB();

    return (ciaprb & ciaddr) | (0xFF & ~ciaddr);
}

u8
ParCable::getVIA(DriveID id)
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
