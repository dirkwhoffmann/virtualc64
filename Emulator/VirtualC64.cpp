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
#include "VirtualC64.h"

string
VirtualC64::version()
{
    return C64::version();
}

string
VirtualC64::build()
{
    return C64::build();
}

VirtualC64::VirtualC64() :
c64(*this),
mem(*this),
cpu(*this),
cia1(*this, _c64.cia1),
cia2(*this, _c64.cia2),
vicii(*this),
muxer(*this),
dmaDebugger(*this),
keyboard(*this),
datasette(*this),
port1(*this, _c64.port1),
port2(*this, _c64.port2),
recorder(*this),
expansionport(*this),
iec(*this),
drive8(*this, _c64.drive8),
drive9(*this, _c64.drive9),
retroShell(*this)
{ }

VirtualC64::~VirtualC64()
{
    halt();
}

u32 *
VirtualC64::getTexture() const
{
    return isPoweredOn() ? _c64.vic.stableEmuTexture() : _c64.vic.getNoise();
}

u32 *
VirtualC64::getNoise() const
{
    return _c64.vic.getNoise();
}

void
VirtualC64::launch(const void *listener, Callback *func)
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
VirtualC64::configure(C64Model model)
{
    Emulator::configure(model);
}

void
VirtualC64::configure(Option option, i64 value)
{
    Emulator::configure(option, value);
}

void
VirtualC64::configure(Option option, long id, i64 value)
{
    Emulator::configure(option, id, value);
}

i64
VirtualC64::getConfigItem(Option option) const
{
    return Emulator::getConfigItem(option);
}

i64
VirtualC64::getConfigItem(Option option, long id) const
{
    return Emulator::getConfigItem(option, id);
}

void
VirtualC64::setConfigItem(Option option, i64 value)
{
    Emulator::setConfigItem(option, value);
}

void
VirtualC64::put(const Cmd &cmd)
{
    cmdQueue.put(cmd);

    if (cmd.type == CMD_HALT) {
        join();
    }
}

/*
void
VirtualC64::put(CmdType type, i64 payload)
{
    put(Cmd(type, payload));
}

void
VirtualC64::put(CmdType type, KeyCmd payload)
{
    put(Cmd(type, payload));
}

void 
VirtualC64::put(CmdType type, CoordCmd payload)
{
    put(Cmd(type, payload));
}

void
VirtualC64::put(CmdType type, GamePadCmd payload)
{
    put( Cmd { .type = type, .action = payload } );
}

void
VirtualC64::put(CmdType type, TapeCmd payload)
{
    put( Cmd { .type = type, .tape = payload } );
}

void
VirtualC64::put(CmdType type, AlarmCmd payload)
{
    put( Cmd { .type = type, .alarm = payload } );
}
*/

//
// C64
//

void
VirtualC64::C64_API::hardReset()
{
    assert(isUserThread());

    {   SUSPENDED

        c64.hardReset();
    }
}

void
VirtualC64::C64_API::softReset()
{
    assert(isUserThread());

    {   SUSPENDED

        c64.hardReset();
    }
}

void 
VirtualC64::C64_API::stopAndGo()
{
    c64.stopAndGo();
}

void
VirtualC64::C64_API::stepInto()
{
    c64.stepInto();
}

void
VirtualC64::C64_API::stepOver()
{
    c64.stepOver();
}

InspectionTarget
VirtualC64::C64_API::getInspectionTarget() const
{
    return c64.getInspectionTarget();
}

void
VirtualC64::C64_API::setInspectionTarget(InspectionTarget target, Cycle trigger)
{
    c64.setInspectionTarget(target, trigger);
}

void
VirtualC64::C64_API::removeInspectionTarget()
{
    c64.removeInspectionTarget();
}

RomInfo
VirtualC64::C64_API::getRomInfo(RomType type) const
{
    return c64.getRomInfo(type);
}

C64Info
VirtualC64::C64_API::getInfo() const
{
    return c64.getState();
}

EventSlotInfo
VirtualC64::C64_API::getSlotInfo(isize nr) const
{
    return c64.getSlotInfo(nr);
}

void 
VirtualC64::C64_API::isReady()
{
    c64.isReady();
}

Snapshot *
VirtualC64::C64_API::latestAutoSnapshot()
{
    return c64.latestAutoSnapshot();
}

