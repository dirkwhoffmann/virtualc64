/*
 * (C) 2006 - 2011 Dirk W. Hoffmann. All rights reserved.
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

// New in 0.9.6.1

// SID stops playing in pause mode (bug was introduced in 0.9.6) (issue #116)
// Screen geometry is now adjusted correctly when dragging an NTSC image into a PAL screen or vice versa (issue #113)
// Memory panel now refreshes correctly (issue #111)
// Floppy disks mow remain in drive after reset (issue +)

#ifndef _C64_INC
#define _C64_INC

#include "basic.h"
#include "VirtualComponent.h"
#include "Message.h"
#include "Snapshot.h"
#include "T64Archive.h"
#include "D64Archive.h"
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
#include "SIDWrapper.h"
#include "TOD.h"
#include "CIA.h"
#include "CPU.h"
#include "VC1541.h"
#include "Cartridge.h"

//! A complete virtual C64
/*! The class puts all components together to a working virtual computer.

    \verbatim
	
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
 |    ------------------------       copy to openGL texture      -----------------
 ---->| GUI                  |<----------------------------------| Screen buffer |
      ------------------------                                   -----------------
	\endverbatim


	The execution thread is the "engine" of the virtual computer. 
	Like all virtual components, the virtual C64 can be in two states: "running" and "halted". 
	When the virtual C64 enters the "run" state, it starts the execution thread which runs asynchoneously. 
	The thread runs until an error occurrs (illegal instruction, etc.) or the user asks the virtual 
	machine to freeze. In both cases, the thread terminates and the virtual C64 enters the "halt" state.

	The execution thread is organized as an infinite loop. In each iteration, control is passed to the
	VIC, CIAs, CPU, VIAs, and the disk drive. The VIC chip draws the screen contents into a
	simple byte array, the so called screen buffer. The asynchronously running GUI copies the screen buffer
	contents onto an OpenGL texture which is then rendered by the graphic card.

    Class C64 is the most important class of the core emulator and MyController the most important GUI class. 
    C64Proxy implements a bridge between the GUI (written in Objective-C) anf the emulator (written in C++).
  
	Initialization sequence:
	
	1. Create C64 object 
	   c64 = new C64()
	   
	2. Configure
	   c64->setPAL() etc.
 
	3. Load Roms
	   c64->loadRom(...)
	
    4. Run
	   c64->run() 
*/

#define BASIC_ROM 1
#define CHAR_ROM 2
#define KERNEL_ROM 4
#define VC1541_ROM 8

#define NUM_INPUT_DEVICES 4

#define BACK_IN_TIME_BUFFER_SIZE 16

enum INPUT_DEVICES {
	IPD_UNCONNECTED = 0,
	IPD_KEYBOARD,
	IPD_JOYSTICK_1,
	IPD_JOYSTICK_2
};


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
	
	//! Reference to the connected joysticks
	Joystick *joystick1, *joystick2;
	
	//! Reference to the virtual IEC bus
	IEC *iec;
	
	//! Reference to the virtual VC1541
	VC1541 *floppy;
		

		
private:

	//! The execution thread
	pthread_t p;

    //! Additional frame delay
    /*! = 0 : Emulator runs at original speed (varies between PAL and NTSC) (default value) 
     > 0 : Emulator runs slower than origonal machine 
     < 0 : Emulator runs faster than original machine 
     */
    int frameDelayOffset;

	//! Indicates if c64 is currently running at maximum speed (with timing synchronization disabled)
	bool warp;
    
    //! Indicates that we should always run as possible
	bool alwaysWarp;
    
    //! Indicates that we should run as fast as possible at least during disk operations
	bool warpLoad;

	//! Game port configuration.
	/*! The value is determined by the enumeration type INPUT_DEVICES */
	int port[2];
    
	//! Snapshot history ring buffer (for cheatbox)
	Snapshot *backInTimeHistory[BACK_IN_TIME_BUFFER_SIZE]; 
    
	//! ring buffer write pointer
	unsigned backInTimeWritePtr;
    
    
    //
    //  State variables (will be saves to snapshot)
    //

    //! PAL or NTSC machine?
    bool pal;

	//! Current clock cycle since power up
	uint64_t cycles;
	
    //! Current clock cycle relative to the current rasterline
    /*! Range: 1 ... 63 on PAL machines
               1 ... 65 on NTSC machines */
	int rasterlineCycle;
    
	//! Current frame number since power up
	uint64_t frame;
	
	//! Current rasterline number
	int rasterline;
	
	//! Target time
	/*! Used to synchronize emulation speed */
	uint64_t targetTime; 
		
    
	// -----------------------------------------------------------------------------------------------
	//                                             Methods
	// -----------------------------------------------------------------------------------------------
	
