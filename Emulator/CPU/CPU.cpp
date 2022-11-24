// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "CPU.h"
#include "C64.h"
#include "IOUtils.h"

CPU::CPU(C64& ref) : Peddle(ref)
{
    subComponents = std::vector<C64Component *> {
        
        &pport
    };
    
	// Establish callback for each instruction
	registerInstructions();
}

CPU::CPU(CPURevision cpuModel, C64& ref) : CPU(ref)
{
    setModel(cpuModel);
}

void
CPU::setModel(CPURevision cpuModel)
{
    this->cpuModel = cpuModel;
}

bool CPU::isC64CPU() const { return cpuModel == MOS_6510; }
bool CPU::isDriveCPU() const { return !isC64CPU(); }

void
CPU::_reset(bool hard)
{
    RESET_SNAPSHOT_ITEMS(hard)
    
    setB(1);
    setI(1);
	rdyLine = true;
	next = fetch;
    
    assert(levelDetector.isClear());
    assert(edgeDetector.isClear());
}

void
CPU::_inspect() const
{    
    {   SYNCHRONIZED
        
        info.cycle = clock;
        info.reg = reg;
        
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
    // We only allow the C64 CPU to run in debug mode
    if (isC64CPU()) { debugMode = true; }
}

void
CPU::_debugOff()
{
    debugMode = false;
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
    
        os << tab("Cycle");
        os << dec(clock) << std::endl;
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
        os << hex(HI_W_LO_W(spypeek(0xFFFF), spypeek(0xFFFE))) << std::endl;
        os << tab("NMI routine");
        os << hex(HI_W_LO_W(spypeek(0xFFFB), spypeek(0xFFFA))) << std::endl;
    }
}

u8
CPU::getP() const
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
CPU::getPWithClearedB() const
{
    return getP() & ~B_FLAG;
}

void
CPU::setP(u8 p)
{
    setPWithoutB(p);
    reg.sr.b = (p & B_FLAG);
}

void
CPU::setPWithoutB(u8 p)
{
    reg.sr.n = (p & N_FLAG);
    reg.sr.v = (p & V_FLAG);
    
    reg.sr.d = (p & D_FLAG);
    reg.sr.i = (p & I_FLAG);
    reg.sr.z = (p & Z_FLAG);
    reg.sr.c = (p & C_FLAG);
}

u8
CPU::peek(u16 addr)
{
    return
    cpuModel == MOS_6510 ?
    mem.peek(addr) :
    id == 1 ? drive8.mem.peek(addr) : drive9.mem.peek(addr);
}

u8
CPU::peekZP(u8 addr)
{
    return cpuModel == MOS_6510 ?
    mem.peekZP(addr) :
    id == 1 ? drive8.mem.peekZP(addr) : drive9.mem.peekZP(addr);
}

u8
CPU::peekStack(u8 sp)
{
    return cpuModel == MOS_6510 ?
    mem.peekStack(sp) :
    id == 1 ? drive8.mem.peekStack(sp) : drive9.mem.peekStack(sp);
}

u8
CPU::spypeek(u16 addr) const
{
    return cpuModel == MOS_6510 ?
    mem.spypeek(addr) :
    id == 1 ? drive8.mem.spypeek(addr) : drive9.mem.spypeek(addr);
}

void
CPU::poke(u16 addr, u8 value)
{
    cpuModel == MOS_6510 ?
    mem.poke(addr, value) :
    id == 1 ? drive8.mem.poke(addr, value) : drive9.mem.poke(addr, value);
}

void
CPU::pokeZP(u8 addr, u8 value)
{
    cpuModel == MOS_6510 ?
    mem.pokeZP(addr, value) :
    id == 1 ? drive8.mem.pokeZP(addr, value) : drive9.mem.pokeZP(addr, value);
}

void
CPU::pokeStack(u8 addr, u8 value)
{
    cpuModel == MOS_6510 ?
    mem.pokeStack(addr, value) :
    id == 1 ? drive8.mem.pokeStack(addr, value) : drive9.mem.pokeStack(addr, value);
}

void
CPU::pullDownNmiLine(IntSource bit)
{
    assert(bit != 0);
    
    // Check for falling edge on physical line
    if (!nmiLine) {
        edgeDetector.write(1);
    }
    
    nmiLine |= bit;
}

