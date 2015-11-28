/* Written by Dirk W. Hoffmann, 2006 - 2015
 *
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

// VERSION 1.4:
// ENHANCEMENTS:
//
// VirtualC64 can now read NIB image files. Please note that a lot of floppy disks provided in NIB format
// contain copy protection mechanisms which are likly to be incompatible with the current drive implementation.
//
// TODO:
// Add subtext "xx Tracks", "Type 0 tape" to Media Dialog
// Use better text descriptions in Mount dialog for G64 and NIB files
// Cartridge dialog
// VIC Sprite compatibility (Sprite enable timing)
//
// CLEANUP:
// SPEEDUP:
//
// 1. Make CIA1 and CIA2 dynamic objects
//    Inline execution functions as much as possible
// 2. Add routine to quickly get the disk name from GCR data
//    Right now, the hardware dialog takes some time to open
//
//
// ENHANCEMENTS (BRAIN STORMING):
//
// 1. Upscaler (like superEagle)
//    https://github.com/libretro/common-shaders/tree/master/eagle/shaders


#ifndef _C64_INC
#define _C64_INC

#define NDEBUG      // RELEASE
#define DEBUG_LEVEL 2  // RELEASE

// Snapshot version number of this release
#define V_MAJOR 1
#define V_MINOR 3
#define V_SUBMINOR 0

#include "basic.h"
#include "VirtualComponent.h"
#include "Message.h"
#include "Snapshot.h"
#include "T64Archive.h"
#include "D64Archive.h"
#include "G64Archive.h"
#include "NIBArchive.h"
#include "TAPArchive.h"
#include "PRGArchive.h"
#include "P00Archive.h"
#include "FileArchive.h"
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
#include "VC1541.h"
#include "Datasette.h"
#include "Cartridge.h"
#include "ExpansionPort.h"


//! A complete virtual C64
/*! The class puts all components together to a working virtual computer.
	
	------------------------    ------------------------
    |                      |    |                      |
 -->|       C64Proxy       |<-->|         C64          |
 |  |  Obj-C / C++ bridge  |    |      (c++ world)     |
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

#define BASIC_ROM 1
#define CHAR_ROM 2
#define KERNEL_ROM 4
#define VC1541_ROM 8

#define BACK_IN_TIME_BUFFER_SIZE 16


class C64 : public VirtualComponent {

public:	
	
	//! Message queue. Used to communicate with the graphical user interface.
	MessageQueue queue;
	
	//! Reference to the connected virtual memory. 	
	C64Memory *mem;
	
	//! Reference to the connected virtual CPU. 	
    CPU *cpu;
	
	//! Reference to the connected virtual video controller (VIC). 	
	VIC *vic;
	
	//! Reference to the first connected virtual complex interface adapter (CIA 1). 	
	CIA1 *cia1;
	
	//! Reference to the first connected virtual complex interface adapter (CIA 2). 	
	CIA2 *cia2;
	
	//! Reference to the connected sound interface device (SID).
	SIDWrapper *sid;
	
	//! Reference to the connected virtual keyboard.
	Keyboard *keyboard;
	
	//! Reference to joystick in port 1
	Joystick *joystick1;

    //! Reference to joystick in port 2
    Joystick *joystick2;

	//! Reference to the virtual IEC bus
	IEC *iec;

    //! Reference to the virtual expansion port (cartdrige slot)
    ExpansionPort *expansionport;

	//! Reference to the virtual VC1541
	VC1541 *floppy;

    //! Virtual tape drive (Datasette)
    Datasette datasette;

    
private:

	//! The execution thread
	pthread_t p;
    
    //! System timer information (needed for running the execution thread at the desired speed)
    mach_timebase_info_data_t timebase;
    
    //! Converts kernel time to nanoseconds
    uint64_t abs_to_nanos(uint64_t abs) { return abs * timebase.numer / timebase.denom; }

    //! Converts nanoseconds to kernel time
    uint64_t nanos_to_abs(uint64_t nanos) { return nanos * timebase.denom / timebase.numer; }
   
	//! Snapshot history ring buffer (for cheatbox)
	Snapshot *backInTimeHistory[BACK_IN_TIME_BUFFER_SIZE]; 
    
	//! ring buffer write pointer
	unsigned backInTimeWritePtr;
    
    // -----------------------------------------------------------------------------------------------
    //                                          Properties
    // -----------------------------------------------------------------------------------------------

    //! Indicates if c64 is currently running at maximum speed (with timing synchronization disabled)
    bool warp;
    
    //! Indicates that we should always run as possible
    bool alwaysWarp;
    
    //! Indicates that we should run as fast as possible at least during disk operations
    bool warpLoad;

	//! Current clock cycle since power up
	uint64_t cycles;
	
    //! Current clock cycle relative to the current rasterline
    /*! Range: 1 ... 63 on PAL machines
               1 ... 65 on NTSC machines */
    int rasterlineCycle;
    
	//! Current frame number since power up
	uint64_t frame;
	
	//! Current rasterline number
	uint16_t rasterline;
	