Snapshot *
VirtualC64::C64_API::latestUserSnapshot()
{
    return c64.latestUserSnapshot();
}

void 
VirtualC64::C64_API::loadSnapshot(const Snapshot &snapshot)
{
    c64.loadSnapshot(snapshot);
}

void VirtualC64::C64_API::loadRom(const string &path)
{
    c64.loadRom(path);
}

void VirtualC64::C64_API::loadRom(const RomFile &file)
{
    c64.loadRom(file);
}

void VirtualC64::C64_API::deleteRom(RomType type)
{
    c64.deleteRom(type);
}

void VirtualC64::C64_API::saveRom(RomType rom, const string &path)
{
    c64.saveRom(rom, path);
}

void VirtualC64::C64_API::flash(const AnyFile &file)
{
    c64.flash(file);
}

void VirtualC64::C64_API::flash(const AnyCollection &file, isize item)
{
    c64.flash(file, item);
}

void VirtualC64::C64_API::flash(const FileSystem &fs, isize item)
{
    c64.flash(fs, item);
}


//
// CPU
//

CPUInfo
VirtualC64::CPU_API::getInfo() const
{
    assert(isUserThread());
    return cpu.getState();
}

i64
VirtualC64::CPU_API::clock() const
{
    assert(isUserThread());
    return cpu.clock;
}

u16
VirtualC64::CPU_API::getPC0() const
{
    assert(isUserThread());
    return cpu.getPC0();
}

isize
VirtualC64::CPU_API::loggedInstructions() const
{
    assert(isUserThread());
    return cpu.debugger.loggedInstructions();
}

u16
VirtualC64::CPU_API::loggedPC0Rel(isize nr) const
{
    assert(isUserThread());
    return cpu.debugger.loggedPC0Rel(nr);
}

u16
VirtualC64::CPU_API::loggedPC0Abs(isize nr) const
{
    assert(isUserThread());
    return cpu.debugger.loggedPC0Abs(nr);
}

RecordedInstruction
VirtualC64::CPU_API::logEntryAbs(isize nr) const
{
    assert(isUserThread());
    return cpu.debugger.logEntryAbs(nr);
}

void
VirtualC64::CPU_API::clearLog()
{
    assert(isUserThread());
    return cpu.debugger.clearLog();
}

void
VirtualC64::CPU_API::setNumberFormat(DasmNumberFormat instrFormat, DasmNumberFormat dataFormat)
{
    assert(isUserThread());
    return cpu.disassembler.setNumberFormat(instrFormat, dataFormat);
}

isize
VirtualC64::CPU_API::disassembleRecordedInstr(isize i, char *str) const
{
    assert(isUserThread());
    return cpu.debugger.disassembleRecordedInstr(i, str);
}

isize
VirtualC64::CPU_API::disassembleRecordedBytes(isize i, char *str) const
{
    assert(isUserThread());
    return cpu.debugger.disassembleRecordedBytes(i, str);
}

void
VirtualC64::CPU_API::disassembleRecordedFlags(isize i, char *str) const
{
    assert(isUserThread());
    return cpu.debugger.disassembleRecordedFlags(i, str);
}

void
VirtualC64::CPU_API::disassembleRecordedPC(isize i, char *str) const
{
    assert(isUserThread());
    return cpu.debugger.disassembleRecordedPC(i, str);

}

isize
VirtualC64::CPU_API::disassemble(char *str, u16 addr) const
{
    assert(isUserThread());
    return cpu.disassembler.disassemble(str, addr);
}

isize
VirtualC64::CPU_API::getLengthOfInstructionAt(u16 addr) const
{
    assert(isUserThread());
    return cpu.getLengthOfInstructionAt(addr);
}

void
VirtualC64::CPU_API::dumpBytes(char *str, u16 addr, isize length) const
{
    assert(isUserThread());
    return cpu.disassembler.dumpBytes(str, addr, length);
}

void
VirtualC64::CPU_API::dumpWord(char *str, u16 addr) const
{
    assert(isUserThread());
    return cpu.disassembler.dumpWord(str, addr);
}


//
// Guards
//

long 
VirtualC64::GUARD_API::elements() const
{
    return guards.elements();
}

