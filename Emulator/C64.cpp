// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "C64.h"
#include "Checksum.h"
#include "IO.h"


//
// Class methods
//

C64::C64()
{
    trace(RUN_DEBUG, "Creating virtual C64 [%p]\n", this);
        
    subComponents = std::vector<C64Component *> {
        
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
        &parCable,
        &datasette,
        &oscillator
    };

    // Set up the initial state
    C64Component::initialize();
    _reset(true);
}

C64::~C64()
{
    trace(RUN_DEBUG, "Destroying C64[%p]\n", this);
}

void
C64::prefix() const
{
    fprintf(stderr, "[%lld] (%3d,%3d) %04X ", frame, rasterLine, rasterCycle, cpu.getPC0());
}

void
C64::initialize(C64Model model)
{
    assert_enum(C64Model, model);
    
    // Power off the emulator
    powerOff();

    // Put all components into their initial state
    C64Component::initialize();
    
    // Apply the selected configuration scheme
    configure(model);
}

void
C64::reset(bool hard)
{
    suspend();
    
    // Execute the standard reset routine
    C64Component::reset(hard);
    
    // Inform the GUI
    putMessage(MSG_RESET);
    
    resume();
}

void
C64::_reset(bool hard)
{
    RESET_SNAPSHOT_ITEMS(hard)
                
    // Initialize the program counter
    cpu.reg.pc = mem.resetVector();
    
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
        case OPT_VIC_POWER_SAVE:
        case OPT_GRAY_DOT_BUG:
        case OPT_GLUE_LOGIC:
        case OPT_HIDE_SPRITES:
        case OPT_SS_COLLISIONS:
        case OPT_SB_COLLISIONS:

        case OPT_PALETTE:
        case OPT_BRIGHTNESS:
        case OPT_CONTRAST:
        case OPT_SATURATION:
            return vic.getConfigItem(option);

        case OPT_DMA_DEBUG_ENABLE:
        case OPT_DMA_DEBUG_MODE:
        case OPT_DMA_DEBUG_OPACITY:
        case OPT_CUT_LAYERS:
        case OPT_CUT_OPACITY:
            return vic.dmaDebugger.getConfigItem(option);
            
        case OPT_CIA_REVISION:
        case OPT_TIMER_B_BUG:
            assert(cia1.getConfigItem(option) == cia2.getConfigItem(option));
            return cia1.getConfigItem(option);

        case OPT_POWER_GRID:
            return oscillator.getConfigItem(option);
            
        case OPT_SID_REVISION:
        case OPT_SID_POWER_SAVE:
        case OPT_SID_FILTER:
        case OPT_SID_ENGINE:
        case OPT_SID_SAMPLING:
        case OPT_AUDVOLL:
        case OPT_AUDVOLR:
            return sid.getConfigItem(option);

        case OPT_RAM_PATTERN:
            return mem.getConfigItem(option);
            
        default:
            assert(false);
            return 0;
    }
}

i64
C64::getConfigItem(Option option, long id) const
{
    const Drive &drive = id == DRIVE8 ? drive8 : drive9;
    
    switch (option) {
            
        case OPT_DMA_DEBUG_ENABLE:
        case OPT_DMA_DEBUG_COLOR:
            return vic.dmaDebugger.getConfigItem(option, id);

        case OPT_SID_ENABLE:
        case OPT_SID_ADDRESS:
        case OPT_AUDPAN:
        case OPT_AUDVOL:
            assert(id >= 0 && id <= 3);
            return sid.getConfigItem(option, id);

        case OPT_DRV_CONNECT:
        case OPT_DRV_AUTO_CONFIG:
        case OPT_DRV_TYPE:
        case OPT_DRV_RAM:
        case OPT_DRV_PARCABLE:
        case OPT_DRV_POWER_SAVE:
        case OPT_DRV_POWER_SWITCH:
        case OPT_DRV_EJECT_DELAY:
        case OPT_DRV_SWAP_DELAY:
        case OPT_DRV_INSERT_DELAY:
        case OPT_DRV_PAN:
        case OPT_DRV_POWER_VOL:
        case OPT_DRV_STEP_VOL:
        case OPT_DRV_INSERT_VOL:
        case OPT_DRV_EJECT_VOL:
            return drive.getConfigItem(option);

        case OPT_MOUSE_MODEL:
        case OPT_SHAKE_DETECTION:
        case OPT_MOUSE_VELOCITY:
            if (id == PORT_ONE) return port1.mouse.getConfigItem(option);
            if (id == PORT_TWO) return port2.mouse.getConfigItem(option);
            assert(false);

        case OPT_AUTOFIRE:
        case OPT_AUTOFIRE_BULLETS:
        case OPT_AUTOFIRE_DELAY:
            if (id == PORT_ONE) return port1.joystick.getConfigItem(option);
            if (id == PORT_TWO) return port2.joystick.getConfigItem(option);
            assert(false);

        default:
            assert(false);
            return 0;
    }
}

