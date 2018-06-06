/*
 * (C) Dirk W. Hoffmann. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "C64.h"

CPU::CPU()
{	
	setDescription("CPU");
	debug(3, "  Creating CPU at address %p...\n", this);
	
    // Chip model
    chipModel = MOS_6510;

	// Establish callback for each instruction
	registerInstructions();
		
	// Clear all breakpoint tags
	for (int i = 0; i <  65536; i++) {
		breakpoint[i] = NO_BREAKPOINT;	
	}
    
    // Register snapshot items
    SnapshotItem items[] = {
        
        // Lifetime items
        { &chipModel,               sizeof(chipModel), KEEP_ON_RESET },

         // Internal state
        { &A,                       sizeof(A),                      CLEAR_ON_RESET },
        { &X,                       sizeof(X),                      CLEAR_ON_RESET },
        { &Y,                       sizeof(Y),                      CLEAR_ON_RESET },
        { &PC,                      sizeof(PC),                     CLEAR_ON_RESET },
        { &PC_at_cycle_0,           sizeof(PC_at_cycle_0),          CLEAR_ON_RESET },
        { &SP,                      sizeof(SP),                     CLEAR_ON_RESET },
        { &N,                       sizeof(N),                      CLEAR_ON_RESET },
        { &V,                       sizeof(V),                      CLEAR_ON_RESET },
        { &B,                       sizeof(B),                      CLEAR_ON_RESET },
        { &D,                       sizeof(D),                      CLEAR_ON_RESET },
        { &I,                       sizeof(I),                      CLEAR_ON_RESET },
        { &Z,                       sizeof(Z),                      CLEAR_ON_RESET },
        { &C,                       sizeof(C),                      CLEAR_ON_RESET },
        { &opcode,                  sizeof(opcode),                 CLEAR_ON_RESET },
        { &next,                    sizeof(next),                   CLEAR_ON_RESET },
        { &addr_lo,                 sizeof(addr_lo),                CLEAR_ON_RESET },
        { &addr_hi,                 sizeof(addr_hi),                CLEAR_ON_RESET },
        { &ptr,                     sizeof(ptr),                    CLEAR_ON_RESET },
        { &pc_lo,                   sizeof(pc_lo),                  CLEAR_ON_RESET },
        { &pc_hi,                   sizeof(pc_hi),                  CLEAR_ON_RESET },
        { &overflow,                sizeof(overflow),               CLEAR_ON_RESET },
        { &data,                    sizeof(data),                   CLEAR_ON_RESET },
        { &rdyLine,                 sizeof(rdyLine),                CLEAR_ON_RESET },
        { &rdyLineUp,               sizeof(rdyLineUp),              CLEAR_ON_RESET },
        { &rdyLineDown,             sizeof(rdyLineDown),            CLEAR_ON_RESET },
        { &nmiLine,                 sizeof(nmiLine),                CLEAR_ON_RESET },
        { &irqLine,                 sizeof(irqLine),                CLEAR_ON_RESET },
        { &edgeDetector,            sizeof(edgeDetector),           CLEAR_ON_RESET },
        { &levelDetector,           sizeof(levelDetector),          CLEAR_ON_RESET },
        { &doNmi,                   sizeof(doNmi),                  CLEAR_ON_RESET },
        { &doIrq,                   sizeof(doIrq),                  CLEAR_ON_RESET },
        { &errorState,              sizeof(errorState),             CLEAR_ON_RESET },
        // { &callStack,               sizeof(callStack),              CLEAR_ON_RESET | WORD_FORMAT },
        // { &callStackPointer,        sizeof(callStackPointer),       CLEAR_ON_RESET },
        { NULL,                     0,                              0 }};
    
    registerSnapshotItems(items, sizeof(items));
}

CPU::~CPU()
{
	debug(3, "  Releasing CPU...\n");
}

void
CPU::reset()
{
    VirtualComponent::reset();

    B = 1;
	rdyLine = true;
	next = fetch;

    clearTraceBuffer();
}

void 
CPU::dumpState()
{
    DisassembledInstruction instr = disassemble(true /* hex output */);
    
	msg("CPU:\n");
	msg("----\n\n");
    msg("%s: %s %s %s   %s %s %s %s %s %s\n",
        instr.pc,
        instr.byte1, instr.byte2, instr.byte3,
        instr.A, instr.X, instr.Y, instr.SP,
        instr.flags,
        instr.command);
	msg("      Rdy line : %s\n", rdyLine ? "high" : "low");
    msg("      Nmi line : %02X\n", nmiLine);
    msg(" Edge detector : %02X\n", read8_delayed(edgeDetector));
    msg("         doNmi : %s\n", doNmi ? "yes" : "no");
    msg("      Irq line : %02X\n", irqLine);
    msg("Level detector : %02X\n", read8_delayed(levelDetector));
    msg("         doIrq : %s\n", doIrq ? "yes" : "no");
	msg("   IRQ routine : %02X%02X\n", mem->snoop(0xFFFF), mem->snoop(0xFFFE));
	msg("   NMI routine : %02X%02X\n", mem->snoop(0xFFFB), mem->snoop(0xFFFA));
	msg("\n");
    
    c64->processorPort.dumpState();
}

