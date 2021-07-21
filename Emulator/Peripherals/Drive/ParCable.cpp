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
    /*
    u8 result = drvValue & c64Value;
    
    printf("Drive side: %x C64 side: %x Cable: %x\n", drvValue, c64Value, result);
    return result;
    */
    
    // Values from the C64 side
    u8 ciaprb = cia2.portBinternal();
    u8 ciaddr = cia2.getDDRB();
    
    // Values from the drive side
    u8 viapra = drive8.via1.portAinternal();
    u8 viaddr = drive8.via1.getDDRA();
    
    u8 cia = ((ciaprb & ciaddr) | (0xFF & ~ciaddr));
    u8 via = ((viapra & viaddr) | (0xFF & ~viaddr));
    
    u8 result = cia & via;
    
    printf("Drive: %x [%x] C64: %x [%x] -> %x\n", ciaprb, ciaddr, viapra, viaddr, result);
    return result;
}

u8
ParCable::getValue(DriveID id)
{
    return getValue();
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
