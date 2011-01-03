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
	c64->setDelay();
	
	while (1) {		
		if (!c64->executeOneLine())
			break;		
		// c64->rasterlineCycle = 1;
		if (c64->getFrame() == 0 && c64->getRasterline() == 0)
			pthread_testcancel();
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
	
	debug(1, "Creating virtual C64 at address %p...\n", this);

	p = NULL;
	warpMode = false;

	// Create components
	mem = new C64Memory();
	cpu = new CPU();	
	vic = new VIC();
	sid = new SID();
	cia1 = new CIA1();
	cia2 = new CIA2();
	keyboard = new Keyboard();
	iec = new IEC();
	floppy = new VC1541();
	
	// Bind components
	cpu->setC64(this);
	cpu->setMemory(mem);
	mem->setVIC(vic);
	mem->setSID(sid);
	mem->setCIA1(cia1);
	mem->setCIA2(cia2);
	mem->setCPU(cpu);
	cia1->setCPU(cpu);
	cia1->setVIC(vic);	
	cia1->setKeyboard(keyboard);
	cia2->setCPU(cpu);
	cia2->setVIC(vic);	
	cia2->setIEC(iec);
	vic->setC64(this);
	vic->setCPU(cpu);
	vic->setMemory(mem);
	iec->setDrive(floppy);
	floppy->setIEC(iec);
	floppy->setC64(this);
	
	// Setup initial game port mapping
	// 0 joysticks connected
	setInputDevice(0, IPD_UNCONNECTED);
	setInputDevice(1, IPD_UNCONNECTED);
	joystick1 = new Joystick;
	joystick2 = new Joystick;
		
	// Configure
	setNTSC(); // Why NTSC??
	
	reset();
	
	// Remove after debugging
	cpu->max_traces = 20000;
	cpu->trace_enable_address = 2070; 	
}

// Construction and destruction
C64::~C64()
{
	// Halt emulator
	halt();	
		
	// Release all components
	delete floppy;
	delete keyboard;
	delete iec;
	delete cia1;
	delete cia2;
	delete vic;
	delete sid;
	delete cpu;	
	delete mem;
	
	if( joystick1 != NULL )
		delete joystick1;
	
	if( joystick2 != NULL )
		delete joystick2;
		
	debug(1, "Cleaned up virtual C64 at address %p\n", this);
}

void C64::reset()
{
	suspend();

	debug (1, "Resetting virtual C64\n");
	mem->reset();
	cpu->reset();
	cpu->setPC(0xFCE2);
	vic->reset();
	sid->reset();
	cia1->reset();
	cia2->reset();
	keyboard->reset();
	iec->reset();
	floppy->reset();

	// archive = NULL;
	cycles = 0UL;
	frame = 0;
	rasterline = 0;
	rasterlineCycle = 1;
	targetTime = 0UL;

	resume();
}

void C64::fastReset()
{
#if 0	
	Snapshot *snapshot; 
	
	debug (1, "Resetting virtual C64 (fast reset via image file)\n");
	
	if ((snapshot = Snapshot::snapshotFromFile("ResetImage.VC64")) != NULL) {
		snapshot->writeToC64(this);
	} else {
		debug(1, "Error while reading reset image\n");
	}

	delete snapshot;
#endif
	warn("fastReset: Not implemented, yet\n");
}

bool 
C64::load(uint8_t **buffer)
{	
	uint8_t *old = *buffer;
	
	suspend();
	
	debug(1, "Loading...\n");

	// Load screenshot
	vic->loadScreenshot(buffer);
	debug(2, "%d\n", *buffer - old);

	// Load internal state
	cycles = read64(buffer);
	frame = (int)read32(buffer);
	rasterline = (int)read32(buffer);
	rasterlineCycle = (int)read32(buffer);
	targetTime = read64(buffer);
	
	// Load internal state of sub components
	cpu->load(buffer);
	debug(2, "%d\n", *buffer - old);
	// cpu->dumpState();
	vic->load(buffer);
	debug(2, "%d\n", *buffer - old);
	// vic->dumpState();
	sid->load(buffer);
	debug(2, "%d\n", *buffer - old);
	cia1->load(buffer);
	debug(2, "%d\n", *buffer - old);
	cia2->load(buffer);	
	debug(2, "%d\n", *buffer - old);
	mem->load(buffer);
	debug(2, "%d\n", *buffer - old);
	keyboard->load(buffer);
	debug(2, "%d\n", *buffer - old);
	iec->load(buffer);
	debug(2, "%d\n", *buffer - old);
	floppy->load(buffer);
	debug(2, "%d\n", *buffer - old);
	
	resume();
	return true;
}

