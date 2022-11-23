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

template <typename M>
CPU<M>::CPU(C64& ref, M& memref) : SubComponent(ref), memref(memref)
{
    subComponents = std::vector<C64Component *> {
        
        &pport,
        &debugger
    };
    
	// Establish callback for each instruction
	registerInstructions();
}

template<> CPURevision CPU<C64Memory>::model() const { return MOS_6510; }
template<> CPURevision CPU<DriveMemory>::model() const { return MOS_6502; }

template<> bool CPU<C64Memory>::isC64CPU() const { return true; }
template<> bool CPU<DriveMemory>::isC64CPU() const { return false; }

template<> bool CPU<C64Memory>::isDriveCPU() const { return false; }
template<> bool CPU<DriveMemory>::isDriveCPU() const { return true; }

template <typename M> void
CPU<M>::_reset(bool hard)
{
    RESET_SNAPSHOT_ITEMS(hard)
    
    setB(1);
    setI(1);
	rdyLine = true;
	next = fetch;
    
    assert(levelDetector.isClear());
    assert(edgeDetector.isClear());
}

template <typename M> void
CPU<M>::_inspect() const
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

template <typename M> void
CPU<M>::_debugOn()
{
    // We only allow the C64 CPU to run in debug mode
    if (isC64CPU()) { debugMode = true; }
}

template <typename M> void
CPU<M>::_debugOff()
{
    debugMode = false;
}

template <typename M> void
CPU<M>::_dump(Category category, std::ostream& os) const
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
        os << hex(HI_W_LO_W(memref.spypeek(0xFFFF), memref.spypeek(0xFFFE))) << std::endl;
        os << tab("NMI routine");
        os << hex(HI_W_LO_W(memref.spypeek(0xFFFB), memref.spypeek(0xFFFA))) << std::endl;
    }
}

template <typename M> u8
CPU<M>::getP() const
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

template <typename M> u8
CPU<M>::getPWithClearedB() const
{
    return getP() & ~B_FLAG;
}

template <typename M> void
CPU<M>::setP(u8 p)
{
    setPWithoutB(p);
    reg.sr.b = (p & B_FLAG);
}

template <typename M> void
CPU<M>::setPWithoutB(u8 p)
{
    reg.sr.n = (p & N_FLAG);
    reg.sr.v = (p & V_FLAG);
    
    reg.sr.d = (p & D_FLAG);
    reg.sr.i = (p & I_FLAG);
    reg.sr.z = (p & Z_FLAG);
    reg.sr.c = (p & C_FLAG);
}

template <typename M> u8
CPU<M>::read(u16 addr)
{
    if (model() == MOS_6510) {
        return memref.peek(addr);
    } else {
        return drive8.mem.peek(addr);
    }
}

template <typename M> void
CPU<M>::write(u16 addr, u8 value)
{
    if (model() == MOS_6510) {
        memref.poke(addr, value);
    } else {
        drive8.mem.poke(addr, value);
    }
}

template <typename M> void
CPU<M>::pullDownNmiLine(IntSource bit)
{
    assert(bit != 0);
    
    // Check for falling edge on physical line
    if (!nmiLine) {
        edgeDetector.write(1);
    }
    
    nmiLine |= bit;
}

template <typename M> void
CPU<M>::releaseNmiLine(IntSource source)
{
    nmiLine &= ~source;
}

template <typename M> void
CPU<M>::pullDownIrqLine(IntSource source)
{
	assert(source != 0);
    
	irqLine |= source;
    levelDetector.write(irqLine);
}

template <typename M> void
CPU<M>::releaseIrqLine(IntSource source)
{
    irqLine &= ~source;
    levelDetector.write(irqLine);
}

template <typename M> void
CPU<M>::setRDY(bool value)
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

template         CPU<C64Memory>::CPU(C64& ref, C64Memory& memref);
template CPUInfo CPU<C64Memory>::getInfo() const;
template void    CPU<C64Memory>::_dump(Category category, std::ostream& os) const;
template void    CPU<C64Memory>::_debugOn();
template void    CPU<C64Memory>::_debugOff();
template void    CPU<C64Memory>::_reset(bool hard);
template void    CPU<C64Memory>::_inspect() const;
template u8      CPU<C64Memory>::getP() const;
template u8      CPU<C64Memory>::getPWithClearedB() const;
template void    CPU<C64Memory>::setP(u8 p);
template void    CPU<C64Memory>::setPWithoutB(u8 p);
template void    CPU<C64Memory>::pullDownNmiLine(IntSource source);
template void    CPU<C64Memory>::releaseNmiLine(IntSource source);
template void    CPU<C64Memory>::pullDownIrqLine(IntSource source);
template void    CPU<C64Memory>::releaseIrqLine(IntSource source);
template void    CPU<C64Memory>::setRDY(bool value);

template         CPU<DriveMemory>::CPU(C64& ref, DriveMemory& memref);
template CPUInfo CPU<DriveMemory>::getInfo() const;
template void    CPU<DriveMemory>::_dump(Category category, std::ostream& os) const;
template void    CPU<DriveMemory>::_debugOn();
template void    CPU<DriveMemory>::_debugOff();
template void    CPU<DriveMemory>::_reset(bool hard);
template void    CPU<DriveMemory>::_inspect() const;
template u8      CPU<DriveMemory>::getP() const;
template u8      CPU<DriveMemory>::getPWithClearedB() const;
template void    CPU<DriveMemory>::setP(u8 p);
template void    CPU<DriveMemory>::setPWithoutB(u8 p);
template void    CPU<DriveMemory>::pullDownNmiLine(IntSource source);
template void    CPU<DriveMemory>::releaseNmiLine(IntSource source);
template void    CPU<DriveMemory>::pullDownIrqLine(IntSource source);
template void    CPU<DriveMemory>::releaseIrqLine(IntSource source);
template void    CPU<DriveMemory>::setRDY(bool value);
