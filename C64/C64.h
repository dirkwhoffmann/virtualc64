/*!
 * @header      C64.h
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

// RELEASE NOTES FOR NEXT RELEASE: 1.8.2
//
// (Hopefully) fixed a bug that caused the emulator to crash on some Macbooks Pros with TouchBar support
// CPU interrupt handler is more accurate now
//
// TODO:
// Fix NSScrubber bug
//
// Add highjacking to IRQs as well (only implemented in BRK so far (?!) )
// !! Move highjacking test to BRK_4 and use oldNmiEdge for testing (nmiEdge will go away)
// 
// Remove nextPossibleXXXcycle, oldI
//
// Change nmiEdge from bool to nmiDown of type uint64_t
//    nmiDown == UINT64_MAX if no edge since last NMI
// Add uint64_t irqUpOrDown
// Replace oldIrq and oldNmi by level_detector(), edge_detector()
// edgeDetector() nmiEdge >(=?) nmiCleared && nmiEdge < currentCycle (-1 ?)
// levelDetector() irqLine == 0 && irqUpOrDown < current-1 || irqLine == 1 && irqChanged == current
// cleanup macros in Instructions.h
// Use Web page INT description in comments



#ifndef _C64_INC
#define _C64_INC

// Disable assert checking in relase versions
// #define NDEBUG

// Data types and constants
#include "C64_defs.h"

// General
#include "Message.h"

// Loading and saving
#include "Snapshot.h"
#include "T64Archive.h"
#include "D64Archive.h"
#include "G64Archive.h"
#include "NIBArchive.h"
#include "PRGArchive.h"
#include "P00Archive.h"
#include "FileArchive.h"
#include "TAPContainer.h"
#include "CRTContainer.h"

// Sub components
#include "ProcessorPort.h"
#include "ExpansionPort.h"
#include "IEC.h"
#include "Keyboard.h"
#include "Joystick.h"
#include "Memory.h"
#include "C64Memory.h"
#include "VC1541Memory.h"
#include "VIC.h"
#include "PixelEngine.h"
#include "SIDWrapper.h"
#include "TOD.h"
#include "CIA.h"
#include "CPU.h"

// Cartridges
#include "CustomCartridges.h"

// Peripherals
#include "VC1541.h"
#include "Datasette.h"

/*
Overall architecture:
---------------------

VirtualC64 consists of three major components:

1. The graphical user interface (written in Swift and ObjC)
2. The communication proxy (written in ObjC)
3. The core emulator (written in C++)

The GUI talks to VirtualC64 by calling proxy methods. VirtualC64 talks back via
a message queue that is queried periodically by the GUI.

------------------------------------------------------------------
| getMessage()                                      putMessage() |
v                                                                |
----------------------          ----------------------          ----------------------
|                    |  func()  |                    |  func()  |                    |
|        GUI         |--------->|      C64Proxy      |--------->|        C64         |
|  (Swift and ObjC)  |          | Swift / C++ bridge |          |    (C++ world)     |
----------------------          ----------------------          ----------------------


Initialization procedure:
-------------------------

To get VirtualC64 up and running, you need to perform the following steps:

1. Create a C64 object
c64 = new C64()

2. Configure
c64->set...() etc.

3. Load Roms
c64->loadRom(...)

4. Power up
c64->powerUp()

loadRom() is an important function as it does mutliple things. In the first place, it will
 initialize one of the four ROMs in the virtual computer. If all ROMs are read in, it does
 two addition things. Firstly, it calls reset() to initialize all components. Note that it
 does not make sense to reset the computer earlier as some information such as the start
 address of the program counter is stored in ROM. Secondly, it sends MSG_READY_TO_RUN to the
 GUI. The GUI reacts with a call to powerUp(). This function brings the emulator
 to life by creating and launching the execution thread.
 

Message queue:
--------------

To receive messages from VirtualC64, the GUI starts a timer that periodically invokes method MyController::timerFunc.
Inside this method, the message queue is queries as follows:

while ((message = [c64 message]) != NULL) {
    switch (message->id) {
            
        case MSG_READY_TO_RUN:
            [c64 run];
            
            ...
    }
}


The execution thread:
---------------------

The execution thread is organized as an infinite loop. In each iteration, control is
passed to the VIC, CIAs, CPU, VIAs, and the disk drive. The VIC chip draws the screen
contents into a simple byte array, the so called screen buffer. The asynchronously
running GUI copies the screen buffer contents onto a GPU texture which is then rendered
by the GPU.

The following methods control the execution thread:


run: Runs the emulation thread
 
The GUI invokes this method, e.g., when the user lauches the
emulator the first time or hits the continue button.

halt: Pauses the emulation thread
 
The GUI invokes this method, e.g., when the user hits the pause button.
VirtualC64 itself calls this method when, e.g., a breakpoint is reached.

suspend / result: Temporarily pauses the emulation thread
 
If multiple operations need to be executed atomically (such as
taking an emulator snapshot), the operations are embedded inside a
suspend() / resume() block. Both methods use halt() and run() internally.

reset:
 
This method restarts the emulation on a freshly initialized computer.
It has the same effect as switching a real C64 off and on again. Note that a
 few items are retained during a reset, e.g., an attached cartridge.
 */



