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

	// Initialize internal state
	errorState       = OK;
	cycles           = 0LL;
	nmiLine          = 0;	
	irqLine          = 0;	
	callStackPointer = 0;
	delay = 0;
	setTraceMode(false);
	
	// Initialize registers and flags
	setA(0);
	setX(0);
	setY(0);
	
	setN(0);
	setV(0);
	setB(1); 
	setD(0);
	setI(0);
	setZ(0);
	setC(0);

	assert(mem != NULL);
	// setPCL(mem->peek(0xFFFC)); 
	// setPCH(mem->peek(0xFFFD));	
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
CPU::disassemble()
{
	char buf[64], msg[128];

	int i, op;
	uint8_t opcode = mem->peek(PC);	
	
	strcpy(msg, "");
	
	// Clock cylce
	sprintf(buf, "%07d: ", cycles);
	strcat(msg, buf);
	
	// Program counter
	sprintf(buf, "%04X: ", PC);
	strcat(msg, buf);
	
	// Hex dump
	for (i = 0; i < 3; i++) {
		if (i < getLengthOfInstruction(mem->peek(PC))) {
			sprintf(buf, "%02X ", mem->peek(PC+i));
			strcat(msg, buf);
		} else {
			sprintf(buf, "   ");
			strcat(msg, buf);
		}
	}

	// Register
	sprintf(buf, "  %02X %02X %02X %02X ", A, X, Y, SP);
	strcat(msg, buf);
	
	// Flags
	sprintf(buf, "%c%c%c%c%c%c%c%c ",
		getN() ? 'N' : 'n',
		getV() ? 'V' : 'v',
		'-',
		getB() ? 'B' : 'b',
		getD() ? 'D' : 'd',
		getI() ? 'I' : 'i',
		getZ() ? 'Z' : 'z',
		getC() ? 'C' : 'c');
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
			op = mem->peek(PC+1);
			break;
		case CPU::ADDR_DIRECT:			
		case CPU::ADDR_INDIRECT:
		case CPU::ADDR_ABSOLUTE:
		case CPU::ADDR_ABSOLUTE_X:
		case CPU::ADDR_ABSOLUTE_Y:
			op = mem->peekWord(PC+1);
			break;
		case CPU::ADDR_RELATIVE:
			op = PC + 2 + (int8_t)mem->peek(PC+1);
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
			sprintf(buf, "<???>");
	}
	strcat(msg, buf);
	sprintf(buf, "\n");
	strcat(msg, buf);
	return strdup(msg);
}

bool 
CPU::load(FILE *file) 
{
	debug("  Loading CPU state...\n");
	setA(read8(file));
	setX(read8(file));
	setY(read8(file));
	setSP(read8(file));
	setP(read8(file));
	setPC(read16(file));
	cycles = read64(file);
	irqLine = read8(file);
	nmiLine = read8(file);
	setErrorState((ErrorState)read32(file));
	callStackPointer = read8(file);
	for (int i = 0; i < 256; i++)
		callStack[i] = read8(file);
	return true;
}

bool
CPU::save(FILE *file) 
{
	debug("  Saving CPU state...\n");
	write8(file, A);
	write8(file, X);
	write8(file, Y);
	write8(file, SP);
	write8(file, getP());
	write16(file, getPC());
	write64(file, cycles);
	write8(file, irqLine);
	write8(file, nmiLine);
	write32(file, (uint32_t)getErrorState());
	write8(file, callStackPointer);
	for (int i = 0; i < 256; i++)
		write8(file, callStack[i]);
	return true;
}

void 
CPU::dumpState() 
{
	debug("PC:%05X SP:%03X A:%03X X:%03X Y:%03X\n", getPC(), getSP(), getA(), getX(), getY());
	debug("N:%d V:%d B:%d D:%d I:%d Z:%d C:%d\n", getN(), getV(), getB(), getD(), getI(), getZ(), getC());
	debug("IRQ:%d NMI:%d\n", irqLine, nmiLine);
}

