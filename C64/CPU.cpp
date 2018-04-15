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
        { &nmiLine,                 sizeof(nmiLine),                CLEAR_ON_RESET },
        { &irqLine,                 sizeof(irqLine),                CLEAR_ON_RESET },
        { &edgeDetector,            sizeof(edgeDetector),           CLEAR_ON_RESET },
        { &levelDetector,           sizeof(levelDetector),          CLEAR_ON_RESET },
        { &doNmi,                   sizeof(doNmi),                  CLEAR_ON_RESET },
        { &doIrq,                   sizeof(doIrq),                  CLEAR_ON_RESET },
        { &errorState,              sizeof(errorState),             CLEAR_ON_RESET },
        { &callStack,               sizeof(callStack),              CLEAR_ON_RESET | WORD_FORMAT },
        { &callStackPointer,        sizeof(callStackPointer),       CLEAR_ON_RESET },
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
}

void 
CPU::dumpState()
{
    DisassembledInstruction instr = disassemble();
    
	msg("CPU:\n");
	msg("----\n\n");
    msg("%s\n", instr.formatted);
	msg("      Rdy line : %s\n", rdyLine ? "high" : "low");
    msg("      Nmi line : %02X\n", nmiLine);
    msg(" Edge detector : %02X\n", read8_delayed(edgeDetector));
    msg("         doNmi : %s\n", doNmi ? "yes" : "no");
    msg("      Irq line : %02X\n", irqLine);
    msg("Level detector : %02X\n", read8_delayed(levelDetector));
    msg("         doIrq : %s\n", doIrq ? "yes" : "no");
	msg("   IRQ routine : %02X%02X\n", mem->read(0xFFFF), mem->read(0xFFFE));
	msg("   NMI routine : %02X%02X\n", mem->read(0xFFFB), mem->read(0xFFFA));
	msg("\n");
    
    c64->processorPort.dumpState();
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

int 
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

DisassembledInstruction
CPU::disassemble(uint16_t addr, bool hex)
{
    DisassembledInstruction instr;
    
    // Get opcode
    uint8_t opcode = mem->read(addr);
    
    // Get operand
    uint16_t operand;
    switch (addressingMode[opcode]) {
        case ADDR_IMMEDIATE:
        case ADDR_ZERO_PAGE:
        case ADDR_ZERO_PAGE_X:
        case ADDR_ZERO_PAGE_Y:
        case ADDR_INDIRECT_X:
        case ADDR_INDIRECT_Y:
            operand = mem->read(addr+1);
            break;
        case ADDR_DIRECT:
        case ADDR_INDIRECT:
        case ADDR_ABSOLUTE:
        case ADDR_ABSOLUTE_X:
        case ADDR_ABSOLUTE_Y:
            operand = LO_HI(mem->read(addr+1),mem->read(addr+2));
            break;
        case ADDR_RELATIVE:
            operand = addr + 2 + mem->read(addr+1);
            break;
        default:
            operand = 0;
            break;
    }
    
    // Convert register contents to strings
    sprintf(instr.pc, (hex ? "%04X" : "%05d"), addr);
    sprintf(instr.A,  (hex ? "%02X" : "%03d"), A);
    sprintf(instr.X,  (hex ? "%02X" : "%03d"), X);
    sprintf(instr.Y,  (hex ? "%02X" : "%03d"), Y);
    sprintf(instr.SP, (hex ? "%02X" : "%03d"), SP);

    // Convert memory contents to strings
    for (unsigned i = 0; i < 3; i++) {
        if (i < getLengthOfInstruction(opcode)) {
            sprintf(instr.byte[i], (hex ? "%02X" : "%03d"), mem->read(addr+i));
        } else {
            sprintf(instr.byte[i], (hex ? "  " : "   "));
        }
    }
    
    // Convert flags to a string
    sprintf(instr.flags, "%c%c-%c%c%c%c%c",
            N ? 'N' : 'n',
            V ? 'V' : 'v',
            B ? 'B' : 'b',
            D ? 'D' : 'd',
            I ? 'I' : 'i',
            Z ? 'Z' : 'z',
            C ? 'C' : 'c');

    // Convert instruction to a string
    const char *mnc = getMnemonic(opcode);
    switch (addressingMode[opcode]) {
        case ADDR_IMPLIED:
        case ADDR_ACCUMULATOR:
            sprintf(instr.instr, "%s", mnc);
            break;
        case ADDR_IMMEDIATE:
            sprintf(instr.instr, (hex ? "%s #%02X" : "%s #%03d"), mnc, operand);
            break;
        case ADDR_ZERO_PAGE:
            sprintf(instr.instr, (hex ? "%s %02X" : "%s %03d"), mnc, operand);
            break;
        case ADDR_ZERO_PAGE_X:
            sprintf(instr.instr, (hex ? "%s %02X,X" : "%s %03d,X"), mnc, operand);
            break;
        case ADDR_ZERO_PAGE_Y:
            sprintf(instr.instr, (hex ? "%s %02X,Y" : "%s %03d,Y"), mnc, operand);
            break;
        case ADDR_ABSOLUTE:
        case ADDR_DIRECT:
            sprintf(instr.instr, (hex ? "%s %04X" : "%s %05d"), mnc, operand);
            break;
        case ADDR_ABSOLUTE_X:
            sprintf(instr.instr, (hex ? "%s %04X,X" : "%s %05d,X"), mnc, operand);
            break;
        case ADDR_ABSOLUTE_Y:
            sprintf(instr.instr, (hex ? "%s %04X,Y" : "%s %05d,Y"), mnc, operand);
            break;
        case ADDR_INDIRECT:
            sprintf(instr.instr, (hex ? "%s (%04X)" : "%s (%05d)"), mnc, operand);
            break;
        case ADDR_INDIRECT_X:
            sprintf(instr.instr, (hex ? "%s (%04X,X)" : "%s (%05d,X)"), mnc, operand);
            break;
        case ADDR_INDIRECT_Y:
            sprintf(instr.instr, (hex ? "%s (%04X),Y" : "%s (%05d),Y"), mnc, operand);
            break;
        case ADDR_RELATIVE:
            sprintf(instr.instr, (hex ? "%s %04X" : "%s %05d"), mnc, operand);
            break;
        default:
            sprintf(instr.instr, "???");
    }
    
    // Put everything together
    sprintf(instr.formatted, "%s: %s %s %s   %s %s %s %s %s %s",
            instr.pc,
            instr.byte[0], instr.byte[1], instr.byte[2],
            instr.A, instr.X, instr.Y, instr.SP,
            instr.flags,
            instr.instr);
    
    return instr;
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


