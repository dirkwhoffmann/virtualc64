/*!
 * @header      C64.h
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   Dirk W. Hoffmann. All rights reserved.
 */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _C64_INC
#define _C64_INC

// Configuration items
#include "Configure.h"

// Data types and constants
#include "C64_types.h"

// General
#include "MessageQueue.h"

// Loading and saving
#include "Snapshot.h"
#include "T64File.h"
#include "D64File.h"
#include "G64File.h"
#include "PRGFile.h"
#include "P00File.h"
#include "ROMFile.h"
#include "TAPFile.h"
#include "CRTFile.h"

// Sub components
#include "ProcessorPort.h"
#include "ExpansionPort.h"
#include "IEC.h"
#include "Keyboard.h"
#include "ControlPort.h"
#include "Memory.h"
#include "C64Memory.h"
#include "FlashRom.h"
#include "DriveMemory.h"
#include "VIC.h"
#include "SIDBridge.h"
#include "TOD.h"
#include "CIA.h"
#include "CPU.h"

// Cartridges
#include "Cartridge.h"
#include "CustomCartridges.h"

// Peripherals
#include "Drive.h"
#include "Datasette.h"
#include "Mouse1350.h"
#include "Mouse1351.h"
#include "NeosMouse.h"

/*! @class    A complete virtual Commodore 64
 *  @brief    This class is the most prominent one of all. To run the emulator,
 *            it is sufficient to create a single object of this type. All
 *            sub-components are created automatically. The public API gives
 *            you control over the emulator's behaviour such as running and
 *            pausing the emulation. Please note that most sub-components
 *            have their own public API. E.g., to query information from VICII,
 *            you need to invoke a public method on c64->vic.
 */
class C64 : public VirtualComponent {
    
    public:
    
    //
    // Hardware components
    //
    
    //! @brief    The C64's virtual memory (ROM, RAM, and color RAM)
    C64Memory mem;
    
    //! @brief    The C64's virtual CPU
    CPU cpu = CPU(MOS_6510, &mem);
    
    //! @brief    The C64's processor port
    ProcessorPort processorPort;
    
    //! @brief    The C64's Video Interface Controller
    VIC vic;
    
    //! @brief    The C64's first Complex Interface Adapter
    CIA1 cia1;
    
    //! @brief    The C64's second Complex Interface Adapter
    CIA2 cia2;
    
    //! @brief    The C64's Sound Interface Device
    SIDBridge sid;
    
    //! @brief    The C64's virtual keyboard
    Keyboard keyboard;
    
    //! @brief    The C64's first control port
    ControlPort port1 = ControlPort(1);
    
    //! @brief    The C64's second control port
    ControlPort port2 = ControlPort(2);
    
    //! @brief    The C64's expansion port (cartdrige slot)
    ExpansionPort expansionport;
    
    //! @brief    The C64's serial bus connecting the VC1541 floppy drives
    IEC iec;

    //! @brief    A VC1541 floppy drive (with device number 8)
    VC1541 drive1 = VC1541(1);
    
    //! @brief    A second VC1541 floppy drive (with device number 9)
    VC1541 drive2 = VC1541(2);
    
    //! @brief    A Commodore 1530 (C2N) Datasette
    Datasette datasette;
    
    //! @brief    A Commodore Mouse 1350
    Mouse1350 mouse1350;
    
    //! @brief    A Commodore Mouse 1351
    Mouse1351 mouse1351;
    
    //! @brief    A Neos Mouse
    NeosMouse neosMouse;
    
    
    //
    // Mouse
    //
    
    //! @brief    Selected mouse model
    //! @details  Points to mouse130, mouse1351, or neosMouse
    Mouse *mouse;
    
    //! @brief    Control port of plugged in mouse
    /*! @details  0 = unconnected, 1,2 = connected to port 1 or 2
     */
    unsigned mousePort;
    
    
    //
    // Frame, rasterline, and rasterline cycle information
    //
    
