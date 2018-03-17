//
//  ProcessorPort.cpp
//  VirtualC64
//
//  Created by Dirk Hoffmann on 17.03.18.
//

#include "C64.h"

// From CPU6510C64.cpp (part of the Emulator Developers Kit):
//
//     The CPU port of the C64 behaves strange in input mode:
//
//     00010111 (17) pull-up resistors, drawn high
//     00100000 (20) pull-down transistor, drawn low
//     11000000 (C0) keeps last high for 65ms, then drawn low
//     00001000 (08) no datasette: keeps last high for 1s, then drawn low
//                   datasette connected: high and low randomly (mains?)
//
//     In reality, times depend on both CPU temperature and how long the output was
//     high (measure bit 3 with CPUPORT3.ASM on the C64).

ProcessorPort::ProcessorPort()
{
    setDescription("ProcessorPort");
    debug(3, "Creating processor port at address %p...\n", this);
    
    // Register snapshot items
    SnapshotItem items[] = {
        
        { &port,               sizeof(port),               0 },
        { &direction,          sizeof(direction),          0 },
        { &external,           sizeof(external),           0 },
        { &dischargeCycleBit3, sizeof(dischargeCycleBit3), 0 },
        { &dischargeCycleBit6, sizeof(dischargeCycleBit6), 0 },
        { &dischargeCycleBit7, sizeof(dischargeCycleBit7), 0 },
        { NULL,                0,                          0 }};
    
    registerSnapshotItems(items, sizeof(items));
}

ProcessorPort::~ProcessorPort()
{
}

void
ProcessorPort::reset()
{
    VirtualComponent::reset();
    
    port =  0x37;
    external = 0x1F;
    direction = 0x2F;
    dischargeCycleBit3 = UINT64_MAX;
    dischargeCycleBit6 = UINT64_MAX;
    dischargeCycleBit7 = UINT64_MAX;
}

void
ProcessorPort::dumpState()
{
    msg("Processor port:\n");
    msg("---------------\n\n");
    msg("port:           %02X\n", port);
    msg("direction:      %02X\n", direction);
    msg("external:       %02X\n", external);
    msg("Bit 3 floating: %s\n", c64->getCycles() < dischargeCycleBit3 ? "yes" : "no");
    msg("Bit 6 floating: %s\n", c64->getCycles() < dischargeCycleBit6 ? "yes" : "no");
    msg("Bit 7 floating: %s\n", c64->getCycles() < dischargeCycleBit7 ? "yes" : "no");
}
            
uint8_t
ProcessorPort::readProcessorPort()
{
    // Clear discharged floating bits if they are still configured as inputs
    if (!(direction & 0x08) && c64->getCycles() > dischargeCycleBit3) {
        CLR_BIT(external, 3);
        dischargeCycleBit6 = UINT64_MAX;
    }
    if (!(direction & 0x40) && c64->getCycles() > dischargeCycleBit6) {
        CLR_BIT(external, 6);
        dischargeCycleBit6 = UINT64_MAX;
    }
    if (!(direction & 0x80) && c64->getCycles() > dischargeCycleBit7) {
        CLR_BIT(external, 7);
        dischargeCycleBit7 = UINT64_MAX;
    }
    
    // Update datasette bit
    if (c64->datasette.getPlayKey()) external &= 0xEF; else external |= 0x10;
    
    uint8_t result = (port & direction) | (external & ~direction);
    
    return result;
}

uint8_t
ProcessorPort::readProcessorPortDirection()
{
    return direction;
}

void
ProcessorPort::writeProcessorPort(uint8_t value)
{
    port = value;
    
    // Update bits 3, 6 and 7 if they are configured as outputs
    uint8_t mask = 0xC8 & direction;
    external = (external & ~mask) | (port & mask);
    
    // Check for datasette motor bit
    if (direction & 0x20) {
        c64->datasette.setMotor((value & 0x20) == 0);
    }
    
    // Switch memory banks
    c64->mem.updatePeekPokeLookupTables();
}

void
ProcessorPort::writeProcessorPortDirection(uint8_t value)
{
    // Check if bits 3, 6, or 7 change from output to input. In that case, they
    // will change to a floating state and discharge over time.
    if (FALLING_EDGE(direction, value, 3)) {
        dischargeCycleBit3 = c64->getCycles() + PAL_CYCLES_PER_SECOND;
    }
    if (FALLING_EDGE(direction, value, 6)) {
        dischargeCycleBit6 = c64->getCycles() + 360000;
    }
    if (FALLING_EDGE(direction, value, 7)) {
        dischargeCycleBit7 = c64->getCycles() + 360000;
    }
    
    direction = value;
    
    // Update bits 3, 6 and 7 if they are configured as outputs
    uint8_t mask = 0xC8 & direction;
    external = (external & ~mask) | (port & mask);
    
    // Switch memory banks
    c64->mem.updatePeekPokeLookupTables();
}

void
ProcessorPort::setExternalBits(uint8_t value)
{
    external = value;
}



