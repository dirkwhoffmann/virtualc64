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


C64Listener::C64Listener()
{
}

void 
C64Listener::drawAction(int *screenBuffer) 
{
	// debug("C64 dummy listener: Frame is complete.\n");
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
C64Listener::okAction() 
{
	// debug("C64 dummy listener: Error state cleared.\n");
}

void 
C64Listener::breakpointAction() 
{
	// debug("C64 dummy listener: Breakpoint reached.\n");
}

void 
C64Listener::watchpointAction() 
{
	// debug("C64 dummy listener: Watchpoint reached.\n");
}

void 
C64Listener::illegalInstructionAction() 
{
	// debug("C64 dummy listener: Halted due to an illegal instruction.\n");
}

void 
C64Listener::missingRomAction() 
{
	debug("C64 dummy listener: At least one ROM image is missing. Emulator can't run.\n");
}

void 
C64Listener::connectDriveAction(void)
{
	debug("C64 dummy listener: Drive connected.\n");
}

void 
C64Listener::insertDiskAction(void)
{
	debug("C64 dummy listener: Disk inserted.\n");
}

void
C64Listener::ejectDiskAction(void)
{
	debug("C64 dummy listener: Disk ejected.\n");
}

void 
C64Listener::disconnectDriveAction(void)
{
	debug("C64 dummy listener: Drive disconnected.\n");
}

void
C64Listener::startDiskAction(void)
{
	debug("C64 dummy listener: Data transfer started.\n");
}
	
void 
C64Listener::stopDiskAction(void)	
{
	debug("C64 dummy listener: Data transfer stopped.\n");
}

void
C64Listener::startWarpAction(void)
{
	debug("C64 dummy listener: Entered warp mode.\n");
}

void 
C64Listener::stopWarpAction(void)	
{
	debug("C64 dummy listener: Exited warp mode.\n");
}

#if 0
void
C64Listener::startTraceAction(void)
{
	debug("C64 dummy listener: Start tracing.\n");
}

void 
C64Listener::stopTraceAction(void)	
{
	debug("C64 dummy listener: Stop tracing.\n");
}
#endif

void
C64Listener::logAction(char *message)
{
	debug("C64 dummy listener: %s\n", message);
}