#define BACK_IN_TIME_BUFFER_SIZE 16

//! @class    A complete virtual C64
class C64 : public VirtualComponent {

    // -----------------------------------------------------------------------------------------
    //                                          Properties
    // -----------------------------------------------------------------------------------------

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
	
    //! @brief    The C64s sound chip
	SIDWrapper sid;
	
    //! @brief    The C64s virtual keyboard
	Keyboard keyboard;
	
    //! @brief    The C64s first virtual joystick (plugged into CONTROL PORT 1)
	Joystick joystickA = Joystick(1);

    //! @brief    The C64s second virtual joystick (plugged into CONTROL PORT 2)
    Joystick joystickB = Joystick(2);

	//! @brief    The C64s interface bus connecting the VC1541 drive
	IEC iec;

    //! @brief    The C64s virtual expansion port (cartdrige slot)
    ExpansionPort expansionport;

    //! @brief    A virtual VC1541 floppy drive
	VC1541 floppy;

    //! @brief    A virtual datasette
    Datasette datasette;

    //
    // Executed cycle, rasterline, and frame
    //
    
    //! @brief    Elapsed C64 clock cycles since power up
    uint64_t cycle;
    
    //! @brief    Total number of frames drawn since power up
    uint64_t frame;
    
    //! @brief    Currently drawn rasterline
    uint16_t rasterline;
    
    /*! @brief    Currently executed clock cycle relative to the current rasterline
     *  @details  Range: 1 ... 63 on PAL machines, 1 ... 65 on NTSC machines
     */
    uint8_t rasterlineCycle;

private:

    //
    // Execution thread
    //
    
    //! @brief    The emulators execution thread
    pthread_t p;
    
    /*! @brief    System timer information
     *  @details  Used to put the emulation thread to sleep for the proper amount of time
     */
    mach_timebase_info_data_t timebase;
    
    /*! @brief    Wake-up time of the synchronization timer in nanoseconds
     *  @details  This value is recomputed each time the emulator thread is put to sleep
     */
    uint64_t nanoTargetTime;

    //! Indicates if c64 is currently running at maximum speed (with timing synchronization disabled)
    bool warp;
    
    //! Indicates that we should always run as possible
    bool alwaysWarp;
    
    //! Indicates that we should run as fast as possible at least during disk operations
    bool warpLoad;
    
    
    //
    // Message queue
    //
    
    /*! @brief    Message queue.
     *  @details  Used to communicate with the graphical user interface.
     */
    MessageQueue queue;
    

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
    // Time travel ring buffer
    //
    
    //! @brief    Ring buffer for storing the time travel snapshot images
    Snapshot *backInTimeHistory[BACK_IN_TIME_BUFFER_SIZE];
    
    //! @brief    Write pointer of the time travel ring buffer
    unsigned backInTimeWritePtr;
    

 
    
    
	// -----------------------------------------------------------------------------------------------
	//                                             Methods
	// -----------------------------------------------------------------------------------------------
	
public:
	
	//! @brief    Constructor
	C64();
	
	//! @brief    Destructor
	~C64();

	//! @brief    Resets the virtual C64 and all of its sub components.
    void reset();
     
    //! @brief    Dumps current configuration into message queue
    void ping();

	//! @brief    Prints debugging information
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
	
	//! @brief    Returns true if the emulator is currently running in PAL mode
    bool isPAL() { return vic.isPAL(); }

