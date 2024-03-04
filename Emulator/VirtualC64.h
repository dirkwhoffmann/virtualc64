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
/// @file

#pragma once

#include "VirtualC64Types.h"
#include "Emulator.h"
#include "Media.h"

/** Public API
 *
 * This class declares the emulator's public API. It consists of functions
 * controlling the emulator state, such as running or pausing the emulator, as
 * well as functions configuring the various components. The class contains
 * separate sub-APIs for the subcomponents of the emulator. For example, a
 * VICII API provides additional functions that interact directly with the
 * VICII graphics chip.
 */
class VirtualC64 : vc64::Emulator {

    using vc64::Emulator::Thread::Suspendable;


    //
    // Static methods
    //

public:

    /** @brief  Returns a version string for this release.
     */
    static string version();

    /** @brief  eturns a build-number string for this release.
     */
    static string build();


    //
    // Initializing
    //

public:

    VirtualC64();
    ~VirtualC64();

    /// @brief  A reference to the user default storage.
    static const Defaults &defaults;

    /// @name Analyzing the emulator
    /// @{

    /** @brief  Returns the component's current configuration.
     */
    const EmulatorConfig &getConfig() const { return Emulator::getConfig(); }

    /** @brief  Returns the component's current state.
     */
    const EmulatorInfo &getState() const { return Emulator::getState(); }

    /** @brief  Returns statistical information about the components.
     */
    const EmulatorStats &getStats() const { return Emulator::getStats(); }

    /// @}
    /// @name Querying the emulator state
    /// @{

    /** @brief  Returns true iff the emulator if the emulator is powered on.
     */
    bool isPoweredOn() { return Emulator::isPoweredOn(); }

    /** @brief  Returns true iff the emulator if the emulator is powered off.
     */
    bool isPoweredOff() { return Emulator::isPoweredOff(); }

    /** @brief  Returns true iff the emulator is in paused state.
     */
    bool isPaused() { return Emulator::isPaused(); }

    /** @brief  Returns true iff the emulator is running.
     */
    bool isRunning() { return Emulator::isRunning(); }

    /** @brief  Returns true iff the emulator has been suspended.
     */
    bool isSuspended() { return Emulator::isSuspended(); }

    /** @brief  Returns true iff the emulator has shut down.
     */
    bool isHalted() { return Emulator::isHalted(); }

    /** @brief  Returns true iff warp mode is active.
     */
    bool isWarping() { return Emulator::isWarping(); }

    /** @brief  Returns true iff the emulator runs in track mode.
     */
    bool isTracking() { return Emulator::isTracking(); }


    /// @}
    /// @name Controlling the emulator state
    /// @{

    /** @brief  Switches the emulator on
     *
     *  Powering on the emulator changes the interal state to #STATE\_PAUSED,
     *  the same state that is entered when the user hits the pause button.
     *  Calling this function on an already powered-on emulator has no effect.
     *  */
    void powerOn() { Emulator::Thread::powerOn(); }

    /** @brief  Switches the emulator off
     *
     *  Powering off the emulator changes the interal state of #STATE\_OFF.
     *  Calling this function on an already powered-off emulator has no effect.
     */
    void powerOff() { Emulator::Thread::powerOff(); }

    /** @brief  Starts emulation
     *
     *  Running the emulator changes the internal state to #STATE\_RUNNING,
     *  which is the normal operation state. Frames are computed at normal
     *  pace, if warp mode is switched off, or as fast as possible, if warp
     *  mode is switched on. If this function is called for a powere-off
     *  emulator, an implicit call to powerOn() will be performed.
     */
    void run() { Emulator::Thread::run(); }

    /** @brief   Pauses emulation
     *
     * Pausing the emulator changes the interal state from #STATE\_RUNNING 
     * to #STATE\_PAUSED after completing the curent frame. The emulator
     * enteres a frozes state where no more frames are computed.
     */
    void pause() { Emulator::Thread::pause(); }

