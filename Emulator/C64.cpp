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
        case OPT_PALETTE:
        case OPT_GRAY_DOT_BUG:
        case OPT_GLUE_LOGIC:
        case OPT_HIDE_SPRITES:
        case OPT_DMA_DEBUG:
        case OPT_DMA_CHANNEL_R:
        case OPT_DMA_CHANNEL_I:
        case OPT_DMA_CHANNEL_C:
        case OPT_DMA_CHANNEL_G:
        case OPT_DMA_CHANNEL_P:
        case OPT_DMA_CHANNEL_S:
        case OPT_DMA_COLOR_R:
        case OPT_DMA_COLOR_I:
        case OPT_DMA_COLOR_C:
        case OPT_DMA_COLOR_G:
        case OPT_DMA_COLOR_P:
        case OPT_DMA_COLOR_S:
        case OPT_DMA_DISPLAY_MODE:
        case OPT_DMA_OPACITY:
        case OPT_CUT_LAYERS:
        case OPT_CUT_OPACITY:
        case OPT_SS_COLLISIONS:
        case OPT_SB_COLLISIONS:
            return vic.getConfigItem(option);
                        
        case OPT_CIA_REVISION:
        case OPT_TIMER_B_BUG:
            assert(cia1.getConfigItem(option) == cia2.getConfigItem(option));
            return cia1.getConfigItem(option);

        case OPT_SID_REVISION:
        case OPT_SID_FILTER:
        case OPT_SID_ENGINE:
        case OPT_SID_SAMPLING:
            return sid.getConfigItem(option);

        case OPT_RAM_PATTERN:
        case OPT_DEBUGCART:
            return mem.getConfigItem(option);

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
            
        case OPT_DRIVE_TYPE:
        case OPT_DRIVE_CONNECT:
        case OPT_DRIVE_POWER_SWITCH:
            return drive.getConfigItem(option);
            
        default:
            assert(false);
            return 0;
    }
}

bool
C64::configure(ConfigOption option, long value)
{
    debug(CNF_DEBUG, "configure(option: %d, value: %d\n", option, value);

    // Propagate configuration request to all components
    bool changed = HardwareComponent::configure(option, value);
    
    // Inform the GUI if the configuration has changed
    if (changed) messageQueue.put(MSG_CONFIG);
    
    return changed;
}

bool
C64::configure(DriveID id, ConfigOption option, long value)
{
    debug(CNF_DEBUG, "configure(id: %d, option: %d, value: %d\n", id, option, value);
    
    // Propagate configuration request to all components
    bool changed = HardwareComponent::configure(id, option, value);
    
    // Inform the GUI if the configuration has changed
    if (changed) messageQueue.put(MSG_CONFIG);
    
    return changed;
}

void
C64::configure(C64Model model)
{
    if (model != C64_CUSTOM) {
        
        suspend();
        configure(OPT_VIC_REVISION, configurations[model].vic);
        configure(OPT_GRAY_DOT_BUG, configurations[model].grayDotBug);
        configure(OPT_GLUE_LOGIC,   configurations[model].glue);
        configure(OPT_CIA_REVISION, configurations[model].cia);
        configure(OPT_TIMER_B_BUG,  configurations[model].timerBBug);
        configure(OPT_SID_REVISION, configurations[model].sid);
        configure(OPT_SID_FILTER,   configurations[model].sidFilter);
        configure(OPT_RAM_PATTERN,  configurations[model].pattern);
        resume();
    }
}

bool
C64::setConfigItem(ConfigOption option, long value)
{
    switch (option) {
            
        case OPT_VIC_REVISION:
        {
            u64 newFrequency = VICII::getFrequency((VICRevision)value);
            debug(" set freq = %lld\n", newFrequency);

            if (frequency == newFrequency) {
                assert(durationOfOneCycle == 10000000000 / newFrequency);
                return false;
            }
            
            frequency = (u32)newFrequency;
            durationOfOneCycle = 10000000000 / newFrequency;
            return true;
        }
        default:
            return false;
    }
}