public:
	
	//! Constructor
	C64();
	
	//! Destructor
	~C64();
	
	//! Reset the virtual C64 and all of its virtual sub-components to its initial state.
	/*! A reset is performed by simulating a hard reset on a real C64. */
	void reset();           
	
	//! Reset the virtual C64 and all of its virtual sub-components to its initial state.
	/*! A (faked) reset is performed by loading a presaved image from disk. */
	void fastReset();           

	//! Dump current state into logfile
	void dumpState();
	
			
	// -----------------------------------------------------------------------------------------------
	//                                         Configure hardware
	// -----------------------------------------------------------------------------------------------
	
public:
	
	//! Returns true for PAL machines
	inline bool isPAL() { return pal; }

	//! Set PAL mode
	void setPAL();
	
	//! Returns true for NTSC machines
	inline bool isNTSC() { return !pal; }

	//! Set NTSC mode
	void setNTSC();
	
    //! Returns the user definable speed adjustment (msec per frame)
	inline int getFrameDelayOffset() { return frameDelayOffset; } 
    
	//! Sets the user definable speed adjustment (msec per frame)
	inline void setFrameDelayOffset(int delay) { frameDelayOffset = delay; }
    
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

    //! Get color scheme
    VIC::ColorScheme getColorScheme() { return vic->getColorScheme(); }
    
	//! Set color scheme
	void setColorScheme(VIC::ColorScheme scheme) { vic->setColorScheme(scheme); }

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

	//! Load state from memory buffer
	void loadFromBuffer(uint8_t **buffer);
	
	//! Save state to snapshot container
	void saveToSnapshot(Snapshot *snapshot);
	
	//! Save state to memory buffer
	void saveToBuffer(uint8_t **buffer);
	
	
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
	
	//! Initialize timer (sets variable target_time)
	void restartTimer();
	
	//! Wait until target_time has been reached and then updates target_time.
	void synchronizeTiming();
	
    //! Returns true iff cpu runs at maximum speed (timing sychronization is disabled)
	bool getWarp() { return warp; }
	
    
	// ---------------------------------------------------------------------------------------------
	//                                 Archives (disks, tapes, etc.)
	// ---------------------------------------------------------------------------------------------
	
	//! Flush specified item from archive into memory and delete archive
	/*! All archive types are flushable */
	bool flushArchive(Archive *a, int item);
	
	//! Mount specified archive in the virtual disk drive.
	/*! Only D64 archives are mountable */
	bool mountArchive(D64Archive *a);

	
	// ---------------------------------------------------------------------------------------------
	//                                            Cartridges
	// ---------------------------------------------------------------------------------------------

	//! Attach cartridge
	bool attachCartridge(Cartridge *c);
	
	// Detach cartridge
	bool detachCartridge();

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
	inline uint64_t getRasterline() { return rasterline; }

	// Returns the number of frames per second
	/*! Number varies between PAL and NTSC machines */	
    inline int getFramesPerSecond() { if (pal) return VIC::PAL_REFRESH_RATE; else return VIC::NTSC_REFRESH_RATE; }
	
	//! Returns the number of rasterlines per frame
	/*! Number varies between PAL and NTSC machines */	
    inline int getRasterlinesPerFrame() { if (pal) return VIC::PAL_RASTERLINES; else return VIC::NTSC_RASTERLINES; }
	
	//! Returns the number of CPU cycles performed per rasterline
	/*! Number varies between PAL and NTSC machines */	
	inline int getCyclesPerRasterline() { if (pal) return VIC::PAL_CYCLES_PER_RASTERLINE; else return VIC::NTSC_CYCLES_PER_RASTERLINE; }
	
	//! Returns the number of CPU cycles performed per frame
	/*! Number varies between PAL and NTSC machines */	
	inline int getCyclesPerFrame() { if (pal) return VIC::PAL_RASTERLINES * VIC::PAL_CYCLES_PER_RASTERLINE; else return VIC::NTSC_RASTERLINES * VIC::NTSC_CYCLES_PER_RASTERLINE; }
        
	//! Returns the time interval between two frames
	inline int getFrameDelay() { if (pal) return 1000000 / VIC::PAL_REFRESH_RATE; else return 1000000 / VIC::NTSC_REFRESH_RATE; }

    
	// ---------------------------------------------------------------------------------------------
	//                                             Misc
	// ---------------------------------------------------------------------------------------------
	
	//! Returns the build number
	/*! The build number is composed out of the build date */
	int build();
	
	//! Bind input device with game port
	void setInputDevice(int portNo, int newDevice);

	//! Switch the input device to the next available
	void switchInputDevice(int port);

	//! Switch input devices between both ports
	void switchInputDevices();
	
	//! Get the device mapped to the port portNo
	int getDeviceOfPort(int portNo);
	
	//! Sets a new joystick
	Joystick *addJoystick();
	
	//! Removes a joystick
	void removeJoystick(Joystick *joystick);
	
	//! The tread exit function.
	/*! Automatically invoked by the execution thread on termination */
	void threadCleanup(); 
};

#endif

