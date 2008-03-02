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

	C64 *c64 = (C64 *)thisC64;
	bool needsRedraw = false;   // Set to true by the video controller when a frame is finished
	uint64_t startTime;
	uint64_t startCycles;	
	int cyclePenalty;
	
	assert(thisC64 != NULL);
	
	debug("CPU execution thread started\n");	
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
	pthread_cleanup_push(threadCleanup, thisC64);

	// Prepare to run...
	c64->cpu->clearErrorState();
	c64->floppy->cpu->clearErrorState();
	startTime   = msec();
	startCycles = c64->cpu->getCycles();
	
	while (1) {

		int executedCycles, i;
	
		executedCycles = VIC::NTSC_CYCLES_PER_RASTERLINE;
		for (i = 0; i < executedCycles; i++) {
			c64->cpu->executeOneCycle(cyclePenalty); 
			cyclePenalty = 0;
			if (c64->cpu->getErrorState() != CPU::OK) break;
			c64->floppy->executeOneCycle();
			if (c64->floppy->cpu->getErrorState() != CPU::OK) break;			
		}
		if (c64->cpu->getErrorState() != CPU::OK) break;
		if (c64->floppy->cpu->getErrorState() != CPU::OK) break;			
		
		// Pass constrol to the virtual CIA chips
		c64->cia1->execute(executedCycles);
		c64->cia2->execute(executedCycles);
	
		// Pass control to the virtual display (draw next raster line)
		c64->vic->execute(&needsRedraw, &cyclePenalty);

		// tell SID how many cycles the cpu computed
		c64->sid->setExecutedCycles(executedCycles);
		
		if (needsRedraw) {

			// Pass control to the virtual sound chip
			// if VIC has drawn one entire frame, the SID chip generates sound samples
			c64->sid->execute(executedCycles);

			// Inform listener that a frame is complete
			needsRedraw = false;			

			// Increment the "time of day clocks" every tenth of a second
			if (c64->vic->getFrame() % 6 == 0) {
				c64->cia1->incrementTOD();
				c64->cia2->incrementTOD();
			}
						   
			// Synchronize time
			uint64_t elapsedCycles = c64->cpu->getCycles() - startCycles;
			uint64_t elapsedTime   = (uint64_t)((float)elapsedCycles / c64->cpu->getMHz());
			uint64_t timeToSleep   = elapsedTime - (msec() - startTime);
			if (timeToSleep > 0 && !c64->cpu->getWarpMode()) {
				sleepMicrosec(timeToSleep);	
			} else {
				// debug("NOT SLEEPING\n");
			}

			// After each second, we reset the startTime and startCycles.
			if (elapsedTime > 1000000) {
				startTime = msec();
				startCycles = c64->cpu->getCycles();
			}
				
			// We have reached a safe point to terminate the thread (if requested)
			pthread_testcancel();
		}
	}
	
	debug("Execution thread terminated\n");	

	pthread_cleanup_pop(1);
	pthread_exit(NULL);	
}


C64::C64()
{
	enableWarpLoad  = true;
	enableFastReset = true;

	// Create virtual memory
	mem = new C64Memory(); //C64Memory();
				
	// Create virtual CPU
	cpu = new CPU();		

	// Create virtual video controller
	vic = new VIC();

	// Create virtual sound interface device
	sid = new SID();

	// Create virtual complex interface adapters
	cia1 = new CIA1();
	cia2 = new CIA2();

	// Create virtual keyboard
	keyboard = new Keyboard();

	// Create joysticks
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

	// Create virtuel IEC-Bus
	iec = new IEC();

	// Create virtuel IEC-Bus
	floppy = new VC1541();
	
	// Create dummy listener for this virtual computer
	setListener(new C64Listener());

	// Bind components together
	cpu->setMemory(mem);
	mem->setVIC(vic);
	mem->setSID(sid);
	mem->setCIA1(cia1);
	mem->setCIA2(cia2);
	mem->setCPU(cpu);
	cia1->setCPU(cpu);
	cia2->setCPU(cpu);
	cia1->setKeyboard(keyboard);
	floppy->setIEC(iec);
	
	// scanJoysticks();
	
	// Setup initial game port mapping
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
				
	cia2->setVIC(vic);	
	cia2->setIEC(iec);
	vic->setCPU(cpu);
	vic->setMemory(mem);
	iec->setDrive(floppy);
	
	p = NULL;
	reset();
	debug("Created virtual C64 at address %p\n", this);
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
	
	printf("Cleaned up virtual C64 at address %p\n", this);
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

void C64::hardReset()
{
	suspend();
	
	mem->reset();
	cpu->reset();
	vic->reset();
	sid->reset();
	cia1->reset();
	cia2->reset();
	keyboard->reset();
	iec->reset();
	floppy->reset();
	
	archive = NULL;
	
	debug("Hard reset performed. Now going to resume...\n");	
	resume();
	debug("Done...\n");
}

void C64::fastReset()
{
	if (loadSnapshot("ResetImage.VC64")) {
		debug("Reset image loaded.\n");	
	} else {
		warn("Cannot load reset image!\n");
	}
}

void C64::reset()
{	
	// Remove...
	if (enableFastReset) {
		debug("Fast reset disabled in beta...\n");
		enableFastReset = false;
	}
	
	if (enableFastReset)
		fastReset();
	else
		hardReset();
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
	// Runstop/restore triggers a non maskable interrupt (NMI) (soft reset).
	cpu->setNMILineReset(); 
	debug("Soft reset performed.\n");
}

void 
C64::threadCleanup()
{
	p = NULL;
	debug("Execution thread terminated\n");
}

void 
C64::run() {
	if (isHalted()) {

		// Check for ROM images
		if (!mem->basicRomIsLoaded() || !mem->charRomIsLoaded() || !mem->kernelRomIsLoaded()) {
			getListener()->missingRomAction();
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
	bool result; 
	
	if (!C64Memory::isRom(filename))
		return false;

	suspend();
	result = mem->loadRom(filename);
	resume();
	
	return result;
}

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

	debug("Retrieving virtual machine state from file %s\n", filename);
	file = fopen(filename, "r");
	if (file != NULL) {
		result = load(file);
		fclose(file);
	} else {
		warn("Cannot read from file %s\n", filename);
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
		warn("Cannot write to file %s\n", filename);
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
C64::switchInputDevice( int portNo ) {

	int otherPortNo = (portNo == 0) ? 1 : 0;
	int newDevice = port[portNo];
	// int newOtherDevice = port[otherPortNo];
	bool invalid;
			
	assert(portNo == 0 || portNo == 1);
	assert(otherPortNo == 0 || otherPortNo == 1);	

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