C64Model
C64::getModel()
{
    VICRevision vicref = (VICRevision)vic.getConfigItem(OPT_VIC_REVISION);
    bool grayDotBug    = vic.getConfigItem(OPT_GRAY_DOT_BUG);
    bool glueLogic     = vic.getConfigItem(OPT_GLUE_LOGIC);
    CIARevision ciaref = (CIARevision)cia1.getConfigItem(OPT_CIA_REVISION);
    bool timerBBug     = cia1.getConfigItem(OPT_TIMER_B_BUG);
    SIDRevision sidref = (SIDRevision)sid.getConfigItem(OPT_SID_REVISION);
    bool sidFilter     = sid.getConfigItem(OPT_SID_FILTER);
    RamPattern pattern = (RamPattern)mem.getConfigItem(OPT_RAM_PATTERN);
    
    // Try to find a matching configuration
    for (unsigned i = 0; i < sizeof(configurations) / sizeof(C64ConfigurationDeprecated); i++) {
        
        if (vicref     != configurations[i].vic) continue;
        if (grayDotBug != configurations[i].grayDotBug) continue;
        if (glueLogic  != configurations[i].glue) continue;
        if (ciaref     != configurations[i].cia) continue;
        if (timerBBug  != configurations[i].timerBBug) continue;
        if (sidref     != configurations[i].sid) continue;
        if (sidFilter  != configurations[i].sidFilter) continue;
        if (pattern    != configurations[i].pattern) continue;
        return (C64Model)i;
    }
    return C64_CUSTOM;
}