// Execute a single command
int
CPU::step() 
{	
	int elapsedCycles; 

	// Execute next CPU instruction
	elapsedCycles = (*this.*actionFunc[peekPC()])();
	cycles += elapsedCycles;

	// Disassemble next instruction, if requested
	if (tracingEnabled()) {
		//getListener()->logAction(disassemble());
		debug("%s", disassemble());
	}

	return elapsedCycles;
}

int 
CPU::execute(int numberOfCycles, int deadCycles)
{	
	assert(deadCycles <= numberOfCycles);
	
	uint64_t startCycles  = cycles;
	uint64_t targetCycles = cycles + numberOfCycles;
	uint64_t currentCycles;
	
	cycles += deadCycles;
	
	do {
		currentCycles = cycles;
		
		// Check for interrupt request
		if (nmiLine) {
			// The NMI line is cleared. Otherwise, the NMI would be recursively interrupted by itself
			clearNMILine(0xff);
			mem->poke(0x100+(SP--), HI_BYTE(PC));
			mem->poke(0x100+(SP--), LO_BYTE(PC));
			mem->poke(0x100+(SP--), getPWithClearedB());	
			setI(1);
			setPCL(mem->peek(0xFFFA));
			setPCH(mem->peek(0xFFFB));
			cycles += 7;
		}

		else if (irqLine && !getI()) {
			mem->poke(0x100+(SP--), HI_BYTE(PC));
			mem->poke(0x100+(SP--), LO_BYTE(PC));
			mem->poke(0x100+(SP--), getPWithClearedB());	
			setI(1);
			setPCL(mem->peek(0xFFFE));
			setPCH(mem->peek(0xFFFF));
			cycles += 7;
		} 
		
		else {						
			// Execute next command
			(void)step();
		}
		
		// Check breakpoint tag
		if (breakpoint[PC] != NO_BREAKPOINT) {
			// Soft breakpoints get deleted when reached
			breakpoint[PC] &= (255 - SOFT_BREAKPOINT);
			setErrorState(BREAKPOINT_REACHED);
			debug("Breakpoint reached\n");
		}

		// Interrupt execution, if we reach a break- or watchpoint
		if (errorState != OK) {
			return 0;
		}		
	} while (cycles < targetCycles);

	return cycles - startCycles;
} 

void 
CPU::executeOneCycle(int deadCycles)
{
	uint64_t startCycles = cycles;
	int executedCycles; // number of cycles consumed by the executed command

	cycles += deadCycles;
	delay += deadCycles;
	
	if (delay > 0) {
		delay --;
		return;
	}

	// Check for interrupt request
	if (nmiLine) {
		// The NMI line is cleared. Otherwise, the NMI would be recursively interrupted by itself
		clearNMILine(0xff);
		mem->poke(0x100+(SP--), HI_BYTE(PC));
		mem->poke(0x100+(SP--), LO_BYTE(PC));
		mem->poke(0x100+(SP--), getPWithClearedB());	
		setI(1);
		setPCL(mem->peek(0xFFFA));
		setPCH(mem->peek(0xFFFB));
		cycles += 7;
	} else if (irqLine && !getI()) {
		mem->poke(0x100+(SP--), HI_BYTE(PC));
		mem->poke(0x100+(SP--), LO_BYTE(PC));
		mem->poke(0x100+(SP--), getPWithClearedB());	
		setI(1);
		setPCL(mem->peek(0xFFFE));
		setPCH(mem->peek(0xFFFF));
		cycles += 7;
	} else {						
		(void)step();
	}
		
	// Check breakpoint tag
	if (breakpoint[PC] != NO_BREAKPOINT) {
		// Soft breakpoints get deleted when reached
		breakpoint[PC] &= (255 - SOFT_BREAKPOINT);
		setErrorState(BREAKPOINT_REACHED);
		debug("Breakpoint reached\n");
	}
			
	executedCycles = (int)(cycles - startCycles);
	if (executedCycles < 1 || executedCycles > 20) 
		debug("WARNING: Something is wrong with the cycle count %d %04X!!!\n", executedCycles, getPC());
	delay = executedCycles - 1;	
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


