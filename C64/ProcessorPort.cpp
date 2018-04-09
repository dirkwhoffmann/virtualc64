//
//  ProcessorPort.cpp
//  VirtualC64
//
//  Created by Dirk Hoffmann on 17.03.18.
//

#include "C64.h"

ProcessorPort::ProcessorPort()
{
    setDescription("ProcessorPort");
    debug(3, "Creating processor port at address %p...\n", this);
    
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

/*
void
ProcessorPort::reset()
{
    VirtualComponent::reset();
}
*/

void
ProcessorPort::dumpState()
{
    msg("Processor port:\n");
    msg("---------------\n\n");
    msg("port:           %02X\n", port);
    msg("direction:      %02X\n", direction);
    msg("Bit 3 discharge cycle: %ld\n", dischargeCycleBit3);
    msg("Bit 6 discharge cycle: %ld\n", dischargeCycleBit6);
    msg("Bit 7 discharge cycle: %ld\n", dischargeCycleBit7);
}
            
uint8_t
ProcessorPort::read()
{
    // If the port bits are configured as inputs and no datasette is attached, the
    // following values are returned:
    //
    //     Bit 0:  1 (bit is driven by a pull-up resistor)
    //     Bit 1:  1 (bit is driven by a pull-up resistor)
    //     Bit 2:  1 (bit is driven by a pull-up resistor)
    //     Bit 3:  Eventually 0 (acts a a capacitor)
    //     Bit 4:  1 (bit is driven by a pull-up resistor)
    //     Bit 5:  0 (bit is driven by a pull-down resistor)
    //     Bit 6:  Eventually 0 (acts a a capacitor)
    //     Bit 7:  Eventually 0 (acts a a capacitor)
    //
    //     In reality, discharging times for bits 3, 6, and 7 depend on both CPU temperature
    //     and how long the output was 1 befor the bit became an input.
    
    uint8_t bit3 = (dischargeCycleBit3 > c64->getCycles()) ? 0x08 : 0x00;
    uint8_t bit6 = (dischargeCycleBit6 > c64->getCycles()) ? 0x40 : 0x00;
    uint8_t bit7 = (dischargeCycleBit7 > c64->getCycles()) ? 0x80 : 0x00;
    uint8_t bit4 = c64->datasette.getPlayKey() ? 0x00 : 0x10;
    uint8_t bits = bit7 | bit6 | bit4 | bit3 | 0x07;
    
    return (port & direction) | (bits & ~direction);
}

uint8_t
ProcessorPort::readDirection()
{
    return direction;
}

void
ProcessorPort::write(uint8_t value)
{
    port = value;
    
    // Check for datasette motor bit
    if (direction & 0x20) {
        c64->datasette.setMotor((value & 0x20) == 0);
    }
    
    // When writing to the port register, the last VIC byte appears in 0x0001
    c64->mem.ram[0x0001] = c64->vic.prevDataBus;
    
    // Switch memory banks
    c64->mem.updatePeekPokeLookupTables();
}

void
ProcessorPort::writeDirection(uint8_t value)
{
    // Check floating status of bits 3, 6, and 7.
    
    // 1) If bits 3, 6, and 7 are configured as outputs, they are not floating
    if (GET_BIT(value, 3)) dischargeCycleBit3 = 0;
    if (GET_BIT(value, 6)) dischargeCycleBit6 = 0;
    if (GET_BIT(value, 7)) dischargeCycleBit7 = 0;

    // 2) If bits 3, 6, and 7 change from output to input, they become floating
    if (FALLING_EDGE_BIT(direction, value, 3) && GET_BIT(port, 3) != 0)
        dischargeCycleBit3 = c64->getCycles() + PAL_CYCLES_PER_SECOND;
    if (FALLING_EDGE_BIT(direction, value, 6) && GET_BIT(port, 6) != 0)
        dischargeCycleBit6 = c64->getCycles() + 360000;
    if (FALLING_EDGE_BIT(direction, value, 7) && GET_BIT(port, 7) != 0)
        dischargeCycleBit7 = c64->getCycles() + 360000;
    
    direction = value;
    
    // When writing to the direction register, the last VIC byte appears in 0x0000
    c64->mem.ram[0x0000] = c64->vic.prevDataBus;
    
    // Switch memory banks
    c64->mem.updatePeekPokeLookupTables();
}


