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

#include "C64Listener.h"
#include "basic.h"

C64Listener::C64Listener()
{
}

void 
C64Listener::drawAction(int *screenBuffer) 
{
	// debug("C64 dummy listener: Frame is complete.\n");
}

void
C64Listener::cpuAction(int state)
{
}

void 
C64Listener::runAction() 
{
	// debug("C64 dummy listener: Entering \"run\" state.\n");
}

void 
C64Listener::haltAction() 
{
	// debug("C64 dummy listener: Entering \"halt\" state.\n");
}

void 
C64Listener::missingRomAction() 
{
	debug("C64 dummy listener: At least one ROM image is missing. Emulator can't run.\n");
}

void
C64Listener::driveAttachedAction(bool connected)
{
	debug("C64 dummy listener: Drive %s\n", connected ? "connected" : "disconnected");
}

void
C64Listener::driveDiscAction(bool inserted)
{
	debug("C64 dummy listener: Floppy disc %s\n", inserted ? "inserted" : "ejected");
}	

void
C64Listener::driveLEDAction(bool on)
{
	debug("C64 dummy listener: Red LED %s\n", on ? "on" : "off");
}

void
C64Listener::driveDataAction(bool transfering)
{
	debug("C64 dummy listener: Data transfer to/from drive: %s\n", transfering ? "started" : "stopped");
}

void
C64Listener::warpAction(bool warping)
{
	debug("C64 dummy listener: Warping %s\n", warping ? "enabled" : "disabled");
}

void
C64Listener::logAction(char *message)
{
	debug("C64 dummy listener: %s\n", message);
}

