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

#pragma once

#include "C64Types.h"
#include "MsgQueue.h"
#include "Thread.h"

// Components
#include "Keyboard.h"
#include "Memory.h"
#include "DriveMemory.h"
#include "FlashRom.h"
#include "VICII.h"
#include "SIDBridge.h"
#include "TOD.h"
#include "CIA.h"
#include "CPU.h"
#include "Recorder.h"

// Ports
#include "AudioPort.h"
#include "VideoPort.h"
#include "ControlPort.h"
#include "PowerPort.h"
#include "ExpansionPort.h"
#include "SerialPort.h"
#include "UserPort.h"

// Peripherals
#include "Drive.h"
#include "ParCable.h"
#include "Datasette.h"
#include "Mouse.h"
#include "Monitor.h"

// Cartridges
#include "Cartridge.h"
#include "CustomCartridges.h"

// Media files
#include "Snapshot.h"
#include "T64File.h"
#include "D64File.h"
#include "G64File.h"
#include "PRGFile.h"
#include "Folder.h"
#include "P00File.h"
#include "RomFile.h"
#include "TAPFile.h"
#include "CRTFile.h"
#include "FileSystem.h"

// Misc
#include "Host.h"
#include "RegressionTester.h"
#include "RemoteManager.h"
#include "RetroShell.h"
#include "RshServer.h"

namespace vc64 {

//
// Macros and constants
//

// Checks the category of an event slot
static constexpr bool isPrimarySlot(isize s) { return s <= SLOT_SEC; }
static constexpr bool isSecondarySlot(isize s) { return s > SLOT_SEC && s <= SLOT_TER; }
static constexpr bool isTertiarySlot(isize s) { return s > SLOT_TER; }

// Time stamp used for messages that never trigger
static constexpr Cycle NEVER = INT64_MAX;

// Inspection interval in seconds (interval between INS_xxx events)
static constexpr double inspectionInterval = 0.1;


class C64 final : public CoreComponent, public Inspectable<C64Info> {

    friend class Emulator;

    Descriptions descriptions = {
        {
            .type           = C64Class,
            .name           = "C64",
            .description    = "Commodore 64",
            .shell          = "c64"
        },
        {
            .type           = C64Class,
            .name           = "C64[run-ahead]",
            .description    = "Commodore 64",
            .shell          = ""
        }
    };

    Options options = {

        OPT_C64_WARP_BOOT,
        OPT_C64_WARP_MODE,
        OPT_C64_SPEED_BOOST,
        OPT_C64_VSYNC,
        OPT_C64_RUN_AHEAD,
        OPT_C64_SNAP_AUTO,
        OPT_C64_SNAP_DELAY,
        OPT_C64_SNAP_COMPRESS
    };
    
private:

    // The current configuration
    C64Config config = {};

    // Result of the latest inspection
    mutable EventSlotInfo slotInfo[SLOT_COUNT];


    //
    // Subcomponents
    //

public:

    // Host system information
    Host host = Host(*this);

    // Components
    Memory mem = Memory(*this);
    CPU cpu = CPU(MOS_6510, *this);
    CIA1 cia1 = CIA1(*this);
    CIA2 cia2 = CIA2(*this);
    VICII vic = VICII(*this);
    SIDBridge sidBridge = SIDBridge(*this);

    // Ports
    AudioPort audioPort = AudioPort(*this);
    VideoPort videoPort = VideoPort(*this);
    PowerPort supply = PowerPort(*this);
    ControlPort port1 = ControlPort(*this, PORT_1);
    ControlPort port2 = ControlPort(*this, PORT_2);
    ExpansionPort expansionport = ExpansionPort(*this);
    SerialPort iec = SerialPort(*this);
    UserPort userPort = UserPort(*this);

    // Peripherals
    Keyboard keyboard = Keyboard(*this);
    Drive drive8 = Drive(*this, 0);
    Drive drive9 = Drive(*this, 1);
    ParCable parCable = ParCable(*this);
    Datasette datasette = Datasette(*this);
    Monitor monitor = Monitor(*this);
    
    // Gateway to the GUI
    MsgQueue msgQueue = MsgQueue();

    // Misc
    RetroShell retroShell = RetroShell(*this);
    RemoteManager remoteManager = RemoteManager(*this);
    RegressionTester regressionTester = RegressionTester(*this);
    Recorder recorder = Recorder(*this);


    //
    // Event scheduler
    //

public:

    // Trigger cycle
    Cycle trigger[SLOT_COUNT] = { };

    // The event identifier
    EventID eventid[SLOT_COUNT] = { };

    // An optional data value
    i64 data[SLOT_COUNT] = { };

    // Next trigger cycle
    Cycle nextTrigger = NEVER;


    //
    // Emulator thread
    //

private:

    /* Run loop flags. This variable is checked at the end of each runloop
     * iteration. Most of the time, the variable is 0 which causes the runloop
     * to repeat. A value greater than 0 means that one or more runloop control
     * flags are set. These flags are flags processed and the loop either
     * repeats or terminates depending on the provided flags.
     */
    RunLoopFlags flags = 0;


    //
    // Storage
    //

private:

    typedef struct { Cycle trigger; i64 payload; } Alarm;
    std::vector<Alarm> alarms;


    //
    // State
    //

public:

    // The total number of frames drawn since power up
    u64 frame = 0;

    // The currently drawn scanline (first scanline = 0)
    u16 scanline = 0;

    // The currently executed scanline cycle (first cylce = 1)
    u8 rasterCycle = 1;

private:

    /* Indicates whether C64 is running in ultimax mode. Ultimax mode can be
     * enabled by external cartridges by pulling game line low and keeping
     * exrom line high. In ultimax mode, most of the C64's RAM and ROM is
     * invisible.
     */
    bool ultimax = false;

    /* Indicates if headless mode is activated. If yes, the pixel drawing code
     * is skipped. Headless mode is used to accelerate warp mode and to speed
     * up the computation of some frames in run-ahead mode.
     */
    bool headless = false;

    /* Indicates whether the state has been altered by an external event.
     * This flag is used to determine whether the run-ahead instance needs to
     * be recreated.
     */
    // bool isDirty = false;
    
    // Duration of a CPU cycle in 1/10 nano seconds
    i64 durationOfOneCycle;

    // Target address for step mode
    std::optional<u16> stepTo = { };


    //
    // Static methods
    //

public:

    // Returns a version string for this release
    static string version();

    // Returns a build number string for this release
    static string build();

    // Returns a textual description for an event
    static const char *eventName(EventSlot slot, EventID id);

    // Converts a time span to an (approximate) cycle count
    static Cycle usec(isize delay) { return Cycle(delay * 1LL); }
    static Cycle msec(isize delay) { return Cycle(delay * 1000LL); }
    static Cycle sec(double delay) { return Cycle(delay * 1000000LL); }


    //
    // Methods
    //

public:

    C64(class Emulator& ref, isize id);
    ~C64();

private:

    void initialize();


    //
    // Operators
    //

public:

    C64& operator= (const C64& other) {

        CLONE(mem)
        CLONE(cpu)
        CLONE(cia1)
        CLONE(cia2)
        CLONE(vic)
        CLONE(sidBridge)
        CLONE(audioPort)
        CLONE(videoPort)
        CLONE(supply)
        CLONE(port1)
        CLONE(port2)
        CLONE(expansionport)
        CLONE(iec)
        CLONE(userPort)
        CLONE(keyboard)
        CLONE(drive8)
        CLONE(drive9)
        CLONE(parCable)
        CLONE(datasette)
        CLONE(monitor)
        CLONE(retroShell)
        CLONE(regressionTester)
        CLONE(recorder)

        CLONE_ARRAY(trigger)
        CLONE_ARRAY(eventid)
        CLONE_ARRAY(data)
        CLONE(nextTrigger)
        CLONE(frame)
        CLONE(scanline)
        CLONE(rasterCycle)
        CLONE(ultimax)

        CLONE(durationOfOneCycle)

        CLONE(config)

        return *this;
    }


    //
    // Methods from Serializable
    //

public:

    template <class T>
    void serialize(T& worker)
    {
        if (isSoftResetter(worker)) return;

        worker

        << trigger
        << eventid
        << data
        << nextTrigger
        << frame
        << scanline
        << rasterCycle
        << ultimax;

        if (isResetter(worker)) return;

        worker

        << durationOfOneCycle

        << config.warpBoot
        << config.warpMode
        << config.vsync
        << config.speedBoost
        << config.snapshots
        << config.snapshotDelay
        << config.runAhead;
    }

    void operator << (SerResetter &worker) override;
    void operator << (SerChecker &worker) override { serialize(worker); }
    void operator << (SerCounter &worker) override { serialize(worker); }
    void operator << (SerReader &worker) override { serialize(worker); }
    void operator << (SerWriter &worker) override { serialize(worker); }


    //
    // Methods from CoreComponent
    //

public:

    const Descriptions &getDescriptions() const override { return descriptions; }
    void prefix(isize level, const char *component, isize line) const override;

private:

    void _dump(Category category, std::ostream& os) const override;

    void _didReset(bool hard) override;
    void _isReady() const throws override;
    void _powerOn() override;
    void _powerOff() override;
    void _run() override;
    void _pause() override;
    void _halt() override;
    void _warpOn() override;
    void _warpOff() override;
    void _trackOn() override;
    void _trackOff() override;