	/*! @brief    Puts the emulator in PAL mode
     *  @details  This method plugs in a PAL VIC chip and reconfigures SID with the proper timing information 
     */
	void setPAL();
	
    //! @brief    Returns true if the emulator is currently running in NTSC mode
    bool isNTSC() { return !vic.isPAL(); }

    /*! @brief    Puts the emulator in PAL mode
     *  @details  This method plugs in a PAL VIC chip and reconfigures SID with the proper timing information
     */
	void setNTSC();

    //! @brief    Returns true iff audio filters are enabled.
    bool getAudioFilter() { return sid.getAudioFilter(); }

	//! @brief    Enables or disables SID audio filters.
	void setAudioFilter(bool value) { sid.setAudioFilter(value); }
      
    //! @brief    Returns true if reSID library is used
    bool getReSID() { return sid.getReSID(); }

    //! @brief    Turns reSID library on or off
    void setReSID(bool value) { sid.setReSID(value); }

    //! @brief    Gets the sampling method
    sampling_method getSamplingMethod() { return sid.getSamplingMethod(); }
    
    //! @brief    Sets the sampling method
    void setSamplingMethod(sampling_method value) { sid.setSamplingMethod(value); }
    
    //! @brief    Gets the SID chip model
    chip_model getChipModel() { return sid.getChipModel(); }
    
    //! @brief    Sets the SID chip model
    void setChipModel(chip_model value) { sid.setChipModel(value); }

    
    //
    //! @functiongroup Running the emulator
    //

    //! @brief    Cold starts the virtual C64
    /*! @details  The emulator and all of its sub components are reset and
     *            the execution thread is started.
     */
    void powerUp();
    
    //! @brief    Continues emulation
    /*! @details  This method recreates the emulation thread and is usually called after
     *            emulation was stopped by a call to halt() or by reaching a breakpoint.
     */
    void run();
    
    /*! @brief    Pauses emulation
     *  @details  The execution thread is canceled, but the internal state remains intact.
     *            Emulation can be continued by a call to run()
     */
    void halt();
    
    /*! @brief    The tread exit function.
     *  @details  This method is invoked automatically when the execution thread terminates.
     */
    void threadCleanup();

    //! @brief    Returns true iff the virtual C64 is able to run (i.e., all ROMs are loaded)
    bool isRunnable();
    
	//! @brief    Returns true iff the virtual C64 is in the "running" state
	bool isRunning();
	
	//! @brief    Returns true iff the virtual C64 is in the "halted" state
	bool isHalted();
	
	/*! @brief    Executes one CPU instruction
     *  @details  This method implements the "step" action of the debugger
     */
	void step(); 
	
	//! @brief    Executes until the end of the rasterline
	bool executeOneLine();
    
private:
	
    //! @brief    Executes virtual C64 for one cycle
    bool executeOneCycle();
    
	//! @brief    Invoked before executing the first cycle of rasterline
	void beginOfRasterline();
	
    //! @brief    Invoked after executing the last cycle of rasterline
	void endOfRasterline();
		
    
    //
    //! @functiongroup Managing the execution thread
    //
    
    //! @brief    Converts kernel time to nanoseconds
    uint64_t abs_to_nanos(uint64_t abs) { return abs * timebase.numer / timebase.denom; }
    
    //! @brief    Converts nanoseconds to kernel time
    uint64_t nanos_to_abs(uint64_t nanos) { return nanos * timebase.denom / timebase.numer; }
    
public:
    
    //! @brief    Returns true iff cpu runs at maximum speed (timing sychronization is disabled).
    bool getWarp() { return warp; }
    
    //! @brief    Enables or disables timing synchronization.
    void setWarp(bool b);
    
    //! @brief    Returns true iff cpu should always run at maximun speed.
    bool getAlwaysWarp() { return alwaysWarp; }
    
    //! @brief    Setter for alwaysWarp.
    void setAlwaysWarp(bool b);
    
    //! @brief    Returns true iff warp mode is activated during disk operations.
    bool getWarpLoad() { return warpLoad; }
    
    //! @brief    Setter for warpLoad.
    void setWarpLoad(bool b);
    
    /*! @brief    Restarts the synchronization timer
     *  @details  The function is invoked at launch time to initialize the timer and reinvoked
     *            when the synchronization timer gets out of sync.
     */
    void restartTimer();
    
