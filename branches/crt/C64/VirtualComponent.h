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
				
public:
	//! Constructor
	VirtualComponent();

	//! Destructor
	virtual ~VirtualComponent() { };
	
			
	//! Reset the component to its initial state.
	/*! The functions is called when a hard reset is performed.
	*/
	virtual void reset() = 0; 
	
	//! Load snapshot from a memory buffer
	/*! The function is used for loading a snapshot of the virtual computer
	/seealso C64::loadSnapshot
	/param file file handle of the snapshot file
	*/
	virtual bool load(uint8_t **ptr) = 0;
	
	//! Save internal state from a memory buffer
	/*! The function is used for saving a snapshot of the virtual computer
		/seealso C64::saveSnapshot
		/param file file handle of the snapshot file
	*/
	virtual bool save(uint8_t **ptr) = 0;

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
	
	//! Write 8 bit value to memory in big endian format
	inline void write8(uint8_t **ptr, uint8_t value) { *((*ptr)++) = value; }
	//! Write 16 bit value to memory in big endian format
	inline void write16(uint8_t **ptr, uint16_t value) { write8(ptr, (uint8_t)(value >> 8)); write8(ptr, (uint8_t)value); }
	//! Write 32 bit value to memory in big endian format
	inline void write32(uint8_t **ptr, uint32_t value) { write16(ptr, (uint16_t)(value >> 16)); write16(ptr, (uint16_t)value); }
	//! Write 64 bit value to memory in big endian format
	inline void write64(uint8_t **ptr, uint64_t value) { write32(ptr, (uint32_t)(value >> 32)); write32(ptr, (uint32_t)value); }
	
	//! Read 8 bit value from a memory in big endian format
	inline uint8_t read8(uint8_t **ptr) { return (uint8_t)(*((*ptr)++)); }
	//! Read 16 bit value from a memory in big endian format
	inline uint16_t read16(uint8_t **ptr) { return ((uint16_t)read8(ptr) << 8) | (uint16_t)read8(ptr); }
	//! Read 32 bit value from a memory in big endian format
	inline uint32_t read32(uint8_t **ptr) { return ((uint32_t)read16(ptr) << 16) | (uint32_t)read16(ptr); }
	//! Read 64 bit value from a memory in big endian format
	inline uint64_t read64(uint8_t **ptr) { return ((uint64_t)read32(ptr) << 32) | (uint64_t)read32(ptr); }
	
	//! Print debug message
	void debug(const char *fmt, ...);
};

#endif

