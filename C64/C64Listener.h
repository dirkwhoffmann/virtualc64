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

//! Proxy object between the virtual C64 and the user interface
class C64Listener {
	
	// bool logging;
	
public:	
	//! Constructor
	C64Listener();
	
	//! Notifies the listener that the emulator won't run due to missing ROM image.
	virtual void missingRomAction(void);
	//! Notfies the listener that the virtual C64 has entered the "running" state.
	virtual void runAction(void);
	//! Notfies the listener that the virtual C64 has entered the "halted" state.
	virtual void haltAction(void);
	//! Notifies the listener about a completed frame
	virtual void drawAction(int *screenBuffer);
	//! Notifies the listener about the current CPU status
	virtual void cpuAction(int state);
	//! Notifies the listener about the drive connection status
	virtual void driveAttachedAction(bool connected);
	//! Notifies the listener about inserted or ejected floppy disks
	virtual void driveDiscAction(bool inserted);
	//! Notifies the listener about the red led signal
	virtual void driveLEDAction(bool on);
	//! Notifies the listener about transferred disc data
	virtual void driveDataAction(bool transfering);
	//! Notifies the listener about warp mode
	virtual void warpAction(bool warping);	
	//! Send log information to the listener
	virtual void logAction(char *message);	
};

#endif
