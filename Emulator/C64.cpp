// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "C64.h"
#include "Checksum.h"

//
// Emulator thread
//

void 
threadTerminated(void* thisC64)
{
    assert(thisC64 != nullptr);
    
    // Inform the C64 that the thread has been canceled
    C64 *c64 = (C64 *)thisC64;
    c64->threadDidTerminate();
}

void 
*threadMain(void *thisC64) {
    
    assert(thisC64 != nullptr);
    
    // Inform the C64 that the thread is about to start
    C64 *c64 = (C64 *)thisC64;
    c64->threadWillStart();
    
    // Configure thread properties...
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, nullptr);
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, nullptr);
    pthread_cleanup_push(threadTerminated, thisC64);
    
    // Enter the run loop
    c64->runLoop();
    
    // Clean up and exit
    pthread_cleanup_pop(1);
    pthread_exit(nullptr);
}


//
// Class methods
//

C64::C64()
{
    trace(RUN_DEBUG, "Creating virtual C64 [%p]\n", this);
        
    subComponents = std::vector<HardwareComponent *> {
        
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
        &oscillator
    };
    
    // Set the initial configuration
    config.cycleLimit = INT64_MAX;
    
    // Set up the initial state
    initialize();
    _reset();
        
    // Initialize mutexes
    pthread_mutex_init(&threadLock, nullptr);
    pthread_mutex_init(&stateChangeLock, nullptr);
}

C64::~C64()
{
    trace(RUN_DEBUG, "Destroying C64[%p]\n", this);
    powerOff();
    
    pthread_mutex_destroy(&threadLock);
    pthread_mutex_destroy(&stateChangeLock);
}

void
C64::prefix() const
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
    
    // Clear all runloop flags
    runLoopCtrl = 0;

    rasterCycle = 1;
}

InspectionTarget
C64::getInspectionTarget() const
{
    return inspectionTarget;
}

void
C64::setInspectionTarget(InspectionTarget target)
{
    assert_enum(InspectionTarget, target);
    inspectionTarget = target;
}

i64
C64::getConfigItem(Option option) const
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
        case OPT_AUDVOLL:
        case OPT_AUDVOLR:
            return sid.getConfigItem(option);

        case OPT_RAM_PATTERN:
        case OPT_DEBUGCART:
            return mem.getConfigItem(option);

        default:
            assert(false);
            return 0;
    }
}

i64
C64::getConfigItem(Option option, long id) const
{
    switch (option) {
            
        case OPT_SID_ENABLE:
        case OPT_SID_ADDRESS:
        case OPT_AUDPAN:
        case OPT_AUDVOL:
        {
            assert(id >= 0 && id <= 3);
            return sid.getConfigItem(option, id);
        }
        case OPT_DRIVE_TYPE:
        case OPT_DRIVE_CONNECT:
        case OPT_DRIVE_POWER_SWITCH:
        {
            assert(id == DRIVE8 || id == DRIVE9);
            const Drive &drive = id == DRIVE8 ? drive8 : drive9;
            return drive.getConfigItem(option);
        }
        default:
            assert(false);
            return 0;
    }
}

bool
C64::configure(Option option, i64 value)
{
    trace(CNF_DEBUG, "configure(option: %lld, value: %lld\n", option, value);

    // Propagate configuration request to all components
    bool changed = HardwareComponent::configure(option, value);
        
    // Inform the GUI if the configuration has changed
    if (changed) messageQueue.put(MSG_CONFIG);
    
    // Dump the current configuration in debugging mode
    if (changed && CNF_DEBUG) dumpConfig();

    return changed;
}

bool
C64::configure(Option option, long id, i64 value)
{
    trace(CNF_DEBUG, "configure(option: %lld, id: %ld, value: %lld\n", option, id, value);
    
    // Propagate configuration request to all components
    bool changed = HardwareComponent::configure(option, id, value);
    
    // Inform the GUI if the configuration has changed
    if (changed) messageQueue.put(MSG_CONFIG);
    
    // Dump the current configuration in debugging mode
    if (changed && CNF_DEBUG) dumpConfig();

    return changed;
}