void
C64::updateVicFunctionTable()
{
    bool dmaDebug = vic.getConfig().dmaDebug;
    
    debug("updateVicFunctionTable (dmaDebug: %d)\n", dmaDebug);
    
    // Assign model independent execution functions
    vicfunc[0] = NULL;
    if (dmaDebug) {
        vicfunc[12] = &VICII::cycle12<PAL_DEBUG_CYCLE>;
        vicfunc[13] = &VICII::cycle13<PAL_DEBUG_CYCLE>;
        vicfunc[14] = &VICII::cycle14<PAL_DEBUG_CYCLE>;
        vicfunc[15] = &VICII::cycle15<PAL_DEBUG_CYCLE>;
        vicfunc[16] = &VICII::cycle16<PAL_DEBUG_CYCLE>;
        vicfunc[17] = &VICII::cycle17<PAL_DEBUG_CYCLE>;
        vicfunc[18] = &VICII::cycle18<PAL_DEBUG_CYCLE>;
        for (unsigned cycle = 19; cycle <= 54; cycle++)
            vicfunc[cycle] = &VICII::cycle19to54<PAL_DEBUG_CYCLE>;
        vicfunc[56] = &VICII::cycle56<PAL_DEBUG_CYCLE>;
    } else {
        vicfunc[12] = &VICII::cycle12<PAL_CYCLE>;
        vicfunc[13] = &VICII::cycle13<PAL_CYCLE>;
        vicfunc[14] = &VICII::cycle14<PAL_CYCLE>;
        vicfunc[15] = &VICII::cycle15<PAL_CYCLE>;
        vicfunc[16] = &VICII::cycle16<PAL_CYCLE>;
        vicfunc[17] = &VICII::cycle17<PAL_CYCLE>;
        vicfunc[18] = &VICII::cycle18<PAL_CYCLE>;
        for (unsigned cycle = 19; cycle <= 54; cycle++)
            vicfunc[cycle] = &VICII::cycle19to54<PAL_CYCLE>;
        vicfunc[56] = &VICII::cycle56<PAL_CYCLE>;
    }
    
    // Assign model specific execution functions
    switch (vic.getRevision()) {
            
        case PAL_6569_R1:
        case PAL_6569_R3:
        case PAL_8565:
            
            if (dmaDebug) {
                vicfunc[1] = &VICII::cycle1<PAL_DEBUG_CYCLE>;
                vicfunc[2] = &VICII::cycle2<PAL_DEBUG_CYCLE>;
                vicfunc[3] = &VICII::cycle3<PAL_DEBUG_CYCLE>;
                vicfunc[4] = &VICII::cycle4<PAL_DEBUG_CYCLE>;
                vicfunc[5] = &VICII::cycle5<PAL_DEBUG_CYCLE>;
                vicfunc[6] = &VICII::cycle6<PAL_DEBUG_CYCLE>;
                vicfunc[7] = &VICII::cycle7<PAL_DEBUG_CYCLE>;
                vicfunc[8] = &VICII::cycle8<PAL_DEBUG_CYCLE>;
                vicfunc[9] = &VICII::cycle9<PAL_DEBUG_CYCLE>;
                vicfunc[10] = &VICII::cycle10<PAL_DEBUG_CYCLE>;
                vicfunc[11] = &VICII::cycle11<PAL_DEBUG_CYCLE>;
                vicfunc[55] = &VICII::cycle55<PAL_DEBUG_CYCLE>;
                vicfunc[57] = &VICII::cycle57<PAL_DEBUG_CYCLE>;
                vicfunc[58] = &VICII::cycle58<PAL_DEBUG_CYCLE>;
                vicfunc[59] = &VICII::cycle59<PAL_DEBUG_CYCLE>;
                vicfunc[60] = &VICII::cycle60<PAL_DEBUG_CYCLE>;
                vicfunc[61] = &VICII::cycle61<PAL_DEBUG_CYCLE>;
                vicfunc[62] = &VICII::cycle62<PAL_DEBUG_CYCLE>;
                vicfunc[63] = &VICII::cycle63<PAL_DEBUG_CYCLE>;
            } else {
                vicfunc[1] = &VICII::cycle1<PAL_CYCLE>;
                vicfunc[2] = &VICII::cycle2<PAL_CYCLE>;
                vicfunc[3] = &VICII::cycle3<PAL_CYCLE>;
                vicfunc[4] = &VICII::cycle4<PAL_CYCLE>;
                vicfunc[5] = &VICII::cycle5<PAL_CYCLE>;
                vicfunc[6] = &VICII::cycle6<PAL_CYCLE>;
                vicfunc[7] = &VICII::cycle7<PAL_CYCLE>;
                vicfunc[8] = &VICII::cycle8<PAL_CYCLE>;
                vicfunc[9] = &VICII::cycle9<PAL_CYCLE>;
                vicfunc[10] = &VICII::cycle10<PAL_CYCLE>;
                vicfunc[11] = &VICII::cycle11<PAL_CYCLE>;
                vicfunc[55] = &VICII::cycle55<PAL_CYCLE>;
                vicfunc[57] = &VICII::cycle57<PAL_CYCLE>;
                vicfunc[58] = &VICII::cycle58<PAL_CYCLE>;
                vicfunc[59] = &VICII::cycle59<PAL_CYCLE>;
                vicfunc[60] = &VICII::cycle60<PAL_CYCLE>;
                vicfunc[61] = &VICII::cycle61<PAL_CYCLE>;
                vicfunc[62] = &VICII::cycle62<PAL_CYCLE>;
                vicfunc[63] = &VICII::cycle63<PAL_CYCLE>;
            }
            vicfunc[64] = NULL;
            vicfunc[65] = NULL;
            break;
            
        case NTSC_6567_R56A:
            
            if (dmaDebug) {
                vicfunc[1] = &VICII::cycle1<PAL_DEBUG_CYCLE>;
                vicfunc[2] = &VICII::cycle2<PAL_DEBUG_CYCLE>;
                vicfunc[3] = &VICII::cycle3<PAL_DEBUG_CYCLE>;
                vicfunc[4] = &VICII::cycle4<PAL_DEBUG_CYCLE>;
                vicfunc[5] = &VICII::cycle5<PAL_DEBUG_CYCLE>;
                vicfunc[6] = &VICII::cycle6<PAL_DEBUG_CYCLE>;
                vicfunc[7] = &VICII::cycle7<PAL_DEBUG_CYCLE>;
                vicfunc[8] = &VICII::cycle8<PAL_DEBUG_CYCLE>;
                vicfunc[9] = &VICII::cycle9<PAL_DEBUG_CYCLE>;
                vicfunc[10] = &VICII::cycle10<PAL_DEBUG_CYCLE>;
                vicfunc[11] = &VICII::cycle11<PAL_DEBUG_CYCLE>;
                vicfunc[55] = &VICII::cycle55<NTSC_DEBUG_CYCLE>;
                vicfunc[57] = &VICII::cycle57<NTSC_DEBUG_CYCLE>;
                vicfunc[58] = &VICII::cycle58<NTSC_DEBUG_CYCLE>;
                vicfunc[59] = &VICII::cycle59<NTSC_DEBUG_CYCLE>;
                vicfunc[60] = &VICII::cycle60<NTSC_DEBUG_CYCLE>;
                vicfunc[61] = &VICII::cycle61<NTSC_DEBUG_CYCLE>;
                vicfunc[62] = &VICII::cycle62<NTSC_DEBUG_CYCLE>;
                vicfunc[63] = &VICII::cycle63<NTSC_DEBUG_CYCLE>;
                vicfunc[64] = &VICII::cycle64<NTSC_DEBUG_CYCLE>;
            } else {
                vicfunc[1] = &VICII::cycle1<PAL_CYCLE>;
                vicfunc[2] = &VICII::cycle2<PAL_CYCLE>;
                vicfunc[3] = &VICII::cycle3<PAL_CYCLE>;
                vicfunc[4] = &VICII::cycle4<PAL_CYCLE>;
                vicfunc[5] = &VICII::cycle5<PAL_CYCLE>;
                vicfunc[6] = &VICII::cycle6<PAL_CYCLE>;
                vicfunc[7] = &VICII::cycle7<PAL_CYCLE>;
                vicfunc[8] = &VICII::cycle8<PAL_CYCLE>;
                vicfunc[9] = &VICII::cycle9<PAL_CYCLE>;
                vicfunc[10] = &VICII::cycle10<PAL_CYCLE>;
                vicfunc[11] = &VICII::cycle11<PAL_CYCLE>;
                vicfunc[55] = &VICII::cycle55<NTSC_CYCLE>;
                vicfunc[57] = &VICII::cycle57<NTSC_CYCLE>;
                vicfunc[58] = &VICII::cycle58<NTSC_CYCLE>;
                vicfunc[59] = &VICII::cycle59<NTSC_CYCLE>;
                vicfunc[60] = &VICII::cycle60<NTSC_CYCLE>;
                vicfunc[61] = &VICII::cycle61<NTSC_CYCLE>;
                vicfunc[62] = &VICII::cycle62<NTSC_CYCLE>;
                vicfunc[63] = &VICII::cycle63<NTSC_CYCLE>;
                vicfunc[64] = &VICII::cycle64<NTSC_CYCLE>;
            }
            vicfunc[65] = NULL;
            break;
            
        case NTSC_6567:
        case NTSC_8562:
            
            if (dmaDebug) {
                vicfunc[1] = &VICII::cycle1<NTSC_DEBUG_CYCLE>;
                vicfunc[2] = &VICII::cycle2<NTSC_DEBUG_CYCLE>;
                vicfunc[3] = &VICII::cycle3<NTSC_DEBUG_CYCLE>;
                vicfunc[4] = &VICII::cycle4<NTSC_DEBUG_CYCLE>;
                vicfunc[5] = &VICII::cycle5<NTSC_DEBUG_CYCLE>;
                vicfunc[6] = &VICII::cycle6<NTSC_DEBUG_CYCLE>;
                vicfunc[7] = &VICII::cycle7<NTSC_DEBUG_CYCLE>;
                vicfunc[8] = &VICII::cycle8<NTSC_DEBUG_CYCLE>;
                vicfunc[9] = &VICII::cycle9<NTSC_DEBUG_CYCLE>;
                vicfunc[10] = &VICII::cycle10<NTSC_DEBUG_CYCLE>;
                vicfunc[11] = &VICII::cycle11<NTSC_DEBUG_CYCLE>;
                vicfunc[55] = &VICII::cycle55<NTSC_DEBUG_CYCLE>;
                vicfunc[57] = &VICII::cycle57<NTSC_DEBUG_CYCLE>;
                vicfunc[58] = &VICII::cycle58<NTSC_DEBUG_CYCLE>;
                vicfunc[59] = &VICII::cycle59<NTSC_DEBUG_CYCLE>;
                vicfunc[60] = &VICII::cycle60<NTSC_DEBUG_CYCLE>;
                vicfunc[61] = &VICII::cycle61<NTSC_DEBUG_CYCLE>;
                vicfunc[62] = &VICII::cycle62<NTSC_DEBUG_CYCLE>;
                vicfunc[63] = &VICII::cycle63<NTSC_DEBUG_CYCLE>;
                vicfunc[64] = &VICII::cycle64<NTSC_DEBUG_CYCLE>;
                vicfunc[65] = &VICII::cycle65<NTSC_DEBUG_CYCLE>;
            } else {
                vicfunc[1] = &VICII::cycle1<NTSC_CYCLE>;
                vicfunc[2] = &VICII::cycle2<NTSC_CYCLE>;
                vicfunc[3] = &VICII::cycle3<NTSC_CYCLE>;
                vicfunc[4] = &VICII::cycle4<NTSC_CYCLE>;
                vicfunc[5] = &VICII::cycle5<NTSC_CYCLE>;
                vicfunc[6] = &VICII::cycle6<NTSC_CYCLE>;
                vicfunc[7] = &VICII::cycle7<NTSC_CYCLE>;
                vicfunc[8] = &VICII::cycle8<NTSC_CYCLE>;
                vicfunc[9] = &VICII::cycle9<NTSC_CYCLE>;
                vicfunc[10] = &VICII::cycle10<NTSC_CYCLE>;
                vicfunc[11] = &VICII::cycle11<NTSC_CYCLE>;
                vicfunc[55] = &VICII::cycle55<NTSC_CYCLE>;
                vicfunc[57] = &VICII::cycle57<NTSC_CYCLE>;
                vicfunc[58] = &VICII::cycle58<NTSC_CYCLE>;
                vicfunc[59] = &VICII::cycle59<NTSC_CYCLE>;
                vicfunc[60] = &VICII::cycle60<NTSC_CYCLE>;
                vicfunc[61] = &VICII::cycle61<NTSC_CYCLE>;
                vicfunc[62] = &VICII::cycle62<NTSC_CYCLE>;
                vicfunc[63] = &VICII::cycle63<NTSC_CYCLE>;
                vicfunc[64] = &VICII::cycle64<NTSC_CYCLE>;
                vicfunc[65] = &VICII::cycle65<NTSC_CYCLE>;
            }
            break;
            
        default:
            assert(false);
    }
}

