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

#include "VirtualComponent.h"

VirtualComponent::VirtualComponent()
{
	running = false;
	suspendCounter = 0;	
	traceMode = false;
	listener = NULL;
	resetTimer();
}

void VirtualComponent::reset()
{
}

#if 0
bool
VirtualComponent::load(FILE *file)
{
}

bool
VirtualComponent::save(FILE *file)
{
}
#endif

void VirtualComponent::run()
{
	running = true;
}

bool VirtualComponent::isRunning()
{
	return running;
}

void VirtualComponent::halt()
{
	running = false;
}

bool VirtualComponent::isHalted()
{
	return !running;
}

void VirtualComponent::suspend()
{
	debug("Suspending...\n");
	if (suspendCounter == 0) {
		suspendedState = isRunning();
		halt();
	}
	suspendCounter++;	

	assert(suspendCounter > 0);
}

void VirtualComponent::resume()
{
	debug("Resuming...\n");
	suspendCounter--;
	if (suspendCounter == 0 && suspendedState == true)
		run();
	
	assert(suspendCounter >= 0);
};



void VirtualComponent::dumpState()
{
}