void
C64::configure(Option option, i64 value)
{
    _configure(option, value);    
    msgQueue.put(MSG_CONFIG);
}

void
C64::_configure(Option option, i64 value)
{
    trace(CNF_DEBUG, "configure(option: %lld, value: %lld\n", option, value);

    // Propagate configuration request to all components
    C64Component::configure(option, value);
}

void
C64::configure(Option option, long id, i64 value)
{
    _configure(option, id, value);
    msgQueue.put(MSG_CONFIG);
}

void
C64::_configure(Option option, long id, i64 value)
{
    trace(CNF_DEBUG, "configure(option: %lld, id: %ld, value: %lld\n", option, id, value);
    
    // Propagate configuration request to all components
    C64Component::configure(option, id, value);
}

void
C64::configure(C64Model model)
{
    suspend();
    
    switch(model) {
            
        case C64_MODEL_PAL:
            _configure(OPT_VIC_REVISION, VICII_PAL_6569_R3);
            _configure(OPT_GRAY_DOT_BUG, false);
            _configure(OPT_CIA_REVISION, MOS_6526);
            _configure(OPT_TIMER_B_BUG,  true);
            _configure(OPT_SID_REVISION, MOS_6581);
            _configure(OPT_SID_FILTER,   true);
            _configure(OPT_POWER_GRID,   GRID_STABLE_50HZ);
            _configure(OPT_GLUE_LOGIC,   GLUE_LOGIC_DISCRETE);
            configure(OPT_RAM_PATTERN,  RAM_PATTERN_VICE);
            break;
            
        case C64_MODEL_PAL_II:
            _configure(OPT_VIC_REVISION, VICII_PAL_8565);
            _configure(OPT_GRAY_DOT_BUG, true);
            _configure(OPT_CIA_REVISION, MOS_8521);
            _configure(OPT_TIMER_B_BUG,  false);
            _configure(OPT_SID_REVISION, MOS_8580);
            _configure(OPT_SID_FILTER,   true);
            _configure(OPT_POWER_GRID,   GRID_STABLE_50HZ);
            _configure(OPT_GLUE_LOGIC,   GLUE_LOGIC_IC);
            configure(OPT_RAM_PATTERN,  RAM_PATTERN_VICE);
            break;

        case C64_MODEL_PAL_OLD:
            _configure(OPT_VIC_REVISION, VICII_PAL_6569_R1);
            _configure(OPT_GRAY_DOT_BUG, false);
            _configure(OPT_CIA_REVISION, MOS_6526);
            _configure(OPT_TIMER_B_BUG,  true);
            _configure(OPT_SID_REVISION, MOS_6581);
            _configure(OPT_SID_FILTER,   true);
            _configure(OPT_POWER_GRID,   GRID_STABLE_50HZ);
            _configure(OPT_GLUE_LOGIC,   GLUE_LOGIC_DISCRETE);
            configure(OPT_RAM_PATTERN,  RAM_PATTERN_VICE);
            break;

        case C64_MODEL_NTSC:
            _configure(OPT_VIC_REVISION, VICII_NTSC_6567);
            _configure(OPT_GRAY_DOT_BUG, false);
            _configure(OPT_CIA_REVISION, MOS_6526);
            _configure(OPT_TIMER_B_BUG,  false);
            _configure(OPT_SID_REVISION, MOS_6581);
            _configure(OPT_SID_FILTER,   true);
            _configure(OPT_POWER_GRID,   GRID_STABLE_60HZ);
            _configure(OPT_GLUE_LOGIC,   GLUE_LOGIC_DISCRETE);
            configure(OPT_RAM_PATTERN,  RAM_PATTERN_VICE);
            break;

        case C64_MODEL_NTSC_II:
            _configure(OPT_VIC_REVISION, VICII_NTSC_8562);
            _configure(OPT_GRAY_DOT_BUG, true);
            _configure(OPT_CIA_REVISION, MOS_8521);
            _configure(OPT_TIMER_B_BUG,  true);
            _configure(OPT_SID_REVISION, MOS_8580);
            _configure(OPT_SID_FILTER,   true);
            _configure(OPT_POWER_GRID,   GRID_STABLE_60HZ);
            _configure(OPT_GLUE_LOGIC,   GLUE_LOGIC_IC);
            configure(OPT_RAM_PATTERN,  RAM_PATTERN_VICE);
            break;

        case C64_MODEL_NTSC_OLD:
            _configure(OPT_VIC_REVISION, VICII_NTSC_6567_R56A);
            _configure(OPT_GRAY_DOT_BUG, false);
            _configure(OPT_CIA_REVISION, MOS_6526);
            _configure(OPT_TIMER_B_BUG,  false);
            _configure(OPT_SID_REVISION, MOS_6581);
            _configure(OPT_SID_FILTER,   true);
            _configure(OPT_POWER_GRID,   GRID_STABLE_60HZ);
            _configure(OPT_GLUE_LOGIC,   GLUE_LOGIC_DISCRETE);
            configure(OPT_RAM_PATTERN,  RAM_PATTERN_VICE);
            break;

        default:
            assert(false);
    }
    
    resume();
}