void
C64::configure(C64Model model)
{
    suspend();
    
    switch(model) {
            
        case C64_MODEL_PAL:
            configure(OPT_VIC_REVISION, VICII_PAL_6569_R3);
            configure(OPT_GRAY_DOT_BUG, false);
            configure(OPT_CIA_REVISION, MOS_6526);
            configure(OPT_TIMER_B_BUG,  true);
            configure(OPT_SID_REVISION, MOS_6581);
            configure(OPT_SID_FILTER,   true);
            configure(OPT_GLUE_LOGIC,   GLUE_LOGIC_DISCRETE);
            configure(OPT_RAM_PATTERN,  RAM_PATTERN_C64);
            break;
            
        case C64_MODEL_PAL_II:
            configure(OPT_VIC_REVISION, VICII_PAL_8565);
            configure(OPT_GRAY_DOT_BUG, true);
            configure(OPT_CIA_REVISION, MOS_8521);
            configure(OPT_TIMER_B_BUG,  false);
            configure(OPT_SID_REVISION, MOS_8580);
            configure(OPT_SID_FILTER,   true);
            configure(OPT_GLUE_LOGIC,   GLUE_LOGIC_IC);
            configure(OPT_RAM_PATTERN,  RAM_PATTERN_C64C);
            break;

        case C64_MODEL_PAL_OLD:
            configure(OPT_VIC_REVISION, VICII_PAL_6569_R1);
            configure(OPT_GRAY_DOT_BUG, false);
            configure(OPT_CIA_REVISION, MOS_6526);
            configure(OPT_TIMER_B_BUG,  true);
            configure(OPT_SID_REVISION, MOS_6581);
            configure(OPT_SID_FILTER,   true);
            configure(OPT_GLUE_LOGIC,   GLUE_LOGIC_DISCRETE);
            configure(OPT_RAM_PATTERN,  RAM_PATTERN_C64);
            break;

        case C64_MODEL_NTSC:
            configure(OPT_VIC_REVISION, VICII_NTSC_6567);
            configure(OPT_GRAY_DOT_BUG, false);
            configure(OPT_CIA_REVISION, MOS_6526);
            configure(OPT_TIMER_B_BUG,  false);
            configure(OPT_SID_REVISION, MOS_6581);
            configure(OPT_SID_FILTER,   true);
            configure(OPT_GLUE_LOGIC,   GLUE_LOGIC_DISCRETE);
            configure(OPT_RAM_PATTERN,  RAM_PATTERN_C64);
            break;

        case C64_MODEL_NTSC_II:
            configure(OPT_VIC_REVISION, VICII_NTSC_8562);
            configure(OPT_GRAY_DOT_BUG, true);
            configure(OPT_CIA_REVISION, MOS_8521);
            configure(OPT_TIMER_B_BUG,  true);
            configure(OPT_SID_REVISION, MOS_8580);
            configure(OPT_SID_FILTER,   true);
            configure(OPT_GLUE_LOGIC,   GLUE_LOGIC_IC);
            configure(OPT_RAM_PATTERN,  RAM_PATTERN_C64C);
            break;

        case C64_MODEL_NTSC_OLD:
            configure(OPT_VIC_REVISION, VICII_NTSC_6567_R56A);
            configure(OPT_GRAY_DOT_BUG, false);
            configure(OPT_CIA_REVISION, MOS_6526);
            configure(OPT_TIMER_B_BUG,  false);
            configure(OPT_SID_REVISION, MOS_6581);
            configure(OPT_SID_FILTER,   true);
            configure(OPT_GLUE_LOGIC,   GLUE_LOGIC_DISCRETE);
            configure(OPT_RAM_PATTERN,  RAM_PATTERN_C64);
            break;

        default:
            assert(false);
    }
    
    resume();
}

bool
C64::setConfigItem(Option option, i64 value)
{
    switch (option) {
            
        case OPT_VIC_REVISION:
        {
            u64 newFrequency = VICII::getFrequency((VICIIRevision)value);

            if (frequency == newFrequency) {
                assert(durationOfOneCycle == 10000000000 / newFrequency);
                return false;
            }
            
            frequency = (u32)newFrequency;
            durationOfOneCycle = 10000000000 / newFrequency;
            return true;
        }
            
        case OPT_CYCLE_LIMIT:
        {
            if (config.cycleLimit == value) return false;
            
            config.cycleLimit = value;
            return true;
        }
        default:
            return false;
    }
}

