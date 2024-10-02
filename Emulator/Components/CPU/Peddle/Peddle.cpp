// -----------------------------------------------------------------------------
// This file is part of Peddle - A MOS 65xx CPU emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Published under the terms of the MIT License
// -----------------------------------------------------------------------------

#include "Peddle.h"
#include "PeddleUtils.h"

namespace vc64::peddle {

// Static lookup tables
MicroInstruction Peddle::actionFunc[256] = { };
const char *Peddle::mnemonic[256] = { };
AddressingMode Peddle::addressingMode[256] = { };

Peddle::Peddle(C64 &ref) : SubComponent(ref)
{
    static isize counter = 0;

    /* Assign a unique ID to this CPU. The ID is not needed for emulation. It
     * is merely provided to allow the client code to easily distinguish
     * between different CPU instances. For instance, VirtualC64 uses it to
     * distinguish the CPUs of the attached floppy drives.
     */
    id = counter++;

    // Initialize the microinstruction lookup table
    registerInstructions();
}

void
Peddle::setModel(CPURevision cpuModel)
{
    this->cpuModel = cpuModel;
}

u16
Peddle::hasProcessorPort() const
{
    switch (cpuModel) {

        case MOS_6502:  return hasProcessorPort<MOS_6502>();
        case MOS_6507:  return hasProcessorPort<MOS_6507>();
        case MOS_6510:  return hasProcessorPort<MOS_6510>();
        case MOS_8502:  return hasProcessorPort<MOS_8502>();

        default:
            fatalError;
    }

}

template <CPURevision C> u16
Peddle::hasProcessorPort() const
{
    return C == MOS_6510 || C == MOS_8502;
}
template u16 Peddle::hasProcessorPort<MOS_6502>() const;
template u16 Peddle::hasProcessorPort<MOS_6507>() const;
template u16 Peddle::hasProcessorPort<MOS_6510>() const;
template u16 Peddle::hasProcessorPort<MOS_8502>() const;

u16
Peddle::addrMask() const
{
    switch (cpuModel) {

        case MOS_6502:  return addrMask<MOS_6502>();
        case MOS_6507:  return addrMask<MOS_6507>();
        case MOS_6510:  return addrMask<MOS_6510>();
        case MOS_8502:  return addrMask<MOS_8502>();

        default:
            fatalError;
    }
}

template <CPURevision C> u16
Peddle::addrMask() const
{
    switch (C) {

        case MOS_6507:  return 0x1FFF;
        default:        return 0xFFFF;
    }
}
template u16 Peddle::addrMask<MOS_6502>() const;
template u16 Peddle::addrMask<MOS_6507>() const;
template u16 Peddle::addrMask<MOS_6510>() const;
template u16 Peddle::addrMask<MOS_8502>() const;

void
Peddle::pullDownNmiLine(u8 mask)
{
    assert(mask != 0);

    // Check for falling edge on physical line
    if (!nmiLine) {
        edgeDetector.write(1);
    }

    nmiLine |= mask;
}

void
Peddle::releaseNmiLine(u8 mask)
{
    nmiLine &= ~mask;
}

void
Peddle::pullDownIrqLine(u8 mask)
{
    assert(mask != 0);

    irqLine |= mask;
    levelDetector.write(irqLine);
}

void
Peddle::releaseIrqLine(u8 mask)
{
    irqLine &= ~mask;
    levelDetector.write(irqLine);
}

void
Peddle::pullDownRdyLine(u8 mask)
{
    assert(mask != 0);

    auto old = rdyLine;
    rdyLine |= mask;
    if (!old && rdyLine) rdyLineDown = clock;
}

void
Peddle::releaseRdyLine(u8 mask)
{
    assert(mask != 0);
    
    auto old = rdyLine;
    rdyLine &= ~mask;
    if (old && !rdyLine) rdyLineUp = clock;
}

u8
Peddle::getP() const
{
    u8 result = 0b00100000;

    if (reg.sr.n) result |= N_FLAG;
    if (reg.sr.v) result |= V_FLAG;
    if (reg.sr.b) result |= B_FLAG;
    if (reg.sr.d) result |= D_FLAG;
    if (reg.sr.i) result |= I_FLAG;
    if (reg.sr.z) result |= Z_FLAG;
    if (reg.sr.c) result |= C_FLAG;

    return result;
}

u8
Peddle::getPWithClearedB() const
{
    return getP() & ~B_FLAG;
}

void
Peddle::setP(u8 p)
{
    setPWithoutB(p);
    reg.sr.b = (p & B_FLAG);
}

void
Peddle::setPWithoutB(u8 p)
{
    reg.sr.n = (p & N_FLAG);
    reg.sr.v = (p & V_FLAG);
    reg.sr.d = (p & D_FLAG);
    reg.sr.i = (p & I_FLAG);
    reg.sr.z = (p & Z_FLAG);
    reg.sr.c = (p & C_FLAG);
}

#include "PeddleInit_cpp.h"
#include "PeddleExec_cpp.h"
#include "PeddleMemory_cpp.h"

}
