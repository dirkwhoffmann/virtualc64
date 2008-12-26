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
}


// Main execution loop
void 
*runThread(void *thisC64) {
	
	assert(thisC64 != NULL);
	
	C64 *c64 = (C64 *)thisC64;
		
	// Configure thread properties...
	c64->debug("Execution thread started\n");
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
	pthread_cleanup_push(threadCleanup, thisC64);
	
	// Prepare to run...
	c64->cpu->clearErrorState();
	c64->floppy->cpu->clearErrorState();
	c64->setDelay((uint64_t)(1000000 / c64->fps));

	// determine cycle relative to the current rasterline
	c64->cycle = (c64->cpu->getCycles() % c64->getCyclesPerRasterline()) + 1;
	
	while (1) {		
		if (!c64->executeOneLine())
			break;		
		if (c64->frame == 0 && c64->rasterline == 0)
			pthread_testcancel();
	}
	
	c64->debug("Execution thread terminated\n");	
	
	pthread_cleanup_pop(1);
	pthread_exit(NULL);	
}


#define EXEC_CPU(x) cpu->executeOneCycle(); if (cpu->getErrorState() != CPU::OK) { return false; }
#define EXEC_FLOPPY(x) floppy->executeOneCycle(); if (floppy->cpu->getErrorState() != CPU::OK) { return false; }
#define EXEC_CIA cia1->executeOneCycle(); cia2->executeOneCycle();
#define EXECUTE(x) EXEC_CPU(x) EXEC_FLOPPY(x) EXEC_CIA; 
#define EXECUTE_ONE cpu->executeOneCycle(); floppy->executeOneCycle(); cia1->executeOneCycle(); cia2->executeOneCycle();


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
	cycle = 1;
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

