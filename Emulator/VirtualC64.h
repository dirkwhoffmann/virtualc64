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

#pragma once

#include "Emulator.h"

class VirtualC64 : vc64::Emulator {

    using vc64::Emulator::Thread::Suspendable;


    //
    // Static methods
    //

public:

    // Returns a version string for this release
    static string version();

    // Returns a build number string for this release
    static string build();


    //
    // Initializing
    //

public:
    
    VirtualC64();
    ~VirtualC64();

    // Expose portions of the Emulator API
    using vc64::Emulator::defaults;
    using vc64::Emulator::getState;
    using vc64::Emulator::getStats;

    //
    // Emulator state
    //

    using vc64::Emulator::Thread::isPoweredOn;
    using vc64::Emulator::Thread::isPoweredOff;
    using vc64::Emulator::Thread::isPaused;
    using vc64::Emulator::Thread::isRunning;
    using vc64::Emulator::Thread::isSuspended;
    using vc64::Emulator::Thread::isHalted;
    using vc64::Emulator::Thread::isWarping;
    using vc64::Emulator::Thread::isTracking;

    using vc64::Emulator::Thread::warpOn;
    using vc64::Emulator::Thread::warpOff;
    using vc64::Emulator::Thread::trackOn;
    using vc64::Emulator::Thread::trackOff;


    //
    // Synchronizing
    //

    using vc64::Emulator::Thread::wakeUp;


    //
    // Audio and Video
    //

    u32 *getTexture() const;
    u32 *getNoise() const;


    //
    // Configuring
    //

    // Launches the emulator thread
    void launch(const void *listener, Callback *func);

    // Queries an option
    i64 get(Option option) const;
    i64 get(Option option, long id) const;

    // Configures the emulator to match a specific C64 model
    void set(C64Model model);

    // Sets an option
    void set(Option option, i64 value) throws;
    void set(Option option, long id, i64 value) throws;

    // Convenience wrappers
    void set(Option option, const string &value) throws;
    void set(Option option, long id, const string &value) throws;


    //
    // Command queue
    //

    void put(const Cmd &cmd);
    void put(CmdType type, i64 payload = 0) { put(Cmd(type, payload)); }
    void put(CmdType type, KeyCmd payload)  { put(Cmd(type, payload)); }
    void put(CmdType type, CoordCmd payload)  { put(Cmd(type, payload)); }
    void put(CmdType type, GamePadCmd payload)  { put(Cmd(type, payload)); }
    void put(CmdType type, TapeCmd payload)  { put(Cmd(type, payload)); }
    void put(CmdType type, AlarmCmd payload)  { put(Cmd(type, payload)); }


    //
    // C64
    //

    struct C64_API : public API {

        using API::API;

        // Performs a hard or soft reset
        void hardReset();
        void softReset();

        void stopAndGo();
        void stepInto();
        void stepOver();

        InspectionTarget getInspectionTarget() const;
        void setInspectionTarget(InspectionTarget target, Cycle trigger = 0);
        void removeInspectionTarget();

        C64Info getInfo() const;
        EventSlotInfo getSlotInfo(isize nr) const;

        void isReady();

        Snapshot *latestAutoSnapshot();
        Snapshot *latestUserSnapshot();
        void loadSnapshot(const Snapshot &snapshot);

        RomInfo getRomInfo(RomType type) const;
        void loadRom(const string &path);
        void loadRom(const RomFile &file);
        void deleteRom(RomType type);
        void saveRom(RomType rom, const string &path);

        void flash(const AnyFile &file);
        void flash(const AnyCollection &file, isize item);
        void flash(const FileSystem &fs, isize item);

    } c64;


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
    // Guards
    //

    struct GUARD_API : API {

        Guards &guards;
        GUARD_API(Emulator &emu, Guards& guards) : API(emu), guards(guards) { }

        long elements() const;
        u32 guardAddr(long nr) const;
        bool isEnabled(long nr) const;
        bool isDisabled(long nr) const;
        bool isSetAt(u32 addr) const;
        bool isSetAndEnabledAt(u32 addr) const;
        bool isSetAndDisabledAt(u32 addr) const;
        bool isSetAndConditionalAt(u32 addr) const;

        void setEnable(long nr, bool val);
        void enable(long nr);
        void disable(long nr);

        void setEnableAt(u32 addr, bool val);
        void enableAt(u32 addr);
        void disableAt(u32 addr);

        void addAt(u32 addr, long skip = 0);
        void removeAt(u32 addr);

        void remove(long nr);
        void removeAll();

        void replace(long nr, u32 addr);
    };


    //
    // CPU
    //

    struct CPU_API : API {

        using API::API;

        GUARD_API breakpoints;
        GUARD_API watchpoints;

        CPU_API(Emulator &emu) :
        API(emu),
        breakpoints(emu, emu.main.cpu.debugger.breakpoints),
        watchpoints(emu, emu.main.cpu.debugger.watchpoints) { }

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

        CIAConfig getConfig() const;
        CIAInfo getInfo() const;
        CIAStats getStats() const;

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