private:
    
	//! Target time in nanoseconds
	/*! Used to synchronize emulation speed. */
	uint64_t nanoTargetTime;

    
	// -----------------------------------------------------------------------------------------------
	//                                             Methods
	// -----------------------------------------------------------------------------------------------
	
public:
	
	//! Constructor
	C64();
	
	//! Destructor
	~C64();

	//! Reset the virtual C64 and all of its sub components. 
	/*! A reset is performed by simulating a hard reset on a real C64. */
    void reset();
    
    //! Dump current configuration into message queue
    void ping();

	//! Dump current state into logfile
	void dumpState();
	
			
	// -----------------------------------------------------------------------------------------------
	//                                         Configure hardware
	// -----------------------------------------------------------------------------------------------
	
public:
	
	//! Returns true for PAL machines
	inline bool isPAL() { return vic->isPAL(); }

	//! Set PAL mode
    //  DEPRECATED. PAL/NTSC IS DETERMINED BY VIC CHIP MODEL
	void setPAL();
	
	//! Returns true for NTSC machines
    //  DEPRECATED. PAL/NTSC IS DETERMINED BY VIC CHIP MODEL
	inline bool isNTSC() { return !vic->isPAL(); }

	//! Set NTSC mode
	void setNTSC();

    //! Returns true iff audio filters are enabled.
    bool getAudioFilter() { return sid->getAudioFilter(); }

	//! Enable or disable filters of SID.
	void setAudioFilter(bool value) { sid->setAudioFilter(value); }
      
    //! Returns true if reSID library is used
    bool getReSID() { return sid->getReSID(); }

    //! Turn reSID library on or off
    void setReSID(bool value) { sid->setReSID(value); }

    //! Get sampling method
    inline sampling_method getSamplingMethod() { return sid->getSamplingMethod(); }
    
    //! Set sampling method
    void setSamplingMethod(sampling_method value) { sid->setSamplingMethod(value); }
    
    //! Get chip model 
    inline chip_model getChipModel() { return sid->getChipModel(); }
    
    //! Set chip model 
    void setChipModel(chip_model value) { sid->setChipModel(value); }

	
	// -----------------------------------------------------------------------------------------------
	//                                       Loading and saving
	// -----------------------------------------------------------------------------------------------

public:
	
	//! Load state from snapshot container
	void loadFromSnapshot(Snapshot *snapshot);

    //! Save state to snapshot container
    void saveToSnapshot(Snapshot *snapshot);

	
	// -----------------------------------------------------------------------------------------------
	//                                           Control
	// -----------------------------------------------------------------------------------------------

public:
	
	//! Perform a soft reset
	/*! On a real C64, a soft reset is triggered by hitting Runstop and Restore */
	void runstopRestore(); 
	
	//! Returns true iff the virtual C64 is able to run (i.e., all ROMs are loaded)
	bool isRunnable();
	
	//! Power on virtual C64
	/*! The execution thread is launched and the virtual computer enters the "running" state */
	void run();
	
	// Returns true iff the virtual C64 is in the "running" state */
	bool isRunning();
	
	//! Freeze the emulator
	/*! The execution thread is terminated and the virtual computers enters the "halted" state */
	void halt();
	
	//! Returns true iff the virtual C64 is in the "halted" state */
	bool isHalted();
	
	//! Execute one command
	void step(); 
	
	//! Execute virtual C64 for one cycle
	inline bool executeOneCycle();
	
	//! Execute until the end of the rasterline
	bool executeOneLine();
	
	//! Get notification message from message queue
	Message *getMessage();
	
	//! Feed notification message into message queue
	void putMessage(int id, int i = 0, void *p = NULL, const char *c = NULL);
	