    //
    // Methods from Inspectable
    //

public:

    // virtual void record() const override;
    void cacheInfo(C64Info &result) const override;

    u64 getAutoInspectionMask() const;
    void setAutoInspectionMask(u64 mask);



    //
    // Methods from Configurable
    //

public:

    const C64Config &getConfig() const { return config; }
    const Options &getOptions() const override { return options; }
    
    i64 getOption(Option opt) const override;
    void checkOption(Option opt, i64 value) override;
    void setOption(Option opt, i64 value) override;
    
    // Exports the current configuration to a script file
    void exportConfig(const fs::path &path) const;
    void exportConfig(std::ostream& stream) const;


    //
    // Main API for configuring the emulator
    //

public:

    // Queries an option
    i64 get(Option opt, isize objid = 0) const throws;

    // Checks an option
    void check(Option opt, i64 value, const std::vector<isize> objids = { }) throws;

    // Sets an option
    void set(Option opt, i64 value, const std::vector<isize> objids = { }) throws;

    // Convenience wrappers
    void set(Option opt, const string &value, const std::vector<isize> objids = { }) throws;
    void set(const string &opt, const string &value, const std::vector<isize> objids = { }) throws;

    // Configures the emulator to match a specific C64 model
    void set(C64Model model);

public: // private

    // Returns the target component for an option
    Configurable *routeOption(Option opt, isize objid);
    const Configurable *routeOption(Option opt, isize objid) const;

    // Overrides a config option if the corresponding debug option is enabled
    i64 overrideOption(Option opt, i64 value) const;


    //
    // Analyzing
    //

public:


    // Ultimax mode
    bool getUltimax() const { return ultimax; }
    void setUltimax(bool b) { ultimax = b; }

    // Headless mode
    bool getHeadless() const { return headless; }
    void setHeadless(bool value) { headless = value; }

    // Returns the native refresh rate (differs between PAL and NTSC)
    double nativeRefreshRate() const;

    // Returns the native clock frequency (differs between PAL and NTSC)
    i64 nativeClockFrequency() const;

    // Returns the emulated refresh rate
    double refreshRate() const;

    // Returns the clock frequency based on the emulated refresh rate
    i64 clockFrequency() const;
    

    //
    // Emulating
    //

public:
    
    // Updates the clock frequency and all variables derived from it
    void updateClockFrequency();

private:

    // Called by the Emulator class in it's own update function
    void update(CmdQueue &queue);

    // Emulates a frame
    void computeFrame();
    void computeFrame(bool headless);
    void computeFrameHeadless() { computeFrame(true); }
    template <bool, bool, bool> void execute();
    template <bool, bool, bool> alwaysinline void executeCycle();
    void processFlags();

    // Fast-forward the run-ahead instance
    void fastForward(isize frames);


    //
    // Controlling the run loop
    //

public:

    /* Sets or clears a flag for controlling the run loop. The functions are
     * thread-safe and can be called safely from outside the emulator thread.
     */
    void setFlag(u32 flags);
    void clearFlag(u32 flags);

    // Convenience wrappers
    void signalBreakpoint() { setFlag(RL::BREAKPOINT); }
    void signalWatchpoint() { setFlag(RL::WATCHPOINT); }
    void signalJammed() { setFlag(RL::CPU_JAM); }
    void signalStop() { setFlag(RL::STOP); }

    // Executes a single clock cycle.
    void executeOneCycle();

private:

    // Invoked after executing the last cycle of a scanline
    void endScanline();

    // Invoked after executing the last scanline of a frame
    void endFrame();


    //
    // Managing commands and events
    //

public:

    // Processes a command from the command queue
    void processCommand(const Cmd &cmd);

    // Processes all pending events
    void processEvents(Cycle cycle);

    // Returns true iff the specified slot contains any event
    template<EventSlot s> bool hasEvent() const { return this->eventid[s] != (EventID)0; }

    // Returns true iff the specified slot contains a specific event
    template<EventSlot s> bool hasEvent(EventID id) const { return this->eventid[s] == id; }

    // Returns true iff the specified slot contains a pending event
    template<EventSlot s> bool isPending() const { return this->trigger[s] != NEVER; }

    // Returns true iff the specified slot contains a due event
    template<EventSlot s> bool isDue(Cycle cycle) const { return cycle >= this->trigger[s]; }

    // Schedules an event in certain ways
    template<EventSlot s> void scheduleAbs(Cycle cycle, EventID id)
    {
        this->trigger[s] = cycle;
        this->eventid[s] = id;

        if (cycle < nextTrigger) nextTrigger = cycle;

        if constexpr (isTertiarySlot(s)) {
            if (cycle < trigger[SLOT_TER]) trigger[SLOT_TER] = cycle;
        }
        if constexpr (isSecondarySlot(s) || isTertiarySlot(s)) {
            if (cycle < trigger[SLOT_SEC]) trigger[SLOT_SEC] = cycle;
        }
    }