u32 
VirtualC64::GUARD_API::guardAddr(long nr) const
{
    return guards.guardAddr(nr);
}

bool 
VirtualC64::GUARD_API::isEnabled(long nr) const
{
    return guards.isEnabled(nr);
}

bool 
VirtualC64::GUARD_API::isDisabled(long nr) const
{
    return guards.isDisabled(nr);
}

bool 
VirtualC64::GUARD_API::isSetAt(u32 addr) const
{
    return guards.isSetAt(addr);
}

bool 
VirtualC64::GUARD_API::isSetAndEnabledAt(u32 addr) const
{
    return guards.isSetAndEnabledAt(addr);
}

bool 
VirtualC64::GUARD_API::isSetAndDisabledAt(u32 addr) const
{
    return guards.isSetAndDisabledAt(addr);
}

bool 
VirtualC64::GUARD_API::isSetAndConditionalAt(u32 addr) const
{
    return guards.isSetAndConditionalAt(addr);
}

void 
VirtualC64::GUARD_API::setEnable(long nr, bool val)
{
    guards.setEnable(nr, val);
}

void 
VirtualC64::GUARD_API::enable(long nr)
{
    guards.enable(nr);
}

void 
VirtualC64::GUARD_API::disable(long nr)
{
    guards.disable(nr);
}

void 
VirtualC64::GUARD_API::setEnableAt(u32 addr, bool val)
{
    guards.setEnableAt(addr, val);
}

void 
VirtualC64::GUARD_API::enableAt(u32 addr)
{
    guards.enableAt(addr);
}

void 
VirtualC64::GUARD_API::disableAt(u32 addr)
{
    guards.disableAt(addr);
}

void 
VirtualC64::GUARD_API::addAt(u32 addr, long skip)
{
    guards.addAt(addr, skip);
}

void 
VirtualC64::GUARD_API::removeAt(u32 addr)
{
    guards.removeAt(addr);
}

void 
VirtualC64::GUARD_API::remove(long nr)
{
    guards.remove(nr);
}

void 
VirtualC64::GUARD_API::removeAll()
{
    guards.removeAll();
}

void 
VirtualC64::GUARD_API::replace(long nr, u32 addr)
{
    guards.replace(nr, addr);
}


//
// Memory
//

MemConfig
VirtualC64::MEM_API::getConfig() const
{
    assert(isUserThread());
    return emulator._c64.mem.getConfig();
}

MemInfo
VirtualC64::MEM_API::getInfo() const
{
    assert(isUserThread());
    return emulator._c64.mem.getState();
}

string
VirtualC64::MEM_API::memdump(u16 addr, isize num, bool hex, isize pads, MemoryType src) const
{
    assert(isUserThread());
    return mem.memdump(addr, num, hex, pads, src);
}

string
VirtualC64::MEM_API::txtdump(u16 addr, isize num, MemoryType src) const
{
    assert(isUserThread());
    return mem.txtdump(addr, num, src);
}


//
// CIAs
//

CIAConfig
VirtualC64::CIA_API::getConfig() const
{
    return cia.getConfig();
}

CIAInfo
VirtualC64::CIA_API::getInfo() const
{
    return cia.getState();
}

CIAStats
VirtualC64::CIA_API::getStats() const
{
    return cia.getStats();
}


//
// VICII
//

VICIIConfig
VirtualC64::VICII_API::getConfig() const
{
    return vic.getConfig();
}

VICIIInfo
VirtualC64::VICII_API::getInfo() const
{
    return vic.getState();
}

SpriteInfo
VirtualC64::VICII_API::getSpriteInfo(isize nr) const
{
    return vic.getSpriteInfo(nr);
}

isize
VirtualC64::VICII_API::getCyclesPerLine() const
{
    return vic.getCyclesPerLine();
}

isize
VirtualC64::VICII_API::getLinesPerFrame() const
{
    return vic.getLinesPerFrame();
}

bool
VirtualC64::VICII_API::pal() const
{
    return vic.pal();
}

u32
VirtualC64::VICII_API::getColor(isize nr) const
{
    return vic.getColor(nr);
}

u32
VirtualC64::VICII_API::getColor(isize nr, Palette palette) const
{
    return vic.getColor(nr, palette);
}

//
// SID
//