void
C64::setConfigItem(Option option, i64 value)
{
    switch (option) {
            
        case OPT_VIC_REVISION:
        {
            u64 newFrequency = VICII::getFrequency((VICIIRevision)value);
            isize newFps = VICII::getFps((VICIIRevision)value);
            
            frequency = (u32)newFrequency;
            durationOfOneCycle = 10000000000 / newFrequency;
            thread.setSyncDelay(1000000000 / newFps);
            return;
        }
            
        default:
            return;
    }
}

bool
C64::readyToPowerOn()
{
    debug(RUN_DEBUG, "readyToPowerOn()\n");
    return isReady();
}

void
C64::threadPowerOff()
{
    debug(RUN_DEBUG, "threadPowerOff()\n");
    
    // Power off all subcomponents
    C64Component::powerOff();

    // Update the recorded debug information
    inspect();
    
    // Inform the GUI
    msgQueue.put(MSG_POWER_OFF);
}

void
C64::threadPowerOn()
{
    debug(RUN_DEBUG, "threadPowerOn()\n");
    
    // Perform a reset
    hardReset();
            
    // Power on all subcomponents
    C64Component::powerOn();
    
    // Update the recorded debug information
    inspect();

    // Inform the GUI
    msgQueue.put(MSG_POWER_ON);
}

void
C64::threadRun()
{
    debug(RUN_DEBUG, "threadRun()\n");
    
    // Launch all subcomponents
    C64Component::run();
    
    // Inform the GUI
    msgQueue.put(MSG_RUN);
}

void
C64::threadPause()
{
    debug(RUN_DEBUG, "threadPause()\n");
    
    // Finish the current instruction to reach a clean state
    finishInstruction();
    
    // Enter pause mode
    C64Component::pause();
    
    // Update the recorded debug information
    inspect();
    
    // Inform the GUI
    msgQueue.put(MSG_PAUSE);
}

void
C64::threadHalt()
{
    debug(RUN_DEBUG, "threadHalt()\n");

    // Inform the GUI
    msgQueue.put(MSG_HALT);
}

void
C64::threadWarpOff()
{
    debug(WARP_DEBUG, "threadWarpOff()\n");
    C64Component::warpOff();
    
    // Inform the GUI
    msgQueue.put(MSG_WARP_OFF);
}

