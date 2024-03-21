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
#include <swift/bridging>

namespace vc64 {

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

    static VirtualC64 *make() { return new VirtualC64(); }

    // Experimental (for new Swift API)
    static VirtualC64 *make(void *p) { return (VirtualC64 *)p; }

    /// @brief  A reference to the user default storage.
    static const Defaults &defaults;

    /// @name Analyzing the emulator
    /// @{

    /** @brief  Returns the component's current configuration.
     */
    const EmulatorConfig &getConfig() const SWIFT_RETURNS_INDEPENDENT_VALUE { return Emulator::getConfig(); }

    /** @brief  Returns the component's current state.
     */
    const EmulatorInfo &getState() const SWIFT_RETURNS_INDEPENDENT_VALUE { return Emulator::getState(); }

    /** @brief  Returns statistical information about the components.
     */
    const EmulatorStats &getStats() const SWIFT_RETURNS_INDEPENDENT_VALUE { return Emulator::getStats(); }

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
    void suspendThread() { suspend(); }

    /** @brief   Suspends the emulator thread
     *
     *  See the \ref vc64::Suspendable "Suspendable" class for a detailes
     *  description of the suspend-resume machanism.
     */
    void resume() { Emulator::resume(); }
    void resumeThread() { resume(); }

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
     * The texture dimensions are given by constants vc64::Texture::width
     * and vc64::Texture::height texels. Each texel is represented by a
     * 32 bit color value.
     */
    u32 *getTexture() const SWIFT_RETURNS_INDEPENDENT_VALUE { return Emulator::getTexture(); }

    /** Returns a pointer to a noise pattern
     *
     *  The pattern resembles the white noise produced by older analog TVs when
     *  TV signal was present. The returned pattern is pseudo-random and
     *  changes with each call of this function.
     *
     *  @note The Mac app displays this pattern when the emulator is powered on.
     *  After powering on, the emulator texture is displayed.
     */
    u32 *getNoise() const SWIFT_RETURNS_INDEPENDENT_VALUE { return Emulator::getNoise(); }


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
     *  @param value    The option's value
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
     *  @param value    The option's value
     *
     *  @note This function must only be called for those options that require
     *  an additional parameter to uniquely determine the configured component.
     */
    void set(Option option, long id, i64 value);


    /** @brief  Exports the current configuration.
     *
     *  The current configuration is exported in form of a RetroShell script.
     *  Reading in the script at a later point will restore the configuration.
     */
    void exportConfig(const fs::path &path) const;
    void exportConfig(std::ostream& stream) const;


    /// @}
    /// @name Using the command queue
    /// @{

    /** @brief  Feeds a command into the command queue.
     */
    void put(const Cmd &cmd);
    void put(CmdType type, i64 payload = 0) { put(Cmd(type, payload)); }
    void put(CmdType type, KeyCmd payload)  { put(Cmd(type, payload)); }
    void put(CmdType type, CoordCmd payload)  { put(Cmd(type, payload)); }
    void put(CmdType type, GamePadCmd payload)  { put(Cmd(type, payload)); }
    void put(CmdType type, TapeCmd payload)  { put(Cmd(type, payload)); }
    void put(CmdType type, AlarmCmd payload)  { put(Cmd(type, payload)); }
    /// @}


    /** C64 API
     */
    struct C64API : public API {

        using API::API;

        /// @name Analyzing the emulator
        /// @{

        /** @brief  Returns the component's current state.
         */
        C64Info getInfo() const { return c64.getState(); }

        /** @brief  Returns the current state of an event slot.
         *
         *  @param  nr      Number of the event slot.
         */
        EventSlotInfo getSlotInfo(isize nr) const { return c64.getSlotInfo(nr); }

        /** @brief  Returns information about one of the installed Roms
         *
         *  @param  type    The ROM type
         */
        RomTraits getRomTraits(RomType type) const;


