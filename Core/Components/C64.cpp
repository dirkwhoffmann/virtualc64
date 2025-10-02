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
#include "RomDatabase.h"
#include "OpenRoms.h"
#include <algorithm>
#include <queue>

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

        case SLOT_SER:

            switch (id) {

                case EVENT_NONE:    return "none";
                case SER_UPDATE:    return "SER_UPDATE";
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

        case SLOT_EXP:

            switch (id) {

                case EVENT_NONE:        return "none";
                case EXP_REU_INITIATE:  return "EXP_REU_INITIATE";
                case EXP_REU_PREPARE:   return "EXP_REU_PREPARE";
                case EXP_REU_STASH:     return "EXP_REU_STASH";
                case EXP_REU_FETCH:     return "EXP_REU_FETCH";
                case EXP_REU_SWAP:      return "EXP_REU_SWAP";
                case EXP_REU_VERIFY:    return "EXP_REU_VERIFY";
                case EXP_REU_AUTOLOAD:  return "EXP_REU_AUTOLOAD";
                case EXP_REU_FINALIZE:  return "EXP_REU_FINALIZE";
                default:                return "*** INVALID ***";
            }
            break;

        case SLOT_TXD:

            switch (id) {

                case EVENT_NONE:    return "none";
                case TXD_BIT:       return "TXD_BIT";
                default:            return "*** INVALID ***";
            }
            break;

        case SLOT_RXD:

            switch (id) {

                case EVENT_NONE:    return "none";
                case RXD_BIT:       return "RXD_BIT";
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

        case SLOT_SNP:

            switch (id) {

                case EVENT_NONE:        return "none";
                case SNP_TAKE:          return "SNP_TAKE";
                default:                return "*** INVALID ***";
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

        case SLOT_SRV:

            switch (id) {

                case EVENT_NONE:        return "none";
                case SRV_LAUNCH_DAEMON: return "SRV_LAUNCH_DAEMON";
                default:                return "*** INVALID ***";
            }
            break;

        case SLOT_DBG:

            switch (id) {

                case EVENT_NONE:    return "none";
                case DBG_WATCHDOG:  return "DBG_WATCHDOG";
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
                case INS_INSPECT:   return "INSPECT";
                default:            return "*** INVALID ***";
            }
            break;

        default:
            fatalError;
    }
}

void
C64::prefix(isize level, const char *component, isize line) const
{
    if (level) {

        if (objid == 1) fprintf(stderr, "[Run-ahead] ");

        if (level >= 3) {

            fprintf(stderr, "[%lld] (%3d,%3d) ", frame, scanline, rasterCycle);
        }
        if (level >= 4) {

            fprintf(stderr, "%04X ", cpu.getPC0());
        }
        if (level >= 5) {

            fprintf(stderr, "<%s%s> ", (cpu.irqLine ? "I" : "i"), (cpu.nmiLine ? "N" : "n"));
        }
        if (level >= 2) {

            fprintf(stderr, "%s:%ld ", component, line);
        }
    }
}

void 
C64::_didReset(bool hard)
{
    /* At this point, all components have executed their reset procedure. In
     * the final step, we need to perform some post-reset actions we could not
     * have performed before since they depend on the state of two
     * interconnected components. In particular, we need to
     *
     *   (1) update the bank map, as it depends on the expansion port lines.
     *   (2) rectify the program counter, as it depends on the memory contents,
     */

    // (1)
    expansionport.resetCartConfig();

    // (2)
    cpu.reg.pc = cpu.reg.pc0 = mem.resetVector();

    // Inform the GUI
    msgQueue.put(Msg::RESET);
}

void
C64::initialize()
{
    auto load = [&](const fs::path &path) {

        msg("Trying to load Rom from %s...\n", path.string().c_str());

        try { loadRom(path); } catch (std::exception& e) {
            warn("Error: %s\n", e.what());
        }
    };

    if (auto path = Emulator::defaults.getRaw("BASIC_PATH");  path != "") load(path);
    if (auto path = Emulator::defaults.getRaw("CHAR_PATH");   path != "") load(path);
    if (auto path = Emulator::defaults.getRaw("KERNAL_PATH"); path != "") load(path);
    if (auto path = Emulator::defaults.getRaw("VC1541_PATH"); path != "") load(path);

    CoreComponent::initialize();
}

void
C64::operator << (SerResetter &worker)
{
    if (isSoftResetter(worker)) return;

    // Reset all items
    serialize(worker);

    // Initialize all event slots
    for (isize i = 0; i < SLOT_COUNT; i++) {

        if (i == SLOT_INS) continue;

        trigger[i] = NEVER;
        eventid[i] = (EventID)0;
        data[i] = 0;
    }

    // Schedule initial events
    scheduleAbs<SLOT_CIA1>(cpu.clock, CIA_EXECUTE);
    scheduleAbs<SLOT_CIA2>(cpu.clock, CIA_EXECUTE);
    scheduleRel<SLOT_SRV>(C64::sec(0.5), SRV_LAUNCH_DAEMON);
    scheduleNextSNPEvent();

    flags = 0;
    rasterCycle = 1;
}

double
C64::nativeRefreshRate() const
{
    return vic.getFps();
}

i64
C64::nativeClockFrequency() const
{
    return vic.getFrequency();
}

double
C64::refreshRate() const
{
    if (config.vsync) {

        return double(host.getOption(Opt::HOST_REFRESH_RATE));

    } else {

        auto boost = config.speedBoost ? config.speedBoost : 100;
        return nativeRefreshRate() * boost / 100.0;
    }
}

i64
C64::clockFrequency() const
{
    auto boost = config.speedBoost ? config.speedBoost : 100;
    return nativeClockFrequency() * boost / 100;
}

void
C64::updateClockFrequency()
{
    durationOfOneCycle = 10000000000 / nativeClockFrequency();

    auto frequency = clockFrequency();
    sidBridge.sid[0].setClockFrequency((u32)frequency);
    sidBridge.sid[1].setClockFrequency((u32)frequency);
    sidBridge.sid[2].setClockFrequency((u32)frequency);
    sidBridge.sid[3].setClockFrequency((u32)frequency);
}

void
C64::exportConfig(const fs::path &path) const
{
    auto fs = std::ofstream(path, std::ofstream::binary);

    if (!fs.is_open()) {
        throw AppError(Fault::FILE_CANT_WRITE);
    }

    exportConfig(fs);
}

void
C64::exportConfig(std::ostream &stream) const
{
    stream << "# VirtualC64 " << C64::build() << "\n";
    stream << "\n";
    stream << "c64 power off\n";
    stream << "\n";
    CoreComponent::exportConfig(stream);
    stream << "c64 power on\n";
}

i64
C64::get(Opt opt, isize objid) const
{
    debug(CNF_DEBUG, "get(%s, %ld)\n", OptEnum::key(opt), objid);

    auto target = routeOption(opt, objid);
    if (target == nullptr) throw AppError(Fault::OPT_INV_ID);
    return target->getOption(opt);
}

void
C64::check(Opt opt, i64 value, const std::vector<isize> objids)
{
    value = overrideOption(opt, value);

    if (objids.empty()) {

        for (isize objid = 0;; objid++) {

            auto target = routeOption(opt, objid);
            if (target == nullptr) break;

            debug(CNF_DEBUG, "check(%s, %lld, %ld)\n", OptEnum::key(opt), value, objid);
            target->checkOption(opt, value);
        }
    }
    for (auto &objid : objids) {

        debug(CNF_DEBUG, "check(%s, %lld, %ld)\n", OptEnum::key(opt), value, objid);

        auto target = routeOption(opt, objid);
        if (target == nullptr) throw AppError(Fault::OPT_INV_ID);

        target->checkOption(opt, value);
    }
}

void
C64::set(Opt opt, i64 value, const std::vector<isize> objids)
{
    if (!isInitialized()) initialize();

    value = overrideOption(opt, value);

    if (objids.empty()) {

        for (isize objid = 0;; objid++) {

            auto target = routeOption(opt, objid);
            if (target == nullptr) break;

            debug(CNF_DEBUG, "set(%s, %lld, %ld)\n", OptEnum::key(opt), value, objid);
            target->setOption(opt, value);
        }
    }
    for (auto &objid : objids) {

        debug(CNF_DEBUG, "set(%s, %lld, %ld)\n", OptEnum::key(opt), value, objid);

        auto target = routeOption(opt, objid);
        if (target == nullptr) throw AppError(Fault::OPT_INV_ID);

        target->setOption(opt, value);
    }
}

void
C64::set(Opt opt, const string &value, const std::vector<isize> objids)
{
    set(opt, OptionParser::parse(opt, value), objids);
}

void
C64::set(const string &opt, const string &value, const std::vector<isize> objids)
{
    set(Opt(util::parseEnum<OptEnum>(opt)), value, objids);
}

void
C64::set(ConfigScheme scheme)
{
    assert_enum(ConfigScheme, scheme);
    
    emulator.revertToDefaultConfig();
    
    switch(scheme) {
            
        case ConfigScheme::PAL:
            
            set(Opt::VICII_REVISION,        (i64)VICIIRev::PAL_6569_R3);
            set(Opt::VICII_GRAY_DOT_BUG,    false);
            set(Opt::CIA_REVISION,          (i64)CIARev::MOS_6526);
            set(Opt::CIA_TIMER_B_BUG,       true);
            set(Opt::SID_REV,               (i64)SIDRevision::MOS_6581);
            set(Opt::SID_FILTER,            true);
            set(Opt::POWER_GRID,            (i64)PowerGrid::STABLE_50HZ);
            set(Opt::GLUE_LOGIC,            (i64)GlueLogic::DISCRETE);
            set(Opt::MEM_INIT_PATTERN,      (i64)RamPattern::VICE);
            break;
            
        case ConfigScheme::PAL_II:
            
            set(Opt::VICII_REVISION,        (i64)VICIIRev::PAL_8565);
            set(Opt::VICII_GRAY_DOT_BUG,    true);
            set(Opt::CIA_REVISION,          (i64)CIARev::MOS_8521);
            set(Opt::CIA_TIMER_B_BUG,       false);
            set(Opt::SID_REV,               (i64)SIDRevision::MOS_8580);
            set(Opt::SID_FILTER,            true);
            set(Opt::POWER_GRID,            (i64)PowerGrid::STABLE_50HZ);
            set(Opt::GLUE_LOGIC,            (i64)GlueLogic::IC);
            set(Opt::MEM_INIT_PATTERN,      (i64)RamPattern::VICE);
            break;
            
        case ConfigScheme::PAL_OLD:
            
            set(Opt::VICII_REVISION,        (i64)VICIIRev::PAL_6569_R1);
            set(Opt::VICII_GRAY_DOT_BUG,    false);
            set(Opt::CIA_REVISION,          (i64)CIARev::MOS_6526);
            set(Opt::CIA_TIMER_B_BUG,       true);
            set(Opt::SID_REV,               (i64)SIDRevision::MOS_6581);
            set(Opt::SID_FILTER,            true);
            set(Opt::POWER_GRID,            (i64)PowerGrid::STABLE_50HZ);
            set(Opt::GLUE_LOGIC,            (i64)GlueLogic::DISCRETE);
            set(Opt::MEM_INIT_PATTERN,      (i64)RamPattern::VICE);
            break;
            
        case ConfigScheme::NTSC:
            
            set(Opt::VICII_REVISION,        (i64)VICIIRev::NTSC_6567);
            set(Opt::VICII_GRAY_DOT_BUG,    false);
            set(Opt::CIA_REVISION,          (i64)CIARev::MOS_6526);
            set(Opt::CIA_TIMER_B_BUG,       false);
            set(Opt::SID_REV,               (i64)SIDRevision::MOS_6581);
            set(Opt::SID_FILTER,            true);
            set(Opt::POWER_GRID,            (i64)PowerGrid::STABLE_60HZ);
            set(Opt::GLUE_LOGIC,            (i64)GlueLogic::DISCRETE);
            set(Opt::MEM_INIT_PATTERN,      (i64)RamPattern::VICE);
            break;
            
        case ConfigScheme::NTSC_II:
            
            set(Opt::VICII_REVISION,        (i64)VICIIRev::NTSC_8562);
            set(Opt::VICII_GRAY_DOT_BUG,    true);
            set(Opt::CIA_REVISION,          (i64)CIARev::MOS_8521);
            set(Opt::CIA_TIMER_B_BUG,       true);
            set(Opt::SID_REV,               (i64)SIDRevision::MOS_8580);
            set(Opt::SID_FILTER,            true);
            set(Opt::POWER_GRID,            (i64)PowerGrid::STABLE_60HZ);
            set(Opt::GLUE_LOGIC,            (i64)GlueLogic::IC);
            set(Opt::MEM_INIT_PATTERN,      (i64)RamPattern::VICE);
            break;
            
        case ConfigScheme::NTSC_OLD:
            
            set(Opt::VICII_REVISION,        (i64)VICIIRev::NTSC_6567_R56A);
            set(Opt::VICII_GRAY_DOT_BUG,    false);
            set(Opt::CIA_REVISION,          (i64)CIARev::MOS_6526);
            set(Opt::CIA_TIMER_B_BUG,       false);
            set(Opt::SID_REV,               (i64)SIDRevision::MOS_6581);
            set(Opt::SID_FILTER,            true);
            set(Opt::POWER_GRID,            (i64)PowerGrid::STABLE_60HZ);
            set(Opt::GLUE_LOGIC,            (i64)GlueLogic::DISCRETE);
            set(Opt::MEM_INIT_PATTERN,      (i64)RamPattern::VICE);
            break;
            
        default:
            fatalError;
    }
}

Configurable *
C64::routeOption(Opt opt, isize objid)
{
    return CoreComponent::routeOption(opt, objid);
}

const Configurable *
C64::routeOption(Opt opt, isize objid) const
{
    auto result = const_cast<C64 *>(this)->routeOption(opt, objid);
    return const_cast<const Configurable *>(result);
}

i64
C64::overrideOption(Opt opt, i64 value) const
{
    static std::map<Opt,i64> overrides = OVERRIDES;

    if (overrides.find(opt) != overrides.end()) {

        msg("Overriding option: %s = %lld\n", OptEnum::key(opt), value);
        return overrides[opt];
    }

    return value;
}

void
C64::update(CmdQueue &queue)
{
    Command cmd;
    bool cmdConfig = false;

    auto drive = [&]() -> Drive& { return cmd.value == 0 ? drive8 : drive9; };

    while (queue.poll(cmd)) {

        debug(CMD_DEBUG, "Command: %s\n", CmdEnum::key(cmd.type));

        switch (cmd.type) {

            case Cmd::CONFIG:

                cmdConfig = true;
                emulator.set(cmd.config.option, cmd.config.value, { cmd.config.id });
                break;

            case Cmd::CONFIG_ALL:

                cmdConfig = true;
                emulator.set(cmd.config.option, cmd.config.value, { });
                break;

            case Cmd::CONFIG_SCHEME:

                cmdConfig = true;
                set(ConfigScheme(cmd.value));
                break;

            case Cmd::HARD_RESET:
            case Cmd::SOFT_RESET:
            case Cmd::POWER_ON:
            case Cmd::POWER_OFF:
            case Cmd::RUN:
            case Cmd::PAUSE:
            case Cmd::WARP_ON:
            case Cmd::WARP_OFF:
            case Cmd::HALT:
            case Cmd::ALARM_ABS:
            case Cmd::ALARM_REL:
            case Cmd::INSPECTION_TARGET:

                processCommand(cmd);
                break;

            case Cmd::CPU_BRK:
            case Cmd::CPU_NMI:
            case Cmd::BP_SET_AT:
            case Cmd::BP_MOVE_TO:
            case Cmd::BP_REMOVE_NR:
            case Cmd::BP_REMOVE_AT:
            case Cmd::BP_REMOVE_ALL:
            case Cmd::BP_ENABLE_NR:
            case Cmd::BP_ENABLE_AT:
            case Cmd::BP_ENABLE_ALL:
            case Cmd::BP_DISABLE_NR:
            case Cmd::BP_DISABLE_AT:
            case Cmd::BP_DISABLE_ALL:
            case Cmd::WP_SET_AT:
            case Cmd::WP_MOVE_TO:
            case Cmd::WP_REMOVE_NR:
            case Cmd::WP_REMOVE_AT:
            case Cmd::WP_REMOVE_ALL:
            case Cmd::WP_ENABLE_NR:
            case Cmd::WP_ENABLE_AT:
            case Cmd::WP_ENABLE_ALL:
            case Cmd::WP_DISABLE_NR:
            case Cmd::WP_DISABLE_AT:
            case Cmd::WP_DISABLE_ALL:

                cpu.processCommand(cmd);
                break;

            case Cmd::KEY_PRESS:
            case Cmd::KEY_RELEASE:
            case Cmd::KEY_RELEASE_ALL:
            case Cmd::KEY_TOGGLE:

                keyboard.processCommand(cmd);
                break;

            case Cmd::DSK_TOGGLE_WP:
            case Cmd::DSK_MODIFIED:
            case Cmd::DSK_UNMODIFIED:

                drive().processCommand(cmd);
                break;

            case Cmd::MOUSE_MOVE_ABS:
            case Cmd::MOUSE_MOVE_REL:

                switch (cmd.coord.port) {

                    case PORT_1: port1.processCommand(cmd); break;
                    case PORT_2: port2.processCommand(cmd); break;
                    default: fatalError;
                }
                break;

            case Cmd::MOUSE_BUTTON:
            case Cmd::JOY_EVENT:

                switch (cmd.action.port) {

                    case PORT_1: port1.processCommand(cmd); break;
                    case PORT_2: port2.processCommand(cmd); break;
                    default: fatalError;
                }
                break;

            case Cmd::DATASETTE_PLAY:
            case Cmd::DATASETTE_STOP:
            case Cmd::DATASETTE_REWIND:

                datasette.processCommand(cmd);
                break;

            case Cmd::CRT_BUTTON_PRESS:
            case Cmd::CRT_BUTTON_RELEASE:
            case Cmd::CRT_SWITCH_LEFT:
            case Cmd::CRT_SWITCH_NEUTRAL:
            case Cmd::CRT_SWITCH_RIGHT:

                expansionport.processCommand(cmd);
                break;

            case Cmd::RSH_EXECUTE:

                retroShell.exec();
                break;

            case Cmd::FOCUS:

                cmd.value ? focus() : unfocus();
                break;

            default:
                fatal("Unhandled command: %s\n", CmdEnum::key(cmd.type));
        }
    }

    // Inform the GUI about a changed machine configuration
    if (cmdConfig) { msgQueue.put(Msg::CONFIG); }

    // Inform the GUI about new RetroShell content
    if (retroShell.isDirty) { retroShell.isDirty = false; msgQueue.put(Msg::RSH_UPDATE); }
}

void
C64::computeFrame()
{
    if (emulator.get(Opt::VICII_POWER_SAVE)) {
        computeFrame(emulator.isWarping() && (frame & 7) != 0);
    } else {
        computeFrame(false);
    }
}

void 
C64::computeFrame(bool headless)
{
    setHeadless(headless);

    cpu.debugger.watchpointPC = -1;
    cpu.debugger.breakpointPC = -1;

    // Dispatch
    switch ((drive8.isPoweredOn()                   ? 4 : 0) |
            (drive9.isPoweredOn()                   ? 2 : 0) |
            (expansionport.needsAccurateEmulation() ? 1 : 0) ) {

        case 0b000: execute <false, false, false> (); break;
        case 0b001: execute <false, false, true>  (); break;
        case 0b010: execute <false, true,  false> (); break;
        case 0b011: execute <false, true,  true>  (); break;
        case 0b100: execute <true,  false, false> (); break;
        case 0b101: execute <true,  false, true>  (); break;
        case 0b110: execute <true,  true,  false> (); break;
        case 0b111: execute <true,  true,  true>  (); break;

        default:
            fatalError;
    }
}

template <bool enable8, bool enable9, bool execExp> void
C64::execute()
{
    auto lastCycle = vic.getCyclesPerLine();

    try {

        do {

            // Run the emulator for the (rest of the) current scanline
            for (; rasterCycle <= lastCycle; rasterCycle++) {

                // Execute one cycle
                executeCycle<enable8, enable9, execExp>();

                // Process all pending flags
                if (flags) processFlags();
            }

            // Finish the scanline
            endScanline();

        } while (scanline != 0);

    } catch (StateChangeException &) {

        // Finish the scanline if needed
        if (++rasterCycle > lastCycle) endScanline();

        // Rethrow the exception
        throw;
    }
}

template <bool enable8, bool enable9, bool execExp>
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

    cpu.execute<CPURevision::MOS_6510>();
    if constexpr (enable8) { if (drive8.needsEmulation) drive8.execute(durationOfOneCycle); }
    if constexpr (enable9) { if (drive9.needsEmulation) drive9.execute(durationOfOneCycle); }
    if constexpr (execExp) { expansionport.execute(); }
}

