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

#include "config.h"
#include "PIA.h"
#include "ParCable.h"
#include "Drive.h"

namespace vc64 {

//
// PIA (commons)
//

PIA6821::PIA6821(C64 &ref, Drive &drvref) : SubComponent(ref), drive(drvref)
{
    
}

void
PIA6821::setCA1External(bool value)
{
    bool active = isActiveTransitionCA1(ca1, value);
    bool enable = GET_BIT(cra, 0);
    
    ca1 = value;
    
    // Handle interrupt
    if (active) {
        
        SET_BIT(cra, 7);
        if (enable) irqAHasOccurred();

        // Release CA2 in "Read Strobe with CA1 Restore" mode
        if (ca2Control() == 0b100) setCA2Internal(true);
    }
}

void
PIA6821::setCA2External(bool value)
{
    bool active = isActiveTransitionCA2(ca2, value);
    bool enable = GET_BIT(cra, 3);
    
    ca2 = value;
    
    if (active) {
        
        SET_BIT(cra, 6);
        if (enable) irqAHasOccurred();
    }
}

void
PIA6821::setCB1External(bool value)
{
    bool active = isActiveTransitionCB1(cb1, value);
    bool enable = GET_BIT(cra, 0);
    
    cb1 = value;
    
    if (active) {
        
        SET_BIT(crb, 7);
        if (enable) irqBHasOccurred();
        
        // Release CB2 in "Write Strobe with CB1 Restore" mode
        if (cb2Control() == 0b100) setCB2Internal(true);
    }
}

void
PIA6821::setCB2External(bool value)
{
    bool active = isActiveTransitionCB2(cb2, value);
    bool enable = GET_BIT(crb, 3);
    
    cb2 = value;
    
    if (active) {
        SET_BIT(crb, 6);
        if (enable) irqBHasOccurred();
    }
}

void
PIA6821::setCA2Internal(bool value)
{
    assert(ca2IsInput());
    
    if (ca2 != value) {
        ca2 = value;
        ca2HasChangedTo(value);
    }
}

void
PIA6821::setCB2Internal(bool value)
{
    assert(cb2IsInput());
    
    if (cb2 != value) {
        cb2 = value;
        cb2HasChangedTo(value);
    }
}

bool
PIA6821::isActiveTransitionCA1(bool oldValue, bool newValue) const
{
    // The active transition is determined by bit 1 in CRA
    // 0 = high-to-low, 1 = low-to-high
    return (oldValue != newValue && !!GET_BIT(cra, 1) == newValue);
}

bool
PIA6821::isActiveTransitionCA2(bool oldValue, bool newValue) const
{
    // The active transition is determined by bit 4 in CRA
    // 0 = high-to-low, 1 = low-to-high
    return (oldValue != newValue && !!GET_BIT(cra, 4) == newValue);
}

bool
PIA6821::isActiveTransitionCB1(bool oldValue, bool newValue) const
{
    // The active transition is determined by bit 1 in CRB
    // 0 = high-to-low, 1 = low-to-high
    return (oldValue != newValue && !!GET_BIT(crb, 1) == newValue);
}

bool
PIA6821::isActiveTransitionCB2(bool oldValue, bool newValue) const
{
    // The active transition is determined by bit 4 in CRB
    // 0 = high-to-low, 1 = low-to-high
    return (oldValue != newValue && !!GET_BIT(crb, 4) == newValue);
}

u8
PIA6821::peek(bool rs1, bool rs0)
{
    u8 result;
    auto addr = rs1 << 3 | rs0 << 2 | (cra & 0b100) >> 1 | (crb & 0b100) >> 2;
    
    /* |  RS1  |  RS0  | CRA-2 | CRB-2  | Register
     * -------------------------------------------
     * |   0   |   0   |   1   |   x    | pra
     * |   0   |   0   |   0   |   x    | ddra
     * |   0   |   1   |   x   |   x    | cra
     * |   1   |   0   |   x   |   1    | prb
     * |   1   |   0   |   x   |   0    | ddrb
     * |   1   |   1   |   x   |   x    | crb
     */
    
    switch (addr) {
            
        case 0x2: case 0x3: // PRA
            
            result = updatePA();
            
            // Clear interrupt bits
            cra &= 0x3F;
            
            switch (ca2Control()) {
                    
                case 0b100: // "Read Strobe with CA1 Restore"
                    
                    setCA2Internal(0);
                    break;
                    
                case 0b101: // "Read Strobe with E Restore"
                    
                    setCA2Internal(0);
                    setCA2Internal(1);
                    break;
            }
            trace(PIA_DEBUG, "peekPRB() = %x\n", result);
            break;
            
        case 0x0: case 0x1: // DDRA
            
            result = ddra;
            trace(PIA_DEBUG, "peekDDRA() = %x\n", result);
            break;
            
        case 0x4: case 0x5: case 0x6: case 0x7: // CRA
            
            result = cra;
            trace(PIA_DEBUG, "peekCRA() = %x\n", result);
            break;
            
        case 0x9: case 0xb: // PRB
            
            result = updatePB();
            
            // Clear interrupt bits
            crb &= 0x3F;

            trace(PIA_DEBUG, "peekPRB() = %x\n", result);
            break;
            
        case 0x8: case 0xa: // DDRB
            
            result = ddrb;
            break;
            
        case 0xc: case 0xd: case 0xe: case 0xf: // CRB
            
            result = crb;
            trace(PIA_DEBUG, "peekCRB() = %x\n", result);
            break;
            
        default:
            
            fatalError;
    }
    
    // trace(PIA_DEBUG, "peek(%x) = %x\n", addr, result);
    return result;
}

void
PIA6821::poke(bool rs1, bool rs0, u8 value)
{
    auto addr = rs1 << 3 | rs0 << 2 | (cra & 0b100) >> 1 | (crb & 0b100) >> 2;
    
    /* |  RS1  |  RS0  | CRA-2 | CRB-2  | Register
     * -------------------------------------------
     * |   0   |   0   |   1   |   x    | pra
     * |   0   |   0   |   0   |   x    | ddra
     * |   0   |   1   |   x   |   x    | cra
     * |   1   |   0   |   x   |   1    | prb
     * |   1   |   0   |   x   |   0    | ddrb
     * |   1   |   1   |   x   |   x    | crb
     */

    switch (addr) {
            
        case 0x2: case 0x3: // PRA
            
            trace(PIA_DEBUG, "pokePRA(%x)\n", value);
            ora = value;
            break;
            
        case 0x0: case 0x1: // DDRA
            
            trace(PIA_DEBUG, "pokeDDRA(%x)\n", value);
            ddra = value;
            break;
            
        case 0x4: case 0x5: case 0x6: case 0x7: // CRA
        {
            trace(PIA_DEBUG, "pokeCRA(%x)\n", value);

            bool pendingIrq =
            (GET_BIT(cra, 7) && RISING_EDGE_BIT(cra, value, 0)) ||
            (GET_BIT(cra, 6) && RISING_EDGE_BIT(cra, value, 3));
            
            cra = value;
            
            switch (ca2Control()) {
                    
                case 0b110: // "Reset CA2"
                    
                    setCA2Internal(0);
                    break;
                    
                case 0b111: // "Set CA2"
                    
                    setCA2Internal(1);
                    break;
            }
            
            if (pendingIrq) irqAHasOccurred();
            break;
        }
        case 0x9: case 0xb: // PRB
            
            trace(PIA_DEBUG, "pokePRB(%x)\n", value);
            orb = value;
            
            switch (cb2Control()) {
                    
                case 0b100: // "Write Strobe with CB1 Restore"
                    
                    setCB2Internal(0);
                    break;
                    
                case 0b101: // "Write Strobe with E Restore"
                    
                    setCB2Internal(0);
                    setCB2Internal(1);
                    break;
            }
            break;
            
        case 0x8: case 0xa: // DDRB
            
            trace(PIA_DEBUG, "pokeDDRB(%x)\n", value);
            ddrb = value;
            break;
            
        case 0xc: case 0xd: case 0xe: case 0xf: // CRB
        {
            trace(PIA_DEBUG, "pokeCRB(%x)\n", value);

            bool pendingIrq =
            (GET_BIT(crb, 7) && RISING_EDGE_BIT(crb, value, 0)) ||
            (GET_BIT(crb, 6) && RISING_EDGE_BIT(crb, value, 3));

            crb = value;
            
            switch (cb2Control()) {
                    
                case 0b110: // "Reset CB2"
                    
                    setCB2Internal(0);
                    break;
                    
                case 0b111: // "Set CB2"
                    
                    setCB2Internal(1);
                    break;
            }
            
            if (pendingIrq) irqBHasOccurred();
            break;
        }
        default:
            fatalError;
    }
}

u8
PIA6821::updatePA()
{
    pa = ora & ddra;
    return pa;
}

u8
PIA6821::updatePB()
{
    pb = orb & ddrb;
    return pb;
}

//
// DolphinDOS 3
//

void
PiaDolphin::ca2HasChangedTo(bool value)
{
    trace(PIA_DEBUG, "ca2HasChangedTo(%d)\n", value);
    
    if (value) {
        parCable.driveHandshake();
    }
}

void
PiaDolphin::cb2HasChangedTo(bool value)
{
    trace(PIA_DEBUG, "cb2HasChangedTo(%d)\n", value);
}

void
PiaDolphin::irqAHasOccurred() const
{
    trace(PIA_DEBUG, "irqAHasOccurred\n");
}

void
PiaDolphin::irqBHasOccurred() const
{
    trace(PIA_DEBUG, "irqBHasOccurred\n");
}

u8
PiaDolphin::updatePA()
{
    switch (drive.getParCableType()) {
            
        case PAR_CABLE_DOLPHIN3:
            
            pa = parCable.getValue();
            break;
            
        default:
            
            PIA6821::updatePA();
            break;
    }
    
    return pa;
}

u8
PiaDolphin::updatePB()
{
    return PIA6821::updatePB();
}

u8
PiaDolphin::peek(u16 addr)
{
    return PIA6821::peek(addr & 0b10, addr & 0b01);
}

u8
PiaDolphin::spypeek(u16 addr) const
{
    return PIA6821::spypeek(addr & 0b10, addr & 0b01);
}

void
PiaDolphin::poke(u16 addr, u8 value)
{
    PIA6821::poke(addr & 0b10, addr & 0b01, value);
}

}
