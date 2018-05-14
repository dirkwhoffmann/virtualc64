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

int debugcnt = 0;
int debugirq = 0;

//
// Execution thread
//

void 
threadCleanup(void* thisC64)
{
	assert(thisC64 != NULL);
	
	C64 *c64 = (C64 *)thisC64;
	c64->threadCleanup();

    c64->sid.halt();
	c64->debug(1, "Execution thread terminated\n");
	c64->putMessage(MSG_HALT);
}

void 
*runThread(void *thisC64) {
		
	assert(thisC64 != NULL);
	
	C64 *c64 = (C64 *)thisC64;
	c64->debug(1, "Execution thread started\n");
	c64->putMessage(MSG_RUN);
	
    // Configure thread properties...
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
	pthread_cleanup_push(threadCleanup, thisC64);
	
	// Prepare to run...
	c64->cpu.clearErrorState();
	c64->floppy.cpu.clearErrorState();
	c64->restartTimer();
    
	while (1) {		
		if (!c64->executeOneLine())
			break;		

		if (c64->getRasterline() == 0 && c64->getFrame() % 8 == 0)
			pthread_testcancel(); // Check if thread was requested to terminate
	}
	
	pthread_cleanup_pop(1);
	pthread_exit(NULL);	
}


//
// Class methods
//

C64::C64()
{
	setDescription("C64");
	debug("Creating virtual C64[%p]\n", this);

	p = NULL;    
    warp = false;
    alwaysWarp = false;
    warpLoad = false;
	
    // Register sub components
    VirtualComponent *subcomponents[] = {
        
        &cpu,
        &processorPort,
        &mem,
        &vic,
        &sid,
        &cia1, &cia2,
        &iec,
        &expansionport,
        &floppy,
        &datasette,
        &mouse1351,
        &neosMouse,
        &keyboard,
        &port1,
        &port2,
        NULL };
    
    registerSubComponents(subcomponents, sizeof(subcomponents));
    setC64(this);
    
    // Register snapshot items
    SnapshotItem items[] = {
 
        { &wakeUpCycleCIA1, sizeof(wakeUpCycleCIA1),    CLEAR_ON_RESET },
        { &idleCounterCIA1, sizeof(idleCounterCIA1),    CLEAR_ON_RESET },
        { &wakeUpCycleCIA2, sizeof(wakeUpCycleCIA2),    CLEAR_ON_RESET },
        { &idleCounterCIA2, sizeof(idleCounterCIA2),    CLEAR_ON_RESET },
        { &warp,            sizeof(warp),               CLEAR_ON_RESET },
        { &alwaysWarp,      sizeof(alwaysWarp),         CLEAR_ON_RESET },
        { &warpLoad,        sizeof(warpLoad),           KEEP_ON_RESET },
        { &cycle,           sizeof(cycle),              CLEAR_ON_RESET },
        { &frame,           sizeof(frame),              CLEAR_ON_RESET },
        { &rasterline,      sizeof(rasterline),         CLEAR_ON_RESET },
        { &rasterlineCycle, sizeof(rasterlineCycle),    CLEAR_ON_RESET },
        { &ultimax,         sizeof(ultimax),            CLEAR_ON_RESET },
        { NULL,             0,                          0 }};
    
    registerSnapshotItems(items, sizeof(items));

    // Setup references
    cpu.mem = &mem;
    mem.cpu = &cpu;
    // cia1.tod.cia = &cia1;
    // cia2.tod.cia = &cia2;
    floppy.cpu.mem = &c64->floppy.mem;
    floppy.mem.cpu = &c64->floppy.cpu;
    floppy.mem.iec = &c64->iec;
    floppy.mem.floppy = &c64->floppy;
    floppy.iec = &c64->iec;
    
    // Set initial hardware configuration
    mouseModel = MOUSE1351;
    mousePort = 0; 
    setPAL();
			
    // Initialize mach timer info
    mach_timebase_info(&timebase);

	// Initialize snapshot ringbuffers
    for (unsigned i = 0; i < MAX_AUTO_SAVED_SNAPSHOTS; i++) {
		autoSavedSnapshots[i] = new Snapshot();
    }
    for (unsigned i = 0; i < MAX_USER_SAVED_SNAPSHOTS; i++) {
        userSavedSnapshots[i] = new Snapshot();
    }
    autoSaveSnapshots = true;
    autoSaveInterval = 3;

    reset();
}