CPUInfo
CPU::getInfo()
{
    CPUInfo info;
    
    info.cycle = c64->cycle;
    info.pc = PC_at_cycle_0;
    info.a = A;
    info.x = X;
    info.y = Y;
    info.sp = SP;
    info.nFlag = N;
    info.vFlag = V;
    info.bFlag = B;
    info.dFlag = D;
    info.iFlag = I;
    info.zFlag = Z;
    info.cFlag = C;
  
    return info;
}

void
CPU::pullDownNmiLine(InterruptSource bit)
{
    assert(bit != 0);
    
    // Check for falling edge on physical line
    if (!nmiLine)
        write8_delayed(edgeDetector, 1);
    
    nmiLine |= bit;
}

void
CPU::releaseNmiLine(InterruptSource source)
{
    nmiLine &= ~source;
}

void
CPU::pullDownIrqLine(InterruptSource source)
{
	assert(source != 0);
    
	irqLine |= source;
    write8_delayed(levelDetector, irqLine);
}

void
CPU::releaseIrqLine(InterruptSource source)
{
    irqLine &= ~source;
    write8_delayed(levelDetector, irqLine);
}

void
CPU::setRDY(bool value)
{
    if (rdyLine)
    {
        rdyLine = value;
        if (!rdyLine) rdyLineDown = c64->cycle;
    }
    else
    {
        rdyLine = value;
        if (rdyLine) rdyLineUp = c64->cycle;
    }
}

// Instruction set
const char 
*CPU::getMnemonic(uint8_t opcode)
{
	return mnemonic[opcode];
}

AddressingMode
CPU::getAddressingMode(uint8_t opcode)
{
	return addressingMode[opcode];
}

unsigned
CPU::getLengthOfInstruction(uint8_t opcode)
{
	switch(addressingMode[opcode]) {
		case ADDR_IMPLIED:			
		case ADDR_ACCUMULATOR:
			return 1;
		case ADDR_IMMEDIATE:
		case ADDR_ZERO_PAGE:
		case ADDR_ZERO_PAGE_X:
		case ADDR_ZERO_PAGE_Y:
		case ADDR_INDIRECT_X:
		case ADDR_INDIRECT_Y:
		case ADDR_RELATIVE:
			return 2;
		case ADDR_ABSOLUTE:
		case ADDR_ABSOLUTE_X:
		case ADDR_ABSOLUTE_Y:
		case ADDR_DIRECT:
		case ADDR_INDIRECT:
			return 3;
	}
	return 1;
}


