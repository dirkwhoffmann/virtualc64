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

#include "C64.h"

int VirtualComponent::debugLevel = 1;

VirtualComponent::VirtualComponent()
{
	name = "Unnamed component";
	running = false;
	suspendCounter = 0;	
	traceMode = false;
	logfile = NULL;
}

VirtualComponent::~VirtualComponent()
{
	// debug(2, "Terminated\n");
	if (logfile)
		fclose(logfile);
}

void 
VirtualComponent::reset()
{
}

void 
VirtualComponent::run()
{
	running = true;
}

bool 
VirtualComponent::isRunning()
{
	return running;
}

void 
VirtualComponent::halt()
{
	running = false;
}

bool 
VirtualComponent::isHalted()
{
	return !running;
}

void 
VirtualComponent::suspend()
{
	debug(1, "Suspending...\n");
	if (suspendCounter == 0) {
		suspendedState = isRunning();
		halt();
	}
	suspendCounter++;	

	assert(suspendCounter > 0);
}

void 
VirtualComponent::resume()
{
	debug(1, "Resuming...\n");
	suspendCounter--;
	if (suspendCounter == 0 && suspendedState == true)
		run();
	
	assert(suspendCounter >= 0);
}

void 
VirtualComponent::dumpState()
{
}

void
VirtualComponent::debug(const char *fmt, ...)
{
	char buf[256];
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap); 
	va_end(ap);
	fprintf(logfile ? logfile : stderr, "%s: %s", name, buf);
}

void
VirtualComponent::debug(int level, const char *fmt, ...)
{
	if (level > debugLevel) 
		return;

	char buf[256];
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap); 
	va_end(ap);
	fprintf(logfile ? logfile : stderr, "%s: %s", name, buf);
}

void format_string(char *fmt,va_list argptr );

void
VirtualComponent::warn(const char *fmt, ...)
{
	char buf[256];
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap); 
	va_end(ap);
	fprintf(logfile ? logfile : stderr, "%s: WARNING: %s", name, buf);
}

void
VirtualComponent::panic(const char *fmt, ...)
{
	char buf[256];
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap); 
	va_end(ap);
	fprintf(logfile ? logfile : stderr, "%s: PANIC: %s", name, buf);
	
	assert(0);
}