// Execute a single cycle
inline bool
C64::executeOneCycle()
{
	// determine cycle relative to the current rasterline
	cycle = (cpu->getCycles() % getCyclesPerRasterline()) + 1;
	
	switch(cycle) {
		case 1:
			beginOfRasterline();			
			vic->cycle1();
			EXECUTE_ONE;
			break;
		case 2: 
			vic->cycle2();
			EXECUTE_ONE;
			break;
		case 3: 
			vic->cycle3();
			EXECUTE_ONE;
			break;
		case 4: 
			vic->cycle4();
			EXECUTE_ONE;
			break;
		case 5: 
			vic->cycle5();
			EXECUTE_ONE;
			break;
		case 6: 
			vic->cycle6();
			EXECUTE_ONE;
			break;
		case 7: 
			vic->cycle7();
			EXECUTE_ONE;
			break;
		case 8: 
			vic->cycle8();
			EXECUTE_ONE;
			break;
		case 9: 
			vic->cycle9();
			EXECUTE_ONE;
			break;
		case 10: 
			vic->cycle10();
			EXECUTE_ONE;
			break;
		case 11: 
			vic->cycle11();
			EXECUTE_ONE;
			break;
		case 12: 
			vic->cycle12();
			EXECUTE_ONE;
			break;
		case 13: 
			vic->cycle13();
			EXECUTE_ONE;
			break;
		case 14: 
			vic->cycle14();
			EXECUTE_ONE;
			break;
		case 15: 
			vic->cycle15();
			EXECUTE_ONE;
			break;
		case 16: 
			vic->cycle16();
			EXECUTE_ONE;
			break;
		case 17: 
			vic->cycle17();
			EXECUTE_ONE;
			break;
		case 18: 
			vic->cycle18();
			EXECUTE_ONE;
			break;
		case 19: 
			vic->cycle19();
			EXECUTE_ONE;
			break;
		case 20: 
			vic->cycle20();
			EXECUTE_ONE;
			break;
		case 21: 
			vic->cycle21();
			EXECUTE_ONE;
			break;
		case 22: 
			vic->cycle22();
			EXECUTE_ONE;
			break;
		case 23: 
			vic->cycle23();
			EXECUTE_ONE;
			break;
		case 24: 
			vic->cycle24();
			EXECUTE_ONE;
			break;
		case 25: 
			vic->cycle25();
			EXECUTE_ONE;
			break;
		case 26: 
			vic->cycle26();
			EXECUTE_ONE;
			break;
		case 27: 
			vic->cycle27();
			EXECUTE_ONE;
			break;
		case 28: 
			vic->cycle28();
			EXECUTE_ONE;
			break;
		case 29: 
			vic->cycle29();
			EXECUTE_ONE;
			break;
		case 30: 
			vic->cycle30();
			EXECUTE_ONE;
			break;
		case 31: 
			vic->cycle31();
			EXECUTE_ONE;
			break;
		case 32: 
			vic->cycle32();
			EXECUTE_ONE;
			break;
		case 33: 
			vic->cycle33();
			EXECUTE_ONE;
			break;
		case 34: 
			vic->cycle34();
			EXECUTE_ONE;
			break;
		case 35: 
			vic->cycle35();
			EXECUTE_ONE;
			break;
		case 36: 
			vic->cycle36();
			EXECUTE_ONE;
			break;
		case 37: 
			vic->cycle37();
			EXECUTE_ONE;
			break;
		case 38: 
			vic->cycle38();
			EXECUTE_ONE;
			break;
		case 39: 
			vic->cycle39();
			EXECUTE_ONE;
			break;
		case 40: 
			vic->cycle40();
			EXECUTE_ONE;
			break;
		case 41: 
			vic->cycle41();
			EXECUTE_ONE;
			break;
		case 42: 
			vic->cycle42();
			EXECUTE_ONE;
			break;
		case 43: 
			vic->cycle43();
			EXECUTE_ONE;
			break;
		case 44: 
			vic->cycle44();
			EXECUTE_ONE;
			break;
		case 45: 
			vic->cycle45();
			EXECUTE_ONE;
			break;
		case 46: 
			vic->cycle46();
			EXECUTE_ONE;
			break;
		case 47: 
			vic->cycle47();
			EXECUTE_ONE;
			break;
		case 48: 
			vic->cycle48();
			EXECUTE_ONE;
			break;
		case 49: 
			vic->cycle49();
			EXECUTE_ONE;
			break;
		case 50: 
			vic->cycle50();
			EXECUTE_ONE;
			break;
		case 51: 
			vic->cycle51();
			EXECUTE_ONE;
			break;
		case 52: 
			vic->cycle52();
			EXECUTE_ONE;
			break;
		case 53: 
			vic->cycle53();
			EXECUTE_ONE;
			break;
		case 54: 
			vic->cycle54();
			EXECUTE_ONE;
			break;
		case 55: 
			vic->cycle55();
			EXECUTE_ONE;
			break;
		case 56: 
			vic->cycle56();
			EXECUTE_ONE;
			break;
		case 57: 
			vic->cycle57();
			EXECUTE_ONE;
			break;
		case 58: 
			vic->cycle58();
			EXECUTE_ONE;
			break;
		case 59: 
			vic->cycle59();
			EXECUTE_ONE;
			break;
		case 60: 
			vic->cycle60();
			EXECUTE_ONE;
			break;
		case 61: 
			vic->cycle61();
			EXECUTE_ONE;
			break;
		case 62: 
			vic->cycle62();
			EXECUTE_ONE;
			break;
		case 63: 
			vic->cycle63();
			EXECUTE_ONE;
			
			if (cycle >= getCyclesPerRasterline()) {
				// last cycle for PAL machines
				endOfRasterline();
				return true;
			}			
		case 64: 
			vic->cycle64();
			EXECUTE_ONE;			
			break;
		case 65: 
			vic->cycle65();
			EXECUTE_ONE;
			endOfRasterline();
			return true;
			
		default:
			// can't reach
			assert(false);
			return false;
	}	
	cycle++;
}

// Execute until the end of the rasterline
inline bool
C64::executeOneLine()
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


// --------------------------------------------------------------------------------
// C64 class
// --------------------------------------------------------------------------------

C64::C64(C64Listener *listener)
{
	debug("Creating virtual C64 at address %p...\n", this);

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

	// Create joysticks
	// NEEDS CLEANUP
	debug("try to load joystick\n");
	try {
		joystick1 = new Joystick( 0 );
		debug("joystick 1found \n");
	} catch( const char *ex ) {
		joystick1 = NULL;
		debug("Joystick 1 is NOT present\n");
	}
	
	try {
		joystick2 = new Joystick( 1 );
		debug("joystick 2 found\n");
	} catch( const char *ex ) {
	    joystick2 = NULL;
		debug("Joystick 2 is NOT present\n");
	}
		
	// Bind components
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
	vic->setCPU(cpu);
	vic->setMemory(mem);
	iec->setDrive(floppy);
	floppy->setIEC(iec);
	floppy->setC64(this);
	
	// Setup initial game port mapping
	// NEEDS CLEANUP, DONT DO THIS HERE!
	if (joystick1 == NULL && joystick2 == NULL) {
		// 0 joysticks connected
		setInputDevice(0, IPD_UNCONNECTED);
		setInputDevice(1, IPD_UNCONNECTED);
	}
	else if (joystick1 != NULL && joystick2 == NULL) {
		// 1 joysticks connected
		setInputDevice(0, IPD_JOYSTICK_1);
		setInputDevice(1, IPD_UNCONNECTED);
	}
	else if( joystick1 != NULL && joystick2 != NULL ) {
		// 2 joysticks connected
		setInputDevice(0, IPD_JOYSTICK_1);
		setInputDevice(1, IPD_JOYSTICK_2);
	} else {
		assert(0);
	}

	// Configure
	setNTSC();

	// Register listener and reset
	setListener(listener);
	reset();	
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
	
	debug("Cleaned up virtual C64 at address %p\n", this);
}

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