    //! @brief    The total number of frames drawn since power up
    uint64_t frame;
    
    //! @brief    The currently drawn rasterline
    /*! @details  The first rasterline is numbered 0. The number of rasterlines
     *            drawn in a single frame depends on the selected VICII model.
     *  @see      VIC::getRasterlinesPerFrame()
     */
    uint16_t rasterLine;
    
    /*! @brief    The currently executed rasterline cycle
     *  @details  The first rasterline cycle is numbered 1. The number of cycles
     *            executed in a single rasterline depends on the selected
     *            VICII model.
     *  @see      VIC::getCyclesPerRasterline()
     */
    uint8_t rasterCycle;

    /*! @brief    Current CPU frequency
     *  @details  This value is set in setClockFrequency()
     */
    uint32_t frequency;

    /*! @brief    Duration of a CPU cycle in 1/10 nano seconds
     *  @details  This value is set in setClockFrequency()
     */
    uint64_t durationOfOneCycle;
    
    //! @brief    VICII function table.
    /*! @details  Stores a pointer to the VICII method that is executed
     *            in a certain rasterline cycle.
     *  @note     vicfunc[0] is a stub. It is never called, because the first
     *            rasterline cycle is numbered 1.
     */
    void (VIC::*vicfunc[66])(void);
    
    
    //
    // Execution thread
    //
    
    //! @brief    An invocation counter for implementing suspend() / resume()
    unsigned suspendCounter = 0;
    
    /*! @brief    The emulators execution thread
     *  @details  The thread is created when the emulator is started and
     *            destroyed when the emulator is halted.
     */
    pthread_t p;
    
    private:
    
    /*! @brief    System timer information
     *  @details  Used to put the emulation thread to sleep for the proper
     *            amount of time.
     */
    mach_timebase_info_data_t timebase;
    
    /*! @brief    Wake-up time of the synchronization timer in nanoseconds
     *  @details  This value is recomputed each time the emulator thread is
     *            put to sleep.
     */
    uint64_t nanoTargetTime;
    
    /*! @brief    Indicates if c64 is currently running at maximum speed
     *            (with timing synchronization disabled)
     */
    bool warp;
    
    //! @brief    Indicates that we should always run as possible.
    bool alwaysWarp;
    
    /*! @brief    Indicates that we should run as fast as possible at least
     *            during disk operations.
     */
    bool warpLoad;
    
    
    //
    // Operation modes
    //
    
    /*! @brief    Indicates whether C64 is running in ultimax mode.
     *  @details  Ultimax mode can be enabled by external cartridges by pulling
     *            game line low and keeping exrom line high. In ultimax mode,
     *            most of the C64's RAM and ROM is invisible. This flag is
     *            frequently read by VIC to determine the correct peek source.
     */
    bool ultimax;
    
    
    //
    // Message queue
    //
    
    /*! @brief    Message queue.
     *  @details  Used to communicate with the graphical user interface. The
     *            GUI registers a listener and a callback function to retrieve
     *            messages.
     */
    MessageQueue queue;
    
    
    //
    // Snapshot storage
    //
    
    public:
    
    //! @brief    Time in seconds between two auto-saved snapshots
    /*! @note     Setting this variable to 0 or less disables auto-snapshots.
     *            Using the sign to code the enabled / disabled status has
     *            the advantage that we can preserve the value that was chosen
     *            before disabling. Hence, when the feature is reenabled, we
     *            can restore the old value by simply turning the sign.
     */
    long snapshotInterval;
    
    private:
    
    //! @brief    Maximum number of stored snapshots
    static const size_t MAX_SNAPSHOTS = 32;
    
    //! @brief    Storage for auto-taken snapshots
    vector<Snapshot *> autoSnapshots;
    
    //! @brief    Storage for user-taken snapshots
    vector<Snapshot *> userSnapshots;
    
    
    //
    //! @functiongroup Constructing and destructing
    //
    