SIDConfig
VirtualC64::SID_API::getConfig() const
{
    assert(isUserThread());
    return muxer.getConfig();
}

SIDInfo
VirtualC64::SID_API::getInfo(isize nr) const
{
    assert(isUserThread());
    return muxer.getInfo(nr);
}

VoiceInfo
VirtualC64::SID_API::getVoiceInfo(isize nr, isize voice) const
{
    assert(isUserThread());
    return muxer.getVoiceInfo(nr, voice);
}


SIDStats
VirtualC64::SID_API::getStats() const
{
    assert(isUserThread());
    return muxer.getStats();
}

void
VirtualC64::SID_API::rampUp()
{
    assert(isUserThread());
    muxer.rampUp();
}

void
VirtualC64::SID_API::rampUp(float from)
{
    assert(isUserThread());
    muxer.rampUp(from);
}

void
VirtualC64::SID_API::rampDown()
{
    assert(isUserThread());
    muxer.rampDown();
}

void
VirtualC64::SID_API::copyMono(float *buffer, isize n)
{
    assert(isUserThread());
    muxer.copyMono(buffer, n);
}

void
VirtualC64::SID_API::copyStereo(float *left, float *right, isize n)
{
    assert(isUserThread());
    muxer.copyStereo(left, right, n);
}

void
VirtualC64::SID_API::copyInterleaved(float *buffer, isize n)
{
    assert(isUserThread());
    muxer.copyInterleaved(buffer, n);
}

float
VirtualC64::SID_API::draw(u32 *buffer, isize width, isize height,
                        float maxAmp, u32 color, isize sid) const
{
    assert(isUserThread());
    return muxer.draw(buffer, width, height, maxAmp, color, sid);
}


//
// DMA Debugger
//

DmaDebuggerConfig
VirtualC64::DMA_DEBUGGER_API::getConfig() const
{
    assert(isUserThread());
    return vic.dmaDebugger.getConfig();
}


//
// Keyboard
//

bool VirtualC64::KBD_API::isPressed(C64Key key) const 
{
    return keyboard.isPressed(key);
}

void VirtualC64::KBD_API::autoType(const string &text)
{
    keyboard.autoType(text);
}

void VirtualC64::KBD_API::abortAutoTyping() 
{
    keyboard.abortAutoTyping();
}


//
// Joystick
//

void VirtualC64::JOYSTICK_API::trigger(GamePadAction event)
{
    joystick.trigger(event);
}


//
// Datasette
//

DatasetteInfo 
VirtualC64::DATASETTE_API::getInfo() const
{
    return datasette.getInfo();
}

void 
VirtualC64::DATASETTE_API::insertTape(TAPFile &file)
{
    datasette.insertTape(file);
}

void 
VirtualC64::DATASETTE_API::ejectTape()
{
    datasette.ejectTape();
}


//
// Mouse
//

bool VirtualC64::MOUSE_API::detectShakeXY(double x, double y)
{
    return mouse.detectShakeXY(x, y);
}

bool VirtualC64::MOUSE_API::detectShakeDxDy(double dx, double dy)
{
    return mouse.detectShakeDxDy(dx, dy);
}

void VirtualC64::MOUSE_API::setXY(double x, double y)
{
    mouse.setXY(x, y);
}

void VirtualC64::MOUSE_API::setDxDy(double dx, double dy)
{
    mouse.setDxDy(dx, dy);
}

void VirtualC64::MOUSE_API::trigger(GamePadAction event)
{
    mouse.trigger(event);
}


//
// Recorder
//

const string
VirtualC64::REC_API::getExecPath() const
{
    return FFmpeg::getExecPath();
}

void VirtualC64::REC_API::setExecPath(const string &path)
{
    FFmpeg::setExecPath(path);
}

bool
VirtualC64::REC_API::available() const
{
    return FFmpeg::available();
}

util::Time
VirtualC64::REC_API::getDuration() const
{
    return recorder.getDuration();
}

isize
VirtualC64::REC_API::getFrameRate() const
{
    return recorder.getFrameRate();
}

isize VirtualC64::REC_API::getBitRate() const
{
    return recorder.getBitRate();
}

isize VirtualC64::REC_API::getSampleRate() const
{
    return recorder.getSampleRate();
}