C64::~C64()
{
    debug(1, "Destroying virtual C64[%p]\n", this);
	halt();
}

void
C64::reset()
{
    debug(1, "Resetting virtual C64[%p]\n", this);
    
	// suspend();

    // Reset all sub components
    VirtualComponent::reset();
    
    // Make CPU ready to go
    cpu.initPC();
    
    // Initialize processor port data direction register and processor port
    mem.poke(0x0000, 0x2F);  // Data direction
    mem.poke(0x0001, 0x1F);  // IO port, set default memory layout
    
    // Make memory ready to go
    // mem.updatePeekPokeLookupTables();
    
	rasterlineCycle = 1;
    nanoTargetTime = 0UL;
    ping();
	// resume();
}

void C64::ping()
{
    debug (1, "Pinging virtual C64[%p]\n", this);

    VirtualComponent::ping();
    putMessage(warp ? MSG_WARP_ON : MSG_WARP_OFF);
    putMessage(alwaysWarp ? MSG_ALWAYS_WARP_ON : MSG_ALWAYS_WARP_OFF);
}
	
void 
C64::dumpState() {
	msg("C64:\n");
	msg("----\n\n");
	msg("            Machine type : %s\n", isPAL() ? "PAL" : "NTSC");
	msg("       Frames per second : %d\n", vic.getFramesPerSecond());
	msg("   Rasterlines per frame : %d\n", vic.getRasterlinesPerFrame());
	msg("   Cycles per rasterline : %d\n", vic.getCyclesPerRasterline());
	msg("           Current cycle : %llu\n", cycle);
	msg("           Current frame : %d\n", frame);
	msg("      Current rasterline : %d\n", rasterline);
	msg("Current rasterline cycle : %d\n", rasterlineCycle);
    msg("            Ultimax mode : %s\n", getUltimax() ? "YES" : "NO");
	msg("\n");
}


//
// Configuring the emulator
//

void
C64::setPAL()
{
    debug(2, "C64::setPAL\n");

    suspend();
    vic.setChipModel(MOS6569_PAL);
	sid.setPAL();
	resume();
}

void 
C64::setNTSC()
{
    debug(2, "C64::setNTSC\n");

	suspend();
    vic.setChipModel(MOS6567_NTSC);
	sid.setNTSC();
	resume();
}


//
// Running the emulator
//

void
C64::powerUp()
{
    suspend();
    reset();
    resume();
    run();
}

void
C64::run()
{
    if (isHalted()) {
        
        // Check for ROM images
        if (!isRunnable()) {
            putMessage(MSG_ROM_MISSING);
            return;
        }
        
        // Power up sub components
        sid.run();
        
        // Start execution thread
        pthread_create(&p, NULL, runThread, (void *)this);
    }
}

void
C64::threadCleanup()
{
    p = NULL;
    debug(1, "Execution thread cleanup\n");
}

bool
C64::isRunnable()
{
    return mem.basicRomIsLoaded() && mem.charRomIsLoaded() && mem.kernalRomIsLoaded() && floppy.mem.romIsLoaded();
}

bool
C64::isRunning()
{
    return p != NULL;
}

void
C64::halt()
{
    if (isRunning()) {
        
        // Cancel execution thread
        pthread_cancel(p);
        // Wait until thread terminates
        pthread_join(p, NULL);
        // Finish the current command (to reach a clean state)
        step();
    }
}

bool
C64::isHalted()
{
    return p == NULL;
}

void
C64::step()
{
    // Clear error states
    cpu.clearErrorState();
    floppy.cpu.clearErrorState();
    
    // Execute next command
    do {
        executeOneCycle();
    } while (!cpu.atBeginningOfNewCommand());
    
    // We are now at cycle 0 of the next command
    // Execute one more cycle (and stop in cycle 1)
    executeOneCycle();
}

