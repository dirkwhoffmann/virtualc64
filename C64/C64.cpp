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

// --------------------------------------------------------------------------------
// Execution thread
// --------------------------------------------------------------------------------

// Exit function of the main execution loop
void 
threadCleanup(void* thisC64)
{
	assert(thisC64 != NULL);
	
	C64 *c64 = (C64 *)thisC64;
	c64->threadCleanup();
	
	c64->debug(1, "Execution thread terminated\n");	
	c64->putMessage(MSG_HALT);
}


// Main execution loop
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
	c64->cpu->clearErrorState();
	c64->floppy->cpu->clearErrorState();
	c64->restartTimer();
    
	while (1) {		
		if (!c64->executeOneLine())
			break;		

		if (c64->getRasterline() == 0 && c64->getFrame() % 8 == 0) {

			// Check if thread was requested to terminate
			pthread_testcancel();
		}
	}
	
	pthread_cleanup_pop(1);
	pthread_exit(NULL);	
}


// --------------------------------------------------------------------------------
// Class methods
// --------------------------------------------------------------------------------

C64::C64()
{	
	name = "C64";
	
	debug(1, "Creating virtual C64\n");

	p = NULL;    
    warp = false;
    alwaysWarp = false;
    warpLoad = false;
	
	// Create components
	cpu = new CPU();
    mem = new C64Memory();
	vic = new VIC();
	sid = new SIDWrapper();
	iec = new IEC();
    expansionport = new ExpansionPort();
	floppy = new VC1541();
    keyboard = new Keyboard();
    joystick1 = new Joystick();
    joystick2 = new Joystick();

    // Register sub components
    VirtualComponent *subcomponents[] = {
        
        cpu,
        mem,
        vic,
        sid,
        &cia1, &cia2,
        iec,
        expansionport,
        floppy,
        &datasette,
        keyboard,
        joystick1, joystick2,
        NULL };
    
    registerSubComponents(subcomponents, sizeof(subcomponents));
    setC64(this);
    
    // Register snapshot items
    SnapshotItem items[] = {
 
        { &warpLoad,        sizeof(warpLoad),           KEEP_ON_RESET },

        { &alwaysWarp,      sizeof(alwaysWarp),         CLEAR_ON_RESET },
        { &warp,            sizeof(warp),               CLEAR_ON_RESET },
        { &cycles,          sizeof(cycles),             CLEAR_ON_RESET },
        { &frame,           sizeof(frame),              CLEAR_ON_RESET },
        { &rasterline,      sizeof(rasterline),         CLEAR_ON_RESET },
        { &rasterlineCycle, sizeof(rasterlineCycle),    CLEAR_ON_RESET },
        { NULL,             0,                          0 }};
    
    registerSnapshotItems(items, sizeof(items));

    // Configure machine type and reset
    setPAL();
    reset();
			
    // Initialie mach timer info
    mach_timebase_info(&timebase);

	// Initialize snapshot ringbuffer (BackInTime feature)
	for (unsigned i = 0; i < BACK_IN_TIME_BUFFER_SIZE; i++)
		backInTimeHistory[i] = new Snapshot();	
	backInTimeWritePtr = 0;
}

// Construction and destruction
C64::~C64()
{
	// Halt emulator
	halt();	
		
	// Release all components
    delete joystick2;
    delete joystick1;
	delete floppy;
    delete expansionport;
    delete iec;
	delete keyboard;
    delete sid;
	delete vic;
	delete cpu;
	delete mem;
    
	debug(1, "Cleaned up virtual C64\n", this);
}

void C64::reset()
{
	suspend();

    VirtualComponent::reset();
    
    cpu->mem = mem;
    cpu->setPC(0xFCE2);
	rasterlineCycle = 1;
    nanoTargetTime = 0UL;
    
    ping();
	resume();
}


void C64::ping()
{
    debug (1, "Pinging virtual C64\n");

    VirtualComponent::ping();
    putMessage(MSG_WARP, warp);
    putMessage(MSG_ALWAYS_WARP, alwaysWarp);
}
	
void 
C64::dumpState() {
	msg("C64:\n");
	msg("----\n\n");
	msg("            Machine type : %s\n", isPAL() ? "PAL" : "NTSC");
	msg("       Frames per second : %d\n", vic->getFramesPerSecond());
	msg("   Rasterlines per frame : %d\n", vic->getRasterlinesPerFrame());
	msg("   Cycles per rasterline : %d\n", vic->getCyclesPerRasterline());
	msg("           Current cycle : %llu\n", cycles);
	msg("           Current frame : %d\n", frame);
	msg("      Current rasterline : %d\n", rasterline);
	msg("Current rasterline cycle : %d\n", rasterlineCycle);
	msg("\n");
}