bool
VirtualC64::REC_API::isRecording() const
{
    return recorder.isRecording();
}

void
VirtualC64::REC_API::startRecording(isize x1, isize y1, isize x2, isize y2,
                                  isize bitRate,
                                  isize aspectX, isize aspectY)
{
    recorder.startRecording(x1, y1, x2, y2, bitRate, aspectX, aspectY);
}

void 
VirtualC64::REC_API::stopRecording()
{
    recorder.stopRecording();
}

bool 
VirtualC64::REC_API::exportAs(const string &path)
{
    return recorder.exportAs(path);
}


const char *
VirtualC64::RSHELL_API::text()
{
    return retroShell.text();
}

isize
VirtualC64::RSHELL_API::cursorRel()
{
    return retroShell.cursorRel();
}

void
VirtualC64::RSHELL_API::press(RetroShellKey key, bool shift)
{
    retroShell.press(key, shift); 
}

void 
VirtualC64::RSHELL_API::press(char c)
{
    retroShell.press(c);
}

void 
VirtualC64::RSHELL_API::press(const string &s)
{
    retroShell.press(s);
}

void 
VirtualC64::RSHELL_API::execScript(std::stringstream &ss)
{
    retroShell.execScript(ss);
}

void
VirtualC64::RSHELL_API::execScript(const std::ifstream &fs)
{
    retroShell.execScript(fs);
}

void
VirtualC64::RSHELL_API::execScript(const string &contents)
{
    retroShell.execScript(contents);
}

void
VirtualC64::RSHELL_API::setStream(std::ostream &os)
{
    retroShell.setStream(os);
}


//
// Expansion port
//

CartridgeTraits 
VirtualC64::EXP_PORT_API::getTraits() const
{
    return expansionport.getTraits();
}

CartridgeInfo
VirtualC64::EXP_PORT_API::getInfo() const
{
    return expansionport.getInfo();
}

CartridgeRomInfo 
VirtualC64::EXP_PORT_API::getRomInfo(isize nr) const
{
    return expansionport.getRomInfo(nr);
}

void
VirtualC64::EXP_PORT_API::attachCartridge(const string &path, bool reset)
{
    expansionport.attachCartridge(path, reset);
}

void 
VirtualC64::EXP_PORT_API::attachCartridge(CRTFile *c, bool reset)
{
    expansionport.attachCartridge(c, reset);
}

void 
VirtualC64::EXP_PORT_API::attachCartridge(Cartridge *c)
{
    expansionport.attachCartridge(c);
}

void 
VirtualC64::EXP_PORT_API::attachReu(isize capacity)
{
    expansionport.attachReu(capacity);
}

void 
VirtualC64::EXP_PORT_API::attachGeoRam(isize capacity)
{
    expansionport.attachGeoRam(capacity);
}

void 
VirtualC64::EXP_PORT_API::attachIsepicCartridge()
{
    expansionport.attachIsepicCartridge();
}

void 
VirtualC64::EXP_PORT_API::detachCartridge()
{
    expansionport.detachCartridge();
}


//
// Drive
//

const DriveConfig &
VirtualC64::DRIVE_API::getConfig() const
{
    return drive.getConfig();
}

DriveInfo
VirtualC64::DRIVE_API::getInfo() const
{
    return drive.getInfo();
}

void
VirtualC64::DRIVE_API::insertBlankDisk(DOSType fstype, PETName<16> name)
{
    drive.insertNewDisk(fstype, name);
}

void
VirtualC64::DRIVE_API::insertD64(const D64File &d64, bool wp)
{
    drive.insertD64(d64, wp);
}

void
VirtualC64::DRIVE_API::insertG64(const G64File &g64, bool wp)
{
    drive.insertG64(g64, wp);
}

void
VirtualC64::DRIVE_API::insertCollection(AnyCollection &archive, bool wp)
{
    drive.insertCollection(archive, wp);
}

void
VirtualC64::DRIVE_API::insertFileSystem(const class FileSystem &device, bool wp)
{
    drive.insertFileSystem(device, wp);
}

void
VirtualC64::DRIVE_API::ejectDisk()
{
    drive.ejectDisk();
}

/*
u8
VirtualC64::DRIVE_API::readBitFromHead() const
{
    return drive.readBitFromHead();
}
*/