// From Wolfgang Lorenz: Clock.txt
//
// o2 high phase |                  o2 low phase
//     .-----.   |   .----------------.     .------.     .------.
//     |     |   |   | 1. Next Clock  |     |      |     |      |
// .-> | CPU | ----> | 2. Fire Timers | --> | CIA1 | --> | CIA2 | -.
// |   |     |   |   | 3. VIC         |     |      |     |      |  |
// |   '-----'   |   '----------------'     '------'     '------'  |
// '---------------------------------------------------------------'

#define EXECUTE \
if (cycle >= wakeUpCycleCIA1) cia1.executeOneCycle(); else idleCounterCIA1++; \
if (cycle >= wakeUpCycleCIA2) cia2.executeOneCycle(); else idleCounterCIA2++; \
if (!cpu.executeOneCycle()) result = false; \
if (!floppy.executeOneCycle()) result = false; \
datasette.execute(); \
cycle++; \
rasterlineCycle++;

bool
C64::executeOneCycle()
{
    bool result = true; // Don't break execution
    
    switch(rasterlineCycle) {
        case 1:
            beginOfRasterline();
            vic.cycle1();
            EXECUTE
            break;
        case 2:
            vic.cycle2();
            EXECUTE
            break;
        case 3:
            vic.cycle3();
            EXECUTE
            break;
        case 4:
            vic.cycle4();
            EXECUTE
            break;
        case 5:
            vic.cycle5();
            EXECUTE
            break;
        case 6:
            vic.cycle6();
            EXECUTE
            break;
        case 7:
            vic.cycle7();
            EXECUTE
            break;
        case 8:
            vic.cycle8();
            EXECUTE
            break;
        case 9:
            vic.cycle9();
            EXECUTE
            break;
        case 10:
            vic.cycle10();
            EXECUTE
            break;
        case 11:
            vic.cycle11();
            EXECUTE
            break;
        case 12:
            vic.cycle12();
            EXECUTE
            break;
        case 13:
            vic.cycle13();
            EXECUTE
            break;
        case 14:
            vic.cycle14();
            EXECUTE
            break;
        case 15:
            vic.cycle15();
            EXECUTE
            break;
        case 16:
            vic.cycle16();
            EXECUTE
            break;
        case 17:
            vic.cycle17();
            EXECUTE
            break;
        case 18:
            vic.cycle18();
            EXECUTE
            break;
        case 19: case 20: case 21: case 22: case 23: case 24: case 25: case 26:
        case 27: case 28: case 29: case 30: case 31: case 32: case 33: case 34:
        case 35: case 36: case 37: case 38: case 39: case 40: case 41: case 42:
        case 43: case 44: case 45: case 46: case 47: case 48: case 49: case 50:
        case 51: case 52: case 53: case 54:
            vic.cycle19to54();
            EXECUTE
            break;
        case 55:
            vic.cycle55();
            EXECUTE
            break;
        case 56:
            vic.cycle56();
            EXECUTE
            break;
        case 57: 
            vic.cycle57();
            EXECUTE
            break;
        case 58: 
            vic.cycle58();
            EXECUTE
            break;
        case 59: 
            vic.cycle59();
            EXECUTE
            break;
        case 60: 
            vic.cycle60();
            EXECUTE
            break;
        case 61: 
            vic.cycle61();
            EXECUTE
            break;
        case 62: 
            vic.cycle62();
            EXECUTE
            break;
        case 63: 
            vic.cycle63();
            EXECUTE
            
            // This is the last cycle on PAL machines
            if (vic.getCyclesPerRasterline() == 63) {
                endOfRasterline();
            }			
            break;
        case 64: 
            vic.cycle64();
            EXECUTE
            break;
        case 65: 
            vic.cycle65();
            EXECUTE
            endOfRasterline();
            break;
            
        default:
            // can't reach
            assert(false);
            return false;
    }
    
    return result;
}

bool
C64::executeOneLine()
{
    uint8_t lastCycle = vic.getCyclesPerRasterline();
    for (unsigned i = rasterlineCycle; i <= lastCycle; i++) {
        if (!executeOneCycle())
            return false;
    }
    return true;
}

