/*
 * (C) 2006 Dirk W. Hoffmann. All rights reserved.
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
	debug("  Creating CPU at address %p...\n", this);

	// Initialize connected components
	mem = NULL;
	
	// Establish callback for each instruction
	registerInstructions();
		
	// Clear all breakpoint tags
	for (int i = 0; i <  65536; i++) {
		breakpoint[i] = NO_BREAKPOINT;	
	}		
}

CPU::~CPU()
{
	debug("  Releasing CPU...\n");
}

void 
CPU::reset()
{
	debug("  Resetting CPU...\n");

	// Registers and flags
	A = 0;
	X = 0;
	Y = 0;
	PC = 0;
	PC_at_cycle_0 = 0;
	SP = 0;
	N = 0;
	V = 0;
	B = 0;
	D = 0;
	I = 0;
	Z = 0;
	C = 0;

	// Internal state
	opcode = 0;
	addr_lo = 0;
	addr_hi = 0;
	ptr = 0;
	pc_lo = 0;
	pc_hi = 0;

	// Reset external lines and IRQ stuff 
	// Note: Variables port and port_direction get their initial value via C64Memory::reset
	external_port_bits = 0x1F;
	rdyLine = true;
	irqLine = 0;
	nmiLine = 0;
	nmiNegEdge = false;
	nextPossibleIrqCycle = 0LL;
	nextPossibleNmiCycle = 0LL;
	
	errorState = OK;
	
	// TODO: Save Breakpoints
	// TODO: Save callstack
	
	callStackPointer = 0;
	oldI = 0;
	
	setTraceMode(false);
	
	// Set initial execution function
	next = &CPU::fetch;
	
	assert(mem != NULL);
}

bool 
CPU::load(FILE *file) 
{
	debug("  Loading CPU state...\n");

	// TODO
	
	return true;
}

bool
CPU::save(FILE *file) 
{
	debug("  Saving CPU state...\n");

	// Saving is only possible if the emulator has reached a clean state, i.e., it must not be in the middle of a command
	assert(next == CPU::fetch);

	// TODO
	
	return true;
}

void 
CPU::dumpState()
{
	debug("CPU:\n");
	debug("----\n\n");
    debug("%s", disassemble());
	debug("\n");
	debug("Processor port : %02X\n", port);
	debug("Port direction : %02X\n", port_direction);
	debug("      Rdy line : %s\n", rdyLine ? "high" : "low");
	debug("      Irq line : %02X\n", irqLine);
	debug("      Nmi line : %02X %s\n", nmiLine, nmiNegEdge ? "(negative edge)" : "");
	debug(" no IRQ before : %ull\n", nextPossibleIrqCycle);
	debug(" no NMI before : %ull\n", nextPossibleNmiCycle);
	debug("   IRQ routine : %02X%02X\n", mem->peek(0xFFFF), mem->peek(0xFFFE));
	debug("   NMI routine : %02X%02X\n", mem->peek(0xFFFB), mem->peek(0xFFFA));	
	debug("\n");
}

void 
CPU::setPortDirection(uint8_t value)
{
	
	port_direction = value;
	
	// TODO: A VIC byte will show up in ram[0x0000];
	// "ram[0] = TheVIC->LastVICByte;" [Frodo]

	// Store value of Bit 7, Bit 6 and Bit 3 if the corresponding bit lines are configured as outputs
	uint8_t mask = 0xC8 & port_direction;	
	external_port_bits &= ~mask;
	external_port_bits |= mask & port;	
}

void 
CPU::setPort(uint8_t value)
{
	port = value;
	
	// TODO: A VIC byte will show up in ram[0x0001];
	// "ram[1] = TheVIC->LastVICByte;" [Frodo]

	// Store value of Bit 7, Bit 6 and Bit 3 if the corresponding bit lines are configured as outputs
	uint8_t mask = 0xC8 & port_direction;	
	external_port_bits &= ~mask;
	external_port_bits |= mask & port;	
}

void 
CPU::setIRQLine(uint8_t bit) 
{ 
	assert(bit != 0);
	if (irqLine == 0) {
		// positive edge on IRQ line
		nextPossibleIrqCycle = c64->getCycles() + 2;
	}
	irqLine |= bit; 
}

bool 
CPU::IRQLineRaisedLongEnough() 
{ 
	return c64->getCycles() >= nextPossibleIrqCycle;
}

bool
CPU::IRQsAreBlocked() { 
	bool result;
	if (opcode == 0x78 /* SEI */ || opcode == 0x58 /* CLI */)
		result = oldI;
	else
		result = I;

	oldI = I;
	return result;
}

void 
CPU::setNMILine(uint8_t bit) 
{ 
	assert(bit != 0);
	if (nmiLine == 0) {
		// positive edge on NMI line
		nmiNegEdge = true;
		nextPossibleNmiCycle = c64->getCycles() + 2;
	}
	nmiLine |= bit; 
}

bool 
CPU::NMILineRaisedLongEnough() 
{ 
	return c64->getCycles() >= nextPossibleNmiCycle;
}


// Instruction set
char 
*CPU::getMnemonic(uint8_t opcode)
{
	return mnemonic[opcode];
}

CPU::AddressingMode 
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

