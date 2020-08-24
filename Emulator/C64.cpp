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
    
    // Inform the C64 that the thread has been canceled
    C64 *c64 = (C64 *)thisC64;
    c64->threadDidTerminate();
}

void 
*threadMain(void *thisC64) {
    
    assert(thisC64 != NULL);
    
    // Inform the C64 that the thread is about to start
    C64 *c64 = (C64 *)thisC64;
    c64->threadWillStart();
    
    // Configure thread properties...
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
    pthread_cleanup_push(threadTerminated, thisC64);
    
    // Enter the run loop
    c64->runLoop();
    
    // Clean up and exit
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
    
    subComponents = vector<HardwareComponent *> {
        
        &mem,
        &cpu,
        &cia1, &cia2,
        &vic,
        &sid,
        &keyboard,
        &port1,
        &port2,
        &expansionport,
        &iec,
        &drive8,
        &drive9,
        &datasette,
        &mouse
    };
    
    // Set up the initial state
    initialize();
    _reset();
    
    // Initialize mach timer info
    mach_timebase_info(&timebase);
    
    // Initialize mutexes
    pthread_mutex_init(&threadLock, NULL);
    pthread_mutex_init(&stateChangeLock, NULL);
}

C64::~C64()
{
    debug(RUN_DEBUG, "Destroying C64[%p]\n", this);
    powerOff();
    
    pthread_mutex_destroy(&threadLock);
    pthread_mutex_destroy(&stateChangeLock);
}

void
C64::setInspectionTarget(InspectionTarget target)
{
    assert(isInspectionTarget(target));
    inspectionTarget = target;
}