void
C64::setWarp(bool enable)
{
    // suspend();
    HardwareComponent::setWarp(enable);
    // resume();
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

void
C64::_dump()
{
    msg("C64:\n");
    msg("----\n\n");
    msg("              Machine type : %s\n", vic.isPAL() ? "PAL" : "NTSC");
    msg("         Frames per second : %f\n", vic.getFramesPerSecond());
    msg("     Rasterlines per frame : %d\n", vic.getRasterlinesPerFrame());
    msg("     Cycles per rasterline : %d\n", vic.getCyclesPerLine());
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
C64::_setDebug(bool enable)
{
    suspend();
    updateVicFunctionTable();
    resume();
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
    if (!hasRom(ROM_BASIC) || !hasRom(ROM_CHAR) || !hasRom(ROM_KERNAL)) {
        if (error) *error = ERR_ROM_MISSING;
        return false;
    }
    
    if (hasMega65Rom(ROM_BASIC) && hasMega65Rom(ROM_KERNAL)) {
        if (strcmp(mega65BasicRev(), mega65KernalRev()) != 0) {
            if (error) *error = ERR_ROM_MEGA65_MISMATCH;
            return false;
        }
    }
    
    return true;
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
                autoSnapshot = Snapshot::makeWithC64(this);
                putMessage(MSG_AUTO_SNAPSHOT_TAKEN);
                clearControlFlags(RL_AUTO_SNAPSHOT);
            }
            if (runLoopCtrl & RL_USER_SNAPSHOT) {
                debug("RL_USER_SNAPSHOT\n");
                userSnapshot = Snapshot::makeWithC64(this);
                putMessage(MSG_USER_SNAPSHOT_TAKEN);
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
    int lastCycle = vic.getCyclesPerLine();
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
    if (drive8.isActive()) drive8.execute(durationOfOneCycle);
    if (drive9.isActive()) drive9.execute(durationOfOneCycle);
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
    keyboard.vsyncHandler();
    drive8.vsyncHandler();
    drive9.vsyncHandler();

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
C64::requestAutoSnapshot()
{
    if (!isRunning()) {
        
        // Take snapshot immediately
        autoSnapshot = Snapshot::makeWithC64(this);
        putMessage(MSG_AUTO_SNAPSHOT_TAKEN);
        
    } else {
        
        // Schedule the snapshot to be taken
        signalAutoSnapshot();
    }
}

void
C64::requestUserSnapshot()
{
    if (!isRunning()) {
        
        // Take snapshot immediately
        userSnapshot = Snapshot::makeWithC64(this);
        putMessage(MSG_USER_SNAPSHOT_TAKEN);
        
    } else {
        
        // Schedule the snapshot to be taken
        signalUserSnapshot();
    }
}

Snapshot *
C64::latestAutoSnapshot()
{
    Snapshot *result = autoSnapshot;
    autoSnapshot = NULL;
    return result;
}

Snapshot *
C64::latestUserSnapshot()
{
    Snapshot *result = userSnapshot;
    userSnapshot = NULL;
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
        
        // Inform the GUI
        messageQueue.put(MSG_SNAPSHOT_RESTORED);
    }
}

u32
C64::romCRC32(RomType type)
{
    switch (type) {
            
        case ROM_BASIC:
            return hasRom(ROM_BASIC)  ? crc32(mem.rom + 0xA000, 0x2000) : 0;
        case ROM_CHAR:
            return hasRom(ROM_CHAR)   ? crc32(mem.rom + 0xD000, 0x1000) : 0;
        case ROM_KERNAL:
            return hasRom(ROM_KERNAL) ? crc32(mem.rom + 0xE000, 0x2000) : 0;
        case ROM_VC1541:
            return hasRom(ROM_VC1541) ? crc32(drive8.mem.rom, 0x4000) : 0;
        default:
            assert(false);
    }
}

/*
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
*/

u64
C64::romFNV64(RomType type)
{
    switch (type) {
            
        case ROM_BASIC:
            return hasRom(ROM_BASIC)  ? fnv_1a_64(mem.rom + 0xA000, 0x2000) : 0;
        case ROM_CHAR:
            return hasRom(ROM_CHAR)   ? fnv_1a_64(mem.rom + 0xD000, 0x1000) : 0;
        case ROM_KERNAL:
            return hasRom(ROM_KERNAL) ? fnv_1a_64(mem.rom + 0xE000, 0x2000) : 0;
        case ROM_VC1541:
            return hasRom(ROM_VC1541) ? fnv_1a_64(drive8.mem.rom, 0x4000) : 0;
        default:
            assert(false);
    }
}

RomIdentifier
C64::romIdentifier(RomType type)
{
    return RomFile::identifier(romFNV64(type));
}

const char *
C64::romTitle(RomType type)
{
    switch (type) {
            
        case ROM_BASIC:
        {
            // Intercept if a MEGA65 Rom is installed
            if (hasMega65Rom(ROM_BASIC)) return "M.E.G.A. C64 OpenROM";
            
            RomIdentifier rev = romIdentifier(ROM_BASIC);
            return rev == ROM_UNKNOWN ? "Unknown Basic Rom" : RomFile::title(rev);
        }
        case ROM_CHAR:
        {
            // Intercept if a MEGA65 Rom is installed
            if (hasMega65Rom(ROM_CHAR)) return "M.E.G.A. C64 OpenROM";
            
            RomIdentifier rev = romIdentifier(ROM_CHAR);
            return rev == ROM_UNKNOWN ? "Unknown Character Rom" : RomFile::title(rev);
        }
        case ROM_KERNAL:
        {
            // Intercept if a MEGA65 Rom is installed
            if (hasMega65Rom(ROM_KERNAL)) return "M.E.G.A. C64 OpenROM";
            
            RomIdentifier rev = romIdentifier(ROM_KERNAL);
            return rev == ROM_UNKNOWN ? "Unknown Kernal Rom" : RomFile::title(rev);
        }
        case ROM_VC1541:
        {
            RomIdentifier rev = romIdentifier(ROM_VC1541);
            return rev == ROM_UNKNOWN ? "Unknown Kernal Rom" : RomFile::title(rev);
        }
        default: assert(false);
    }
    return NULL;
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
C64::romSubTitle(RomType type)
{
    switch (type) {
            
        case ROM_BASIC:
        {
            // Intercept if a MEGA65 Rom is installed
            if (hasMega65Rom(ROM_BASIC)) return "Free Basic Replacement";
            
            return romSubTitle(romFNV64(ROM_BASIC));
        }
        case ROM_CHAR:
        {
            // Intercept if a MEGA65 Rom is installed
            if (hasMega65Rom(ROM_CHAR)) return "Free Charset Replacement";
            
            return romSubTitle(romFNV64(ROM_CHAR));
        }
        case ROM_KERNAL:
        {
            // Intercept if a MEGA65 Rom is installed
            if (hasMega65Rom(ROM_KERNAL)) return "Free Kernal Replacement";
            
            return romSubTitle(romFNV64(ROM_KERNAL));
        }
        case ROM_VC1541:
        {
            return romSubTitle(romFNV64(ROM_VC1541));
        }
        default: assert(false);
    }
    return NULL;
}

const char *
C64::romRevision(RomType type)
{
    switch (type) {
             
         case ROM_BASIC:
         {
             // Intercept if a MEGA65 Rom is installed
             if (hasMega65Rom(ROM_BASIC)) return mega65BasicRev();
             
             return RomFile::revision(romIdentifier(ROM_BASIC));
         }
         case ROM_CHAR:
         {
             return RomFile::revision(romIdentifier(ROM_CHAR));
         }
         case ROM_KERNAL:
         {
             // Intercept if a MEGA65 Rom is installed
             if (hasMega65Rom(ROM_KERNAL)) return mega65KernalRev();
             
             return RomFile::revision(romIdentifier(ROM_KERNAL));
         }
         case ROM_VC1541:
         {
             return RomFile::revision(romIdentifier(ROM_VC1541));
         }
         default: assert(false);
     }
     return NULL;
}

bool
C64::hasRom(RomType type)
{
    switch (type) {
            
        case ROM_BASIC:
        {
            return (mem.rom[0xA000] | mem.rom[0xA001]) != 0x00;
        }
        case ROM_CHAR:
        {
            return (mem.rom[0xD000] | mem.rom[0xD001]) != 0x00;
        }
        case ROM_KERNAL:
        {
            return (mem.rom[0xE000] | mem.rom[0xE001]) != 0x00;
        }
        case ROM_VC1541:
        {
            assert(drive8.mem.rom[0] == drive9.mem.rom[0]);
            assert(drive8.mem.rom[1] == drive9.mem.rom[1]);
            return (drive8.mem.rom[0] | drive8.mem.rom[1]) != 0x00;
        }
        default: assert(false);
    }
    return false;
}

bool
C64::hasMega65Rom(RomType type)
{
    switch (type) {
            
        case ROM_BASIC:
        {
            return mem.rom[0xBF52] == 'O' && mem.rom[0xBF53] == 'R';
        }
        case ROM_CHAR:
        {
            return RomFile::isMega65Rom(romIdentifier(ROM_CHAR));
        }
        case ROM_KERNAL:
        {
            return mem.rom[0xE4B9] == 'O' && mem.rom[0xE4BA] == 'R';
        }
        case ROM_VC1541:
        {
            return false;
        }
        default: assert(false);
    }
    return false;
}

char *
C64::mega65BasicRev()
{
    static char rev[17];
    rev[0] = 0;
    
    if (hasMega65Rom(ROM_BASIC)) memcpy(rev, &mem.rom[0xBF55], 16);
    rev[16] = 0;
    
    return rev;
}

char *
C64::mega65KernalRev()
{
    static char rev[17];
    rev[0] = 0;
    
    if (hasMega65Rom(ROM_KERNAL)) memcpy(rev, &mem.rom[0xE4BC], 16);
    rev[16] = 0;
    
    return rev;
}

bool
C64::loadRom(RomType type, RomFile *file)
{
    assert(file != NULL);
    
    switch (type) {
            
        case ROM_BASIC:
        {
            if (file->type() == FILETYPE_BASIC_ROM) {
                debug("Flashing Basic Rom\n");
                file->flash(mem.rom, 0xA000);
                
                debug("hasMega65Rom() = %d\n", hasMega65Rom(ROM_BASIC));
                debug("mega65BasicRev() = %s\n", mega65BasicRev());
                
                return true;
            }
            return false;
        }
        case ROM_CHAR:
        {
            if (file->type() == FILETYPE_CHAR_ROM) {
                debug("Flashing Character Rom\n");
                file->flash(mem.rom, 0xD000);
                return true;
            }
            return false;
        }
        case ROM_KERNAL:
        {
            if (file->type() == FILETYPE_KERNAL_ROM) {
                debug("Flashing Kernal Rom\n");
                file->flash(mem.rom, 0xE000);
                
                debug("hasMega65Rom() = %d\n", hasMega65Rom(ROM_KERNAL));
                debug("mega65KernalRev() = %s\n", mega65KernalRev());
                
                return true;
            }
            return false;
        }
        case ROM_VC1541:
        {
            if (file->type() == FILETYPE_VC1541_ROM) {
                debug("Flashing VC1541 Rom\n");
                file->flash(drive8.mem.rom);
                file->flash(drive9.mem.rom);
                return true;
            }
            return false;
        }
        default: assert(false);
    }
    return false;
}

bool
C64::loadRomFromBuffer(RomType type, const u8 *buffer, size_t length)
{
    assert(buffer != NULL);

    switch (type) {
            
        case ROM_BASIC:
        {
            if (RomFile *file = RomFile::makeWithBuffer(buffer, length)) {
                return loadRom(ROM_BASIC, file);
            }
            msg("Failed to read Basic Rom from buffer\n");
            return false;
        }
        case ROM_CHAR:
        {
            if (RomFile *file = RomFile::makeWithBuffer(buffer, length)) {
                return loadRom(ROM_CHAR, file);
            }
            msg("Failed to read Character Rom from buffer\n");
            return false;
        }
        case ROM_KERNAL:
        {
            if (RomFile *file = RomFile::makeWithBuffer(buffer, length)) {
                return loadRom(ROM_KERNAL, file);
            }
            msg("Failed to read Kernal Rom from buffer\n");
            return false;
        }
        case ROM_VC1541:
        {
            if (RomFile *file = RomFile::makeWithBuffer(buffer, length)) {
                return loadRom(ROM_VC1541, file);
            }
            msg("Failed to read VC1541 Rom from buffer\n");
            return false;
        }
        default: assert(false);
    }
    return false;
}

bool
C64::loadRomFromFile(RomType type, const char *path)
{
    assert(path != NULL);

    switch (type) {
            
        case ROM_BASIC:
        {
            if (RomFile *file = RomFile::makeWithFile(path)) {
                return loadRom(ROM_BASIC, file);
            }
            msg("Failed to read Basic Rom from %s\n", path);
            return false;
        }
        case ROM_CHAR:
        {
            if (RomFile *file = RomFile::makeWithFile(path)) {
                return loadRom(ROM_CHAR, file);
            }
            msg("Failed to read Character Rom from %s\n", path);
            return false;
        }
        case ROM_KERNAL:
        {
            if (RomFile *file = RomFile::makeWithFile(path)) {
                return loadRom(ROM_KERNAL, file);
            }
            msg("Failed to read Kernal Rom from %s\n", path);
            return false;
        }
        case ROM_VC1541:
        {
            if (RomFile *file = RomFile::makeWithFile(path)) {
                return loadRom(ROM_VC1541, file);
            }
            msg("Failed to read VC1541 Rom from %s\n", path);
            return false;
        }
        default: assert(false);
    }
    return false;
}

void
C64::deleteRom(RomType type)
{
    switch (type) {
            
        case ROM_BASIC:
        {
            memset(mem.rom + 0xA000, 0, 0x2000);
        }
        case ROM_CHAR:
        {
            memset(mem.rom + 0xD000, 0, 0x1000);
        }
        case ROM_KERNAL:
        {
            memset(mem.rom + 0xE000, 0, 0x2000);
        }
        case ROM_VC1541:
        {
            memset(drive8.mem.rom, 0, 0x4000);
            memset(drive9.mem.rom, 0, 0x4000);
        }
        default: assert(false);
    }
}

bool
C64::saveRom(RomType type, const char *path)
{
    switch (type) {
            
        case ROM_BASIC:
        {
            if (!hasRom(ROM_BASIC)) return false;
            
            RomFile *file = RomFile::makeWithBuffer(mem.rom + 0xA000, 0x2000);
            return file && file->writeToFile(path);
        }
        case ROM_CHAR:
        {
            if (!hasRom(ROM_CHAR)) return false;
            
            RomFile *file = RomFile::makeWithBuffer(mem.rom + 0xD000, 0x1000);
            return file && file->writeToFile(path);
        }
        case ROM_KERNAL:
        {
            if (!hasRom(ROM_KERNAL)) return false;
            
            RomFile *file = RomFile::makeWithBuffer(mem.rom + 0xE000, 0x2000);
            return file && file->writeToFile(path);
        }
        case ROM_VC1541:
        {
            if (!hasRom(ROM_VC1541)) return false;
            
            RomFile *file = RomFile::makeWithBuffer(drive8.mem.rom, 0x4000);
            return file && file->writeToFile(path);
        }
        default: assert(false);
    }
    return false;
}

bool
C64::flash(AnyFile *file)
{
    bool result = true;
    
    suspend();
    switch (file->type()) {
            
        case FILETYPE_BASIC_ROM:
            file->flash(mem.rom, 0xA000);
            break;
            
        case FILETYPE_CHAR_ROM:
            file->flash(mem.rom, 0xD000);
            break;
            
        case FILETYPE_KERNAL_ROM:
            file->flash(mem.rom, 0xE000);
            break;
            
        case FILETYPE_VC1541_ROM:
            file->flash(drive8.mem.rom);
            file->flash(drive9.mem.rom);
            break;
            
        case FILETYPE_V64:
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
            
        case FILETYPE_D64:
        case FILETYPE_T64:
        case FILETYPE_P00:
        case FILETYPE_PRG:
        case FILETYPE_PRG_FOLDER:
            
            file->selectItem(item);
            file->flashItem(mem.ram);
            break;
            
        default:
            assert(false);
            result = false;
    }
    resume();
    messageQueue.put(MSG_FILE_FLASHED);
    
    return result;
}