void 
CPU::setErrorState(ErrorState state)
{
	if (errorState == state)
        return;

    errorState = state;
    
    switch (errorState) {
        case CPU_OK:
            c64->putMessage(MSG_CPU_OK);
            return;
        case CPU_SOFT_BREAKPOINT_REACHED:
            c64->putMessage(MSG_CPU_SOFT_BREAKPOINT_REACHED);
            return; 
        case CPU_HARD_BREAKPOINT_REACHED:
            c64->putMessage(MSG_CPU_HARD_BREAKPOINT_REACHED);
            return;
        case CPU_ILLEGAL_INSTRUCTION:
            c64->putMessage(MSG_CPU_ILLEGAL_INSTRUCTION);
            return;
        default:
            assert(false);
    }
}

void
CPU::recordInstruction()
{
    RecordedInstruction i;
    uint8_t opcode = mem->snoop(PC_at_cycle_0);
    unsigned length = getLengthOfInstruction(opcode);
    
    i.pc = PC_at_cycle_0;
    i.byte1 = opcode;
    i.byte2 = length > 1 ? mem->snoop(i.pc + 1) : 0;
    i.byte3 = length > 2 ? mem->snoop(i.pc + 2) : 0;
    i.a = A;
    i.x = X;
    i.y = Y;
    i.sp = SP;
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
    assert(previous < recordedInstructions());    
    return traceBuffer[(writePtr + traceBufferSize - 1) % traceBufferSize];
}


