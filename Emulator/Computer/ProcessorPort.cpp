// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "C64.h"

ProcessorPort::ProcessorPort(C64 &ref) : C64Component(ref)
{
    setDescription("ProcessorPort");
    
    // Register snapshot items
    SnapshotItem items[] = {
        
        { &port,               sizeof(port),               CLEAR_ON_RESET },
        { &direction,          sizeof(direction),          CLEAR_ON_RESET },
        { &dischargeCycleBit3, sizeof(dischargeCycleBit3), CLEAR_ON_RESET },
        { &dischargeCycleBit6, sizeof(dischargeCycleBit6), CLEAR_ON_RESET },
        { &dischargeCycleBit7, sizeof(dischargeCycleBit7), CLEAR_ON_RESET },
        { NULL,                0,                          0 }};
    
    registerSnapshotItems(items, sizeof(items));
}

ProcessorPort::~ProcessorPort()
{
}

void
ProcessorPort::dump()
{
    msg("Processor port:\n");
    msg("---------------\n\n");
    msg("port:           %02X\n", port);
    msg("direction:      %02X\n", direction);
    msg("Bit 3 discharge cycle: %ld\n", dischargeCycleBit3);
    msg("Bit 6 discharge cycle: %ld\n", dischargeCycleBit6);
    msg("Bit 7 discharge cycle: %ld\n", dischargeCycleBit7);
}
            
u8
ProcessorPort::read()
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
ProcessorPort::readDirection()
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
    
    // When writing to the port register, the last VIC byte appears in 0x0001
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
    
    // 1) If bits 3, 6, and 7 are configured as outputs, they are not floating
    if (GET_BIT(value, 3)) dischargeCycleBit3 = 0;
    if (GET_BIT(value, 6)) dischargeCycleBit6 = 0;
    if (GET_BIT(value, 7)) dischargeCycleBit7 = 0;

    // 2) If bits 3, 6, and 7 change from output to input, they become floating
    if (FALLING_EDGE_BIT(direction, value, 3) && GET_BIT(port, 3) != 0)
        dischargeCycleBit3 = UINT64_MAX;
    if (FALLING_EDGE_BIT(direction, value, 6) && GET_BIT(port, 6) != 0)
        dischargeCycleBit6 = cpu.cycle + dischargeCycles;
    if (FALLING_EDGE_BIT(direction, value, 7) && GET_BIT(port, 7) != 0)
        dischargeCycleBit7 = cpu.cycle + dischargeCycles;
    
    direction = value;
    
    // When writing to the direction register, the last VIC byte appears
    mem.ram[0x0000] = vic.getDataBusPhi1();
    
    // Switch memory banks
    mem.updatePeekPokeLookupTables();
}