        /** @brief  Checks if the emulator is runnable.
         *  The function checks if the necessary ROMs are installed to lauch the
         *  emulator. On success, the functions returns. Otherwise, an exception
         *  is thrown.
         *
         *  @throw  VC64Error (ERROR_ROM_BASIC_MISSING)
         *  @throw  VC64Error (ERROR_ROM_CHAR_MISSING)
         *  @throw  VC64Error (ERROR_ROM_KERNAL_MISSING)
         *  @throw  VC64Error (ERROR_ROM_CHAR_MISSING)
         *  @throw  VC64Error (ERROR_ROM_MEGA65_MISMATCH)
         */
        void isReady();


        /// @}
        /// @name Resetting the C64
        /// @{

        /** @brief  Performs a hard reset
         *
         *  A hard reset affects all components. The effect is similar to
         *  switching power off and on.
         */
        void hardReset();

        /** @brief  Performs a hard reset
         *
         *  A soft reset emulates a real reset of the C64 which can be initiated
         *  via the reset line on the expansion port.
         */
        void softReset();

        /// @}
        /// @name Setting up auto-inspection
        /// @{

        /** @brief  Returns the current inspection target.
         *
         *  If you open the inspector panel in the Mac app while the emulator
         *  is running, you will see continuous updates of the emulator state.
         *  The displayed information is recorded via the auto-inspection
         *  mechanism. If auto-inspection is active, the emulator schedules an
         *  inspect event which calls function recordState() on the inspection
         *  target in constant intervals. The recorded information is later
         *  picked up by the GUI.
         *
         *  If you change to a different panel in the inspector window, the
         *  emulator will change the inspection target to only record the
         *  information you're seeing in the currently open panel.
         */
        InspectionTarget getInspectionTarget() const;

        /** @brief  Sets the current inspection target.
         */
        void setInspectionTarget(InspectionTarget target);

        /** @brief  Removes the current inspection target.
         */
        void removeInspectionTarget();


        /// @}
        /// @name Handling snapshots
        /// @{

        Snapshot *latestAutoSnapshot();
        Snapshot *latestUserSnapshot();
        void loadSnapshot(const Snapshot &snapshot);

        Snapshot *takeSnapshot();

        /// @}
        /// @name Handling media files
        /// @{

        /** @brief  Loads a ROM from a file
         */
        void loadRom(const string &path);

        /** @brief  Loads a ROM, provided by a RomFile object
         */
        void loadRom(const RomFile &file);

        /** @brief  Removes an installed ROM
         */
        void deleteRom(RomType type);

        /** @brief  Saves a ROM to disk
         */
        void saveRom(RomType rom, const string &path);

        /** @brief  Flashes a file into memory
         */
        void flash(const AnyFile &file);

        /** @brief  Flashes a file from a collection into memory
         */
        void flash(const AnyCollection &file, isize item);

        /** @brief  Flashes a file from a file system into memory
         */
        void flash(const FileSystem &fs, isize item);
        /// @}

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


    /** API for breakpoints and watchpoints
     */
    struct GuardAPI : API {

        Guards &guards;
        GuardAPI(Emulator &emu, Guards& guards) : API(emu), guards(guards) { }

        /// @name Inspecting the guard list
        /// @{

        /** @brief  Returns the number of guards in the guard list.
         */
        long elements() const;

        /** @brief  Returns a guard from the guard list.
         *  @param  nr      Number of the guard.
         *  @return A pointer to the guard or nullptr if the guard does not exist.
         */
        Guard *guardNr(long nr) const;

        /** @brief  Returns the guard set on a specific address.
         *  @param  addr    Memory address.
         *  @return A pointer to the guard or nullptr if the address is not guarded.
         */
        Guard *guardAt(u32 addr) const;

        /// @}
        /// @name Adding or removing guards
        /// @{

        /** Checks whether the guard list contains a guard at a specific position.
         *  @param  nr      A position in the guard list.
         */
        bool isSet(long nr) const;

        /** Checks whether a guard is set on a specific memory location.
         *  @param  addr    A memory location.
         */
        bool isSetAt(u32 addr) const;

        /** Sets a guard on a specific memory location.
         *  @param  addr    A memory location.
         *  @param  ignore  An optional ignore counter.
         */
        void setAt(u32 addr, long ignore = 0);

        /** Replaces the observed address of a specific guard.
         *  @param  nr      A position in the guard list.
         *  @param  addr    The new memory location.
         */
        void replace(long nr, u32 addr);

