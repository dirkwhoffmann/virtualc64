// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _CPU_DEBUGGER_H
#define _CPU_DEBUGGER_H

#include "C64Component.h"

// Base structure for a single breakpoint or watchpoint
struct Guard {
    
    // The observed address
    u32 addr;
    
    // Disabled guards never trigger
    bool enabled;
    
    // Counts the number of hits
    long hits;
    
    // Number of skipped hits before a match is signalled
    long skip;
    
public:
    
    // Returns true if the guard hits
    bool eval(u32 addr);
};

// Base class for a collection of guards
class Guards {

    friend class CPUDebugger;

public:
    
    virtual ~Guards() { };
    
protected:

    // Reference to the connected CPU
    class CPU &cpu;

    // Capacity of the guards array
    long capacity = 1;

    // Array holding all guards
    Guard *guards = new Guard[1];

    // Number of currently stored guards
    long count = 0;

    // Indicates if guard checking is necessary
    virtual void setNeedsCheck(bool value) = 0;
    
    
    //
    // Constructing and destructing
    //
    
public:
    
    Guards(CPU& ref) : cpu(ref) { }
    
    
    //
    // Inspecting the guard list
    //
    
    long elements() { return count; }
    Guard *guardWithNr(long nr);
    Guard *guardAtAddr(u32 addr);
    
    u32 guardAddr(long nr) { return nr < count ? guards[nr].addr : 0; }
    
    bool isSetAt(u32 addr);
    bool isSetAndEnabledAt(u32 addr);
    bool isSetAndDisabledAt(u32 addr);
    bool isSetAndConditionalAt(u32 addr);
    
    //
    // Adding or removing guards
    //
    
    void addAt(u32 addr, long skip = 0);
    void removeAt(u32 addr);
    
    void remove(long nr);
    void removeAll() { count = 0; setNeedsCheck(false); }
    
    void replace(long nr, u32 addr);
    
    //
    // Enabling or disabling guards
    //
    
    bool isEnabled(long nr);
    bool isDisabled(long nr) { return !isEnabled(nr); }
    
    void setEnable(long nr, bool val);
    void enable(long nr) { setEnable(nr, true); }
    void disable(long nr) { setEnable(nr, false); }
    
    void setEnableAt(u32 addr, bool val);
    void enableAt(u32 addr) { setEnableAt(addr, true); }
    void disableAt(u32 addr) { setEnableAt(addr, false); }
    
    //
    // Checking a guard
    //
    
private:
    
    bool eval(u32 addr);
};

class Breakpoints : public Guards {
    
public:
    
    Breakpoints(CPU& ref) : Guards(ref) { }
    void setNeedsCheck(bool value) override;
};

class Watchpoints : public Guards {
    
public:
    
    Watchpoints(CPU& ref) : Guards(ref) { }
    void setNeedsCheck(bool value) override;
};

class CPUDebugger : public C64Component {
    
    friend class CPU;
    
    // Textual representation for each opcode (used by the disassembler)
    const char *mnemonic[256];
     
    // Adressing mode of each opcode (used by the disassembler)
     AddressingMode addressingMode[256];
    
public:
    
    // Log buffer
    RecordedInstruction logBuffer[LOG_BUFFER_CAPACITY];

    // Breakpoint storage
    Breakpoints breakpoints = Breakpoints(cpu);

    // Watchpoint storage (not yet supported)
    Watchpoints watchpoints = Watchpoints(cpu);
    
private:
    
    /* Number of logged instructions.
     * Note: This variable counts the total number of logged instructions and
     * eventually exceeds the log buffer capacity. Use 'loggedInstruction()'
     * to obtain the number of available instructions in the log buffer.
     */
    long logCnt = 0;

    /* Soft breakpoint for implementing single-stepping.
     * In contrast to a standard (hard) breakpoint, a soft breakpoint is
     * deleted when reached. The CPU halts if softStop matches the CPU's
     * program counter (used to implement "step over") or if softStop equals
     * UINT64_MAX (used to implement "step into"). To disable soft stopping,
     * simply set softStop to an unreachable memory location such as
     * UINT64_MAX - 1.
     */
    u64 softStop = UINT64_MAX - 1;
        
public:
    
    // Number format used by the disassembler
    bool hex = true;

    
    //
    // Constructing and serializing
    //
    
public:
    
    CPUDebugger(C64 &ref);

    // Initializes an entry of the lookup tables
    void registerInstruction(u8 opcode, const char *mnemonic, AddressingMode mode);

    
    //
    // Methods from HardwareComponent
    //
    
private:
    
    void _powerOn() override;
    void _reset() override;


    //
    // Working with breakpoints and watchpoints
    //

public:

    // Sets a soft breakpoint
    void setSoftStop(u64 addr);
    void setSoftStopAtNextInstr() { setSoftStop(getAddressOfNextInstruction()); }
    
    // Returns true if a breakpoint hits at the provides address
    bool breakpointMatches(u32 addr);

    // Returns true if a watchpoint hits at the provides address
    bool watchpointMatches(u32 addr);
    
    
    //
    // Working with the instruction log
    //
        
    // Returns the number of logged instructions
    int loggedInstructions();
    
    // Logs an instruction
    void logInstruction();
    
    /* Reads an entry from the log buffer
     *    logEntry: n == 0 returns the most recently recorded entry
     * logEntryAbs: n == 0 returns the oldest entry
     */
    RecordedInstruction logEntry(int n);
    RecordedInstruction logEntryAbs(int n);
    
    // Clears the log buffer
    void clearLog() { logCnt = 0; }
    
    //
    // Examining instructions
    //
    
    // Returns the length of an instruction in bytes
    unsigned getLengthOfInstruction(u8 opcode);
    unsigned getLengthOfInstructionAtAddress(u16 addr);
    unsigned getLengthOfCurrentInstruction();

    // Returns the address of the instruction following the current one
    u16 getAddressOfNextInstruction();
    
    //
    // Running the disassembler
    //
    
    // Disassembles a previously recorded instruction
    DisassembledInstruction disassemble(RecordedInstruction instr);
    
    // Disassembles an instruction at a specified memory location
    DisassembledInstruction disassemble(u16 addr);
    
    // Disassembles the currently executed instruction
    DisassembledInstruction disassemble();
};

#endif
