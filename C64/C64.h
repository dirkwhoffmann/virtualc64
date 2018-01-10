/*!
 * @header      C64.h
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   2006 - 2017 Dirk W. Hoffmann
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

// NEXT RELEASE: 1.5.3
//
// VirtualC64 passes VICE test VICII/phi1timing.
// Sprite X expansion register is now updated after pixel 6 and not after pixel 8 as it was before.
// VICE color palette has been updated and made the default palette.
//
// TODO:
//
// TODO:
// Use better text descriptions in Mount dialog for G64 and NIB files
// Cartridge dialog
// 
// SPEEDUP:
//
// 1. Add routine to quickly get the disk name from GCR data
//    Right now, the hardware dialog takes some time to open
//
// ENHANCEMENTS (BRAIN STORMING):
//
// 1. Upscaler (like superEagle)
//    https://github.com/libretro/common-shaders/tree/master/eagle/shaders


#ifndef _C64_INC
#define _C64_INC

// #define NDEBUG      // RELEASE

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

// Sub components
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

// Peripherals
#include "VC1541.h"
#include "Datasette.h"
#include "Cartridge.h"
#include "ExpansionPort.h"


//! @class    A complete virtual C64

/*
	
	------------------------    ------------------------
    |                      |    |                      |
 -->|       C64Proxy       |<-->|         C64          |
 |  |  Obj-C / C++ bridge  |    |      (C++ world)     |
 |  ------------------------    ------------------------
 |                                         |
 |  ------------------------               |
 |	|  (Execution Thread)  |               |
 |	|                      |<--------------- run()            
 |	|         C64          |
 |	------------------------      
 |      |
 |      |
 |      |    --------------------------------------------------------------------------------------         
 |      |--->|                                      CPU                                           |<--------
 |      |    --------------------------------------------------------------------------------------        |
 |      |  execute()                                   |                                                   |
 |      |                                         peek | poke                                              |
 |      |                                              |                                                   |
 |      |                                     A000     V AFFF     D000       DFFF                 FFFF     |    
 |      |    --------------------------------------------------------------------------------------        |
 |      |    |          Memory                | Basic ROM|        | Char ROM | Kernel ROM         |        |
 |      |    --------------------------------------------------------------------------------------        |
 |      |                                                              |                                   |
 |      |                                                         peek | poke                              |
 |      |                                                              V                                   |
 |   	|    							  execute()               -------------            interrupt       |     
 |      |-------------------------------------------------------->|    CIA    |----------------------------|
 |      |                                 execute()               ------------                             |
 |      |-------------------------------------------------------->|    SID    |                            |
 |		|                                                         -------------  setDeviceXXXPin()         |
 |      |-------------------------------------------------------->|    IEC    |<------                     |
 |      |                                 execute()               -------------      |     interrupt       |
 |      |-------------------------------------------------------->|    VIC    |-----------------------------
 |      |                                                         -------------      |
 |      V                                                              |             | execute()
 |  --------------------------                                         |             | 
 |	|   Message queue        |                                         |             V
 |  --------------------------                                         |          -------------
 |				|													   |          |   Drive   |
 |				|													   |          -------------
 |			    |                                                      |
 |	            |                                                      |
 |              |                                                      |
 |	            |                                                      |
 |			    V                                                      V 
 |    ------------------------       copy to GPU texture         -----------------
 ---->| GUI                  |<----------------------------------| Screen buffer |
      ------------------------                                   -----------------

	The execution thread is the "engine" of the virtual computer. 
	Like all virtual components, the virtual C64 can be in two states: "running" and "halted". 
	When the virtual C64 enters the "run" state, it starts the execution thread which runs asynchoneously. 
	The thread runs until an error occurrs (illegal instruction, etc.) or the user asks the virtual 
	machine to freeze. In both cases, the thread terminates and the virtual C64 enters the "halt" state.

	The execution thread is organized as an infinite loop. In each iteration, control is passed to the
	VIC, CIAs, CPU, VIAs, and the disk drive. The VIC chip draws the screen contents into a
	simple byte array, the so called screen buffer. The asynchronously running GUI copies the screen buffer
	contents onto a GPU texture which is then rendered by the graphic card.

    Class C64 is the most important class of the core emulator and MyController the most important GUI class. 
    C64Proxy implements a bridge between the GUI (written in Objective-C) anf the emulator (written in C++).
  
	Initialization sequence:
	
	1. Create C64 object 
	   c64 = new C64()
	   
	2. Configure
	   c64->set...() etc.
 
	3. Load Roms
	   c64->loadRom(...)
	
    4. Run
	   c64->run() 
*/

#define BACK_IN_TIME_BUFFER_SIZE 16

class C64 : public VirtualComponent {

    // -----------------------------------------------------------------------------------------------
    //                                          Properties
    // -----------------------------------------------------------------------------------------------

public:
    
    //
    // Sub components
    //
    
	//! @brief    The C64s virtual memory (ROM, RAM, and color RAM)
	C64Memory mem;
	
	//! @brief    The C64s virtual CPU
    CPU cpu;
	
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

    
    //
    // Time travel ring buffer
    //
    
    //! @brief    Ring buffer for storing the time travel snapshot images
    Snapshot *backInTimeHistory[BACK_IN_TIME_BUFFER_SIZE];
    
    //! @brief    Write pointer of the time travel ring buffer
    unsigned backInTimeWritePtr;
    

    //
    // Message queue
    //
    
    /*! @brief    Message queue.
     *  @details  Used to communicate with the graphical user interface.
     */
    MessageQueue queue;
    
    
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
	
			
    //
    //! @functiongroup Configuring the emulator
    //
	
