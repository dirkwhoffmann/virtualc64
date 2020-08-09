// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

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
		    
    // Register snapshot items
    SnapshotItem items[] = {
        
         // Internal state
        { &cycle,              sizeof(cycle),        CLEAR_ON_RESET },
        { &next,               sizeof(next),         CLEAR_ON_RESET },

        { &reg.a,              sizeof(reg.a),        CLEAR_ON_RESET },
        { &reg.x,              sizeof(reg.x),        CLEAR_ON_RESET },
        { &reg.y,              sizeof(reg.y),        CLEAR_ON_RESET },
        { &reg.pc,             sizeof(reg.pc),       CLEAR_ON_RESET },
        { &reg.pc0,            sizeof(reg.pc0),      CLEAR_ON_RESET },
        { &reg.sp,             sizeof(reg.sp),       CLEAR_ON_RESET },
        // { &reg.p,           sizeof(reg.p),        CLEAR_ON_RESET },
        { &reg.sr.n,           sizeof(reg.sr.n),     CLEAR_ON_RESET },
        { &reg.sr.v,           sizeof(reg.sr.v),     CLEAR_ON_RESET },
        { &reg.sr.b,           sizeof(reg.sr.b),     CLEAR_ON_RESET },
        { &reg.sr.d,           sizeof(reg.sr.d),     CLEAR_ON_RESET },
        { &reg.sr.i,           sizeof(reg.sr.i),     CLEAR_ON_RESET },
        { &reg.sr.z,           sizeof(reg.sr.z),     CLEAR_ON_RESET },
        { &reg.sr.c,           sizeof(reg.sr.c),     CLEAR_ON_RESET },

        { &reg.adl,            sizeof(reg.adl),      CLEAR_ON_RESET },
        { &reg.adh,            sizeof(reg.adh),      CLEAR_ON_RESET },
        { &reg.idl,            sizeof(reg.idl),      CLEAR_ON_RESET },
        { &reg.d,              sizeof(reg.d),        CLEAR_ON_RESET },
        { &reg.ovl,            sizeof(reg.ovl),      CLEAR_ON_RESET },
        { &rdyLine,            sizeof(rdyLine),      CLEAR_ON_RESET },
        { &rdyLineUp,          sizeof(rdyLineUp),    CLEAR_ON_RESET },
        { &rdyLineDown,        sizeof(rdyLineDown),  CLEAR_ON_RESET },
        { &nmiLine,            sizeof(nmiLine),      CLEAR_ON_RESET },
        { &irqLine,            sizeof(irqLine),      CLEAR_ON_RESET },
        { &doNmi,              sizeof(doNmi),        CLEAR_ON_RESET },
        { &doIrq,              sizeof(doIrq),        CLEAR_ON_RESET },
        { NULL,                0,                    0 }};
    
    registerSnapshotItems(items, sizeof(items));
}

template <typename M> void
CPU<M>::_reset()
{
    // Clear snapshot items marked with 'CLEAR_ON_RESET'
     if (snapshotItems != NULL)
         for (unsigned i = 0; snapshotItems[i].data != NULL; i++)
             if (snapshotItems[i].flags & CLEAR_ON_RESET)
                 memset(snapshotItems[i].data, 0, snapshotItems[i].size);
    
    setB(1);
	rdyLine = true;
	next = fetch;
    levelDetector.clear();
    edgeDetector.clear();
}

template <typename M> void
CPU<M>::_inspect()
{    
    synchronized {
        
        info.cycle = cycle;

        info.pc0 = reg.pc0;
        info.sp = reg.sp;
        info.a = reg.a;
        info.x = reg.x;
        info.y = reg.y;
        
        info.nFlag = getN();
        info.vFlag = getV();
        info.bFlag = getB();
        info.dFlag = getD();
        info.iFlag = getI();
        info.zFlag = getZ();
        info.cFlag = getC();
        
        info.irq = irqLine;
        info.nmi = nmiLine;
        info.rdy = rdyLine;
        info.halted = isJammed();
        
        info.processorPort = pport.read();
        info.processorPortDir = pport.readDirection();
        
        // Disassemble the program starting at 'dasmStart'
        /*
        instrStart = dasmStart;
        for (unsigned i = 0; i < CPUINFO_INSTR_COUNT; i++) {
            instr[i] = debugger.disassemble(dasmStart);
            dasmStart += instr[i].size;
        }
        
        // Disassemble the most recent entries in the trace buffer
        long count = debugger.loggedInstructions();
        for (int i = 0; i < count; i++) {
            RecordedInstruction rec = debugger.logEntryAbs(i);
            loggedInstr[i] = debugger.disassemble(rec);
        }
        */
    }
}

