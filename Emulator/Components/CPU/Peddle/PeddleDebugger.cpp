// -----------------------------------------------------------------------------
// This file is part of Peddle - A MOS 65xx CPU emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Published under the terms of the MIT License
// -----------------------------------------------------------------------------

#include "PeddleConfig.h"
#include "Peddle.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cassert>

namespace vc64::peddle {

//
// Guard
//

bool
Guard::eval(u32 addr)
{
    if (this->addr == addr && this->enabled) {
        if (++hits > ignore) {
            return true;
        }
    }
    return false;
}

void 
Guard::moveTo(u32 newAddr)
{
    addr = newAddr;
    hits = 0;
}


//
// Guards
//

Guards::~Guards()
{
    assert(guards);
    delete [] guards;
}

Guard *
Guards::guardWithNr(long nr) const
{
    return nr < count ? &guards[nr] : nullptr;
}

Guard *
Guards::guardAtAddr(u32 addr) const
{
    for (int i = 0; i < count; i++) {
        if (guards[i].addr == addr) return &guards[i];
    }

    return nullptr;
}

void
Guards::setAt(u32 addr, long skip)
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
    guards[count].ignore = skip;
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
Guards::moveTo(long nr, u32 newAddr)
{
    if (nr >= count || isSetAt(newAddr)) return;
    guards[nr].moveTo(newAddr);
}

bool
Guards::isEnabled(long nr) const
{
    Guard *guard = guardWithNr(nr);
    return guard != nullptr && guard->enabled;
}

bool
Guards::isEnabledAt(u32 addr) const
{
    Guard *guard = guardAtAddr(addr);
    return guard != nullptr && guard->enabled;
}

bool
Guards::isDisabled(long nr) const
{
    Guard *guard = guardWithNr(nr);
    return guard != nullptr && !guard->enabled;
}

bool
Guards::isDisabledAt(u32 addr) const
{
    Guard *guard = guardAtAddr(addr);
    return guard != nullptr && !guard->enabled;
}

void
Guards::setEnable(long nr, bool val)
{
    if (nr < count) guards[nr].enabled = val;
}

void
Guards::setEnableAt(u32 addr, bool val)
{
    Guard *guard = guardAtAddr(addr);
    if (guard) guard->enabled = val;
}

void 
Guards::setEnableAll(bool val)
{
    Guard *guard = guardWithNr(0);
    for (isize i = 0; guard != nullptr; guard = guardWithNr(++i)) {
        guard->enabled = val;
    }
}

void
Guards::ignore(long nr, long count)
{
    Guard *guard = guardWithNr(nr);
    if (guard) guard->ignore = count;
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
        cpu.flags |= CPU_CHECK_BP;
    } else {
        cpu.flags &= ~CPU_CHECK_BP;
    }
}

void
Watchpoints::setNeedsCheck(bool value)
{
    if (value) {
        cpu.flags |= CPU_CHECK_WP;
    } else {
        cpu.flags &= ~CPU_CHECK_WP;
    }
}

//
// Debugger
//

void
Debugger::reset()
{
    breakpoints.setNeedsCheck(breakpoints.elements() != 0);
    watchpoints.setNeedsCheck(watchpoints.elements() != 0);
    clearLog();
}

void
Debugger::setSoftStop(u64 addr)
{
    softStop = addr;
    breakpoints.setNeedsCheck(true);
}

void
Debugger::setSoftStopAtNextInstr()
{
    setSoftStop(cpu.getAddressOfNextInstruction());
}

bool
Debugger::breakpointMatches(u32 addr)
{
    // Check if a soft breakpoint has been reached
    if (addr == softStop || softStop == UINT64_MAX) {

        // Soft breakpoints are deleted when reached
        softStop = UINT64_MAX - 1;
        breakpoints.setNeedsCheck(breakpoints.elements() != 0);

        return true;
    }

    if (!breakpoints.eval(addr)) return false;

    breakpointPC = cpu.reg.pc;
    return true;
}

bool
Debugger::watchpointMatches(u32 addr)
{
    if (!watchpoints.eval(addr)) return false;
    
    watchpointPC = cpu.reg.pc0;
    return true;
}