    /** @brief   Terminates the emulator thread
     *
     *  Halting the emulator changes the internal state to #STATE\_HALTED.
     *  This state is part of the shutdown procedure and never entered during
     *  normal operation.
     */
    void halt() { Emulator::Thread::halt(); }

    /** @brief   Runs or pauses the emulator
     *
     *  The function calls pause() if the emulator is running, and run()
     *  otherwise.
     */
    void stopAndGo() { Emulator::stopAndGo(); }

    /** @brief   Suspends the emulator thread
     *
     *  See the \ref vc64::Suspendable "Suspendable" class for a detailes 
     *  description of the suspend-resume machanism.
     */
    void suspend() { Emulator::suspend(); }

    /** @brief   Suspends the emulator thread
     *
     *  See the \ref vc64::Suspendable "Suspendable" class for a detailes
     *  description of the suspend-resume machanism.
     */
    void resume() { Emulator::resume(); }

    /** @brief  Enables warp mode.
     */
    void warpOn(isize source = 0) { Emulator::warpOn(source); }

    /** @brief  Disables warp mode.
     */
    void warpOff(isize source = 0) { Emulator::warpOff(source); }

    /** @brief  Enables track mode.
     */
    void trackOn(isize source = 0) { Emulator::trackOn(source); }

    /** @brief  Disables track mode.
     */
    void trackOff(isize source = 0) { Emulator::trackOff(source); }


    /// @}
    /// @name Single-stepping
    /// @{

    /** @brief  Steps a single instruction
     *
     *  If the emulator is paused, calling this functions steps the CPU for
     *  a single instruction. Otherwise, calling this function
     *  has no effect. The CPU debugger utilizes this function to implement single
     *  stepping.
     */
    void stepInto();

    /** @brief  Steps over the current instruction
     *
     *  If the emulator is paused, calling this functions runs the emulator
     *  until the instruction following the current
     *  instruction has been reached. Otherwise, calling this function
     *  has no effect. The CPU debugger utilizes this function to implement single
     *  stepping.
     *
     *  stepOver() acts like stepInto() except for branching instructions.
     *  For instance, if the current instruction is a `JSR` instruction, the
     *  subroutine will be executed in a single chunk.
     *
     *  @note If the current instruction is a branch instruction which never
     *  returns, the emulator will remain in running state. Calling this
     *  function will then be equivalent to a calling run(), with the exception
     *  of a small runtime overhead since the emulator will check after the
     *  each instruction if the program counter has reached it's target
     *  location.
     */
    void stepOver();


    /// @}
    /// @name Synchronizing the emulator thread
    /// @{

    /** @brief  Sends a wakeup signal to the emulator thread.
     *
     *  To compute frames at the proper pace, the emulator core expects the GUI
     *  to send a wakeup signal on each VSYNC pulse. Once this signal is
     *  received, the emulator thread starts computing all missing frames. To
     *  minimize jitter, the wakeup signal should be sent right after the
     *  current texture has been handed over to the GPU.
     */
    void wakeUp() { vc64::Emulator::Thread::wakeUp(); }


    /// @}
    /// @name Accessing video data
    /// @{

    /** @brief  Returns a pointer to the most recent stable texture
      *
      * The texture size vc64::TEX_WIDTH x vc64::TEX_HEIGHT texels.
      * Each texel is 8 bit.
     */
    u32 *getTexture() const { return Emulator::getTexture(); }

    /** Returns a pointer to a noise pattern
     *
     *  The pattern resembles the white noise produced by older analog TVs when
     *  TV signal was present. The returned pattern is pseudo-random and
     *  changes with each call of this function.
     *
     *  @note The Mac app displays this pattern when the emulator is powered on.
     *  After powering on, the emulator texture is displayed.
     */
    u32 *getNoise() const { return Emulator::getNoise(); }


    /// @}
    /// @name Configuring the emulator
    /// @{