    //! @brief    Waits until target_time has been reached and then updates target_time.
    void synchronizeTiming();
    
    
    //
    //! @functiongroup Accessing cycle, rasterline, and frame information
    //
    
    //! @brief    Returns the number of CPU cycles elapsed so far.
    uint64_t getCycles() { return cycle; }
    
    //! @brief    Returns the number of the currently drawn frame.
    uint64_t getFrame() { return frame; }
    
    //! @brief    Returns the number of the currently drawn rasterline.
    uint16_t getRasterline() { return rasterline; }

    //! @brief    Returns the currently executed rasterline clock cycle
    uint8_t getRasterlineCycle() { return rasterlineCycle; }

    
    //
    //! @functiongroup Operation modes
    //
    
    //! @brief    Returns the ultimax flag
    bool getUltimax() { return ultimax; }
    
    //! @brief    Setter for ultimax.
    void setUltimax(bool b) { ultimax = b; }
    
    
    //
    //! @functiongroup Loading ROM images
    //
    
    //! @brief    Loads ROM image into memory
    bool loadRom(const char *filename);


    //
    //! @functiongroup Loading and saving snapshots
    //
    
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

    /*! @brief    Overwrites current state with a snapshot stored in the time travel ringbuffer
     *  @details  Returns true iff the snapshot was found and restored successfully 
     *  @note     THIS FUNCTION IS NOT THREAD SAFE.
     *            Only use on halted emulators or within the emulation thread
     *  @seealso  restoreHistoricSnapshotSafe
     */
    bool restoreHistoricSnapshotUnsafe(unsigned nr);

    /*! @brief    Thread-safe version of restoreHistoricSnapshot
     *  @details  A running emulator is paused before performing the operation
     */
    bool restoreHistoricSnapshotSafe(unsigned nr);

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

    
    /*! @brief    Takes a snapshot and stores it into the time travel ringbuffer
     *  @note     This function does not halt the emulator and must therefore be
     *            called inside the execution thread, only.
     */
    void takeTimeTravelSnapshot();

    /*! @brief    Thread-safe version of takeSnapshoptUnsafe
     *  @details  A running emulator is paused before performing the operation
     */
    // void takeSnapshotSafe();

    /*! @brief    Returns the number of previously taken snapshots
     *  @result   Value between 0 and BACK_IN_TIME_BUFFER_SIZE
     */
    unsigned numHistoricSnapshots();
    
    /*! @brief    Reads a snapshopt from the time travel ringbuffer
     *  @details  The latest snapshot is indexed 0.
     *  @result   A reference to a snapshot, if present. NULL, otherwise.
     */
    Snapshot *getHistoricSnapshot(int nr);
    

    //
    //! @functiongroup Handling disks, tapes, and cartridges
    //
    
	/*! @brief    Flushes a single item from an archive into memory.
     */
	bool flushArchive(Archive *a, int item);
	
	/*! @brief    Inserts an archive into the floppy drive as a virtual disk.
     *  @details  Only D64 and G64 archives are supported.
     */
    bool insertDisk(Archive *a);
    
    /*! @brief    Old function for mounting an archive as a disk.
     *  @details  Only D64 and G64 archives are supported.
     *  @deprecated Use insertDisk instead
     */
	bool mountArchive(Archive *a);

    /*! @brief    Inserts a TAP container as a virtual datasette tape.
     *  @details  Only TAP archives can be used as tape.
     */
    bool insertTape(TAPContainer *a);

	//! @brief    Attaches a cartridge to the expansion port.
	bool attachCartridgeAndReset(CRTContainer *c);

	//! @brief    Detaches a cartridge from the expansion port.
	void detachCartridgeAndReset();

	//! @brief    Returns true iff a cartridge is attached.
	bool isCartridgeAttached();

    
    //
    //! @functiongroup Accessing the message queue
    //
    
    //! @brief    Registers a listener callback function
    void setListener(const void *sender, void(*func)(const void *, int) ) {
        queue.setListener(sender, func);
    }
    //! @brief    Gets a notification message from message queue
    VC64Message getMessage() { return queue.getMessage(); }
    
    //! @brief    Feeds a notification message into message queue
    void putMessage(VC64Message msg) {
        
       queue.putMessage(msg);
    }
};

#endif

