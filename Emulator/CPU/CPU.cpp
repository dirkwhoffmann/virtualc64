// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "C64.h"

CPU::CPU(CPUModel model, C64& ref, Memory &memref) : C64Component(ref), mem(memref)
{
    this->model = model;
	
    setDescription(model == MOS_6502 ? "CPU(6502)" : "CPU");
    
    subComponents = vector<HardwareComponent *> {
        
        &pport,
        &debugger
    };
    
    // Chip model
    model = MOS_6510;

	// Establish callback for each instruction
	registerInstructions();
		    
    // Register snapshot items
    SnapshotItem items[] = {
        
        // Lifetime items
        { &model,              sizeof(model),        KEEP_ON_RESET },

         // Internal state
        { &cycle,              sizeof(cycle),        CLEAR_ON_RESET },
        { &errorState,         sizeof(errorState),   CLEAR_ON_RESET },
        { &next,               sizeof(next),         CLEAR_ON_RESET },

        { &regA,               sizeof(regA),         CLEAR_ON_RESET },
        { &regX,               sizeof(regX),         CLEAR_ON_RESET },
        { &regY,               sizeof(regY),         CLEAR_ON_RESET },
        { &regPC,              sizeof(regPC),        CLEAR_ON_RESET },
        { &regSP,              sizeof(regSP),        CLEAR_ON_RESET },
        { &regP,               sizeof(regP),         CLEAR_ON_RESET },
        { &regADL,             sizeof(regADL),       CLEAR_ON_RESET },
        { &regADH,             sizeof(regADH),       CLEAR_ON_RESET },
        { &regIDL,             sizeof(regIDL),       CLEAR_ON_RESET },
        { &regD,               sizeof(regD),         CLEAR_ON_RESET },
        { &overflow,           sizeof(overflow),     CLEAR_ON_RESET },
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

DisassembledInstruction
CPU::getInstrInfo(long nr, u16 start)
{
    // Update the cache if necessary
    if (info.start != start) _inspect(start);

    return getInstrInfo(nr);
}

DisassembledInstruction
CPU::getInstrInfo(long nr)
{
    assert(nr < CPUINFO_INSTR_COUNT);
    
    DisassembledInstruction result;
    synchronized { result = info.instr[nr]; }
    return result;
}

DisassembledInstruction
CPU::getLoggedInstrInfo(long nr)
{
    assert(nr < CPUINFO_INSTR_COUNT);
    
    DisassembledInstruction result;
    synchronized { result = info.loggedInstr[nr]; }
    return result;
}

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
    
    // Enable of disable guard checking
    checkForBreakpoints = debugger.breakpoints.elements() != 0;
    checkForWatchpoints = debugger.watchpoints.elements() != 0;
    
    clearTraceBuffer();
}

void
CPU::_inspect()
{
    _inspect(getPC());
}

void
CPU::_inspect(u32 dasmStart)
{
    synchronized {
        
        info.cycle = cycle;

        info.pc = pc;
        info.sp = regSP;
        info.a = regA;
        info.x = regX;
        info.y = regY;
        
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
        info.start = dasmStart;
        for (unsigned i = 0; i < CPUINFO_INSTR_COUNT; i++) {
            info.instr[i] = debugger.disassemble(dasmStart);
            dasmStart += info.instr[i].size;
        }
        
        // Disassemble the most recent entries in the trace buffer
        long count = recordedInstructions();
        for (int i = 0; i < count; i++) {
            RecordedInstruction rec = readRecordedInstruction(i);
            info.loggedInstr[i] = debugger.disassemble(rec);
        }
    }
}

void
CPU::_setDebug(bool enable)
{
    if (enable) {
        
        msg("Enabling debug mode\n");
        logInstructions = true;
        
    } else {
        
        msg("Disabling debug mode\n");
        logInstructions = false;
    }
}

void 
CPU::_dump()
{
    DisassembledInstruction instr = debugger.disassemble();
    
	msg("CPU:\n");
	msg("----\n\n");
    msg("%s: %s %s %s   %s %s %s %s %s %s\n",
        instr.pc,
        instr.byte1, instr.byte2, instr.byte3,
        instr.a, instr.x, instr.y, instr.sp,
        instr.flags,
        instr.command);
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
CPU::setErrorState(ErrorState state)
{
	if (errorState == state) return;

    errorState = state;
    
    switch (errorState) {
        case CPU_OK:
            c64.putMessage(MSG_CPU_OK);
            return;
        case CPU_BREAKPOINT_REACHED:
            c64.putMessage(MSG_BREAKPOINT_REACHED);
            return; 
        case CPU_ILLEGAL_INSTRUCTION:
            c64.putMessage(MSG_ILLEGAL_INSTRUCTION);
            return;
        default:
            assert(false);
    }
}

unsigned
CPU::recordedInstructions()
{
    return writePtr >= readPtr ? writePtr - readPtr : traceBufferSize + writePtr - readPtr;
}

void
CPU::recordInstruction()
{
    RecordedInstruction i;
    u8 opcode = mem.spypeek(pc);
    unsigned length = debugger.getLengthOfInstruction(opcode);
    
    i.cycle = cycle;
    i.pc = pc;
    i.byte1 = opcode;
    i.byte2 = length > 1 ? mem.spypeek(i.pc + 1) : 0;
    i.byte3 = length > 2 ? mem.spypeek(i.pc + 2) : 0;
    i.a = regA;
    i.x = regX;
    i.y = regY;
    i.sp = regSP;
    i.flags = getP();
    
    assert(writePtr < traceBufferSize);

    traceBuffer[writePtr] = i;
    writePtr = (writePtr + 1) % traceBufferSize;
    if (writePtr == readPtr) {
        readPtr = (readPtr + 1) % traceBufferSize;
    }
}

RecordedInstruction
CPU::readRecordedInstruction()
{
    assert(recordedInstructions() != 0);
    assert(readPtr < traceBufferSize);
    
    RecordedInstruction result = traceBuffer[readPtr];
    readPtr = (readPtr + 1) % traceBufferSize;
    
    return result;
}

RecordedInstruction
CPU::readRecordedInstruction(unsigned previous)
{
    return traceBuffer[(writePtr + traceBufferSize - previous - 1) % traceBufferSize];
}