        /** Deletes a guard from the guard list.
         *  @param  nr      A position in the guard list.
         */
        void remove(long nr);

        /** Deletes a guard from the guard list.
         *  @param  addr    Observed memory address of the guard to be removed.
         */
        void removeAt(u32 addr);

        /** Deletes the entire guard list.
         */
        void removeAll();

        /// @}
        /// @name Enabling or disabling guards
        /// @{

        /** Checks if a guards is enabled.
         *  @param  nr      A position in the guard list.
         *  @return true if the guard list contains an enabled guard at the
         *  provided location.
         */
        bool isEnabled(long nr) const;

        /** Checks if a guards is enabled.
         *  @param  addr    Observed memory address of the guard to check.
         *  @return true if the provided memory location is observed by an
         *  enabled guard.
         */
        bool isEnabledAt(u32 addr) const;

        /** Checks if a guards is disabled.
         *  @param  nr      A position in the guard list.
         *  @return true if the guard list contains a disabled guard at the
         *  provided location.
         */
        bool isDisabled(long nr) const;

        /** Checks if a guards is disabled.
         *  @param  addr    Observed memory address of the guard to check.
         *  @return true if the provided memory location is observed by a
         *  disabled guard.
         */
        bool isDisabledAt(u32 addr) const;

        /** Enables a guard
         *  @param  nr      Position of the guard in the guard list.
         */
        void enable(long nr);

        /** Enables a guard
         *  @param  addr    Observed memory address of the guard to enable.
         */
        void enableAt(u32 addr);

        /** Disables a guard
         *  @param  nr      Position of the guard in the guard list.
         */
        void disable(long nr);

        /** Disables a guard
         *  @param  addr    Observed memory address of the guard to disable.
         */
        void disableAt(u32 addr);

        /// @}
    };


    /** CPU API
     */
    struct CPUAPI : API {

        using API::API;

        GuardAPI breakpoints;
        GuardAPI watchpoints;

        CPUAPI(Emulator &emu) :
        API(emu),
        breakpoints(emu, emu.main.cpu.debugger.breakpoints),
        watchpoints(emu, emu.main.cpu.debugger.watchpoints) { }

        CPUInfo getInfo() const;

        /** @brief  Returns the number of instructions in the instruction log.
         */
        isize loggedInstructions() const;

        /** @brief  Empties the instruction log.
         */
        void clearLog();

        /** Determines how the disassembler displays numbers
         *  @param  instrFormat Format for numbers inside instructions
         *  @param  dataFormat  Format for printed data values
         */
        void setNumberFormat(DasmNumberFormat instrFormat, DasmNumberFormat dataFormat);

        /** Disassembles an instruction
         *  @param  dst     Destination buffer
         *  @param  fmt     String definining the output format
         *  @param  addr    Address of the instruction to disassemble
         */
        isize disassemble(char *dst, const char *fmt, u16 addr) const;

        /** Disassembles an instruction from the record buffer
         *  @param  dst     Destination buffer
         *  @param  fmt     String definining the output format
         *  @param  nr      Index of the instruction in the record buffer
         */
        isize disassembleRecorded(char *dst, const char *fmt, isize nr) const;

    } cpu;


    /** CIA API
     */
    struct CIAAPI : API {

        CIA &cia;
        CIAAPI(Emulator &emu, CIA& cia) : API(emu), cia(cia) { }

        /** @brief  Returns the component's current configuration.
         */
        CIAConfig getConfig() const;

        /** @brief  Returns the component's current state.
         */
        CIAInfo getInfo() const;

        /** @brief  Returns statistical information about the components.
         */
        CIAStats getStats() const;

    } cia1, cia2;


    /** VICII API
     */
    struct VICIIAPI : API {

        using API::API;

        /** @brief  Returns the component's current configuration.
         */
        VICIIConfig getConfig() const;

        /** @brief  Returns the component's current state.
         */
        VICIIInfo getInfo() const;

        /** @brief  Returns information about a sprite.
         *  @param  nr   Number of the sprite (0 .. 7)
         */
        SpriteInfo getSpriteInfo(isize nr) const;