void
C64::beginOfRasterline()
{
    // First cycle of rasterline
    if (rasterline == 0) {
        vic.beginFrame();
    }
    vic.beginRasterline(rasterline);
}

void
C64::endOfRasterline()
{
    vic.endRasterline();
    rasterlineCycle = 1;
    rasterline++;
    
    if (rasterline >= vic.getRasterlinesPerFrame()) {
        rasterline = 0;
        endOfFrame();
    }
}

void
C64::endOfFrame()
{
    frame++;
    vic.endFrame();
    
    // Increment time of day clocks every tenth of a second
    cia1.incrementTOD();
    cia2.incrementTOD();
    
    // Execute remaining SID cycles
    sid.executeUntil(cycle);
    
    // Execute other components
    mouse1351.execute();
    neosMouse.execute();
    iec.execute();
    expansionport.execute();
    
    // Take a snapshot once in a while
    if (autoSaveSnapshots && frame % (vic.getFramesPerSecond() * autoSaveInterval) == 0) {
        takeAutoSnapshot();
    }
    
    // Count some sheep (zzzzzz) ...
    if (!getWarp()) {
            synchronizeTiming();
    }
}

//
//! @functiongroup Managing the execution thread
//

void
C64::setWarp(bool b)
{
    if (warp == b)
        return;
    
    warp = b;

    // Warping has the unavoidable drawback that audio playback gets out of sync.
    // To cope with this issue, we silence SID during warp mode and fade in smoothly
    // after warping has ended.
    
    if (warp) {
        // Quickly fade out SID
        sid.rampDown();
        
    } else {
        // Smoothly fade in SID
        sid.rampUp();
        restartTimer();
    }
    
    putMessage(b ? MSG_WARP_ON : MSG_WARP_OFF);
}

void
C64::setAlwaysWarp(bool b)
{
    if (alwaysWarp == b)
        return;
    
    if (alwaysWarp != b) {
        alwaysWarp = b;
        setWarp(b);
        putMessage(b ? MSG_ALWAYS_WARP_ON : MSG_ALWAYS_WARP_OFF);
    }
}

void
C64::setWarpLoad(bool b)
{
	warpLoad = b;
}

void
C64::restartTimer()
{
    uint64_t kernelNow = mach_absolute_time();
    uint64_t nanoNow = abs_to_nanos(kernelNow);
    
    nanoTargetTime = nanoNow + vic.getFrameDelay();
}

void
C64::synchronizeTiming()
{
    const uint64_t earlyWakeup = 1500000; /* 1.5 milliseconds */
    
    // Convert usec into kernel unit
    int64_t kernelTargetTime = nanos_to_abs(nanoTargetTime);
    
    // Check how long we're supposed to sleep
    int64_t timediff = kernelTargetTime - (int64_t)mach_absolute_time();
    if (timediff > 200000000 /* 0.2 sec */) {
        
        // The emulator seems to be out of sync, so we better reset the synchronization timer
        
        debug(2, "Emulator lost synchronization (%lld). Restarting synchronization timer.\n", timediff);
        restartTimer();
        // return;
    }
    
    // Sleep and update target timer
    // debug(2, "%p Sleeping for %lld\n", this, kernelTargetTime - mach_absolute_time());
    int64_t jitter = sleepUntil(kernelTargetTime, earlyWakeup);
    nanoTargetTime += vic.getFrameDelay();
    
    // debug(2, "Jitter = %d", jitter);
    if (jitter > 1000000000 /* 1 sec */) {
        
        // The emulator did not keep up with the real time clock. Instead of
        // running behind for a long time, we reset the synchronization timer
        
        debug(2, "Jitter exceeds limit (%lld). Restarting synchronization timer.\n", jitter);
        restartTimer();
    }
}


//
//! @functiongroup Loading ROM images
//