void
CPU::releaseNmiLine(IntSource source)
{
    nmiLine &= ~source;
}

void
CPU::pullDownIrqLine(IntSource source)
{
	assert(source != 0);
    
	irqLine |= source;
    levelDetector.write(irqLine);
}

void
CPU::releaseIrqLine(IntSource source)
{
    irqLine &= ~source;
    levelDetector.write(irqLine);
}

void
CPU::setRDY(bool value)
{
    if (rdyLine)
    {
        rdyLine = value;
        if (!rdyLine) rdyLineDown = clock;
    }
    else
    {
        rdyLine = value;
        if (rdyLine) rdyLineUp = clock;
    }
}


//
// Template instantiations
//

/*
template         CPU<MOS_6510>::CPU(C64& ref);
template         CPU<MOS_6510>::CPU(CPURevision, C64& ref);
template CPUInfo CPU<MOS_6510>::getInfo() const;
template void    CPU<MOS_6510>::_dump(Category category, std::ostream& os) const;
template void    CPU<MOS_6510>::_debugOn();
template void    CPU<MOS_6510>::_debugOff();
template void    CPU<MOS_6510>::_reset(bool hard);
template void    CPU<MOS_6510>::_inspect() const;
template u8      CPU<MOS_6510>::getP() const;
template u8      CPU<MOS_6510>::getPWithClearedB() const;
template void    CPU<MOS_6510>::setP(u8 p);
template void    CPU<MOS_6510>::setPWithoutB(u8 p);
template void    CPU<MOS_6510>::pullDownNmiLine(IntSource source);
template void    CPU<MOS_6510>::releaseNmiLine(IntSource source);
template void    CPU<MOS_6510>::pullDownIrqLine(IntSource source);
template void    CPU<MOS_6510>::releaseIrqLine(IntSource source);
template void    CPU<MOS_6510>::setRDY(bool value);

template u8      CPU<MOS_6510>::peek(u16 addr);
template u8      CPU<MOS_6510>::peekZP(u8 addr);
template u8      CPU<MOS_6510>::peekStack(u8 sp);
template u8      CPU<MOS_6510>::spypeek(u16 addr) const;
template void    CPU<MOS_6510>::poke(u16 addr, u8 value);
template void    CPU<MOS_6510>::pokeZP(u8 addr, u8 value);
template void    CPU<MOS_6510>::pokeStack(u8 sp, u8 value);

template         CPU<MOS_6502>::CPU(C64& ref);
template         CPU<MOS_6502>::CPU(CPURevision, C64& ref);
template CPUInfo CPU<MOS_6502>::getInfo() const;
template void    CPU<MOS_6502>::_dump(Category category, std::ostream& os) const;
template void    CPU<MOS_6502>::_debugOn();
template void    CPU<MOS_6502>::_debugOff();
template void    CPU<MOS_6502>::_reset(bool hard);
template void    CPU<MOS_6502>::_inspect() const;
template u8      CPU<MOS_6502>::getP() const;
template u8      CPU<MOS_6502>::getPWithClearedB() const;
template void    CPU<MOS_6502>::setP(u8 p);
template void    CPU<MOS_6502>::setPWithoutB(u8 p);
template void    CPU<MOS_6502>::pullDownNmiLine(IntSource source);
template void    CPU<MOS_6502>::releaseNmiLine(IntSource source);
template void    CPU<MOS_6502>::pullDownIrqLine(IntSource source);
template void    CPU<MOS_6502>::releaseIrqLine(IntSource source);
template void    CPU<MOS_6502>::setRDY(bool value);

template u8      CPU<MOS_6502>::peek(u16 addr);
template u8      CPU<MOS_6502>::peekZP(u8 addr);
template u8      CPU<MOS_6502>::peekStack(u8 sp);
template u8      CPU<MOS_6502>::spypeek(u16 addr) const;
template void    CPU<MOS_6502>::poke(u16 addr, u8 value);
template void    CPU<MOS_6502>::pokeZP(u8 addr, u8 value);
template void    CPU<MOS_6502>::pokeStack(u8 sp, u8 value);
*/