        /** @brief  Returns the number of CPU cycles per rasterline.
         */
        isize getCyclesPerLine() const;

        /** @brief  Returns the number of rasterlines per frame.
         */
        isize getLinesPerFrame() const;

        /** @brief  Checks if the seleted VICII chip is a PAL revision.
         */
        bool pal() const;

        /** @brief  Returns the RGBA value of a color
         */
        u32 getColor(isize nr) const;

        /** @brief  Returns the RGBA value of a color from a color palette.
         */
        u32 getColor(isize nr, Palette palette) const;

    } vicii;


    /** SID Public API
     */
    struct SIDAPI : API {

        using API::API;

        /// @name Querying the component
        /// @{

        /** @brief  Returns the component's current configuration.
         */
        MuxerConfig getConfig() const;

        /** @brief  Returns the current state of a specific SID.
         *  @param  nr      SID number (0 - 3). 0 is the primary SID.
         */
        SIDInfo getInfo(isize nr) const;

        /** @brief  Returns the current state of a specific voice.
         *  @param  nr      SID number (0 - 3). 0 is the primary SID.
         *  @param  voice   Voice nuber (0 - 3).
         */
        VoiceInfo getVoiceInfo(isize nr, isize voice) const;

        /** @brief  Returns statistical information about the components.
         */
        MuxerStats getStats() const;

        /// @}
        /// @name Modulating the volume
        /// @{

        /** @brief  Ramps the volume up.
         *  Starting from the current volume, the volume is slowly increased
         *  until the target volume has been reached. Smooth modulation of the
         *  volume helps to avoid cracks in the audio output.
         */
        void rampUp();

        /** @brief  Ramps the volume up.
         *  Similar to rampUp() with a provides starting volume.
         *  @param  from    The starting volume.
         */
        void rampUp(float from);

        /** @brief  Ramps the volume down.
         *  Starting from the current volume, the volume is slowly decreased
         *  until 0 has been reached. This function is used, for example, when
         *  the user hits the pause button to avoid cracks in the audio output.
         */
        void rampDown();

        /// @}
        /// @name Retrieving audio data
        /// @{

        /** @brief  Extracts a number of mono samples from the audio buffers
         *  Internally, the muxer maintains two buffers, one for the left audio
         *  channel, and one for the right audio channel. When this function
         *  is used, both internal stream are added together and written to
         *  to the provided destination buffer.
         *  @param  buffer  Pointer to the destination buffer
         *  @param  n       Number of sound samples to copy.
         */
        void copyMono(float *buffer, isize n);

        /** @brief  Extracts a number of stereo samples from the audio buffers.
         *  @param  left    Pointer to the left channel's destination buffer.
         *  @param  right   Pointer to the right channel's destination buffer.
         *  @param  n       Number of sound samples to copy.
         */
        void copyStereo(float *left, float *right, isize n);

        /** @brief  Extracts a number of stereo samples from the audio buffers.
         *  This function has to be used if a stereo stream is managed in a
         *  single destination buffer. The samples of both channels will be
         *  interleaved, that is, a sample for the left channel will be
         *  followed by a sample of the right channel and vice versa.
         *  @param  buffer  Pointer to the destinationleft buffer.
         *  @param  n       Number of sound samples to copy.
         */
        void copyInterleaved(float *buffer, isize n);

        /// @}
        /// @name Visualizing waveforms
        /// @{

        /** @brief  Draws a visual representation of the waveform.
         *  The Mac app uses this function to visualize the contents of the
         *  audio buffer in one of it's inspector panels. */
        float draw(u32 *buffer, isize width, isize height,
                   float maxAmp, u32 color, isize sid = -1) const;
        /// @}

    } muxer;


    /** DMA Debugger Public API
     */
    struct DmaDebuggerAPI : API {

        using API::API;

        /** @brief  Returns the component's current configuration
         */
        DmaDebuggerConfig getConfig() const;

    } dmaDebugger;


    /** Keyboard Public API
     */
    struct KeyboardAPI : API {

        using API::API;

        /** @brief  Checks if a key is currently pressed.
         *  @param  key     The key to check.
         */
        bool isPressed(C64Key key) const;