bool
C64::loadRom(const char *filename)
{
    bool result = false;
    
    debug(2, "Trying to load ROM image %s\n", filename);
    
    suspend();
    bool wasRunnable = isRunnable();
    
    if (C64Memory::isBasicRom(filename)) {
        result = mem.loadBasicRom(filename);
        if (result) putMessage(MSG_BASIC_ROM_LOADED);
    }
    
    if (C64Memory::isCharRom(filename)) {
        result = mem.loadCharRom(filename);
        if (result) putMessage(MSG_CHAR_ROM_LOADED);
    }
    
    if (C64Memory::isKernalRom(filename)) {
        result = mem.loadKernalRom(filename);
        if (result) putMessage(MSG_KERNAL_ROM_LOADED);
    }
    
    if (VC1541Memory::is1541Rom(filename)) {
        result = floppy.mem.loadRom(filename);
        if (result) putMessage(MSG_VC1541_ROM_LOADED);
    }
    
    if (result) {
        debug(2, "ROM image %s loaded successfully\n", filename);
    } else {
        warn("FAILED to load ROM image %s\n", filename);
    }
    
    if (!wasRunnable && isRunnable()) {
        
        // Reset emulator and let the GUI know that the emulator is ready to run.
        reset();
        putMessage(MSG_READY_TO_RUN);
    }
    resume();
    
    return result;
}


//
//! @functiongroup Loading and saving snapshots
//

void C64::loadFromSnapshotUnsafe(Snapshot *snapshot)
{    
    uint8_t *ptr;
    
    if (snapshot && (ptr = snapshot->getData())) {
        loadFromBuffer(&ptr);
        keyboard.releaseAll(); // Avoid constantly pressed keys
        ping();
    }
}

void
C64::loadFromSnapshotSafe(Snapshot *snapshot)
{
    debug(1, "C64::loadFromSnapshotSafe\n");

    suspend();
    loadFromSnapshotUnsafe(snapshot);
    resume();
}

bool
C64::restoreAutoSnapshot(unsigned nr)
{
    if (autoSnapshot(nr)->isEmpty())
        return false;
    
    loadFromSnapshotSafe(autoSnapshot(nr));
    return true;
}

bool
C64::restoreLatestAutoSnapshot()
{
    if (!restoreAutoSnapshot(0))
        return false;
    
    deleteAutoSnapshot(0);
    return true;
}

bool
C64::restoreUserSnapshot(unsigned nr)
{
    if (userSnapshot(nr)->isEmpty())
        return false;
    
    loadFromSnapshotSafe(userSnapshot(nr));
    return true;
}

bool
C64::restoreLatestUserSnapshot()
{
    return restoreUserSnapshot(0);
}

void
C64::saveToSnapshotUnsafe(Snapshot *snapshot)
{
    if (snapshot == NULL)
        return;
    
    snapshot->setCapacity(stateSize());
    snapshot->setTimestamp(time(NULL));
    snapshot->takeScreenshot((uint32_t *)vic.screenBuffer(), isPAL());

    uint8_t *ptr = snapshot->getData();
    saveToBuffer(&ptr);
}

void
C64::saveToSnapshotSafe(Snapshot *snapshot)
{
    debug(1, "C64::saveToSnapshotSafe\n");
 
    suspend();
    saveToSnapshotUnsafe(snapshot);
    resume();
}

Snapshot *
C64::takeSnapshotUnsafe()
{
    Snapshot *snapshot = new Snapshot;
    saveToSnapshotUnsafe(snapshot);
    return snapshot;
}

Snapshot *
C64::takeSnapshotSafe()
{
    Snapshot *snapshot;
    
    suspend();
    snapshot = takeSnapshotUnsafe();
    resume();
    
    return snapshot;
}

unsigned
C64::numAutoSnapshots()
{
    unsigned result;
    
    for (result = 0; result < MAX_AUTO_SAVED_SNAPSHOTS; result++) {
        if (autoSavedSnapshots[result]->isEmpty())
            break;
    }
    
    return result;
}

