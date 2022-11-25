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
    subComponents = std::vector<C64Component *> {
        
        &pport
    };    
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
        
        info.processorPort = pport.read();
        info.processorPortDir = pport.readDirection();
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
