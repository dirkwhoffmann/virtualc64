//
//  ProcessorPort.cpp
//  VirtualC64
//
//  Created by Dirk Hoffmann on 17.03.18.
//

#include "C64.h"

// From CPU6510C64.cpp (part of the Emulator Developers Kit):
// The CPU port of the C64 behaves strange in input mode:
// 00010111 (17) pull-up resistors, drawn high
// 00100000 (20) pull-down transistor, drawn low
// 11000000 (C0) keeps last high for 65ms, then drawn low
// 00001000 (08) no datasette: keeps last high for 1s, then drawn low
//               datasette connected: high and low randomly (mains?)
// In reality, times depend on both CPU temperature and how long the output was
// high (measure bit 3 with CPUPORT3.ASM on the C64).

ProcessorPort::ProcessorPort()
{
    setDescription("ProcessorPort");
    debug(3, "Creating processor port at address %p...\n", this);
    
    // Register snapshot items
    SnapshotItem items[] = {
        
        { &port,               sizeof(port),               0 },
        { &direction,          sizeof(direction),          0 },
        { &external,           sizeof(external),           CLEAR_ON_RESET },
        { &dischargeCycleBit6, sizeof(dischargeCycleBit6), CLEAR_ON_RESET },
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
    msg("Bit 6 floating: %s\n", c64->getCycles() < dischargeCycleBit6 ? "yes" : "no");
    msg("Bit 7 floating: %s\n", c64->getCycles() < dischargeCycleBit7 ? "yes" : "no");
}
            
uint8_t
ProcessorPort::readProcessorPort()
{
    /*
    // Check if floating bits reached zero
    if (c64->getCycles() > dischargeCycleBit6) {
        port &= 0xBF; // clear bit 6
        dischargeCycleBit6 = UINT64_MAX;
    }
    if (c64->getCycles() > dischargeCycleBit7) {
        port &= 0x7F; // clear bit 7
        dischargeCycleBit7 = UINT64_MAX;
    }
    */
    
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
    // Bit 6 and 7 are not connected and the external port bits act like an capacitor
    /*
    if (direction & 0x80) {
        external = (external & 0x7F) | (value & 0x80);
    }
    if (direction & 0x40) {
        external = (external & 0xBF) | (value & 0x40);
    }
    */
    
    port = value;
    
    // Bits 3, 6 and 7 act like capacitors in input mode
    uint8_t mask = 0xC8 & direction;
    external &= ~mask;
    external |= mask & port;
    
    // Datasette
    if (direction & 0x20) {
        c64->datasette.setMotor((value & 0x20) == 0);
    }
    
    // Switch memory banks
    c64->mem.updatePeekPokeLookupTables();
}

void
ProcessorPort::writeProcessorPortDirection(uint8_t value)
{
    
    /*
    // Check if bit 6 or bit 7 is now an output
    if (value & 0x40) {
        dischargeCycleBit6 = UINT64_MAX;
    }
    if (value & 0x80) {
        dischargeCycleBit7 = UINT64_MAX;
    }

    // Check if bits 6 and 7 are changed from output to inputs. In that case, they
    // will change to a floating state and discharge over time.
    // TODO: Change to if (FALLING_EDGE(direction, value, 6)) {
    if ((direction & 0x40) && !(value & 0x40)) {
        debug("Bit 6 becomes floating\n");
        dischargeCycleBit6 = c64->getCycles() + 360000;
    }
    // TODO: Change to if (FALLING_EDGE(direction, value, 7)) {
    if ((direction & 0x80) && !(value & 0x80)) {
        debug("Bit 7 becomes floating\n");
        dischargeCycleBit7 = c64->getCycles() + 360000;
    }
    */
    
    direction = value;
    
    // Bits 3, 6 and 7 act like capacitors in input mode
    uint8_t mask = 0xC8 & direction;
    external &= ~mask;
    external |= mask & port;
    
    // Switch memory banks
    c64->mem.updatePeekPokeLookupTables();
}

void
ProcessorPort::setExternalBits(uint8_t value)
{
    external = value;
}