void
C64::processFlags()
{
    bool interrupt = false;

    if (flags & RL::BREAKPOINT) {

        clearFlag(RL::BREAKPOINT);
        msgQueue.put(Msg::BREAKPOINT_REACHED, CpuMsg {u16(cpu.debugger.breakpointPC)});
        interrupt = true;
    }

    if (flags & RL::WATCHPOINT) {

        clearFlag(RL::WATCHPOINT);
        msgQueue.put(Msg::WATCHPOINT_REACHED, CpuMsg {u16(cpu.debugger.watchpointPC)});
        interrupt = true;
    }

    if (flags & RL::STOP) {

        clearFlag(RL::STOP);
        interrupt = true;
    }

    if (flags & RL::CPU_JAM) {

        clearFlag(RL::CPU_JAM);
        msgQueue.put(Msg::CPU_JAMMED);
        interrupt = true;
    }

    if (flags & RL::SINGLE_STEP) {

        if ((!stepTo.has_value() && cpu.inFetchPhase()) || stepTo == cpu.getPC0()) {

            clearFlag(RL::SINGLE_STEP);
            msgQueue.put(Msg::STEP);
            interrupt = true;
        }
    }

    if (flags & RL::SINGLE_CYCLE) {
        
        clearFlag(RL::SINGLE_CYCLE);
        msgQueue.put(Msg::STEP);
        interrupt = true;
    }
    
    if (flags & RL::FINISH_LINE) {
        
        if (rasterCycle == vic.getCyclesPerLine()) {

            clearFlag(RL::FINISH_LINE);
            msgQueue.put(Msg::EOL_TRAP);
            interrupt = true;
        }
    }
    
    if (flags & RL::FINISH_FRAME) {
        
        if (scanline + 1 == vic.getLinesPerFrame() && rasterCycle == vic.getCyclesPerLine()) {
            
            clearFlag(RL::FINISH_FRAME);
            msgQueue.put(Msg::EOF_TRAP);
            interrupt = true;
        }
    }

    if (interrupt) throw StateChangeException((long)ExecState::PAUSED);
}

