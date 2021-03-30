// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "C64.h"

template <typename M>
CPU<M>::CPU(C64& ref, M& memref) : C64Component(ref), mem(memref)
{
    subComponents = vector<HardwareComponent *> {
        
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
CPU<M>::_reset()
{
    RESET_SNAPSHOT_ITEMS
    
    setB(1);
	rdyLine = true;
	next = fetch;
    
    // This should not be necessary. Delete it.
    levelDetector.clear();
    edgeDetector.clear();
}

template <typename M> void
CPU<M>::_inspect()
{    
    synchronized {
        
        info.cycle = cycle;
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
CPU<M>::_setDebug(bool enable)
{
    // We only allow the C64 CPU to run in debug mode
    if (isC64CPU()) { debugMode = enable; }
}

template <typename M> void
CPU<M>::_dump() const
{
	msg("CPU:\n");
	msg("----\n\n");

	msg("      Rdy line : %s\n", rdyLine ? "high" : "low");
    msg("      Nmi line : %02X\n", nmiLine);
    msg(" Edge detector : %02X\n", edgeDetector.current());
    msg("         doNmi : %s\n", doNmi ? "yes" : "no");
    msg("      Irq line : %02X\n", irqLine);
    msg("Level detector : %02X\n", levelDetector.current());
    msg("         doIrq : %s\n", doIrq ? "yes" : "no");
    msg("   IRQ routine : %02X%02X\n", mem.spypeek(0xFFFF), mem.spypeek(0xFFFE));
    msg("   NMI routine : %02X%02X\n", mem.spypeek(0xFFFB), mem.spypeek(0xFFFA));
	msg("\n");
    
    // pport.dump();
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
        if (!rdyLine) rdyLineDown = cycle;
    }
    else
    {
        rdyLine = value;
        if (rdyLine) rdyLineUp = cycle;
    }
}


//
// Template instantiations
//

template         CPU<C64Memory>::CPU(C64& ref, C64Memory& memref);
template CPUInfo CPU<C64Memory>::getInfo();
template void    CPU<C64Memory>::_dump() const;
template void    CPU<C64Memory>::_setDebug(bool enable);
template void    CPU<C64Memory>::_reset();
template void    CPU<C64Memory>::_inspect();
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
template CPUInfo CPU<DriveMemory>::getInfo();
template void    CPU<DriveMemory>::_dump() const;
template void    CPU<DriveMemory>::_setDebug(bool enable);
template void    CPU<DriveMemory>::_reset();
template void    CPU<DriveMemory>::_inspect();
template u8      CPU<DriveMemory>::getP() const;
template u8      CPU<DriveMemory>::getPWithClearedB() const;
template void    CPU<DriveMemory>::setP(u8 p);
template void    CPU<DriveMemory>::setPWithoutB(u8 p);
template void    CPU<DriveMemory>::pullDownNmiLine(IntSource source);
template void    CPU<DriveMemory>::releaseNmiLine(IntSource source);
template void    CPU<DriveMemory>::pullDownIrqLine(IntSource source);
template void    CPU<DriveMemory>::releaseIrqLine(IntSource source);
template void    CPU<DriveMemory>::setRDY(bool value);