bool 
C64::save(uint8_t **buffer)
{	
	uint8_t *old = *buffer;
	
	suspend();
	
	debug(1, "Saving...\n");
		
	// Save screenshot
	vic->saveScreenshot(buffer);
	debug(2, "%d\n", *buffer - old);

	// Save internal state
	write64(buffer, cycles);
	write32(buffer, (uint32_t)frame);
	write32(buffer, (uint32_t)rasterline);
	write32(buffer, (uint32_t)rasterlineCycle);
	write64(buffer, targetTime);
	
	// Save internal state of sub components
	cpu->save(buffer);
	debug(2, "%d\n", *buffer - old);
	// cpu->dumpState();
	vic->save(buffer);
	debug(2, "%d\n", *buffer - old);
	// vic->dumpState();
	sid->save(buffer);
	debug(2, "%d\n", *buffer - old);
	cia1->save(buffer);
	debug(2, "%d\n", *buffer - old);
	cia2->save(buffer);
	debug(2, "%d\n", *buffer - old);
	mem->save(buffer);
	debug(2, "%d\n", *buffer - old);
	keyboard->save(buffer);
	debug(2, "%d\n", *buffer - old);
	iec->save(buffer);
	debug(2, "%d\n", *buffer - old);
	floppy->save(buffer);
	debug(2, "%d\n", *buffer - old);
	
	resume();
	return true;
}

void 
C64::dumpState() {
	debug(1, "C64:\n");
	debug(1, "----\n\n");
	debug(1, "            Machine type : %s\n", (noOfRasterlines == VIC::PAL_RASTERLINES) ? "PAL" : "NTSC");
	debug(1, "       Frames per second : %d\n", fps);
	debug(1, "   Rasterlines per frame : %d\n", noOfRasterlines);
	debug(1, "   Cycles per rasterline : %d\n", cpuCyclesPerRasterline);
	debug(1, "           Current cycle : %llu\n", cycles);
	debug(1, "           Current frame : %d\n", frame);
	debug(1, "      Current rasterline : %d\n", rasterline);
	debug(1, "Current rasterline cycle : %d\n", rasterlineCycle);
	debug(1, "\n");
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
	fps = VIC::PAL_REFRESH_RATE;
	noOfRasterlines = VIC::PAL_RASTERLINES; 
	cpuCyclesPerRasterline = VIC::PAL_CYCLES_PER_RASTERLINE;
	vic->setPAL();
	sid->setVideoMode(CPU::CLOCK_FREQUENCY_PAL);
	frameDelay = (1000000 / fps);
}

void 
C64::setNTSC()
{
	fps = VIC::NTSC_REFRESH_RATE;
	noOfRasterlines = VIC::NTSC_RASTERLINES; 
	cpuCyclesPerRasterline = VIC::NTSC_CYCLES_PER_RASTERLINE;
	vic->setNTSC();
	sid->setVideoMode(CPU::CLOCK_FREQUENCY_NTSC);
	frameDelay = (1000000 / fps);
}

bool 
C64::getWarpMode() 
{ 
	return warpMode;
}

void
C64::setWarpMode(bool b)
{
	warpMode = b;
	restartTimer();
	putMessage(MSG_WARP, b, NULL, NULL);
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
	return (numberOfMissingRoms() == 0);
}