void
C64::updateVicFunctionTable()
{
    bool dmaDebug = vic.getConfig().dmaDebug;
    
    trace(VIC_DEBUG, "updateVicFunctionTable (dmaDebug: %d)\n", dmaDebug);
    
    // Assign model independent execution functions
    vicfunc[0] = nullptr;
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
            
        case VICII_PAL_6569_R1:
        case VICII_PAL_6569_R3:
        case VICII_PAL_8565:
            
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
            vicfunc[64] = nullptr;
            vicfunc[65] = nullptr;
            break;
            
        case VICII_NTSC_6567_R56A:
            
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
            vicfunc[65] = nullptr;
            break;
            
        case VICII_NTSC_6567:
        case VICII_NTSC_8562:
            
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
C64::setDebug(bool enable)
{
    suspend();
    HardwareComponent::setDebug(enable);
    resume();
}

void
C64::powerOn()
{
    trace(RUN_DEBUG, "powerOn()\n");
    
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
    trace(RUN_DEBUG, "_powerOn()\n");
        
    putMessage(MSG_POWER_ON);
}

void
C64::powerOff()
{
    trace(RUN_DEBUG, "powerOff()\n");
    
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
    trace(RUN_DEBUG, "_powerOff()\n");
    
    putMessage(MSG_POWER_OFF);
}

void
C64::run()
{
    trace(RUN_DEBUG, "run()\n");
    
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
    trace(RUN_DEBUG, "_run()\n");
    
    // Start the emulator thread
    pthread_create(&p, nullptr, threadMain, (void *)this);
    
    // Inform the GUI
    putMessage(MSG_RUN);
}

void
C64::pause()
{
    trace(RUN_DEBUG, "pause()\n");
    
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
            
        case INSPECTION_TARGET_CPU: cpu.inspect(); break;
        case INSPECTION_TARGET_MEM: mem.inspect(); break;
        case INSPECTION_TARGET_CIA: cia1.inspect(); cia2.inspect(); break;
        case INSPECTION_TARGET_VIC: vic.inspect(); break;
        case INSPECTION_TARGET_SID: sid.inspect(); break;
        default: break;
    }
}

void
C64::_pause()
{
    trace(RUN_DEBUG, "_pause()\n");
    
    // When we reach this line, the emulator thread is already gone
    assert(p == (pthread_t)0);
    
    // Update the recorded debug information
    inspect();
    
    // Inform the GUI
    putMessage(MSG_PAUSE);
}

void
C64::_dump() const
{
    msg("C64:\n");
    msg("----\n\n");
    msg("              Machine type : %s\n", vic.isPAL() ? "PAL" : "NTSC");
    msg("         Frames per second : %f\n", vic.getFramesPerSecond());
    msg("     Rasterlines per frame : %ld\n", vic.getRasterlinesPerFrame());
    msg("     Cycles per rasterline : %d\n", vic.getCyclesPerLine());
    msg("             Current cycle : %llu\n", cpu.cycle);
    msg("             Current frame : %llu\n", frame);
    msg("        Current rasterline : %d\n", rasterLine);
    msg("  Current rasterline cycle : %d\n", rasterCycle);
    msg("              Ultimax mode : %s\n\n", getUltimax() ? "YES" : "NO");
    msg("\n");
}