void
C64::threadWarpOn()
{
    debug(WARP_DEBUG, "threadWarpOn()\n");
    C64Component::warpOn();

    // Inform the GUI
    msgQueue.put(MSG_WARP_ON);
}

void
C64::threadExecute()
{
    // Run the emulator
    executeOneFrame();
    
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
            thread.newState = EXEC_PAUSED;
        }
        
        // Did we reach a watchpoint?
        if (runLoopCtrl & ACTION_FLAG_WATCHPOINT) {
            putMessage(MSG_WATCHPOINT_REACHED);
            trace(RUN_DEBUG, "WATCHPOINT_REACHED pc: %x\n", cpu.getPC0());
            clearActionFlags(ACTION_FLAG_WATCHPOINT);
            thread.newState = EXEC_PAUSED;
        }
        
        // Are we requested to terminate the run loop?
        if (runLoopCtrl & ACTION_FLAG_STOP) {
            clearActionFlags(ACTION_FLAG_STOP);
            trace(RUN_DEBUG, "STOP\n");
            thread.newState = EXEC_PAUSED;
        }
        
        // Are we requested to pull the NMI line down?
        if (runLoopCtrl & ACTION_FLAG_EXTERNAL_NMI) {
            cpu.pullDownNmiLine(INTSRC_EXP);
            trace(RUN_DEBUG, "EXTERNAL_NMI\n");
            clearActionFlags(ACTION_FLAG_EXTERNAL_NMI);
        }
        
        // Is the CPU jammed due the execution of an illegal instruction?
        if (runLoopCtrl & ACTION_FLAG_CPU_JAM) {
            putMessage(MSG_CPU_JAMMED);
            trace(RUN_DEBUG, "CPU_JAMMED\n");
            clearActionFlags(ACTION_FLAG_CPU_JAM);
            thread.newState = EXEC_PAUSED;
        }
                    
        assert(runLoopCtrl == 0);
    }
}

void
C64::debugOn()
{
    suspend();
    C64Component::debugOn();
    resume();
}

void
C64::debugOff()
{
    suspend();
    C64Component::debugOff();
    resume();
}

void
C64::_powerOn()
{
}

void
C64::_powerOff()
{
}

void
C64::_run()
{
}

