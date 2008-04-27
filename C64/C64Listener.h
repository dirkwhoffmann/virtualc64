/*
 * (C) 2006-2008 Dirk W. Hoffmann. All rights reserved.
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
		
public:	
	//! Notifies the listener that the emulator won't run due to missing ROM image.
	virtual void missingRomAction(void) = 0;
	//! Notfies the listener that the virtual C64 has entered the "running" state.
	virtual void runAction(void) = 0;
	//! Notfies the listener that the virtual C64 has entered the "halted" state.
	virtual void haltAction(void) = 0;
	//! Notifies the listener about a completed frame
	virtual void drawAction(int *screenBuffer) = 0;
	//! Notifies the listener about the current CPU status
	virtual void cpuAction(int state) = 0;
	//! Notifies the listener about the drive connection status
	virtual void driveAttachedAction(bool connected) = 0;
	//! Notifies the listener about inserted or ejected floppy disks
	virtual void driveDiscAction(bool inserted) = 0;
	//! Notifies the listener about the red led signal
	virtual void driveLEDAction(bool on) = 0;
	//! Notifies the listener about transferred disc data
	virtual void driveDataAction(bool transfering) = 0;
	//! Notifies the listener about the drive motor status
	virtual void driveMotorAction(bool rotating) = 0;	
	//! Notifies the listener about warp mode
	virtual void warpAction(bool warping) = 0;	
	//! Send log information to the listener
	virtual void logAction(char *message) = 0;	
};

#endif