    public:
    
    //! @brief    Standard constructor
    C64();
    
    //! @brief    Stabdard destructor
    ~C64();
    
    //
    //! @functiongroup Methods from VirtualComponent
    //
    
    void reset();
    void ping();
    void setClockFrequency(uint32_t frequency);
    void suspend();
    void resume();
    void dump();
    
 
    //
    //! @functiongroup Configuring the emulator
    //
    
    /*! @brief    Returns the emulated C64 model
     *  @return   C64_CUSTOM, if the selected sub-components do not match any
     *            of the supported C64 models.
     */
    C64Model getModel();
    
    /*! @brief    Sets the currently emulated C64 model
     *  @param    model is any C64Model other than C64_CUSTOM.
     *  @note     It it safe to call this function on a running emulator.
     */
    void setModel(C64Model m);
    
    //! @brief    Updates the VIC function table
    /*! @details  This function is invoked by VIC::setModel(), only.
     */
    void updateVicFunctionTable();
    
    
    //
    //! @functiongroup Accessing the message queue
    //
    
    //! @brief    Registers a listener callback function
    void addListener(const void *sender, void(*func)(const void *, int, long) ) {
        queue.addListener(sender, func);
    }
    
    //! @brief    Removes a listener callback function
    void removeListener(const void *sender) {
        queue.removeListener(sender);
    }
    
    //! @brief    Gets a notification message from message queue
    Message getMessage() { return queue.getMessage(); }
    
    //! @brief    Feeds a notification message into message queue
    void putMessage(MessageType msg, uint64_t data = 0) { queue.putMessage(msg, data); }
    
    
    //
    //! @functiongroup Running the emulator
    //
    
    /*! @brief    Cold starts the virtual C64.
     *  @details  The emulator and all of its sub components are reset and
     *            the execution thread is started.
     *  @note     It it safe to call this function on a running emulator.
     */
    void powerUp();
    
    /*! @brief    Starts the execution thread.
     *  @details  This method launches the execution thread and is usually
     *            called after emulation was stopped by a call to halt() or by
     *            reaching a breakpoint. Calling this functions has no effect,
     *            if the emulator is currently running.
     */
    void run();
    
    /*! @brief    Stops the emulation execution thread.
     *  @details  The execution thread is canceled, but the internal state
     *            remains intact. Emulation can be continued by a call to run().
     *            Calling this functions has no effect, if the emulator is
     *            not running.
     */
    void halt();
        
    /*! @brief    The tread exit function.
     *  @details  This method is invoked automatically when the emulator thread
     *            terminates.
     */
    void threadCleanup();
    
    //! @brief    Returns true iff the virtual C64 is able to run.
    /*! @details  The emulator needs all four Roms to run. Hence, this method
     *            returns true if and only if all four Roms are installed.
     *  @see      loadRom()
     */
    bool isRunnable();
    
    //! @brief    Returns true if the emulator is running.
    bool isRunning();
    
    //! @brief    Returns true if the emulator is not running.
    bool isHalted();
    
    /*! @brief    Executes a single instruction.
     *  @details  This method implements the debugger's 'step' action.
     */
    void step();
    
    /*! @brief    Executes until the instruction is reached
     *  @details  This method implements the debugger's 'step over' action.
     */
    void stepOver();
    
    /*! @brief    Executes until the end of the current rasterline is reached.
     *  @details  This method can be called even if a certain portion of the
     *            current rasterline has already been processed.
     */
    bool executeOneLine();
    
    /*! @brief    Executes until the end of the current frame is reached.
     *  @details  This method can be called even if a certain portion of the
     *            current frame has already been processed.
     */
    bool executeOneFrame();
    
    private:
    
    //! @brief    Executes a single CPU cycle
    bool executeOneCycle();
    
    //! @brief    Work horse for executeOneCycle()
    bool _executeOneCycle();
    