Message *C64::getMessage()
{
	return queue.getMessage();	
}

void C64::putMessage(int id, int i, void *p, const char *c)
{	
	queue.putMessage(id, i, p, c);
}


// -----------------------------------------------------------------------------------------------
//                                           Configure
// -----------------------------------------------------------------------------------------------

void
C64::setPAL()
{
	suspend();
	
    vic->setChipModel(MOS6569_PAL);
	sid->setPAL();

    debug(2, "Switching VIC chip model to MOS6569 (PAL)\n");
	resume();
}

void 
C64::setNTSC()
{
	suspend();
	    
    vic->setChipModel(MOS6567_NTSC);
	sid->setNTSC();

    debug(2, "Switching VIC chip model to MOS6567 (NTSC)\n");
	resume();
}

void
C64::setWarp(bool b)
{
    if (warp == b)
        return;
    
    warp = b;

    // Warping has the unavoidable drawback that audio playback gets out of sync.
    // Therefore, we silence SID during warp mode and smoothly bring back sound when
    // warping ends.
    
    if (warp) {
        // Quickly fade out SID
        sid->rampDown();
        
    } else {
        // Smoothly fade in SID
        sid->rampUp();
        restartTimer();
    }
    
    putMessage(MSG_WARP, b);
}

void
C64::setAlwaysWarp(bool b)
{
    if (alwaysWarp == b)
        return;
    
    if (alwaysWarp != b) {
        alwaysWarp = b;
        setWarp(b);
        putMessage(MSG_ALWAYS_WARP, b);
    }
}

void
C64::setWarpLoad(bool b)
{
	warpLoad = b;
}

// -----------------------------------------------------------------------------------------------
//                                       Loading and saving
// -----------------------------------------------------------------------------------------------

void C64::loadFromSnapshot(Snapshot *snapshot)
{
	if (snapshot == NULL)
		return;

	uint8_t *ptr = snapshot->getData();
	loadFromBuffer(&ptr);
    ping();
}

void 
C64::saveToSnapshot(Snapshot *snapshot)
{
	if (snapshot == NULL)
		return;
	
	snapshot->setTimestamp(time(NULL));
	// snapshot->setPAL(isPAL());
	snapshot->takeScreenshot((uint32_t *)vic->screenBuffer(), isPAL());
	
    snapshot->alloc(stateSize());
	uint8_t *ptr = snapshot->getData();
	saveToBuffer(&ptr);
}


// -----------------------------------------------------------------------------------------------
//                                              Control
// -----------------------------------------------------------------------------------------------

void 
C64::runstopRestore()
{
	// Note: The restore key is directly connected to the NMI line of the CPU
	// Thus, the runstop/restore key combination triggers an interrupts that causes a soft reset
	keyboard->pressRunstopKey();
	cpu->setNMILineReset(); 
	// Hold runstop key down for a while...
	sleepMicrosec((uint64_t)100000);
	keyboard->releaseRunstopKey();
}

bool
C64::isRunnable() {
	return mem->basicRomIsLoaded() && mem->charRomIsLoaded() && mem->kernelRomIsLoaded() && floppy->mem->romIsLoaded();
}

void 
C64::run() {
	
	if (isHalted()) {
		
		// Check for ROM images
		if (getMissingRoms()) {
			putMessage(MSG_ROM_MISSING, getMissingRoms());
			return;
		}

        // Power on sub components
		sid->run();

		// Start execution thread
		pthread_create(&p, NULL, runThread, (void *)this);	
	}
}