void C64::reset()
{
	debug ("Resetting virtual C64\n");
	suspend();
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
	archive = NULL;
	frame = 0;
	rasterline = 0;
	cycle = 1;
	resume();
}

void C64::fastReset()
{
	debug("NOTE: FAST RESET IS DISABLED, BECAUSE THE CURRENT IMAGE FORMAT IS NOT YET COMPATIBLE WITH THE INTERNAL MODEL\n");
#if 0
	debug ("Resetting virtual C64 (fast reset via image file)\n");

	if (loadSnapshot("ResetImage.VC64")) {
		debug("Reset image loaded.\n");	
	} else {
		debug("Cannot load reset image. Will do a hard reset...\n");
		reset();
	}
#endif
}

void C64::setListener(C64Listener *l)
{
	VirtualComponent::setListener(l);
	mem->setListener(l);
	cpu->setListener(l);
	vic->setListener(l);
	sid->setListener(l);
	cia1->setListener(l);	
	cia2->setListener(l);	
	iec->setListener(l);
	floppy->setListener(l);
	floppy->cpu->setListener(l);
	floppy->mem->setListener(l);
	floppy->via1->setListener(l);
	floppy->via2->setListener(l);	
}

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

void 
C64::threadCleanup()
{
	p = NULL;
	debug("Execution thread terminated\n");
}

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
C64::isRunnable() {
	return (numberOfMissingRoms() == 0);
}

void 
C64::run() {

	if (isHalted()) {

		// Check for ROM images
		if (getMissingRoms()) {
			getListener()->missingRomAction(getMissingRoms());
			return;
		}
		
		// Start execution thread
		pthread_create(&p, NULL, runThread, (void *)this);	

		// Power on sub components
		sid->run();
		
		// Notify listener
		getListener()->runAction();
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
		// Notify listener
		getListener()->haltAction();
		
	}
}

bool
C64::isHalted()
{
	return p == NULL;
}

// Load/save
bool 
C64::load(FILE *file)
{
	uint8_t major, minor;

	debug("Loading...\n");
	
	// Read header
	if ((char)read8(file) != 'V') return false;
	if ((char)read8(file) != 'I') return false;
	if ((char)read8(file) != '6') return false;
	if ((char)read8(file) != '4') return false;
	
	// Read version number
	major = read8(file);
	minor = read8(file);

	// Load data
	suspend();

	cpu->load(file);
	vic->load(file);
	sid->load(file);
	cia1->load(file);
	cia2->load(file);	
	mem->load(file);
	keyboard->load(file);
	iec->load(file);
	floppy->load(file);
	
	resume();
	return true;
}

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

bool 
C64::save(FILE *file)
{	
	suspend();

	debug("Saving...\n");

	// Write header
	write8(file, (uint8_t)'V');
	write8(file, (uint8_t)'I');
	write8(file, (uint8_t)'6');
	write8(file, (uint8_t)'4');
	
	// Write version number
	write8(file, 0);
	write8(file, 1);
	
	// Write data
	cpu->save(file);
	vic->save(file);
	sid->save(file);
	cia1->save(file);
	cia2->save(file);
	mem->save(file);
	keyboard->save(file);
	iec->save(file);
	floppy->save(file);
	
	resume();
	return true;
}

void
C64::setPAL()
{
	fps = VIC::PAL_REFRESH_RATE;
	noOfRasterlines = VIC::PAL_RASTERLINES; 
	cpuCyclesPerRasterline = VIC::PAL_CYCLES_PER_RASTERLINE;
	vic->setPAL();
	// TODO
	// sid->setPAL()
	frameDelay = (1000000 / fps);
}

void 
C64::setNTSC()
{
	fps = VIC::NTSC_REFRESH_RATE;
	noOfRasterlines = VIC::NTSC_RASTERLINES; 
	cpuCyclesPerRasterline = VIC::NTSC_CYCLES_PER_RASTERLINE;
	vic->setNTSC();
	// TODO
	// sid->setNTSC()
	frameDelay = (1000000 / fps);
}