        /** @brief  Uses the auto-typing daemon to type a string.
         *  @param  text    The text to type.
         */
        void autoType(const string &text);

        /** @brief  Aborts any active auto-typing activity.
         */
        void abortAutoTyping();

    } keyboard;


    /** Mouse Public API
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

        /** @brief  Signals a mouse button to the emulator.
         *
         *  @param  event   The button event.
         */
        void trigger(GamePadAction event);
    };


    /** Joystick Public API
     */
    struct JoystickAPI : API {

        Joystick &joystick;
        JoystickAPI(Emulator &emu, Joystick& joystick) : API(emu), joystick(joystick) { }

        /** @brief  Triggers a gamepad event
         *  @param  event    The event to trigger.
         */
        void trigger(GamePadAction event);
    };


    /** Datasette Public API
     */
    struct DatasetteAPI : API {

        using API::API;

        /** @brief  Returns the component's current state.
         */
        DatasetteInfo getInfo() const;

        /** @brief  Inserts a tape.
         *  @param  file    The tape to insert.
         */
        void insertTape(TAPFile &file);

        /** @brief  Ejects a tape.
         *  This function has no effect if no tape is inserted.
         */
        void ejectTape();

    } datasette;


    /** Control Port API
     */
    struct ControlPortAPI : API {

        ControlPort &port;
        ControlPortAPI(Emulator &emu, ControlPort& port) :
        API(emu), port(port), joystick(emu, port.joystick), mouse(emu, port.mouse) { }

        JoystickAPI joystick;
        MouseAPI mouse;

    } port1, port2;


    /** Screen Recorder Public API
     */
    struct RecorderAPI : API {

        using API::API;

        /** @brief  Returns the component's configuration.
         */
        const RecorderConfig &getConfig() const;

        /** @brief  Returns the component's current state.
         */
        RecorderInfo getState() const;

        /** @brief  Returns the path to the FFmpeg executable.
         */
        const string getExecPath() const;

        /** @brief  Sets the path to the FFmpeg executable.
         */
        void setExecPath(const string &path);

        /** @brief  Starts the recorder.
         *  @param  x1      Horizontal start coordinate of the recorded area
         *  @param  y1      Vertical start coordinate of the recorded area
         *  @param  x2      Horizontal end coordinate of the recorded area
         *  @param  y2      Vertical stop coordinate of the recorded area
         */
        void startRecording(isize x1, isize y1, isize x2, isize y2);

        /** @brief  Interrupts a recording in progress.
         */
        void stopRecording();

        /** @brief  Exports the recorded video to a file.
         *  @param  path    The export destination.
         *  @return true on success.
         */
        bool exportAs(const string &path);

    } recorder;


    /** Expansion Port Public API
     */
    struct ExpansionPortAPI : API {

        using API::API;

        /// @{
        /// @name Analyzing cartridges.

        /** @brief  Returns the traits of the current cartridge.
         */
        CartridgeTraits getCartridgeTraits() const;

        /** @brief  Returns the state of the current cartridge.
         */
        CartridgeInfo getInfo() const;

        /** @brief  Returns the state of one of the cartridge ROM packets.
         *  @param  nr      Number of the ROM packet.
         */
        CartridgeRomInfo getRomInfo(isize nr) const;

        /// @}
        /// @name Attaching and detaching cartridges.
        /// @{

        /** @brief  Attaches a cartridge to the expansion port.
         */
        void attachCartridge(const string &path, bool reset = true);

        /** @brief  Attaches a cartridge to the expansion port.
         */
        void attachCartridge(const CRTFile &c, bool reset = true);

        /** @brief  Attaches a cartridge to the expansion port.
         */
        void attachCartridge(Cartridge *c);

        /** @brief  Attaches a RAM Expansion Unit to the expansion port.
         */
        void attachReu(isize capacity);

        /** @brief  Attaches a GeoRAM module to the expansion port.
         */
        void attachGeoRam(isize capacity);

        /** @brief  Attaches an Isepic module to the expansion port.
         */
        void attachIsepicCartridge();

        /** @brief  Detaches the currently plugged in cartridge
         */
        void detachCartridge();

