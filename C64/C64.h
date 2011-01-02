/*
 * (C) 2006 Dirk W. Hoffmann. All rights reserved.
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


// Release notes
// 1. First release milestone reached: VirtualC64 passes all Lorenz tests
// 2. CIA code has been simplified 
// 3. Fixed an issue with the virtual keyboard mapping (Turbo assembler now works well)

#ifndef _C64_INC
#define _C64_INC

// Personal Includes
#include "basic.h"
#include "VirtualComponent.h"
#include "Message.h"
#include "Snapshot.h"
#include "T64Archive.h"
#include "D64Archive.h"
#include "PRGArchive.h"
#include "P00Archive.h"
#include "IEC.h"
#include "Keyboard.h"
#include "Joystick.h"
#include "Memory.h"
#include "C64Memory.h"
#include "VC1541Memory.h"
#include "VIC.h"
#include "SID.h"
#include "TOD.h"
#include "CIA.h"
#include "CPU.h"
#include "VC1541.h"
#include "Cartridge.h"

//! A complete virtual C64
/*! The class puts all components together to a working virtual computer.

    \verbatim
	------------------------
	|  (Execution Thread)  |
	|                      |
	|         C64          |
	------------------------      
        ^
        |
        |    --------------------------------------------------------------------------------------         
        |--->|                                      CPU                                           |<--------
        |    --------------------------------------------------------------------------------------        |
        |                                              |                                                   |
        |                                         peek | poke                                              |
        |                                              |                                                   |
        |                                     A000     V AFFF     D000       DFFF                 FFFF     |    
        |    --------------------------------------------------------------------------------------        |
        |    |          Memory                | Basic ROM|        | Char ROM | Kernel ROM         |        |
        |    --------------------------------------------------------------------------------------        |
        |                                                              |                                   |
        |                                                         peek | poke                              |
        |                                                              V                                   |
		|    							  execute()               -------------            interrupt       |     
        |-------------------------------------------------------->|    CIA    |----------------------------|
        |                                 execute()               ------------                             |
        |-------------------------------------------------------->|    SID    |                            |
		|                                                         -------------  setDeviceXXXPin()         |
        |-------------------------------------------------------->|    IEC    |<------                     |
        |                                 execute()               -------------      |     interrupt       |
        |-------------------------------------------------------->|    VIC    |-----------------------------
        |                                                         -------------      |
        V                                                              |             | execute()
    ----------------------------                                       |             | 
	|   | C64Listener          |                                       |             V
    |   -----------------------|                                       |          -------------
	|       |   C64 Proxy      |                                       |          |   Drive   |
	|       |                  |                                       |          -------------
	--------|-------------------                                       |
	        |                                                          |
            | draw(), run(), halt()                                    |
	        |                                                          |
			V                                                          V 
     ------------------------       copy to openGL texture      -----------------
     | GUI                  |<----------------------------------| Screen buffer |
     ------------------------                                   -----------------
	\endverbatim


	The execution thread is the "engine" of the virtual computer. 
	Like all virtual components, the virtual C64 can be in two states: "running" and "halted". 
	When the virtual C64 enters the "run" state, it starts the asynchronously running execution thread. 
	The thread runs until an error occurrs (illegal instruction, etc.) or the user asks the virtual 
	machine to freeze. In both cases, the thread terminates and the virtual C64 enters the "halt" state.

	The execution thread is organized as an infinite loop. In each iteration, control is passed to the
	virtual CPU, the virtual CIA and the virtual VIC chip. The VIC chip draws the screen contents into a
	simple byte array, the so called screen buffer. The asynchronously running GUI copies the screen buffer
	contents onto an OpenGL texture which is then rendered by the graphic card.

	The C64Listener serves as a proxy object to the outer world (i.e., the GUI). When the C64 enters
	the "running" or "halt" state, it invokes run() or halt(), respectively. Similarily, the draw() method
	is invoked when a frame has been completed by the VIC chip.

	Initialization sequence:
	
	1. Create C64 object 
	   c64 = new C64()
	   
	2. Register listener
	   c64->setListener(...)
	   
	3. Reset emulator
	   c64->reset()
	   
	4. Load Roms
	   c64->loadRom(...)
	
	   The emulator launches automatically when the last ROM is loaded
 */

#define BASIC_ROM 1
#define CHAR_ROM 2
#define KERNEL_ROM 4
#define VC1541_ROM 8

#define NUM_INPUT_DEVICES 4

enum INPUT_DEVICES {
	IPD_UNCONNECTED = 0,
	IPD_KEYBOARD,
	IPD_JOYSTICK_1,
	IPD_JOYSTICK_2
};
	
class C64 : public VirtualComponent {

public:	
	
	//! Message queue. Used to communicate with the GUI
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
	SID *sid;
	
	//! Reference to the connected virtual keyboard.
	Keyboard *keyboard;
	
	//! Reference to the connected joysticks
	Joystick *joystick1, *joystick2;
	
	//! Reference to the virtual IEC bus
	IEC *iec;
	
