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
#include "PeddleConfig.h"
#include "CPU.h"
#include "Emulator.h"
#include "IOUtils.h"

namespace vc64 {

CPU::CPU(C64& ref) : Peddle(ref)
{

}

CPU::CPU(CPURevision cpuModel, C64& ref) : CPU(ref)
{
    setModel(cpuModel);
}

const char *
CPU::getDescription() const
{
    switch (id) {

        case 0: return "CPU";
        case 1: return "CPU(8)";
        case 2: return "CPU(9)";

        default:
            return "???";
    }
}

void
CPU::_reset(bool hard)
{    
    Peddle::reset();

    // Enable or disable CPU debugging
    c64.emulator.isTracking() ? debugger.enableLogging() : debugger.disableLogging();

    assert(levelDetector.isClear());
    assert(edgeDetector.isClear());
}

bool 
CPU::autoInspect() const
{
    return c64.getInspectionTarget() == INSPECTION_CPU && isRunning();
}

void 
CPU::recordState(CPUInfo &result) const
{
    result.cycle = clock;

    result.pc0 = reg.pc0;
    result.sp = reg.sp;
    result.a = reg.a;
    result.x = reg.x;
    result.y = reg.y;
    result.sr = getP();

    result.irq = irqLine;
    result.nmi = nmiLine;
    result.rdy = rdyLine;

    result.processorPort = readPort();
    result.processorPortDir = readPortDir();
}

void
CPU::_trackOn()
{
    // We only allow the C64 CPU to enter track mode
    if (!isC64CPU()) return;

    debugger.enableLogging();
}

void
CPU::_trackOff()
{
    debugger.disableLogging();
}

void
CPU::_dump(Category category, std::ostream& os) const
{
    inspector._dump(category, os);
    /*
    using namespace util;

    if (category == Category::Registers) {

        os << tab("Instruction Address") << hex(reg.pc0) << std::endl;
        os << tab("Program Counter") << hex(reg.pc) << std::endl;
        os << tab("Accumulator") << hex(reg.a) << std::endl;
        os << tab("X Register") << hex(reg.x) << std::endl;
        os << tab("Y Register") << hex(reg.y) << std::endl;
        os << tab("Stack Pointer") << hex(reg.sp) << std::endl;
        os << tab("Flags");
        os << (reg.sr.n ? "N" : "n");
        os << (reg.sr.v ? "V" : "v");
        os << (reg.sr.b ? "B" : "b");
        os << (reg.sr.d ? "D" : "d");
        os << (reg.sr.i ? "I" : "i");
        os << (reg.sr.z ? "Z" : "z");
        os << (reg.sr.c ? "C" : "c");
        os << std::endl;
    }
    
    if (category == Category::State) {

        auto append = [&](const string &s1, const string &s2) {
            return s1.empty() ? s2 : s1 + ", " + s2;
        };

        string str = "";
        if (flags & CPU_LOG_INSTRUCTION) str = append(str, "LOG_INSTRUCTION");
        if (flags & CPU_CHECK_BP) str = append(str, "CHECK_BP");
        if (flags & CPU_CHECK_WP) str = append(str, "CHECK_WP");

        os << tab("Clock");
        os << dec(clock) << std::endl;
        os << tab("Flags");
        os << (str.empty() ? "-" : str) << std::endl;
        os << tab("Next microinstruction");
        os << dec(next) << std::endl;
        os << tab("Rdy Line");
        os << bol(rdyLine, "high", "low") << std::endl;
        os << tab("Nmi Line");
        os << hex(nmiLine) << std::endl;
        os << tab("Edge detector");
        os << hex(edgeDetector.current()) << std::endl;
        os << tab("doNmi");
        os << bol(doNmi) << std::endl;
        os << tab("Irq Line");
        os << hex(irqLine) << std::endl;
        os << tab("Edge Detector");
        os << hex(levelDetector.current()) << std::endl;
        os << tab("doIrq");
        os << bol(doIrq) << std::endl;
        os << tab("IRQ Routine");
        os << hex(HI_W_LO_W(readDasm(0xFFFF), readDasm(0xFFFE))) << std::endl;
        os << tab("NMI Routine");
        os << hex(HI_W_LO_W(readDasm(0xFFFB), readDasm(0xFFFA))) << std::endl;

        if (hasProcessorPort()) {

            os << tab("Processor port");
            os << hex(reg.pport.data) << std::endl;
            os << tab("Direction bits");
            os << hex(reg.pport.direction) << std::endl;
            os << tab("Bit 3 discharge cycle");
            os << dec(dischargeCycleBit3) << std::endl;
            os << tab("Bit 6 discharge cycle");
            os << dec(dischargeCycleBit6) << std::endl;
            os << tab("Bit 7 discharge cycle");
            os << dec(dischargeCycleBit7) << std::endl;
        }
    }
    */
}

void
CPU::writePort(u8 val)
{
    Peddle::writePort(val);

    // Check the motor bit of the datasette
    if (reg.pport.direction & 0x20) {
        datasette.setMotor((val & 0x20) == 0);
    }

    // When writing to the port register, the last VICII byte appears
    mem.ram[0x0001] = vic.getDataBusPhi1();

    // Switch memory banks
    mem.updatePeekPokeLookupTables();
}

void
CPU::writePortDir(u8 val)
{
    auto port = reg.pport.data;
    auto direction = reg.pport.direction;

    // If bits 3, 6, or 7 are configured as outputs, they are not floating
    if (GET_BIT(val, 3)) dischargeCycleBit3 = 0;
    if (GET_BIT(val, 6)) dischargeCycleBit6 = 0;
    if (GET_BIT(val, 7)) dischargeCycleBit7 = 0;

    // If bits 3, 6, or 7 change from output to input, they become floating
    if (FALLING_EDGE_BIT(direction, val, 3) && GET_BIT(port, 3) != 0)
        dischargeCycleBit3 = UINT64_MAX;
    if (FALLING_EDGE_BIT(direction, val, 6) && GET_BIT(port, 6) != 0)
        dischargeCycleBit6 = cpu.clock + dischargeCyclesVICE;
    if (FALLING_EDGE_BIT(direction, val, 7) && GET_BIT(port, 7) != 0)
        dischargeCycleBit7 = cpu.clock + dischargeCyclesVICE;

    // Update the port register
    Peddle::writePortDir(val);

    // When writing to the direction register, the last VICII byte appears
    mem.ram[0x0000] = vic.getDataBusPhi1();

    // Switch memory banks
    mem.updatePeekPokeLookupTables();
}

u8
CPU::externalPortBits() const
{
    /* If the port bits are configured as inputs and no datasette is attached,
     * the following values are returned:
     *
     *     Bit 0:  1 (bit is driven by a pull-up resistor)
     *     Bit 1:  1 (bit is driven by a pull-up resistor)
     *     Bit 2:  1 (bit is driven by a pull-up resistor)
     * ??? Bit 3:  Eventually 0 (acts a a capacitor)
     * ??? Bit 3:  0 (bit is driven by a pull-down resistor)
     *     Bit 4:  1 (bit is driven by a pull-up resistor)
     *     Bit 5:  0 (bit is driven by a pull-down resistor)
     *     Bit 6:  Eventually 0 (acts a a capacitor)
     *     Bit 7:  Eventually 0 (acts a a capacitor)
     */
    u8 bit3 = (dischargeCycleBit3 > cpu.clock) ? 0x08 : 0x00;
    u8 bit6 = (dischargeCycleBit6 > cpu.clock) ? 0x40 : 0x00;
    u8 bit7 = (dischargeCycleBit7 > cpu.clock) ? 0x80 : 0x00;
    u8 bit4 = datasette.getPlayKey() ? 0x00 : 0x10;

    return bit7 | bit6 | bit4 | bit3 | 0x07;
}

void
CPU::cpuDidJam()
{
    c64.signalJammed();
}

void
CPU::nmiWillTrigger()
{
    if (isC64CPU()) {
        expansionport.nmiWillTrigger();
    }
}

void
CPU::nmiDidTrigger()
{
    if (isC64CPU()) {
        expansionport.nmiDidTrigger();
    }
}

void
CPU::breakpointReached(u16 addr) const
{
    c64.signalBreakpoint();
}

void
CPU::watchpointReached(u16 addr) const
{
    c64.signalWatchpoint();
}

void
CPU::instructionLogged() const
{

}

void
CPU::jumpedTo(u16 addr) const
{
    msgQueue.put(MSG_CPU_JUMPED, CpuMsg { .pc = addr } );
}

void
CPU::jump(u16 addr)
{
    {   SUSPENDED

        debugger.jump(addr);
    }
}


//
// Memory API
//

u8
CPU::read(u16 addr)
{
    switch (id) {

        case 0: return mem.peek(addr);
        case 1: return drive8.mem.peek(addr);
        case 2: return drive9.mem.peek(addr);

        default:
            fatalError;
    }
}

void
CPU::write(u16 addr, u8 val)
{
    switch (id) {

        case 0: mem.poke(addr, val); break;
        case 1: drive8.mem.poke(addr, val); break;
        case 2: drive9.mem.poke(addr, val); break;

        default:
            fatalError;
    }
}

u8
CPU::readDasm(u16 addr) const
{
    switch (id) {

        case 0: return mem.spypeek(addr);
        case 1: return drive8.mem.spypeek(addr);
        case 2: return drive9.mem.spypeek(addr);

        default:
            fatalError;
    }
}

u16
CPU::readResetVector()
{
    return mem.resetVector();
}

#if PEDDLE_SIMPLE_MEMORY_API == false

namespace vc64::peddle {

#define CHECK_WATCHPOINT \
if constexpr (PEDDLE_ENABLE_WATCHPOINTS) { \
if ((flags & CPU_CHECK_WP) && debugger.watchpointMatches(addr)) { \
watchpointReached(addr); \
}}

template <CPURevision C> u8
Peddle::read(u16 addr)
{
    CHECK_WATCHPOINT

    if (hasProcessorPort<C>()) {

        if (addr < 2) return addr ? readPort() : readPortDir();
    }
    return read(addr & addrMask<C>());
}
template u8 Peddle::read<MOS_6502>(u16 addr);
template u8 Peddle::read<MOS_6507>(u16 addr);
template u8 Peddle::read<MOS_6510>(u16 addr);
template u8 Peddle::read<MOS_8502>(u16 addr);

template <CPURevision C> u8
Peddle::readZeroPage(u8 addr)
{
    CHECK_WATCHPOINT

    switch (C) {

        case MOS_6510: return mem.peekZP(addr);
        case MOS_6502: return id == 1 ? drive8.mem.peekZP(addr) : drive9.mem.peekZP(addr);

        default:
            fatalError;
    }
}
template u8 Peddle::readZeroPage<MOS_6502>(u8 addr);
template u8 Peddle::readZeroPage<MOS_6507>(u8 addr);
template u8 Peddle::readZeroPage<MOS_6510>(u8 addr);
template u8 Peddle::readZeroPage<MOS_8502>(u8 addr);

template <CPURevision C> u8
Peddle::readStack(u8 addr)
{
    CHECK_WATCHPOINT

    switch (C) {

        case MOS_6510: return mem.peekStack(addr);
        case MOS_6502: return id == 1 ? drive8.mem.peekStack(addr) : drive9.mem.peekStack(addr);

        default:
            fatalError;
    }
}
template u8 Peddle::readStack<MOS_6502>(u8 addr);
template u8 Peddle::readStack<MOS_6507>(u8 addr);
template u8 Peddle::readStack<MOS_6510>(u8 addr);
template u8 Peddle::readStack<MOS_8502>(u8 addr);

template <CPURevision C> void
Peddle::readIdle(u16 addr)
{
    if (PEDDLE_EMULATE_IDLE_ACCESSES) {

        CHECK_WATCHPOINT

        switch (C) {

            case MOS_6510: return mem.peekIdle(addr);
            case MOS_6502: return id == 1 ? drive8.mem.peekIdle(addr) : drive9.mem.peekIdle(addr);

            default:
                fatalError;
        }
    }
}
template void Peddle::readIdle<MOS_6502>(u16 addr);
template void Peddle::readIdle<MOS_6507>(u16 addr);
template void Peddle::readIdle<MOS_6510>(u16 addr);
template void Peddle::readIdle<MOS_8502>(u16 addr);

template <CPURevision C> void
Peddle::readZeroPageIdle(u8 addr)
{
    if (PEDDLE_EMULATE_IDLE_ACCESSES) {

        CHECK_WATCHPOINT

        switch (C) {

            case MOS_6510: return mem.peekZPIdle(addr);
            case MOS_6502: return id == 1 ? drive8.mem.peekZPIdle(addr) : drive9.mem.peekZPIdle(addr);

            default:
                fatalError;
        }
    }
}
template void Peddle::readZeroPageIdle<MOS_6502>(u8 addr);
template void Peddle::readZeroPageIdle<MOS_6507>(u8 addr);
template void Peddle::readZeroPageIdle<MOS_6510>(u8 addr);
template void Peddle::readZeroPageIdle<MOS_8502>(u8 addr);

template <CPURevision C> void
Peddle::readStackIdle(u8 addr)
{
    if (PEDDLE_EMULATE_IDLE_ACCESSES) {

        CHECK_WATCHPOINT

        switch (C) {

            case MOS_6510: return mem.peekStackIdle(addr);
            case MOS_6502: return id == 1 ? drive8.mem.peekStackIdle(addr) : drive9.mem.peekStackIdle(addr);

            default:
                fatalError;
        }
    }
}
template void Peddle::readStackIdle<MOS_6502>(u8 addr);
template void Peddle::readStackIdle<MOS_6507>(u8 addr);
template void Peddle::readStackIdle<MOS_6510>(u8 addr);
template void Peddle::readStackIdle<MOS_8502>(u8 addr);


template <CPURevision C> void
Peddle::write(u16 addr, u8 val)
{
    CHECK_WATCHPOINT

    switch (C) {

        case MOS_6510: return mem.poke(addr, val);
        case MOS_6502: return id == 1 ? drive8.mem.poke(addr, val) : drive9.mem.poke(addr, val);

        default:
            fatalError;
    }
}
template void Peddle::write<MOS_6502>(u16 addr, u8 val);
template void Peddle::write<MOS_6507>(u16 addr, u8 val);
template void Peddle::write<MOS_6510>(u16 addr, u8 val);
template void Peddle::write<MOS_8502>(u16 addr, u8 val);

template <CPURevision C> void
Peddle::writeZeroPage(u8 addr, u8 val)
{
    CHECK_WATCHPOINT

    switch (C) {

        case MOS_6510: return mem.pokeZP(addr, val);
        case MOS_6502: return id == 1 ? drive8.mem.pokeZP(addr, val) : drive9.mem.pokeZP(addr, val);

        default:
            fatalError;
    }
}
template void Peddle::writeZeroPage<MOS_6502>(u8 addr, u8 val);
template void Peddle::writeZeroPage<MOS_6507>(u8 addr, u8 val);
template void Peddle::writeZeroPage<MOS_6510>(u8 addr, u8 val);
template void Peddle::writeZeroPage<MOS_8502>(u8 addr, u8 val);

template <CPURevision C> void
Peddle::writeStack(u8 addr, u8 val)
{
    CHECK_WATCHPOINT

    switch (C) {

        case MOS_6510: return mem.pokeStack(addr, val);
        case MOS_6502: return id == 1 ? drive8.mem.pokeStack(addr, val) : drive9.mem.pokeStack(addr, val);

        default:
            fatalError;
    }
}
template void Peddle::writeStack<MOS_6502>(u8 addr, u8 val);
template void Peddle::writeStack<MOS_6507>(u8 addr, u8 val);
template void Peddle::writeStack<MOS_6510>(u8 addr, u8 val);
template void Peddle::writeStack<MOS_8502>(u8 addr, u8 val);

template <CPURevision C> u16
Peddle::readResetVector()
{
    u16 addr = 0xFFFC & addrMask<C>();
    return u16(read<C>(addr) | read<C>(addr + 1) << 8);
}
template u16 Peddle::readResetVector<MOS_6502>();
template u16 Peddle::readResetVector<MOS_6507>();
template u16 Peddle::readResetVector<MOS_6510>();
template u16 Peddle::readResetVector<MOS_8502>();

}

#endif

}
