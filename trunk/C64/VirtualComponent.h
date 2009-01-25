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

// Last review: 25.7.06

#ifndef _VIRTUAL_COMPONENT_INC
#define _VIRTUAL_COMPONENT_INC

#include "basic.h"
#include "C64Listener.h"

//! Common functionality of all virtual computer components
/*! This class defines the base functionality of all virtual components.
    The class comprises functions for resetting, suspending and resuming the component,
	as well as functions for loading and saving state (snapshots). 
*/
class VirtualComponent {

private:
	//! Returns true iff the component is currently active.
	/*! All virtual components can be in two states. They can either be running or halted.
	    During normal operation, all components are running. If an error occurrs, or if the
	    user requests the virtual machine to halt in the debugger, the components will enter
	    the "halted" state. 
	*/	
	bool running;

	//! True iff tracing is enabled
	/*! In trace mode, all components are requested to dump debug informatik perodically.
		Only a few components will react to this flag.
	*/
	bool traceMode;
	
	//! The original state before the first call of suspend()
	bool suspendedState;

	//! Number of times the component is suspended  
	/*! The value is equal to the number of suspend calls minus the number of resume calls 
	*/
	int suspendCounter;
			
	//! Reference to the connected C64 listener.
	C64Listener *listener;
	
public:
	//! Constructor
	VirtualComponent();

	//! Destructor
	virtual ~VirtualComponent() { };

	//! Get listener
	C64Listener *getListener() { assert(listener != NULL); return listener; }
	
	//! Set listener
	/*! The listener can be changed multiple times during program execution */
	void setListener(C64Listener *l) { listener = l; } 
		
	//! Reset the component to its initial state.
	/*! The functions is called when a hard reset is performed.
	*/
	virtual void reset() = 0; 
	
	//! Load internal state from a file
	/*! The function is used for loading a snapshot of the virtual computer
	/seealso C64::loadSnapshot
	/param file file handle of the snapshot file
	*/
	virtual bool load(FILE *file) = 0;
	
	//! Save internal state to a file
	/*! The function is used for saving a snapshot of the virtual computer
		/seealso C64::saveSnapshot
		/param file file handle of the snapshot file
	*/
	virtual bool save(FILE *file) = 0;

	//! Print info about the internal state
	/*! This functions is intended for debugging purposes only. Any derived component should override
	 this method and print out some useful debugging information. 
	 */ 
	virtual void dumpState();
	
	//! Start component
	/*! The function is called when the virtual computer is requested to run
		Some components such as the CPU require asynchronously running threads and will start them here.
		Most of the other components are of a static nature and won't implement additional functionality.
	*/
	virtual void run();

	//! Returns true iff the component is running		
	virtual bool isRunning();
	
	//! Stop component
	/*! The function is called when the virtual computer is requested to freeze
		For example, the CPU will ask its asynchronously running thread to halt.
		Most of the other components are of a static nature and won't implement additional functionality.
	*/	
	virtual void halt();

	//! Returns true iff the component is halted
	virtual bool isHalted();

	// suspend : Current state is saved and CPU halted
	// resume  : Restore saved state
	//           Note: suspend()/resume() calls can be nested, i.e., 
	//           n x suspend() requires n x resume() to make the CPU flying again

	//! Suspend component
	/*! The suspend mechanism is a nested run/halt mechanism. First of all, it works like halt,
		i.e., the component freezes. In contrast to halt, the suspend function remembers whether
		the component was already halted or running. When the resume function is invoked, the original
		running state is reestablished. In other words: If your component is currently running and you
		suspend it 10 times, you'll have to resume it 10 times to make it run again.
	*/	
	void suspend();
	
	//! Resume component
	/*! This functions concludes a suspend operation.
		/seealso suspend. 
	*/
	void resume();

	//! Returns true iff trace mode is enabled
	inline bool tracingEnabled() { return traceMode; }

	//! Enable or disable trace mode
	inline void setTraceMode(bool b) { traceMode = b; }
	
	// Helper functions for reading and writing data
	
	//! Write up to four bytes to file
	void write(FILE *file, uint64_t value, int count);

	//! Read up to four bytes from a file
	uint64_t read(FILE *file, int count);

	//! Write 8 bit value to a file
	inline void write8(FILE *file, uint8_t value) { write(file, (uint64_t)value, sizeof(uint8_t)); }
	//! Write 16 bit value to a file
	inline void write16(FILE *file, uint16_t value) { write(file, (uint64_t)value, sizeof(uint16_t)); }
	//! Write 32 bit value to a file
	inline void write32(FILE *file, uint32_t value) { write(file, (uint64_t)value, sizeof(uint32_t)); }
	//! Write 64 bit value to a file
	inline void write64(FILE *file, uint64_t value) { write(file, (uint64_t)value, sizeof(uint64_t)); }
	
	//! Read 8 bit value from a file
	inline uint8_t read8(FILE *file) { return (uint8_t)read(file, sizeof(uint8_t)); }
	//! Read 16 bit value from a file
	inline uint16_t read16(FILE *file) { return (uint16_t)read(file, sizeof(uint16_t)); }
	//! Read 32 bit value from a file
	inline uint32_t read32(FILE *file) { return (uint32_t)read(file, sizeof(uint32_t)); }
	//! Read 64 bit value from a file
	inline uint64_t read64(FILE *file) { return (uint64_t)read(file, sizeof(uint64_t)); }
	
	//! Print debug message
	void debug(char *fmt, ...);
};

#endif