private:
	
	//! Actions to be performed at the beginning of each rasterline
	void beginOfRasterline();
	
	//! Actions to be performed at the end of each rasterline	
	void endOfRasterline();
	
	
	// -----------------------------------------------------------------------------------------------
	//                                  ROM and snapshot handling
	// -----------------------------------------------------------------------------------------------

public:
		
	//! Missing ROMs are indicated by a 1 in the returned bitmap */
	uint8_t getMissingRoms();

	//! Load ROM image into memory
	bool loadRom(const char *filename);

	//! Take a snapshot and store it in ringbuffer
	void takeSnapshot();
	
	//! Returns the number of previously taken snapshots
	/*! Returns a number between 0 and BACK_IN_TIME_BUFFER_SIZE */
	unsigned numHistoricSnapshots();
	
	//! Get snapshot from history buffer
	/*! The latest snapshot has number 0. Return NULL, if requested snapshot does not exist */
	Snapshot *getHistoricSnapshot(int nr);
	
	
	// -----------------------------------------------------------------------------------------------
	//                                           Timing
	// -----------------------------------------------------------------------------------------------
	
public:
    
    //! Returns true iff cpu runs at maximum speed (timing sychronization is disabled)
    inline bool getWarp() { return warp; }
    
    //! Enable or disable timing synchronization
    void setWarp(bool b);
    
    //! Returns true iff cpu should always run at maximun speed
    inline bool getAlwaysWarp() { return alwaysWarp; }
    
    //! Setter for alwaysWarp
    void setAlwaysWarp(bool b);
    
    //! Returns true iff warp mode is activated during disk operations
    inline bool getWarpLoad() { return warpLoad; }
    
    //! Setter for warpLoad
    void setWarpLoad(bool b);
    
	//! Initialize timer (sets variable target_time)
	void restartTimer();
	
	//! Wait until target_time has been reached and then updates target_time.
	void synchronizeTiming();
	
    
	// ---------------------------------------------------------------------------------------------
	//                                 Archives (disks, tapes, etc.)
	// ---------------------------------------------------------------------------------------------
	
public:
    
	//! Flush specified item from archive into memory and delete archive
	/*! All archive types are flushable */
	bool flushArchive(Archive *a, int item);
	
	//! @brief      Inserts an archive as a virtual floppy disk
    /*! @discussion Only D64 and G64 archives are supported */
	bool mountArchive(Archive *a);

    //! @brief      Inserts a TAP archive as a virtual datasette tape
    /*! @discussion Only TAP archives can be used as tape */
    bool insertTape(TAPArchive *a);

	
	// ---------------------------------------------------------------------------------------------
	//                                            Cartridges
	// ---------------------------------------------------------------------------------------------

	//! Attach cartridge
	bool attachCartridge(Cartridge *c);
	
	// Detach cartridge
	void detachCartridge();

	//! Returns true iff a cartridge is attached
	bool isCartridgeAttached();

	
	// ---------------------------------------------------------------------------------------------
	//                                        Getter and setter 
	// ---------------------------------------------------------------------------------------------
			
	//! Returns the number of CPU cycles elapsed so far
	inline uint64_t getCycles() { return cycles; }

	//! Returns the number of the currently drawn frame
	inline uint64_t getFrame() { return frame; }

	//! Returns the number of the currently drawn rasterline
	inline uint16_t getRasterline() { return rasterline; }
    
    
	// ---------------------------------------------------------------------------------------------
	//                                             Misc
	// ---------------------------------------------------------------------------------------------
	
	//! The tread exit function.
	/*! Automatically invoked by the execution thread on termination */
	void threadCleanup(); 
};

#endif