	//! Reference to the virtual VC1541
	VC1541 *floppy;
		
	//! Size of a snapshot file in bytes
	static const int SNAPSHOT_SIZE = 3000000; 

	//! Current clock cycle since beginning of rasterline (ranges from 1 .. 63 (PAL) or 65 (NTSC))
	int rasterlineCycle;

	//! Logfile
	//* For debugging only */
	FILE *logfile;
		
private:

	//! The execution thread
	pthread_t p;

	//! Current clock cycle since power up
	uint64_t cycles;
	
	//! Current frame number
	int frame;
	
	//! Current rasterline number
	int rasterline;
	
	//! Target time
	/*! Used to synchronize emulation speed */
	uint64_t targetTime; 
	
	//! Number of frames per second
	/*! Number varies between PAL and NTSC machines. Don't modify this value directly. It is automatically computed 
		in setPAL or setNTSC. */	
	int fps;
	
	//! Number of rasterlines
	/*! Number varies between PAL and NTSC machines. Don't modify this value directly. It is automatically computed 
		in setPAL or setNTSC. */	
	int noOfRasterlines;
	
	//! Number of cycles per rasterline
	/*! Number varies between PAL and NTSC machines. Don't modify this value directly. It is automatically computed 
		in setPAL or setNTSC. */	
	int cpuCyclesPerRasterline;
	
	//! Time between two frames
	int frameDelay;
	
	//! Indicates that we should always run as fast as possible
	bool warpMode;
	
	//! Holds the configuration for the game port.
	/*! The value is determined by the enumeration type INPUT_DEVICES */
	int port[2];
			
	
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

	//! Load snapshot
	bool load(uint8_t **buffer);
	
	//! Save snapshot
	bool save(uint8_t **buffer);

	//! Dump current state into logfile
	void dumpState();
	
	
	// -----------------------------------------------------------------------------------------------
	//                                         Configure
	// -----------------------------------------------------------------------------------------------
	
public:
	
	//! Set PAL mode
	void setPAL();
	
	//! Set NTSC mode
	void setNTSC();
	
	//! Returns true iff warp mode is enabled
	bool getWarpMode();
	
	//! Enable or disable warp mode
	void setWarpMode(bool b);
		

	// -----------------------------------------------------------------------------------------------
	//                                           Control
	// -----------------------------------------------------------------------------------------------

public:
	
	//! Perform a soft reset
	/*! On a real C64, a soft reset is triggered by hitting Runstop and Restore */
	void runstopRestore(); 
	
	//! Returns true iff the virtual C64 is able to run */
	/*! The function checks for missing ROM images etc. */
	bool isRunnable();
	
	//! Power on the virtual C64
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
	
	//! Get a notification message from the message queue
	Message *getMessage();
	
	//! Put a notification message into the message queue
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
		
	//! Returns the number of missing ROM images */
	int numberOfMissingRoms();
	
	//! Missing ROMs are indicated by a 1 in the returned bitmap */
	int getMissingRoms();

	//! Load a ROM image into memory
	/*! The functions can be safely invoked even if the virtual C64 is in "running" state. Execution is automatically
	    suspended before loading and resumed afterwards.
	*/ 
	bool loadRom(const char *filename);

	// -----------------------------------------------------------------------------------------------
	//                                           Timing
	// -----------------------------------------------------------------------------------------------
	
	//! Set interval timer delay
	void setDelay(int d);

	//! Set interval timer delay based on the current value of fps
	void setDelay() { setDelay((uint64_t)(1000000 / fps)); }

	//! Restart timer
	void restartTimer();
	
	//! Synchronize timing
	void synchronizeTiming();
	
	
	// ---------------------------------------------------------------------------------------------
	//                                 Archives (disks, tapes, etc.)
	// ---------------------------------------------------------------------------------------------
	
	//! Assign an archive to the virtual C64
	// void setArchive (Archive *a) { archive = a; }
	
	//! Flush specified item from archive into memory and delete archive
	bool flushArchive(Archive *a, int item);
	
	//! Mount specified archive in the virtual disk drive
	bool mountArchive(Archive *a);

	
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
	inline int getFramesPerSecond() { return fps; }
	
	//! Returns the number of rasterlines per frame
	/*! Number varies between PAL and NTSC machines */	
	inline int getRasterlinesPerFrame() { return noOfRasterlines; }
	
	//! Returns the number of CPU cycles performed per rasterline
	/*! Number varies between PAL and NTSC machines */	
	inline int getCyclesPerRasterline() { return cpuCyclesPerRasterline; }
	
	//! Returns the number of CPU cycles performed per frame
	/*! Number varies between PAL and NTSC machines */	
	inline int getCyclesPerFrame() { return getRasterlinesPerFrame() * getCyclesPerRasterline(); }

	//! Returns the time interval between two frames
	inline int getFrameDelay() { return frameDelay; }

	//! Returns the time interval between two frames
	inline void setFrameDelay(int delay) { frameDelay = delay; }


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

