// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "C64.h"

//
// Emulator thread
//

void 
threadTerminated(void* thisC64)
{
    assert(thisC64 != NULL);
    
    C64 *c64 = (C64 *)thisC64;
    c64->threadDidTerminate();
}

void 
*threadMain(void *thisC64) {
        
    assert(thisC64 != NULL);
    
    C64 *c64 = (C64 *)thisC64;
    c64->threadWillStart();
    bool success = true;
        
    // Configure thread properties...
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
    pthread_cleanup_push(threadTerminated, thisC64);
    
    // Prepare to run...
    c64->cpu.clearErrorState();
    c64->drive1.cpu.clearErrorState();
    c64->drive2.cpu.clearErrorState();
    c64->restartTimer();
    
    while (likely(success)) {
        pthread_testcancel();
        success = c64->executeOneFrame();
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
    debug(RUN_DEBUG, "Creating virtual C64 [%p]\n", this);

    p = NULL;
    warp = false;
    alwaysWarp = false;
    warpLoad = false;
    
    subComponents = vector<HardwareComponent *> {
        
        &mem,
        &cpu,
        &processorPort,
        &cia1, &cia2,
        &vic,
        &sid,
        &keyboard,
        &port1,
        &port2,
        &expansionport,
        &iec,
        &drive1,
        &drive2,
        &datasette,
        &mouse
    };
    
    // Register snapshot items
    SnapshotItem items[] = {
 
        { &frame,              sizeof(frame),              CLEAR_ON_RESET },
        { &rasterLine,         sizeof(rasterLine),         CLEAR_ON_RESET },
        { &rasterCycle,        sizeof(rasterCycle),        CLEAR_ON_RESET },
        { &frequency,          sizeof(frequency),          KEEP_ON_RESET  },
        { &durationOfOneCycle, sizeof(durationOfOneCycle), KEEP_ON_RESET  },
        { &warp,               sizeof(warp),               CLEAR_ON_RESET },
        { &ultimax,            sizeof(ultimax),            CLEAR_ON_RESET },
        
        { NULL,             0,                       0 }};
    
    registerSnapshotItems(items, sizeof(items));
        
    // Set up the initial state
    initialize();
    _reset();

    // Initialize mach timer info
    mach_timebase_info(&timebase);
    
    // Initialize mutexes
    pthread_mutex_init(&threadLock, NULL);
}

C64::~C64()
{
    debug(RUN_DEBUG, "Destroying C64[%p]\n", this);
    powerOffEmulator();
    
    pthread_mutex_destroy(&threadLock);
}

C64Configuration
C64::getConfig()
{
    C64Configuration config;

    config.vic = vic.getConfig();
    config.cia1 = cia1.getConfig();
    config.cia2 = cia2.getConfig();
    config.sid = sid.getConfig();
    config.mem = mem.getConfig();

    // Assure both CIAs are configured equally
    assert(config.cia1.revision == config.cia1.revision);
    assert(config.cia1.timerBBug == config.cia2.timerBBug);

    return config;
}

long
C64::getConfig(ConfigOption option)
{
    switch (option) {
            
        case OPT_VIC_REVISION: return (long)vic.getRevision();
        case OPT_GRAY_DOT_BUG: return (long)vic.getGrayDotBug();
            
        case OPT_CIA_REVISION: return (long)cia1.getRevision();
        case OPT_TIMER_B_BUG:  return (long)cia1.getTimerBBug();
            
        case OPT_SID_REVISION: return (long)sid.getRevision();
        case OPT_SID_FILTER:   return (long)sid.getAudioFilter();
            
        case OPT_GLUE_LOGIC:   return (long)vic.getGlueLogic();
            
        case OPT_SID_ENGINE:   return (long)sid.getEngine();
        case OPT_SID_SAMPLING: return (long)sid.getSamplingMethod();
            
        case OPT_RAM_PATTERN:  return (long)mem.getRamPattern();
            
        default:
            assert(false);
            return 0;
    }
}

long
C64::getDriveConfig(DriveID id, ConfigOption option)
{
    assert(isDriveID(id));
    
    switch (option) {
            
        case OPT_DRIVE_CONNECT: return id == DRIVE8; // TODO
        case OPT_DRIVE_TYPE: return DRIVE_VC1541II; // TODO
            
        default:
            assert(false);
            return 0;
    }
}

bool
C64::configure(ConfigOption option, long value)
{
    suspend();
    
    C64Configuration current = getConfig();
    
    switch (option) {

        case OPT_VIC_REVISION:
            
            if (!isVICRevision(value)) {
                warn("Invalid VIC revision: %d\n", value);
                goto error;
            }
            
            if (current.vic.revision == value) goto exit;
            vic.setRevision((VICRevision)value);
            goto success;

        case OPT_GRAY_DOT_BUG:

            if (current.vic.grayDotBug == value) goto exit;
            vic.setGrayDotBug(value);
            goto success;
                        
        case OPT_CIA_REVISION:
            
            if (!isCIARevision(value)) {
                warn("Invalid CIA revision: %d\n", value);
                goto error;
            }
                        
            assert(cia1.getRevision() == cia2.getRevision());
            if (current.cia1.revision == value) goto exit;
            cia1.setRevision((CIARevision)value);
            cia2.setRevision((CIARevision)value);
            goto success;

        case OPT_TIMER_B_BUG:
            
            assert(cia1.getTimerBBug() == cia2.getTimerBBug());
            if (current.cia1.timerBBug == value) goto exit;
            cia1.setTimerBBug(value);
            cia2.setTimerBBug(value);
            goto success;
            
        case OPT_SID_REVISION:
            
            if (!isSIDRevision(value)) {
                warn("Invalid SID revision: %d\n", value);
                goto error;
            }

            if (current.sid.revision == value) goto exit;
            sid.setRevision((SIDRevision)value);
            goto success;
            
        case OPT_SID_FILTER:
                        
            if (current.sid.filter == value) goto exit;
            sid.setFilter(value);
            goto success;

            
        case OPT_GLUE_LOGIC:
            
            if (!isGlueLogic(value)) {
                warn("Invalid glue logic type: %d\n", value);
                goto error;
            }
            
            if (current.vic.glueLogic == value) goto exit;
            vic.setGlueLogic((GlueLogic)value);
            goto success;

        case OPT_SID_ENGINE:

            debug("OPT_SID_ENGINE: %d\n", value); 
            if (!isAudioEngine(value)) {
                warn("Invalid audio engine: %d\n", value);
                goto error;
            }
            
            if (current.sid.engine == value) goto exit;
            sid.setEngine((SIDEngine)value);
            goto success;

        case OPT_SID_SAMPLING:
            
            if (!isSamplingMethod(value)) {
                warn("Invalid sampling method: %d\n", value);
                goto error;
            }
            
            if (current.sid.sampling == value) goto exit;
            sid.setSamplingMethod((SamplingMethod)value);
            goto success;
            
        case OPT_RAM_PATTERN:

            if (!isRamPattern(value)) {
                warn("Invalid RAM pattern: %d\n", value);
                goto error;
            }
            
            if (current.mem.ramPattern == value) goto exit;
            mem.setRamPattern((RamPattern)value);
            goto success;
            
        default: assert(false);
    }
    
    error:
        resume();
        return false;
        
    success:
        putMessage(MSG_CONFIG);
        
    exit:
        resume();
        return true;
}

bool
C64::configureDrive(DriveID id, ConfigOption option, long value)
{
    assert(isDriveID(id));

    suspend();

    Drive &drive = id == DRIVE8 ? drive1 : drive2;
    DriveConfig current = drive.getConfig();
        
    switch (option) {
            
        case OPT_DRIVE_CONNECT:
            
            if (current.connected == value) goto exit;
            drive.setConnected(value);
            goto success;
            
        case OPT_DRIVE_TYPE:
            
            if (!isDriveType(value)) {
                warn("Invalid drive type: %d\n", value);
                goto error;
            }
                        
            if (current.type == value) goto exit;
            drive.setType((DriveType)value);
            goto success;

        default: assert(false);
    }
    
error:
    resume();
    return false;
    
success:
    putMessage(MSG_CONFIG);
    
exit:
    resume();
    return true;
}

void
C64::prefix()
{
    fprintf(stderr, "[%lld] (%3d,%3d) %04X ", frame, rasterLine, rasterCycle, cpu.getPC());
}

void
C64::reset()
{
    suspend();
    assert(!isRunning());
        
    // Execute the standard reset routine
    HardwareComponent::reset();
    
    // Inform the GUI
    putMessage(MSG_RESET);
    
    resume();
}

void
C64::_reset()
{
    debug(RUN_DEBUG, "Resetting virtual C64[%p]\n", this);
    
    // Clear snapshot items marked with 'CLEAR_ON_RESET'
     if (snapshotItems != NULL)
         for (unsigned i = 0; snapshotItems[i].data != NULL; i++)
             if (snapshotItems[i].flags & CLEAR_ON_RESET)
                 memset(snapshotItems[i].data, 0, snapshotItems[i].size);
    
    // Initialize processor port
    mem.poke(0x0000, 0x2F);  // Data direction
    mem.poke(0x0001, 0x1F);  // IO port, set default memory layout

    // Initialize program counter
    cpu.regPC = mem.resetVector();
    
    rasterCycle = 1;
    nanoTargetTime = 0UL;
    ping();
}

void
C64::_powerOn()
{
    debug(RUN_DEBUG, "Power on\n");
        
    putMessage(MSG_POWER_ON);
}

void
C64::_powerOff()
{
    debug("Power off\n");
    
    putMessage(MSG_POWER_OFF);
}

void
C64::_run()
{
    debug("_run\n");
    
    // Start the emulator thread
    pthread_create(&p, NULL, threadMain, (void *)this);
    
    // Inform the GUI
    putMessage(MSG_RUN);
}

void
C64::_pause()
{
    if (p) {
        
        // Cancel emulator thread
        pthread_cancel(p);
        
        // Wait until thread terminates
        pthread_join(p, NULL);
        debug("Thread stopped\n");

        // Finish the current command (to reach a clean state)
        stepInto();
    }
    
    // When we reach this line, the emulator thread is already gone
    assert(p == NULL);
    
    // Inform the GUI
    putMessage(MSG_PAUSE);
}

void C64::_ping()
{
    putMessage(warp ? MSG_WARP_ON : MSG_WARP_OFF);
    putMessage(alwaysWarp ? MSG_ALWAYS_WARP_ON : MSG_ALWAYS_WARP_OFF);
}

void
C64::_setClockFrequency(u32 value)
{
    frequency = value;
    durationOfOneCycle = 10000000000 / value;
}

void
C64::_dump() {
    msg("C64:\n");
    msg("----\n\n");
    msg("              Machine type : %s\n", vic.isPAL() ? "PAL" : "NTSC");
    msg("         Frames per second : %f\n", vic.getFramesPerSecond());
    msg("     Rasterlines per frame : %d\n", vic.getRasterlinesPerFrame());
    msg("     Cycles per rasterline : %d\n", vic.getCyclesPerRasterline());
    msg("             Current cycle : %llu\n", cpu.cycle);
    msg("             Current frame : %d\n", frame);
    msg("        Current rasterline : %d\n", rasterLine);
    msg("  Current rasterline cycle : %d\n", rasterCycle);
    msg("              Ultimax mode : %s\n\n", getUltimax() ? "YES" : "NO");
    msg("warp, warpLoad, alwaysWarp : %d %d %d\n", warp, warpLoad, alwaysWarp);
    msg("\n");
}

void
C64::suspend()
{
    synchronized {
        
        debug(RUN_DEBUG, "Suspending (%d)...\n", suspendCounter);
        
        if (suspendCounter || isRunning()) {
            
            // Acquire the thread lock
            requestThreadLock();
            pthread_mutex_lock(&threadLock);
            
            // At this point, the emulator must be paused or powered off
            assert(!isRunning());
            
            suspendCounter++;
        }
    }
}

void
C64::resume()
{
    synchronized {
        
        debug(RUN_DEBUG, "Resuming (%d)...\n", suspendCounter);
        
        if (suspendCounter && --suspendCounter == 0) {
            
            // Acquire the thread lock
            requestThreadLock();
            pthread_mutex_lock(&threadLock);
            
            run();
        }
    }
}

void
C64::requestThreadLock()
{
    if (state == STATE_RUNNING) {

        // The emulator thread is running
        assert(p != NULL);
        
        // Free the thread lock by terminating the thread
        // signalStop();
        
        // Cancel emulator thread
        pthread_cancel(p);
        
        // Wait until thread terminates
        pthread_join(p, NULL);
        debug("Thread stopped\n");
        assert(p == NULL);
        
        // Acquire the lock
        
        
        // Finish the current command (to reach a clean state)
        stepInto();
        
    } else {
        
        // There must be no emulator thread
        assert(p == NULL);
        
        // It's save to free the lock immediately
        pthread_mutex_unlock(&threadLock);
    }
}

void
C64::powerOnEmulator()
{
    synchronized {
        
#ifdef BOOT_DISK
        
        ADFFile *adf = ADFFile::makeWithFile(BOOT_DISK);
        if (adf) {
            Disk *disk = Disk::makeWithFile(adf);
            df0.ejectDisk();
            df0.insertDisk(disk);
            df0.setWriteProtection(false);
        }
        
#endif
        
#ifdef INITIAL_BREAKPOINT
        
        debugMode = true;
        cpu.debugger.breakpoints.addAt(INITIAL_BREAKPOINT);
        
#endif
        
        if (isReady()) {
            
            // Acquire the thread lock
            requestThreadLock();
            pthread_mutex_lock(&threadLock);
            
            powerOn();
        }
    }
}

void
C64::powerOffEmulator()
{
    synchronized {
        
        // Acquire the thread lock
        requestThreadLock();
        pthread_mutex_lock(&threadLock);
        
        powerOff();
    }
}

void
C64::runEmulator()
{
    synchronized {
        
        if (isReady()) {
            
            // Acquire the thread lock
            requestThreadLock();
            pthread_mutex_lock(&threadLock);
            
            run();
        }
    }
}

void
C64::pauseEmulator()
{
    synchronized {
        
        // Acquire the thread lock
        requestThreadLock();
        pthread_mutex_lock(&threadLock);
        
        // At this point, the emulator is already paused or powered off
        assert(!isRunning());
    }
}

bool
C64::isReady(ErrorCode *error)
{
    if (!hasBasicRom() || !hasCharRom() || !hasKernalRom()) {
        if (error) *error = ERR_ROM_MISSING;
        return false;
    }
    
    if (hasMega65BasicRom() && hasMega65KernalRom()) {
        if (strcmp(mega65BasicRev(), mega65KernalRev()) != 0) {
            if (error) *error = ERR_ROM_MEGA65_MISMATCH;
            return false;
        }
    }
    
    return true;
}

C64Model
C64::getModel()
{
    // Look for known configurations
    for (unsigned i = 0; i < sizeof(configurations) / sizeof(C64ConfigurationDeprecated); i++) {
        if (vic.getRevision() == configurations[i].vic &&
            vic.getGrayDotBug() == configurations[i].grayDotBug &&
            cia1.getRevision() == configurations[i].cia &&
            cia1.getTimerBBug() == configurations[i].timerBBug &&
            sid.getRevision() == configurations[i].sid &&
            vic.getGlueLogic() == configurations[i].glue &&
            mem.getRamPattern() == configurations[i].pattern) {
            return (C64Model)i;
        }
    }
    
    // We've got a non-standard configuration
    return C64_CUSTOM; 
}

void
C64::setModel(C64Model m)
{
    if (m != C64_CUSTOM) {
        
        suspend();
        vic.setRevision(configurations[m].vic);
        vic.setGrayDotBug(configurations[m].grayDotBug);
        cia1.setRevision(configurations[m].cia);
        cia2.setRevision(configurations[m].cia);
        cia1.setTimerBBug(configurations[m].timerBBug);
        cia2.setTimerBBug(configurations[m].timerBBug);
        sid.setRevision(configurations[m].sid);
        sid.setFilter(configurations[m].sidFilter);
        vic.setGlueLogic(configurations[m].glue);
        mem.setRamPattern(configurations[m].pattern);
        resume();
    }
}

void
C64::updateVicFunctionTable()
{
    // Assign model independent execution functions
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
    
    // Assign model specific execution functions
    switch (vic.getRevision()) {
            
        case PAL_6569_R1:
        case PAL_6569_R3:
        case PAL_8565:
            
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
            break;
            
        case NTSC_6567_R56A:
            
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
            vicfunc[55] = &VIC::cycle55ntsc;
            vicfunc[57] = &VIC::cycle57ntsc;
            vicfunc[58] = &VIC::cycle58ntsc;
            vicfunc[59] = &VIC::cycle59ntsc;
            vicfunc[60] = &VIC::cycle60ntsc;
            vicfunc[61] = &VIC::cycle61ntsc;
            vicfunc[62] = &VIC::cycle62ntsc;
            vicfunc[63] = &VIC::cycle63ntsc;
            vicfunc[64] = &VIC::cycle64ntsc;
            vicfunc[65] = NULL;
            break;
            
        case NTSC_6567:
        case NTSC_8562:
            
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
            break;
            
        default:
            assert(false);
    }
}

/*
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
        if (!isReady()) {
            putMessage(MSG_ROM_MISSING);
            return;
        }
        
        // Power up sub components
        sid.run();
        
        // Start execution thread
        pthread_create(&p, NULL, threadMain, (void *)this);
    }
}

void
C64::halt()
{
    if (oldIsRunning()) {
        
        // Cancel execution thread
        pthread_cancel(p);
        // Wait until thread terminates
        pthread_join(p, NULL);
        // Finish the current command (to reach a clean state)
        stepInto();
    }
}
*/

void
C64::threadWillStart()
{
    debug(RUN_DEBUG, "Emulator thread started\n");
}

void
C64::threadDidTerminate()
{
    debug(RUN_DEBUG, "Emulator thread terminated\n");

    // Trash the thread pointer
    p = NULL;
        
    // Enter pause mode
    pause();
    
    // Release the thread lock
    pthread_mutex_unlock(&threadLock);
}

void
C64::stepInto()
{
    cpu.clearErrorState();
    drive1.cpu.clearErrorState();
    drive2.cpu.clearErrorState();

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
    drive1.cpu.clearErrorState();
    drive2.cpu.clearErrorState();
    
    // If the next instruction is a JSR instruction, ...
    if (mem.spypeek(cpu.getPC()) == 0x20) {
        // set a soft breakpoint at the next memory location.
        cpu.setSoftBreakpoint(cpu.getAddressOfNextInstruction());
        run();
        return;
    }

    // Otherwise, stepOver behaves like stepInto
    stepInto();
}

bool
C64::executeOneLine()
{
    if (rasterCycle == 1)
    beginRasterLine();
    
    int lastCycle = vic.getCyclesPerRasterline();
    for (unsigned i = rasterCycle; i <= lastCycle; i++) {
        if (!_executeOneCycle()) {
            if (i == lastCycle)
            endRasterLine();
            return false;
        }
    }
    endRasterLine();
    return true;
}

bool
C64::executeOneFrame()
{
    do {
        if (!executeOneLine())
        return false;
    } while (rasterLine != 0);
    return true;
}

bool
C64::executeOneCycle()
{
    bool isFirstCycle = rasterCycle == 1;
    bool isLastCycle = vic.isLastCycleInRasterline(rasterCycle);
    
    if (isFirstCycle) beginRasterLine();
    bool result = _executeOneCycle();
    if (isLastCycle) endRasterLine();
    
    return result;
}

bool
C64::_executeOneCycle()
{
    u8 result = true;
    u64 cycle = ++cpu.cycle;
    
    //  <---------- o2 low phase ----------->|<- o2 high phase ->|
    //                                       |                   |
    // ,-- C64 ------------------------------|-------------------|--,
    // |   ,-----,     ,-----,     ,-----,   |    ,-----,        |  |
    // |   |     |     |     |     |     |   |    |     |        |  |
    // '-->| VIC | --> | CIA | --> | CIA | --|--> | CPU | -------|--'
    //     |     |     |  1  |     |  2  |   |    |     |        |
    //     '-----'     '-----'     '-----'   |    '-----'        |
    //                                       v
    //                                 IEC bus update      IEC bus update
    //                                                           ^
    //                                       |    ,--------,     |
    //                                       |    |        |     |
    // ,-- Drive ----------------------------|--> | VC1541 | ----|--,
    // |                                     |    |        |     |  |
    // |                                     |    '--------'     |  |
    // '-------------------------------------|-------------------|--'
    
    // First clock phase (o2 low)
    (vic.*vicfunc[rasterCycle])();
    if (cycle >= cia1.wakeUpCycle) cia1.executeOneCycle(); else cia1.idleCounter++;
    if (cycle >= cia2.wakeUpCycle) cia2.executeOneCycle(); else cia2.idleCounter++;
    if (iec.isDirtyC64Side) iec.updateIecLinesC64Side();
    
    // Second clock phase (o2 high)
    result &= cpu.executeOneCycle();
    if (drive1.isConnected()) result &= drive1.execute(durationOfOneCycle);
    if (drive2.isConnected()) result &= drive2.execute(durationOfOneCycle);
    // if (iec.isDirtyDriveSide) iec.updateIecLinesDriveSide();
    datasette.execute();
    
    rasterCycle++;
    return result;
}

void
C64::beginRasterLine()
{
    // First cycle of rasterline
    if (rasterLine == 0) {
        vic.beginFrame();
    }
    vic.beginRasterline(rasterLine);
}

void
C64::endRasterLine()
{
    vic.endRasterline();
    rasterCycle = 1;
    rasterLine++;
    
    if (rasterLine >= vic.getRasterlinesPerFrame()) {
        rasterLine = 0;
        endFrame();
    }
}

void
C64::endFrame()
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
    port1.execute();
    port2.execute();

    // Update mouse coordinates
    mouse.execute();
    
    // Take a snapshot once in a while
    if (takeAutoSnapshots && autoSnapshotInterval > 0) {
        unsigned fps = (unsigned)vic.getFramesPerSecond();
        if (frame % (fps * autoSnapshotInterval) == 0) {
            takeAutoSnapshot();
        }
    }
    
    // Count some sheep (zzzzzz) ...
    if (!getWarp()) {
            synchronizeTiming();
    }
}

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
            sid.alignWritePtr();
            restartTimer();
        }
        
        putMessage(warp ? MSG_WARP_ON : MSG_WARP_OFF);
    }
    
    return warp;
}

