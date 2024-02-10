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
