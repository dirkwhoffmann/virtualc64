// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "ProcessorPort.h"
#include "C64.h"

void
ProcessorPort::_reset(bool hard)
{
    RESET_SNAPSHOT_ITEMS(hard)
    
    port = 0xFF;
    
    mem.updatePeekPokeLookupTables();
};

void
ProcessorPort::_dump(dump::Category category, std::ostream& os) const
{
    using namespace util;
    
    if (category & dump::State) {
        
        os << tab("Port");
        os << hex(port) << std::endl;
        os << tab("Direction");
        os << hex(direction) << std::endl;
        os << tab("Bit 3 discharge cycle");
        os << hex(dischargeCycleBit3) << std::endl;
        os << tab("Bit 6 discharge cycle");
        os << hex(dischargeCycleBit6) << std::endl;
        os << tab("Bit 7 discharge cycle");
        os << hex(dischargeCycleBit7) << std::endl;
    }
}
            
u8
ProcessorPort::read() const
{
    // If the port bits are configured as inputs and no datasette is attached,
    // the following values are returned:
    //
    //     Bit 0:  1 (bit is driven by a pull-up resistor)
    //     Bit 1:  1 (bit is driven by a pull-up resistor)
    //     Bit 2:  1 (bit is driven by a pull-up resistor)
    //     ??? Bit 3:  Eventually 0 (acts a a capacitor)
    //     Bit 3:  0 (bit is driven by a pull-down resistor)
    //     Bit 4:  1 (bit is driven by a pull-up resistor)
    //     Bit 5:  0 (bit is driven by a pull-down resistor)
    //     Bit 6:  Eventually 0 (acts a a capacitor)
    //     Bit 7:  Eventually 0 (acts a a capacitor)
    //
    //     In reality, discharging times for bits 3, 6, and 7 depend on both
    //     CPU temperature and how long the output was 1 befor the bit became
    //     an input.
    
    u8 bit3 = (dischargeCycleBit3 > cpu.cycle) ? 0x08 : 0x00;
    u8 bit6 = (dischargeCycleBit6 > cpu.cycle) ? 0x40 : 0x00;
    u8 bit7 = (dischargeCycleBit7 > cpu.cycle) ? 0x80 : 0x00;
    u8 bit4 = datasette.getPlayKey() ? 0x00 : 0x10;
    u8 bits = bit7 | bit6 | bit4 | bit3 | 0x07;

    return (port & direction) | (bits & ~direction);
}

u8
ProcessorPort::readDirection() const
{
    return direction;
}

void
ProcessorPort::write(u8 value)
{    
    port = value;
    
    // Check for datasette motor bit
    if (direction & 0x20) {
        datasette.setMotor((value & 0x20) == 0);
    }
    
    // When writing to the port register, the last VICII byte appears in 0x0001
    mem.ram[0x0001] = vic.getDataBusPhi1();
    
    // Switch memory banks
    mem.updatePeekPokeLookupTables();
}

void
ProcessorPort::writeDirection(u8 value)
{
    u64 dischargeCycles = 350000; // VICE value
    // u64 dischargeCycles = 246312; // Hoxs64 value

    // Check floating status of bits 3, 6, and 7.
    
    // If bits 3, 6, and 7 are configured as outputs, they are not floating
    if (GET_BIT(value, 3)) dischargeCycleBit3 = 0;
    if (GET_BIT(value, 6)) dischargeCycleBit6 = 0;
    if (GET_BIT(value, 7)) dischargeCycleBit7 = 0;

    // If bits 3, 6, and 7 change from output to input, they become floating
    if (FALLING_EDGE_BIT(direction, value, 3) && GET_BIT(port, 3) != 0)
        dischargeCycleBit3 = UINT64_MAX;
    if (FALLING_EDGE_BIT(direction, value, 6) && GET_BIT(port, 6) != 0)
        dischargeCycleBit6 = cpu.cycle + dischargeCycles;
    if (FALLING_EDGE_BIT(direction, value, 7) && GET_BIT(port, 7) != 0)
        dischargeCycleBit7 = cpu.cycle + dischargeCycles;
    
    direction = value;
    
    // When writing to the direction register, the last VICII byte appears
    mem.ram[0x0000] = vic.getDataBusPhi1();
    
    // Switch memory banks
    mem.updatePeekPokeLookupTables();
}
