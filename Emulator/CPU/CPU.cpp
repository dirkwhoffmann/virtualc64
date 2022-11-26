// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "PeddleConfig.h"
#include "CPU.h"
#include "C64.h"
#include "IOUtils.h"

CPU::CPU(C64& ref) : Peddle(ref)
{
    /*
    subComponents = std::vector<C64Component *> {
        
        &pport
    };
    */
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
    RESET_SNAPSHOT_ITEMS(hard)

    Peddle::reset();
    mem.updatePeekPokeLookupTables();

    assert(levelDetector.isClear());
    assert(edgeDetector.isClear());
}

void
CPU::_inspect() const
{    
    {   SYNCHRONIZED
        
        info.cycle = clock;

        info.pc0 = reg.pc0;
        info.sp = reg.sp;
        info.a = reg.a;
        info.x = reg.x;
        info.y = reg.y;
        info.sr = getP();

        info.irq = irqLine;
        info.nmi = nmiLine;
        info.rdy = rdyLine;
        info.jammed = isJammed();
        
        info.processorPort = readPort();
        info.processorPortDir = readPortDir();
    }
}

void
CPU::_debugOn()
{
    // We only allow the C64 CPU to enter debug mode
    if (!isC64CPU()) return;

    debug(RUN_DEBUG, "Enabling debug mode\n");
    debugger.enableLogging();
}

void
CPU::_debugOff()
{
    debug(RUN_DEBUG, "Disabling debug mode\n");
    debugger.disableLogging();
}

void
CPU::_dump(Category category, std::ostream& os) const
{
    using namespace util;

    if (category == Category::Registers) {

        os << tab("PC") << hex(reg.pc) << std::endl;
        os << tab("SP") << hex(reg.sp) << std::endl;
        os << tab("A") << hex(reg.a) << std::endl;
        os << tab("X") << hex(reg.x) << std::endl;
        os << tab("Y") << hex(reg.y) << std::endl;
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

        os << tab("Cycle");
        os << dec(clock) << std::endl;
        os << tab("Flags");
        os << (str.empty() ? "-" : str) << std::endl;
        os << tab("Rdy line");
        os << bol(rdyLine, "high", "low") << std::endl;
        os << tab("Nmi line");
        os << hex(nmiLine) << std::endl;
        os << tab("Edge detector");
        os << hex(edgeDetector.current()) << std::endl;
        os << tab("doNmi");
        os << bol(doNmi) << std::endl;
        os << tab("Irq line");
        os << hex(irqLine) << std::endl;
        os << tab("Edge detector");
        os << hex(levelDetector.current()) << std::endl;
        os << tab("doIrq");
        os << bol(doIrq) << std::endl;
        os << tab("IRQ routine");
        os << hex(HI_W_LO_W(read8Dasm(0xFFFF), read8Dasm(0xFFFE))) << std::endl;
        os << tab("NMI routine");
        os << hex(HI_W_LO_W(read8Dasm(0xFFFB), read8Dasm(0xFFFA))) << std::endl;

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
}

u8
CPU::read8(u16 addr) const
{
    switch (id) {

        case 0: return mem.peek(addr);
        case 1: return drive8.mem.peek(addr);
        case 2: return drive9.mem.peek(addr);

        default:
            fatalError;
    }
}

u16
CPU::read16Reset(u16 addr) const
{
    return mem.resetVector();
}

u8
CPU::read8Dasm(u16 addr) const
{
    switch (id) {

        case 0: return mem.spypeek(addr);
        case 1: return drive8.mem.spypeek(addr);
        case 2: return drive9.mem.spypeek(addr);

        default:
            fatalError;
    }
}

void
CPU::write8(u16 addr, u8 val) const
{
    switch (id) {

        case 0: mem.poke(addr, val); break;
        case 1: drive8.mem.poke(addr, val); break;
        case 2: drive9.mem.poke(addr, val); break;

        default:
            fatalError;
    }
}

void
CPU::writePort(u8 val)
{
    Peddle::writePort(val);

    // Check for datasette motor bit
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
    /* Discharging times for bit 3, 6, and 7 as seen in other emulators. In
     * reality, the discharge delay depend on both CPU temperature and how
     * long the output was 1 befor the bit became an input.
     */
    static constexpr i64 dischargeCyclesVICE = 350000;
    static constexpr i64 dischargeCyclesHOXS = 246312;

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
    u8 bit3 = (dischargeCycleBit3 > i64(cpu.clock)) ? 0x08 : 0x00;
    u8 bit6 = (dischargeCycleBit6 > i64(cpu.clock)) ? 0x40 : 0x00;
    u8 bit7 = (dischargeCycleBit7 > i64(cpu.clock)) ? 0x80 : 0x00;
    u8 bit4 = datasette.getPlayKey() ? 0x00 : 0x10;

    return bit7 | bit6 | bit4 | bit3 | 0x07;
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
CPU::breakpointReached(u16 addr)
{
    c64.signalBreakpoint();
}

void
CPU::watchpointReached(u16 addr)
{

}

void
CPU::cpuDidJam()
{
    c64.signalJammed();
}

void
CPU::instructionLogged()
{
    // Perform an inspection from time to time
    c64.autoInspect();
}

namespace peddle {

}