	//! @brief    Returns true if the emulator is currently running in PAL mode
	inline bool isPAL() { return vic.isPAL(); }

	/*! @brief    Puts the emulator in PAL mode
     *  @details  This method plugs in a PAL VIC chip and reconfigures SID with the proper timing information 
     */
	void setPAL();
	
    //! @brief    Returns true if the emulator is currently running in NTSC mode
	inline bool isNTSC() { return !vic.isPAL(); }

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
    inline sampling_method getSamplingMethod() { return sid.getSamplingMethod(); }
    
    //! @brief    Sets the sampling method
    void setSamplingMethod(sampling_method value) { sid.setSamplingMethod(value); }
    
    //! @brief    Gets the SID chip model
    inline chip_model getChipModel() { return sid.getChipModel(); }
    
    //! @brief    Sets the SID chip model
    void setChipModel(chip_model value) { sid.setChipModel(value); }

    
    //
    //! @functiongroup Running the emulator
    //
		
	//! @brief    Launches the emulator
	/*! @details  The execution thread is launched and the virtual computer enters the "running" state 
     */
	void run();
	
    /*! @brief    The tread exit function.
     *  @details  This method is invoked automatically when the execution thread terminates.
     */
    void threadCleanup();

    //! @brief    Returns true iff the virtual C64 is able to run (i.e., all ROMs are loaded)
    bool isRunnable();
    
	//! @brief    Returns true iff the virtual C64 is in the "running" state
	bool isRunning();
	
	/*! @brief    Freezes the emulator
	 *  @details  The execution thread is terminated and the virtual computers enters the "halted" state 
     */
	void halt();
	
	//! @brief    Returns true iff the virtual C64 is in the "halted" state
	bool isHalted();
	
    /*! @brief    Perform a manually triggered NMI interrupt
     *  @details  On a real C64, an NMI interrupt is triggered by hitting the restore key
     */
    // void restore();

    /*! @brief    Perform a soft reset
     *  @details  On a real C64, a soft reset is triggered by hitting Runstop and Restore
     */
    // void runstopRestore();

	/*! @brief    Executes one CPU instruction
     *  @details  This method implements the "step" action of the debugger
     */
	void step(); 
	
	//! @brief    Executes until the end of the rasterline
	bool executeOneLine();
    
private:
	
    //! @brief    Executes virtual C64 for one cycle
    inline bool executeOneCycle();
    
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
    inline bool getWarp() { return warp; }
    
    //! @brief    Enables or disables timing synchronization.
    void setWarp(bool b);
    
    //! @brief    Returns true iff cpu should always run at maximun speed.
    inline bool getAlwaysWarp() { return alwaysWarp; }
    
    //! @brief    Setter for alwaysWarp.
    void setAlwaysWarp(bool b);
    
    //! @brief    Returns true iff warp mode is activated during disk operations.
    inline bool getWarpLoad() { return warpLoad; }
    
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
    inline uint64_t getCycles() { return cycle; }
    
    //! @brief    Returns the number of the currently drawn frame.
    inline uint64_t getFrame() { return frame; }
    
    //! @brief    Returns the number of the currently drawn rasterline.
    inline uint16_t getRasterline() { return rasterline; }

    //! @brief    Returns the currently executed rasterline clock cycle
    inline uint8_t getRasterlineCycle() { return rasterlineCycle; }

    
    //
    //! @functiongroup Loading ROM images
    //
    
    /*! @brief    Provides information about missing ROM images.
     *  @details  Each missing ROM is indicated by a 1 in the returned bitmap.
     */
    uint8_t getMissingRoms();
    
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

    /*! @brief    Saves the current state to a snapshot container
     *  @note     THIS FUNCTION IS NOT THREAD SAFE.
     *            Only use on halted emulators or within the emulation thread
     *  @seealso  saveToSnapshotSafe
     */
    void saveToSnapshotUnsafe(Snapshot *snapshot);
    
    /*! @brief    Thread-safe version of saveToSnapshot
     *  @details  A running emulator is paused before performing the operation
     */
    void saveToSnapshotSafe(Snapshot *snapshot);

    /*! @brief    Takes a snapshot and stores it into the time travel ringbuffer
     *  @note     THIS FUNCTION IS NOT THREAD SAFE.
     *            Only use on halted emulators or within the emulation thread
     *  @seealso  takeSnapshotSafe
     */
    void takeSnapshotUnsafe();

    /*! @brief    Thread-safe version of takeSnapshoptUnsafe
     *  @details  A running emulator is paused before performing the operation
     */
    void takeSnapshotSafe();

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
    //! @functiongroup Handling archives, tapes, and cartridges
    //
    
	/*! @brief    Flush specified item from archive into memory and delete archive.
	 *  @details  All archive types are flushable.
     */
	bool flushArchive(Archive *a, int item);
	
	/*! @brief    Inserts an archive as a virtual floppy disk.
     *  @details  Only D64 and G64 archives are supported.
     */
	bool mountArchive(Archive *a);

    /*! @brief    Inserts a TAP container as a virtual datasette tape.
     *  @details  Only TAP archives can be used as tape.
     */
    bool insertTape(TAPContainer *a);

	//! @brief    Attaches a cartridge to the expansion port.
	bool attachCartridge(Cartridge *c);
	
	//! @brief    Detaches a cartridge from the expansion port.
	void detachCartridge();

	//! @brief    Returns true iff a cartridge is attached.
	bool isCartridgeAttached();

    
    //
    //! @functiongroup Accessing the message queue
    //
    
    //! @brief    Gets a notification message from message queue
    Message *getMessage() { return queue.getMessage(); }
    
    //! @brief    Feeds a notification message into message queue
    void putMessage(int id, int i = 0) { queue.putMessage(id, i); }
};

#endif

