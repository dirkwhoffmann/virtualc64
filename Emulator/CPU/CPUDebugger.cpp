// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "C64.h"

//
// Guard
//

bool
Guard::eval(u32 addr)
{
    if (this->addr == addr && this->enabled) {
        if (++hits > skip) {
            return true;
        }
    }
    return false;
}

//
// Guards
//

Guard *
Guards::guardWithNr(long nr)
{
    return nr < count ? &guards[nr] : NULL;
}

Guard *
Guards::guardAtAddr(u32 addr)
{
    for (int i = 0; i < count; i++) {
        if (guards[i].addr == addr) return &guards[i];
    }

    return NULL;
}

bool
Guards::isSetAt(u32 addr)
{
    Guard *guard = guardAtAddr(addr);

    return guard != NULL;
}

bool
Guards::isSetAndEnabledAt(u32 addr)
{
    Guard *guard = guardAtAddr(addr);

    return guard != NULL && guard->enabled;
}

bool
Guards::isSetAndDisabledAt(u32 addr)
{
    Guard *guard = guardAtAddr(addr);

    return guard != NULL && !guard->enabled;
}

bool
Guards::isSetAndConditionalAt(u32 addr)
{
    Guard *guard = guardAtAddr(addr);

    return guard != NULL && guard->skip != 0;
}

void
Guards::addAt(u32 addr, long skip)
{
    if (isSetAt(addr)) return;

    if (count >= capacity) {

        Guard *newguards = new Guard[2 * capacity];
        for (long i = 0; i < capacity; i++) newguards[i] = guards[i];
        delete [] guards;
        guards = newguards;
        capacity *= 2;
    }

    guards[count].addr = addr;
    guards[count].enabled = true;
    guards[count].hits = 0;
    guards[count].skip = skip;
    count++;
    setNeedsCheck(true);
}

void
Guards::remove(long nr)
{
    if (nr < count) removeAt(guards[nr].addr);
}

void
Guards::removeAt(u32 addr)
{
    for (int i = 0; i < count; i++) {

        if (guards[i].addr == addr) {

            for (int j = i; j + 1 < count; j++) guards[j] = guards[j + 1];
            count--;
            break;
        }
    }
    setNeedsCheck(count != 0);
}

void
Guards::replace(long nr, u32 addr)
{
    if (nr >= count || isSetAt(addr)) return;
    
    guards[nr].addr = addr;
    guards[nr].hits = 0;
}

bool
Guards::isEnabled(long nr)
{
    return nr < count ? guards[nr].enabled : false;
}

void
Guards::setEnable(long nr, bool val)
{
    if (nr < count) guards[nr].enabled = val;
}

void
Guards::setEnableAt(u32 addr, bool value)
{
    Guard *guard = guardAtAddr(addr);
    if (guard) guard->enabled = value;
}

bool
Guards::eval(u32 addr)
{
    for (int i = 0; i < count; i++)
        if (guards[i].eval(addr)) return true;

    return false;
}

void
Breakpoints::setNeedsCheck(bool value)
{
    if (value) {
        cpu.flags |= CPU::CPU_CHECK_BP;
    } else {
        cpu.flags &= ~CPU::CPU_CHECK_BP;
    }
}

void
Watchpoints::setNeedsCheck(bool value)
{
    if (value) {
         cpu.flags |= CPU::CPU_CHECK_WP;
     } else {
         cpu.flags &= ~CPU::CPU_CHECK_WP;
     }
}

//
// CPUDebugger
//

CPUDebugger::CPUDebugger(C64 &ref) : C64Component(ref)
{
    setDescription("CPU Debugger");
}

void
CPUDebugger::registerInstruction(u8 opcode, const char *mnemonic, AddressingMode mode)
{
    this->mnemonic[opcode] = mnemonic;
    this->addressingMode[opcode] = mode;
}

void
CPUDebugger::_powerOn()
{
#ifdef INITIAL_BREAKPOINT
    breakpoints.addAt(INITIAL_BREAKPOINT);
#endif
}

void
CPUDebugger::_reset()
{
    breakpoints.setNeedsCheck(breakpoints.elements() != 0);
    watchpoints.setNeedsCheck(watchpoints.elements() != 0);
    clearLog();
}

void
CPUDebugger::setSoftStop(u64 addr)
{
    softStop = addr;
    breakpoints.setNeedsCheck(true);
}