    //! @brief    Invoked before executing the first cycle of a rasterline
    void beginRasterLine();
    
    //! @brief    Invoked after executing the last cycle of a rasterline
    void endRasterLine();
    
    //! @brief    Invoked after executing the last rasterline of a frame
    void endFrame();
    
    
    //
    //! @functiongroup Handling mice
    //
    
    public:
    
    //! @brief    Returns the hardware model of the selected mouse.
    MouseModel getMouseModel() { return mouse->mouseModel(); }
    
    //! @brief    Sets the mouse hardware model.
    void setMouseModel(MouseModel value);
    
    //! @brief    Connects the selected mouse with the control port.
    void connectMouse(unsigned port);
    
    //! @brief    Reads the control port mouse bits.
    uint8_t mouseBits(unsigned port);
    
    //! @brief    Returns the potX bits as they show up in the SID register.
    uint8_t potXBits();
    
    //! @brief    Returns the potY bits as they show up in the SID register.
    uint8_t potYBits();
    
    
    //
    //! @functiongroup Managing the execution thread
    //
    
    private:
    
    //! @brief    Converts kernel time to nanoseconds.
    uint64_t abs_to_nanos(uint64_t abs) { return abs * timebase.numer / timebase.denom; }
    
    //! @brief    Converts nanoseconds to kernel time.
    uint64_t nanos_to_abs(uint64_t nanos) { return nanos * timebase.denom / timebase.numer; }
    
    public:
    
    //! @brief    Updates variable warp and returns the new value.
    /*! @details  As a side effect, messages are sent to the GUI if the
     *            variable has changed its value.
     */
    bool getWarp();
    
    //! @brief    Returns if the emulator should always run full speed.
    bool getAlwaysWarp() { return alwaysWarp; }
    
    //! @brief    Setter for alwaysWarp
    void setAlwaysWarp(bool b);
    
    //! @brief    Returns if warp mode should be activated during disk access.
    bool getWarpLoad() { return warpLoad; }
    
    //! @brief    Setter for warpLoad
    void setWarpLoad(bool b);
    
    /*! @brief    Restarts the synchronization timer.
     *  @details  The function is invoked at launch time to initialize the timer
     *            and reinvoked when the synchronization timer gets out of sync.
     */
    void restartTimer();
    
    private:
    
    /*! @brief    Puts the emulation the thread to sleep for a while.
     *  @details  This function is called inside endFrame(). It makes the
     *            emulation thread wait until nanoTargetTime has been reached.
     *            Before returning, nanoTargetTime is assigned with a new target
     *            value.
     */
    void synchronizeTiming();
    
 
    //
    //! @functiongroup Handling snapshots
    //
    
    public:
    
    //! @brief    Disables the auto-snapshot feature.
    void disableAutoSnapshots() { if (snapshotInterval > 0) snapshotInterval *= -1; }
    
    //! @brief    Enables the auto-snapshot feature.
    void enableAutoSnapshots() { if (snapshotInterval < 0) snapshotInterval *= -1; }
    
    /*! @brief    Disables the auto-snapshot feature temporarily.
     *  @details  This method is called when the snaphshot browser opens.
     */
    void suspendAutoSnapshots() { snapshotInterval -= (LONG_MAX / 2); }
    
    /*! @brief    Heal a call to suspendAutoSnapshots()
     *  @details  This method is called when the snaphshot browser closes.
     */
    void resumeAutoSnapshots() { snapshotInterval += (LONG_MAX / 2); }
    
    //! @brief    Returns the time between two auto-snapshots in seconds.
    long getSnapshotInterval() { return snapshotInterval; }
    
    //! @brief    Sets the time between two auto-snapshots in seconds.
    void setSnapshotInterval(long value) { snapshotInterval = value; }
    
