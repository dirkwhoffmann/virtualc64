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

#ifndef _C64_LISTENER_INC
#define _C64_LISTENER_INC

#include "basic.h"

//! Proxy object between the virtual C64 and the user interface
class C64Listener {
	
	// bool logging;
	
public:	
	//! Constructor
	C64Listener();
	
	//! Notifies the listener about a completed frame
	virtual void drawAction(int *screenBuffer);
	//! Notfies the listener that the virtual C64 has entered the "running" state.
	virtual void runAction(void);
	//! Notfies the listener that the virtual C64 has entered the "halted" state.
	virtual void haltAction(void);
	//! Notfies the listener that the error state of the virtual CPU is cleared.
	virtual void okAction(void);		
	//! Notfies the listener that the virtual C64 has halted due to a breakpoint.
	virtual void breakpointAction(void);	
	//! Notfies the listener that the virtual C64 has halted due to a watchpoint.
	virtual void watchpointAction(void);	
	//! Notfies the listener that the virtual C64 has halted due to an illegal instruction.
	virtual void illegalInstructionAction(void);	
	//! Notifies the listener that the emulator won't run due to missing ROM image.
	virtual void missingRomAction(void);
	//! Notifies the listener that a drive has been connected to the IEC bus.
	virtual void connectDriveAction(void);
	//! Notifies the listener that a disk has been inserted into the virtual drive.
	virtual void insertDiskAction(void);
	//! Notifies the listener that a disk has been ejected from the virtual drive.
	virtual void ejectDiskAction(void);
	//! Notifies the listener that a drive has been disconnected to the IEC bus.
	virtual void disconnectDriveAction(void);
	//! Notifies the listener that the virtual disk startet to transfer data.
	virtual void startDiskAction(void);
	//! Notifies the listener that the virtual disk stopped to transfer data.
	virtual void stopDiskAction(void);	
	//! Notifies the listener about the file transfer progress.
	//virtual void driveProgressAction(int percentage_completed);	

	//! Notifies the listener that the cpu has entered warp mode
	virtual void startWarpAction(void);
	//! Notifies the listener that the cpu has exited warp mode
	virtual void stopWarpAction(void);	
	//! Notifies the listener that the cpu started sending trace information to the debug console
	//virtual void startTraceAction(void);
	//! Notifies the listener that the cpu stopped sending trace information to the debug console
	//virtual void stopTraceAction(void);	
	//! Send log information to the listener
	virtual void logAction(char *message);	
};

#endif
