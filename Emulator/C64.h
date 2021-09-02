// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "C64Types.h"
#include "MsgQueue.h"
#include "SuspendableThread.h"

// Sub components
#include "ExpansionPort.h"
#include "IEC.h"
#include "Keyboard.h"
#include "ControlPort.h"
#include "C64Memory.h"
#include "DriveMemory.h"
#include "FlashRom.h"
#include "VICII.h"
#include "Muxer.h"
#include "TOD.h"
#include "CIA.h"
#include "CPU.h"
#include "PowerSupply.h"
#include "Recorder.h"
#include "RegressionTester.h"
#include "RetroShell.h"

// Cartridges
#include "Cartridge.h"
#include "CustomCartridges.h"

// Peripherals
#include "Drive.h"
#include "ParCable.h"
#include "Datasette.h"
#include "Mouse.h"

// Loading and saving
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
#include "FSDevice.h"


/* A complete virtual C64. This class is the most prominent one of all. To run
 * the emulator, it is sufficient to create a single object of this type. All
 * subcomponents are created automatically. The public API gives you control
 * over the emulator's behaviour such as running and pausing the emulation.
 * Please note that most subcomponents have their own public API. E.g., to
 * query information from VICII, you need to invoke a method on c64.vicii.
 */
class C64 : public SuspendableThread {
                
    // The component which is currently observed by the debugger
    InspectionTarget inspectionTarget;


    //
    // Sub components
    //
    
public:
    
    // Core components
    C64Memory mem = C64Memory(*this);
    C64CPU cpu = C64CPU(*this, mem);
    CIA1 cia1 = CIA1(*this);
    CIA2 cia2 = CIA2(*this);
    VICII vic = VICII(*this);
    Muxer muxer = Muxer(*this);

    // Logic board
    PowerSupply supply = PowerSupply(*this);
    ControlPort port1 = ControlPort(*this, PORT_ONE);
    ControlPort port2 = ControlPort(*this, PORT_TWO);
    ExpansionPort expansionport = ExpansionPort(*this);
    IEC iec = IEC(*this);
    
    // Peripherals
    Keyboard keyboard = Keyboard(*this);
    Drive drive8 = Drive(DRIVE8, *this);
    Drive drive9 = Drive(DRIVE9, *this);
    ParCable parCable = ParCable(*this);
    Datasette datasette = Datasette(*this);
    
    // Misc
    RetroShell retroShell = RetroShell(*this);
    RegressionTester regressionTester = RegressionTester(*this);
    Recorder recorder = Recorder(*this);
    MsgQueue msgQueue = MsgQueue(*this);

    
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
    // Snapshot storage
    //
    
private:
    
    Snapshot *autoSnapshot = nullptr;
    Snapshot *userSnapshot = nullptr;

    
    //
    // State
    //
    
public:
    
    // The total number of frames drawn since power up
    u64 frame = 0;
    
    /* The currently drawn scanline. The first scanline is numbered 0. The
     * number of the last scanline varies between PAL and NTSC models.
     */
    u16 scanline = 0;
    
    /* The currently executed scanline cycle. The first scanline cycle is
     * numbered 1. The number of the last cycle varies between PAL and NTSC
     * models.
     */
    u8 rasterCycle = 1;
        
private:
    
    /* Indicates whether C64 is running in ultimax mode. Ultimax mode can be
     * enabled by external cartridges by pulling game line low and keeping
     * exrom line high. In ultimax mode, most of the C64's RAM and ROM is
     * invisible.
     */
    bool ultimax = false;

    /* Duration of a CPU cycle in 1/10 nano seconds. The first value depends
     * on the selected VICII model and the selected speed setting. The second
     * value depends on the VICII model, only. Both values match if VICII is
     * run in speed mode "native".
     */
    i64 durationOfOneCycle;
    i64 nativeDurationOfOneCycle;
        
    
    //
    // Initializing
    //
    
public:
    
    C64();
    ~C64();
    
    const char *getDescription() const override { return "C64"; }
    void prefix() const override;

    void reset(bool hard);
    void hardReset() { reset(true); }
    void softReset() { reset(false); }

private:
    
    void _reset(bool hard) override;

    
    //
    // Configuring
    //

public:
    
    // Gets a single configuration item
    i64 getConfigItem(Option option) const;
    i64 getConfigItem(Option option, long id) const;
    
    // Sets a single configuration item
    void configure(Option option, i64 value) throws;
    void configure(Option option, long id, i64 value) throws;
    
    // Configures the C64 to match a specific C64 model
    void configure(C64Model model);
        
    // Powers off and resets the emulator to it's initial state
    void revertToFactorySettings();

    // Updates the clock frequency and all variables derived from it
    void updateClockFrequency(VICIIRevision rev, VICIISpeed speed);

private:
    
    // Overrides a config option if the corresponding debug option is enabled
    i64 overrideOption(Option option, i64 value);
    
    
    //
    // Analyzing
    //
    
public:
       
    void inspect();
    InspectionTarget getInspectionTarget() const;
    void setInspectionTarget(InspectionTarget target);
    void removeInspectionTarget() { setInspectionTarget(INSPECTION_NONE); }
        
private:
    
    void _dump(dump::Category category, std::ostream& os) const override;
    
    
    //
    // Serializing
    //
    
private:
    
    template <class T>
    void applyToPersistentItems(T& worker)
    {
        worker
        
        << durationOfOneCycle
        << nativeDurationOfOneCycle;
    }
    