bool
CPUDebugger::breakpointMatches(u32 addr)
{
    // Check if a soft breakpoint has been reached
    if (addr == softStop || softStop == UINT64_MAX) {

        // Soft breakpoints are deleted when reached
        softStop = UINT64_MAX - 1;
        breakpoints.setNeedsCheck(breakpoints.elements() != 0);

        return true;
    }

    return breakpoints.eval(addr);
}

bool
CPUDebugger::watchpointMatches(u32 addr)
{
    return watchpoints.eval(addr);
}

int
CPUDebugger::loggedInstructions()
{
    return logCnt < LOG_BUFFER_CAPACITY ? (int)logCnt : LOG_BUFFER_CAPACITY;
}

void
CPUDebugger::logInstruction()
{
    u16 pc = cpu.getPC();
    u8 opcode = cpu.spypeek(pc);
    unsigned length = getLengthOfInstruction(opcode);

    int i = logCnt++ % LOG_BUFFER_CAPACITY;
    
    logBuffer[i].cycle = cpu.cycle;
    logBuffer[i].pc = pc;
    logBuffer[i].sp = cpu.reg.sp;
    logBuffer[i].byte1 = opcode;
    logBuffer[i].byte2 = length > 1 ? cpu.spypeek(pc + 1) : 0;
    logBuffer[i].byte3 = length > 2 ? cpu.spypeek(pc + 2) : 0;
    logBuffer[i].a = cpu.reg.a;
    logBuffer[i].x = cpu.reg.x;
    logBuffer[i].y = cpu.reg.y;
    logBuffer[i].flags = cpu.getP();
}

RecordedInstruction
CPUDebugger::logEntry(int n)
{
    assert(n < loggedInstructions());

    // n == 0 returns the most recently recorded entry
    int offset = (logCnt - 1 - n) % LOG_BUFFER_CAPACITY;

    return logBuffer[offset];
}

RecordedInstruction
CPUDebugger::logEntryAbs(int n)
{
    assert(n < loggedInstructions());

    // n == 0 returns the oldest entry
    return logEntry(loggedInstructions() - n - 1);
}

unsigned
CPUDebugger::getLengthOfInstruction(u8 opcode)
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

unsigned
CPUDebugger::getLengthOfInstructionAtAddress(u16 addr)
{
    return getLengthOfInstruction(cpu.spypeek(addr));
}

unsigned
CPUDebugger::getLengthOfCurrentInstruction()
{
    return getLengthOfInstructionAtAddress(cpu.getPC());
}

u16
CPUDebugger::getAddressOfNextInstruction()
{
    return cpu.getPC() + getLengthOfCurrentInstruction();
}

const char *
CPUDebugger::disassemble(u16 addr)
{
    RecordedInstruction instr;
    
    instr.pc = addr;
    instr.byte1 = cpu.spypeek(addr);
    instr.byte2 = cpu.spypeek(addr + 1);
    instr.byte3 = cpu.spypeek(addr + 2);
    instr.a = cpu.reg.a;
    instr.x = cpu.reg.x;
    instr.y = cpu.reg.y;
    instr.sp = cpu.reg.sp;
    instr.flags = cpu.getP();
    
    return disassemble(instr);
}