void
C64::clearInspectionTarget()
{
    inspectionTarget = INSPECT_NONE;
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
C64::getConfigItem(ConfigOption option)
{
    switch (option) {
            
        case OPT_VIC_REVISION:
        case OPT_GRAY_DOT_BUG:
        case OPT_GLUE_LOGIC:
            return vic.getConfigItem(option);
                        
        case OPT_CIA_REVISION: return (long)cia1.getRevision();
        case OPT_TIMER_B_BUG:  return (long)cia1.getTimerBBug();
            
        case OPT_SID_REVISION: return (long)sid.getRevision();
        case OPT_SID_FILTER:   return (long)sid.getAudioFilter();
                        
        case OPT_SID_ENGINE:   return (long)sid.getEngine();
        case OPT_SID_SAMPLING: return (long)sid.getSamplingMethod();
            
        case OPT_RAM_PATTERN:  return (long)mem.getRamPattern();
            
        default:
            assert(false);
            return 0;
    }
}

long
C64::getConfigItem(DriveID id, ConfigOption option)
{
    assert(isDriveID(id));
    
    Drive &drive = id == DRIVE8 ? drive8 : drive9;
    
    switch (option) {
            
        case OPT_DRIVE_CONNECT: return drive.isConnected();
        case OPT_DRIVE_TYPE:    return drive.getType();
            
        default:
            assert(false);
            return 0;
    }
}

bool
C64::configure(ConfigOption option, long value)
{
    C64Configuration current = getConfig();
    
    switch (option) {
                    
        case OPT_CIA_REVISION:
            
            if (!isCIARevision(value)) {
                warn("Invalid CIA revision: %d\n", value);
                goto error;
            }
            
            assert(cia1.getRevision() == cia2.getRevision());
            if (current.cia1.revision == value) goto exit;
            suspend();
            cia1.setRevision((CIARevision)value);
            cia2.setRevision((CIARevision)value);
            resume();
            goto success;
            
        case OPT_TIMER_B_BUG:
            
            assert(cia1.getTimerBBug() == cia2.getTimerBBug());
            if (current.cia1.timerBBug == value) goto exit;
            suspend();
            cia1.setTimerBBug(value);
            cia2.setTimerBBug(value);
            resume();
            goto success;
            
        case OPT_SID_REVISION:
            
            if (!isSIDRevision(value)) {
                warn("Invalid SID revision: %d\n", value);
                goto error;
            }
            
            if (current.sid.revision == value) goto exit;
            suspend();
            sid.setRevision((SIDRevision)value);
            resume();
            goto success;
            
        case OPT_SID_FILTER:
            
            if (current.sid.filter == value) goto exit;
            suspend();
            sid.setFilter(value);
            resume();
            goto success;
        
        case OPT_SID_ENGINE:
            
            debug("OPT_SID_ENGINE: %d\n", value);
            if (!isAudioEngine(value)) {
                warn("Invalid audio engine: %d\n", value);
                goto error;
            }
            
            if (current.sid.engine == value) goto exit;
            suspend();
            sid.setEngine((SIDEngine)value);
            resume();
            goto success;
            
        case OPT_SID_SAMPLING:
            
            if (!isSamplingMethod(value)) {
                warn("Invalid sampling method: %d\n", value);
                goto error;
            }
            
            if (current.sid.sampling == value) goto exit;
            suspend();
            sid.setSamplingMethod((SamplingMethod)value);
            resume();
            goto success;
            
        case OPT_RAM_PATTERN:
            
            if (!isRamPattern(value)) {
                warn("Invalid RAM pattern: %d\n", value);
                goto error;
            }
            
            if (current.mem.ramPattern == value) goto exit;
            suspend();
            mem.setRamPattern((RamPattern)value);
            resume();
            goto success;
            
        default:
            
            //
            // NEW CODE (EVERYTHING ELSE WILL GO AWAY)
            //
            
            // Propagate configuration request to all components
            bool changed = HardwareComponent::configure(option, value);
            
            // Inform the GUI if the configuration has changed
            if (changed) queue.putMessage(MSG_CONFIG);
            
            return changed;
    }
    
error:
    return false;
    
success:
    putMessage(MSG_CONFIG);
    
exit:
    return true;
}

bool
C64::configure(DriveID id, ConfigOption option, long value)
{
    assert(isDriveID(id));
    
    suspend();
    
    Drive &drive = id == DRIVE8 ? drive8 : drive9;
    DriveConfig current = drive.getConfig();
    
    switch (option) {
            
        case OPT_DRIVE_CONNECT:
            
            debug("OPT_DRIVE_CONNECT\n");
            
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
    fprintf(stderr, "[%lld] (%3d,%3d) %04X ", frame, rasterLine, rasterCycle, cpu.getPC0());
}

void
C64::reset()
{
    suspend();
    
    // Execute the standard reset routine
    HardwareComponent::reset();
    
    // Inform the GUI
    putMessage(MSG_RESET);
    
    resume();
}

void
C64::_reset()
{
    RESET_SNAPSHOT_ITEMS
            
    // Initialize processor port
    mem.poke(0x0000, 0x2F);
    mem.poke(0x0001, 0x1F);
    
    // Initialize program counter (MOVE TO CPU)
    cpu.reg.pc = mem.resetVector();
    
    rasterCycle = 1;
    nanoTargetTime = 0UL;
}

void
C64::setWarp(bool enable)
{
    suspend();
    HardwareComponent::setWarp(enable);
    resume();
}

void
C64::powerOn()
{
    debug(RUN_DEBUG, "powerOn()\n");
    
    pthread_mutex_lock(&stateChangeLock);
    
    if (!isPoweredOn() && isReady()) {
        
        acquireThreadLock();
        HardwareComponent::powerOn();
    }
    
    pthread_mutex_unlock(&stateChangeLock);
}

void
C64::_powerOn()
{
    debug(RUN_DEBUG, "_powerOn()\n");
    
    // Clear all runloop flags
    runLoopCtrl = 0;
    
    putMessage(MSG_POWER_ON);
}

void
C64::powerOff()
{
    debug(RUN_DEBUG, "powerOff()\n");
    
    pthread_mutex_lock(&stateChangeLock);
    
    if (!isPoweredOff()) {
        
        acquireThreadLock();
        HardwareComponent::powerOff();
    }
    
    pthread_mutex_unlock(&stateChangeLock);
}

void
C64::_powerOff()
{
    debug(RUN_DEBUG, "_powerOff()\n");
    
    putMessage(MSG_POWER_OFF);
}

void
C64::run()
{
    debug(RUN_DEBUG, "run()\n");
    
    pthread_mutex_lock(&stateChangeLock);
    
    if (!isRunning() && isReady()) {
        
        acquireThreadLock();
        HardwareComponent::run();
    }
    
    pthread_mutex_unlock(&stateChangeLock);
}

void
C64::_run()
{
    debug(RUN_DEBUG, "_run()\n");
    
    // Start the emulator thread
    pthread_create(&p, NULL, threadMain, (void *)this);
    
    // Inform the GUI
    putMessage(MSG_RUN);
}

void
C64::pause()
{
    debug(RUN_DEBUG, "pause()\n");
    
    pthread_mutex_lock(&stateChangeLock);
    
    if (!isPaused()) {
        
        acquireThreadLock();
        HardwareComponent::pause();
    }
    
    pthread_mutex_unlock(&stateChangeLock);
}

void
C64::inspect()
{
    switch(inspectionTarget) {
            
        case INSPECT_CPU: cpu.inspect(); break;
        case INSPECT_MEM: mem.inspect(); break;
        case INSPECT_CIA: cia1.inspect(); cia2.inspect(); break;
        case INSPECT_VIC: vic.inspect(); break;
        case INSPECT_SID: sid.inspect(); break;
        default: break;
    }
}

void
C64::_pause()
{
    debug(RUN_DEBUG, "_pause()\n");
    
    // When we reach this line, the emulator thread is already gone
    assert(p == NULL);
    
    // Update the recorded debug information
    inspect();
    
    // Inform the GUI
    putMessage(MSG_PAUSE);
}

void C64::_ping()
{
    putMessage(warpMode ? MSG_WARP_ON : MSG_WARP_OFF);
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
    msg("\n");
}

void
C64::_setWarp(bool enable)
{
    if (enable) {
        
        putMessage(MSG_WARP_ON);
        
    } else {
        
        restartTimer();
        putMessage(MSG_WARP_OFF);
    }
}

void
C64::suspend()
{
    pthread_mutex_lock(&stateChangeLock);
    
    debug(RUN_DEBUG, "Suspending (%d)...\n", suspendCounter);
    
    if (suspendCounter || isRunning()) {
        
        acquireThreadLock();
        assert(!isRunning()); // At this point, the emulator is already paused
        
        suspendCounter++;
    }
    
    pthread_mutex_unlock(&stateChangeLock);
}

void
C64::resume()
{
    pthread_mutex_lock(&stateChangeLock);
    
    debug(RUN_DEBUG, "Resuming (%d)...\n", suspendCounter);
    
    if (suspendCounter && --suspendCounter == 0) {
        
        acquireThreadLock();
        HardwareComponent::run();
    }
    
    pthread_mutex_unlock(&stateChangeLock);
}

void
C64::acquireThreadLock()
{
    // Free the lock
    if (state == STATE_RUNNING) {
        
        // Assure the emulator thread exists
        assert(p != NULL);
        
        // Free the lock by terminating the thread
        requestStop();
        
    } else {
        
        // There must be no emulator thread
        assert(p == NULL);
        
        // It's save to free the lock immediately
        pthread_mutex_unlock(&threadLock);
    }
    
    // Acquire the lock
    pthread_mutex_lock(&threadLock);
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
    VICRevision vicref = vic.getRevision();
    bool grayDotBug = vic.getConfigItem(OPT_GRAY_DOT_BUG);
    bool glueLogic = vic.getConfigItem(OPT_GLUE_LOGIC);
    
    // Look for known configurations
    for (unsigned i = 0; i < sizeof(configurations) / sizeof(C64ConfigurationDeprecated); i++) {
        if (vicref == configurations[i].vic &&
            grayDotBug == configurations[i].grayDotBug &&
            cia1.getRevision() == configurations[i].cia &&
            cia1.getTimerBBug() == configurations[i].timerBBug &&
            sid.getRevision() == configurations[i].sid &&
            glueLogic == configurations[i].glue &&
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
        
        configure(OPT_VIC_REVISION, configurations[m].vic);
        configure(OPT_GRAY_DOT_BUG, configurations[m].grayDotBug);
        configure(OPT_GLUE_LOGIC, configurations[m].glue);
        cia1.setRevision(configurations[m].cia);
        cia2.setRevision(configurations[m].cia);
        cia1.setTimerBBug(configurations[m].timerBBug);
        cia2.setTimerBBug(configurations[m].timerBBug);
        sid.setRevision(configurations[m].sid);
        sid.setFilter(configurations[m].sidFilter);
        mem.setRamPattern(configurations[m].pattern);
        
        resume();
    }
}

void
C64::updateVicFunctionTable()
{
    // Assign model independent execution functions
    vicfunc[0] = NULL;
    vicfunc[12] = &VICII::cycle12;
    vicfunc[13] = &VICII::cycle13;
    vicfunc[14] = &VICII::cycle14;
    vicfunc[15] = &VICII::cycle15;
    vicfunc[16] = &VICII::cycle16;
    vicfunc[17] = &VICII::cycle17;
    vicfunc[18] = &VICII::cycle18;
    
    for (unsigned cycle = 19; cycle <= 54; cycle++)
        vicfunc[cycle] = &VICII::cycle19to54;
    
    vicfunc[56] = &VICII::cycle56;
    
    // Assign model specific execution functions
    switch (vic.getRevision()) {
            
        case PAL_6569_R1:
        case PAL_6569_R3:
        case PAL_8565:
            
            vicfunc[1] = &VICII::cycle1pal;
            vicfunc[2] = &VICII::cycle2pal;
            vicfunc[3] = &VICII::cycle3pal;
            vicfunc[4] = &VICII::cycle4pal;
            vicfunc[5] = &VICII::cycle5pal;
            vicfunc[6] = &VICII::cycle6pal;
            vicfunc[7] = &VICII::cycle7pal;
            vicfunc[8] = &VICII::cycle8pal;
            vicfunc[9] = &VICII::cycle9pal;
            vicfunc[10] = &VICII::cycle10pal;
            vicfunc[11] = &VICII::cycle11pal;
            vicfunc[55] = &VICII::cycle55pal;
            vicfunc[57] = &VICII::cycle57pal;
            vicfunc[58] = &VICII::cycle58pal;
            vicfunc[59] = &VICII::cycle59pal;
            vicfunc[60] = &VICII::cycle60pal;
            vicfunc[61] = &VICII::cycle61pal;
            vicfunc[62] = &VICII::cycle62pal;
            vicfunc[63] = &VICII::cycle63pal;
            vicfunc[64] = NULL;
            vicfunc[65] = NULL;
            break;
            
        case NTSC_6567_R56A:
            
            vicfunc[1] = &VICII::cycle1pal;
            vicfunc[2] = &VICII::cycle2pal;
            vicfunc[3] = &VICII::cycle3pal;
            vicfunc[4] = &VICII::cycle4pal;
            vicfunc[5] = &VICII::cycle5pal;
            vicfunc[6] = &VICII::cycle6pal;
            vicfunc[7] = &VICII::cycle7pal;
            vicfunc[8] = &VICII::cycle8pal;
            vicfunc[9] = &VICII::cycle9pal;
            vicfunc[10] = &VICII::cycle10pal;
            vicfunc[11] = &VICII::cycle11pal;
            vicfunc[55] = &VICII::cycle55ntsc;
            vicfunc[57] = &VICII::cycle57ntsc;
            vicfunc[58] = &VICII::cycle58ntsc;
            vicfunc[59] = &VICII::cycle59ntsc;
            vicfunc[60] = &VICII::cycle60ntsc;
            vicfunc[61] = &VICII::cycle61ntsc;
            vicfunc[62] = &VICII::cycle62ntsc;
            vicfunc[63] = &VICII::cycle63ntsc;
            vicfunc[64] = &VICII::cycle64ntsc;
            vicfunc[65] = NULL;
            break;
            
        case NTSC_6567:
        case NTSC_8562:
            
            vicfunc[1] = &VICII::cycle1ntsc;
            vicfunc[2] = &VICII::cycle2ntsc;
            vicfunc[3] = &VICII::cycle3ntsc;
            vicfunc[4] = &VICII::cycle4ntsc;
            vicfunc[5] = &VICII::cycle5ntsc;
            vicfunc[6] = &VICII::cycle6ntsc;
            vicfunc[7] = &VICII::cycle7ntsc;
            vicfunc[8] = &VICII::cycle8ntsc;
            vicfunc[9] = &VICII::cycle9ntsc;
            vicfunc[10] = &VICII::cycle10ntsc;
            vicfunc[11] = &VICII::cycle11ntsc;
            vicfunc[55] = &VICII::cycle55ntsc;
            vicfunc[57] = &VICII::cycle57ntsc;
            vicfunc[58] = &VICII::cycle58ntsc;
            vicfunc[59] = &VICII::cycle59ntsc;
            vicfunc[60] = &VICII::cycle60ntsc;
            vicfunc[61] = &VICII::cycle61ntsc;
            vicfunc[62] = &VICII::cycle62ntsc;
            vicfunc[63] = &VICII::cycle63ntsc;
            vicfunc[64] = &VICII::cycle64ntsc;
            vicfunc[65] = &VICII::cycle65ntsc;
            break;
            
        default:
            assert(false);
    }
}

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
    
    // Pause all components
    HardwareComponent::pause();
    
    // Finish the current instruction to reach a clean state
    finishInstruction();
    
    // Release the thread lock
    pthread_mutex_unlock(&threadLock);
}

void
C64::runLoop()
{
    debug(RUN_DEBUG, "runLoop()\n");
    
    // Prepare to run
    restartTimer();
    
    // Enter the loop
    while (1) {
        
        // Run the emulator
        while (runLoopCtrl == 0) { executeOneFrame(); }
        
        // Check if special action needs to be taken
        if (runLoopCtrl) {
            
            // Are we requested to take a snapshot?
            if (runLoopCtrl & RL_AUTO_SNAPSHOT) {
                debug(RUN_DEBUG, "RL_AUTO_SNAPSHOT\n");
                // autoSnapshot = Snapshot::makeWithC64(this);
                // putMessage(MSG_AUTO_SNAPSHOT_TAKEN);
                clearControlFlags(RL_AUTO_SNAPSHOT);
            }
            if (runLoopCtrl & RL_USER_SNAPSHOT) {
                debug("RL_USER_SNAPSHOT\n");
                snapshot = Snapshot::makeWithC64(this);
                putMessage(MSG_SNAPSHOT_TAKEN);
                clearControlFlags(RL_USER_SNAPSHOT);
            }
            
            // Are we requested to update the debugger info structs?
            if (runLoopCtrl & RL_INSPECT) {
                debug(RUN_DEBUG, "RL_INSPECT\n");
                inspect();
                clearControlFlags(RL_INSPECT);
            }
            
            // Did we reach a breakpoint?
            if (runLoopCtrl & RL_BREAKPOINT_REACHED) {
                putMessage(MSG_BREAKPOINT_REACHED);
                debug(RUN_DEBUG, "BREAKPOINT_REACHED pc: %x\n", cpu.getPC0());
                clearControlFlags(RL_BREAKPOINT_REACHED);
                break;
            }
            
            // Did we reach a watchpoint?
            if (runLoopCtrl & RL_WATCHPOINT_REACHED) {
                putMessage(MSG_WATCHPOINT_REACHED);
                debug(RUN_DEBUG, "WATCHPOINT_REACHED pc: %x\n", cpu.getPC0());
                clearControlFlags(RL_WATCHPOINT_REACHED);
                break;
            }
            
            // Are we requested to terminate the run loop?
            if (runLoopCtrl & RL_STOP) {
                clearControlFlags(RL_STOP);
                debug("RL_STOP\n");
                break;
            }
            
            // Is the CPU jammed due the execution of an illegal instruction?
            if (runLoopCtrl & RL_CPU_JAMMED) {
                putMessage(MSG_CPU_JAMMED);
                debug("RL_CPU_JAMMED\n");
                clearControlFlags(RL_CPU_JAMMED);
                break;
            }
            
            assert(runLoopCtrl == 0);
        }
    }
}

void
C64::stopAndGo()
{
    debug("stopAndGo()");
    isRunning() ? pause() : run();
}

void
C64::stepInto()
{
    debug("stepInto()");
    if (isRunning()) return;
    
    // Execute the next instruction
    executeOneCycle();
    finishInstruction();
    
    // Trigger a GUI refresh
    putMessage(MSG_BREAKPOINT_REACHED);
}

void
C64::stepOver()
{
    debug("stepOver()");
    if (isRunning()) return;
    
    // If the next instruction is a JSR instruction (0x20), we set a breakpoint
    // at the next memory location. Otherwise, stepOver behaves like stepInto.
    if (mem.spypeek(cpu.getPC0()) == 0x20) {
        cpu.debugger.setSoftStopAtNextInstr();
        run();
    } else {
        stepInto();
    }
}

void
C64::executeOneFrame()
{
    do { executeOneLine(); } while (rasterLine != 0 && runLoopCtrl == 0);
}

void
C64::executeOneLine()
{
    // Emulate the beginning of a rasterline
    if (rasterCycle == 1) beginRasterLine();
    
    // Emulate the middle of a rasterline
    int lastCycle = vic.getCyclesPerRasterline();
    for (unsigned i = rasterCycle; i <= lastCycle; i++) {
        
        _executeOneCycle();
        if (runLoopCtrl != 0) {
            if (i == lastCycle) endRasterLine();
            return;
        }
    }
    
    // Emulate the end of a rasterline
    endRasterLine();
}

void
C64::executeOneCycle()
{
    bool isFirstCycle = rasterCycle == 1;
    bool isLastCycle = vic.isLastCycleInRasterline(rasterCycle);
    
    if (isFirstCycle) beginRasterLine();
    _executeOneCycle();
    if (isLastCycle) endRasterLine();
}

void
C64::_executeOneCycle()
{
    u64 cycle = ++cpu.cycle;
    
    //  <---------- o2 low phase ----------->|<- o2 high phase ->|
    //                                       |                   |
    // ,-- C64 ------------------------------|-------------------|--,
    // |   ,-----,     ,-----,     ,-----,   |    ,-----,        |  |
    // |   |     |     |     |     |     |   |    |     |        |  |
    // '-->| VIC | --> | CIA | --> | CIA | --|--> | CPU | -------|--'
    //     |     |     |  1  |     |  2  |   |    |     |        |
    //     '-----'     '-----'     '-----'   |    '-----'        |
    //                                  ,---------,              |
    //                                  | IEC bus |              |
    //                                  '---------'              |
    //                                       |    ,--------,     |
    //                                       |    |        |     |
    // ,-- Drive ----------------------------|--> | VC1541 | ----|--,
    // |                                     |    |        |     |  |
    // |                                     |    '--------'     |  |
    // '-------------------------------------|-------------------|--'
    
    // First clock phase (o2 low)
    (vic.*vicfunc[rasterCycle])();
    if (cycle >= cia1.wakeUpCycle) cia1.executeOneCycle();
    if (cycle >= cia2.wakeUpCycle) cia2.executeOneCycle();
    if (iec.isDirtyC64Side) iec.updateIecLinesC64Side();
    
    // Second clock phase (o2 high)
    cpu.executeOneCycle();
    if (drive8.isConnected()) drive8.execute(durationOfOneCycle);
    if (drive9.isConnected()) drive9.execute(durationOfOneCycle);
    datasette.execute();
    
    rasterCycle++;
}

void
C64::finishInstruction()
{
    while (!cpu.inFetchPhase()) executeOneCycle();
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
    
    // Check if the run loop is requested to stop
    if (stopFlag) { stopFlag = false; signalStop(); }
    
    // Count some sheep (zzzzzz) ...
    if (!inWarpMode()) {
        synchronizeTiming();
    }
}

void
C64::setControlFlags(u32 flags)
{
    synchronized { runLoopCtrl |= flags; }
}

void
C64::clearControlFlags(u32 flags)
{
    synchronized { runLoopCtrl &= ~flags; }
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

void
C64::requestSnapshot()
{
    if (!isRunning()) {
        
        // Take snapshot immediately
        snapshot = Snapshot::makeWithC64(this);
        putMessage(MSG_SNAPSHOT_TAKEN);
        
    } else {
        
        // Schedule the snapshot to be taken
        signalUserSnapshot();
    }
}

Snapshot *
C64::latestSnapshot()
{
    Snapshot *result = snapshot;
    snapshot = NULL;
    return result;
}

void C64::loadFromSnapshot(Snapshot *snapshot)
{
    u8 *ptr;
    
    if (snapshot && (ptr = snapshot->getData())) {
        
        // Make sure the emulator is not running
        assert(!isRunning());
        
        // Restore the saved state
        load(ptr);
        
        // Clear the keyboard matrix to avoid constantly pressed keys
        keyboard.releaseAll();
        
        ping();
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
    return hasVC1541Rom() ? crc32(drive8.mem.rom, 0x4000) : 0;
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
    return hasVC1541Rom() ? fnv_1a_64(drive8.mem.rom, 0x4000) : 0;
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
    assert(drive8.mem.rom[0] == drive9.mem.rom[0]);
    assert(drive8.mem.rom[1] == drive9.mem.rom[1]);
    return (drive8.mem.rom[0] | drive8.mem.rom[1]) != 0x00;
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
        file->flash(drive8.mem.rom);
        file->flash(drive9.mem.rom);
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
    memset(drive8.mem.rom, 0, 0x4000);
    memset(drive9.mem.rom, 0, 0x4000);
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
    
    RomFile *file = RomFile::makeWithBuffer(drive8.mem.rom, 0x4000);
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
            file->flash(drive8.mem.rom);
            file->flash(drive9.mem.rom);
            break;
            
        case V64_FILE:
            loadFromSnapshot((Snapshot *)file);
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