    template <class T>
    void applyToResetItems(T& worker, bool hard = true)
    {
        if (hard) {
            
            worker
            
            << frame
            << scanline
            << rasterCycle
            << ultimax;
        }
    }
    
    isize _size() override { COMPUTE_SNAPSHOT_SIZE }
    isize _load(const u8 *buffer) override { LOAD_SNAPSHOT_ITEMS }
    isize _save(u8 *buffer) override { SAVE_SNAPSHOT_ITEMS }

    
    //
    // Controlling
    //
        
private:

    void _isReady() const throws override;
    void _powerOn() override;
    void _powerOff() override;
    void _run() override;
    void _pause() override;
    void _halt() override;
    void _warpOn() override;
    void _warpOff() override;
    void _debugOn() override;
    void _debugOff() override;

    
    //
    // Running the emulator
    //
    
private:
    
    // Main execution method (from Thread class)
    void execute() override;

public:

    bool getUltimax() const { return ultimax; }
    void setUltimax(bool b) { ultimax = b; }

    /* Sets or clears a flag for controlling the run loop. The functions are
     * thread-safe and can be called safely from outside the emulator thread.
     */
    void setFlag(u32 flags);
    void clearFlag(u32 flags);
    
    // Convenience wrappers
    void signalAutoSnapshot() { setFlag(RL::AUTO_SNAPSHOT); }
    void signalUserSnapshot() { setFlag(RL::USER_SNAPSHOT); }
    void signalBreakpoint() { setFlag(RL::BREAKPOINT); }
    void signalWatchpoint() { setFlag(RL::WATCHPOINT); }
    void signalInspect() { setFlag(RL::INSPECT); }
    void signalJammed() { setFlag(RL::CPU_JAM); }
    void signalStop() { setFlag(RL::STOP); }
    void signalExpPortNmi() { setFlag(RL::EXTERNAL_NMI); }

    // Runs or pauses the emulator
    void stopAndGo();

    /* Executes a single instruction. This function is used for single-stepping
     * through the code inside the debugger. It starts the execution thread and
     * terminates it after the next instruction has been executed.
     */
    void stepInto();
    
    /* Emulates the C64 until the instruction following the current one is
     * reached. This function is used for single-stepping through the code
     * inside the debugger. It sets a soft breakpoint to PC+n where n is the
     * length bytes of the current instruction and starts the emulator thread.
     */
    void stepOver();
    
    /* Emulates the C64 until the end of the current frame. Under certain
     * circumstances the function may terminate earlier, in the middle of a
     * frame. This happens, e.g., if the CPU jams or a breakpoint is reached.
     * It is save to call the function in the middle of a frame. In this case,
     * the C64 is emulated until the curent frame has been completed.
     */
    void executeOneFrame();
    
    /* Emulates the C64 until the end of the current scanline. This function
     * is called inside executeOneFrame().
     */
    void executeOneLine();
    
    // Executes a single clock cycle
    void executeOneCycle();
    void _executeOneCycle();

    /* Finishes the current instruction. This function is called when the
     * emulator threads terminates in order to reach a clean state. It emulates
     * the CPU until the next fetch cycle is reached.
     */
    void finishInstruction();
    
    // Finishes the current frame
    void finishFrame();
    
private:
    
    // Invoked before executing the first cycle of a scanline
    void beginScanline();
    
    // Invoked after executing the last cycle of a scanline
    void endScanline();
    
    // Invoked after executing the last scanline of a frame
    void endFrame();
    
    
    //
    // Handling snapshots
    //
    
public:
    
    /* Requests a snapshot to be taken. Once the snapshot is ready, a message
     * is written into the message queue. The snapshot can then be picked up by
     * calling latestAutoSnapshot() or latestUserSnapshot(), depending on the
     * requested snapshot type.
     */
    void requestAutoSnapshot();
    void requestUserSnapshot();

    // Returns the most recent snapshot or nullptr if none was taken
    Snapshot *latestAutoSnapshot();
    Snapshot *latestUserSnapshot();
    
    // Loads the current state from a snapshot file
    void loadSnapshot(const Snapshot &snapshot) throws;
    
    
    //
    // Handling Roms
    //
    
public:
    
    // Computes a Rom checksum
    u32 romCRC32(RomType type) const;
    u64 romFNV64(RomType type) const;
     
    // Returns a unique identifier for the installed ROMs
    RomIdentifier romIdentifier(RomType type) const;
    
    // Returns printable titles for the installed ROMs
    const string romTitle(RomType type) const;
    
    // Returns printable sub titles for the installed ROMs
    const string romSubTitle(u64 fnv) const;
    const string romSubTitle(RomType type) const;
    
    // Returns printable revision strings or hash values for the installed ROMs
    const string romRevision(RomType type) const;
    
    // Checks if a certain Rom is present
    bool hasRom(RomType type) const;
    bool hasMega65Rom(RomType type) const;

private:
    
    // Returns a revision string if a Mega65 Rom is installed
    const char *mega65BasicRev() const;
    const char *mega65KernalRev() const;

public:
    
    // Installs a Rom
    void loadRom(const string &path) throws;
    void loadRom(const RomFile &file);
    
    // Erases an installed Rom
    void deleteRom(RomType type);
    
    // Saves a Rom to disk
    void saveRom(RomType rom, const string &path) throws;

    
    //
    // Flashing files
    //
    
    // Flashes a single file into memory
    void flash(const AnyFile &file) throws;
    void flash(const AnyCollection &file, isize item) throws;
    void flash(const FSDevice &fs, isize item) throws;
};