    /*! @brief    Loads the current state from a snapshot file
     *  @note     There is an thread-unsafe and thread-safe version of this
     *            function. The first one can be unsed inside the emulator
     *            thread or from outside if the emulator is halted. The second
     *            one can be called any time.
     */
    void loadFromSnapshotUnsafe(Snapshot *snapshot);
    void loadFromSnapshotSafe(Snapshot *snapshot);
    
    //! @brief    Restores a certain snapshot from the snapshot storage
    bool restoreSnapshot(vector<Snapshot *> &storage, unsigned nr);
    bool restoreAutoSnapshot(unsigned nr) { return restoreSnapshot(autoSnapshots, nr); }
    bool restoreUserSnapshot(unsigned nr) { return restoreSnapshot(userSnapshots, nr); }

    //! @brief    Restores the latest snapshot from the snapshot storage
    bool restoreLatestAutoSnapshot() { return restoreAutoSnapshot(0); }
    bool restoreLatestUserSnapshot() { return restoreUserSnapshot(0); }
    
    //! @brief    Returns the number of stored snapshots
    size_t numSnapshots(vector<Snapshot *> &storage);
    size_t numAutoSnapshots() { return numSnapshots(autoSnapshots); }
    size_t numUserSnapshots() { return numSnapshots(userSnapshots); }
    
    //! @brief    Returns an snapshot from the snapshot storage
    Snapshot *getSnapshot(vector<Snapshot *> &storage, unsigned nr);
    Snapshot *autoSnapshot(unsigned nr) { return getSnapshot(autoSnapshots, nr); }
    Snapshot *userSnapshot(unsigned nr) { return getSnapshot(userSnapshots, nr); }
    
    /*! @brief    Takes a snapshot and inserts it into the snapshot storage
     *  @details  The new snapshot is inserted at position 0 and all others are
     *            moved one position up. If the buffer is full, the oldest
     *            snapshot is deleted.
     *  @note     Make sure to call the 'Safe' version outside the emulator
     *            thread.
     */
    void takeSnapshot(vector<Snapshot *> &storage);
    void takeAutoSnapshot() { takeSnapshot(autoSnapshots); }
    void takeUserSnapshot() { takeSnapshot(userSnapshots); }
    void takeAutoSnapshotSafe() { suspend(); takeSnapshot(autoSnapshots); resume(); }
    void takeUserSnapshotSafe() { suspend(); takeSnapshot(userSnapshots); resume(); }
    
    /*! @brief    Deletes a snapshot from the snapshot storage
     *  @details  All remaining snapshots are moved one position down.
     */
    void deleteSnapshot(vector<Snapshot *> &storage, unsigned nr);
    void deleteAutoSnapshot(unsigned nr) { deleteSnapshot(autoSnapshots, nr); }
    void deleteUserSnapshot(unsigned nr) { deleteSnapshot(userSnapshots, nr); }
    

    //
    //! @functiongroup Handling Roms
    //
    
    //! @brief    Loads a ROM image into memory
    bool loadRom(const char *filename);

    
    //
    //! @functiongroup Flashing files
    //
    
    //! @brief    Flashes a single file into memory
    bool flash(AnyC64File *file);

    //! @brief    Flashes a single item of an archive into memory
    bool flash(AnyArchive *file, unsigned item);
    
 
    //
    //! @functiongroup Set and query ultimax mode
    //
    
    public:
    
    //! @brief    Returns the ultimax flag
    bool getUltimax() { return ultimax; }
    
    /*! @brief    Setter for ultimax
     *  @details  This method is called in function updatePeekPokeLookupTables()
     *            if a certain game / exrom line combination is provided.
     */
    void setUltimax(bool b) { ultimax = b; }
    
    
    //
    //! @functiongroup Debugging
    //
    
    /*! @brief    Returns true if the executable was compiled for development
     *  @details  In release mode, assertion checking should be switched off
     */
    inline bool developmentMode() {
#ifndef NDEBUG
        return true;
#endif
        return false;
    }
};

#endif

