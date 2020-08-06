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
    cpu.checkForBreakpoints = value;
}

void
Watchpoints::setNeedsCheck(bool value)
{
}

//
// CPUDebugger
//

CPUDebugger::CPUDebugger(C64 &ref) : C64Component(ref)
{
    setDescription("CPU Debugger");
}

void
CPUDebugger::_reset()
{

}

void
CPUDebugger::stepInto()
{
    softStop = UINT64_MAX;
    breakpoints.setNeedsCheck(true);
}

void
CPUDebugger::stepOver()
{
    softStop = cpu.getAddressOfNextInstruction();
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

void
CPUDebugger::enableLogging()
{
    cpu.logInstructions = true;
}

void
CPUDebugger::disableLogging()
{
    cpu.logInstructions = false;
}

int
CPUDebugger::loggedInstructions()
{
    return logCnt < logBufferCapacity ? (int)logCnt : logBufferCapacity;
}

void
CPUDebugger::logInstruction()
{
    u16 pc = cpu.getPC();
    u8 opcode = mem.spypeek(pc);
    unsigned length = cpu.getLengthOfInstruction(opcode);

    int i = logCnt++ % logBufferCapacity;
    
    logBuffer[i].cycle = cpu.cycle;
    logBuffer[i].pc = pc;
    logBuffer[i].sp = cpu.regSP;
    logBuffer[i].byte1 = opcode;
    logBuffer[i].byte2 = length > 1 ? mem.spypeek(pc + 1) : 0;
    logBuffer[i].byte3 = length > 2 ? mem.spypeek(pc + 2) : 0;
    logBuffer[i].a = cpu.regA;
    logBuffer[i].x = cpu.regX;
    logBuffer[i].y = cpu.regY;
    logBuffer[i].flags = cpu.getP();
}

RecordedInstruction
CPUDebugger::logEntry(int n)
{
    assert(n < loggedInstructions());

    // n == 0 returns the most recently recorded entry
    int offset = (logCnt - 1 - n) % logBufferCapacity;

    return logBuffer[offset];
}

RecordedInstruction
CPUDebugger::logEntryAbs(int n)
{
    assert(n < loggedInstructions());

    // n == 0 returns the oldest entry
    return logEntry(loggedInstructions() - n - 1);
}