        u32 getColor(isize nr) const;
        u32 getColor(isize nr, Palette palette) const;

    } vicii;


    //
    // SID
    //

    struct SID_API : API {

        using API::API;

        MuxerConfig getConfig() const;
        SIDInfo getInfo(isize nr) const;
        VoiceInfo getVoiceInfo(isize nr, isize voice) const;
        MuxerStats getStats() const;

        void rampUp();
        void rampUp(float from);
        void rampDown();

        void copyMono(float *buffer, isize n);
        void copyStereo(float *left, float *right, isize n);
        void copyInterleaved(float *buffer, isize n);

        float draw(u32 *buffer, isize width, isize height,
                   float maxAmp, u32 color, isize sid = -1) const;
    } muxer;


    //
    // DMA Debugger
    //

    struct DMA_DEBUGGER_API : API {

        using API::API;

        // Returns the current configuration
        DmaDebuggerConfig getConfig() const;

    } dmaDebugger;


    //
    // Keyboard
    //

    struct KBD_API : API {

        using API::API;

        bool isPressed(C64Key key) const;
        void autoType(const string &text);
        void abortAutoTyping();

    } keyboard;


    //
    // Mouse
    //

    struct MOUSE_API : API {

        Mouse &mouse;
        MOUSE_API(Emulator &emu, Mouse& mouse) : API(emu), mouse(mouse) { }

        bool detectShakeXY(double x, double y);
        bool detectShakeDxDy(double dx, double dy);

        // Emulates a mouse movement
        void setXY(double x, double y);
        void setDxDy(double dx, double dy);

        // Triggers a gamepad event
        void trigger(GamePadAction event);
    };


    //
    // Joystick
    //

    struct JOYSTICK_API : API {

        Joystick &joystick;
        JOYSTICK_API(Emulator &emu, Joystick& joystick) : API(emu), joystick(joystick) { }

        // Triggers a gamepad event
        void trigger(GamePadAction event);
    };


    //
    // Mouse
    //

    struct DATASETTE_API : API {

        using API::API;

        DatasetteInfo getInfo() const;

        void insertTape(TAPFile &file);
        void ejectTape();

    } datasette;


    //
    // Control port
    //

    struct CP_API : API {

        ControlPort &port;
        CP_API(Emulator &emu, ControlPort& port) :
        API(emu), port(port), joystick(emu, port.joystick), mouse(emu, port.mouse) { }

        JOYSTICK_API joystick;
        MOUSE_API mouse;

    } port1, port2;


    //
    // Recorder
    //

    struct REC_API : API {

        using API::API;

        const string getExecPath() const;
        void setExecPath(const string &path);
        bool available() const;
        util::Time getDuration() const;
        isize getFrameRate() const;
        isize getBitRate() const;
        isize getSampleRate() const;
        bool isRecording() const;
        void startRecording(isize x1, isize y1, isize x2, isize y2,
                            isize bitRate,
                            isize aspectX, isize aspectY);
        void stopRecording();
        bool exportAs(const string &path);

    } recorder;


    //
    // Expansion port
    //

    struct EXP_PORT_API : API {

        using API::API;

        CartridgeTraits getTraits() const;
        CartridgeInfo getInfo() const;
        CartridgeRomInfo getRomInfo(isize nr) const;

        void attachCartridge(const string &path, bool reset = true);
        void attachCartridge(CRTFile *c, bool reset = true);
        void attachCartridge(Cartridge *c);
        void attachReu(isize capacity);
        void attachGeoRam(isize capacity);
        void attachIsepicCartridge();
        void detachCartridge();

    } expansionport;


    //
    // IEC bus
    //

    struct IEC_API : API {

        using API::API;

    } iec;


    //
    // Disk
    //

    struct DISK_API : API {

        Drive &drive;
        DISK_API(Emulator &emu, Drive& drive) : API(emu), drive(drive) { }
    };


    //
    // Drive
    //

    struct DRIVE_API : API {

        Drive &drive;
        DRIVE_API(Emulator &emu, Drive& drive) : API(emu), drive(drive), disk(emu, drive) { }

        DISK_API disk;

        const DriveConfig &getConfig() const;
        DriveInfo getInfo() const;

        void insertBlankDisk(DOSType fstype, PETName<16> name);
        void insertD64(const D64File &d64, bool wp);
        void insertG64(const G64File &g64, bool wp);
        void insertCollection(AnyCollection &archive, bool wp) throws;
        void insertFileSystem(const class FileSystem &device, bool wp);
        void ejectDisk();

    } drive8, drive9;


    //
    // RetroShell
    //

    struct RSHELL_API : API {

        using API::API;

        const char *text();
        isize cursorRel();

        void press(RetroShellKey key, bool shift = false);
        void press(char c);
        void press(const string &s);

        void execScript(std::stringstream &ss);
        void execScript(const std::ifstream &fs);
        void execScript(const string &contents);

        void setStream(std::ostream &os);

    } retroShell;
};