bool 
C64::flushArchive(int item)
{
	uint16_t addr;
	int data;
	
	// Archive loaded?
	if (archive == NULL)
		return false;
	
	addr = archive->getDestinationAddrOfItem(item);
	archive->selectItem(item);
	while (1) {
		data = archive->getByte();
		if (data < 0) break;
		
		mem->pokeRam(addr, (uint8_t)data);
		if (addr == 0xFFFF) break;

		addr++;
	}
	return true;
}

bool 
C64::mountArchive()
{	
	// Archive loaded?
	if (archive == NULL)
		return false;

	// Insert disc
	floppy->insertDisc((D64Archive *)archive);

	return true;
}

bool 
C64::loadRom(const char *filename)
{
	bool result = false; 

	suspend(); 
	
	if (C64Memory::isBasicRom(filename)) {
		result = mem->loadBasicRom(filename);
		if (result) getListener()->loadRomAction(BASIC_ROM);
	}

	if (C64Memory::isCharRom(filename)) {
		result = mem->loadCharRom(filename);
		if (result) getListener()->loadRomAction(CHAR_ROM);
	}

	if (C64Memory::isKernelRom(filename)) {
		result = mem->loadKernelRom(filename);
		if (result) getListener()->loadRomAction(KERNEL_ROM);
	}
	
	if (VC1541Memory::is1541Rom(filename)) {
		result = floppy->mem->loadRom(filename);
		if (result) getListener()->loadRomAction(VC1541_ROM);
	}

	resume();
	return result;
}

bool 
C64::getWarpMode() 
{ 
	return warpMode; // || (enableWarpLoad && floppy->isRotating());
}

void
C64::setWarpMode(bool b)
{
	warpMode = b;
	restartTimer();
	getListener()->warpAction(getWarpMode());	
}

#if 0
void 
C64::setWarpLoad(bool b) 
{ 
	enableWarpLoad = b; 
	restartTimer();
	getListener()->warpAction(getWarpMode());	
}

void 
C64::setAlwaysWarp(bool b) { 
	alwaysWarp = b; 
	restartTimer();
	getListener()->warpAction(getWarpMode());	
}
#endif

void 
C64::dumpState() {
	
	suspend();
	
	debug("CPU:\n");
	debug("----\n");
	cpu->dumpState();
	
	debug("Memory:\n");
	debug("-------\n");
	mem->dumpState();
	
	debug("VIC:\n");
	debug("----\n");
	vic->dumpState();
	
	debug("SID:\n");
	debug("----\n");
	sid->dumpState();
	
	debug("CIA 1:\n");
	debug("------\n");
	cia1->dumpState();
	
	debug("CIA 2:\n");
	debug("------\n");
	cia2->dumpState();

	debug("IEC bus:\n");
	debug("--------\n");
	iec->dumpState();
	
	debug("Keyboard:\n");
	debug("---------\n");
	keyboard->dumpState();

	debug("Disk drive:\n");
	debug("-----------\n");
	floppy->dumpState();
	
	resume();
}

bool 
C64::loadSnapshot(const char *filename)
{
	FILE *file;
	bool result = false;
	
	assert(filename != NULL);

	file = fopen(filename, "r");
	if (file != NULL) {
		result = load(file);
		fclose(file);
	} else {
		debug("WARNING: Cannot read from file %s\n", filename);
	}
	
	return result;
}

bool 
C64::saveSnapshot(const char *filename)
{
	FILE *file;
	bool result = false;
	
	assert(filename != NULL);
	
	debug("Saving virtual machine state to file %s\n", filename);	
	file = fopen(filename, "w");
	if (file != NULL) {
		result = save(file);
		fclose(file);
	} else {
		debug("WARNING: Cannot write to file %s\n", filename);
	}
	return result;
}


void 
C64::scanJoysticks() {
	// check if a new joystick is present now
	if( joystick1 == NULL ) {
		try {
			joystick1 = new Joystick( 0 );
		} catch( const char *ex ) {
			joystick1 = NULL;
			debug("Joystick 1 is NOT present\n");
		}
	}	
	
	// check if a new joystick is present now
	if( joystick2 == NULL ) {
		try {
			joystick2 = new Joystick( 1 );
		} catch( const char *ex ) {
			joystick2 = NULL;
			debug("Joystick 2 is NOT present\n");
		}
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
	
		if (newDevice == IPD_JOYSTICK_1 && joystick1 == NULL)
			invalid = true;
		else if (newDevice == IPD_JOYSTICK_2 && joystick2 == NULL)
			invalid = true;
		else 
			invalid = false;
	} while (invalid);
				
	setInputDevice(portNo, newDevice);
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
C64::switchInputDevices()
{
	debug("Switching input devides\n");
	int tmp_port = port[0];
	port[0] = port[1];
	port[1] = tmp_port;
}

int
C64::getDeviceOfPort( int portNo ) {
	return port[portNo];
}