void 
C64::fastForward(isize frames)
{
    auto target = frame + frames;

    // Execute until the target frame has been reached
    while (frame < target) computeFrame();
}

void
C64::_isReady() const
{
    bool mega = hasMega65Rom(RomType::BASIC) && hasMega65Rom(RomType::KERNAL);
    
    if (!hasRom(RomType::BASIC)) {
        throw AppError(Fault::ROM_BASIC_MISSING);
    }
    if (!hasRom(RomType::CHAR)) {
        throw AppError(Fault::ROM_CHAR_MISSING);
    }
    if (!hasRom(RomType::KERNAL) || FORCE_ROM_MISSING) {
        throw AppError(Fault::ROM_KERNAL_MISSING);
    }
    if (FORCE_MEGA64_MISMATCH || (mega && string(mega65BasicRev()) != string(mega65KernalRev()))) {
        throw AppError(Fault::ROM_MEGA65_MISMATCH);
    }
}

void
C64::_powerOn()
{
    debug(RUN_DEBUG, "_powerOn\n");
    
    hardReset();
    msgQueue.put(Msg::POWER, 1);
}

void
C64::_powerOff()
{
    debug(RUN_DEBUG, "_powerOff\n");

    msgQueue.put(Msg::POWER, 0);
}

void
C64::_run()
{
    debug(RUN_DEBUG, "_run\n");
    // assert(cpu.inFetchPhase());

    msgQueue.put(Msg::RUN);
}