void
C64::setAlwaysWarp(bool b)
{
    if (alwaysWarp != b) {
        
        alwaysWarp = b;
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
    u64 kernelNow = mach_absolute_time();
    u64 nanoNow = abs_to_nanos(kernelNow);
    
    nanoTargetTime = nanoNow + vic.getFrameDelay();
}

void
C64::synchronizeTiming()
{
    const u64 earlyWakeup = 1500000; /* 1.5 milliseconds */
    
    // Get current time in nano seconds
    u64 nanoAbsTime = abs_to_nanos(mach_absolute_time());
    
    // Check how long we're supposed to sleep
    i64 timediff = (i64)nanoTargetTime - (i64)nanoAbsTime;
    if (timediff > 200000000 || timediff < -200000000 /* 0.2 sec */) {
        
        // The emulator seems to be out of sync, so we better reset the
        // synchronization timer
        
        debug(RUN_DEBUG, "Synchronization lost: (%lld)\n", timediff);
        restartTimer();
    }
    
    // Convert nanoTargetTime into kernel unit
    i64 kernelTargetTime = nanos_to_abs(nanoTargetTime);
    
    // Sleep and update target timer
    i64 jitter = sleepUntil(kernelTargetTime, earlyWakeup);
    nanoTargetTime += vic.getFrameDelay();
    
    if (jitter > 1000000000 /* 1 sec */) {
        
        // The emulator did not keep up with the real time clock. Instead of
        // running behind for a long time, we reset the synchronization timer
        
        debug(RUN_DEBUG, "Jitter exceeds limit: (%lld)\n", jitter);
        restartTimer();
    }
}

void C64::loadFromSnapshotUnsafe(Snapshot *snapshot)
{    
    u8 *ptr;
    
    if (snapshot && (ptr = snapshot->getData())) {
        loadFromBuffer(&ptr);
        keyboard.releaseAll(); // Avoid constantly pressed keys
        ping();
    }
}

void
C64::loadFromSnapshotSafe(Snapshot *snapshot)
{
    debug(SNP_DEBUG, "C64::loadFromSnapshotSafe\n");

    suspend();
    loadFromSnapshotUnsafe(snapshot);
    resume();
}

bool
C64::restoreSnapshot(vector<Snapshot *> &storage, unsigned nr)
{
    Snapshot *snapshot = getSnapshot(storage, nr);
    
    if (snapshot) {
        loadFromSnapshotSafe(snapshot);
        return true;
    }
    
    return false;
}

size_t
C64::numSnapshots(vector<Snapshot *> &storage)
{
    return storage.size();
}

Snapshot *
C64::getSnapshot(vector<Snapshot *> &storage, unsigned nr)
{
    return nr < storage.size() ? storage.at(nr) : NULL;
    
}

void
C64::takeSnapshot(vector<Snapshot *> &storage)
{
    // Delete oldest snapshot if capacity limit has been reached
    if (storage.size() >= MAX_SNAPSHOTS) {
        deleteSnapshot(storage, MAX_SNAPSHOTS - 1);
    }
    
    Snapshot *snapshot = Snapshot::makeWithC64(this);
    storage.insert(storage.begin(), snapshot);
    putMessage(MSG_SNAPSHOT_TAKEN);
}

void
C64::deleteSnapshot(vector<Snapshot *> &storage, unsigned index)
{
    Snapshot *snapshot = getSnapshot(storage, index);
    
    if (snapshot) {
        delete snapshot;
        storage.erase(storage.begin() + index);
    }
}

u32
C64::basicRomCRC32()
{
    return hasBasicRom() ? crc32(mem.rom + 0xA000, 0x2000) : 0;
}

u32
C64::charRomCRC32()
{
    return hasCharRom() ? crc32(mem.rom + 0xD000, 0x1000) : 0;
}

u32
C64::kernalRomCRC32()
{
    return hasKernalRom() ? crc32(mem.rom + 0xE000, 0x2000) : 0;
}

u32
C64::vc1541RomCRC32()
{
    return hasVC1541Rom() ? crc32(drive1.mem.rom, 0x4000) : 0;
}

u64
C64::basicRomFNV64()
{
    return hasBasicRom() ? fnv_1a_64(mem.rom + 0xA000, 0x2000) : 0;
}

u64
C64::charRomFNV64()
{
    return hasCharRom() ? fnv_1a_64(mem.rom + 0xD000, 0x1000) : 0;
}

u64
C64::kernalRomFNV64()
{
    return hasKernalRom() ? fnv_1a_64(mem.rom + 0xE000, 0x2000) : 0;
}

u64
C64::vc1541RomFNV64()
{
    return hasVC1541Rom() ? fnv_1a_64(drive1.mem.rom, 0x4000) : 0;
}

const char *
C64::basicRomTitle()
{
    // Intercept if a MEGA65 Rom is installed
    if (hasMega65BasicRom()) return "M.E.G.A. C64 OpenROM";

    RomIdentifier rev = basicRomIdentifier();
    return rev == ROM_UNKNOWN ? "Unknown Basic Rom" : RomFile::title(rev);
}

const char *
C64::charRomTitle()
{
    // Intercept if a MEGA65 Rom is installed
    if (hasMega65CharRom()) return "M.E.G.A. C64 OpenROM";

    RomIdentifier rev = charRomIdentifier();
    return rev == ROM_UNKNOWN ? "Unknown Character Rom" : RomFile::title(rev);
}

const char *
C64::kernalRomTitle()
{
    // Intercept if a MEGA65 Rom is installed
    if (hasMega65BasicRom()) return "M.E.G.A. C64 OpenROM";

    RomIdentifier rev = kernalRomIdentifier();
    return rev == ROM_UNKNOWN ? "Unknown Kernal Rom" : RomFile::title(rev);
}

const char *
C64::vc1541RomTitle()
{
    RomIdentifier rev = vc1541RomIdentifier();
    return rev == ROM_UNKNOWN ? "Unknown Kernal Rom" : RomFile::title(rev);
}

const char *
C64::romSubTitle(u64 fnv)
{
    RomIdentifier rev = RomFile::identifier(fnv);

    if (rev != ROM_UNKNOWN) return RomFile::subTitle(rev);
    
    static char str[32];
    sprintf(str, "FNV %llx", fnv);
    return str;
}

const char *
C64::basicRomSubTitle()
{
    // Intercept if a MEGA65 Rom is installed
    if (hasMega65BasicRom()) return "Free Basic Replacement";

    return romSubTitle(basicRomFNV64());
}

const char *
C64::charRomSubTitle()
{
    // Intercept if a MEGA65 Rom is installed
    if (hasMega65CharRom()) return "Free Charset Replacement";
    
    return romSubTitle(charRomFNV64());
}

const char *
C64::kernalRomSubtitle()
{
    // Intercept if a MEGA65 Rom is installed
    if (hasMega65BasicRom()) return "Free Kernal Replacement";

    return romSubTitle(kernalRomFNV64());
}

const char *
C64::vc1541RomSubtitle()
{
    return romSubTitle(vc1541RomFNV64());
}

const char *
C64::basicRomRevision()
{
    // Intercept if a MEGA65 Rom is installed
    if (hasMega65BasicRom()) return mega65BasicRev();
    
    return RomFile::revision(basicRomIdentifier());
}

const char *
C64::charRomRevision()
{
    return RomFile::revision(charRomIdentifier());
}

const char *
C64::kernalRomRevision()
{
    // Intercept if a MEGA65 Rom is installed
    if (hasMega65KernalRom()) return mega65KernalRev();

    return RomFile::revision(kernalRomIdentifier());
}

const char *
C64::vc1541RomRevision()
{
    return RomFile::revision(vc1541RomIdentifier());
}

bool
C64::hasBasicRom()
{
    return (mem.rom[0xA000] | mem.rom[0xA001]) != 0x00;
}

bool
C64::hasCharRom()
{
    return (mem.rom[0xD000] | mem.rom[0xD001]) != 0x00;
}

bool
C64::hasKernalRom()
{
    return (mem.rom[0xE000] | mem.rom[0xE001]) != 0x00;
}

bool
C64::hasVC1541Rom()
{
    assert(drive1.mem.rom[0] == drive2.mem.rom[0]);
    assert(drive1.mem.rom[1] == drive2.mem.rom[1]);
    return (drive1.mem.rom[0] | drive1.mem.rom[1]) != 0x00;
}
bool
C64::hasMega65BasicRom()
{
    return mem.rom[0xBF52] == 'O' && mem.rom[0xBF53] == 'R';
}

bool
C64::hasMega65CharRom()
{
    return RomFile::isMega65Rom(charRomIdentifier());
}

bool
C64::hasMega65KernalRom()
{
    return mem.rom[0xE4B9] == 'O' && mem.rom[0xE4BA] == 'R';
}

char *
C64::mega65BasicRev()
{
    static char rev[17];
    rev[0] = 0;

    if (hasMega65BasicRom()) memcpy(rev, &mem.rom[0xBF55], 16);
    rev[16] = 0;

    return rev;
}

char *
C64::mega65KernalRev()
{
    static char rev[17];
    rev[0] = 0;

    if (hasMega65BasicRom()) memcpy(rev, &mem.rom[0xE4BC], 16);
    rev[16] = 0;

    return rev;
}

bool
C64::loadBasicRom(RomFile *file)
{
    assert(file != NULL);
    
    if (file->type() == BASIC_ROM_FILE) {
        debug("Flashing Basic Rom\n");
        file->flash(mem.rom, 0xA000);

        debug("hasMega65BasicRom() = %d\n", hasMega65BasicRom());
        debug("mega65BasicRev() = %s\n", mega65BasicRev());

        return true;
    }
    return false;
}

bool
C64::loadBasicRomFromBuffer(const u8 *buffer, size_t length)
{
    assert(buffer != NULL);
    
    if (RomFile *file = RomFile::makeWithBuffer(buffer, length)) {
        return loadBasicRom(file);
    }
    
    msg("Failed to read Basic Rom from buffer\n");
    return false;
}

bool
C64::loadBasicRomFromFile(const char *path)
{
    assert(path != NULL);
    
    if (RomFile *file = RomFile::makeWithFile(path)) {
        return loadBasicRom(file);
    }
    
    msg("Failed to read Basic Rom from %s\n", path);
    return false;
}

bool
C64::loadCharRom(RomFile *file)
{
    assert(file != NULL);
    
    if (file->type() == CHAR_ROM_FILE) {
        debug("Flashing Character Rom\n");
        file->flash(mem.rom, 0xD000);
        return true;
    }
    return false;
}

bool
C64::loadCharRomFromBuffer(const u8 *buffer, size_t length)
{
    assert(buffer != NULL);
    
    if (RomFile *file = RomFile::makeWithBuffer(buffer, length)) {
        return loadCharRom(file);
    }
    
    msg("Failed to read Character Rom from buffer\n");
    return false;
}

bool
C64::loadCharRomFromFile(const char *path)
{
    assert(path != NULL);
    
    if (RomFile *file = RomFile::makeWithFile(path)) {
        return loadCharRom(file);
    }
    
    msg("Failed to read Character Rom from %s\n", path);
    return false;
}

bool
C64::loadKernalRom(RomFile *file)
{
    assert(file != NULL);
    
    if (file->type() == KERNAL_ROM_FILE) {
        debug("Flashing Kernal Rom\n");
        file->flash(mem.rom, 0xE000);

        debug("hasMega65KernalRom() = %d\n", hasMega65KernalRom());
        debug("mega65KernalRev() = %s\n", mega65KernalRev());

        return true;
    }
    return false;
}

bool
C64::loadKernalRomFromBuffer(const u8 *buffer, size_t length)
{
    assert(buffer != NULL);
    
    if (RomFile *file = RomFile::makeWithBuffer(buffer, length)) {
        return loadKernalRom(file);
    }
    
    msg("Failed to read Kernal Rom from buffer\n");
    return false;
}

bool
C64::loadKernalRomFromFile(const char *path)
{
    assert(path != NULL);
    
    if (RomFile *file = RomFile::makeWithFile(path)) {
        return loadKernalRom(file);
    }
    
    msg("Failed to read Kernal Rom from %s\n", path);
    return false;
}

bool
C64::loadVC1541Rom(RomFile *file)
{
    assert(file != NULL);
    
    if (file->type() == VC1541_ROM_FILE) {
        debug("Flashing VC1541 Rom\n");
        file->flash(drive1.mem.rom);
        file->flash(drive2.mem.rom);
        return true;
    }
    return false;
}

bool
C64::loadVC1541RomFromBuffer(const u8 *buffer, size_t length)
{
    assert(buffer != NULL);
    
    if (RomFile *file = RomFile::makeWithBuffer(buffer, length)) {
        return loadVC1541Rom(file);
    }
    
    msg("Failed to read VC1541 Rom from buffer\n");
    return false;
}

bool
C64::loadVC1541RomFromFile(const char *path)
{
    assert(path != NULL);
    
    if (RomFile *file = RomFile::makeWithFile(path)) {
        return loadVC1541Rom(file);
    }
    
    msg("Failed to read VC1541 Rom from %s\n", path);
    return false;
}

void
C64::deleteBasicRom()
{
    memset(mem.rom + 0xA000, 0, 0x2000);
}

void
C64::deleteCharRom()
{
    memset(mem.rom + 0xD000, 0, 0x1000);
}

void
C64::deleteKernalRom()
{
    memset(mem.rom + 0xE000, 0, 0x2000);
}

void 
C64::deleteVC1541Rom()
{
    memset(drive1.mem.rom, 0, 0x4000);
    memset(drive2.mem.rom, 0, 0x4000);
}

bool
C64::saveBasicRom(const char *path)
{
    if (!hasBasicRom()) return false;

    RomFile *file = RomFile::makeWithBuffer(mem.rom + 0xA000, 0x2000);
    return file && file->writeToFile(path);
}

bool
C64::saveCharRom(const char *path)
{
    if (!hasCharRom()) return false;
    
    RomFile *file = RomFile::makeWithBuffer(mem.rom + 0xD000, 0x1000);
    return file && file->writeToFile(path);
}

bool
C64::saveKernalRom(const char *path)
{
    if (!hasKernalRom()) return false;
    
    RomFile *file = RomFile::makeWithBuffer(mem.rom + 0xE000, 0x2000);
    return file && file->writeToFile(path);
}

bool
C64::saveVC1541Rom(const char *path)
{
    if (!hasVC1541Rom()) return false;
    
    RomFile *file = RomFile::makeWithBuffer(drive1.mem.rom, 0x4000);
    return file && file->writeToFile(path);
}

bool
C64::flash(AnyFile *file)
{
    bool result = true;
    
    suspend();
    switch (file->type()) {
        
        case BASIC_ROM_FILE:
        file->flash(mem.rom, 0xA000);
        break;
        
        case CHAR_ROM_FILE:
        file->flash(mem.rom, 0xD000);
        break;
        
        case KERNAL_ROM_FILE:
        file->flash(mem.rom, 0xE000);
        break;
        
        case VC1541_ROM_FILE:
        file->flash(drive1.mem.rom);
        file->flash(drive2.mem.rom);
        break;
        
        case V64_FILE:
        loadFromSnapshotUnsafe((Snapshot *)file);
        break;
        
        default:
        assert(false);
        result = false;
    }
    resume();
    return result;
}

bool
C64::flash(AnyArchive *file, unsigned item)
{
    bool result = true;
    
    suspend();
    switch (file->type()) {
        
        case D64_FILE:
        case T64_FILE:
        case PRG_FILE:
        case P00_FILE:
        file->selectItem(item);
        file->flashItem(mem.ram);
        break;
        
        default:
        assert(false);
        result = false;
    }
    resume();
    return result;
}
