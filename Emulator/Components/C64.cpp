// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// This FILE is dual-licensed. You are free to choose between:
//
//     - The GNU General Public License v3 (or any later version)
//     - The Mozilla Public License v2
//
// SPDX-License-Identifier: GPL-3.0-or-later OR MPL-2.0
// -----------------------------------------------------------------------------

#include "config.h"
#include "Emulator.h"
#include "Checksum.h"
#include "IOUtils.h"
#include <algorithm>

namespace vc64 {

string
C64::version()
{
    string result;

    result = std::to_string(VER_MAJOR) + "." + std::to_string(VER_MINOR);
    if constexpr (VER_SUBMINOR > 0) result += "." + std::to_string(VER_SUBMINOR);
    if constexpr (VER_BETA > 0) result += 'b' + std::to_string(VER_BETA);

    return result;
}

string
C64::build()
{
    string db = debugBuild ? " [DEBUG BUILD]" : "";

    return version() + db + " (" + __DATE__ + " " + __TIME__ + ")";
}

const char *
C64::eventName(EventSlot slot, EventID id)
{
    assert_enum(EventSlot, slot);

    switch (slot) {

        case SLOT_CIA1:
        case SLOT_CIA2:

            switch (id) {
                case EVENT_NONE:    return "none";
                case CIA_EXECUTE:   return "CIA_EXECUTE";
                case CIA_WAKEUP:    return "CIA_WAKEUP";
                default:            return "*** INVALID ***";
            }
            break;

        case SLOT_SEC:

            switch (id) {

                case EVENT_NONE:    return "none";
                case SEC_TRIGGER:   return "SEC_TRIGGER";
                default:            return "*** INVALID ***";
            }
            break;

        case SLOT_IEC:

            switch (id) {

                case EVENT_NONE:    return "none";
                case IEC_UPDATE:    return "IEC_UPDATE";
                default:            return "*** INVALID ***";
            }
            break;

        case SLOT_DAT:

            switch (id) {

                case EVENT_NONE:    return "none";
                case DAT_EXECUTE:   return "DAT_EXECUTE";
                default:            return "*** INVALID ***";
            }
            break;

        case SLOT_TER:

            switch (id) {

                case EVENT_NONE:    return "none";
                case TER_TRIGGER:   return "TER_TRIGGER";
                default:            return "*** INVALID ***";
            }
            break;

        case SLOT_AFI1:
        case SLOT_AFI2:

            switch (id) {

                case EVENT_NONE:    return "none";
                case AFI_FIRE:      return "AFI_FIRE";
                default:            return "*** INVALID ***";
            }
            break;

        case SLOT_MOT:

            switch (id) {

                case EVENT_NONE:    return "none";
                case MOT_START:     return "MOT_START";
                case MOT_STOP:      return "MOT_STOP";
                default:            return "*** INVALID ***";
            }
            break;

        case SLOT_DC8:
        case SLOT_DC9:

            switch (id) {

                case EVENT_NONE:    return "none";
                case DCH_INSERT:    return "DCH_INSERT";
                case DCH_EJECT:     return "DCH_EJECT";
                default:            return "*** INVALID ***";
            }
            break;

        case SLOT_RSH:

            switch (id) {

                case EVENT_NONE:        return "none";
                case RSH_WAKEUP:        return "RSH_WAKEUP";
                default:                return "*** INVALID ***";
            }
            break;

        case SLOT_KEY:

            switch (id) {

                case EVENT_NONE:    return "none";
                case KEY_AUTO_TYPE: return "AUTO_TYPE";
                default:            return "*** INVALID ***";
            }
            break;

        case SLOT_ALA:

            switch (id) {

                case EVENT_NONE:    return "none";
                case ALA_TRIGGER:   return "ALA_TRIGGER";
                default:            return "*** INVALID ***";
            }
            break;

        case SLOT_INS:

            switch (id) {

                case EVENT_NONE:    return "none";
                case INS_C64:       return "INS_C64";
                case INS_CPU:       return "INS_CPU";
                case INS_MEM:       return "INS_MEM";
                case INS_CIA:       return "INS_CIA";
                case INS_VICII:     return "INS_VICII";
                case INS_SID:       return "INS_SID";
                case INS_EVENTS:    return "INS_EVENTS";
                default:            return "*** INVALID ***";
            }
            break;

        default:
            fatalError;
    }
}

C64::C64(class Emulator& ref) : CoreComponent(ref)
{
    trace(RUN_DEBUG, "Creating virtual C64\n");

    subComponents = std::vector<CoreComponent *> {
        
        &mem,
        &cpu,
        &cia1, &cia2,
        &vic,
        &muxer,
        &supply,
        &port1,
        &port2,
        &expansionport,
        &iec,
        &keyboard,
        &drive8,
        &drive9,
        &parCable,
        &datasette,
        &retroShell,
        &regressionTester,
        &recorder
    };

    // Assign a unique ID to the CPU
    cpu.setID(0);
}

C64::~C64()
{
    trace(RUN_DEBUG, "Destructing virtual C64\n");
}

void
C64::prefix() const
{
    fprintf(stderr, "[%lld] (%3d,%3d) %04X ", frame, scanline, rasterCycle, cpu.getPC0());
}

void
C64::hardReset()
{
    // Start over from a zeroed-out state
    Serializable::hardReset();

    // Let all components perform their specific actions
    reset(true);

    // Reinitialize the program counter
    cpu.reg.pc = cpu.reg.pc0 = mem.resetVector();
    
    // Inform the GUI
    msgQueue.put(MSG_RESET);
}

void
C64::softReset()
{
    // Start over from a zeroed-out state
    Serializable::softReset();

    // Let all components perform their specific actions
    reset(false);

    // Reinitialize the program counter
    cpu.reg.pc = cpu.reg.pc0 = mem.resetVector();

    // Inform the GUI
    msgQueue.put(MSG_RESET);
}

void
C64::initialize()
{
    auto load = [&](const string &path) {

        msg("Trying to load Rom from %s...\n", path.c_str());

        try { loadRom(path); } catch (std::exception& e) {
            warn("Error: %s\n", e.what());
        }
    };

    if (auto path = Emulator::defaults.getString("BASIC_PATH");  path != "") load(path);
    if (auto path = Emulator::defaults.getString("CHAR_PATH");   path != "") load(path);
    if (auto path = Emulator::defaults.getString("KERNAL_PATH"); path != "") load(path);
    if (auto path = Emulator::defaults.getString("VC1541_PATH"); path != "") load(path);

    CoreComponent::initialize();
}

void
C64::operator << (SerResetter &worker)
{
    auto insEvent = id[SLOT_INS];

    // Reset all items
    serialize(worker);

    // Initialize all event slots
    for (isize i = 0; i < SLOT_COUNT; i++) {

        trigger[i] = NEVER;
        id[i] = (EventID)0;
        data[i] = 0;
    }

    // Schedule initial events
    scheduleAbs<SLOT_CIA1>(cpu.clock, CIA_EXECUTE);
    scheduleAbs<SLOT_CIA2>(cpu.clock, CIA_EXECUTE);
    if (insEvent) scheduleRel <SLOT_INS> (0, insEvent);

    flags = 0;
    rasterCycle = 1;
}

void
C64::updateClockFrequency()
{
    auto nativeFps = vic.getFps();
    auto chosenFps = emulator.refreshRate();

    auto nativeFrequency = vic.getFrequency();
    auto chosenFrequency = nativeFrequency * chosenFps / nativeFps;

    muxer.setClockFrequency((u32)chosenFrequency);
    durationOfOneCycle = 10000000000 / vic.getFrequency();
}

InspectionTarget
C64::getInspectionTarget() const
{
    switch(id[SLOT_INS]) {

        case EVENT_NONE:  return INSPECTION_NONE;
        case INS_C64:     return INSPECTION_C64;
        case INS_CPU:     return INSPECTION_CPU;
        case INS_MEM:     return INSPECTION_MEM;
        case INS_CIA:     return INSPECTION_CIA;
        case INS_VICII:   return INSPECTION_VICII;
        case INS_SID:     return INSPECTION_SID;
        case INS_EVENTS:  return INSPECTION_EVENTS;

        default:
            fatalError;
    }
}

void
C64::setInspectionTarget(InspectionTarget target, Cycle trigger)
{
    EventID id;

    {   SUSPENDED

        switch(target) {

            case INSPECTION_NONE:    cancel<SLOT_INS>(); return;

            case INSPECTION_C64:     id = INS_C64; break;
            case INSPECTION_CPU:     id = INS_CPU; break;
            case INSPECTION_MEM:     id = INS_MEM; break;
            case INSPECTION_CIA:     id = INS_CIA; break;
            case INSPECTION_VICII:   id = INS_VICII; break;
            case INSPECTION_SID:     id = INS_SID; break;
            case INSPECTION_EVENTS:  id = INS_EVENTS; break;

            default:
                fatalError;
        }

        scheduleRel<SLOT_INS>(trigger, id);
        if (trigger == 0) processINSEvent(id);
    }
}

void
C64::execute()
{
    if (emulator.get(OPT_VICII_POWER_SAVE)) {
        execute(emulator.isWarping() && (frame & 7) != 0);
    } else {
        execute(false);
    }
}

void 
C64::execute(bool headless)
{
    setHeadless(headless);

    cpu.debugger.watchpointPC = -1;
    cpu.debugger.breakpointPC = -1;

    switch ((drive8.needsEmulation ? 2 : 0) + (drive9.needsEmulation ? 1 : 0)) {

        case 0b00: execute <false,false> (); break;
        case 0b01: execute <false,true>  (); break;
        case 0b10: execute <true,false>  (); break;
        case 0b11: execute <true,true>   (); break;

        default:
            fatalError;
    }
}

template <bool enable8, bool enable9> void
C64::execute()
{
    auto lastCycle = vic.getCyclesPerLine();

    try {

        do {

            // Run the emulator for the (rest of the) current scanline
            for (; rasterCycle <= lastCycle; rasterCycle++) {

                // Execute one cycle
                executeCycle<enable8, enable9>();

                // Process all pending flags
                if (flags) processFlags();
            }

            // Finish the scanline
            endScanline();

        } while (scanline != 0);

        // Finish the current instruction
        finishInstruction<enable8, enable9>();

    } catch (StateChangeException &) {

        // Finish the scanline
        if (++rasterCycle > lastCycle) endScanline();

        // Finish the current instruction
        finishInstruction<enable8, enable9>();

        // Rethrow the exception
        throw;
    }
}

template <bool enable8, bool enable9>
alwaysinline void C64::executeCycle()
{
    //
    // Run the emulator for one cycle
    //

    //  <---------- o2 low phase ----------->|<- o2 high phase ->|
    //                                       |                   |
    // ,-- C64 ------------------------------|-------------------|--,
    // |   ,-----,     ,-----,     ,-----,   |    ,-----,        |  |
    // |   |     |     |     |     |     |   |    |     |        |  |
    // '-->| CIA | --> | CIA | --> | VIC | --|--> | CPU | -------|--'
    //     |  1  |     |  2  |     |     |   |    |     |        |
    //     '-----'     '-----'     '-----'   |    '-----'        |
    //                                       |                   |
    //                                       |    ,--------,     |
    //                                       |    |        |     |
    // ,-- Drive ----------------------------|--> | VC1541 | ----|--,
    // |                                     |    |        |     |  |
    // |                                     |    '--------'     |  |
    // '-------------------------------------|-------------------|--'

    Cycle cycle = ++cpu.clock;

    //
    // First clock phase (o2 low)
    //

    if (nextTrigger <= cycle) processEvents(cycle);
    (vic.*vic.vicfunc[rasterCycle])();


    //
    // Second clock phase (o2 high)
    //

    cpu.execute<MOS_6510>();
    if constexpr (enable8) { drive8.execute(durationOfOneCycle); }
    if constexpr (enable9) { drive9.execute(durationOfOneCycle); }
}

template <bool enable8, bool enable9> void 
C64::finishInstruction()
{
    while (!cpu.inFetchPhase()) {

        executeCycle<enable8,enable9>();
        rasterCycle++;
    }
}

void
C64::processFlags()
{
    bool interrupt = false;

    // Did we reach a breakpoint?
    if (flags & RL::BREAKPOINT) {
        clearFlag(RL::BREAKPOINT);
        msgQueue.put(MSG_BREAKPOINT_REACHED, CpuMsg {u16(cpu.debugger.breakpointPC)});
        interrupt = true;
    }

    // Did we reach a watchpoint?
    if (flags & RL::WATCHPOINT) {
        clearFlag(RL::WATCHPOINT);
        msgQueue.put(MSG_WATCHPOINT_REACHED, CpuMsg {u16(cpu.debugger.watchpointPC)});
        interrupt = true;
    }

    // Are we requested to terminate the run loop?
    if (flags & RL::STOP) {
        clearFlag(RL::STOP);
        interrupt = true;
    }

    // Are we requested to pull the NMI line down?
    if (flags & RL::EXTERNAL_NMI) {
        clearFlag(RL::EXTERNAL_NMI);
        cpu.pullDownNmiLine(INTSRC_EXP);
        interrupt = true;
    }

    // Is the CPU jammed due the execution of an illegal instruction?
    if (flags & RL::CPU_JAM) {
        clearFlag(RL::CPU_JAM);
        msgQueue.put(MSG_CPU_JAMMED);
        interrupt = true;
    }

    // Are we requested to simulate a BRK instruction
    if (flags & RL::EXTERNAL_BRK) {
        clearFlag(RL::EXTERNAL_BRK);
        cpu.next = BRK;
        cpu.reg.pc0 = cpu.reg.pc - 1;
    }

    // Are we requested to run for a single cycle?
    if (flags & RL::SINGLE_STEP) {

        if (!stepTo.has_value() || *stepTo == cpu.getPC0()) {

            clearFlag(RL::SINGLE_STEP);
            interrupt = true;
        }
    }

    if (interrupt) throw StateChangeException(STATE_PAUSED);
}

void 
C64::fastForward(isize frames)
{
    auto target = frame + frames;

    // Execute until the target frame has been reached
    while (frame < target) execute();
}

void
C64::_isReady() const
{
    bool mega = hasMega65Rom(ROM_TYPE_BASIC) && hasMega65Rom(ROM_TYPE_KERNAL);
    
    if (!hasRom(ROM_TYPE_BASIC)) {
        throw VC64Error(ERROR_ROM_BASIC_MISSING);
    }
    if (!hasRom(ROM_TYPE_CHAR)) {
        throw VC64Error(ERROR_ROM_CHAR_MISSING);
    }
    if (!hasRom(ROM_TYPE_KERNAL) || FORCE_ROM_MISSING) {
        throw VC64Error(ERROR_ROM_KERNAL_MISSING);
    }
    if (FORCE_MEGA64_MISMATCH || (mega && string(mega65BasicRev()) != string(mega65KernalRev()))) {
        throw VC64Error(ERROR_ROM_MEGA65_MISMATCH);
    }
}

void
C64::_powerOn()
{
    debug(RUN_DEBUG, "_powerOn\n");
    
    hardReset();
    msgQueue.put(MSG_POWER, 1);
}

void
C64::_powerOff()
{
    debug(RUN_DEBUG, "_powerOff\n");

    msgQueue.put(MSG_POWER, 0);
}

void
C64::_run()
{
    debug(RUN_DEBUG, "_run\n");
    assert(cpu.inFetchPhase());

    msgQueue.put(MSG_RUN);
}

void
C64::_pause()
{
    debug(RUN_DEBUG, "_pause\n");
    assert(cpu.inFetchPhase());

    msgQueue.put(MSG_PAUSE);
}

void
C64::_halt()
{
    debug(RUN_DEBUG, "_halt\n");

    msgQueue.put(MSG_SHUTDOWN);
}

void
C64::_warpOn()
{
    debug(RUN_DEBUG, "_warpOn\n");

    msgQueue.put(MSG_WARP, 1);
}

void
C64::_warpOff()
{
    debug(RUN_DEBUG, "_warpOff\n");

    msgQueue.put(MSG_WARP, 0);
}

void
C64::_trackOn()
{
    debug(RUN_DEBUG, "_trackOn\n");

    msgQueue.put(MSG_TRACK, 1);
}

void
C64::_trackOff()
{
    debug(RUN_DEBUG, "_trackOff\n");

    msgQueue.put(MSG_TRACK, 0);
}

isize
C64::size()
{
    return Serializable::size() + 8 /* checksum */;
}

isize
C64::load(const u8 *buffer)
{
    assert(!isRunning());

    // Load checksum
    isize count = 8;
    auto hash = read64(buffer);

    // Load internal state
    count += Serializable::load(buffer);

    // Check integrity
    debug(SNP_DEBUG, "Loaded %ld bytes (expected %ld)\n", count, size());

    if (hash != checksum() || FORCE_SNAP_CORRUPTED) {

        debug(SNP_DEBUG, "Corrupted snapshot detected\n");
        throw VC64Error(ERROR_SNAP_CORRUPTED);
    }

    return count;
}

isize
C64::save(u8 *buffer)
{
    // Save checksum
    isize count = 8;
    write64(buffer, checksum());

    // Save internal state
    count += Serializable::save(buffer);

    // Check integrity
    debug(SNP_DEBUG, "Saved %ld bytes (expected %ld)\n", count, size());
    assert(count == size());

    return count;
}

void
C64::_dump(Category category, std::ostream& os) const
{
    using namespace util;

    if (category == Category::Config) {

        dumpConfig(os);
    }

    if (category == Category::State) {

        os << tab("Power");
        os << bol(isPoweredOn()) << std::endl;
        os << tab("Running");
        os << bol(isRunning()) << std::endl;
        os << tab("Suspended");
        os << bol(isSuspended()) << std::endl;
        os << tab("Warping");
        os << bol(emulator.isWarping()) << std::endl;
        os << tab("Tracking");
        os << bol(emulator.isTracking()) << std::endl;
        os << std::endl;

        os << tab("Ultimax mode");
        os << bol(getUltimax()) << std::endl;
        os << std::endl;

        os << tab("Frame");
        os << dec(frame) << std::endl;
        os << tab("CPU progress");
        os << dec(cpu.clock) << " Cycles" << std::endl;
        os << tab("CIA 1 progress");
        os << dec(cia1.isSleeping() ? cia1.sleepCycle : cpu.clock) << " Cycles" << std::endl;
        os << tab("CIA 2 progress");
        os << dec(cia2.isSleeping() ? cia2.sleepCycle : cpu.clock) << " Cycles" << std::endl;
    }

    if (category == Category::Summary) {

        auto vicRev = (VICIIRevision)emulator.get(OPT_VICII_REVISION);
        auto sidRev = (SIDRevision)emulator.get(OPT_SID_REVISION);
        auto cia1Rev = (CIARevision)cia1.getOption(OPT_CIA_REVISION);
        auto cia2Rev = (CIARevision)cia2.getOption(OPT_CIA_REVISION);

        os << tab("Model");
        os << (vic.pal() ? "PAL" : "NTSC") << std::endl;
        os << tab("VICII");
        os << VICIIRevisionEnum::key(vicRev) << std::endl;
        os << tab("SID");
        os << SIDRevisionEnum::key(sidRev) << std::endl;
        os << tab("CIA 1");
        os << CIARevisionEnum::key(cia1Rev) << std::endl;
        os << tab("CIA 2");
        os << CIARevisionEnum::key(cia2Rev) << std::endl;
    }

    if (category == Category::Checksums) {

        for (auto &c : subComponents) {

            os << tab(c->getDescription());
            os << hex(c->checksum()) << std::endl;
        }
    }

    if (category == Category::Current) {

        os << std::setfill('0') << std::uppercase << std::hex << std::left;
        os << " PC  SR AC XR YR SP  NV-BDIZC" << std::endl;
        os << std::setw(4) << isize(cpu.reg.pc0) << " ";
        os << std::setw(2) << isize(cpu.getP()) << " ";
        os << std::setw(2) << isize(cpu.reg.a) << " ";
        os << std::setw(2) << isize(cpu.reg.x) << " ";
        os << std::setw(2) << isize(cpu.reg.y) << " ";
        os << std::setw(2) << isize(cpu.reg.sp) << "  ";
        os << (cpu.getN() ? "1" : "0");
        os << (cpu.getV() ? "1" : "0");
        os << "1";
        os << (cpu.getB() ? "1" : "0");
        os << (cpu.getD() ? "1" : "0");
        os << (cpu.getI() ? "1" : "0");
        os << (cpu.getZ() ? "1" : "0");
        os << (cpu.getC() ? "1" : "0");
        os << std::endl;
    }

    if (category == Category::Sizeof) {

        os << tab("Emulator");
        os << dec(sizeof(Emulator)) << " Bytes" << std::endl;
        os << tab("C64");
        os << dec(sizeof(C64)) << " Bytes" << std::endl;
        os << tab("C64 Memory");
        os << dec(sizeof(C64Memory)) << " Bytes" << std::endl;
        os << tab("Drive Memory");
        os << dec(sizeof(DriveMemory)) << " Bytes" << std::endl;
        os << tab("CPU");
        os << dec(sizeof(CPU)) << " Bytes" << std::endl;
        os << tab("CIA");
        os << dec(sizeof(CIA)) << " Bytes" << std::endl;
        os << tab("VICII");
        os << dec(sizeof(VICII)) << " Bytes" << std::endl;
        os << tab("Muxer");
        os << dec(sizeof(Muxer)) << " Bytes" << std::endl;
        os << tab("Power supply");
        os << dec(sizeof(PowerSupply)) << " Bytes" << std::endl;
        os << tab("Control port");
        os << dec(sizeof(ControlPort)) << " Bytes" << std::endl;
        os << tab("Expansion port");
        os << dec(sizeof(ExpansionPort)) << " Bytes" << std::endl;
        os << tab("IEC");
        os << dec(sizeof(IEC)) << " Bytes" << std::endl;
        os << tab("Keyboard");
        os << dec(sizeof(Keyboard)) << " Bytes" << std::endl;
        os << tab("Drive");
        os << dec(sizeof(Drive)) << " Bytes" << std::endl;
        os << tab("Parallel Cable");
        os << dec(sizeof(ParCable)) << " Bytes" << std::endl;
        os << tab("Datasette");
        os << dec(sizeof(Datasette)) << " Bytes" << std::endl;
        os << tab("RetroShell");
        os << dec(sizeof(RetroShell)) << " Bytes" << std::endl;
        os << tab("Regression Tester");
        os << dec(sizeof(RegressionTester)) << " Bytes" << std::endl;
        os << tab("Recorder");
        os << dec(sizeof(Recorder)) << " Bytes" << std::endl;
        os << tab("MsgQueue");
        os << dec(sizeof(MsgQueue)) << " Bytes" << std::endl;
        os << tab("CmdQueue");
        os << dec(sizeof(CmdQueue)) << " Bytes" << std::endl;
    }
}

void
C64::record() const
{
    Inspectable<C64Info, Void>::record();

    for (EventSlot i = 0; i < SLOT_COUNT; i++) {
            inspectSlot(i);
    }
}

bool 
C64::autoInspect() const
{
    return getInspectionTarget() == INSPECTION_C64 && isRunning();
}

void
C64::recordState(C64Info &result) const
{
    SYNCHRONIZED

    result.cpuProgress = cpu.clock;
    result.cia1Progress = cia1.sleeping ? cia1.sleepCycle : cpu.clock;
    result.cia2Progress = cia2.sleeping ? cia2.sleepCycle : cpu.clock;
    result.frame = frame;
    result.vpos = scanline;
    result.hpos = rasterCycle;
}

EventSlotInfo
C64::getSlotInfo(isize nr) const
{
    assert_enum(EventSlot, nr);

    {   SYNCHRONIZED

        if (!autoInspect()) { inspectSlot(nr); }
        return slotInfo[nr];
    }
}

void
C64::inspectSlot(EventSlot nr) const
{
    assert_enum(EventSlot, nr);

    auto &info = slotInfo[nr];
    auto cycle = trigger[nr];

    info.slot = nr;
    info.eventId = id[nr];
    info.trigger = cycle;
    info.triggerRel = cycle - cpu.clock;

    // Compute clock at pos (0,0)
    auto clock00 = cpu.clock - vic.getCyclesPerLine() * scanline - rasterCycle;

    // Compute the number of elapsed cycles since then
    auto diff = cycle - clock00;

    // Split into frame / line / cycle
    info.frameRel = long(diff / vic.getCyclesPerFrame());
    diff = diff % vic.getCyclesPerFrame();
    info.vpos = long(diff / vic.getCyclesPerLine());
    info.hpos = long(diff % vic.getCyclesPerLine());

    info.eventName = eventName((EventSlot)nr, id[nr]);
}

/*
void
C64::stopAndGo()
{
    isRunning() ? emulator.pause() : emulator.run();
}
*/

void
C64::executeOneCycle()
{
    setFlag(RL::SINGLE_STEP);
    execute();
    clearFlag(RL::SINGLE_STEP);
}

void
C64::finishFrame()
{
    while (scanline != 0 || rasterCycle > 1) executeOneCycle();
}

void
C64::endScanline()
{
    cia1.tod.increment();
    cia2.tod.increment();

    vic.endScanline();
    rasterCycle = 1;
    scanline++;
    
    if (scanline >= vic.getLinesPerFrame()) {
        scanline = 0;
        endFrame();
    }
}

void
C64::endFrame()
{
    frame++;
    
    vic.endFrame();

    // Execute remaining SID cycles
    muxer.executeUntil(cpu.clock);
    
    // Execute other components
    iec.execute();
    expansionport.execute();
    port1.execute();
    port2.execute();
    drive8.vsyncHandler();
    drive9.vsyncHandler();
    recorder.vsyncHandler();
}

void
C64::process(const Cmd &cmd)
{
    switch (cmd.type) {

        case CMD_BRK:

            signalBrk();
            break;

        case CMD_SNAPSHOT_AUTO:

            autoSnapshot = new Snapshot(*this);
            msgQueue.put(MSG_AUTO_SNAPSHOT_TAKEN);
            break;

        case CMD_SNAPSHOT_USER:

            userSnapshot = new Snapshot(*this);
            msgQueue.put(MSG_USER_SNAPSHOT_TAKEN);
            break;

        case CMD_ALARM_ABS:

            setAlarmAbs(cmd.alarm.cycle, cmd.alarm.value);
            break;

        case CMD_ALARM_REL:

            setAlarmRel(cmd.alarm.cycle, cmd.alarm.value);
            break;
            
        default:
            fatalError;
    }
}

void
C64::processEvents(Cycle cycle)
{
    //
    // Check primary slots
    //

    if (isDue<SLOT_CIA1>(cycle)) {
        cia1.serviceEvent(id[SLOT_CIA1]);
    }
    if (isDue<SLOT_CIA2>(cycle)) {
        cia2.serviceEvent(id[SLOT_CIA2]);
    }

    if (isDue<SLOT_SEC>(cycle)) {

        //
        // Check secondary slots
        //

        if (isDue<SLOT_IEC>(cycle)) {
            iec.update();
        }

        if (isDue<SLOT_DAT>(cycle)) {
            datasette.processDatEvent(id[SLOT_DAT], data[SLOT_DAT]);
        }

        if (isDue<SLOT_TER>(cycle)) {

            //
            // Check tertiary slots
            //

            if (isDue<SLOT_AFI1>(cycle)) {
                port1.joystick.processEvent();
            }
            if (isDue<SLOT_AFI2>(cycle)) {
                port2.joystick.processEvent();
            }
            if (isDue<SLOT_MOT>(cycle)) {
                datasette.processMotEvent(id[SLOT_MOT]);
            }
            if (isDue<SLOT_DC8>(cycle)) {
                drive8.processDiskChangeEvent(id[SLOT_DC8]);
            }
            if (isDue<SLOT_DC9>(cycle)) {
                drive9.processDiskChangeEvent(id[SLOT_DC9]);
            }
            if (isDue<SLOT_RSH>(cycle)) {
                retroShell.serviceEvent();
            }
            if (isDue<SLOT_KEY>(cycle)) {
                keyboard.processKeyEvent(id[SLOT_KEY]);
            }
            if (isDue<SLOT_ALA>(cycle)) {
                processAlarmEvent();
            }
            if (isDue<SLOT_INS>(cycle)) {

                processINSEvent(id[SLOT_INS]);
            }

            // Determine the next trigger cycle for all tertiary slots
            Cycle next = trigger[SLOT_TER + 1];
            for (isize i = SLOT_TER + 2; i < SLOT_COUNT; i++) {
                if (trigger[i] < next) next = trigger[i];
            }
            rescheduleAbs<SLOT_TER>(next);
        }

        // Determine the next trigger cycle for all secondary slots
        Cycle next = trigger[SLOT_SEC + 1];
        for (isize i = SLOT_SEC + 2; i <= SLOT_TER; i++) {
            if (trigger[i] < next) next = trigger[i];
        }
        rescheduleAbs<SLOT_SEC>(next);
    }

    // Determine the next trigger cycle for all primary slots
    Cycle next = trigger[0];
    for (isize i = 1; i <= SLOT_SEC; i++) {
        if (trigger[i] < next) next = trigger[i];
    }
    nextTrigger = next;
}

void
C64::processINSEvent(EventID id)
{
    switch (id) {

        case INS_C64:       record(); break;
        case INS_CPU:       cpu.record(); break;
        case INS_MEM:       mem.record(); break;
        case INS_CIA:       cia1.record(); cia2.record(); break;
        case INS_VICII:     vic.record(); break;
        case INS_SID:       muxer.record(); break;
        // case INS_EVENTS:    c64.record(); break;

        default:
            fatalError;
    }

    // Reschedule event
    rescheduleRel<SLOT_INS>((Cycle)(inspectionInterval * PAL_CYCLES_PER_SECOND));
}

void
C64::setFlag(u32 flag)
{
    SYNCHRONIZED

    flags |= flag;
}

void
C64::clearFlag(u32 flag)
{
    SYNCHRONIZED

    flags &= ~flag;
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

void
C64::loadSnapshot(const Snapshot &snapshot)
{
    {   SUSPENDED

        try {

            // Restore the saved state
            load(snapshot.getData());

            // Clear the keyboard matrix to avoid constantly pressed keys
            keyboard.releaseAll();

            // Print some debug info if requested
            if (SNP_DEBUG) dump(Category::State);

        } catch (VC64Error &error) {

            /* If we reach this point, the emulator has been put into an
             * inconsistent state due to corrupted snapshot data. We cannot
             * continue emulation, because it would likely crash the
             * application. Because we cannot revert to the old state either,
             * we perform a hard reset to eliminate the inconsistency.
             */
            hardReset();
            throw error;
        }
    }

    // Inform the GUI
    msgQueue.put(MSG_SNAPSHOT_RESTORED);
}

RomInfo 
C64::getRomInfo(RomType type) const
{
    RomInfo result = {};

    auto id = romIdentifier(type);

    result.crc32 = romCRC32(type);

    result.title = romTitle(type);
    result.subtitle = romSubTitle(type);
    result.revision = romRevision(type);
    
    result.isCommodoreRom = RomFile::isCommodoreRom(id);
    result.isPatchedRom = RomFile::isPatchedRom(id);
    result.isMega65Rom = hasMega65Rom(type);

    return result;
}

u32
C64::romCRC32(RomType type) const
{
    if (!hasRom(type)) return 0;
    
    switch (type) {
            
        case ROM_TYPE_BASIC:  return util::crc32(mem.rom + 0xA000, 0x2000);
        case ROM_TYPE_CHAR:   return util::crc32(mem.rom + 0xD000, 0x1000);
        case ROM_TYPE_KERNAL: return util::crc32(mem.rom + 0xE000, 0x2000);
        case ROM_TYPE_VC1541: return drive8.mem.romCRC32();

        default:
            fatalError;
    }
}

u64
C64::romFNV64(RomType type) const
{
    if (!hasRom(type)) return 0;
    
    switch (type) {
            
        case ROM_TYPE_BASIC:  return util::fnv64(mem.rom + 0xA000, 0x2000);
        case ROM_TYPE_CHAR:   return util::fnv64(mem.rom + 0xD000, 0x1000);
        case ROM_TYPE_KERNAL: return util::fnv64(mem.rom + 0xE000, 0x2000);
        case ROM_TYPE_VC1541: return drive8.mem.romFNV64();

        default:
            fatalError;
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
    RomIdentifier rev = romIdentifier(type);
    
    switch (type) {
            
        case ROM_TYPE_BASIC:

            if (hasMega65Rom(ROM_TYPE_BASIC)) return "M.E.G.A. C64 OpenROM";
            return rev == ROM_UNKNOWN ? "Unknown Basic Rom" : RomFile::title(rev);

        case ROM_TYPE_CHAR:

            if (hasMega65Rom(ROM_TYPE_CHAR)) return "M.E.G.A. C64 OpenROM";
            return rev == ROM_UNKNOWN ? "Unknown Character Rom" : RomFile::title(rev);

        case ROM_TYPE_KERNAL:

            if (hasMega65Rom(ROM_TYPE_KERNAL)) return "M.E.G.A. C64 OpenROM";
            return rev == ROM_UNKNOWN ? "Unknown Kernal Rom" : RomFile::title(rev);

        case ROM_TYPE_VC1541:

            return rev == ROM_UNKNOWN ? "Unknown Drive Firmware" : RomFile::title(rev);

        default:
            fatalError;
    }
}

const char *
C64::romSubTitle(u64 fnv) const
{
    RomIdentifier rev = RomFile::identifier(fnv);
    
    if (rev != ROM_UNKNOWN) return RomFile::subTitle(rev);
    
    static char str[32];
    snprintf(str, sizeof(str), "FNV %llx", fnv);
    return str;
}

const char *
C64::romSubTitle(RomType type) const
{
    switch (type) {
            
        case ROM_TYPE_BASIC:

            if (hasMega65Rom(ROM_TYPE_BASIC)) return "Free Basic Replacement";
            return romSubTitle(romFNV64(ROM_TYPE_BASIC));

        case ROM_TYPE_CHAR:

            if (hasMega65Rom(ROM_TYPE_CHAR)) return "Free Charset Replacement";
            return romSubTitle(romFNV64(ROM_TYPE_CHAR));

        case ROM_TYPE_KERNAL:

            if (hasMega65Rom(ROM_TYPE_KERNAL)) return "Free Kernal Replacement";
            return romSubTitle(romFNV64(ROM_TYPE_KERNAL));

        case ROM_TYPE_VC1541:

            return romSubTitle(romFNV64(ROM_TYPE_VC1541));

        default:
            fatalError;
    }
}

const char *
C64::romRevision(RomType type) const
{
    switch (type) {

        case ROM_TYPE_BASIC:

            if (hasMega65Rom(ROM_TYPE_BASIC)) return mega65BasicRev();
            return RomFile::revision(romIdentifier(ROM_TYPE_BASIC));

        case ROM_TYPE_CHAR:

            return RomFile::revision(romIdentifier(ROM_TYPE_CHAR));

        case ROM_TYPE_KERNAL:

            if (hasMega65Rom(ROM_TYPE_KERNAL)) return mega65KernalRev();
            return RomFile::revision(romIdentifier(ROM_TYPE_KERNAL));

        case ROM_TYPE_VC1541:

            return RomFile::revision(romIdentifier(ROM_TYPE_VC1541));

        default:
            fatalError;
    }
}

bool
C64::hasRom(RomType type) const
{
    switch (type) {
            
        case ROM_TYPE_BASIC:

            return (mem.rom[0xA000] | mem.rom[0xA001]) != 0x00;

        case ROM_TYPE_CHAR:

            return (mem.rom[0xD000] | mem.rom[0xD001]) != 0x00;

        case ROM_TYPE_KERNAL:

            return (mem.rom[0xE000] | mem.rom[0xE001]) != 0x00;

        case ROM_TYPE_VC1541:

            assert(drive8.mem.hasRom() == drive9.mem.hasRom());
            return drive8.mem.hasRom();

        default:
            fatalError;
    }
}

bool
C64::hasMega65Rom(RomType type) const
{
    RomIdentifier id;
    
    switch (type) {
            
        case ROM_TYPE_BASIC:

            return mem.rom[0xBF52] == 'O' && mem.rom[0xBF53] == 'R';

        case ROM_TYPE_CHAR:

            id = romIdentifier(ROM_TYPE_CHAR);
            return id == CHAR_MEGA65 || id == CHAR_PXLFONT_V23;

        case ROM_TYPE_KERNAL:

            return mem.rom[0xE4B9] == 'O' && mem.rom[0xE4BA] == 'R';

        case ROM_TYPE_VC1541:

            return false;

        default:
            fatalError;
    }
}

const char *
C64::mega65BasicRev() const
{
    static char rev[17];
    rev[0] = 0;
    
    if (hasMega65Rom(ROM_TYPE_BASIC)) std::memcpy(rev, &mem.rom[0xBF55], 16);
    rev[16] = 0;
    
    return rev;
}

const char *
C64::mega65KernalRev() const
{
    static char rev[17];
    rev[0] = 0;
    
    if (hasMega65Rom(ROM_TYPE_KERNAL)) std::memcpy(rev, &mem.rom[0xE4BC], 16);
    rev[16] = 0;
    
    return rev;
}

void
C64::loadRom(const string &path)
{
    RomFile file(path);
    loadRom(file);
}

void
C64::loadRom(const RomFile &file)
{
    switch (file.type()) {
            
        case FILETYPE_BASIC_ROM:
            
            file.flash(mem.rom, 0xA000);
            debug(MEM_DEBUG, "Basic Rom flashed\n");
            debug(MEM_DEBUG, "hasMega65Rom() = %d\n", hasMega65Rom(ROM_TYPE_BASIC));
            debug(MEM_DEBUG, "mega65BasicRev() = %s\n", mega65BasicRev());
            break;
            
        case FILETYPE_CHAR_ROM:
            
            file.flash(mem.rom, 0xD000);
            debug(MEM_DEBUG, "Character Rom flashed\n");
            break;
            
        case FILETYPE_KERNAL_ROM:
            
            file.flash(mem.rom, 0xE000);
            debug(MEM_DEBUG, "Kernal Rom flashed\n");
            debug(MEM_DEBUG, "hasMega65Rom() = %d\n", hasMega65Rom(ROM_TYPE_KERNAL));
            debug(MEM_DEBUG, "mega65KernalRev() = %s\n", mega65KernalRev());
            break;
            
        case FILETYPE_VC1541_ROM:
            
            drive8.mem.loadRom(file.data, file.size);
            drive9.mem.loadRom(file.data, file.size);
            debug(MEM_DEBUG, "VC1541 Rom flashed\n");
            break;
            
        default:
            fatalError;
    }
}

void
C64::deleteRom(RomType type)
{
    switch (type) {
            
        case ROM_TYPE_BASIC:

            memset(mem.rom + 0xA000, 0, 0x2000);
            break;

        case ROM_TYPE_CHAR:

            memset(mem.rom + 0xD000, 0, 0x1000);
            break;

        case ROM_TYPE_KERNAL:

            memset(mem.rom + 0xE000, 0, 0x2000);
            break;

        case ROM_TYPE_VC1541:

            drive8.mem.deleteRom();
            drive9.mem.deleteRom();
            break;

        default:
            fatalError;
    }
}

void
C64::saveRom(RomType type, const string &path)
{
    switch (type) {
            
        case ROM_TYPE_BASIC:

            if (hasRom(ROM_TYPE_BASIC)) {
                RomFile file(mem.rom + 0xA000, 0x2000);
                file.writeToFile(path);
            }
            break;

        case ROM_TYPE_CHAR:

            if (hasRom(ROM_TYPE_CHAR)) {
                RomFile file(mem.rom + 0xD000, 0x1000);
                file.writeToFile(path);
            }
            break;

        case ROM_TYPE_KERNAL:

            if (hasRom(ROM_TYPE_KERNAL)) {
                RomFile file(mem.rom + 0xE000, 0x2000);
                file.writeToFile(path);
            }
            break;

        case ROM_TYPE_VC1541:

            if (hasRom(ROM_TYPE_VC1541)) {
                drive8.mem.saveRom(path);
            }
            break;
            
        default:
            fatalError;
    }
}

void
C64::flash(const AnyFile &file)
{
    {   SUSPENDED
        
        switch (file.type()) {
                
            case FILETYPE_BASIC_ROM:
                file.flash(mem.rom, 0xA000);
                break;
                
            case FILETYPE_CHAR_ROM:
                file.flash(mem.rom, 0xD000);
                break;
                
            case FILETYPE_KERNAL_ROM:
                file.flash(mem.rom, 0xE000);
                break;
                
            case FILETYPE_VC1541_ROM:
                drive8.mem.loadRom(dynamic_cast<const RomFile &>(file));
                drive9.mem.loadRom(dynamic_cast<const RomFile &>(file));
                break;

            case FILETYPE_SNAPSHOT:
                loadSnapshot(dynamic_cast<const Snapshot &>(file));
                break;
                
            default:
                fatalError;
        }
    }
}

void
C64::flash(const AnyCollection &file, isize nr)
{
    auto addr = (u16)file.itemLoadAddr(nr);
    auto size = file.itemSize(nr);
    if (size <= 2) return;
    
    {   SUSPENDED
        
        switch (file.type()) {
                
            case FILETYPE_D64:
            case FILETYPE_T64:
            case FILETYPE_P00:
            case FILETYPE_PRG:
            case FILETYPE_FOLDER:

                // Flash data into memory
                size = std::min(size - 2, isize(0x10000 - addr));
                file.copyItem(nr, mem.ram + addr, size, 2);

                // Rectify zero page
                mem.ram[0x2D] = LO_BYTE(addr + size);   // VARTAB (lo byte)
                mem.ram[0x2E] = HI_BYTE(addr + size);   // VARTAB (high byte)
                break;
                
            default:
                fatalError;
        }
    }
    
    msgQueue.put(MSG_FILE_FLASHED);
}

void
C64::flash(const FileSystem &fs, isize nr)
{
    u16 addr = fs.loadAddr(nr);
    u64 size = fs.fileSize(nr);
    
    if (size <= 2) {
        return;
    }
    
    {   SUSPENDED

        // Flash data into memory
        size = std::min(size - 2, (u64)(0x10000 - addr));
        fs.copyFile(nr, mem.ram + addr, size, 2);

        // Rectify zero page
        mem.ram[0x2D] = LO_BYTE(addr + size);   // VARTAB (lo byte)
        mem.ram[0x2E] = HI_BYTE(addr + size);   // VARTAB (high byte)
    }
    
    msgQueue.put(MSG_FILE_FLASHED);
}

void
C64::setAlarmAbs(Cycle trigger, i64 payload)
{
    {   SUSPENDED

        alarms.push_back(Alarm { trigger, payload });
        scheduleNextAlarm();
    }
}

void
C64::setAlarmRel(Cycle trigger, i64 payload)
{
    {   SUSPENDED

        alarms.push_back(Alarm { cpu.clock + trigger, payload });
        scheduleNextAlarm();
    }
}

void
C64::processAlarmEvent()
{
    for (auto it = alarms.begin(); it != alarms.end(); ) {

        if (it->trigger <= cpu.clock) {
            msgQueue.put(MSG_ALARM, it->payload);
            it = alarms.erase(it);
        } else {
            it++;
        }
    }
    scheduleNextAlarm();
}

void
C64::scheduleNextAlarm()
{
    Cycle trigger = INT64_MAX;

    cancel<SLOT_ALA>();

    for(Alarm alarm : alarms) {

        if (alarm.trigger < trigger) {
            scheduleAbs<SLOT_ALA>(alarm.trigger, ALA_TRIGGER);
            trigger = alarm.trigger;
        }
    }
}

bool
C64::getDebugVariable(DebugFlag flag)
{
#ifdef RELEASEBUILD

    throw VC64Error(ERROR_OPT_UNSUPPORTED, "Debug variables are only accessible in debug builds.");

#else

    switch (flag) {

        case FLAG_XFILES:           return XFILES;
        case FLAG_CNF_DEBUG:        return CNF_DEBUG;
        case FLAG_DEF_DEBUG:        return DEF_DEBUG;

        case FLAG_RUN_DEBUG:        return RUN_DEBUG;
        case FLAG_TIM_DEBUG:        return TIM_DEBUG;
        case FLAG_WARP_DEBUG:       return WARP_DEBUG;
        case FLAG_CMD_DEBUG:        return CMD_DEBUG;
        case FLAG_MSG_DEBUG:        return MSG_DEBUG;
        case FLAG_SNP_DEBUG:        return SNP_DEBUG;

        case FLAG_RUA_DEBUG:        return RUA_DEBUG;
        case FLAG_RUA_ON_STEROIDS:  return RUA_ON_STEROIDS;

        case FLAG_CPU_DEBUG:        return CPU_DEBUG;
        case FLAG_IRQ_DEBUG:        return IRQ_DEBUG;

        case FLAG_MEM_DEBUG:        return MEM_DEBUG;

        case FLAG_CIA_DEBUG:        return CIA_DEBUG;
        case FLAG_CIAREG_DEBUG:     return CIAREG_DEBUG;
        case FLAG_CIA_ON_STEROIDS:  return CIA_ON_STEROIDS;

        case FLAG_VICII_DEBUG:      return VICII_DEBUG;
        case FLAG_VICII_REG_DEBUG:  return VICII_REG_DEBUG;
        case FLAG_VICII_SAFE_MODE:  return VICII_SAFE_MODE;
        case FLAG_VICII_STATS:      return VICII_STATS;
        case FLAG_RASTERIRQ_DEBUG:  return RASTERIRQ_DEBUG;

        case FLAG_SID_DEBUG:        return SID_DEBUG;
        case FLAG_SID_EXEC:         return SID_EXEC;
        case FLAG_SIDREG_DEBUG:     return SIDREG_DEBUG;
        case FLAG_AUDBUF_DEBUG:     return AUDBUF_DEBUG;

        case FLAG_VIA_DEBUG:        return VIA_DEBUG;
        case FLAG_PIA_DEBUG:        return PIA_DEBUG;
        case FLAG_IEC_DEBUG:        return IEC_DEBUG;
        case FLAG_DSK_DEBUG:        return DSK_DEBUG;
        case FLAG_DSKCHG_DEBUG:     return DSKCHG_DEBUG;
        case FLAG_GCR_DEBUG:        return GCR_DEBUG;
        case FLAG_FS_DEBUG:         return FS_DEBUG;
        case FLAG_PAR_DEBUG:        return PAR_DEBUG;

        case FLAG_CRT_DEBUG:        return CRT_DEBUG;
        case FLAG_FILE_DEBUG:       return FILE_DEBUG;

        case FLAG_JOY_DEBUG:        return JOY_DEBUG;
        case FLAG_DRV_DEBUG:        return DRV_DEBUG;
        case FLAG_TAP_DEBUG:        return TAP_DEBUG;
        case FLAG_KBD_DEBUG:        return KBD_DEBUG;
        case FLAG_PRT_DEBUG:        return PRT_DEBUG;
        case FLAG_EXP_DEBUG:        return EXP_DEBUG;
        case FLAG_LIP_DEBUG:        return LIP_DEBUG;

        case FLAG_REC_DEBUG:        return REC_DEBUG;
        case FLAG_REU_DEBUG:        return REU_DEBUG;

        case FLAG_FORCE_ROM_MISSING:        return FORCE_ROM_MISSING;
        case FLAG_FORCE_MEGA64_MISMATCH:    return FORCE_MEGA64_MISMATCH;
        case FLAG_FORCE_SNAP_TOO_OLD:       return FORCE_SNAP_TOO_OLD;
        case FLAG_FORCE_SNAP_TOO_NEW:       return FORCE_SNAP_TOO_NEW;
        case FLAG_FORCE_SNAP_IS_BETA:       return FORCE_SNAP_IS_BETA;
        case FLAG_FORCE_SNAP_CORRUPTED:     return FORCE_SNAP_CORRUPTED;
        case FLAG_FORCE_CRT_UNKNOWN:        return FORCE_CRT_UNKNOWN;
        case FLAG_FORCE_CRT_UNSUPPORTED:    return FORCE_CRT_UNSUPPORTED;
        case FLAG_FORCE_RECORDING_ERROR:    return FORCE_RECORDING_ERROR;
        case FLAG_FORCE_NO_FFMPEG:          return FORCE_NO_FFMPEG;

        default:
            throw VC64Error(ERROR_OPT_UNSUPPORTED, 
                            "Unhandled debug variable: " + string(DebugFlagEnum::key(flag)));
    }

#endif
}

void
C64::setDebugVariable(DebugFlag flag, bool val)
{
#ifdef RELEASEBUILD

    throw VC64Error(ERROR_OPT_UNSUPPORTED, "Debug variables are only accessible in debug builds.");

#else

    switch (flag) {

        case FLAG_XFILES:           XFILES          = val; break;
        case FLAG_CNF_DEBUG:        CNF_DEBUG       = val; break;
        case FLAG_DEF_DEBUG:        DEF_DEBUG       = val; break;

        case FLAG_RUN_DEBUG:        RUN_DEBUG       = val; break;
        case FLAG_TIM_DEBUG:        TIM_DEBUG       = val; break;
        case FLAG_WARP_DEBUG:       WARP_DEBUG      = val; break;
        case FLAG_CMD_DEBUG:        CMD_DEBUG       = val; break;
        case FLAG_MSG_DEBUG:        MSG_DEBUG       = val; break;
        case FLAG_SNP_DEBUG:        SNP_DEBUG       = val; break;

        case FLAG_RUA_DEBUG:        RUA_DEBUG       = val; break;
        case FLAG_RUA_ON_STEROIDS:  RUA_ON_STEROIDS = val; break;

        case FLAG_CPU_DEBUG:        CPU_DEBUG       = val; break;
        case FLAG_IRQ_DEBUG:        IRQ_DEBUG       = val; break;

        case FLAG_MEM_DEBUG:        MEM_DEBUG       = val; break;

        case FLAG_CIA_DEBUG:        CIA_DEBUG       = val; break;
        case FLAG_CIAREG_DEBUG:     CIAREG_DEBUG    = val; break;
        case FLAG_CIA_ON_STEROIDS:  CIA_ON_STEROIDS = val; break;

        case FLAG_VICII_DEBUG:      VICII_DEBUG     = val; break;
        case FLAG_VICII_REG_DEBUG:  VICII_REG_DEBUG = val; break;
        case FLAG_VICII_SAFE_MODE:  VICII_SAFE_MODE = val; break;
        case FLAG_VICII_STATS:      VICII_STATS     = val; break;
        case FLAG_RASTERIRQ_DEBUG:  RASTERIRQ_DEBUG = val; break;

        case FLAG_SID_DEBUG:        SID_DEBUG       = val; break;
        case FLAG_SID_EXEC:         SID_EXEC        = val; break;
        case FLAG_SIDREG_DEBUG:     SIDREG_DEBUG    = val; break;
        case FLAG_AUDBUF_DEBUG:     AUDBUF_DEBUG    = val; break;

        case FLAG_VIA_DEBUG:        VIA_DEBUG       = val; break;
        case FLAG_PIA_DEBUG:        PIA_DEBUG       = val; break;
        case FLAG_IEC_DEBUG:        IEC_DEBUG       = val; break;
        case FLAG_DSK_DEBUG:        DSK_DEBUG       = val; break;
        case FLAG_DSKCHG_DEBUG:     DSKCHG_DEBUG    = val; break;
        case FLAG_GCR_DEBUG:        GCR_DEBUG       = val; break;
        case FLAG_FS_DEBUG:         FS_DEBUG        = val; break;
        case FLAG_PAR_DEBUG:        PAR_DEBUG       = val; break;

        case FLAG_CRT_DEBUG:        CRT_DEBUG       = val; break;
        case FLAG_FILE_DEBUG:       FILE_DEBUG      = val; break;

        case FLAG_JOY_DEBUG:        JOY_DEBUG       = val; break;
        case FLAG_DRV_DEBUG:        DRV_DEBUG       = val; break;
        case FLAG_TAP_DEBUG:        TAP_DEBUG       = val; break;
        case FLAG_KBD_DEBUG:        KBD_DEBUG       = val; break;
        case FLAG_PRT_DEBUG:        PRT_DEBUG       = val; break;
        case FLAG_EXP_DEBUG:        EXP_DEBUG       = val; break;
        case FLAG_LIP_DEBUG:        LIP_DEBUG       = val; break;

        case FLAG_REC_DEBUG:        REC_DEBUG       = val; break;
        case FLAG_REU_DEBUG:        REU_DEBUG       = val; break;

        case FLAG_FORCE_ROM_MISSING:        FORCE_ROM_MISSING = val; break;
        case FLAG_FORCE_MEGA64_MISMATCH:    FORCE_MEGA64_MISMATCH = val; break;
        case FLAG_FORCE_SNAP_TOO_OLD:       FORCE_SNAP_TOO_OLD = val; break;
        case FLAG_FORCE_SNAP_TOO_NEW:       FORCE_SNAP_TOO_NEW = val; break;
        case FLAG_FORCE_SNAP_IS_BETA:       FORCE_SNAP_IS_BETA = val; break;
        case FLAG_FORCE_SNAP_CORRUPTED:     FORCE_SNAP_CORRUPTED = val; break;
        case FLAG_FORCE_CRT_UNKNOWN:        FORCE_CRT_UNKNOWN = val; break;
        case FLAG_FORCE_CRT_UNSUPPORTED:    FORCE_CRT_UNSUPPORTED = val; break;
        case FLAG_FORCE_RECORDING_ERROR:    FORCE_RECORDING_ERROR = val; break;
        case FLAG_FORCE_NO_FFMPEG:          FORCE_NO_FFMPEG = val; break;

        default:
            throw VC64Error(ERROR_OPT_UNSUPPORTED,
                            "Unhandled debug variable: " + string(DebugFlagEnum::key(flag)));
    }
#endif
}

}