void
C64::_pause()
{
    debug(RUN_DEBUG, "_pause\n");
    // assert(cpu.inFetchPhase());

    // Clear pending runloop flags
    flags = 0;

    msgQueue.put(Msg::PAUSE);
}

void
C64::_halt()
{
    debug(RUN_DEBUG, "_halt\n");

    msgQueue.put(Msg::SHUTDOWN);
}

void
C64::_warpOn()
{
    debug(RUN_DEBUG, "_warpOn\n");

    msgQueue.put(Msg::WARP, 1);
}

void
C64::_warpOff()
{
    debug(RUN_DEBUG, "_warpOff\n");

    msgQueue.put(Msg::WARP, 0);
}

void
C64::_trackOn()
{
    debug(RUN_DEBUG, "_trackOn\n");

    msgQueue.put(Msg::TRACK, 1);
}

void
C64::_trackOff()
{
    debug(RUN_DEBUG, "_trackOff\n");

    msgQueue.put(Msg::TRACK, 0);
}

void
C64::cacheInfo(C64Info &result) const
{
    {   SYNCHRONIZED

        result.cpuProgress = cpu.clock;
        result.cia1Progress = cia1.sleeping ? cia1.sleepCycle : cpu.clock;
        result.cia2Progress = cia2.sleeping ? cia2.sleepCycle : cpu.clock;
        result.frame = frame;
        result.vpos = scanline;
        result.hpos = rasterCycle;

        for (isize i = 0; i < SLOT_COUNT; i++) {

            auto cycle = trigger[i];

            result.slotInfo[i].slot = EventSlot(i);
            result.slotInfo[i].eventId = eventid[i];
            result.slotInfo[i].trigger = cycle;
            result.slotInfo[i].triggerRel = cycle - cpu.clock;

            // Compute clock at pos (0,0)
            auto clock00 = cpu.clock - vic.getCyclesPerLine() * scanline - rasterCycle;

            // Compute the number of elapsed cycles since then
            auto diff = cycle - clock00;

            // Split into frame / line / cycle
            result.slotInfo[i].frameRel = long(diff / vic.getCyclesPerFrame());
            diff = diff % vic.getCyclesPerFrame();
            result.slotInfo[i].vpos = long(diff / vic.getCyclesPerLine());
            result.slotInfo[i].hpos = long(diff % vic.getCyclesPerLine());

            result.slotInfo[i].eventName = eventName(EventSlot(i), eventid[i]);
        }
    }
}

