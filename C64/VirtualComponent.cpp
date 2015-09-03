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

int VirtualComponent::debugLevel = DEBUG_LEVEL;

VirtualComponent::VirtualComponent()
{    
	name = "Unnamed component";
	running = false;
	suspendCounter = 0;	
	traceMode = false;
	logfile = NULL;
    snapshotItems = NULL;
}

VirtualComponent::~VirtualComponent()
{
	// debug(2, "Terminated\n");
    
    if (snapshotItems)
        delete [] snapshotItems;
    
    if (logfile)
		fclose(logfile);
}

void
VirtualComponent::reset(C64 *c64)
{
    debug(2, "    Resetting ...\n");
    
    // Establish binding to top-level object
    this->c64 = c64;
}

void
VirtualComponent::ping()
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
	debug(2, "Suspending...\n");
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
	debug(2, "Resuming...\n");
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
VirtualComponent::msg(const char *fmt, ...)
{
	char buf[256];
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap); 
	va_end(ap);
	fprintf(logfile ? logfile : stderr, "%s", buf);
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

// ---------------------------------------------------------------------------------------------
//                                      Snapshots
// ---------------------------------------------------------------------------------------------

void
VirtualComponent::registerSnapshotItems(SnapshotItem *items, unsigned length) {
    
    assert(items != NULL);
    assert(length % sizeof(SnapshotItem) == 0);
    
    unsigned i, numItems = length / sizeof(SnapshotItem);
    
    // Allocate new array on heap and copy array data
    snapshotItems = new SnapshotItem[numItems];
    std::copy(items, items + numItems, &snapshotItems[0]);
    
    // Determine size of snapshot on disk
    for (i = snapshotSize = 0; snapshotItems[i].data != NULL; i++)
        snapshotSize += snapshotItems[i].size;
}

void
VirtualComponent::loadFromBuffer(uint8_t **buffer)
{
    if (snapshotItems == NULL)
        return;

    uint8_t *old = *buffer;

    debug(3, "    Loading internal state (%d bytes) ...\n", VirtualComponent::stateSize());
    
    for (unsigned i = 0; snapshotItems[i].data != NULL; i++) {
        
        switch (snapshotItems[i].size) {
            case 1: *(uint8_t *)snapshotItems[i].data = read8(buffer); break;
            case 2: *(uint16_t *)snapshotItems[i].data = read16(buffer); break;
            case 4: *(uint32_t *)snapshotItems[i].data = read32(buffer); break;
            case 8: *(uint64_t *)snapshotItems[i].data = read64(buffer); break;
            default: readBlock(buffer, (uint8_t *)snapshotItems[i].data, snapshotItems[i].size);
        }
    }
    assert(*buffer - old == VirtualComponent::stateSize());
}

void
VirtualComponent::saveToBuffer(uint8_t **buffer)
{
    if (snapshotItems == NULL)
        return;
    
    uint8_t *old = *buffer;

    debug(3, "    Saving internal state (%d bytes) ...\n", VirtualComponent::stateSize());

    for (unsigned i = 0; snapshotItems[i].data != NULL; i++) {
        
        switch (snapshotItems[i].size) {
            case 1: write8(buffer, *(uint8_t *)snapshotItems[i].data); break;
            case 2: write16(buffer, *(uint16_t *)snapshotItems[i].data); break;
            case 4: write32(buffer, *(uint32_t *)snapshotItems[i].data); break;
            case 8: write64(buffer, *(uint64_t *)snapshotItems[i].data); break;
            default: writeBlock(buffer, (uint8_t *)snapshotItems[i].data, snapshotItems[i].size);
        }
    }
    assert(*buffer - old == VirtualComponent::stateSize());
}