void
C64::_pause()
{
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
C64::_dump(dump::Category category, std::ostream& os) const
{
    using namespace util;
        
    if (category & dump::State) {
                
        os << tab("Machine type") << bol(vic.isPAL(), "PAL", "NTSC") << std::endl;
        os << tab("Frames per second") << vic.getFramesPerSecond() << std::endl;
        os << tab("Rasterlines per frame") << vic.getRasterlinesPerFrame() << std::endl;
        os << tab("Cycles per rasterline") << vic.getCyclesPerLine() << std::endl;
        os << tab("Current cycle") << cpu.cycle << std::endl;
        os << tab("Current frame") << frame << std::endl;
        os << tab("Current rasterline") << rasterLine << std::endl;
        os << tab("Current rasterline cycle") << dec(rasterCycle) << std::endl;
        os << tab("Ultimax mode") << bol(getUltimax()) << std::endl;
        os << tab("Warp mode") << bol(inWarpMode()) << std::endl;
        os << tab("Debug mode") << bol(debugMode) << std::endl;
    }
}

void
C64::_debugOn()
{
    vic.updateVicFunctionTable();
}

void
C64::_debugOff()
{
    vic.updateVicFunctionTable();
}

void
C64::suspend()
{
    // debug(RUN_DEBUG, "Suspending (%zu)...\n", suspendCounter);
    
    if (suspendCounter || isRunning()) {
        pause();
        suspendCounter++;
    }
}

void
C64::resume()
{
    // debug(RUN_DEBUG, "Resuming (%zd)...\n", suspendCounter);
    
    if (suspendCounter && --suspendCounter == 0) {
        run();
    }
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
    (vic.*vic.vicfunc[rasterCycle])();
    if (cycle >= cia1.wakeUpCycle) cia1.executeOneCycle();
    if (cycle >= cia2.wakeUpCycle) cia2.executeOneCycle();
    if (iec.isDirtyC64Side) iec.updateIecLinesC64Side();
    
    // Second clock phase (o2 high)
    cpu.executeOneCycle();
    if (drive8.needsEmulation) drive8.execute(durationOfOneCycle);
    if (drive9.needsEmulation) drive9.execute(durationOfOneCycle);
    datasette.execute();
    
    rasterCycle++;
}

void
C64::finishInstruction()
{
    while (!cpu.inFetchPhase()) executeOneCycle();
}

void
C64::finishFrame()
{
    while (rasterLine != 0 || rasterCycle > 1) executeOneCycle();
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
    cia1.tod.increment();
    cia2.tod.increment();

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
    datasette.vsyncHandler();
    retroShell.vsyncHandler();
    recorder.vsyncHandler();
    
    // Count some sheep (zzzzzz) ...
    // if (!warp) oscillator.synchronize();
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
        msgQueue.put(MSG_SNAPSHOT_TOO_OLD);
        return false;
    }
    if (snapshot->isTooNew() || FORCE_SNAPSHOT_TOO_NEW) {
        msgQueue.put(MSG_SNAPSHOT_TOO_NEW);
        return false;
    }
    
    // Restore the saved state
    load(snapshot->getData());
    
    // Clear the keyboard matrix to avoid constantly pressed keys
    keyboard.releaseAll();
    
    // Inform the GUI
    msgQueue.put(MSG_SNAPSHOT_RESTORED);
    
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
            return drive8.mem.romCRC32();
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
            return drive8.mem.romFNV64();
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
            return rev == ROM_UNKNOWN ? "Unknown Drive Firmware" : RomFile::title(rev);
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
            assert(drive8.mem.hasRom() == drive9.mem.hasRom());
            return drive8.mem.hasRom();
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
C64::loadRom(const string &path)
{
    RomFile *file = RomFile::make <RomFile> (path.c_str());
    loadRom(file);
}

void
C64::loadRom(const string &path, ErrorCode *ec)
{
    try { loadRom(path); *ec = ERROR_OK; }
    catch (VC64Error &exception) { *ec = exception.data; }
}

void
C64::loadRom(RomFile *file)
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
            
            drive8.mem.loadRom(file->data, file->size);
            drive9.mem.loadRom(file->data, file->size);
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
            drive8.mem.deleteRom();
            drive9.mem.deleteRom();
            break;
        }
        default: assert(false);
    }
}

void
C64::saveRom(RomType type, const string &path)
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
                drive8.mem.saveRom(path);
            }
            break;
        }
            
        default:
            assert(false);
    }
}

void
C64::saveRom(RomType type, const string &path, ErrorCode *ec)
{
    try { saveRom(type, path); *ec = ERROR_OK; }
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
            drive8.mem.loadRom((RomFile *)file);
            drive9.mem.loadRom((RomFile *)file);
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
C64::flash(AnyCollection *file, isize nr)
{
    bool result = true;
    
    u16 addr = (u16)file->itemLoadAddr(nr);
    u64 size = (u64)file->itemSize(nr);
    if (size <= 2) return false;
    
    suspend();
    
    switch (file->type()) {
            
        case FILETYPE_D64:
        case FILETYPE_T64:
        case FILETYPE_P00:
        case FILETYPE_PRG:
        case FILETYPE_FOLDER:
            
            size = std::min(size - 2, (u64)(0x10000 - addr));
            file->copyItem(nr, mem.ram + addr, size, 2);
            break;
            
        default:
            assert(false);
            result = false;
    }
    resume();
    
    msgQueue.put(MSG_FILE_FLASHED);
    return result;
}

bool
C64::flash(const FSDevice &fs, isize nr)
{
    bool result = true;
    
    u16 addr = fs.loadAddr(nr);
    u64 size = fs.fileSize(nr);
    if (size <= 2) return false;
    
    suspend();
                
    size = std::min(size - 2, (u64)(0x10000 - addr));
    fs.copyFile(nr, mem.ram + addr, size, 2);

    resume();
    
    msgQueue.put(MSG_FILE_FLASHED);
    return result;
}
