// -----------------------------------------------------------------------------
// This file is part of Peddle - A MOS 65xx CPU emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Published under the terms of the MIT License
// -----------------------------------------------------------------------------

#pragma once

#include "PeddleDebugger.h"
#include "Peddle.h"

namespace vc64::peddle {

/*
// Base structure for a single breakpoint or watchpoint
struct Guard {
    
    // The observed address
    u32 addr;
    
    // Disabled guards never trigger
    bool enabled;
    
    // Counts the number of hits
    long hits;
    
    // Ignore counter
    long ignore;
    
public:
    
    // Returns true if the guard hits
    bool eval(u32 addr);

    // Replaces the address by another
    void moveTo(u32 newAddr);
};
*/

// Base class for a collection of guards
class Guards {

    friend class Debugger;
    
protected:

    // Reference to the connected CPU
    class Peddle &cpu;

    // Capacity of the guards array
    long capacity = 1;

    // Array holding all guards
    Guard *guards = new Guard[1];

    // Number of currently stored guards
    long count = 0;

    // Indicates if guard checking is necessary
    virtual void setNeedsCheck(bool value) = 0;
    
    
    //
    // Constructing
    //

public:

    Guards(Peddle& ref) : cpu(ref) { }
    virtual ~Guards();


    //
    // Inspecting the guard list
    //
    
    long elements() const { return count; }
    Guard *guardNr(long nr) const;
    Guard *guardAt(u32 addr) const;
    
    u32 guardAddr(long nr) const { return nr < count ? guards[nr].addr : 0; }
    

    //
    // Adding or removing guards
    //

    bool isSet(long nr) const { return guardNr(nr) != nullptr; }
    bool isSetAt(u32 addr) const { return guardAt(addr) != nullptr; }

    void setAt(u32 addr, long ignores = 0);
    void moveTo(long nr, u32 newAddr);

    void remove(long nr);
    void removeAt(u32 addr);
    void removeAll() { count = 0; setNeedsCheck(false); }


    //
    // Enabling or disabling guards
    //
    
    bool isEnabled(long nr) const;
    bool isEnabledAt(u32 addr) const;
    bool isDisabled(long nr) const;
    bool isDisabledAt(u32 addr) const;

    void enable(long nr) { setEnable(nr, true); }
    void enableAt(u32 addr) { setEnableAt(addr, true); }
    void enableAll() { setEnableAll(true); }

    void disable(long nr) { setEnable(nr, false); }
    void disableAt(u32 addr) { setEnableAt(addr, false); }
    void disableAll() { setEnableAll(false); }

    void setEnable(long nr, bool val);
    void setEnableAt(u32 addr, bool val);
    void setEnableAll(bool val);

    void ignore(long nr, long count);


    //
    // Checking a guard
    //
    
private:
    
    // Returns true if the guard hits
    bool eval(u32 addr);
};

class Breakpoints : public Guards {
    
public:
    
    Breakpoints(Peddle& ref) : Guards(ref) { }
    void setNeedsCheck(bool value) override;
};

class Watchpoints : public Guards {
    
public:
    
    Watchpoints(Peddle& ref) : Guards(ref) { }
    void setNeedsCheck(bool value) override;
};

class Debugger {
    
    friend class Peddle;
    
    // Reference to the connected CPU
    class Peddle &cpu;
    
public:
    
    // Log buffer
    RecordedInstruction logBuffer[LOG_BUFFER_CAPACITY];

    // Breakpoint storage
    Breakpoints breakpoints = Breakpoints(cpu);

    // Watchpoint storage (not yet supported)
    Watchpoints watchpoints = Watchpoints(cpu);
    
    // Saved program counters
    i32 breakpointPC = -1;
    i32 watchpointPC = -1;

private:
    
    /* Number of logged instructions.
     * Note: This variable counts the total number of logged instructions and
     * eventually exceeds the log buffer capacity. Use 'loggedInstruction()'
     * to obtain the number of available instructions in the log buffer.
     */
    isize logCnt = 0;

    /* Soft breakpoint for implementing single-stepping.
     * In contrast to a standard (hard) breakpoint, a soft breakpoint is
     * deleted when reached. The CPU halts if softStop matches the CPU's
     * program counter (used to implement "step over") or if softStop equals
     * UINT64_MAX (used to implement "step into"). To disable soft stopping,
     * simply set softStop to an unreachable memory location such as
     * UINT64_MAX - 1.
     */
    u64 softStop = UINT64_MAX - 1;

    
    //
    // Initializing
    //
    
public:
    
    Debugger(Peddle& ref) : cpu(ref) { };
    void reset();

    
    //
    // Working with breakpoints and watchpoints
    //

public:

    // Sets a soft breakpoint
    void setSoftStop(u64 addr);
    void setSoftStopAtNextInstr();
    
    // Returns true if a breakpoint hits at the provides address
    bool breakpointMatches(u32 addr);

    // Returns true if a watchpoint hits at the provides address
    bool watchpointMatches(u32 addr);
    
    
    //
    // Working with the instruction log
    //

    // Dumps a portion of the log buffer
    void dumpLogBuffer(std::ostream& os, isize count);
    void dumpLogBuffer(std::ostream& os);

    // Turns instruction logging on or off
    void enableLogging();
    void disableLogging();

    // Returns the number of logged instructions
    isize loggedInstructions() const;
    
    // Logs an instruction
    void logInstruction();
    
    /* Reads an item from the log buffer
     *
     *    xxxRel: n == 0 returns the most recently recorded entry
     *    xxxAbs: n == 0 returns the oldest entry
     */
    const RecordedInstruction &logEntryRel(isize n) const;
    const RecordedInstruction &logEntryAbs(isize n) const;
    u16 loggedPC0Rel(isize n) const;
    u16 loggedPC0Abs(isize n) const;

    // Disassembles an item from the log buffer
    isize disassRecorded(char *dst, const char *fmt, isize addr) const;

    // DEPRECATED
    isize disassembleRecordedInstr(isize i, char *str) const;
    isize disassembleRecordedBytes(isize i, char *str) const;
    void disassembleRecordedFlags(isize i, char *str) const;
    void disassembleRecordedPC(isize i, char *str) const;

    // Clears the log buffer
    void clearLog() { logCnt = 0; }


    //
    // Changing state
    //

    // Continues program execution at the specified address
    void jump(u16 addr);
};

}