void
C64::takeAutoSnapshot()
{
    Snapshot *last = autoSavedSnapshots[MAX_AUTO_SAVED_SNAPSHOTS - 1];
    
    // Shuffle slots
    for (unsigned i = MAX_AUTO_SAVED_SNAPSHOTS - 1; i > 0; i--)
        autoSavedSnapshots[i] = autoSavedSnapshots[i - 1];
    autoSavedSnapshots[0] = last;
    
    // Save state
    saveToSnapshotUnsafe(autoSavedSnapshots[0]);
    putMessage(MSG_SNAPSHOT_TAKEN);
}

void
C64::deleteAutoSnapshot(unsigned index)
{
    Snapshot *first = autoSavedSnapshots[index];
    first->dealloc();
    
    // Shuffle slots
    for (unsigned i = index; i < MAX_AUTO_SAVED_SNAPSHOTS - 1; i++)
        autoSavedSnapshots[i] = autoSavedSnapshots[i + 1];
    autoSavedSnapshots[MAX_AUTO_SAVED_SNAPSHOTS - 1] = first;
}

unsigned
C64::numUserSnapshots()
{
    unsigned result;
    
    for (result = 0; result < MAX_USER_SAVED_SNAPSHOTS; result++) {
        if (userSavedSnapshots[result]->isEmpty())
            break;
    }
    
    return result;
}

bool
C64::takeUserSnapshot()
{
    debug("Taking user snapshop\n");
    
    Snapshot *last = userSavedSnapshots[MAX_USER_SAVED_SNAPSHOTS - 1];
    
    // Check for free space
    if (!last->isEmpty())
        return false;
    
    // Shuffle slots
    for (unsigned i = MAX_USER_SAVED_SNAPSHOTS - 1; i > 0; i--)
        userSavedSnapshots[i] = userSavedSnapshots[i - 1];
    userSavedSnapshots[0] = last;
    
    // Save state
    saveToSnapshotSafe(userSavedSnapshots[0]);
    putMessage(MSG_SNAPSHOT_TAKEN);
    
    return true;
}

void
C64::deleteUserSnapshot(unsigned index)
{
    Snapshot *first = userSavedSnapshots[index];
    first->dealloc();
    
    // Shuffle slots
    for (unsigned i = index; i < MAX_USER_SAVED_SNAPSHOTS - 1; i++)
        userSavedSnapshots[i] = userSavedSnapshots[i + 1];
    userSavedSnapshots[MAX_USER_SAVED_SNAPSHOTS - 1] = first;
}


//
//! @functiongroup Handling archives, tapes, and cartridges
//

bool 
C64::flushArchive(Archive *a, int item)
{
	uint16_t addr;
	int data;
	
	if (a == NULL)
		return false;
	
	addr = a->getDestinationAddrOfItem(item);
    debug("Flushing at addr: %04X %d\n", addr, addr);
	a->selectItem(item);
	while (1) {
		data = a->getByte();
		if (data < 0) break;
		mem.pokeRam(addr, (uint8_t)data);
		if (addr == 0xFFFF) break;
		
		addr++;
	}
	return true;
}

bool
C64::insertDisk(Archive *a)
{
    assert(a != NULL);
    
    floppy.insertDisk(a);
    return true;
    
}

bool 
C64::mountArchive(Archive *a)
{	
	if (a == NULL)
		return false;
		
	floppy.insertDisk(a);
	return true;
}

bool
C64::insertTape(TAPContainer *a)
{
    if (a == NULL)
        return false;
    
    debug(1, "Inserting TAP archive into datasette\n");
    
    suspend();
    datasette.insertTape(a);
    resume();
    
    return true;
}

bool
C64::attachCartridgeAndReset(CRTContainer *container)
{
    assert(container != NULL);
    
    Cartridge *cartridge = Cartridge::makeCartridgeWithCRTContainer(this, container);
    if (!cartridge)
        return false;
    
    suspend();
    expansionport.attachCartridge(cartridge);
    reset();
    resume();
    return true;
}

void
C64::detachCartridgeAndReset()
{
    if (expansionport.getCartridgeAttached()) {
        suspend();
        expansionport.detachCartridge();
        reset();
        resume();
    }
}

bool
C64::isCartridgeAttached()
{
    return expansionport.getCartridgeAttached();
}