void
C64::_setWarp(bool enable)
{
    if (enable) {
        
        trace(RUN_DEBUG, "Warp on\n");
        putMessage(MSG_WARP_ON);
        
    } else {

        trace(RUN_DEBUG, "Warp off\n");
        oscillator.restart();
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
    
    trace(RUN_DEBUG, "Suspending (%d)...\n", suspendCounter);
    
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
    
    trace(RUN_DEBUG, "Resuming (%d)...\n", suspendCounter);
    
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
    if (state == EMULATOR_STATE_RUNNING) {
        
        // Assure the emulator thread exists
        assert(p != (pthread_t)0);
        
        // Free the lock by terminating the thread
        requestStop();
        
    } else {
        
        // There must be no emulator thread
        assert(p == (pthread_t)0);
        
        // It's save to free the lock immediately
        pthread_mutex_unlock(&threadLock);
    }
    
    // Acquire the lock
    pthread_mutex_lock(&threadLock);
}

bool
C64::isReady(ErrorCode *err) const
{
    bool mega = hasMega65Rom(ROM_TYPE_BASIC) && hasMega65Rom(ROM_TYPE_KERNAL);
    
    if (!hasRom(ROM_TYPE_BASIC)) {
        if (err) *err = ERROR_ROM_BASIC_MISSING;
        return false;
    }
    if (!hasRom(ROM_TYPE_CHAR)) {
        if (err) *err = ERROR_ROM_CHAR_MISSING;
        return false;
    }
    if (!hasRom(ROM_TYPE_KERNAL) || FORCE_ROM_MISSING) {
        if (err) *err = ERROR_ROM_KERNAL_MISSING;
        return false;
    }
    if (FORCE_MEGA64_MISMATCH ||
        (mega && strcmp(mega65BasicRev(), mega65KernalRev()) != 0)) {
        if (err) *err = ERROR_ROM_MEGA65_MISMATCH;
        return false;
    }
    
    if (err) *err = ERROR_OK;
    return true;
}

void
C64::threadWillStart()
{
    trace(RUN_DEBUG, "Emulator thread started\n");
}

void
C64::threadDidTerminate()
{
    trace(RUN_DEBUG, "Emulator thread terminated\n");
    
    // Trash the thread pointer
    p = (pthread_t)0;
    
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
    trace(RUN_DEBUG, "runLoop()\n");
    
    // Prepare to run
    oscillator.restart();
    // restartTimer();
    
    // Enter the loop
    while (1) {
        
        // Run the emulator
        while (runLoopCtrl == 0) { executeOneFrame(); }
        
        // Check if special action needs to be taken
        if (runLoopCtrl) {
            
            // Are we requested to take a snapshot?
            if (runLoopCtrl & ACTION_FLAG_AUTO_SNAPSHOT) {
                trace(RUN_DEBUG, "RL_AUTO_SNAPSHOT\n");
                autoSnapshot = Snapshot::makeWithC64(this);
                putMessage(MSG_AUTO_SNAPSHOT_TAKEN);
                clearActionFlags(ACTION_FLAG_AUTO_SNAPSHOT);
            }
            if (runLoopCtrl & ACTION_FLAG_USER_SNAPSHOT) {
                trace(RUN_DEBUG, "RL_USER_SNAPSHOT\n");
                userSnapshot = Snapshot::makeWithC64(this);
                putMessage(MSG_USER_SNAPSHOT_TAKEN);
                clearActionFlags(ACTION_FLAG_USER_SNAPSHOT);
            }
            
            // Are we requested to update the debugger info structs?
            if (runLoopCtrl & ACTION_FLAG_INSPECT) {
                trace(RUN_DEBUG, "RL_INSPECT\n");
                inspect();
                clearActionFlags(ACTION_FLAG_INSPECT);
            }
            
            // Did we reach a breakpoint?
            if (runLoopCtrl & ACTION_FLAG_BREAKPOINT) {
                putMessage(MSG_BREAKPOINT_REACHED);
                trace(RUN_DEBUG, "BREAKPOINT_REACHED pc: %x\n", cpu.getPC0());
                clearActionFlags(ACTION_FLAG_BREAKPOINT);
                break;
            }
            
            // Did we reach a watchpoint?
            if (runLoopCtrl & ACTION_FLAG_WATCHPOINT) {
                putMessage(MSG_WATCHPOINT_REACHED);
                trace(RUN_DEBUG, "WATCHPOINT_REACHED pc: %x\n", cpu.getPC0());
                clearActionFlags(ACTION_FLAG_WATCHPOINT);
                break;
            }
            
            // Are we requested to terminate the run loop?
            if (runLoopCtrl & ACTION_FLAG_STOP) {
                clearActionFlags(ACTION_FLAG_STOP);
                trace(RUN_DEBUG, "STOP\n");
                break;
            }
            
            // Are we requested to pull the NMI line down?
            if (runLoopCtrl & ACTION_FLAG_EXTERNAL_NMI) {
                cpu.pullDownNmiLine(INTSRC_EXP);
                trace(RUN_DEBUG, "EXTERNAL_NMI\n");
                clearActionFlags(ACTION_FLAG_EXTERNAL_NMI);
            }
            
            // Is the CPU jammed due the execution of an illegal instruction?
            if (runLoopCtrl & ACTION_FLAG_CPU_JAMMED) {
                putMessage(MSG_CPU_JAMMED);
                trace(RUN_DEBUG, "CPU_JAMMED\n");
                clearActionFlags(ACTION_FLAG_CPU_JAMMED);
                break;
            }
            
            // Has the cycle limit been reached?
            if (runLoopCtrl & ACTION_FLAG_CYCLE_LIMIT) {
                msg("Cycle limit reached (> %lld).", config.cycleLimit);
                exit(1);
            }
            
            assert(runLoopCtrl == 0);
        }
    }
}

void
C64::stopAndGo()
{
    isRunning() ? pause() : run();
}

void
C64::stepInto()
{
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

    // Check if the cycle limit has been reached
    if ((Cycle)cpu.cycle > config.cycleLimit) runLoopCtrl |= ACTION_FLAG_CYCLE_LIMIT;
}

void
C64::executeOneLine()
{
    // Emulate the beginning of a rasterline
    if (rasterCycle == 1) beginRasterLine();
    
    // Emulate the middle of a rasterline
    unsigned lastCycle = vic.getCyclesPerLine();
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
    Cycle cycle = ++cpu.cycle;
    
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
    
    /*
    if ((frame % 50) == 0) printf("Ping %p\n", this);
    */
    
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
    
    // Check if the run loop is requested to stop
    if (stopFlag) { stopFlag = false; signalStop(); }
    
    // Count some sheep (zzzzzz) ...
    oscillator.synchronize();
}

void
C64::setActionFlags(u32 flags)
{
    synchronized { runLoopCtrl |= flags; }
}

void
C64::clearActionFlags(u32 flags)
{
    synchronized { runLoopCtrl &= ~flags; }
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
    autoSnapshot = nullptr;
    return result;
}

Snapshot *
C64::latestUserSnapshot()
{
    Snapshot *result = userSnapshot;
    userSnapshot = nullptr;
    return result;
}

bool
C64::loadFromSnapshot(Snapshot *snapshot)
{
    assert(snapshot);
    assert(snapshot->getData());
    assert(!isRunning());
    
    // Check if this snapshot is compatible with the emulator
    if (snapshot->isTooOld() || FORCE_SNAPSHOT_TOO_OLD) {
        messageQueue.put(MSG_SNAPSHOT_TOO_OLD);
        return false;
    }
    if (snapshot->isTooNew() || FORCE_SNAPSHOT_TOO_NEW) {
        messageQueue.put(MSG_SNAPSHOT_TOO_NEW);
        return false;
    }
    
    // Restore the saved state
    load(snapshot->getData());
    
    // Clear the keyboard matrix to avoid constantly pressed keys
    keyboard.releaseAll();
    
    // Inform the GUI
    messageQueue.put(MSG_SNAPSHOT_RESTORED);
    
    if (SNP_DEBUG) dump();
    return true;
}

u32
C64::romCRC32(RomType type) const
{
    switch (type) {
            
        case ROM_TYPE_BASIC:
            return hasRom(ROM_TYPE_BASIC)  ? util::crc32(mem.rom + 0xA000, 0x2000) : 0;
        case ROM_TYPE_CHAR:
            return hasRom(ROM_TYPE_CHAR)   ? util::crc32(mem.rom + 0xD000, 0x1000) : 0;
        case ROM_TYPE_KERNAL:
            return hasRom(ROM_TYPE_KERNAL) ? util::crc32(mem.rom + 0xE000, 0x2000) : 0;
        case ROM_TYPE_VC1541:
            return hasRom(ROM_TYPE_VC1541) ? util::crc32(drive8.mem.rom, 0x4000) : 0;
        default:
            assert(false);
            return 0;
    }
}

u64
C64::romFNV64(RomType type) const
{
    switch (type) {
            
        case ROM_TYPE_BASIC:
            return hasRom(ROM_TYPE_BASIC)  ? util::fnv_1a_64(mem.rom + 0xA000, 0x2000) : 0;
        case ROM_TYPE_CHAR:
            return hasRom(ROM_TYPE_CHAR)   ? util::fnv_1a_64(mem.rom + 0xD000, 0x1000) : 0;
        case ROM_TYPE_KERNAL:
            return hasRom(ROM_TYPE_KERNAL) ? util::fnv_1a_64(mem.rom + 0xE000, 0x2000) : 0;
        case ROM_TYPE_VC1541:
            return hasRom(ROM_TYPE_VC1541) ? util::fnv_1a_64(drive8.mem.rom, 0x4000) : 0;
        default:
            assert(false);
            return 0;
    }
}

RomIdentifier
C64::romIdentifier(RomType type) const
{
    return RomFile::identifier(romFNV64(type));
}

const char *
C64::romTitle(RomType type) const
{
    switch (type) {
            
        case ROM_TYPE_BASIC:
        {
            // Intercept if a MEGA65 Rom is installed
            if (hasMega65Rom(ROM_TYPE_BASIC)) return "M.E.G.A. C64 OpenROM";
            
            RomIdentifier rev = romIdentifier(ROM_TYPE_BASIC);
            return rev == ROM_UNKNOWN ? "Unknown Basic Rom" : RomFile::title(rev);
        }
        case ROM_TYPE_CHAR:
        {
            // Intercept if a MEGA65 Rom is installed
            if (hasMega65Rom(ROM_TYPE_CHAR)) return "M.E.G.A. C64 OpenROM";
            
            RomIdentifier rev = romIdentifier(ROM_TYPE_CHAR);
            return rev == ROM_UNKNOWN ? "Unknown Character Rom" : RomFile::title(rev);
        }
        case ROM_TYPE_KERNAL:
        {
            // Intercept if a MEGA65 Rom is installed
            if (hasMega65Rom(ROM_TYPE_KERNAL)) return "M.E.G.A. C64 OpenROM";
            
            RomIdentifier rev = romIdentifier(ROM_TYPE_KERNAL);
            return rev == ROM_UNKNOWN ? "Unknown Kernal Rom" : RomFile::title(rev);
        }
        case ROM_TYPE_VC1541:
        {
            RomIdentifier rev = romIdentifier(ROM_TYPE_VC1541);
            return rev == ROM_UNKNOWN ? "Unknown Kernal Rom" : RomFile::title(rev);
        }
        default: assert(false);
    }
    return nullptr;
}

const char *
C64::romSubTitle(u64 fnv) const
{
    RomIdentifier rev = RomFile::identifier(fnv);
    
    if (rev != ROM_UNKNOWN) return RomFile::subTitle(rev);
    
    static char str[32];
    sprintf(str, "FNV %llx", fnv);
    return str;
}

const char *
C64::romSubTitle(RomType type) const
{
    switch (type) {
            
        case ROM_TYPE_BASIC:
        {
            // Intercept if a MEGA65 Rom is installed
            if (hasMega65Rom(ROM_TYPE_BASIC)) return "Free Basic Replacement";
            
            return romSubTitle(romFNV64(ROM_TYPE_BASIC));
        }
        case ROM_TYPE_CHAR:
        {
            // Intercept if a MEGA65 Rom is installed
            if (hasMega65Rom(ROM_TYPE_CHAR)) return "Free Charset Replacement";
            
            return romSubTitle(romFNV64(ROM_TYPE_CHAR));
        }
        case ROM_TYPE_KERNAL:
        {
            // Intercept if a MEGA65 Rom is installed
            if (hasMega65Rom(ROM_TYPE_KERNAL)) return "Free Kernal Replacement";
            
            return romSubTitle(romFNV64(ROM_TYPE_KERNAL));
        }
        case ROM_TYPE_VC1541:
        {
            return romSubTitle(romFNV64(ROM_TYPE_VC1541));
        }
        default: assert(false);
    }
    return nullptr;
}

const char *
C64::romRevision(RomType type) const
{
    switch (type) {
             
         case ROM_TYPE_BASIC:
         {
             // Intercept if a MEGA65 Rom is installed
             if (hasMega65Rom(ROM_TYPE_BASIC)) return mega65BasicRev();
             
             return RomFile::revision(romIdentifier(ROM_TYPE_BASIC));
         }
         case ROM_TYPE_CHAR:
         {
             return RomFile::revision(romIdentifier(ROM_TYPE_CHAR));
         }
         case ROM_TYPE_KERNAL:
         {
             // Intercept if a MEGA65 Rom is installed
             if (hasMega65Rom(ROM_TYPE_KERNAL)) return mega65KernalRev();
             
             return RomFile::revision(romIdentifier(ROM_TYPE_KERNAL));
         }
         case ROM_TYPE_VC1541:
         {
             return RomFile::revision(romIdentifier(ROM_TYPE_VC1541));
         }
         default: assert(false);
     }
     return nullptr;
}

bool
C64::hasRom(RomType type) const
{
    switch (type) {
            
        case ROM_TYPE_BASIC:
        {
            return (mem.rom[0xA000] | mem.rom[0xA001]) != 0x00;
        }
        case ROM_TYPE_CHAR:
        {
            return (mem.rom[0xD000] | mem.rom[0xD001]) != 0x00;
        }
        case ROM_TYPE_KERNAL:
        {
            return (mem.rom[0xE000] | mem.rom[0xE001]) != 0x00;
        }
        case ROM_TYPE_VC1541:
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
C64::hasMega65Rom(RomType type) const
{
    switch (type) {
            
        case ROM_TYPE_BASIC:
        {
            return mem.rom[0xBF52] == 'O' && mem.rom[0xBF53] == 'R';
        }
        case ROM_TYPE_CHAR:
        {
            auto id = romIdentifier(ROM_TYPE_CHAR);
            return id == CHAR_MEGA65 || id == CHAR_PXLFONT_V23;
        }
        case ROM_TYPE_KERNAL:
        {
            return mem.rom[0xE4B9] == 'O' && mem.rom[0xE4BA] == 'R';
        }
        case ROM_TYPE_VC1541:
        {
            return false;
        }
        default: assert(false);
    }
    return false;
}

char *
C64::mega65BasicRev() const
{
    static char rev[17];
    rev[0] = 0;
    
    if (hasMega65Rom(ROM_TYPE_BASIC)) memcpy(rev, &mem.rom[0xBF55], 16);
    rev[16] = 0;
    
    return rev;
}

char *
C64::mega65KernalRev() const
{
    static char rev[17];
    rev[0] = 0;
    
    if (hasMega65Rom(ROM_TYPE_KERNAL)) memcpy(rev, &mem.rom[0xE4BC], 16);
    rev[16] = 0;
    
    return rev;
}

void
C64::installRom(RomFile *file)
{
    assert(file);
    
    switch (file->type()) {
            
        case FILETYPE_BASIC_ROM:
            
            file->flash(mem.rom, 0xA000);
            debug(MEM_DEBUG, "Basic Rom flashed\n");
            debug(MEM_DEBUG, "hasMega65Rom() = %d\n", hasMega65Rom(ROM_TYPE_BASIC));
            debug(MEM_DEBUG, "mega65BasicRev() = %s\n", mega65BasicRev());
            break;
            
        case FILETYPE_CHAR_ROM:
            
            file->flash(mem.rom, 0xD000);
            debug(MEM_DEBUG, "Character Rom flashed\n");
            break;
            
        case FILETYPE_KERNAL_ROM:
            
            file->flash(mem.rom, 0xE000);
            debug(MEM_DEBUG, "Kernal Rom flashed\n");
            debug(MEM_DEBUG, "hasMega65Rom() = %d\n", hasMega65Rom(ROM_TYPE_KERNAL));
            debug(MEM_DEBUG, "mega65KernalRev() = %s\n", mega65KernalRev());
            break;
            
        case FILETYPE_VC1541_ROM:
            
            file->flash(drive8.mem.rom);
            file->flash(drive9.mem.rom);
            debug(MEM_DEBUG, "VC1541 Rom flashed\n");
            break;
            
        default:
            assert(false);
    }
}

void
C64::deleteRom(RomType type)
{
    switch (type) {
            
        case ROM_TYPE_BASIC:
        {
            memset(mem.rom + 0xA000, 0, 0x2000);
            break;
        }
        case ROM_TYPE_CHAR:
        {
            memset(mem.rom + 0xD000, 0, 0x1000);
            break;
        }
        case ROM_TYPE_KERNAL:
        {
            memset(mem.rom + 0xE000, 0, 0x2000);
            break;
        }
        case ROM_TYPE_VC1541:
        {
            memset(drive8.mem.rom, 0, 0x4000);
            memset(drive9.mem.rom, 0, 0x4000);
            break;
        }
        default: assert(false);
    }
}

void
C64::saveRom(RomType type, const char *path)
{
    switch (type) {
            
        case ROM_TYPE_BASIC:
        {
            if (hasRom(ROM_TYPE_BASIC)) {
                RomFile *file = RomFile::make <RomFile> (mem.rom + 0xA000, 0x2000);
                file->writeToFile(path);
                delete file;
            }
            break;
        }
        case ROM_TYPE_CHAR:
        {
            if (hasRom(ROM_TYPE_CHAR)) {
                RomFile *file = RomFile::make <RomFile> (mem.rom + 0xD000, 0x1000);
                file->writeToFile(path);
                delete file;
            }
            break;
        }
        case ROM_TYPE_KERNAL:
        {
            if (hasRom(ROM_TYPE_KERNAL)) {
                RomFile *file = RomFile::make <RomFile> (mem.rom + 0xE000, 0x2000);
                file->writeToFile(path);
                delete file;
            }
            break;
        }
        case ROM_TYPE_VC1541:
        {
            if (hasRom(ROM_TYPE_VC1541)) {
                RomFile *file = RomFile::make <RomFile> (drive8.mem.rom, 0x4000);
                file->writeToFile(path);
                delete file;
            }
            break;
        }
            
        default:
            assert(false);
    }
}

void
C64::saveRom(RomType type, const char *path, ErrorCode *ec)
{
    *ec = ERROR_OK;
    
    try { saveRom(type, path); }
    catch (VC64Error &exception) { *ec = exception.data; }
}

bool
C64::flash(AnyFile *file)
{
    assert(file);
    
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
            result = loadFromSnapshot((Snapshot *)file);
            break;
            
        default:
            assert(false);
            result = false;
    }
    resume();
    return result;
}

bool
C64::flash(AnyCollection *file, unsigned nr)
{
    bool result = true;
    
    u16 addr = file->itemLoadAddr(nr);
    u64 size = file->itemSize(nr);
    if (size <= 2) return false;
    
    suspend();
    
    switch (file->type()) {
            
        case FILETYPE_D64:
        case FILETYPE_T64:
        case FILETYPE_P00:
        case FILETYPE_PRG:
        case FILETYPE_FOLDER:
            
            size = MIN(size - 2, 0x10000 - addr);
            file->copyItem(nr, mem.ram + addr, size, 2);
            break;
            
        default:
            assert(false);
            result = false;
    }
    resume();
    
    messageQueue.put(MSG_FILE_FLASHED);
    return result;
}

bool
C64::flash(const FSDevice &fs, usize nr)
{
    bool result = true;
    
    u16 addr = fs.loadAddr(nr);
    u64 size = fs.fileSize(nr);
    if (size <= 2) return false;
    
    suspend();
                
    size = MIN(size - 2, 0x10000 - addr);
    fs.copyFile(nr, mem.ram + addr, size, 2);

    resume();
    
    messageQueue.put(MSG_FILE_FLASHED);
    return result;
}
