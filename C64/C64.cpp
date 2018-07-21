/*
 * (C) 2006 - 2018 Dirk W. Hoffmann. All rights reserved.
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
    
    c64->debug(2, "Execution thread terminated\n");
    c64->putMessage(MSG_HALT);
}

void 
*runThread(void *thisC64) {
        
    assert(thisC64 != NULL);
    
    C64 *c64 = (C64 *)thisC64;
    bool success = true;
    
    c64->debug(2, "Execution thread started\n");
    c64->putMessage(MSG_RUN);
    
    // Configure thread properties...
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
    pthread_cleanup_push(threadCleanup, thisC64);
    
    // Prepare to run...
    c64->cpu.clearErrorState();
    c64->floppy.cpu.clearErrorState();
    c64->restartTimer();
    
    while (likely(success)) {
        pthread_testcancel();
        pthread_mutex_lock(&c64->mutex);
        success = c64->executeOneFrame();
        pthread_mutex_unlock(&c64->mutex);
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
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&mutex, &attr);

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
        &floppy,
        &datasette,
        &mouse1350,
        &mouse1351,
        &neosMouse,
        &keyboard,
        &port1,
        &port2,
        &expansionport,
        NULL };
    
    registerSubComponents(subcomponents, sizeof(subcomponents));
    setC64(this);
    
    // Register snapshot items
    SnapshotItem items[] = {
 
        { &warp,                sizeof(warp),                CLEAR_ON_RESET },
        { &alwaysWarp,          sizeof(alwaysWarp),          CLEAR_ON_RESET },
        { &warpLoad,            sizeof(warpLoad),            KEEP_ON_RESET },
        { &durationOfHalfCycle, sizeof(durationOfHalfCycle), KEEP_ON_RESET },
        { &frame,               sizeof(frame),               CLEAR_ON_RESET },
        { &rasterline,          sizeof(rasterline),          CLEAR_ON_RESET },
        { &rasterlineCycle,     sizeof(rasterlineCycle),     CLEAR_ON_RESET },
        { &ultimax,             sizeof(ultimax),             CLEAR_ON_RESET },
        { NULL,                 0,                           0 }};
    
    registerSnapshotItems(items, sizeof(items));

    // Setup references
    cpu.mem = &mem;
    floppy.cpu.mem = &c64->floppy.mem;
    floppy.mem.iec = &c64->iec;
    floppy.mem.floppy = &c64->floppy;
    
    // Set initial hardware configuration
    mouse = &mouse1350;
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
    snapshotInterval = 3;

    reset();
}

C64::~C64()
{
    debug(1, "Destroying virtual C64[%p]\n", this);
    
    halt();
    pthread_mutex_destroy(&mutex);
}

void
C64::reset()
{
    debug(1, "Resetting virtual C64[%p]\n", this);
    
    // Reset all sub components
    VirtualComponent::reset();
    
    // Initialize processor port data direction register and processor port
    mem.poke(0x0000, 0x2F);  // Data direction
    mem.poke(0x0001, 0x1F);  // IO port, set default memory layout

    // Initialize program counter
    bool takeFromRom = mem.getPeekSource(0xFFFC) == M_ROM;
    cpu.setPC(takeFromRom ? 0xFCE2 : LO_HI(mem.spypeek(0xFFFC), mem.spypeek(0xFFFD)));
    
    rasterlineCycle = 1;
    nanoTargetTime = 0UL;
    ping();
}

void C64::ping()
{
    debug (2, "Pinging virtual C64[%p]\n", this);

    VirtualComponent::ping();
    putMessage(warp ? MSG_WARP_ON : MSG_WARP_OFF);
    putMessage(alwaysWarp ? MSG_ALWAYS_WARP_ON : MSG_ALWAYS_WARP_OFF);
}

void
C64::setClockFrequency(uint32_t frequency)
{
    VirtualComponent::setClockFrequency(frequency);
    
    durationOfHalfCycle = 1000000000000 / frequency / 2;
    debug("Duration of a CPU cycle is %lld pico seconds.\n", 2 * durationOfHalfCycle);
}

void 
C64::dumpState() {
    msg("C64:\n");
    msg("----\n\n");
    msg("              Machine type : %s\n", isPAL() ? "PAL" : "NTSC");
    msg("         Frames per second : %d\n", vic.getFramesPerSecond());
    msg("     Rasterlines per frame : %d\n", vic.getRasterlinesPerFrame());
    msg("     Cycles per rasterline : %d\n", vic.getCyclesPerRasterline());
    msg("             Current cycle : %llu\n", cpu.cycle);
    msg("             Current frame : %d\n", frame);
    msg("        Current rasterline : %d\n", rasterline);
    msg("  Current rasterline cycle : %d\n", rasterlineCycle);
    msg("              Ultimax mode : %s\n\n", getUltimax() ? "YES" : "NO");
    
    msg("warp, warpLoad, alwaysWarp : %d %d %d\n", warp, warpLoad, alwaysWarp);
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
    resume();
}

void 
C64::setNTSC()
{
    debug(2, "C64::setNTSC\n");

    suspend();
    vic.setChipModel(MOS6567_NTSC);
    resume();
}

void
C64::updateVicFunctionTable()
{
    // Initialize VIC function table
    vicfunc[0] = NULL;
    vicfunc[12] = &VIC::cycle12;
    vicfunc[13] = &VIC::cycle13;
    vicfunc[14] = &VIC::cycle14;
    vicfunc[15] = &VIC::cycle15;
    vicfunc[16] = &VIC::cycle16;
    vicfunc[17] = &VIC::cycle17;
    vicfunc[18] = &VIC::cycle18;
    for (unsigned cycle = 19; cycle <= 54; cycle++)
        vicfunc[cycle] = &VIC::cycle19to54;
    vicfunc[56] = &VIC::cycle56;
    
    if (isPAL()) {
        
        vicfunc[1] = &VIC::cycle1pal;
        vicfunc[2] = &VIC::cycle2pal;
        vicfunc[3] = &VIC::cycle3pal;
        vicfunc[4] = &VIC::cycle4pal;
        vicfunc[5] = &VIC::cycle5pal;
        vicfunc[6] = &VIC::cycle6pal;
        vicfunc[7] = &VIC::cycle7pal;
        vicfunc[8] = &VIC::cycle8pal;
        vicfunc[9] = &VIC::cycle9pal;
        vicfunc[10] = &VIC::cycle10pal;
        vicfunc[11] = &VIC::cycle11pal;
        vicfunc[55] = &VIC::cycle55pal;
        vicfunc[57] = &VIC::cycle57pal;
        vicfunc[58] = &VIC::cycle58pal;
        vicfunc[59] = &VIC::cycle59pal;
        vicfunc[60] = &VIC::cycle60pal;
        vicfunc[61] = &VIC::cycle61pal;
        vicfunc[62] = &VIC::cycle62pal;
        vicfunc[63] = &VIC::cycle63pal;
        vicfunc[64] = NULL;
        vicfunc[65] = NULL;

    } else {
        
        vicfunc[1] = &VIC::cycle1ntsc;
        vicfunc[2] = &VIC::cycle2ntsc;
        vicfunc[3] = &VIC::cycle3ntsc;
        vicfunc[4] = &VIC::cycle4ntsc;
        vicfunc[5] = &VIC::cycle5ntsc;
        vicfunc[6] = &VIC::cycle6ntsc;
        vicfunc[7] = &VIC::cycle7ntsc;
        vicfunc[8] = &VIC::cycle8ntsc;
        vicfunc[9] = &VIC::cycle9ntsc;
        vicfunc[10] = &VIC::cycle10ntsc;
        vicfunc[11] = &VIC::cycle11ntsc;
        vicfunc[55] = &VIC::cycle55ntsc;
        vicfunc[57] = &VIC::cycle57ntsc;
        vicfunc[58] = &VIC::cycle58ntsc;
        vicfunc[59] = &VIC::cycle59ntsc;
        vicfunc[60] = &VIC::cycle60ntsc;
        vicfunc[61] = &VIC::cycle61ntsc;
        vicfunc[62] = &VIC::cycle62ntsc;
        vicfunc[63] = &VIC::cycle63ntsc;
        vicfunc[64] = &VIC::cycle64ntsc;
        vicfunc[65] = &VIC::cycle65ntsc;
    }
}

void
C64::setMouseModel(MouseModel value)
{
    suspend();
    
    switch(value) {
        case MOUSE1350:
            mouse = &mouse1350;
            break;
        case MOUSE1351:
            mouse = &mouse1351;
            break;
        case NEOSMOUSE:
            mouse = &neosMouse;
            break;
        default:
            warn("Unknown mouse model selected.\n");
            mouse = &mouse1350;
    }
    
    mouse->reset();
    resume();
}

void
C64::connectMouse(unsigned port)
{
    assert(port <= 2);
    mousePort = port;
}

uint8_t
C64::mouseBits(unsigned port)
{
    if (mousePort != port) {
        return 0xFF;
    } else {
        return mouse->readControlPort();
    }
}

uint8_t
C64::potXBits()
{
    if (mousePort != 0) {
        switch (mouse->mouseModel()) {
            case MOUSE1350:
                return mouse1350.rightButton ? 0x00 : 0xFF;
            case MOUSE1351:
                return mouse1351.mouseXBits();
            case NEOSMOUSE:
                return neosMouse.rightButton ? 0xFF : 0x00;
        }
    }
    return 0xFF;
}

uint8_t
C64::potYBits()
{
    if (mousePort != 0 && mouse->mouseModel() == MOUSE1351) {
        return mouse1351.mouseYBits();
    } else {
        return 0xFF;
    }
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
C64::suspend()
{
    debug(2, "Suspending...\n");
    pthread_mutex_lock(&mutex);
}

void
C64::resume()
{
    debug(2, "Resuming...\n");
    pthread_mutex_unlock(&mutex);
}

void
C64::threadCleanup()
{
    p = NULL;
    debug(2, "Execution thread cleanup\n");
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
    cpu.clearErrorState();
    floppy.cpu.clearErrorState();
    
    // Wait until the execution of the next command has begun
    while (cpu.inFetchPhase()) executeOneCycle();

    // Finish the command
    while (!cpu.inFetchPhase()) executeOneCycle();
    
    // Execute the first microcycle (fetch phase) and stop there
    executeOneCycle();
}

void
C64::stepOver()
{
    cpu.clearErrorState();
    floppy.cpu.clearErrorState();
    
    // If the next instruction is a JSR instruction, ...
    if (mem.spypeek(cpu.getPC_at_cycle_0()) == 0x20) {
        // set a soft breakpoint at the next memory location.
        cpu.setSoftBreakpoint(cpu.getAddressOfNextInstruction());
        run();
        return;
    }

    // Otherwise, stepOver behaves like step
    step();
}

bool
C64::executeOneCycle()
{
    bool isFirstCycle = rasterlineCycle == 1;
    bool isLastCycle = vic.isLastCycleInRasterline(rasterlineCycle);
    
    if (isFirstCycle) beginOfRasterline();
    bool result = _executeOneCycle();
    if (isLastCycle) endOfRasterline();
    
    return result;
}

bool
C64::_executeOneCycle()
{
    uint8_t result = true;
    uint64_t cycle = ++cpu.cycle;
    
    //  <---------- o2 low phase ----------->|<- o2 high phase ->|
    //                                       |                   |
    // ,-- C64 ------------------------------|-------------------|--,
    // |   ,-----,     ,-----,     ,-----,   |    ,-----,        |  |
    // |   |     |     |     |     |     |   |    |     |        |  |
    // '-->| VIC | --> | CIA | --> | CIA | --|--> | CPU | -------|--'
    //     |     |     |  1  |     |  2  |   |    |     |        |
    //     '-----'     '-----'     '-----'   |    '-----'        |
    //                                       v                   |
    //                                 IEC bus update            |
    //                                       ^                   |
    //     ,--------,                        |    ,--------,     |
    //     |        |                        |    |        |     |
    // ,-->| VC1541 | -----------------------|--> | VC1541 | ----|--,
    // |   |        |                        |    |        |     |  |
    // |   '--------'                        |    '--------'     |  |
    // '-- Drive ----------------------------|-------------------|--'
    
    // First clock phase (o2 low)
    (vic.*vicfunc[rasterlineCycle])();
    if (cycle >= cia1.wakeUpCycle) cia1.executeOneCycle(); else cia1.idleCounter++;
    if (cycle >= cia2.wakeUpCycle) cia2.executeOneCycle(); else cia2.idleCounter++;
    result &= floppy.execute(durationOfHalfCycle);
    if (iec.isDirty) iec.updateIecLines();
    
    // Second clock phase (o2 high)
    result &= cpu.executeOneCycle();
    result &= floppy.execute(durationOfHalfCycle);
    datasette.execute();
    
    rasterlineCycle++;
    return result;
}

bool
C64::executeOneLine()
{
    if (rasterlineCycle == 1)
        beginOfRasterline();

    int lastCycle = vic.getCyclesPerRasterline();
    for (unsigned i = rasterlineCycle; i <= lastCycle; i++) {
        if (!_executeOneCycle()) {
            if (i == lastCycle)
                endOfRasterline();
            return false;
        }
    }
    endOfRasterline();
    return true;
}

bool
C64::executeOneFrame()
{
    do {
        if (!executeOneLine())
            return false;
    } while (rasterline != 0);
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
    sid.executeUntil(cpu.cycle);
    
    // Execute other components
    iec.execute();
    expansionport.execute();
    
    // Update mouse coordinates
    if (mousePort != 0) mouse->execute();
    
    // Take a snapshot once in a while
    if (snapshotInterval > 0 &&
        frame % (vic.getFramesPerSecond() * snapshotInterval) == 0) {
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

/*
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
*/