template <typename M> void
CPU<M>::_setDebug(bool enable)
{
    // We only allow the C64 CPU to run in debug mode
    if (isC64CPU()) { debugMode = enable; }
}

template <typename M> void
CPU<M>::_dump()
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
    
    pport.dump();
}

template <typename M> size_t
CPU<M>::stateSize()
{
    return HardwareComponent::stateSize()
    + levelDetector.stateSize()
    + edgeDetector.stateSize();
}

template <typename M> void
CPU<M>::didLoadFromBuffer(u8 **buffer)
{
    levelDetector.loadFromBuffer(buffer);
    edgeDetector.loadFromBuffer(buffer);
}

template <typename M> void
CPU<M>::didSaveToBuffer(u8 **buffer)
{
    levelDetector.saveToBuffer(buffer);
    edgeDetector.saveToBuffer(buffer);
}

template <typename M> u8
CPU<M>::getP()
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
CPU<M>::getPWithClearedB()
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

template         CPU<C64Memory>::CPU(C64& ref, C64Memory& memref);
template CPUInfo CPU<C64Memory>::getInfo();
template void    CPU<C64Memory>::_dump();
template void    CPU<C64Memory>::_setDebug(bool enable);
template void    CPU<C64Memory>::_reset();
template void    CPU<C64Memory>::_inspect();
template size_t  CPU<C64Memory>::stateSize();
template void    CPU<C64Memory>::didLoadFromBuffer(u8 **buffer);
template void    CPU<C64Memory>::didSaveToBuffer(u8 **buffer);
template u8      CPU<C64Memory>::getP();
template u8      CPU<C64Memory>::getPWithClearedB();
template void    CPU<C64Memory>::setP(u8 p);
template void    CPU<C64Memory>::setPWithoutB(u8 p);
template void    CPU<C64Memory>::pullDownNmiLine(IntSource source);
template void    CPU<C64Memory>::releaseNmiLine(IntSource source);
template void    CPU<C64Memory>::pullDownIrqLine(IntSource source);
template void    CPU<C64Memory>::releaseIrqLine(IntSource source);
template void    CPU<C64Memory>::setRDY(bool value);

template         CPU<DriveMemory>::CPU(C64& ref, DriveMemory& memref);
template CPUInfo CPU<DriveMemory>::getInfo();
template void    CPU<DriveMemory>::_dump();
template void    CPU<DriveMemory>::_setDebug(bool enable);
template void    CPU<DriveMemory>::_reset();
template void    CPU<DriveMemory>::_inspect();
template size_t  CPU<DriveMemory>::stateSize();
template void    CPU<DriveMemory>::didLoadFromBuffer(u8 **buffer);
template void    CPU<DriveMemory>::didSaveToBuffer(u8 **buffer);
template u8      CPU<DriveMemory>::getP();
template u8      CPU<DriveMemory>::getPWithClearedB();
template void    CPU<DriveMemory>::setP(u8 p);
template void    CPU<DriveMemory>::setPWithoutB(u8 p);
template void    CPU<DriveMemory>::pullDownNmiLine(IntSource source);
template void    CPU<DriveMemory>::releaseNmiLine(IntSource source);
template void    CPU<DriveMemory>::pullDownIrqLine(IntSource source);
template void    CPU<DriveMemory>::releaseIrqLine(IntSource source);
template void    CPU<DriveMemory>::setRDY(bool value);