    template<EventSlot s> void scheduleAbs(Cycle cycle, EventID id, i64 data)
    {
        scheduleAbs<s>(cycle, id);
        this->data[s] = data;
    }

    template<EventSlot s> void rescheduleAbs(Cycle cycle)
    {
        trigger[s] = cycle;
        if (cycle < nextTrigger) nextTrigger = cycle;

        if constexpr (isTertiarySlot(s)) {
            if (cycle < trigger[SLOT_TER]) trigger[SLOT_TER] = cycle;
        }
        if constexpr (isSecondarySlot(s) || isTertiarySlot(s)) {
            if (cycle < trigger[SLOT_SEC]) trigger[SLOT_SEC] = cycle;
        }
    }

    template<EventSlot s> void scheduleImm(EventID id)
    {
        scheduleAbs<s>(cpu.clock, id);
    }

    template<EventSlot s> void scheduleImm(EventID id, i64 data)
    {
        scheduleAbs<s>(cpu.clock, id);
        this->data[s] = data;
    }

    template<EventSlot s> void scheduleRel(Cycle cycle, EventID id) {
        scheduleAbs<s>(cpu.clock + cycle, id);
    }

    template<EventSlot s> void scheduleRel(Cycle cycle, EventID id, i64 data) {
        scheduleAbs<s>(cpu.clock + cycle, id, data);
    }

    template<EventSlot s> void rescheduleRel(Cycle cycle) {
        rescheduleAbs<s>(cpu.clock + cycle);
    }

    template<EventSlot s> void scheduleInc(Cycle cycle, EventID id)
    {
        scheduleAbs<s>(trigger[s] + cycle, id);
    }

    template<EventSlot s> void scheduleInc(Cycle cycle, EventID id, i64 data)
    {
        scheduleAbs<s>(trigger[s] + cycle, id);
        this->data[s] = data;
    }

    template<EventSlot s> void rescheduleInc(Cycle cycle)
    {
        rescheduleAbs<s>(trigger[s] + cycle);
    }

    template<EventSlot s> void cancel()
    {
        eventid[s] = (EventID)0;
        data[s] = 0;
        trigger[s] = NEVER;
    }

private:

    // Services an inspection event
    void processINSEvent();


    //
    // Handling snapshots
    //

public:

    // Takes a snapshot
    MediaFile *takeSnapshot();

    // Loads the current state from a snapshot file
    void loadSnapshot(const MediaFile &snapshot) throws;

private:

    // Services a snapshot event
    void processSNPEvent(EventID id);

    // Schedules the next snapshot event
    void scheduleNextSNPEvent();


    //
    // Handling Roms
    //

public:

    // Queries ROM information
    static RomTraits getRomTraits(u64 fnv);
    RomTraits getRomTraits(RomType type) const;

    // Computes a Rom checksum
    u32 romCRC32(RomType type) const;
    u64 romFNV64(RomType type) const;

    // Checks if a certain Rom is present
    bool hasRom(RomType type) const;
    bool hasMega65Rom(RomType type) const;

private:

    // Returns a revision string if a Mega65 Rom is installed
    const char *mega65BasicRev() const;
    const char *mega65KernalRev() const;

public:

    // Installs a Rom
    void loadRom(const fs::path &path) throws;
    void loadRom(const MediaFile &file);

    // Erases an installed Rom or all Roms
    void deleteRom(RomType type);
    void deleteRoms();

    // Saves a Rom to disk
    void saveRom(RomType rom, const fs::path &path) throws;

    // Installs an OpenROM or all three of them
    void installOpenRom(RomType type);
    void installOpenRoms();

    
    //
    // Flashing files
    //

    // Flashes a single file into memory
    void flash(const MediaFile &file) throws;
    void flash(const MediaFile &file, isize item) throws;
    void flash(const FileSystem &fs, isize item) throws;


    //
    // Handling alarms
    //

public:

    /* Alarms are scheduled notifications set by the client (GUI). Once the
     * trigger cycle of an alarm has been reached, the emulator sends a
     * MSG_ALARM to the client.
     */
    void setAlarmAbs(Cycle trigger, i64 payload);
    void setAlarmRel(Cycle trigger, i64 payload);

    // Services an alarm event
    void processAlarmEvent();

private:

    // Schedules the next alarm event
    void scheduleNextAlarm();


    //
    // Miscellaneous
    //

public:

    // Translates the current clock cycle into pseudo-random number
    u32 random();

    // Translates seed into a pseudo-random number
    u32 random(u32 seed);
};

}