const char *
CPUDebugger::disassemble(RecordedInstruction &instr)
{
    static char result[16];
        
    u8 opcode = instr.byte1;
    u8 length = getLengthOfInstruction(opcode);
        
    // Convert command
    char operand[6];
    switch (addressingMode[opcode]) {
            
        case ADDR_IMMEDIATE:
        case ADDR_ZERO_PAGE:
        case ADDR_ZERO_PAGE_X:
        case ADDR_ZERO_PAGE_Y:
        case ADDR_INDIRECT_X:
        case ADDR_INDIRECT_Y: {
            u8 value = cpu.spypeek(instr.pc + 1);
            hex ? sprint8x(operand, value) : sprint8d(operand, value);
            break;
        }
        case ADDR_DIRECT:
        case ADDR_INDIRECT:
        case ADDR_ABSOLUTE:
        case ADDR_ABSOLUTE_X:
        case ADDR_ABSOLUTE_Y: {
            u16 value = LO_HI(cpu.spypeek(instr.pc + 1),cpu.spypeek(instr.pc + 2));
            hex ? sprint16x(operand, value) : sprint16d(operand, value);
            break;
        }
        case ADDR_RELATIVE: {
            u16 value = instr.pc + 2 + (i8)cpu.spypeek(instr.pc + 1);
            hex ? sprint16x(operand, value) : sprint16d(operand, value);
            break;
        }
        default:
            break;
    }
    
    switch (addressingMode[opcode]) {
            
        case ADDR_IMPLIED:
        case ADDR_ACCUMULATOR:
            strcpy(result, "xxx");
            break;
        case ADDR_IMMEDIATE:
            strcpy(result, hex ? "xxx #hh" : "xxx #ddd");
            memcpy(&result[5], operand, hex ? 2 : 3);
            break;
        case ADDR_ZERO_PAGE:
            strcpy(result, hex ? "xxx hh" : "xxx ddd");
            memcpy(&result[4], operand, hex ? 2 : 3);
            break;
        case ADDR_ZERO_PAGE_X:
            strcpy(result, hex ? "xxx hh,X" : "xxx ddd,X");
            memcpy(&result[4], operand, hex ? 2 : 3);
            break;
        case ADDR_ZERO_PAGE_Y:
            strcpy(result, hex ? "xxx hh,Y" : "xxx ddd,Y");
            memcpy(&result[4], operand, hex ? 2 : 3);
            break;
        case ADDR_ABSOLUTE:
        case ADDR_DIRECT:
            strcpy(result, hex ? "xxx hhhh" : "xxx ddddd");
            memcpy(&result[4], operand, hex ? 4 : 5);
            break;
        case ADDR_ABSOLUTE_X:
            strcpy(result, hex ? "xxx hhhh,X" : "xxx ddddd,X");
            memcpy(&result[4], operand, hex ? 4 : 5);
            break;
        case ADDR_ABSOLUTE_Y:
            strcpy(result, hex ? "xxx hhhh,Y" : "xxx ddddd,Y");
            memcpy(&result[4], operand, hex ? 4 : 5);
            break;
        case ADDR_INDIRECT:
            strcpy(result, hex ? "xxx (hhhh)" : "xxx (ddddd)");
            memcpy(&result[5], operand, hex ? 4 : 5);
            break;
        case ADDR_INDIRECT_X:
            strcpy(result, hex ? "xxx (hh,X)" : "xxx (ddd,X)");
            memcpy(&result[5], operand, hex ? 2 : 3);
            break;
        case ADDR_INDIRECT_Y:
            strcpy(result, hex ? "xxx (hh),Y" : "xxx (ddd),Y");
            memcpy(&result[5], operand, hex ? 2 : 3);
            break;
        case ADDR_RELATIVE:
            strcpy(result, hex ? "xxx hhhh" : "xxx ddddd");
            memcpy(&result[4], operand, hex ? 4 : 5);
            break;
        default:
            strcpy(result, "???");
    }
    
    // Copy mnemonic
    strncpy(result, mnemonic[opcode], 3);
    
    // Convert register contents to strings
    /*
    hex ? sprint16x(result.pc, instr.pc) : sprint16d(result.pc, instr.pc);
    hex ? sprint8x(result.a, instr.a) : sprint8d(result.a, instr.a);
    hex ? sprint8x(result.x, instr.x) : sprint8d(result.x, instr.x);
    hex ? sprint8x(result.y, instr.y) : sprint8d(result.y, instr.y);
    hex ? sprint8x(result.sp, instr.sp) : sprint8d(result.sp, instr.sp);
    */
    
    // Convert memory contents to strings
    /*
    char *ptr = result.data;
    if (hex) {
        if (length >= 1) { sprint8x(ptr, instr.byte1); ptr[2] = ' '; ptr += 3; }
        if (length >= 2) { sprint8x(ptr, instr.byte2); ptr[2] = ' '; ptr += 3; }
        if (length >= 3) { sprint8x(ptr, instr.byte3); ptr[2] = ' '; ptr += 3; }
    } else {
        if (length >= 1) { sprint8d(ptr, instr.byte1); ptr[3] = ' '; ptr += 4; }
        if (length >= 2) { sprint8d(ptr, instr.byte2); ptr[3] = ' '; ptr += 4; }
        if (length >= 3) { sprint8d(ptr, instr.byte3); ptr[3] = ' '; ptr += 4; }
    }
    ptr[0] = 0;
    */
    
    // Convert flags to a string
    /*
    result.flags[0] = (instr.flags & N_FLAG) ? 'N' : 'n';
    result.flags[1] = (instr.flags & V_FLAG) ? 'V' : 'v';
    result.flags[2] = '-';
    result.flags[3] = (instr.flags & B_FLAG) ? 'B' : 'b';
    result.flags[4] = (instr.flags & D_FLAG) ? 'D' : 'd';
    result.flags[5] = (instr.flags & I_FLAG) ? 'I' : 'i';
    result.flags[6] = (instr.flags & Z_FLAG) ? 'Z' : 'z';
    result.flags[7] = (instr.flags & C_FLAG) ? 'C' : 'c';
    result.flags[8] = 0;
    */
    
    return result;
}

