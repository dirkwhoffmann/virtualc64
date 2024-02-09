// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// This FILE is dual-licensed. You are free to choose between:
//
//     - The GNU General Public License v3 (or any later version)
//     - The Mozilla Public License v2
//
// SPDX-License-Identifier: GPL-3.0-or-later OR MPL-2.0
// -----------------------------------------------------------------------------

/* 
 * This files declares the emulator's public API.
 */

//
// Emulator
//

// Launches the emulator thread
void launch(const void *listener, Callback *func);

// Configures the emulator to match a specific C64 model
void configure(C64Model model);

// Sets a single configuration option
void configure(Option option, i64 value) throws;
void configure(Option option, long id, i64 value) throws;

// Queries a single configuration option
i64 getConfigItem(Option option) const;
i64 getConfigItem(Option option, long id) const;
void setConfigItem(Option option, i64 value);

// Returns the emulated refresh rate of the virtual C64
double refreshRate() const override;


//
// C64
//

struct C64_API : API {

    using API::API;

    // Performs a hard or soft reset
    void hardReset();
    void softReset();

} c64;


//
// CPU
//

struct CPU_API : API {

    using API::API;

    CPUInfo getInfo() const;
    i64 clock() const;
    u16 getPC0() const;
    isize loggedInstructions() const;
    u16 loggedPC0Rel(isize nr) const;
    u16 loggedPC0Abs(isize nr) const;
    RecordedInstruction logEntryAbs(isize index) const;
    void clearLog();
    void setNumberFormat(DasmNumberFormat instrFormat, DasmNumberFormat dataFormat);
    isize disassembleRecordedInstr(isize, char *) const;
    isize disassembleRecordedBytes(isize, char *) const;
    void disassembleRecordedFlags(isize, char *) const;
    void disassembleRecordedPC(isize, char *) const;
    isize disassemble(char *, u16 addr) const;
    isize getLengthOfInstructionAt(u16 addr) const;
    void dumpBytes(char *, u16 addr, isize length) const;
    void dumpWord(char *, u16 addr) const;

} cpu;

//
// CIAs
//

struct CIA_API : API {

    CIA &cia;
    CIA_API(Emulator &emu, CIA& cia) : API(emu), cia(cia) { }

    CIAInfo getInfo() const;

} cia1, cia2;


//
// VICII
//

struct VICII_API : API {

    using API::API;

    VICIIConfig getConfig() const;
    VICIIInfo getInfo() const;
    SpriteInfo getSpriteInfo(isize nr) const;

    isize getCyclesPerLine() const;
    isize getLinesPerFrame() const;
    bool pal() const;

    u32 *stableEmuTexture() const;
    u32 *getNoise() const;
    u32 getColor(isize nr) const;
    u32 getColor(isize nr, Palette palette) const;

} vicii;


//
// Memory
//

struct MEM_API : API {

    using API::API;

    // Returns the current configuration and state
    MemConfig getConfig() const;
    MemInfo getInfo() const;

    // Returns a string representations for a portion of memory
    string memdump(u16 addr, isize num, bool hex, isize pads, MemoryType src) const;
    string txtdump(u16 addr, isize num, MemoryType src) const;

} mem;


//
// DMA Debugger
//

struct DMA_DEBUGGER_API : API {

    using API::API;

    // Returns the current configuration
    DmaDebuggerConfig getConfig() const;

} dmaDebugger;