    /** @brief  Launches the emulator thread.
     *
     *  This function must be called in the initialization procedure of the
     *  emulator. It launches the emulator thread and registers a callback
     *  function to the message queue. The callback function is a message
     *  receiver processing all incoming messages from the emulator.
     *
     *  @param  listener    An arbitrary pointer which will be passed to the
     *  registered callback function as first argument. You can use the argument
     *  to pass the address of an object into the callback function. Inside the
     *  callback you can utilize the pointer to invoke a message processing
     *  function on that object.
     *  @param  func        The callback function.
     */
    void launch(const void *listener, Callback *func);

    /** @brief  Queries a configuration option.
     *
     *  This is the main function to query a configuration option.
     *
     *  @param option   The option to query
     *
     *  @note Some options require an additional parameter to uniquely
     *  determine the configured component. For those options, this function
     *  must not be called.
     */
    i64 get(Option option) const;

    /** @brief  Queries a configuration option.
     *
     *  This is the main function to query a configuration option.
     *
     *  @param option   The option to query
     *  @param id       The component to query
     *
     *  @note This function must only be called for those options that require
     *  an additional parameter to uniquely determine the configured component.
     */
    i64 get(Option option, long id) const;

    /** Configures the emulator to match a specific C64 model
     *
     *  @param model    The C64 model to emulate
     */
    void set(C64Model model);

    /** @brief  Configures a component.
     *
     *  This is the main function to set an configuration option.
     *
     *  @param option   The option to set
     *
     *  @note If this function is called for an options that applies to multiple
     *  components, all components are configured with the specified value.
     */
    void set(Option option, i64 value) throws;

    /** @brief  Configures a component.
     *
     *  This is the main function to set an configuration option.
     *
     *  @param option   The option to set
     *  @param id       The component to configure
     *
     *  @note This function must only be called for those options that require
     *  an additional parameter to uniquely determine the configured component.
     */
    void set(Option option, long id, i64 value) throws;

    
    /// @}
    /// @name Using the command queue
    /// @{

    /** @brief  Feeds a command into the command queue
     */
    void put(const Cmd &cmd);
    void put(CmdType type, i64 payload = 0) { put(Cmd(type, payload)); }
    void put(CmdType type, KeyCmd payload)  { put(Cmd(type, payload)); }
    void put(CmdType type, CoordCmd payload)  { put(Cmd(type, payload)); }
    void put(CmdType type, GamePadCmd payload)  { put(Cmd(type, payload)); }
    void put(CmdType type, TapeCmd payload)  { put(Cmd(type, payload)); }
    void put(CmdType type, AlarmCmd payload)  { put(Cmd(type, payload)); }
    /// }


    /** C64 API
     */
    struct C64API : public API {

        using API::API;

        /** Performs a hard reset
         *
         *  A hard reset affects all components. The effect is similar to
         *  switching power off and on.
         */
        void hardReset();

        /** Performs a hard reset
         *
         *  A hard reset affects all components. The effect is similar to
         *  switching power off and on.
         */
        void softReset();

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


    /** Memory API
     */
    struct MemoryAPI : API {

        using API::API;

        /** @brief  Returns the component's current configuration.
         */
        MemConfig getConfig() const;

        /** @brief  Returns the component's current state.
         */
        MemInfo getInfo() const;

        /** @brief  Returns a string representations for a portion of memory.
         */
        string memdump(u16 addr, isize num, bool hex, isize pads, MemoryType src) const;
        string txtdump(u16 addr, isize num, MemoryType src) const;

    } mem;


    //
    // Guards
    //

    struct GuardAPI : API {

        Guards &guards;
        GuardAPI(Emulator &emu, Guards& guards) : API(emu), guards(guards) { }

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

    struct CPUAPI : API {

        using API::API;

        GuardAPI breakpoints;
        GuardAPI watchpoints;

        CPUAPI(Emulator &emu) :
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

    struct CIAAPI : API {