int
CPU::getAddressOfNextIthInstruction(int i, uint16_t addr)
{
	int length = 0;
	
	// determine the instruction to display...
	do {
		addr += length;
		length = getLengthOfInstruction(mem->peek(addr));
		i--;
	} while (i >= 0);
	
	return addr;
}

char *
CPU::disassemble(uint64_t state)
{
	char buf[64], msg[128];
	int i, op;

	// Unpack state
	uint8_t y = (uint8_t)(state & 0xff); state >>= 8;
	uint8_t x = (uint8_t)(state & 0xff); state >>= 8;
	uint8_t a = (uint8_t)(state & 0xff); state >>= 8;
	uint8_t p = (uint8_t)(state & 0xff); state >>= 8;
	uint8_t sp = (uint8_t)(state & 0xff); state >>= 8;		
	uint16_t pc = (uint16_t)(state & 0xffff);
	uint8_t opcode = mem->peek(pc);	
		
	strcpy(msg, "");
	
	// Program counter
	sprintf(buf, "%04X: ", pc);
	strcat(msg, buf);
	
	// Hex dump
	for (i = 0; i < 3; i++) {
		if (i < getLengthOfInstruction(opcode)) {
			sprintf(buf, "%02X ", mem->peek(pc+i));
			strcat(msg, buf);
		} else {
			sprintf(buf, "   ");
			strcat(msg, buf);
		}
	}
	
	// Register
	sprintf(buf, "  %02X %02X %02X %02X ", a, x, y, sp);
	strcat(msg, buf);
	
	// Flags
	sprintf(buf, "%c%c%c%c%c%c%c%c ",
			(p & N_FLAG) ? 'N' : 'n',
			(p & V_FLAG) ? 'V' : 'v',
			'-',
			(p & B_FLAG) ? 'B' : 'b',
			(p & D_FLAG) ? 'D' : 'd',
			(p & I_FLAG) ? 'I' : 'i',
			(p & Z_FLAG) ? 'Z' : 'z',
			(p & C_FLAG) ? 'C' : 'c');
	strcat(msg, buf);
	
	// Mnemonic
	sprintf(buf, "%s ", getMnemonic(opcode));
	strcat(msg, buf);
	
	// Get operand as number
	switch (addressingMode[opcode]) {
		case CPU::ADDR_IMMEDIATE:
		case CPU::ADDR_ZERO_PAGE:
		case CPU::ADDR_ZERO_PAGE_X:
		case CPU::ADDR_ZERO_PAGE_Y:
		case CPU::ADDR_INDIRECT_X:
		case CPU::ADDR_INDIRECT_Y:
			op = mem->peek(pc+1);
			break;
		case CPU::ADDR_DIRECT:			
		case CPU::ADDR_INDIRECT:
		case CPU::ADDR_ABSOLUTE:
		case CPU::ADDR_ABSOLUTE_X:
		case CPU::ADDR_ABSOLUTE_Y:
			op = mem->peekWord(pc+1);
			break;
		case CPU::ADDR_RELATIVE:
			op = pc + 2 + (int8_t)mem->peek(pc+1);
			break;
		default:
			op = -1;
	}
	
	// Format operand
	switch (addressingMode[opcode]) {
		case CPU::ADDR_IMPLIED:
		case CPU::ADDR_ACCUMULATOR:
			break;
		case CPU::ADDR_IMMEDIATE:					
			sprintf(buf, "#%02X", op);
			break;
		case CPU::ADDR_ZERO_PAGE:	
			sprintf(buf, "%02X", op);
			break;
		case CPU::ADDR_ZERO_PAGE_X:	
			sprintf(buf, "%02X,X", op);
			break;
		case CPU::ADDR_ZERO_PAGE_Y:	
			sprintf(buf, "%02X,Y", op);
			break;
		case CPU::ADDR_ABSOLUTE:	
		case CPU::ADDR_DIRECT:
			sprintf(buf, "%04X", op);
			break;
		case CPU::ADDR_ABSOLUTE_X:	
			sprintf(buf, "%04X,X", op);
			break;
		case CPU::ADDR_ABSOLUTE_Y:	
			sprintf(buf, "%04X,Y", op);
			break;
		case CPU::ADDR_INDIRECT:	
			sprintf(buf, "(%04X)", op);
			break;
		case CPU::ADDR_INDIRECT_X:	
			sprintf(buf, "(%04X,X)", op);
			break;
		case CPU::ADDR_INDIRECT_Y:	
			sprintf(buf, "(%04X),Y", op);
			break;
		case CPU::ADDR_RELATIVE:
			sprintf(buf, "%04X", op);
			break;
		default:
			sprintf(buf, "???");
	}
	strcat(msg, buf);
	sprintf(buf, "\n");
	strcat(msg, buf);
	return strdup(msg);
}

char *
CPU::disassemble()
{
	return disassemble(packState());
}

void 
CPU::dumpHistory()
{
	uint8_t i = historyPtr;
	for (int j = 0; j < 256; j++) {
		debug("%s", disassemble(history[i]));
		i++;
	}
	debug("End of history trace\n");
}

CPU::ErrorState 
CPU::getErrorState() 
{ 
	return errorState; 
}

void 
CPU::setErrorState(ErrorState state)
{
	if (errorState != state) {

		// Change state
		errorState = state;
		
		// Inform listener
		getListener()->cpuAction(state);		
	}
}

void
CPU::clearErrorState() 
{ 
	setErrorState(OK);
}