bool 
C64::isRunning() {
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
		// Shut down sub components
		sid->halt();
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
	cpu->clearErrorState();
	floppy->cpu->clearErrorState();
	
	// Execute next command 
	do {
		executeOneCycle();
	} while (!cpu->atBeginningOfNewCommand()); 
	
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

#define EXECUTE(x) \
		cia1.executeOneCycle(); \
		cia2.executeOneCycle(); \
        if (!cpu->executeOneCycle()) result = false; \
		if (!floppy->executeOneCycle()) result = false; \
        datasette.execute(); \
		cycles++; \
        rasterlineCycle++;

void
C64::beginOfRasterline()
{
	// First cycle of rasterline
	if (rasterline == 0) {
		vic->beginFrame();			
	}
	vic->beginRasterline(rasterline);	
}

void
C64::endOfRasterline()
{
	vic->endRasterline();
	rasterlineCycle = 1;
	rasterline++;

	if (rasterline >= vic->getRasterlinesPerFrame()) {
        
		// Last rasterline of frame
		rasterline = 0;			
		vic->endFrame();
		frame++;

		// Increment time of day clocks every tenth of a second
		if (frame % (vic->getFramesPerSecond() / 10) == 0) {
			cia1.incrementTOD();
			cia2.incrementTOD();
		}
		
		// Take a snapshot once in a while
		if (frame % (vic->getFramesPerSecond() * 4) == 0) {
			takeSnapshot();			
		}
		
		// Execute remaining SID cycles
        sid->executeUntil(cycles);
        /*
        int diff = sid->resid->writePtr - sid->resid->readPtr;
         debug(2,"SID readCnt: %8d writeCnt: %8d readPtr: %8d writePtr: %8d diff: %8d volume:%d target:%d\n",
              sid->resid->readDataCnt, sid->resid->writeDataCnt,
              sid->resid->readPtr, sid->resid->writePtr,
              (diff > 0) ? diff : 44100 + diff,sid->resid->volume,sid->resid->targetVolume);
         sid->resid->readDataCnt = sid->resid->writeDataCnt = 0;
        */
         
        // Execute the IEC bus
        iec->execute();

        // Count some sheep (zzzzzz) ...
        if (!getWarp())
            synchronizeTiming();
    }
}

inline bool
C64::executeOneCycle()
{
	bool result = true; // Don't break execution
    
	switch(rasterlineCycle) {
		case 1:
			beginOfRasterline();			
			vic->cycle1();
			EXECUTE(1);
			break;
		case 2: 
			vic->cycle2();
			EXECUTE(2);
			break;
		case 3: 
			vic->cycle3();
			EXECUTE(3);
			break;
		case 4: 
			vic->cycle4();
			EXECUTE(4);
			break;
		case 5: 
			vic->cycle5();
			EXECUTE(5);
			break;
		case 6: 
			vic->cycle6();
			EXECUTE(6);
			break;
		case 7: 
			vic->cycle7();
			EXECUTE(7);
			break;
		case 8: 
			vic->cycle8();
			EXECUTE(8);
			break;
		case 9: 
			vic->cycle9();
			EXECUTE(9);
			break;
		case 10: 
			vic->cycle10();
			EXECUTE(10);
			break;
		case 11: 
			vic->cycle11();
			EXECUTE(11);
			break;
		case 12: 
			vic->cycle12();
			EXECUTE(12);
			break;
		case 13: 
			vic->cycle13();
			EXECUTE(13);
			break;
		case 14: 
			vic->cycle14();
			EXECUTE(14);
			break;
		case 15: 
			vic->cycle15();
			EXECUTE(15);
			break;
		case 16: 
			vic->cycle16();
			EXECUTE(16);
			break;
		case 17: 
			vic->cycle17();
			EXECUTE(17);
			break;
		case 18: 
			vic->cycle18();
			EXECUTE(18);
			break;
		case 19: 
			vic->cycle19to54();
			EXECUTE(19);
			break;
		case 20: 
			vic->cycle19to54();
			EXECUTE(20);
			break;
		case 21: 
			vic->cycle19to54();
			EXECUTE(21);
			break;
		case 22: 
			vic->cycle19to54();
			EXECUTE(22);
			break;
		case 23: 
			vic->cycle19to54();
			EXECUTE(23);
			break;
		case 24: 
			vic->cycle19to54();
			EXECUTE(24);
			break;
		case 25: 
			vic->cycle19to54();
			EXECUTE(25);
			break;
		case 26: 
			vic->cycle19to54();
			EXECUTE(26);
			break;
		case 27: 
			vic->cycle19to54();
			EXECUTE(27);
			break;
		case 28: 
			vic->cycle19to54();
			EXECUTE(28);
			break;
		case 29: 
			vic->cycle19to54();
			EXECUTE(29);
			break;
		case 30: 
			vic->cycle19to54();
			EXECUTE(30);
			break;
		case 31: 
			vic->cycle19to54();
			EXECUTE(31);
			break;
		case 32: 
			vic->cycle19to54();
			EXECUTE(32);
			break;
		case 33: 
			vic->cycle19to54();
			EXECUTE(33);
			break;
		case 34: 
			vic->cycle19to54();
			EXECUTE(34);
			break;
		case 35: 
			vic->cycle19to54();
			EXECUTE(35);
			break;
		case 36: 
			vic->cycle19to54();
			EXECUTE(36);
			break;
		case 37: 
			vic->cycle19to54();
			EXECUTE(37);
			break;
		case 38: 
			vic->cycle19to54();
			EXECUTE(38);
			break;
		case 39: 
			vic->cycle19to54();
			EXECUTE(39);
			break;
		case 40: 
			vic->cycle19to54();
			EXECUTE(40);
			break;
		case 41: 
			vic->cycle19to54();
			EXECUTE(41);
			break;
		case 42: 
			vic->cycle19to54();
			EXECUTE(42);
			break;
		case 43: 
			vic->cycle19to54();
			EXECUTE(43);
			break;
		case 44: 
			vic->cycle19to54();
			EXECUTE(44);
			break;
		case 45: 
			vic->cycle19to54();
			EXECUTE(45);
			break;
		case 46: 
			vic->cycle19to54();
			EXECUTE(46);
			break;
		case 47: 
			vic->cycle19to54();
			EXECUTE(47);
			break;
		case 48: 
			vic->cycle19to54();
			EXECUTE(48);
			break;
		case 49: 
			vic->cycle19to54();
			EXECUTE(49);
			break;
		case 50: 
			vic->cycle19to54();
			EXECUTE(50);
			break;
		case 51: 
			vic->cycle19to54();
			EXECUTE(51);
			break;
		case 52: 
			vic->cycle19to54();
			EXECUTE(52);
			break;
		case 53: 
			vic->cycle19to54();
			EXECUTE(53);
			break;
		case 54: 
			vic->cycle19to54();
			EXECUTE(54);
			break;
		case 55: 
			vic->cycle55();
			EXECUTE(55);
			break;
		case 56: 
			vic->cycle56();
			EXECUTE(56);
			break;
		case 57: 
			vic->cycle57();
			EXECUTE(57);
			break;
		case 58: 
			vic->cycle58();
			EXECUTE(58);
			break;
		case 59: 
			vic->cycle59();
			EXECUTE(59);
			break;
		case 60: 
			vic->cycle60();
			EXECUTE(60);
			break;
		case 61: 
			vic->cycle61();
			EXECUTE(61);
			break;
		case 62: 
			vic->cycle62();
			EXECUTE(62);
			break;
		case 63: 
			vic->cycle63();
			EXECUTE(63);
			if (vic->getCyclesPerRasterline() == 63) {
				// last cycle for PAL machines
				endOfRasterline();
			}			
			break;
		case 64: 
			vic->cycle64();
			EXECUTE(64);			
			break;
		case 65: 
			vic->cycle65();
			EXECUTE(65);
			endOfRasterline();
			break;
			
		default:
			// can't reach
			assert(false);
			return false;
	}
    
	return result;
}

// Execute until the end of the rasterline
inline bool
C64::executeOneLine()
{
	uint8_t lastCycle = vic->getCyclesPerRasterline();
	for (int i = rasterlineCycle; i <= lastCycle; i++) {
		if (!executeOneCycle())
			return false;
	}
	return true;
}


// -----------------------------------------------------------------------------------------------
//                                  ROM and snapshot handling
// -----------------------------------------------------------------------------------------------

uint8_t
C64::getMissingRoms() {
	
	uint8_t missingRoms = 0;
	
	if (!mem->basicRomIsLoaded()) missingRoms |= BASIC_ROM;
	if (!mem->charRomIsLoaded()) missingRoms |= CHAR_ROM;
	if (!mem->kernelRomIsLoaded()) missingRoms |= KERNEL_ROM;
	if (!floppy->mem->romIsLoaded()) missingRoms |= VC1541_ROM;
	return missingRoms;
}

bool 
C64::loadRom(const char *filename)
{
	bool result = false; 
		
	suspend(); 
	
	bool wasRunnable = isRunnable();
	
	if (C64Memory::isBasicRom(filename)) {
		result = mem->loadBasicRom(filename);
		if (result) putMessage(MSG_ROM_LOADED, BASIC_ROM);
	}
	
	if (C64Memory::isCharRom(filename)) {
		result = mem->loadCharRom(filename);
		if (result) putMessage(MSG_ROM_LOADED, CHAR_ROM);
	}
	
	if (C64Memory::isKernelRom(filename)) {
		result = mem->loadKernelRom(filename);
		if (result) putMessage(MSG_ROM_LOADED, KERNEL_ROM);
	}
	
	if (VC1541Memory::is1541Rom(filename)) {
		result = floppy->mem->loadRom(filename);
		if (result) putMessage(MSG_ROM_LOADED, VC1541_ROM);
	}
			
	bool isNowRunnable = isRunnable();
	
	if (!wasRunnable && isNowRunnable) {
		// Last missing ROM was loaded
		putMessage(MSG_ROM_COMPLETE);
	}
	
	resume();
	return result;
}

void 
C64::takeSnapshot() 
{
    debug(3, "Taking snapshop %d (%p)\n", backInTimeWritePtr, backInTimeHistory[backInTimeWritePtr]);
    
	saveToSnapshot(backInTimeHistory[backInTimeWritePtr]);
	backInTimeWritePtr = (backInTimeWritePtr + 1) % BACK_IN_TIME_BUFFER_SIZE;
}

unsigned 
C64::numHistoricSnapshots()
{
	for (int i = BACK_IN_TIME_BUFFER_SIZE - 1; i >= 0; i--) {
		if (!backInTimeHistory[i]->isEmpty()) 
			return i + 1;
	}
	return 0;
}

Snapshot *
C64::getHistoricSnapshot(int nr)
{
	if (nr >= BACK_IN_TIME_BUFFER_SIZE)
		return NULL;
	
	int pos = (BACK_IN_TIME_BUFFER_SIZE + backInTimeWritePtr - 1 - nr) % BACK_IN_TIME_BUFFER_SIZE;
	Snapshot *snapshot = backInTimeHistory[pos];
	assert(snapshot != NULL);
	
	if (snapshot->isEmpty())
		return NULL;
	
	return snapshot;
}


// -----------------------------------------------------------------------------------------------
//                                           Timing
// -----------------------------------------------------------------------------------------------

void 
C64::restartTimer() 
{
    uint64_t kernelNow = mach_absolute_time();
    uint64_t nanoNow = abs_to_nanos(kernelNow);
    
    nanoTargetTime = nanoNow + vic->getFrameDelay();
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
    nanoTargetTime += vic->getFrameDelay();

    // debug(2, "Jitter = %d", jitter);
    if (jitter > 1000000000 /* 1 sec */) {
        
        // The emulator did not keep up with the real time clock. Instead of
        // running behind for a long time, we reset the synchronization timer
        
        debug(2, "Jitter exceeds limit (%lld). Restarting synchronization timer.\n", jitter);
        restartTimer();
    }
}

// ---------------------------------------------------------------------------------------------
//                                 Archives (disks, tapes, etc.)
// ---------------------------------------------------------------------------------------------

bool 
C64::flushArchive(Archive *a, int item)
{
	uint16_t addr;
	int data;
	
	if (a == NULL)
		return false;
	
	addr = a->getDestinationAddrOfItem(item);
	a->selectItem(item);
	while (1) {
		data = a->getByte();
		if (data < 0) break;
		
		mem->pokeRam(addr, (uint8_t)data);
		if (addr == 0xFFFF) break;
		
		addr++;
	}
	return true;
}

bool 
C64::mountArchive(Archive *a)
{	
	if (a == NULL)
		return false;
		
	floppy->insertDisk(a);
	return true;
}

bool
C64::insertTape(TAPArchive *a)
{
    if (a == NULL)
        return false;
    
    suspend();
    debug("Inserting tape %p\n", a);
    datasette.insertTape(a);
    resume();
    
    return true;
}

// ---------------------------------------------------------------------------------------------
//                                            Cartridges
// ---------------------------------------------------------------------------------------------

bool
C64::attachCartridge(Cartridge *c)
{
    return expansionport->attachCartridge(c);
}

void
C64::detachCartridge()
{
    expansionport->detachCartridge();
}

bool
C64::isCartridgeAttached()
{
    return expansionport->getCartridgeAttached();
}


// ---------------------------------------------------------------------------------------------
//                                            Misc
// ---------------------------------------------------------------------------------------------

void 
C64::threadCleanup()
{
	p = NULL;
	debug(1, "Execution thread cleanup\n");
}

