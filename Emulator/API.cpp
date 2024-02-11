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

namespace vc64 {

//
// Top level
//

void
Emulator::launch(const void *listener, Callback *func)
{
    _c64.msgQueue.setListener(listener, func);

    // Initialize all components
    initialize();

    // Reset the emulator
    _c64.hardReset();

    // Launch the emulator thread
    Thread::launch();
}

void
Emulator::configure(Option option, i64 value)
{
    debug(CNF_DEBUG, "configure(%s, %lld)\n", OptionEnum::key(option), value);

    // The following options do not send a message to the GUI
    static std::vector<Option> quiet = {

        OPT_BRIGHTNESS,
        OPT_CONTRAST,
        OPT_SATURATION,
        OPT_CUT_OPACITY,
        OPT_DMA_DEBUG_OPACITY,
        OPT_MOUSE_VELOCITY,
        OPT_AUTOFIRE_DELAY,
        OPT_AUDPAN,
        OPT_AUDVOL,
        OPT_AUDVOLL,
        OPT_AUDVOLR,
        OPT_DRV_PAN,
        OPT_DRV_POWER_VOL,
        OPT_DRV_STEP_VOL,
        OPT_DRV_INSERT_VOL,
        OPT_DRV_EJECT_VOL
    };

    // Check if this option has been locked for debugging
    value = overrideOption(option, value);

    switch (option) {

        case OPT_WARP_MODE:
        case OPT_WARP_BOOT:
        case OPT_VSYNC:
        case OPT_TIME_LAPSE:
        case OPT_RUN_AHEAD:

            setConfigItem(option, value);
            break;

        case OPT_HOST_SAMPLE_RATE:

            host.setConfigItem(option, value);
            _c64.muxer.setConfigItem(option, value);
            break;

        case OPT_HOST_REFRESH_RATE:
        case OPT_HOST_FRAMEBUF_WIDTH:
        case OPT_HOST_FRAMEBUF_HEIGHT:

            host.setConfigItem(option, value);
            break;

        case OPT_VIC_REVISION:
        case OPT_PALETTE:
        case OPT_BRIGHTNESS:
        case OPT_CONTRAST:
        case OPT_SATURATION:
        case OPT_GRAY_DOT_BUG:
        case OPT_VIC_POWER_SAVE:
        case OPT_HIDE_SPRITES:
        case OPT_SS_COLLISIONS:
        case OPT_SB_COLLISIONS:
        case OPT_GLUE_LOGIC:

            _c64.vic.setConfigItem(option, value);
            break;

        case OPT_CUT_LAYERS:
        case OPT_CUT_OPACITY:
        case OPT_DMA_DEBUG_ENABLE:
        case OPT_DMA_DEBUG_MODE:
        case OPT_DMA_DEBUG_OPACITY:

            _c64.vic.dmaDebugger.setConfigItem(option, value);
            break;

        case OPT_POWER_GRID:

            _c64.supply.setConfigItem(option, value);
            break;

        case OPT_CIA_REVISION:
        case OPT_TIMER_B_BUG:

            _c64.cia1.setConfigItem(option, value);
            _c64.cia2.setConfigItem(option, value);
            break;

        case OPT_SID_ENABLE:
        case OPT_SID_ADDRESS:

            _c64.muxer.setConfigItem(option, 0, value);
            _c64.muxer.setConfigItem(option, 1, value);
            _c64.muxer.setConfigItem(option, 2, value);
            _c64.muxer.setConfigItem(option, 3, value);

        case OPT_SID_REVISION:
        case OPT_SID_FILTER:
        case OPT_SID_SAMPLING:
        case OPT_SID_POWER_SAVE:
        case OPT_SID_ENGINE:
        case OPT_AUDPAN:
        case OPT_AUDVOL:
        case OPT_AUDVOLL:
        case OPT_AUDVOLR:

            _c64.muxer.setConfigItem(option, value);
            break;

        case OPT_RAM_PATTERN:
        case OPT_SAVE_ROMS:

            _c64.mem.setConfigItem(option, value);
            break;

        case OPT_DRV_AUTO_CONFIG:
        case OPT_DRV_TYPE:
        case OPT_DRV_RAM:
        case OPT_DRV_PARCABLE:
        case OPT_DRV_CONNECT:
        case OPT_DRV_POWER_SWITCH:
        case OPT_DRV_POWER_SAVE:
        case OPT_DRV_EJECT_DELAY:
        case OPT_DRV_SWAP_DELAY:
        case OPT_DRV_INSERT_DELAY:
        case OPT_DRV_PAN:
        case OPT_DRV_POWER_VOL:
        case OPT_DRV_STEP_VOL:
        case OPT_DRV_INSERT_VOL:
        case OPT_DRV_EJECT_VOL:

            _c64.drive8.setConfigItem(option, value);
            _c64.drive9.setConfigItem(option, value);
            break;

        case OPT_DAT_MODEL:
        case OPT_DAT_CONNECT:
            _c64.datasette.setConfigItem(option, value);

        case OPT_MOUSE_MODEL:
        case OPT_SHAKE_DETECTION:
        case OPT_MOUSE_VELOCITY:

            _c64.port1.mouse.setConfigItem(option, value);
            _c64.port2.mouse.setConfigItem(option, value);
            break;

        case OPT_AUTOFIRE:
        case OPT_AUTOFIRE_BULLETS:
        case OPT_AUTOFIRE_DELAY:

            _c64.port1.joystick.setConfigItem(option, value);
            _c64.port2.joystick.setConfigItem(option, value);
            break;

        default:
            warn("Unrecognized option: %s\n", OptionEnum::key(option));
            fatalError;
    }

    if (std::find(quiet.begin(), quiet.end(), option) == quiet.end()) {
        _c64.msgQueue.put(MSG_CONFIG, option);
    }
}

void
Emulator::configure(Option option, long id, i64 value)
{
    debug(CNF_DEBUG, "configure(%s, %ld, %lld)\n", OptionEnum::key(option), id, value);

    // Check if this option has been locked for debugging
    value = overrideOption(option, value);

    // The following options do not send a message to the GUI
    static std::vector<Option> quiet = {

        OPT_MOUSE_VELOCITY,
        OPT_AUTOFIRE_DELAY,
        OPT_AUDPAN,
        OPT_AUDVOL,
        OPT_AUDVOLL,
        OPT_AUDVOLR,
        OPT_DRV_PAN,
        OPT_DRV_POWER_VOL,
        OPT_DRV_STEP_VOL,
        OPT_DRV_INSERT_VOL,
        OPT_DRV_EJECT_VOL
    };

    switch (option) {


        case OPT_DMA_DEBUG_ENABLE:
        case OPT_DMA_DEBUG_CHANNEL:
        case OPT_DMA_DEBUG_COLOR:

            _c64.vic.dmaDebugger.setConfigItem(option, id, value);
            break;

        case OPT_CIA_REVISION:
        case OPT_TIMER_B_BUG:

            switch (id) {
                case 0: _c64.cia1.setConfigItem(option, value); break;
                case 1: _c64.cia2.setConfigItem(option, value); break;
                default: fatalError;
            }
            break;

        case OPT_MOUSE_MODEL:
        case OPT_SHAKE_DETECTION:
        case OPT_MOUSE_VELOCITY:

            switch (id) {
                case PORT_1: _c64.port1.mouse.setConfigItem(option, value); break;
                case PORT_2: _c64.port2.mouse.setConfigItem(option, value); break;
                default: fatalError;
            }
            break;

        case OPT_AUTOFIRE:
        case OPT_AUTOFIRE_BULLETS:
        case OPT_AUTOFIRE_DELAY:

            switch (id) {
                case PORT_1: _c64.port1.joystick.setConfigItem(option, value); break;
                case PORT_2: _c64.port2.joystick.setConfigItem(option, value); break;
                default: fatalError;
            }
            break;

        case OPT_SID_ENABLE:
        case OPT_SID_ADDRESS:
        case OPT_SID_REVISION:
        case OPT_SID_FILTER:
        case OPT_SID_POWER_SAVE:
        case OPT_SID_ENGINE:
        case OPT_SID_SAMPLING:
        case OPT_AUDPAN:
        case OPT_AUDVOL:
        case OPT_AUDVOLL:
        case OPT_AUDVOLR:

            _c64.muxer.setConfigItem(option, id, value);
            break;

        case OPT_DRV_AUTO_CONFIG:
        case OPT_DRV_TYPE:
        case OPT_DRV_RAM:
        case OPT_DRV_PARCABLE:
        case OPT_DRV_CONNECT:
        case OPT_DRV_POWER_SWITCH:
        case OPT_DRV_POWER_SAVE:
        case OPT_DRV_EJECT_DELAY:
        case OPT_DRV_SWAP_DELAY:
        case OPT_DRV_INSERT_DELAY:
        case OPT_DRV_PAN:
        case OPT_DRV_POWER_VOL:
        case OPT_DRV_STEP_VOL:
        case OPT_DRV_INSERT_VOL:
        case OPT_DRV_EJECT_VOL:

            switch (id) {
                case DRIVE8: _c64.drive8.setConfigItem(option, value); break;
                case DRIVE9: _c64.drive9.setConfigItem(option, value); break;
                default: fatalError;
            }
            break;

        default:
            warn("Unrecognized option: %s\n", OptionEnum::key(option));
            fatalError;
    }

    if (std::find(quiet.begin(), quiet.end(), option) == quiet.end()) {
        _c64.msgQueue.put(MSG_CONFIG, option);
    }
}

void
Emulator::configure(C64Model model)
{
    assert_enum(C64Model, model);

    {   SUSPENDED

        revertToFactorySettings();

        switch(model) {

            case C64_MODEL_PAL:

                configure(OPT_VIC_REVISION, VICII_PAL_6569_R3);
                configure(OPT_GRAY_DOT_BUG, false);
                configure(OPT_CIA_REVISION, MOS_6526);
                configure(OPT_TIMER_B_BUG,  true);
                configure(OPT_SID_REVISION, MOS_6581);
                configure(OPT_SID_FILTER,   true);
                configure(OPT_POWER_GRID,   GRID_STABLE_50HZ);
                configure(OPT_GLUE_LOGIC,   GLUE_LOGIC_DISCRETE);
                configure(OPT_RAM_PATTERN,   RAM_PATTERN_VICE);
                break;

            case C64_MODEL_PAL_II:

                configure(OPT_VIC_REVISION, VICII_PAL_8565);
                configure(OPT_GRAY_DOT_BUG, true);
                configure(OPT_CIA_REVISION, MOS_8521);
                configure(OPT_TIMER_B_BUG,  false);
                configure(OPT_SID_REVISION, MOS_8580);
                configure(OPT_SID_FILTER,   true);
                configure(OPT_POWER_GRID,   GRID_STABLE_50HZ);
                configure(OPT_GLUE_LOGIC,   GLUE_LOGIC_IC);
                configure(OPT_RAM_PATTERN,   RAM_PATTERN_VICE);
                break;

            case C64_MODEL_PAL_OLD:

                configure(OPT_VIC_REVISION, VICII_PAL_6569_R1);
                configure(OPT_GRAY_DOT_BUG, false);
                configure(OPT_CIA_REVISION, MOS_6526);
                configure(OPT_TIMER_B_BUG,  true);
                configure(OPT_SID_REVISION, MOS_6581);
                configure(OPT_SID_FILTER,   true);
                configure(OPT_POWER_GRID,   GRID_STABLE_50HZ);
                configure(OPT_GLUE_LOGIC,   GLUE_LOGIC_DISCRETE);
                configure(OPT_RAM_PATTERN,   RAM_PATTERN_VICE);
                break;

            case C64_MODEL_NTSC:

                configure(OPT_VIC_REVISION, VICII_NTSC_6567);
                configure(OPT_GRAY_DOT_BUG, false);
                configure(OPT_CIA_REVISION, MOS_6526);
                configure(OPT_TIMER_B_BUG,  false);
                configure(OPT_SID_REVISION, MOS_6581);
                configure(OPT_SID_FILTER,   true);
                configure(OPT_POWER_GRID,   GRID_STABLE_60HZ);
                configure(OPT_GLUE_LOGIC,   GLUE_LOGIC_DISCRETE);
                configure(OPT_RAM_PATTERN,   RAM_PATTERN_VICE);
                break;

            case C64_MODEL_NTSC_II:

                configure(OPT_VIC_REVISION, VICII_NTSC_8562);
                configure(OPT_GRAY_DOT_BUG, true);
                configure(OPT_CIA_REVISION, MOS_8521);
                configure(OPT_TIMER_B_BUG,  true);
                configure(OPT_SID_REVISION, MOS_8580);
                configure(OPT_SID_FILTER,   true);
                configure(OPT_POWER_GRID,   GRID_STABLE_60HZ);
                configure(OPT_GLUE_LOGIC,   GLUE_LOGIC_IC);
                configure(OPT_RAM_PATTERN,   RAM_PATTERN_VICE);
                break;

            case C64_MODEL_NTSC_OLD:

                configure(OPT_VIC_REVISION, VICII_NTSC_6567_R56A);
                configure(OPT_GRAY_DOT_BUG, false);
                configure(OPT_CIA_REVISION, MOS_6526);
                configure(OPT_TIMER_B_BUG,  false);
                configure(OPT_SID_REVISION, MOS_6581);
                configure(OPT_SID_FILTER,   true);
                configure(OPT_POWER_GRID,   GRID_STABLE_60HZ);
                configure(OPT_GLUE_LOGIC,   GLUE_LOGIC_DISCRETE);
                configure(OPT_RAM_PATTERN,   RAM_PATTERN_VICE);
                break;

            default:
                fatalError;
        }
    }
}

i64
Emulator::getConfigItem(Option option) const
{
    switch (option) {

        case OPT_WARP_BOOT:     return config.warpBoot;
        case OPT_WARP_MODE:     return config.warpMode;
        case OPT_VSYNC:         return config.vsync;
        case OPT_TIME_LAPSE:    return config.timeLapse;
        case OPT_RUN_AHEAD:     return config.runAhead;

        case OPT_HOST_REFRESH_RATE:
        case OPT_HOST_SAMPLE_RATE:
        case OPT_HOST_FRAMEBUF_WIDTH:
        case OPT_HOST_FRAMEBUF_HEIGHT:

            return host.getConfigItem(option);

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

            return _c64.vic.getConfigItem(option);

        case OPT_DMA_DEBUG_ENABLE:
        case OPT_DMA_DEBUG_MODE:
        case OPT_DMA_DEBUG_OPACITY:
        case OPT_CUT_LAYERS:
        case OPT_CUT_OPACITY:

            return _c64.vic.dmaDebugger.getConfigItem(option);

        case OPT_CIA_REVISION:
        case OPT_TIMER_B_BUG:

            assert(_c64.cia1.getConfigItem(option) == _c64.cia2.getConfigItem(option));
            return _c64.cia1.getConfigItem(option);

        case OPT_POWER_GRID:

            return _c64.supply.getConfigItem(option);

        case OPT_SID_REVISION:
        case OPT_SID_POWER_SAVE:
        case OPT_SID_FILTER:
        case OPT_SID_ENGINE:
        case OPT_SID_SAMPLING:
        case OPT_AUDVOLL:
        case OPT_AUDVOLR:

            return _c64.muxer.getConfigItem(option);

        case OPT_RAM_PATTERN:
        case OPT_SAVE_ROMS:

            return _c64.mem.getConfigItem(option);

        case OPT_DAT_MODEL:
        case OPT_DAT_CONNECT:

            return _c64.datasette.getConfigItem(option);

        default:
            fatalError;
    }
}

i64
Emulator::getConfigItem(Option option, long id) const
{
    const Drive &drive = id == DRIVE8 ? _c64.drive8 : _c64.drive9;

    switch (option) {

        case OPT_DMA_DEBUG_CHANNEL:
        case OPT_DMA_DEBUG_COLOR:

            return _c64.vic.dmaDebugger.getConfigItem(option, id);

        case OPT_SID_ENABLE:
        case OPT_SID_ADDRESS:
        case OPT_AUDPAN:
        case OPT_AUDVOL:

            assert(id >= 0 && id <= 3);
            return _c64.muxer.getConfigItem(option, id);

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

            if (id == PORT_1) return _c64.port1.mouse.getConfigItem(option);
            if (id == PORT_2) return _c64.port2.mouse.getConfigItem(option);
            fatalError;

        case OPT_AUTOFIRE:
        case OPT_AUTOFIRE_BULLETS:
        case OPT_AUTOFIRE_DELAY:

            if (id == PORT_1) return _c64.port1.joystick.getConfigItem(option);
            if (id == PORT_2) return _c64.port2.joystick.getConfigItem(option);
            fatalError;

        default:
            fatalError;
    }
}

void
Emulator::setConfigItem(Option option, i64 value)
{
    switch (option) {

        case OPT_WARP_BOOT:

            config.warpBoot = isize(value);
            return;

        case OPT_WARP_MODE:

            if (!WarpModeEnum::isValid(value)) {
                throw VC64Error(ERROR_OPT_INVARG, WarpModeEnum::keyList());
            }

            config.warpMode = WarpMode(value);
            return;

        case OPT_VSYNC:

            config.vsync = bool(value);
            return;

        case OPT_TIME_LAPSE:

            if (value < 50 || value > 200) {
                throw VC64Error(ERROR_OPT_INVARG, "50...200");
            }

            config.timeLapse = isize(value);
            return;

        case OPT_RUN_AHEAD:

            if (value < 0 || value > 5) {
                throw VC64Error(ERROR_OPT_INVARG, "0...5");
            }

            config.runAhead = isize(value);
            return;

        default:
            fatalError;
    }
}


//
// C64
//

void
Emulator::C64_API::hardReset()
{
    assert(isUserThread());

    {   SUSPENDED

        c64.reset(true);
    }
}

void
Emulator::C64_API::softReset()
{
    assert(isUserThread());

    {   SUSPENDED

        c64.reset(false);
    }
}

void 
Emulator::C64_API::stopAndGo()
{
    c64.stopAndGo();
}

void
Emulator::C64_API::stepInto()
{
    c64.stepInto();
}

void
Emulator::C64_API::stepOver()
{
    c64.stepOver();
}

InspectionTarget
Emulator::C64_API::getInspectionTarget() const
{
    return c64.getInspectionTarget();
}

void
Emulator::C64_API::setInspectionTarget(InspectionTarget target, Cycle trigger)
{
    c64.setInspectionTarget(target, trigger);
}

void
Emulator::C64_API::removeInspectionTarget()
{
    c64.removeInspectionTarget();
}

u64 
Emulator::C64_API::getFrame()
{
    return c64.frame;
}

EventInfo
Emulator::C64_API::getEventInfo() const
{
    return c64.getEventInfo();
}

EventSlotInfo
Emulator::C64_API::getSlotInfo(isize nr) const
{
    return c64.getSlotInfo(nr);
}

void 
Emulator::C64_API::isReady()
{
    c64.isReady();
}

void 
Emulator::C64_API::requestAutoSnapshot()
{
    c64.requestAutoSnapshot();
}

void 
Emulator::C64_API::requestUserSnapshot()
{
    c64.requestUserSnapshot();
}

Snapshot *
Emulator::C64_API::latestAutoSnapshot()
{
    return c64.latestAutoSnapshot();
}

Snapshot *
Emulator::C64_API::latestUserSnapshot()
{
    return c64.latestUserSnapshot();
}

void 
Emulator::C64_API::loadSnapshot(const Snapshot &snapshot)
{
    c64.loadSnapshot(snapshot);
}

void 
Emulator::C64_API::signalBrk()
{
    c64.signalBrk();
}

RomIdentifier Emulator::C64_API::romIdentifier(RomType type) const
{
    return c64.romIdentifier(type);
}

const string Emulator::C64_API::romTitle(RomType type) const
{
    return c64.romTitle(type);
}

const string Emulator::C64_API::romSubTitle(u64 fnv) const
{
    return c64.romSubTitle(fnv);
}

const string Emulator::C64_API::romSubTitle(RomType type) const
{
    return c64.romSubTitle(type);
}

const string Emulator::C64_API::romRevision(RomType type) const
{
    return c64.romRevision(type);
}

bool Emulator::C64_API::hasRom(RomType type) const
{
    return c64.hasRom(type);
}

bool Emulator::C64_API::hasMega65Rom(RomType type) const
{
    return c64.hasMega65Rom(type);
}

void Emulator::C64_API::loadRom(const string &path)
{
    c64.loadRom(path);
}

void Emulator::C64_API::loadRom(const RomFile &file)
{
    c64.loadRom(file);
}

void Emulator::C64_API::deleteRom(RomType type)
{
    c64.deleteRom(type);
}

void Emulator::C64_API::saveRom(RomType rom, const string &path)
{
    c64.saveRom(rom, path);
}

void Emulator::C64_API::flash(const AnyFile &file)
{
    c64.flash(file);
}

void Emulator::C64_API::flash(const AnyCollection &file, isize item)
{
    c64.flash(file, item);
}

void Emulator::C64_API::flash(const FileSystem &fs, isize item)
{
    c64.flash(fs, item);
}

void Emulator::C64_API::setAlarmAbs(Cycle trigger, i64 payload)
{
    c64.setAlarmAbs(trigger, payload);
}

void Emulator::C64_API::setAlarmRel(Cycle trigger, i64 payload)
{
    c64.setAlarmRel(trigger, payload);
}


//
// CPU
//

CPUInfo
Emulator::CPU_API::getInfo() const
{
    assert(isUserThread());
    return cpu.getInfo();
}

i64
Emulator::CPU_API::clock() const
{
    assert(isUserThread());
    return cpu.clock;
}

u16
Emulator::CPU_API::getPC0() const
{
    assert(isUserThread());
    return cpu.getPC0();
}

isize
Emulator::CPU_API::loggedInstructions() const
{
    assert(isUserThread());
    return cpu.debugger.loggedInstructions();
}

u16
Emulator::CPU_API::loggedPC0Rel(isize nr) const
{
    assert(isUserThread());
    return cpu.debugger.loggedPC0Rel(nr);
}

u16
Emulator::CPU_API::loggedPC0Abs(isize nr) const
{
    assert(isUserThread());
    return cpu.debugger.loggedPC0Abs(nr);
}

RecordedInstruction
Emulator::CPU_API::logEntryAbs(isize nr) const
{
    assert(isUserThread());
    return cpu.debugger.logEntryAbs(nr);
}

void
Emulator::CPU_API::clearLog()
{
    assert(isUserThread());
    return cpu.debugger.clearLog();
}

void
Emulator::CPU_API::setNumberFormat(DasmNumberFormat instrFormat, DasmNumberFormat dataFormat)
{
    assert(isUserThread());
    return cpu.disassembler.setNumberFormat(instrFormat, dataFormat);
}

isize
Emulator::CPU_API::disassembleRecordedInstr(isize i, char *str) const
{
    assert(isUserThread());
    return cpu.debugger.disassembleRecordedInstr(i, str);
}

isize
Emulator::CPU_API::disassembleRecordedBytes(isize i, char *str) const
{
    assert(isUserThread());
    return cpu.debugger.disassembleRecordedBytes(i, str);
}

void
Emulator::CPU_API::disassembleRecordedFlags(isize i, char *str) const
{
    assert(isUserThread());
    return cpu.debugger.disassembleRecordedFlags(i, str);
}

void
Emulator::CPU_API::disassembleRecordedPC(isize i, char *str) const
{
    assert(isUserThread());
    return cpu.debugger.disassembleRecordedPC(i, str);

}

isize
Emulator::CPU_API::disassemble(char *str, u16 addr) const
{
    assert(isUserThread());
    return cpu.disassembler.disassemble(str, addr);
}

isize
Emulator::CPU_API::getLengthOfInstructionAt(u16 addr) const
{
    assert(isUserThread());
    return cpu.getLengthOfInstructionAt(addr);
}

void
Emulator::CPU_API::dumpBytes(char *str, u16 addr, isize length) const
{
    assert(isUserThread());
    return cpu.disassembler.dumpBytes(str, addr, length);
}

void
Emulator::CPU_API::dumpWord(char *str, u16 addr) const
{
    assert(isUserThread());
    return cpu.disassembler.dumpWord(str, addr);
}


//
// Guards
//

long 
Emulator::GUARD_API::elements() const
{
    return guards.elements();
}

u32 
Emulator::GUARD_API::guardAddr(long nr) const
{
    return guards.guardAddr(nr);
}

bool 
Emulator::GUARD_API::isEnabled(long nr) const
{
    return guards.isEnabled(nr);
}

bool 
Emulator::GUARD_API::isDisabled(long nr) const
{
    return guards.isDisabled(nr);
}

bool 
Emulator::GUARD_API::isSetAt(u32 addr) const
{
    return guards.isSetAt(addr);
}

bool 
Emulator::GUARD_API::isSetAndEnabledAt(u32 addr) const
{
    return guards.isSetAndEnabledAt(addr);
}

bool 
Emulator::GUARD_API::isSetAndDisabledAt(u32 addr) const
{
    return guards.isSetAndDisabledAt(addr);
}

bool 
Emulator::GUARD_API::isSetAndConditionalAt(u32 addr) const
{
    return guards.isSetAndConditionalAt(addr);
}

void 
Emulator::GUARD_API::setEnable(long nr, bool val)
{
    guards.setEnable(nr, val);
}

void 
Emulator::GUARD_API::enable(long nr)
{
    guards.enable(nr);
}

void 
Emulator::GUARD_API::disable(long nr)
{
    guards.disable(nr);
}

void 
Emulator::GUARD_API::setEnableAt(u32 addr, bool val)
{
    guards.setEnableAt(addr, val);
}

void 
Emulator::GUARD_API::enableAt(u32 addr)
{
    guards.enableAt(addr);
}

void 
Emulator::GUARD_API::disableAt(u32 addr)
{
    guards.disableAt(addr);
}

void 
Emulator::GUARD_API::addAt(u32 addr, long skip)
{
    guards.addAt(addr, skip);
}

void 
Emulator::GUARD_API::removeAt(u32 addr)
{
    guards.removeAt(addr);
}

void 
Emulator::GUARD_API::remove(long nr)
{
    guards.remove(nr);
}

void 
Emulator::GUARD_API::removeAll()
{
    guards.removeAll();
}

void 
Emulator::GUARD_API::replace(long nr, u32 addr)
{
    guards.replace(nr, addr);
}


//
// Memory
//

MemConfig
Emulator::MEM_API::getConfig() const
{
    assert(isUserThread());
    return emulator._c64.mem.getConfig();
}

MemInfo
Emulator::MEM_API::getInfo() const
{
    assert(isUserThread());
    return emulator._c64.mem.getInfo();
}

string
Emulator::MEM_API::memdump(u16 addr, isize num, bool hex, isize pads, MemoryType src) const
{
    assert(isUserThread());
    return mem.memdump(addr, num, hex, pads, src);
}

string
Emulator::MEM_API::txtdump(u16 addr, isize num, MemoryType src) const
{
    assert(isUserThread());
    return mem.txtdump(addr, num, src);
}


//
// CIAs
//

CIAInfo
Emulator::CIA_API::getInfo() const
{
    assert(isUserThread());
    return cia.getInfo();
}


//
// VICII
//

VICIIConfig
Emulator::VICII_API::getConfig() const
{
    return vic.getConfig();
}

VICIIInfo
Emulator::VICII_API::getInfo() const
{
    return vic.getInfo();
}

SpriteInfo
Emulator::VICII_API::getSpriteInfo(isize nr) const
{
    return vic.getSpriteInfo(nr);
}

isize
Emulator::VICII_API::getCyclesPerLine() const
{
    return vic.getCyclesPerLine();
}

isize
Emulator::VICII_API::getLinesPerFrame() const
{
    return vic.getLinesPerFrame();
}

bool
Emulator::VICII_API::pal() const
{
    return vic.pal();
}

u32 *
Emulator::VICII_API::stableEmuTexture() const
{
    return vic.stableEmuTexture();
}

u32 *
Emulator::VICII_API::getNoise() const
{
    return vic.getNoise();
}

u32
Emulator::VICII_API::getColor(isize nr) const
{
    return vic.getColor(nr);
}

u32
Emulator::VICII_API::getColor(isize nr, Palette palette) const
{
    return vic.getColor(nr, palette);
}

//
// SID
//

SIDConfig
Emulator::SID_API::getConfig() const
{
    assert(isUserThread());
    return muxer.getConfig();
}

SIDInfo
Emulator::SID_API::getInfo(isize nr) const
{
    assert(isUserThread());
    return muxer.getInfo(nr);
}

VoiceInfo
Emulator::SID_API::getVoiceInfo(isize nr, isize voice) const
{
    assert(isUserThread());
    return muxer.getVoiceInfo(nr, voice);
}


SIDStats
Emulator::SID_API::getStats() const
{
    assert(isUserThread());
    return muxer.getStats();
}

void
Emulator::SID_API::rampUp()
{
    assert(isUserThread());
    muxer.rampUp();
}

void
Emulator::SID_API::rampUpFromZero()
{
    assert(isUserThread());
    muxer.rampUpFromZero();
}

void
Emulator::SID_API::rampDown()
{
    assert(isUserThread());
    muxer.rampDown();
}

void
Emulator::SID_API::copyMono(float *buffer, isize n)
{
    assert(isUserThread());
    muxer.copyMono(buffer, n);
}

void
Emulator::SID_API::copyStereo(float *left, float *right, isize n)
{
    assert(isUserThread());
    muxer.copyStereo(left, right, n);
}

void
Emulator::SID_API::copyInterleaved(float *buffer, isize n)
{
    assert(isUserThread());
    muxer.copyInterleaved(buffer, n);
}

float
Emulator::SID_API::draw(u32 *buffer, isize width, isize height,
                        float maxAmp, u32 color, isize sid) const
{
    assert(isUserThread());
    return muxer.draw(buffer, width, height, maxAmp, color, sid);
}


//
// DMA Debugger
//

DmaDebuggerConfig
Emulator::DMA_DEBUGGER_API::getConfig() const
{
    assert(isUserThread());
    return vic.dmaDebugger.getConfig();
}


//
// Keyboard
//

bool Emulator::KBD_API::isPressed(C64Key key) const { return keyboard.isPressed(key); }
bool Emulator::KBD_API::shiftLockIsPressed() const { return keyboard.shiftLockIsPressed(); }
bool Emulator::KBD_API::restoreIsPressed() const { return keyboard.shiftLockIsPressed(); }

// Presses a key
void Emulator::KBD_API::press(C64Key key) { keyboard.press(key); }
void Emulator::KBD_API::pressShiftLock() { keyboard.pressShiftLock(); }
void Emulator::KBD_API::pressRestore() { keyboard.pressRestore(); }

// Releases a pressed key
void Emulator::KBD_API::release(C64Key key) { keyboard.release(key); }
void Emulator::KBD_API::releaseShiftLock() { keyboard.releaseShiftLock(); }
void Emulator::KBD_API::releaseRestore() { keyboard.releaseRestore(); }

// Clears the keyboard matrix
void Emulator::KBD_API::releaseAll() { keyboard.releaseAll(); }

// Auto typing
void Emulator::KBD_API::autoType(const string &text) { keyboard.autoType(text); }

void Emulator::KBD_API::scheduleKeyPress(std::vector<C64Key> keys, double delay) {
    keyboard.scheduleKeyPress(keys, delay);
}
void Emulator::KBD_API::scheduleKeyRelease(std::vector<C64Key> keys, double delay) {
    keyboard.scheduleKeyRelease(keys, delay);
}
void Emulator::KBD_API::scheduleKeyReleaseAll(double delay) {
    keyboard.scheduleKeyReleaseAll(delay);
}

// Deletes all pending actions and clears the keyboard matrix
void Emulator::KBD_API::abortAutoTyping() { keyboard.abortAutoTyping(); }


//
// Joystick
//

void Emulator::JOYSTICK_API::trigger(GamePadAction event)
{
    joystick.trigger(event);
}


//
// Datasette
//

bool 
Emulator::DATASETTE_API::hasTape() const
{
    return datasette.hasTape();
}

isize Emulator::DATASETTE_API::getCounter() const
{
    return datasette.getCounter();
}

void Emulator::DATASETTE_API::insertTape(TAPFile &file)
{
    datasette.insertTape(file);
}

void Emulator::DATASETTE_API::ejectTape()
{
    datasette.ejectTape();
}

u8 Emulator::DATASETTE_API::getType() const
{
    return datasette.getType();
}

bool Emulator::DATASETTE_API::getMotor() const
{
    return datasette.getMotor();
}

bool Emulator::DATASETTE_API::getPlayKey() const
{
    return datasette.getPlayKey();
}

void Emulator::DATASETTE_API::pressPlay()
{
    datasette.pressPlay();
}

void Emulator::DATASETTE_API::pressStop()
{
    datasette.pressStop();
}

void Emulator::DATASETTE_API::rewind(isize seconds)
{
    datasette.rewind(seconds);
}


//
// Mouse
//

bool Emulator::MOUSE_API::detectShakeXY(double x, double y)
{
    return mouse.detectShakeXY(x, y);
}

bool Emulator::MOUSE_API::detectShakeDxDy(double dx, double dy)
{
    return mouse.detectShakeDxDy(dx, dy);
}

void Emulator::MOUSE_API::setXY(double x, double y)
{
    mouse.setXY(x, y);
}

void Emulator::MOUSE_API::setDxDy(double dx, double dy)
{
    mouse.setDxDy(dx, dy);
}

void Emulator::MOUSE_API::trigger(GamePadAction event)
{
    mouse.trigger(event);
}


//
// Recorder
//

const string
Emulator::REC_API::getExecPath() const
{
    return FFmpeg::getExecPath();
}

void Emulator::REC_API::setExecPath(const string &path)
{
    FFmpeg::setExecPath(path);
}

std::vector<string> &
Emulator::REC_API::paths() const
{
    return FFmpeg::paths;
}

bool
Emulator::REC_API::available() const
{
    return FFmpeg::available();
}

util::Time
Emulator::REC_API::getDuration() const
{
    return recorder.getDuration();
}

isize
Emulator::REC_API::getFrameRate() const
{
    return recorder.getFrameRate();
}

isize Emulator::REC_API::getBitRate() const
{
    return recorder.getBitRate();
}

isize Emulator::REC_API::getSampleRate() const
{
    return recorder.getSampleRate();
}

bool
Emulator::REC_API::isRecording() const
{
    return recorder.isRecording();
}

void
Emulator::REC_API::startRecording(isize x1, isize y1, isize x2, isize y2,
                                  isize bitRate,
                                  isize aspectX, isize aspectY)
{
    recorder.startRecording(x1, y1, x2, y2, bitRate, aspectX, aspectY);
}

void 
Emulator::REC_API::stopRecording()
{
    recorder.stopRecording();
}

bool 
Emulator::REC_API::exportAs(const string &path)
{
    return recorder.exportAs(path);
}


const char *
Emulator::RSHELL_API::text()
{
    return retroShell.text();
}

void 
Emulator::RSHELL_API::press(RetroShellKey key, bool shift)
{
    retroShell.press(key, shift); 
}

void 
Emulator::RSHELL_API::press(char c)
{
    retroShell.press(c);
}

void 
Emulator::RSHELL_API::press(const string &s)
{
    retroShell.press(s);
}

isize 
Emulator::RSHELL_API::cursorRel()
{
    return retroShell.cursorRel();
}

void 
Emulator::RSHELL_API::continueScript()
{
    return retroShell.continueScript();
}


//
// Expansion port
//

CartridgeInfo
Emulator::EXP_PORT_API::getInfo() const
{
    return expansionport.getInfo();
}

CartridgeRomInfo 
Emulator::EXP_PORT_API::getRomInfo(isize nr) const
{
    return expansionport.getRomInfo(nr);
}

CartridgeType 
Emulator::EXP_PORT_API::getCartridgeType() const
{
    return expansionport.getCartridgeType();
}

bool 
Emulator::EXP_PORT_API::getCartridgeAttached() const
{
    return expansionport.getCartridgeAttached();
}

void 
Emulator::EXP_PORT_API::attachCartridge(const string &path, bool reset)
{
    expansionport.attachCartridge(path, reset);
}

void 
Emulator::EXP_PORT_API::attachCartridge(CRTFile *c, bool reset)
{
    expansionport.attachCartridge(c, reset);
}

void 
Emulator::EXP_PORT_API::attachCartridge(Cartridge *c)
{
    expansionport.attachCartridge(c);
}

void 
Emulator::EXP_PORT_API::attachReu(isize capacity)
{
    expansionport.attachReu(capacity);
}

void 
Emulator::EXP_PORT_API::attachGeoRam(isize capacity)
{
    expansionport.attachGeoRam(capacity);
}

void 
Emulator::EXP_PORT_API::attachIsepicCartridge()
{
    expansionport.attachIsepicCartridge();
}

void 
Emulator::EXP_PORT_API::detachCartridge()
{
    expansionport.detachCartridge();
}

void 
Emulator::EXP_PORT_API::detachCartridgeAndReset()
{
    expansionport.detachCartridgeAndReset();
}

isize 
Emulator::EXP_PORT_API::numButtons() const
{
    return expansionport.numButtons();
}

const 
string Emulator::EXP_PORT_API::getButtonTitle(isize nr) const
{
    return expansionport.getButtonTitle(nr);
}

void 
Emulator::EXP_PORT_API::pressButton(isize nr)
{
    return expansionport.pressButton(nr);
}

void 
Emulator::EXP_PORT_API::releaseButton(isize nr)
{
    return expansionport.releaseButton(nr);
}

bool 
Emulator::EXP_PORT_API::hasSwitch() const
{
    return expansionport.hasSwitch();
}

isize 
Emulator::EXP_PORT_API::getSwitch() const
{
    return expansionport.getSwitch();
}

bool 
Emulator::EXP_PORT_API::switchIsNeutral() const
{
    return expansionport.switchIsNeutral();
}

bool 
Emulator::EXP_PORT_API::switchIsLeft() const
{
    return expansionport.switchIsLeft();
}

bool 
Emulator::EXP_PORT_API::switchIsRight() const
{
    return expansionport.switchIsRight();
}

const string 
Emulator::EXP_PORT_API::getSwitchDescription(isize pos) const
{
    return expansionport.getSwitchDescription(pos);
}

const string
Emulator::EXP_PORT_API::getSwitchDescription() const
{
    return expansionport.getSwitchDescription();
}

bool 
Emulator::EXP_PORT_API::validSwitchPosition(isize pos) const
{
    return expansionport.validSwitchPosition(pos);
}

void 
Emulator::EXP_PORT_API::setSwitch(isize pos)
{
    return expansionport.setSwitch(pos);
}

bool 
Emulator::EXP_PORT_API::hasLED() const
{
    return expansionport.hasLED();
}

bool 
Emulator::EXP_PORT_API::getLED() const
{
    return expansionport.getLED();
}

void 
Emulator::EXP_PORT_API::setLED(bool value)
{
    return expansionport.setLED(value);
}

isize 
Emulator::EXP_PORT_API::getRamCapacity() const
{
    return expansionport.getRamCapacity();
}

bool 
Emulator::EXP_PORT_API::hasBattery() const
{
    return expansionport.hasBattery();
}

void 
Emulator::EXP_PORT_API::setBattery(bool value)
{
    expansionport.setLED(value);
}

bool 
Emulator::IEC_API::isTransferring() const
{
    return iec.isTransferring();
}

bool
Emulator::DISK_API::isWriteProtected() const
{
    Disk *disk = drive.disk.get();
    return disk ? false: disk->isWriteProtected();
}

void 
Emulator::DISK_API::setWriteProtection(bool b) 
{
    Disk *disk = drive.disk.get();
    if (disk) disk->setWriteProtection(b);
}

void Emulator::DISK_API::toggleWriteProtection()
{
    Disk *disk = drive.disk.get();
    if (disk) disk->toggleWriteProtection();
}

//
// Drive
//

bool
Emulator::DRIVE_API::hasDisk() const
{
    return drive.hasDisk();
}

bool 
Emulator::DRIVE_API::hasPartiallyRemovedDisk() const
{
    return drive.hasPartiallyRemovedDisk();
}

bool 
Emulator::DRIVE_API::hasProtectedDisk() const
{
    return drive.hasProtectedDisk();
}

bool 
Emulator::DRIVE_API::hasModifiedDisk() const
{
    return drive.hasModifiedDisk();
}

bool 
Emulator::DRIVE_API::hasUnmodifiedDisk() const
{
    return drive.hasUnmodifiedDisk();
}

bool 
Emulator::DRIVE_API::hasUnprotectedDisk() const
{
    return drive.hasUnprotectedDisk();
}

void 
Emulator::DRIVE_API::setModificationFlag(bool value)
{
    drive.setModificationFlag(value);
}

void 
Emulator::DRIVE_API::markDiskAsModified()
{
    drive.markDiskAsModified();
}

void 
Emulator::DRIVE_API::markDiskAsUnmodified()
{
    drive.markDiskAsUnmodified();
}

/*
bool
Emulator::DRIVE_API::isModified() const
{
    return drive.isModified();
}

void 
Emulator::DRIVE_API::setModified(bool b)
{
    drive.setModified(b);
}
*/

isize
Emulator::DRIVE_API::getDeviceNr() const
{
    return drive.getDeviceNr();
}

bool 
Emulator::DRIVE_API::getRedLED() const
{
    return drive.getRedLED();
}

bool 
Emulator::DRIVE_API::isRotating() const
{
    return drive.isRotating();
}

const DriveConfig &
Emulator::DRIVE_API::getConfig() const
{
    return drive.getConfig();
}

bool
Emulator::DRIVE_API::readMode() const
{
    return drive.readMode();
}

bool
Emulator::DRIVE_API::writeMode() const
{
    return drive.writeMode();
}

Halftrack 
Emulator::DRIVE_API::getHalftrack() const
{
    return drive.getHalftrack();
}

Track
Emulator::DRIVE_API::getTrack() const
{
    return drive.getTrack();
}

isize
Emulator::DRIVE_API::sizeOfHalftrack(Halftrack ht)
{
    return drive.sizeOfHalftrack(ht);
}

isize
Emulator::DRIVE_API::sizeOfCurrentHalftrack()
{
    return drive.sizeOfCurrentHalftrack();
}

HeadPos
Emulator::DRIVE_API::getOffset() const
{
    return drive.getOffset();
}

void 
Emulator::DRIVE_API::insertNewDisk(DOSType fstype, PETName<16> name)
{
    drive.insertNewDisk(fstype, name);
}

void
Emulator::DRIVE_API::insertD64(const D64File &d64, bool wp)
{
    drive.insertD64(d64, wp);
}

void
Emulator::DRIVE_API::insertG64(const G64File &g64, bool wp)
{
    drive.insertG64(g64, wp);
}

void
Emulator::DRIVE_API::insertCollection(AnyCollection &archive, bool wp)
{
    drive.insertCollection(archive, wp);
}

void
Emulator::DRIVE_API::insertFileSystem(const class FileSystem &device, bool wp)
{
    drive.insertFileSystem(device, wp);
}

void
Emulator::DRIVE_API::ejectDisk()
{
    drive.ejectDisk();
}

u8 
Emulator::DRIVE_API::readBitFromHead() const
{
    return drive.readBitFromHead();
}

}