const char *
CPUDebugger::disassembleLogEntry(int i)
{
    RecordedInstruction instr = logEntryAbs(i);
    return disassemble(instr);
}

const char *
CPUDebugger::disassembleDataBytes(RecordedInstruction &instr)
{
    return "LDA something";
}

const char *
CPUDebugger::disassembleDataBytes(u16 addr)
{
    return "XXX XXX XXX";
}

const char *
CPUDebugger::disassembleFlags(RecordedInstruction &instr)
{
    return "--- Flags ---";
}

/*
DisassembledInstruction
CPUDebugger::disassemble(RecordedInstruction instr)
{
    DisassembledInstruction result;
    
    u8 opcode = instr.byte1;
    u8 length = getLengthOfInstruction(opcode);
    
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
            u8 value = cpu.spypeek(instr.pc + 1);
            hex ? sprint8x(operand, value) : sprint8d(operand, value);
            break;
        }
        case ADDR_DIRECT:
        case ADDR_INDIRECT:
        case ADDR_ABSOLUTE:
        case ADDR_ABSOLUTE_X:
        case ADDR_ABSOLUTE_Y: {
            u16 value = LO_HI(cpu.spypeek(instr.pc + 1),cpu.spypeek(instr.pc + 2));
            hex ? sprint16x(operand, value) : sprint16d(operand, value);
            break;
        }
        case ADDR_RELATIVE: {
            u16 value = instr.pc + 2 + (i8)cpu.spypeek(instr.pc + 1);
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
    strncpy(result.command, mnemonic[opcode], 3);
    
    // Convert register contents to strings
    hex ? sprint16x(result.pc, instr.pc) : sprint16d(result.pc, instr.pc);
    hex ? sprint8x(result.a, instr.a) : sprint8d(result.a, instr.a);
    hex ? sprint8x(result.x, instr.x) : sprint8d(result.x, instr.x);
    hex ? sprint8x(result.y, instr.y) : sprint8d(result.y, instr.y);
    hex ? sprint8x(result.sp, instr.sp) : sprint8d(result.sp, instr.sp);
    
    // Convert memory contents to strings
    char *ptr = result.data;
    if (hex) {
        if (length >= 1) { sprint8x(ptr, instr.byte1); ptr[2] = ' '; ptr += 3; }
        if (length >= 2) { sprint8x(ptr, instr.byte2); ptr[2] = ' '; ptr += 3; }
        if (length >= 3) { sprint8x(ptr, instr.byte3); ptr[2] = ' '; ptr += 3; }
    } else {
        if (length >= 1) { sprint8d(ptr, instr.byte1); ptr[3] = ' '; ptr += 4; }
        if (length >= 2) { sprint8d(ptr, instr.byte2); ptr[3] = ' '; ptr += 4; }
        if (length >= 3) { sprint8d(ptr, instr.byte3); ptr[3] = ' '; ptr += 4; }
    }
    ptr[0] = 0;

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
CPUDebugger::disassemble(u16 addr)
{
    RecordedInstruction instr;

    instr.pc = addr;
    instr.byte1 = cpu.spypeek(addr);
    instr.byte2 = cpu.spypeek(addr + 1);
    instr.byte3 = cpu.spypeek(addr + 2);
    instr.a = cpu.reg.a;
    instr.x = cpu.reg.x;
    instr.y = cpu.reg.y;
    instr.sp = cpu.reg.sp;
    instr.flags = cpu.getP();
    
    return disassemble(instr);
}

DisassembledInstruction
CPUDebugger::disassemble()
{
    return disassemble(cpu.pc);
}
*/