        CIA &cia;
        CIAAPI(Emulator &emu, CIA& cia) : API(emu), cia(cia) { }

        CIAConfig getConfig() const;
        CIAInfo getInfo() const;
        CIAStats getStats() const;

    } cia1, cia2;


    //
    // VICII
    //

    struct VICIIAPI : API {

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

    struct SIDAPI : API {

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


    /** DMA Debugger API
     */
    struct DmaDebuggerAPI : API {

        using API::API;

        /** @brief  Returns the component's current configuration
         */
        DmaDebuggerConfig getConfig() const;

    } dmaDebugger;


    /** Keyboard API
     */
    struct KeyboardAPI : API {

        using API::API;

        bool isPressed(C64Key key) const;
        void autoType(const string &text);
        void abortAutoTyping();

    } keyboard;


    /** Mouse API
     */
    struct MouseAPI : API {

        Mouse &mouse;
        MouseAPI(Emulator &emu, Mouse& mouse) : API(emu), mouse(mouse) { }

        /** Feeds a coordinate into the shake detector.
         *
         *  The shake detector keeps track of the transmitted coordinates and
         *  scans for rapid movements caused by shaking the mouse.
         *
         *  @param x    Current horizontal mouse position.
         *  @param y    Current vertical mouse position.
         *  @return     true iff a shaking mouse has been detected.
         */
        bool detectShakeXY(double x, double y);

        /** Feeds a coordinate into the shake detector.
         *
         *  The shake detector keeps track of the transmitted coordinates and
         *  scans for rapid movements caused by shaking the mouse.
         *
         *  @param dx   Current horizontal mouse position, relative to the
         *              previous position.
         *  @param dy   Current vertical mouse position, relative to the
         *              previous position.
         *  @return     true iff a shaking mouse has been detected.
         */
        bool detectShakeDxDy(double dx, double dy);

        /** Signals a mouse movement to the emulator.
         *
         *  @param x    Current horizontal mouse position.
         *  @param y    Current vertical mouse position.
         */
        void setXY(double x, double y);

        /** Signals a mouse movement to the emulator.
         *
         *  @param dx   Current horizontal mouse position, relative to the
         *              previous position.
         *  @param dy   Current vertical mouse position, relative to the
         *              previous position.
         */
        void setDxDy(double dx, double dy);

        /** Signals a mouse button to the emulator.
         *
         *  @param event The button event.
         */
        void trigger(GamePadAction event);
    };


    //
    // Joystick
    //

    struct JoystickAPI : API {

        Joystick &joystick;
        JoystickAPI(Emulator &emu, Joystick& joystick) : API(emu), joystick(joystick) { }

        // Triggers a gamepad event
        void trigger(GamePadAction event);
    };


    //
    // Datasette
    //

    struct DatasetteAPI : API {

        using API::API;

        DatasetteInfo getInfo() const;

        void insertTape(TAPFile &file);
        void ejectTape();

    } datasette;


    //
    // Control port
    //

    struct ControlPortAPI : API {

        ControlPort &port;
        ControlPortAPI(Emulator &emu, ControlPort& port) :
        API(emu), port(port), joystick(emu, port.joystick), mouse(emu, port.mouse) { }

        JoystickAPI joystick;
        MouseAPI mouse;

    } port1, port2;


    //
    // Recorder
    //

    struct RecorderAPI : API {

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

    struct ExpansionPortAPI : API {

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

    struct IECAPI : API {

        using API::API;

    } iec;


    //
    // Disk
    //

    struct DiskAPI : API {

        Drive &drive;
        DiskAPI(Emulator &emu, Drive& drive) : API(emu), drive(drive) { }
    };


    //
    // Drive
    //

    struct DriveAPI : API {

        Drive &drive;
        DriveAPI(Emulator &emu, Drive& drive) : API(emu), drive(drive), disk(emu, drive) { }

        DiskAPI disk;

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

    struct RetroShellAPI : API {

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