void
Debugger::dumpLogBuffer(std::ostream& os, isize count)
{
    isize num = loggedInstructions();

    char pc[16];
    char instr[16];
    char flags[16];

    for (isize i = num - count; i < num ; i++) {

        if (i >= 0) {

            cpu.debugger.disassembleRecordedPC(i, pc);
            cpu.debugger.disassembleRecordedInstr(i, instr);
            cpu.debugger.disassembleRecordedFlags(i, flags);

            os << std::setfill('0');
            os << "   ";
            os << std::right << std::setw(4) << pc;
            os << "   ";
            os << flags;
            os << "    ";
            os << instr;
            os << std::endl;
        }
    }
}

void
Debugger::dumpLogBuffer(std::ostream& os)
{
    dumpLogBuffer(os, loggedInstructions());
}

void
Debugger::enableLogging()
{
    cpu.flags |= CPU_LOG_INSTRUCTION;
}

void
Debugger::disableLogging()
{
    cpu.flags &= ~CPU_LOG_INSTRUCTION;
}

isize
Debugger::loggedInstructions() const
{
    return logCnt < LOG_BUFFER_CAPACITY ? logCnt : LOG_BUFFER_CAPACITY;
}

void
Debugger::logInstruction()
{
    u16 pc = cpu.getPC0();
    u8 opcode = cpu.readDasm(pc);
    isize length = cpu.getLengthOfInstruction(opcode);

    isize i = logCnt++ % LOG_BUFFER_CAPACITY;
    
    logBuffer[i].cycle = cpu.clock;
    logBuffer[i].pc = pc;
    logBuffer[i].sp = cpu.reg.sp;
    logBuffer[i].byte1 = opcode;
    logBuffer[i].byte2 = length > 1 ? cpu.readDasm(pc + 1) : 0;
    logBuffer[i].byte3 = length > 2 ? cpu.readDasm(pc + 2) : 0;
    logBuffer[i].a = cpu.reg.a;
    logBuffer[i].x = cpu.reg.x;
    logBuffer[i].y = cpu.reg.y;
    logBuffer[i].flags = cpu.getP();
}

const RecordedInstruction &
Debugger::logEntryRel(isize n) const
{
    assert(n < loggedInstructions());
    return logBuffer[(logCnt - 1 - n) % LOG_BUFFER_CAPACITY];
}

const RecordedInstruction &
Debugger::logEntryAbs(isize n) const
{
    assert(n < loggedInstructions());
    return logEntryRel(loggedInstructions() - n - 1);
}

u16
Debugger::loggedPC0Rel(isize n) const
{
    assert(n < loggedInstructions());
    return logBuffer[(logCnt - 1 - n) % LOG_BUFFER_CAPACITY].pc;
}

u16
Debugger::loggedPC0Abs(isize n) const
{
    assert(n < loggedInstructions());
    return loggedPC0Rel(loggedInstructions() - n - 1);
}

isize
Debugger::disassRecorded(char *dst, const char *fmt, isize nr) const
{
    RecordedInstruction instr = logEntryAbs(nr);
    
    return cpu.disassembler.disass(dst, fmt, instr);
}

isize
Debugger::disassembleRecordedInstr(isize i, char *str) const
{
    RecordedInstruction instr = logEntryAbs(i);

    return cpu.disassembler.disassemble(str,
                                        instr.pc,
                                        instr.byte1,
                                        instr.byte2,
                                        instr.byte3);
}

isize
Debugger::disassembleRecordedBytes(isize i, char *str) const
{
    RecordedInstruction instr = logEntryAbs(i);

    u8 bytes[] = {instr.byte1, instr.byte2, instr.byte3 };
    isize len = cpu.getLengthOfInstruction(instr.byte1);

    cpu.disassembler.dumpBytes(str, bytes, len);
    return len;
}

void
Debugger::disassembleRecordedFlags(isize i, char *str) const
{
    cpu.disassembler.disassembleFlags(str, logEntryAbs(i).flags);
}

void
Debugger::disassembleRecordedPC(isize i, char *str) const
{
    cpu.disassembler.dumpWord(str, logEntryAbs(i).pc);
}

void
Debugger::jump(u16 addr)
{
    cpu.reg.pc = addr;
    cpu.reg.pc0 = addr;
    cpu.next = fetch;

    cpu.jumpedTo(addr);
}

}
