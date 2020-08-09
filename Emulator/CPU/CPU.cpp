// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "C64.h"

CPU::CPU(C64& ref) : C64Component(ref)
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
        { &flags,              sizeof(flags),        CLEAR_ON_RESET },
        { &cycle,              sizeof(cycle),        CLEAR_ON_RESET },
        { &halted,             sizeof(halted),       CLEAR_ON_RESET },
        { &next,               sizeof(next),         CLEAR_ON_RESET },

        { &reg.a,              sizeof(reg.a),        CLEAR_ON_RESET },
        { &reg.x,              sizeof(reg.x),        CLEAR_ON_RESET },
        { &reg.y,              sizeof(reg.y),        CLEAR_ON_RESET },
        { &reg.pc,             sizeof(reg.pc),       CLEAR_ON_RESET },
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
        { &pc,                 sizeof(pc),           CLEAR_ON_RESET },
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

/*
DisassembledInstruction
CPU::getInstrInfo(long nr, u16 start)
{
    // Update the cache if necessary
    if (instrStart != start) _inspect(start);

    return getInstrInfo(nr);
}

DisassembledInstruction
CPU::getInstrInfo(long nr)
{
    assert(nr < CPUINFO_INSTR_COUNT);
    
    DisassembledInstruction result;
    synchronized { result = instr[nr]; }
    return result;
}

DisassembledInstruction
CPU::getLoggedInstrInfo(long nr)
{
    assert(nr < CPUINFO_INSTR_COUNT);
    
    DisassembledInstruction result;
    synchronized { result = loggedInstr[nr]; }
    return result;
}
*/

void
CPU::_reset()
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

void
CPU::_inspect()
{    
    synchronized {
        
        info.cycle = cycle;

        info.pc = pc;
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
        info.halted = isHalted();
        
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

void
CPU::_setDebug(bool enable)
{
    if (enable && isC64CPU()) {
        flags |= CPU::CPU_LOG_INSTRUCTION;
    } else {
        flags |= ~CPU::CPU_LOG_INSTRUCTION;
    }
}

void 
CPU::_dump()
{
    // DisassembledInstruction instr = debugger.disassemble();
    
	msg("CPU:\n");
	msg("----\n\n");
    /*
    msg("%s: %s %s %s   %s %s %s %s %s %s\n",
        instr.pc,
        instr.byte1, instr.byte2, instr.byte3,
        instr.a, instr.x, instr.y, instr.sp,
        instr.flags,
        instr.command);
    */
	msg("      Rdy line : %s\n", rdyLine ? "high" : "low");
    msg("      Nmi line : %02X\n", nmiLine);
    msg(" Edge detector : %02X\n", edgeDetector.current());
    msg("         doNmi : %s\n", doNmi ? "yes" : "no");
    msg("      Irq line : %02X\n", irqLine);
    msg("Level detector : %02X\n", levelDetector.current());
    msg("         doIrq : %s\n", doIrq ? "yes" : "no");
    msg("   IRQ routine : %02X%02X\n", spypeek(0xFFFF), spypeek(0xFFFE));
    msg("   NMI routine : %02X%02X\n", spypeek(0xFFFB), spypeek(0xFFFA));
	msg("\n");
    
    pport.dump();
}

size_t
CPU::stateSize()
{
    return HardwareComponent::stateSize()
    + levelDetector.stateSize()
    + edgeDetector.stateSize();
}

void
CPU::didLoadFromBuffer(u8 **buffer)
{
    levelDetector.loadFromBuffer(buffer);
    edgeDetector.loadFromBuffer(buffer);
}

void
CPU::didSaveToBuffer(u8 **buffer)
{
    levelDetector.saveToBuffer(buffer);
    edgeDetector.saveToBuffer(buffer);
}

u8
CPU::getP()
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
CPU::getPWithClearedB()
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
        if (!rdyLine) rdyLineDown = cycle;
    }
    else
    {
        rdyLine = value;
        if (rdyLine) rdyLineUp = cycle;
    }
}

void
CPU::processFlags()
{
    // Record the instruction if requested
    if (flags & CPU_LOG_INSTRUCTION) {
        debugger.logInstruction();
    }
    
    // Check if a breakpoint has been reached
    if (flags & CPU_LOG_INSTRUCTION && debugger.breakpointMatches(reg.pc)) {
        c64.signalBreakpoint();
    }
}
