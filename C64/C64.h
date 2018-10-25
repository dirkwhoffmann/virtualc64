/*!
 * @header      C64.h
 * @brief       This file is part of VirtualC64.
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   2006 - 2018 Dirk W. Hoffmann
 */
/*              This program is free software; you can redistribute it and/or modify
 *              it under the terms of the GNU General Public License as published by
 *              the Free Software Foundation; either version 2 of the License, or
 *              (at your option) any later version.
 *
 *              This program is distributed in the hope that it will be useful,
 *              but WITHOUT ANY WARRANTY; without even the implied warranty of
 *              MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *              GNU General Public License for more details.
 *
 *              You should have received a copy of the GNU General Public License
 *              along with this program; if not, write to the Free Software
 *              Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
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
#include "VC1541Memory.h"
#include "VIC.h"
// #include "PixelEngine.h"
#include "SIDBridge.h"
#include "TOD.h"
#include "CIA.h"
#include "CPU.h"

// Cartridges
#include "CustomCartridges.h"

// Peripherals
#include "VC1541.h"
#include "Datasette.h"
#include "Mouse1350.h"
#include "Mouse1351.h"
#include "NeosMouse.h"

//! @class    A complete virtual C64
class C64 : public VirtualComponent {

public:
    
    //
    // Sub components
    //
    
    //! @brief    The C64s virtual memory (ROM, RAM, and color RAM)
    C64Memory mem;
    
    //! @brief    The C64s virtual CPU
    CPU cpu;
    
    //! @brief    The C64s processor port
    ProcessorPort processorPort;

    //! @brief    The C64s video controller chip
    VIC vic;
    
    //! @brief    The C64s first versatile interface adapter
    CIA1 cia1;
    
    //! @brief    The C64s second versatile interface adapter
    CIA2 cia2;

    //! @brief    Sound chip
    SIDBridge sid;
    
    //! @brief    Virtual keyboard
    Keyboard keyboard;
    
    //! @brief    Control port 1
    ControlPort port1 = ControlPort(1);

    //! @brief    Control port 2
    ControlPort port2 = ControlPort(2);

    //! @brief    Bus interface for connecting the VC1541 drive
    IEC iec;

    //! @brief    Expansion port (cartdrige slot)
    ExpansionPort expansionport;

    //! @brief    First Commodore VC1541 floppy drive
    VC1541 drive1 = VC1541(1);

    //! @brief    Second Commodore VC1541 floppy drive
    VC1541 drive2 = VC1541(2);

    //! @brief    Commodore 1530 (C2N) Datasette
    Datasette datasette;

    //! @brief    Commodore Mouse 1350
    Mouse1350 mouse1350;

    //! @brief    Commodore Mouse 1351
    Mouse1351 mouse1351;

    //! @brief    Neos Mouse
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
    // Frame, rasterline, and rasterline cycle
    //
        
    //! @brief    Total number of frames drawn since power up
    uint64_t frame;
    
    //! @brief    Currently drawn rasterline
    uint16_t rasterLine;
    
    /*! @brief    Currently executed cycle within rasterline
     *  @details  Range: 1 ... 63 on PAL machines, 1 ... 65 on NTSC machines
     */
    uint8_t rasterCycle;

    //! @brief    Duration of a CPU cycle in 1/10 nano second
    uint64_t durationOfCycle;
    
    //! @brief    VIC execution function table.
    /*! @details  Stores a pointer to the VIC method that needs to be executed
     *            in the current rasterline cycle.
     */
    void (VIC::*vicfunc[66])(void);

    
    //
    // Execution thread
    //
    
    //! @brief    A mutex for implementing the suspend / resume mechanism
    // pthread_mutex_t mutex;

    //! @brief    An invocation counter for implementing the suspend / resume mechanism
    unsigned suspendCounter = 0;
    
    //! @brief    The emulators execution thread
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
     *            gameLine low and keeping exromLine high. In ultimax mode,
     *            most of the C64's RAM and ROM is invisible. This flag is
     *            frequently read by VIC to determine the correct peek source.
     */
    bool ultimax;
    
    
    //
    // Message queue
    //
    
    /*! @brief    Message queue.
     *  @details  Used to communicate with the graphical user interface.
     */
    MessageQueue queue;
    

    //
    // Snapshot storage
    //
    