void 
C64::run() {
	
	if (isHalted()) {
		
		// Check for ROM images
		if (getMissingRoms()) {
			putMessage(MSG_ROM_MISSING, getMissingRoms());
			return;
		}
		
		// Start execution thread
		pthread_create(&p, NULL, runThread, (void *)this);	
		
		// Power on sub components
		sid->run();
		
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
		
		// Shut down sub components
		sid->halt();
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

#if 0
#define EXECUTE(x) \
cia1->triggerInterrupts(); \
cia2->triggerInterrupts(); \
cia1->executeOneCycle(); \
cia2->executeOneCycle(); \
if (!cpu->executeOneCycle()) result = false; \
if (!floppy->executeOneCycle()) result = false; \
cycles++; \
rasterlineCycle++;
#endif

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
		cia1->executeOneCycle(); \
		cia2->executeOneCycle(); \
		if (!cpu->executeOneCycle()) result = false; \
		if (!floppy->executeOneCycle()) result = false; \
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
	if (rasterline >= getRasterlinesPerFrame()) {
		// Last rasterline of frame
		rasterline = 0;			
		vic->endFrame();
		frame++;
		if (frame >= getFramesPerSecond() / 10) {
			// Increment the "time of day clocks" every tenth of a second
			frame = 0;
			cia1->incrementTOD();
			cia2->incrementTOD();
		}
		// Pass control to the virtual sound chip
		sid->execute(getCyclesPerFrame());
			
		// Pass control to the virtual IEC bus
		iec->execute();
			
		// Sleep... 
		if (!getWarpMode()) 
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
			vic->cycle19();
			EXECUTE(19);
			break;
		case 20: 
			vic->cycle20();
			EXECUTE(20);
			break;
		case 21: 
			vic->cycle21();
			EXECUTE(21);
			break;
		case 22: 
			vic->cycle22();
			EXECUTE(22);
			break;
		case 23: 
			vic->cycle23();
			EXECUTE(23);
			break;
		case 24: 
			vic->cycle24();
			EXECUTE(24);
			break;
		case 25: 
			vic->cycle25();
			EXECUTE(25);
			break;
		case 26: 
			vic->cycle26();
			EXECUTE(26);
			break;
		case 27: 
			vic->cycle27();
			EXECUTE(27);
			break;
		case 28: 
			vic->cycle28();
			EXECUTE(28);
			break;
		case 29: 
			vic->cycle29();
			EXECUTE(29);
			break;
		case 30: 
			vic->cycle30();
			EXECUTE(30);
			break;
		case 31: 
			vic->cycle31();
			EXECUTE(31);
			break;
		case 32: 
			vic->cycle32();
			EXECUTE(32);
			break;
		case 33: 
			vic->cycle33();
			EXECUTE(33);
			break;
		case 34: 
			vic->cycle34();
			EXECUTE(34);
			break;
		case 35: 
			vic->cycle35();
			EXECUTE(35);
			break;
		case 36: 
			vic->cycle36();
			EXECUTE(36);
			break;
		case 37: 
			vic->cycle37();
			EXECUTE(37);
			break;
		case 38: 
			vic->cycle38();
			EXECUTE(38);
			break;
		case 39: 
			vic->cycle39();
			EXECUTE(39);
			break;
		case 40: 
			vic->cycle40();
			EXECUTE(40);
			break;
		case 41: 
			vic->cycle41();
			EXECUTE(41);
			break;
		case 42: 
			vic->cycle42();
			EXECUTE(42);
			break;
		case 43: 
			vic->cycle43();
			EXECUTE(43);
			break;
		case 44: 
			vic->cycle44();
			EXECUTE(44);
			break;
		case 45: 
			vic->cycle45();
			EXECUTE(45);
			break;
		case 46: 
			vic->cycle46();
			EXECUTE(46);
			break;
		case 47: 
			vic->cycle47();
			EXECUTE(47);
			break;
		case 48: 
			vic->cycle48();
			EXECUTE(48);
			break;
		case 49: 
			vic->cycle49();
			EXECUTE(49);
			break;
		case 50: 
			vic->cycle50();
			EXECUTE(50);
			break;
		case 51: 
			vic->cycle51();
			EXECUTE(51);
			break;
		case 52: 
			vic->cycle52();
			EXECUTE(52);
			break;
		case 53: 
			vic->cycle53();
			EXECUTE(53);
			break;
		case 54: 
			vic->cycle54();
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
			if (getCyclesPerRasterline() == 63) {
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
	uint8_t lastCycle = getCyclesPerRasterline();
	for (int i = rasterlineCycle; i <= lastCycle; i++) {
		if (!executeOneCycle())
			return false;
	}
	return true;
}

#if 0
// Execute until the end of the rasterline
inline bool
C64::executeOneLine(int cycle)
{
	switch(cycle) {
		case 1:
			beginOfRasterline();			
			vic->cycle1();
			EXECUTE(1);
		case 2: 
			vic->cycle2();
			EXECUTE(2);
		case 3: 
			vic->cycle3();
			EXECUTE(3);
		case 4: 
			vic->cycle4();
			EXECUTE(4);
		case 5: 
			vic->cycle5();
			EXECUTE(5);
		case 6: 
			vic->cycle6();
			EXECUTE(6);
		case 7: 
			vic->cycle7();
			EXECUTE(7);
		case 8: 
			vic->cycle8();
			EXECUTE(8);
		case 9: 
			vic->cycle9();
			EXECUTE(9);
		case 10: 
			vic->cycle10();
			EXECUTE(10);
		case 11: 
			vic->cycle11();
			EXECUTE(11);
		case 12: 
			vic->cycle12();
			EXECUTE(12);
		case 13: 
			vic->cycle13();
			EXECUTE(13);
		case 14: 
			vic->cycle14();
			EXECUTE(14);
		case 15: 
			vic->cycle15();
			EXECUTE(15);
		case 16: 
			vic->cycle16();
			EXECUTE(16);
		case 17: 
			vic->cycle17();
			EXECUTE(17);
		case 18: 
			vic->cycle18();
			EXECUTE(18);
		case 19: 
			vic->cycle19();
			EXECUTE(19);
		case 20: 
			vic->cycle20();
			EXECUTE(20);
		case 21: 
			vic->cycle21();
			EXECUTE(21);
		case 22: 
			vic->cycle22();
			EXECUTE(22);
		case 23: 
			vic->cycle23();
			EXECUTE(23);
		case 24: 
			vic->cycle24();
			EXECUTE(24);
		case 25: 
			vic->cycle25();
			EXECUTE(25);
		case 26: 
			vic->cycle26();
			EXECUTE(26);
		case 27: 
			vic->cycle27();
			EXECUTE(27);
		case 28: 
			vic->cycle28();
			EXECUTE(28);
		case 29: 
			vic->cycle29();
			EXECUTE(29);
		case 30: 
			vic->cycle30();
			EXECUTE(30);
		case 31: 
			vic->cycle31();
			EXECUTE(31);
		case 32: 
			vic->cycle32();
			EXECUTE(32);
		case 33: 
			vic->cycle33();
			EXECUTE(33);
		case 34: 
			vic->cycle34();
			EXECUTE(34);
		case 35: 
			vic->cycle35();
			EXECUTE(35);
		case 36: 
			vic->cycle36();
			EXECUTE(36);
		case 37: 
			vic->cycle37();
			EXECUTE(37);
		case 38: 
			vic->cycle38();
			EXECUTE(38);
		case 39: 
			vic->cycle39();
			EXECUTE(39);
		case 40: 
			vic->cycle40();
			EXECUTE(40);
		case 41: 
			vic->cycle41();
			EXECUTE(41);
		case 42: 
			vic->cycle42();
			EXECUTE(42);
		case 43: 
			vic->cycle43();
			EXECUTE(43);
		case 44: 
			vic->cycle44();
			EXECUTE(44);
		case 45: 
			vic->cycle45();
			EXECUTE(45);
		case 46: 
			vic->cycle46();
			EXECUTE(46);
		case 47: 
			vic->cycle47();
			EXECUTE(47);
		case 48: 
			vic->cycle48();
			EXECUTE(48);
		case 49: 
			vic->cycle49();
			EXECUTE(49);
		case 50: 
			vic->cycle50();
			EXECUTE(50);
		case 51: 
			vic->cycle51();
			EXECUTE(51);
		case 52: 
			vic->cycle52();
			EXECUTE(52);
		case 53: 
			vic->cycle53();
			EXECUTE(53);
		case 54: 
			vic->cycle54();
			EXECUTE(54);
		case 55: 
			vic->cycle55();
			EXECUTE(55);
		case 56: 
			vic->cycle56();
			EXECUTE(56);
		case 57: 
			vic->cycle57();
			EXECUTE(57);
		case 58: 
			vic->cycle58();
			EXECUTE(58);
		case 59: 
			vic->cycle59();
			EXECUTE(59);
		case 60: 
			vic->cycle60();
			EXECUTE(60);
		case 61: 
			vic->cycle61();
			EXECUTE(61);
		case 62: 
			vic->cycle62();
			EXECUTE(62);
		case 63: 
			vic->cycle63();
			EXECUTE(63);
			
			if (getCyclesPerRasterline() == 63) {
				// last cycle for PAL machines
				endOfRasterline();
				return true;
			}			
		case 64: 
			vic->cycle64();
			EXECUTE(64);			
		case 65: 
			vic->cycle65();
			EXECUTE(65);
			endOfRasterline();
			return true;
			
		default:
			// can't reach
			assert(false);
			return false;
	}	
}
#endif

// -----------------------------------------------------------------------------------------------
//                                  ROM and snapshot handling
// -----------------------------------------------------------------------------------------------

int
C64::numberOfMissingRoms() {
	
	int result = 0;
	
	if (!mem->basicRomIsLoaded()) result++;
	if (!mem->charRomIsLoaded()) result++;
	if (!mem->kernelRomIsLoaded()) result++;
	if (!floppy->mem->romIsLoaded()) result++;
	return result;
}

int
C64::getMissingRoms() {
	
	int missingRoms = 0;
	
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
	
	resume();
	return result;
}

// -----------------------------------------------------------------------------------------------
//                                           Timing
// -----------------------------------------------------------------------------------------------

void 
C64::setDelay(int delay) 
{ 
	frameDelay = delay;
	restartTimer();
}

void 
C64::restartTimer() 
{ 
	targetTime = msec() + frameDelay;
}

void 
C64::synchronizeTiming()
{
	// determine how long we should wait
	uint64_t timeToSleep = targetTime - msec();
	
	// update target time
	targetTime += (uint64_t)frameDelay;
	
	// sleep
	if (timeToSleep > 0) {
		sleepMicrosec(timeToSleep);
	} else {
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
	// Archive loaded and mountable?
	if (a == NULL || !a->isMountable())
		return false;
	
	// Insert disc
	floppy->insertDisc((D64Archive *)a);
	
	return true;
}


// ---------------------------------------------------------------------------------------------
//                                            Cartridges
// ---------------------------------------------------------------------------------------------

bool
C64::attachCartridge(Cartridge *c)
{
	mem->attachCartridge(c);
	
	putMessage(MSG_CARTRIDGE, 1);
	return true;
}

bool
C64::detachCartridge()
{
	mem->detachCartridge();

	putMessage(MSG_CARTRIDGE, 0);
	return true;
}

bool
C64::isCartridgeAttached()
{
 	return mem->isCartridgeAttached();
}


// ---------------------------------------------------------------------------------------------
//                                            Misc
// ---------------------------------------------------------------------------------------------

int 
C64::build()
{
	char month[11];
	int year, mon, day;

	sscanf(__DATE__, "%s %d %d", month, &day, &year);
	
	if (!strcmp(month, "Jan")) mon = 1;
	else if (!strcmp(month, "Feb")) mon = 2;
	else if (!strcmp(month, "Mar")) mon = 3;
	else if (!strcmp(month, "Apr")) mon = 4;
	else if (!strcmp(month, "May")) mon = 5;
	else if (!strcmp(month, "Jun")) mon = 6;
	else if (!strcmp(month, "Jul")) mon = 7;
	else if (!strcmp(month, "Aug")) mon = 8;
	else if (!strcmp(month, "Sep")) mon = 9;
	else if (!strcmp(month, "Oct")) mon = 10;
	else if (!strcmp(month, "Nov")) mon = 11;
	else if (!strcmp(month, "Dez")) mon = 12;
	else mon = 0; // Huh?

	return ((year - 2000) * 10000) + (mon * 100) + day;
}

void
C64::setInputDevice(int portNo, int newDevice) 
{			
	port[portNo] = newDevice;
	
	// Update CIA structure
	switch(newDevice) {
		case IPD_UNCONNECTED:
			cia1->setJoystickToPort( portNo, NULL );
			cia1->setKeyboardToPort( portNo, false );
			break;
			
		case IPD_KEYBOARD: 
			cia1->setJoystickToPort( portNo, NULL );
			cia1->setKeyboardToPort( portNo, true );
			break;
			
		case IPD_JOYSTICK_1: 
			cia1->setJoystickToPort( portNo, joystick1 );
			cia1->setKeyboardToPort( portNo, false );
			break;
			
		case IPD_JOYSTICK_2: 
			cia1->setJoystickToPort( portNo, joystick2 );
			cia1->setKeyboardToPort( portNo, false );
			break;
			
		default:
			assert(false);
	}
}

void
C64::switchInputDevice( int portNo ) 
{
	int newDevice = port[portNo];
	bool invalid;
			
	assert(portNo == 0 || portNo == 1);

	do {
		newDevice = (newDevice + 1) % NUM_INPUT_DEVICES;
	
		if (newDevice == IPD_JOYSTICK_1 && !joystick1->IsActive())
			invalid = true;
		else if (newDevice == IPD_JOYSTICK_2 && !joystick2->IsActive())
			invalid = true;
		else 
			invalid = false;
	} while (invalid);
				
	setInputDevice(portNo, newDevice);
}

void 
C64::switchInputDevices()
{
	debug(1, "Switching input devides\n");
	int tmp_port = port[0];
	port[0] = port[1];
	port[1] = tmp_port;
}

int
C64::getDeviceOfPort( int portNo ) {
	return port[portNo];
}

Joystick *C64::addJoystick()
{
	if( !joystick1->IsActive())
	{
		joystick1->SetActiveState( true );
		return joystick1;
	}
	else if( !joystick2->IsActive() )
	{
		joystick2->SetActiveState( true );
		return joystick2;
	}
	else
		throw( "Joystick 1 and 2 are allready assigned!" );
}

void C64::removeJoystick( Joystick *joystick )
{
	assert( (joystick == joystick1) || (joystick == joystick2) );
	
	if( joystick == joystick1 )
	{
		if( !joystick1->IsActive() )
			throw( "Joystick1 is not assigned" );
		
		if( getDeviceOfPort( 0 ) == IPD_JOYSTICK_1 )
			setInputDevice( 0, IPD_UNCONNECTED );
		
		if( getDeviceOfPort( 1 ) == IPD_JOYSTICK_1 )
			setInputDevice( 1, IPD_UNCONNECTED ); 
		
		joystick1->SetActiveState( false );
	}
	else if( joystick == joystick2 )
	{
		if( !joystick2->IsActive() )
			throw( "Joystick2 is not assigned" );
		
		if( getDeviceOfPort( 0 ) == IPD_JOYSTICK_2 )
			setInputDevice( 0, IPD_UNCONNECTED );
		
		if( getDeviceOfPort( 1 ) == IPD_JOYSTICK_2 )
			setInputDevice( 1, IPD_UNCONNECTED ); 
		
		joystick2->SetActiveState( false );
	}
	else
		throw( "Invalid joystick" );
}

void 
C64::threadCleanup()
{
	p = NULL;
	debug(1, "Execution thread cleanup\n");
}

