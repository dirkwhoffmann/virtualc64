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
    breakpoints(emu, emu._c64.cpu.debugger.breakpoints),
    watchpoints(emu, emu._c64.cpu.debugger.watchpoints) { }

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
// SID
//

struct SID_API : API {

    using API::API;

    SIDConfig getConfig() const;
    SIDInfo getInfo(isize nr) const;
    VoiceInfo getVoiceInfo(isize nr, isize voice) const;
    SIDStats getStats() const;

    void rampUp();
    void rampUpFromZero();
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
    bool shiftLockIsPressed() const;
    bool restoreIsPressed() const;
    bool commodoreIsPressed() const { return isPressed(C64Key::commodore); }
    bool ctrlIsPressed() const { return isPressed(C64Key::control); }
    bool runstopIsPressed() const { return isPressed(C64Key::runStop); }
    bool leftShiftIsPressed() const { return isPressed(C64Key::leftShift); }
    bool rightShiftIsPressed() const { return isPressed(C64Key::rightShift); }

    // Presses a key
    void press(C64Key key);
    void pressShiftLock();
    void pressRestore();
    void pressCommodore() { press(C64Key::commodore); }
    void pressCtrl() { press(C64Key::control); }
    void pressRunstop() { press(C64Key::runStop); }
    void pressLeftShift() { press(C64Key::leftShift); }
    void pressRightShift() { press(C64Key::rightShift); }

    // Releases a pressed key
    void release(C64Key key);
    void releaseShiftLock();
    void releaseRestore();
    void releaseCommodore() { release(C64Key::commodore); }
    void releaseCtrl() { release(C64Key::control); }
    void releaseRunstop() { release(C64Key::runStop); }
    void releaseLeftShift() { release(C64Key::leftShift); }
    void releaseRightShift() { release(C64Key::rightShift); }

    // Presses a released key and vice versa
    void toggle(C64Key key) { isPressed(key) ? release(key) : press(key); }
    void toggleShiftLock() { shiftLockIsPressed() ? releaseShiftLock() : pressShiftLock(); }
    void toggleRestore() { restoreIsPressed() ? releaseRestore() : pressRestore(); }
    void toggleCommodore() { toggle(C64Key::commodore); }
    void toggleCtrl() { toggle(C64Key::control); }
    void toggleRunstop() { toggle(C64Key::runStop); }
    void toggleLeftShift() { toggle(C64Key::leftShift); }
    void toggleRightShift() { toggle(C64Key::rightShift); }

    // Clears the keyboard matrix
    void releaseAll();


    // Auto typing
    void autoType(const string &text);

    void scheduleKeyPress(std::vector<C64Key> keys, double delay);
    void scheduleKeyPress(C64Key key, double delay) { scheduleKeyPress(std::vector<C64Key>{key}, delay); }
    void scheduleKeyPress(char c, double delay) { scheduleKeyPress(C64Key::translate(c), delay); }

    void scheduleKeyRelease(std::vector<C64Key> keys, double delay);
    void scheduleKeyRelease(C64Key key, double delay) { scheduleKeyRelease(std::vector<C64Key>{key}, delay); }
    void scheduleKeyRelease(char c, double delay) { scheduleKeyRelease(C64Key::translate(c), delay); }

    void scheduleKeyReleaseAll(double delay);

    // Deletes all pending actions and clears the keyboard matrix
    void abortAutoTyping();

} keyboard;


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
    std::vector<string> &paths() const;
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

    CartridgeInfo getInfo() const;
    CartridgeRomInfo getRomInfo(isize nr) const;

    CartridgeType getCartridgeType() const;
    bool getCartridgeAttached() const;
    void attachCartridge(const string &path, bool reset = true);
    void attachCartridge(CRTFile *c, bool reset = true);
    void attachCartridge(Cartridge *c);
    void attachReu(isize capacity);
    void attachGeoRam(isize capacity);
    void attachIsepicCartridge();

    void detachCartridge();
    void detachCartridgeAndReset();
    isize numButtons() const;
    const string getButtonTitle(isize nr) const;
    void pressButton(isize nr);
    void releaseButton(isize nr);
    bool hasSwitch() const;
    isize getSwitch() const;
    bool switchIsNeutral() const;
    bool switchIsLeft() const;
    bool switchIsRight() const;
    const string getSwitchDescription(isize pos) const;
    const string getSwitchDescription() const;
    bool validSwitchPosition(isize pos) const;
    void setSwitch(isize pos);
    bool hasLED() const;
    bool getLED() const;
    void setLED(bool value);
    isize getRamCapacity() const;
    bool hasBattery() const;
    void setBattery(bool value);

} expansionport;


//
// IEC bus
//

struct IEC_API : API {

    using API::API;

    bool isTransferring() const;

} iec;


//
// Disk
//

struct DISK_API : API {

    Drive &drive;
    DISK_API(Emulator &emu, Drive& drive) : API(emu), drive(drive) { }

    bool isWriteProtected() const;
    void setWriteProtection(bool b);
    void toggleWriteProtection();
};


//
// Drive
//

struct DRIVE_API : API {

    Drive &drive;
    DRIVE_API(Emulator &emu, Drive& drive) : API(emu), drive(drive), disk(emu, drive) { }

    DISK_API disk;

    bool hasDisk() const;
    bool hasPartiallyRemovedDisk() const;
    bool hasProtectedDisk() const;
    bool hasModifiedDisk() const;
    bool hasUnmodifiedDisk() const;
    bool hasUnprotectedDisk() const;
    void setModificationFlag(bool value);
    void markDiskAsModified();
    void markDiskAsUnmodified();
    /*
     bool isModified() const;
     void setModified(bool b);
     */

    isize getDeviceNr() const;
    bool getRedLED() const;
    bool isRotating() const;
    const DriveConfig &getConfig() const;
    bool readMode() const;
    bool writeMode() const;

    Halftrack getHalftrack() const;
    Track getTrack() const;
    isize sizeOfHalftrack(Halftrack ht);
    isize sizeOfCurrentHalftrack();
    HeadPos getOffset() const;

    void insertNewDisk(DOSType fstype, PETName<16> name);
    void insertD64(const D64File &d64, bool wp);
    void insertG64(const G64File &g64, bool wp);
    void insertCollection(AnyCollection &archive, bool wp) throws;
    void insertFileSystem(const class FileSystem &device, bool wp);
    void ejectDisk();

    u8 readBitFromHead() const;

} drive8, drive9;




//
// RetroShell
//

struct RSHELL_API : API {

    using API::API;

    const char *text();
    void press(RetroShellKey key, bool shift = false);
    void press(char c);
    void press(const string &s);

    isize cursorRel();

} retroShell;