public:
    
    //! @brief    Time in seconds between two auto-saved snapshots
    /*! @note     Setting this variable to 0 or less disables auto-snapshots.
     */
    long snapshotInterval;
    
private:
    
    //! @brief    Maximum number of stored snapshots
    static const size_t MAX_SNAPSHOTS = 32;

    //! @brief    Storage for auto-taken snapshots
    vector<Snapshot *> autoSnapshots;
        
    //! @brief    Storage for user-taken snapshots
    vector<Snapshot *> userSnapshots;

    
public:
    
    //! @brief    Constructor
    C64();
    
    //! @brief    Destructor
    ~C64();

    //! @brief    Method from VirtualComponent
    void reset();
     
    //! @brief    Method from VirtualComponent
    void ping();

    //! @brief    Method from VirtualComponent
    void setClockFrequency(uint32_t frequency);
    
    //! @brief    Method from VirtualComponent
    void dumpState();
    
    //! @brief    Returns true if the executable was compiled for development
    /*! @details  In release mode, assertion checking should be switched off
     */
    bool developmentMode() {
    #ifndef NDEBUG
        return true;
    #endif
        return false;
    }
            
    //
    //! @functiongroup Configuring the emulator
    //

    /*! @brief    Returns the currently emulated C64 model
     *  @return   C64 model or 0, if a custom configurarion is used that does
     *            not match any real C64 models.
     */
    C64Model getModel();

    //! @brief    Sets the currently emulated C64 model
    void setModel(C64Model model);

    //! @brief    Updates the VIC function table
    /*! @details  This function is called inside VIC::setChipModel().
     */
    void updateVicFunctionTable();
    
     
    //
    //! @functiongroup Handling mice
    //
    
    //! @brief    Returns the mouse hardware model
    MouseModel getMouseModel() { return mouse->mouseModel(); }
    
    //! @brief    Sets the mouse hardware model
    void setMouseModel(MouseModel value);

    //! @brief    Connect mouse to control port
    void connectMouse(unsigned port);
    
    //! @brief    Read the control port mouse bits
    uint8_t mouseBits(unsigned port);
    
    //! @brief    Returns the potX bits as they show up in the SID register
    uint8_t potXBits();

    //! @brief    Returns the potY bits as they show up in the SID register
    uint8_t potYBits();

    
    //
    //! @functiongroup Running the emulator
    //

    //! @brief    Cold starts the virtual C64.
    /*! @details  The emulator and all of its sub components are reset and
     *            the execution thread is started.
     */
    void powerUp();
    
    //! @brief    Starts the execution thread.
    /*! @details  This method launches the execution thread and is usually
     *            called after emulation was stopped by a call to halt() or by
     *            reaching a breakpoint.
     */
    void run();
    
    /*! @brief    Stops the emulation execution thread.
     *  @details  The execution thread is canceled, but the internal state
     *            remains intact. Emulation can be continued by a call to run().
     */
    void halt();
    
    /*! @brief    Freezes the emulation thread.
     *  @details  If the internal state of the emulator is changed from outside
     *            the emulation thread, the change must be embedded in a
     *            suspend / resume block as follows:
     *
     *            suspend();
     *            do something with the internal state;
     *            resume();
     *
     *  @note     Multiple suspend / resume blocks can be nested.
     *  @see      resume
     */
    void suspend();
    
    /*! @brief    Continues the emulation thread.
     *  @details  This functions concludes a suspend operation.
     *  @see      suspend
     */
    void resume();
    
    /*! @brief    The tread exit function.
     *  @details  This method is invoked automatically when the emulator thread
     *            terminates.
     */
    void threadCleanup();

    //! @brief    Returns true iff the virtual C64 is able to run.
    /*! @details  The emulator can run when all ROMs are loaded.
     */
    bool isRunnable();
    
    //! @brief    Returns true iff the virtual C64 is in the "running" state
    bool isRunning();
    
    //! @brief    Returns true iff the virtual C64 is in the "halted" state
    bool isHalted();
    
    /*! @brief    Executes a single instruction
     *  @details  This method implements the "step" action of the debugger
     */
    void step(); 

    /*! @brief    Executes until the instruction is reached
     *  @details  This method implements the "step over" action of the debugger
     */
    void stepOver();
    
    //! @brief    Executes until the end of the rasterline
    bool executeOneLine();

    //! @brief    Executes until the end of the frame
    bool executeOneFrame();