DisassembledInstruction
CPU::disassemble(RecordedInstruction instr, bool hex)
{
    DisassembledInstruction result;
    
    uint8_t opcode = instr.byte1;
    uint8_t length = getLengthOfInstruction(opcode);
    
    result.addr = instr.pc;
    result.size = length;
    
    // Convert command
    char operand[6];
    switch (addressingMode[opcode]) {
            
        case ADDR_IMMEDIATE:
        case ADDR_ZERO_PAGE:
        case ADDR_ZERO_PAGE_X:
        case ADDR_ZERO_PAGE_Y:
        case ADDR_INDIRECT_X:
        case ADDR_INDIRECT_Y: {
            uint8_t value = mem->snoop(instr.pc + 1);
            hex ? sprint8x(operand, value) : sprint8d(operand, value);
            break;
        }
        case ADDR_DIRECT:
        case ADDR_INDIRECT:
        case ADDR_ABSOLUTE:
        case ADDR_ABSOLUTE_X:
        case ADDR_ABSOLUTE_Y: {
            uint16_t value = LO_HI(mem->snoop(instr.pc + 1),mem->snoop(instr.pc + 2));
            hex ? sprint16x(operand, value) : sprint16d(operand, value);
            break;
        }
        case ADDR_RELATIVE: {
            uint16_t value = instr.pc + 2 + (int8_t)mem->snoop(instr.pc + 1);
            hex ? sprint16x(operand, value) : sprint16d(operand, value);
            break;
        }
        default:
            break;
    }
    
    switch (addressingMode[opcode]) {
        case ADDR_IMPLIED:
        case ADDR_ACCUMULATOR:
            strcpy(result.command, "xxx");
            break;
        case ADDR_IMMEDIATE:
            strcpy(result.command, hex ? "xxx #hh" : "xxx #ddd");
            memcpy(&result.command[5], operand, hex ? 2 : 3);
            break;
        case ADDR_ZERO_PAGE:
            strcpy(result.command, hex ? "xxx hh" : "xxx ddd");
            memcpy(&result.command[4], operand, hex ? 2 : 3);
            break;
        case ADDR_ZERO_PAGE_X:
            strcpy(result.command, hex ? "xxx hh,X" : "xxx ddd,X");
            memcpy(&result.command[4], operand, hex ? 2 : 3);
            break;
        case ADDR_ZERO_PAGE_Y:
            strcpy(result.command, hex ? "xxx hh,Y" : "xxx ddd,Y");
            memcpy(&result.command[4], operand, hex ? 2 : 3);
            break;
        case ADDR_ABSOLUTE:
        case ADDR_DIRECT:
            strcpy(result.command, hex ? "xxx hhhh" : "xxx ddddd");
            memcpy(&result.command[4], operand, hex ? 4 : 5);
            break;
        case ADDR_ABSOLUTE_X:
            strcpy(result.command, hex ? "xxx hhhh,X" : "xxx ddddd,X");
            memcpy(&result.command[4], operand, hex ? 4 : 5);
            break;
        case ADDR_ABSOLUTE_Y:
            strcpy(result.command, hex ? "xxx hhhh,Y" : "xxx ddddd,Y");
            memcpy(&result.command[4], operand, hex ? 4 : 5);
            break;
        case ADDR_INDIRECT:
            strcpy(result.command, hex ? "xxx (hhhh)" : "xxx (ddddd)");
            memcpy(&result.command[5], operand, hex ? 4 : 5);
            break;
        case ADDR_INDIRECT_X:
            strcpy(result.command, hex ? "xxx (hh,X)" : "xxx (ddd,X)");
            memcpy(&result.command[5], operand, hex ? 2 : 3);
            break;
        case ADDR_INDIRECT_Y:
            strcpy(result.command, hex ? "xxx (hh),Y" : "xxx (ddd),Y");
            memcpy(&result.command[5], operand, hex ? 2 : 3);
            break;
        case ADDR_RELATIVE:
            strcpy(result.command, hex ? "xxx hhhh" : "xxx ddddd");
            memcpy(&result.command[4], operand, hex ? 4 : 5);
            break;
        default:
            strcpy(result.command, "???");
    }
    
    // Copy mnemonic
    const char *mnc = getMnemonic(opcode);
    strncpy(result.command, mnc, 3);
    
    // Convert register contents to strings
    hex ? sprint16x(result.pc, instr.pc) : sprint16d(result.pc, instr.pc);
    hex ? sprint8x(result.A, instr.a) : sprint8d(result.A, instr.a);
    hex ? sprint8x(result.X, instr.x) : sprint8d(result.X, instr.x);
    hex ? sprint8x(result.Y, instr.y) : sprint8d(result.Y, instr.y);
    hex ? sprint8x(result.SP, instr.sp) : sprint8d(result.SP, instr.sp);
    
    // Convert memory contents to strings
    if (length >= 1) {
        hex ? sprint8x(result.byte1, instr.byte1) : sprint8d(result.byte1, instr.byte1);
    } else {
        hex ? strcpy(result.byte1, "  ") : strcpy(result.byte1, "   ");
    }
    if (length >= 2) {
        hex ? sprint8x(result.byte2, instr.byte2) : sprint8d(result.byte2, instr.byte2);
    } else {
        hex ? strcpy(result.byte2, "  ") : strcpy(result.byte2, "   ");
    }
    if (length >= 3) {
        hex ? sprint8x(result.byte3, instr.byte3) : sprint8d(result.byte3, instr.byte3);
    } else {
        hex ? strcpy(result.byte3, "  ") : strcpy(result.byte3, "   ");
    }
    
    // Convert flags to a string
    result.flags[0] = (instr.flags & N_FLAG) ? 'N' : 'n';
    result.flags[1] = (instr.flags & V_FLAG) ? 'V' : 'v';
    result.flags[2] = '-';
    result.flags[3] = (instr.flags & B_FLAG) ? 'B' : 'b';
    result.flags[4] = (instr.flags & D_FLAG) ? 'D' : 'd';
    result.flags[5] = (instr.flags & I_FLAG) ? 'I' : 'i';
    result.flags[6] = (instr.flags & Z_FLAG) ? 'Z' : 'z';
    result.flags[7] = (instr.flags & C_FLAG) ? 'C' : 'c';
    result.flags[8] = 0;
    
    return result;
}

DisassembledInstruction
CPU::disassemble(uint16_t addr, bool hex)
{
    RecordedInstruction instr;

    instr.pc = addr;
    instr.byte1 = mem->snoop(addr);
    instr.byte2 = mem->snoop(addr + 1);
    instr.byte3 = mem->snoop(addr + 2);
    instr.a = A;
    instr.x = X;
    instr.y = Y;
    instr.sp = SP;
    instr.flags = getP();
    
    return disassemble(instr, hex);
}