u64
C64::getAutoInspectionMask() const
{
    return data[SLOT_INS];
}

void
C64::setAutoInspectionMask(u64 mask)
{
    if (mask) {

        data[SLOT_INS] = mask;
        processINSEvent();

    } else {

        data[SLOT_INS] = 0;
        cancel<SLOT_INS>();
    }
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
    sidBridge.endFrame();
    mem.endFrame();
    iec.execute();
    expansionport.endOfFrame();
    port1.execute();
    port2.execute();
    drive8.vsyncHandler();
    drive9.vsyncHandler();
    recorder.vsyncHandler();
}

void
C64::processCommand(const Command &cmd)
{
    switch (cmd.type) {

        case Cmd::ALARM_ABS:

            setAlarmAbs(cmd.alarm.cycle, cmd.alarm.value);
            break;

        case Cmd::ALARM_REL:

            setAlarmRel(cmd.alarm.cycle, cmd.alarm.value);
            break;
            
        case Cmd::INSPECTION_TARGET:

            setAutoInspectionMask(cmd.value);
            break;

        case Cmd::HARD_RESET:
            
            emulator.hardReset();
            break;
            
        case Cmd::SOFT_RESET:
            
            emulator.softReset();
            break;
            
        case Cmd::POWER_ON:
            
            emulator.powerOn();
            break;
            
        case Cmd::POWER_OFF:
            
            emulator.powerOff();
            break;
            
        case Cmd::RUN:
            
            emulator.run();
            break;
            
        case Cmd::PAUSE:
            
            emulator.pause();
            break;
            
        case Cmd::WARP_ON:
            
            if (cmd.value == 0) {
                throw std::runtime_error("Source 0 is reserved for implementing config.warpMode.");
            }
            emulator.warpOn(isize(cmd.value));
            break;
            
        case Cmd::WARP_OFF:

            if (cmd.value == 0) {
                throw std::runtime_error("Source 0 is reserved for implementing config.warpMode.");
            }
            emulator.warpOff(isize(cmd.value));
            break;

        case Cmd::HALT:

            emulator.halt();
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
        cia1.serviceEvent(eventid[SLOT_CIA1]);
    }
    if (isDue<SLOT_CIA2>(cycle)) {
        cia2.serviceEvent(eventid[SLOT_CIA2]);
    }

    if (isDue<SLOT_SEC>(cycle)) {

        //
        // Check secondary slots
        //

        if (isDue<SLOT_SER>(cycle)) {
            iec.update();
        }

        if (isDue<SLOT_DAT>(cycle)) {
            datasette.processDatEvent(eventid[SLOT_DAT], data[SLOT_DAT]);
        }

        if (isDue<SLOT_TER>(cycle)) {

            //
            // Check tertiary slots
            //
            if (isDue<SLOT_EXP>(cycle)) {
                expansionport.processEvent(eventid[SLOT_EXP]);
            }
            if (isDue<SLOT_TXD>(cycle)) {
                userPort.rs232.processTxdEvent();
            }
            if (isDue<SLOT_RXD>(cycle)) {
                userPort.rs232.processRxdEvent();
            }
            if (isDue<SLOT_MOT>(cycle)) {
                datasette.processMotEvent(eventid[SLOT_MOT]);
            }
            if (isDue<SLOT_DC8>(cycle)) {
                drive8.processDiskChangeEvent(eventid[SLOT_DC8]);
            }
            if (isDue<SLOT_DC9>(cycle)) {
                drive9.processDiskChangeEvent(eventid[SLOT_DC9]);
            }
            if (isDue<SLOT_SNP>(cycle)) {
                processSNPEvent(eventid[SLOT_SNP]);
            }
            if (isDue<SLOT_RSH>(cycle)) {
                retroShell.serviceEvent();
            }
            if (isDue<SLOT_KEY>(cycle)) {
                keyboard.processKeyEvent(eventid[SLOT_KEY]);
            }
            if (isDue<SLOT_SRV>(cycle)) {
                remoteManager.serviceServerEvent();
            }
            if (isDue<SLOT_DBG>(cycle)) {
                regressionTester.processEvent(eventid[SLOT_DBG]);
            }
            if (isDue<SLOT_ALA>(cycle)) {
                processAlarmEvent();
            }
            if (isDue<SLOT_INS>(cycle)) {
                processINSEvent();
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
C64::processINSEvent()
{
    u64 mask = data[SLOT_INS];

    // Analyze bit mask
    if (mask & 1LL << long(Class::C64))             { record(); }
    if (mask & 1LL << long(Class::CPU))             { cpu.record(); }
    if (mask & 1LL << long(Class::Memory))          { mem.record(); }
    if (mask & 1LL << long(Class::CIA))             { cia1.record(); cia2.record(); }
    if (mask & 1LL << long(Class::VICII))           { vic.record(); }
    
    if (mask & 1LL << long(Class::SID)) {
        for (isize i = 0; i < 4; i++) sidBridge.sid[i].record();
    }

    // Reschedule the event
    scheduleRel<SLOT_INS>(Cycle(inspectionInterval * PAL::CYCLES_PER_SECOND), INS_INSPECT, mask);
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

MediaFile *
C64::takeSnapshot()
{
    Snapshot *result;

    // Take the snapshot
    result = new Snapshot(*this);

    // Compress the snapshot if requested
    result->compress(config.snapshotCompressor);

    return result;
}

void
C64::loadSnapshot(const MediaFile &file)
{
    try {

        const Snapshot &snap = dynamic_cast<const Snapshot &>(file);

        // Make a copy so we can modify the snapshot
        Snapshot snapshot(snap);

        // Uncompress the snapshot
        snapshot.uncompress();
        
        try {
            
            // Restore the saved state
            load(snapshot.getSnapshotData());
            
            // Rectify the VICII function table (varies between PAL and NTSC)
            vic.updateVicFunctionTable();
            
            // Clear the keyboard matrix to avoid constantly pressed keys
            keyboard.releaseAll();
            
            // Print some debug info if requested
            if (SNP_DEBUG) dump(Category::State);
            
        } catch (AppError &error) {
            
            /* If we reach this point, the emulator has been put into an
             * inconsistent state due to corrupted snapshot data. We cannot
             * continue emulation, because it would likely crash the
             * application. Because we cannot revert to the old state either,
             * we perform a hard reset to eliminate the inconsistency.
             */
            hardReset();
            throw error;
        }

        // Inform the GUI
        msgQueue.put(vic.pal() ? Msg::PAL : Msg::NTSC);
        msgQueue.put(Msg::SNAPSHOT_RESTORED);

    } catch (...) {

        throw AppError(Fault::FILE_TYPE_MISMATCH);
    }
}

void
C64::processSNPEvent(EventID eventId)
{
    // Check for the main instance (ignore the run-ahead instance)
    if (objid == 0) {

        // Take snapshot and hand it over to GUI
        msgQueue.put( Message { .type = Msg::SNAPSHOT_TAKEN, .snapshot = { new Snapshot(*this) } } );
    }

    // Schedule the next event
    scheduleNextSNPEvent();
}

void 
C64::scheduleNextSNPEvent()
{
    auto snapshots = emulator.get(Opt::C64_SNAP_AUTO);
    auto delay = emulator.get(Opt::C64_SNAP_DELAY);

    if (snapshots) {
        scheduleRel<SLOT_SNP>(C64::sec(double(delay)), SNP_TAKE);
    } else {
        cancel<SLOT_SNP>();
    }
}

RomTraits
C64::getRomTraits(u64 fnv)
{
    // Crawl through the Rom database
    for (auto &traits : roms) if (traits.fnv == fnv) return traits;

    return RomTraits {
        .title = "Unknown ROM",
        .subtitle = "",
        .revision = "",
        .vendor = RomVendor::UNKNOWN
    };
}

RomTraits
C64::getRomTraits(RomType type) const
{
    RomTraits result = getRomTraits(romFNV64(type));

    if (!result.fnv) result.fnv = romFNV64(type);
    if (!result.crc) result.crc = romCRC32(type);

    if (hasMega65Rom(type)) {

        result.title = "M.E.G.A. C64 OpenROM";
        result.vendor = RomVendor::MEGA65;
        result.patched = false;

        switch (type) {

            case RomType::BASIC:

                result.subtitle = "Free Basic Replacement";
                result.revision =  mega65BasicRev();
                break;

            case RomType::CHAR:

                result.subtitle = "Free Charset Replacement";
                break;

            case RomType::KERNAL:

                result.subtitle = "Free Kernal Replacement";
                result.revision = mega65KernalRev();
                break;

            default:
                fatalError;
        }
    }

    return result;
}

u32
C64::romCRC32(RomType type) const
{
    if (!hasRom(type)) return 0;
    
    switch (type) {
            
        case RomType::BASIC:  return util::crc32(mem.rom + 0xA000, 0x2000);
        case RomType::CHAR:   return util::crc32(mem.rom + 0xD000, 0x1000);
        case RomType::KERNAL: return util::crc32(mem.rom + 0xE000, 0x2000);
        case RomType::VC1541: return drive8.mem.romCRC32();

        default:
            fatalError;
    }
}

u64
C64::romFNV64(RomType type) const
{
    if (!hasRom(type)) return 0;
    
    switch (type) {
            
        case RomType::BASIC:  return util::fnv64(mem.rom + 0xA000, 0x2000);
        case RomType::CHAR:   return util::fnv64(mem.rom + 0xD000, 0x1000);
        case RomType::KERNAL: return util::fnv64(mem.rom + 0xE000, 0x2000);
        case RomType::VC1541: return drive8.mem.romFNV64();

        default:
            fatalError;
    }
}

bool
C64::hasRom(RomType type) const
{
    switch (type) {
            
        case RomType::BASIC:

            return (mem.rom[0xA000] | mem.rom[0xA001]) != 0x00;

        case RomType::CHAR:

            return (mem.rom[0xD000] | mem.rom[0xD001]) != 0x00;

        case RomType::KERNAL:

            return (mem.rom[0xE000] | mem.rom[0xE001]) != 0x00;

        case RomType::VC1541:

            assert(drive8.mem.hasRom() == drive9.mem.hasRom());
            return drive8.mem.hasRom();

        default:
            fatalError;
    }
}

bool
C64::hasMega65Rom(RomType type) const
{
    switch (type) {
            
        case RomType::BASIC:

            return mem.rom[0xBF52] == 'O' && mem.rom[0xBF53] == 'R';

        case RomType::CHAR:

            return getRomTraits(romFNV64(RomType::CHAR)).vendor == RomVendor::MEGA65;

        case RomType::KERNAL:

            return mem.rom[0xE4B9] == 'O' && mem.rom[0xE4BA] == 'R';

        case RomType::VC1541:

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
    
    if (hasMega65Rom(RomType::BASIC)) std::memcpy(rev, &mem.rom[0xBF55], 16);
    rev[16] = 0;
    
    return rev;
}

const char *
C64::mega65KernalRev() const
{
    static char rev[17];
    rev[0] = 0;
    
    if (hasMega65Rom(RomType::KERNAL)) std::memcpy(rev, &mem.rom[0xE4BC], 16);
    rev[16] = 0;
    
    return rev;
}

void
C64::loadRom(const fs::path &path)
{
    RomFile file(path);
    loadRom(file);
}

void
C64::loadRom(const MediaFile &file)
{
    switch (file.type()) {
            
        case FileType::BASIC_ROM:
            
            file.flash(mem.rom, 0xA000);
            debug(MEM_DEBUG, "Basic Rom flashed\n");
            debug(MEM_DEBUG, "hasMega65Rom() = %d\n", hasMega65Rom(RomType::BASIC));
            debug(MEM_DEBUG, "mega65BasicRev() = %s\n", mega65BasicRev());
            break;
            
        case FileType::CHAR_ROM:
            
            file.flash(mem.rom, 0xD000);
            debug(MEM_DEBUG, "Character Rom flashed\n");
            break;
            
        case FileType::KERNAL_ROM:
            
            file.flash(mem.rom, 0xE000);
            debug(MEM_DEBUG, "Kernal Rom flashed\n");
            debug(MEM_DEBUG, "hasMega65Rom() = %d\n", hasMega65Rom(RomType::KERNAL));
            debug(MEM_DEBUG, "mega65KernalRev() = %s\n", mega65KernalRev());
            break;
            
        case FileType::VC1541_ROM:
            
            drive8.mem.loadRom(file.getData(), file.getSize());
            drive9.mem.loadRom(file.getData(), file.getSize());
            debug(MEM_DEBUG, "VC1541 Rom flashed\n");
            break;
            
        default:
            
            throw AppError(Fault::FILE_TYPE_MISMATCH);
    }
}

void
C64::deleteRom(RomType type)
{
    switch (type) {
            
        case RomType::BASIC:
            
            memset(mem.rom + 0xA000, 0, 0x2000);
            break;
            
        case RomType::CHAR:
            
            memset(mem.rom + 0xD000, 0, 0x1000);
            break;
            
        case RomType::KERNAL:
            
            memset(mem.rom + 0xE000, 0, 0x2000);
            break;
            
        case RomType::VC1541:
            
            drive8.mem.deleteRom();
            drive9.mem.deleteRom();
            break;
            
        default:
            fatalError;
    }
}

void 
C64::deleteRoms()
{
    deleteRom(RomType::BASIC);
    deleteRom(RomType::KERNAL);
    deleteRom(RomType::CHAR);
    deleteRom(RomType::VC1541);
}

void
C64::saveRom(RomType type, const fs::path &path)
{
    switch (type) {
            
        case RomType::BASIC:

            if (hasRom(RomType::BASIC)) {
                RomFile file(mem.rom + 0xA000, 0x2000);
                file.writeToFile(path);
            }
            break;

        case RomType::CHAR:

            if (hasRom(RomType::CHAR)) {
                RomFile file(mem.rom + 0xD000, 0x1000);
                file.writeToFile(path);
            }
            break;

        case RomType::KERNAL:

            if (hasRom(RomType::KERNAL)) {
                RomFile file(mem.rom + 0xE000, 0x2000);
                file.writeToFile(path);
            }
            break;

        case RomType::VC1541:

            if (hasRom(RomType::VC1541)) {
                drive8.mem.saveRom(path);
            }
            break;
            
        default:
            fatalError;
    }
}

void 
C64::installOpenRoms()
{
    installOpenRom(RomType::BASIC);
    installOpenRom(RomType::KERNAL);
    installOpenRom(RomType::CHAR);
}

void
C64::installOpenRom(RomType type)
{
    switch (type) {
            
        case RomType::BASIC:
            
            assert(sizeof(basic_generic) == 0x2000);
            memcpy(mem.rom + 0xA000, basic_generic, 0x2000);
            break;
            
        case RomType::CHAR:
            
            assert(sizeof(chargen_openroms) == 0x1000);
            memcpy(mem.rom + 0xD000, chargen_openroms, 0x1000);
            break;
            
        case RomType::KERNAL:
            
            assert(sizeof(kernel_generic) == 0x2000);
            memcpy(mem.rom + 0xE000, kernel_generic, 0x2000);
            break;
            
        default:
            fatalError;
    }
}

void
C64::flash(const MediaFile &file)
{
    switch (file.type()) {
            
        case FileType::BASIC_ROM:
            file.flash(mem.rom, 0xA000);
            break;
            
        case FileType::CHAR_ROM:
            file.flash(mem.rom, 0xD000);
            break;
            
        case FileType::KERNAL_ROM:
            file.flash(mem.rom, 0xE000);
            break;
            
        case FileType::VC1541_ROM:
            drive8.mem.loadRom(dynamic_cast<const RomFile &>(file));
            drive9.mem.loadRom(dynamic_cast<const RomFile &>(file));
            break;
            
        case FileType::SNAPSHOT:
            loadSnapshot(dynamic_cast<const Snapshot &>(file));
            break;
            
        case FileType::D64:
        case FileType::T64:
        case FileType::P00:
        case FileType::PRG:
        case FileType::FOLDER:
            
            flash(file, 0);
            break;
            
        default:
            fatalError;
    }
}

void
C64::flash(const MediaFile &file, isize nr)
{
    try {
        
        const AnyCollection &collection = dynamic_cast<const AnyCollection &>(file);
        auto addr = (u16)collection.itemLoadAddr(nr);
        auto size = collection.itemSize(nr);
        if (size <= 2) return;
        
        switch (collection.type()) {
                
            case FileType::D64:
            case FileType::T64:
            case FileType::P00:
            case FileType::PRG:
            case FileType::FOLDER:
                
                // Flash data into memory
                size = std::min(size - 2, isize(0x10000 - addr));
                collection.copyItem(nr, mem.ram + addr, size, 2);
                
                // Rectify zero page
                mem.ram[0x2D] = LO_BYTE(addr + size);   // VARTAB (lo byte)
                mem.ram[0x2E] = HI_BYTE(addr + size);   // VARTAB (high byte)
                break;
                
            default:
                fatalError;
        }

        msgQueue.put(Msg::FILE_FLASHED);

    } catch (...) {

        throw AppError(Fault::FILE_TYPE_MISMATCH);
    }
}

void
C64::flash(const FileSystem &fs, isize nr)
{
    u16 addr = fs.loadAddr(nr);
    u64 size = fs.fileSize(nr);
    
    if (size <= 2) return;
    
    // Flash data into memory
    size = std::min(size - 2, (u64)(0x10000 - addr));
    fs.copyFile(nr, mem.ram + addr, size, 2);
    
    // Rectify zero page
    mem.ram[0x2D] = LO_BYTE(addr + size);   // VARTAB (lo byte)
    mem.ram[0x2E] = HI_BYTE(addr + size);   // VARTAB (high byte)
    
    msgQueue.put(Msg::FILE_FLASHED);
}

void
C64::setAlarmAbs(Cycle trigger, i64 payload)
{
    alarms.push_back(Alarm { trigger, payload });
    scheduleNextAlarm();
}

void
C64::setAlarmRel(Cycle trigger, i64 payload)
{
    alarms.push_back(Alarm { cpu.clock + trigger, payload });
    scheduleNextAlarm();
}

void
C64::processAlarmEvent()
{
    for (auto it = alarms.begin(); it != alarms.end(); ) {

        if (it->trigger <= cpu.clock) {
            msgQueue.put(Msg::ALARM, it->payload);
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

u32
C64::random()
{
    return random(u32(cpu.clock));
}

u32
C64::random(u32 seed)
{
    // Parameters for the Linear Congruential Generator (LCG)
    u64 a = 1664525;
    u64 c = 1013904223;
    u64 m = 1LL << 32;

    // Apply the LCG formula
    return u32((a * seed + c) % m);
}

}