private:
    
    //! @brief    Executes virtual C64 for one cycle
    bool executeOneCycle();

    //! @brief    Work horse for executeOneCycle()
    bool _executeOneCycle();

    //! @brief    Invoked before executing the first cycle of rasterline
    void beginRasterLine();
    
    //! @brief    Invoked after executing the last cycle of rasterline
    void endRasterLine();

    //! @brief    Invoked after executing the last rasterline of a frame
    void endFrame();

    
    //
    //! @functiongroup Managing the execution thread
    //
    
    //! @brief    Converts kernel time to nanoseconds
    uint64_t abs_to_nanos(uint64_t abs) { return abs * timebase.numer / timebase.denom; }
    
    //! @brief    Converts nanoseconds to kernel time
    uint64_t nanos_to_abs(uint64_t nanos) { return nanos * timebase.denom / timebase.numer; }
    
public:
    
    //! @brief    Updates variable warp and returns the new value
    /*! @details  As a side effect, messages are sent to the GUI if the variable
     *            has changed its value.
     */
    bool getWarp();
    
    //! @brief    Returns true iff cpu should always run at maximun speed.
    bool getAlwaysWarp() { return alwaysWarp; }
    
    //! @brief    Setter for alwaysWarp.
    void setAlwaysWarp(bool b);
    
    //! @brief    Returns true iff warp mode is activated during disk operations.
    bool getWarpLoad() { return warpLoad; }
    
    //! @brief    Setter for warpLoad.
    void setWarpLoad(bool b);
    
    /*! @brief    Restarts the synchronization timer
     *  @details  The function is invoked at launch time to initialize the timer
     *            and reinvoked when the synchronization timer gets out of sync.
     */
    void restartTimer();
    
    //! @brief    Waits until target_time has been reached and then updates target_time.
    void synchronizeTiming();
    
    
    //
    //! @functiongroup Accessing cycle, rasterline, and frame information
    //
    
    //! @brief    Returns the number of CPU cycles elapsed so far.
    uint64_t currentCycle() { return cpu.cycle; }
    
    //! @brief    Returns the number of the currently drawn frame.
    // uint64_t getFrame() { return frame; }
    
    //! @brief    Returns the number of the currently drawn rasterline.
    // uint16_t getRasterline() { return rasterLine; }

    //! @brief    Returns the currently executed rasterline clock cycle
    // uint8_t getRasterlineCycle() { return rasterCycle; }

    
    //
    //! @functiongroup Operation modes
    //
    
    //! @brief    Returns the ultimax flag
    bool getUltimax() { return ultimax; }
    
    //! @brief    Setter for ultimax.
    void setUltimax(bool b) { ultimax = b; }
    

    //
    //! @functiongroup Handling snapshots
    //
    
    //! @brief    Disables the auto-snapshot feature.
    void disableAutoSnapshots() { if (snapshotInterval > 0) snapshotInterval *= -1; }

    //! @brief    Enables the auto-snapshot feature.
    void enableAutoSnapshots() { if (snapshotInterval < 0) snapshotInterval *= -1; }

    //! @brief    Disables the auto-snapshot feature temporarily.
    void suspendAutoSnapshots() { snapshotInterval -= (LONG_MAX / 2); }

    //! @brief    Disables the auto-snapshot feature temporarily.
    void resumeAutoSnapshots() { snapshotInterval += (LONG_MAX / 2); }

    //! @brief    Returns the currently set snapshot interval in seconds.
    long getSnapshotInterval() { return snapshotInterval; }

    //! @brief    Sets the time interval between two auto-snapshots.
    void setSnapshotInterval(long value) { snapshotInterval = value; }

    /*! @brief    Loads the current state from a snapshot container
     *  @note     THIS FUNCTION IS NOT THREAD SAFE. 
     *            Only use on halted emulators or within the emulation thread
     *  @seealso  loadFromSnapshotSafe
     */
    void loadFromSnapshotUnsafe(Snapshot *snapshot);

    /*! @brief    Thread-safe version of loadFromSnapshotUnsafe
     *  @details  A running emulator is paused before performing the operation
     */
    void loadFromSnapshotSafe(Snapshot *snapshot);

    //! @brief    Restores a snapshot from the auto-save ringbuffer
    bool restoreAutoSnapshot(unsigned nr);

    //! @brief    Restored the latest auto-saved snapshot.
    /*! @note     The reverted snapshot is deleted from the snapshot buffer.
     */
    bool restoreLatestAutoSnapshot();

    //! @brief    Restores a snapshot from the user storage
    bool restoreUserSnapshot(unsigned nr);

    //! @brief    Restores the latest user-saved snapshot.
    bool restoreLatestUserSnapshot();
    
    /*! @brief    Saves the current state into an existing snapshot.
     *  @note     Use this function inside the execution thread.
     *  @seealso  saveToSnapshotSafe
     */
    void saveToSnapshotUnsafe(Snapshot *snapshot);
    
    /*! @brief    Saves the current state into an existing snapshot.
     *  @note     Use this function outside the execution thread.
     *  @seealso  saveToSnapshotUnsafe
     */
    void saveToSnapshotSafe(Snapshot *snapshot);
    
    /*! @brief    Creates a new snapshot object storing the current state.
     *  @note     Use this function inside the execution thread.
     *  @seealso  takeSnapshotSafe
     */
    Snapshot *takeSnapshotUnsafe();
    
    /*! @brief    Creates a new snapshot object storing the current state.
     *  @note     Use this function outside the execution thread.
     *  @seealso  saveToSnapshotUnsafe
     */
    Snapshot *takeSnapshotSafe();

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
    //! @functiongroup Attaching media objects
    //
    
    //! @brief    Mounts an object stored in a file of supported type
    /*! @details  Mountable objects are disks, tapes, and cartridges.
     */
    bool mount(AnyC64File *file);
    
    //! @brief    Flashes an item stored in a file of supported type
    /*! @details  Flashable objects are single programs, roms, and snapshots.
     */
    bool flash(AnyC64File *file, unsigned item = 0);
    
    //! @brief    Loads ROM image into memory
    bool loadRom(const char *filename);
    
    /*! @brief    Inserts an archive into the floppy drive as a virtual disk.
     *  @details  Only D64 and G64 archives are supported.
     */
    bool insertDisk(AnyArchive *a, unsigned drive);
    
    /*! @brief    Inserts a TAP container as a virtual datasette tape.
     *  @details  Only TAP archives can be used as tape.
     *  @deprecated
     */
    bool insertTape(TAPFile *a);

    //! @brief    Attaches a cartridge to the expansion port.
    bool attachCartridgeAndReset(CRTFile *c);

    //! @brief    Detaches a cartridge from the expansion port.
    void detachCartridgeAndReset();

    //! @brief    Returns true iff a cartridge is attached.
    bool isCartridgeAttached();

    
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
};

#endif