bool
C64::getWarp()
{
    bool newValue = (warpLoad && iec.isBusy()) || alwaysWarp;
    
    if (newValue != warp) {
        warp = newValue;
        
        /* Warping has the unavoidable drawback that audio playback gets out of
         * sync. To cope with this issue, we silence SID during warp mode and
         * fade in smoothly after warping has ended.
         */
        
        if (warp) {
            // Quickly fade out SID
            sid.rampDown();
            
        } else {
            // Smoothly fade in SID
            sid.rampUp();
            restartTimer();
        }
        
        putMessage(warp ? MSG_WARP_ON : MSG_WARP_OFF);
    }
    
    return warp;
}



/*
void
C64::updateWarp()
{
    bool newValue = (warpLoad && iec.isBusy()) || alwaysWarp;

    if (newValue == warp)
        return;
    
    warp = newValue;
    
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
    
    putMessage(warp ? MSG_WARP_ON : MSG_WARP_OFF);
}
*/

void
C64::setAlwaysWarp(bool b)
{
    if (alwaysWarp != b) {
        
        alwaysWarp = b;
        // updateWarp();
        putMessage(b ? MSG_ALWAYS_WARP_ON : MSG_ALWAYS_WARP_OFF);
    }
}

void
C64::setWarpLoad(bool b)
{
    warpLoad = b;
    /*
    if (warpLoad != b) {
        
        warpLoad = b;
        updateWarp();
    }
    */
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
    
    // Get current time in nano seconds
    uint64_t nanoAbsTime = abs_to_nanos(mach_absolute_time());
    
    // Check how long we're supposed to sleep
    int64_t timediff = (int64_t)nanoTargetTime - (int64_t)nanoAbsTime;
    if (timediff > 200000000 || timediff < -200000000 /* 0.2 sec */) {
        
        // The emulator seems to be out of sync, so we better reset the
        // synchronization timer
        
        debug(2, "Emulator lost synchronization (%lld). Restarting timer.\n", timediff);
        restartTimer();
    }
    
    // Convert nanoTargetTime into kernel unit
    int64_t kernelTargetTime = nanos_to_abs(nanoTargetTime);
    
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
//! @functiongroup Handling snapshots
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
C64::mount(File *file)
{
    bool result = true;
    
    suspend();
    switch (file->type()) {
       
        case CRT_CONTAINER:
            result = attachCartridgeAndReset((CRTFile *)file);
            break;

        case D64_CONTAINER:
        case T64_CONTAINER:
        case PRG_CONTAINER:
        case P00_CONTAINER:
        case G64_CONTAINER:
        case NIB_CONTAINER:
            result = insertDisk((Archive *)file);
            break;
    
        case TAP_CONTAINER:
            result = insertTape((TAPFile *)file);
            break;
            
        default: 
            assert(false); // not mountable
            result = false;
    }
    resume();
    return result;
}

bool
C64::flash(File *file, unsigned item)
{
    bool result = true;
    
    suspend();
    switch (file->type()) {
            
        case BASIC_ROM_FILE:
            ((ROMFile *)file)->flash(mem.rom + 0xA000);
            break;
            
        case CHAR_ROM_FILE:
            ((ROMFile *)file)->flash(mem.rom + 0xD000);
            break;
            
        case KERNAL_ROM_FILE:
            ((ROMFile *)file)->flash(mem.rom + 0xE000);
            break;
            
        case VC1541_ROM_FILE:
            ((ROMFile *)file)->flash(floppy.mem.rom);
            break;
                    
        case V64_CONTAINER:
            loadFromSnapshotUnsafe((Snapshot *)file);
            break;
            
        case D64_CONTAINER:
        case T64_CONTAINER:
        case PRG_CONTAINER:
        case P00_CONTAINER:
            result = flushArchive((Archive *)file, item);
            break;
            
        default:
            assert(false); // not mountable
            result = false;
    }
    resume();
    return result;
}

bool
C64::loadRom(const char *filename)
{
    bool result;
    bool wasRunnable = isRunnable();
    ROMFile *rom = ROMFile::makeRomFileWithFile(filename);
    
    if (!rom) {
        warn("Failed to read ROM image file %s\n", filename);
        return false;
    }
    
    result = flash(rom);
    
    if (result) {
        debug(2, "Loaded ROM image %s.\n", filename);
    } else {
        debug(2, "Failed to flush ROM image %s.\n", filename);
    }
    
    if (!wasRunnable && isRunnable())
        putMessage(MSG_READY_TO_RUN);
    
    delete rom;
    return result;
}


// OLD STUFF


bool 
C64::flushArchive(Archive *a, int item)
{
    uint16_t addr;
    int data;
    
    if (a == NULL)
        return false;
    
    addr = a->getDestinationAddrOfItem(item);
    debug("Flushing item %d at addr: %04X %d\n", item, addr, addr);
    a->selectItem(item);
    while (1) {
        data = a->getByte();
        if (data < 0) break;
        mem.ram[addr] = (uint8_t)data;
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
C64::insertTape(TAPFile *a)
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
C64::attachCartridgeAndReset(CRTFile *container)
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