        /// @}

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

    /** Disk Public API
     */
    struct DiskAPI : API {

        Drive &drive;
        DiskAPI(Emulator &emu, Drive& drive) : API(emu), drive(drive) { }

        Disk *get() { return drive.disk.get(); }
    };


    //
    // Drive
    //

    /** Drive Public API
     */
    struct DriveAPI : API {

        Drive &drive;
        DriveAPI(Emulator &emu, Drive& drive) : API(emu), drive(drive), disk(emu, drive) { }

        DiskAPI disk;

        /** @brief  Returns the component's current configuration.
         */
        const DriveConfig &getConfig() const;

        /** @brief  Returns the component's current state.
         */
        DriveInfo getInfo() const;

        /** @brief  Inserts a new disk.
         *  @param  fstype  The file system the disk should be formatted with.
         *  @param  name    A PET string with the name of the new disk.
         */
        void insertBlankDisk(DOSType fstype, PETName<16> name);

        /** @brief  Inserts a disk from a D64 file.
         *  @param  d64     A D64 file wrapper object.
         *  @param  wp      Write-protection status of the disk.
         */
        void insertD64(const D64File &d64, bool wp);

        /** @brief  Inserts a disk from a G64 file.
         *  @param  g64     A G64 file wrapper object.
         *  @param  wp      Write-protection status of the disk.
         */
        void insertG64(const G64File &g64, bool wp);

        /** @brief  Inserts a disk with the contents of a file collection.
         *  @param  archive A file collection wrapper object.
         *  @param  wp      Write-protection status of the disk.
         *  @throw  VC64Error
         */
        void insertCollection(AnyCollection &archive, bool wp);

        /** @brief  Inserts a disk created from a file system.
         *  @param  fs      A file system wrapper object.
         *  @param  wp      Write-protection status of the disk.
         */
        void insertFileSystem(const class FileSystem &fs, bool wp);

        /** @brief  Ejects the current disk.
         */
        void ejectDisk();

    } drive8, drive9;


    /** RetroShell Public API
     */
    struct RetroShellAPI : API {

        using API::API;

        /// @name Querying the console
        /// @{
        ///
        /** @brief  Returns a pointer to the text buffer.
         *  The text buffer contains the complete contents of the console. It
         *  will be expanded when new output is generated. When the buffer
         *  grows too large, old contents is cropped.
         */
        const char *text();

        /** @brief  Returns the relative cursor position.
         *  The returned value is relative to the end of the input line. A value
         *  of 0 indicates that the cursor is at the rightmost position, that
         *  is, one character after the last character of the input line. If the
         *  cursor is at the front of the input line, the value matches the
         *  length of the input line.
         */
        isize cursorRel();

        /// @}
        /// @name Typing characters and strings
        /// @{

        /** @brief  Informs RetroShell that a key has been typed.
         *  @param  key     The pressed key
         *  @param  shift   Status of the shift key
         */
        void press(RetroShellKey key, bool shift = false);

        /** @brief  Informs RetroShell that a key has been typed.
         *  @param  c       The pressed key
         */
        void press(char c);

        /** @brief  Informs RetroShell that multiple keys have been typed.
         *  @param  s       The typed text
         */
        void press(const string &s);

        /// @}
        /// @name Controlling the output stream
        /// @{

        /** @brief  Assign an additional output stream.
         *  In addition to writing the RetroShell output into the text buffer,
         *  RetroShell will write the output into the provides stream.
         */
        void setStream(std::ostream &os);

        /// @}
        /// @name Executing scripts
        /// @{

        /** @brief  Executes a script.
         *  The script is executes asynchroneously. However, RetroShell will
         *  send messages back to the GUI thread to inform about the execution
         *  state. After the last script command has been executed,
         *  MSG\_SCRIPT\_DONE is sent. If shell execution has been aborted due
         *  to an error, MSG\_SCRIPT\_ABORT is sent.
         */
        void execScript(std::stringstream &ss);
        void execScript(const std::ifstream &fs);
        void execScript(const string &contents);

        /// @}

    } retroShell;

} SWIFT_IMMORTAL_REFERENCE;

}
