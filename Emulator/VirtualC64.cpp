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

const Defaults &
VirtualC64::defaults = Emulator::defaults;

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
cia1(*this, main.cia1),
cia2(*this, main.cia2),
vicii(*this),
muxer(*this),
dmaDebugger(*this),
keyboard(*this),
datasette(*this),
port1(*this, main.port1),
port2(*this, main.port2),
recorder(*this),
expansionport(*this),
iec(*this),
drive8(*this, main.drive8),
drive9(*this, main.drive9),
retroShell(*this)
{ }

VirtualC64::~VirtualC64()
{
    halt();
}

void
VirtualC64::stepInto()
{
    Emulator::stepInto();
}

void
VirtualC64::stepOver()
{
    Emulator::stepOver();
}

void
VirtualC64::launch(const void *listener, Callback *func)
{
    Emulator::launch(listener, func);
}

i64
VirtualC64::get(Option option) const
{
    return Emulator::get(option);
}

i64 
VirtualC64::get(Option option, long id) const
{
    return Emulator::get(option, id);
}

void
VirtualC64::set(C64Model model)
{
    Emulator::set(model);
    main.markAsDirty();
}

void 
VirtualC64::set(Option option, i64 value) throws
{
    Emulator::set(option, value);
    main.markAsDirty();
}

void 
VirtualC64::set(Option option, long id, i64 value) throws
{
    Emulator::set(option, id, value);
    main.markAsDirty();
}

void
VirtualC64::put(const Cmd &cmd)
{
    Emulator::put(cmd);
}


//
// C64
//

void
VirtualC64::C64API::hardReset()
{
    assert(isUserThread());

    {   SUSPENDED

        c64.hardReset();
        c64.markAsDirty();
    }
}

void
VirtualC64::C64API::softReset()
{
    assert(isUserThread());

    {   SUSPENDED

        c64.hardReset();
        c64.markAsDirty();
    }
}

InspectionTarget
VirtualC64::C64API::getInspectionTarget() const
{
    return c64.getInspectionTarget();
}

void
VirtualC64::C64API::setInspectionTarget(InspectionTarget target)
{
    c64.setInspectionTarget(target);
}

void
VirtualC64::C64API::removeInspectionTarget()
{
    c64.removeInspectionTarget();
}

RomInfo
VirtualC64::C64API::getRomInfo(RomType type) const
{
    return c64.getRomInfo(type);
}

void
VirtualC64::C64API::isReady()
{
    c64.isReady();
}

Snapshot *
VirtualC64::C64API::latestAutoSnapshot()
{
    return c64.latestAutoSnapshot();
}

Snapshot *
VirtualC64::C64API::latestUserSnapshot()
{
    return c64.latestUserSnapshot();
}

void 
VirtualC64::C64API::loadSnapshot(const Snapshot &snapshot)
{
    c64.loadSnapshot(snapshot);
}

void VirtualC64::C64API::loadRom(const string &path)
{
    c64.loadRom(path);
    c64.markAsDirty();
}

void VirtualC64::C64API::loadRom(const RomFile &file)
{
    c64.loadRom(file);
    c64.markAsDirty();
}

void VirtualC64::C64API::deleteRom(RomType type)
{
    c64.deleteRom(type);
    c64.markAsDirty();
}

void VirtualC64::C64API::saveRom(RomType rom, const string &path)
{
    c64.saveRom(rom, path);
    c64.markAsDirty();
}

void VirtualC64::C64API::flash(const AnyFile &file)
{
    c64.flash(file);
    c64.markAsDirty();
}

void VirtualC64::C64API::flash(const AnyCollection &file, isize item)
{
    c64.flash(file, item);
    c64.markAsDirty();
}

void VirtualC64::C64API::flash(const FileSystem &fs, isize item)
{
    c64.flash(fs, item);
    c64.markAsDirty();
}


//
// CPU
//

CPUInfo
VirtualC64::CPUAPI::getInfo() const
{
    assert(isUserThread());
    return cpu.getState();
}

i64
VirtualC64::CPUAPI::clock() const
{
    assert(isUserThread());
    return cpu.clock;
}

u16
VirtualC64::CPUAPI::getPC0() const
{
    assert(isUserThread());
    return cpu.getPC0();
}

isize
VirtualC64::CPUAPI::loggedInstructions() const
{
    assert(isUserThread());
    return cpu.debugger.loggedInstructions();
}

u16
VirtualC64::CPUAPI::loggedPC0Rel(isize nr) const
{
    assert(isUserThread());
    return cpu.debugger.loggedPC0Rel(nr);
}

u16
VirtualC64::CPUAPI::loggedPC0Abs(isize nr) const
{
    assert(isUserThread());
    return cpu.debugger.loggedPC0Abs(nr);
}

RecordedInstruction
VirtualC64::CPUAPI::logEntryAbs(isize nr) const
{
    assert(isUserThread());
    return cpu.debugger.logEntryAbs(nr);
}

void
VirtualC64::CPUAPI::clearLog()
{
    assert(isUserThread());
    return cpu.debugger.clearLog();
}

void
VirtualC64::CPUAPI::setNumberFormat(DasmNumberFormat instrFormat, DasmNumberFormat dataFormat)
{
    assert(isUserThread());
    return cpu.disassembler.setNumberFormat(instrFormat, dataFormat);
}

isize
VirtualC64::CPUAPI::disassembleRecordedInstr(isize i, char *str) const
{
    assert(isUserThread());
    return cpu.debugger.disassembleRecordedInstr(i, str);
}

isize
VirtualC64::CPUAPI::disassembleRecordedBytes(isize i, char *str) const
{
    assert(isUserThread());
    return cpu.debugger.disassembleRecordedBytes(i, str);
}

void
VirtualC64::CPUAPI::disassembleRecordedFlags(isize i, char *str) const
{
    assert(isUserThread());
    return cpu.debugger.disassembleRecordedFlags(i, str);
}

void
VirtualC64::CPUAPI::disassembleRecordedPC(isize i, char *str) const
{
    assert(isUserThread());
    return cpu.debugger.disassembleRecordedPC(i, str);

}

isize
VirtualC64::CPUAPI::disassemble(char *str, u16 addr) const
{
    assert(isUserThread());
    return cpu.disassembler.disassemble(str, addr);
}

isize
VirtualC64::CPUAPI::getLengthOfInstructionAt(u16 addr) const
{
    assert(isUserThread());
    return cpu.getLengthOfInstructionAt(addr);
}

void
VirtualC64::CPUAPI::dumpBytes(char *str, u16 addr, isize length) const
{
    assert(isUserThread());
    return cpu.disassembler.dumpBytes(str, addr, length);
}

void
VirtualC64::CPUAPI::dumpWord(char *str, u16 addr) const
{
    assert(isUserThread());
    return cpu.disassembler.dumpWord(str, addr);
}


//
// Guards
//

long 
VirtualC64::GuardAPI::elements() const
{
    return guards.elements();
}

u32 
VirtualC64::GuardAPI::guardAddr(long nr) const
{
    return guards.guardAddr(nr);
}

bool 
VirtualC64::GuardAPI::isEnabled(long nr) const
{
    return guards.isEnabled(nr);
}

bool 
VirtualC64::GuardAPI::isDisabled(long nr) const
{
    return guards.isDisabled(nr);
}

bool 
VirtualC64::GuardAPI::isSetAt(u32 addr) const
{
    return guards.isSetAt(addr);
}

bool 
VirtualC64::GuardAPI::isSetAndEnabledAt(u32 addr) const
{
    return guards.isSetAndEnabledAt(addr);
}

bool 
VirtualC64::GuardAPI::isSetAndDisabledAt(u32 addr) const
{
    return guards.isSetAndDisabledAt(addr);
}

bool 
VirtualC64::GuardAPI::isSetAndConditionalAt(u32 addr) const
{
    return guards.isSetAndConditionalAt(addr);
}

void 
VirtualC64::GuardAPI::setEnable(long nr, bool val)
{
    guards.setEnable(nr, val);
}

void 
VirtualC64::GuardAPI::enable(long nr)
{
    guards.enable(nr);
}

void 
VirtualC64::GuardAPI::disable(long nr)
{
    guards.disable(nr);
}

void 
VirtualC64::GuardAPI::setEnableAt(u32 addr, bool val)
{
    guards.setEnableAt(addr, val);
}

void 
VirtualC64::GuardAPI::enableAt(u32 addr)
{
    guards.enableAt(addr);
}

void 
VirtualC64::GuardAPI::disableAt(u32 addr)
{
    guards.disableAt(addr);
}

void 
VirtualC64::GuardAPI::addAt(u32 addr, long skip)
{
    guards.addAt(addr, skip);
}

void 
VirtualC64::GuardAPI::removeAt(u32 addr)
{
    guards.removeAt(addr);
}

void 
VirtualC64::GuardAPI::remove(long nr)
{
    guards.remove(nr);
}

void 
VirtualC64::GuardAPI::removeAll()
{
    guards.removeAll();
}

void 
VirtualC64::GuardAPI::replace(long nr, u32 addr)
{
    guards.replace(nr, addr);
}


//
// Memory
//

MemConfig
VirtualC64::MemoryAPI::getConfig() const
{
    assert(isUserThread());
    return emulator.main.mem.getConfig();
}

MemInfo
VirtualC64::MemoryAPI::getInfo() const
{
    assert(isUserThread());
    return emulator.main.mem.getState();
}

string
VirtualC64::MemoryAPI::memdump(u16 addr, isize num, bool hex, isize pads, MemoryType src) const
{
    assert(isUserThread());
    return mem.memdump(addr, num, hex, pads, src);
}

string
VirtualC64::MemoryAPI::txtdump(u16 addr, isize num, MemoryType src) const
{
    assert(isUserThread());
    return mem.txtdump(addr, num, src);
}


//
// CIAs
//

CIAConfig
VirtualC64::CIAAPI::getConfig() const
{
    return cia.getConfig();
}

CIAInfo
VirtualC64::CIAAPI::getInfo() const
{
    return cia.getState();
}

CIAStats
VirtualC64::CIAAPI::getStats() const
{
    return cia.getStats();
}


//
// VICII
//

VICIIConfig
VirtualC64::VICIIAPI::getConfig() const
{
    return vic.getConfig();
}

VICIIInfo
VirtualC64::VICIIAPI::getInfo() const
{
    return vic.getState();
}

SpriteInfo
VirtualC64::VICIIAPI::getSpriteInfo(isize nr) const
{
    return vic.getSpriteInfo(nr);
}

isize
VirtualC64::VICIIAPI::getCyclesPerLine() const
{
    return vic.getCyclesPerLine();
}

isize
VirtualC64::VICIIAPI::getLinesPerFrame() const
{
    return vic.getLinesPerFrame();
}

bool
VirtualC64::VICIIAPI::pal() const
{
    return vic.pal();
}

u32
VirtualC64::VICIIAPI::getColor(isize nr) const
{
    return vic.getColor(nr);
}

u32
VirtualC64::VICIIAPI::getColor(isize nr, Palette palette) const
{
    return vic.getColor(nr, palette);
}

//
// SID
//

MuxerConfig
VirtualC64::SIDAPI::getConfig() const
{
    assert(isUserThread());
    return muxer.getConfig();
}

SIDInfo
VirtualC64::SIDAPI::getInfo(isize nr) const
{
    assert(isUserThread());
    return muxer.getInfo(nr);
}

VoiceInfo
VirtualC64::SIDAPI::getVoiceInfo(isize nr, isize voice) const
{
    assert(isUserThread());
    return muxer.getVoiceInfo(nr, voice);
}


MuxerStats
VirtualC64::SIDAPI::getStats() const
{
    assert(isUserThread());
    return muxer.getStats();
}

void
VirtualC64::SIDAPI::rampUp()
{
    assert(isUserThread());
    muxer.rampUp();
}

void
VirtualC64::SIDAPI::rampUp(float from)
{
    assert(isUserThread());
    muxer.rampUp(from);
}

void
VirtualC64::SIDAPI::rampDown()
{
    assert(isUserThread());
    muxer.rampDown();
}

void
VirtualC64::SIDAPI::copyMono(float *buffer, isize n)
{
    assert(isUserThread());
    muxer.copyMono(buffer, n);
}

void
VirtualC64::SIDAPI::copyStereo(float *left, float *right, isize n)
{
    assert(isUserThread());
    muxer.copyStereo(left, right, n);
}

void
VirtualC64::SIDAPI::copyInterleaved(float *buffer, isize n)
{
    assert(isUserThread());
    muxer.copyInterleaved(buffer, n);
}

float
VirtualC64::SIDAPI::draw(u32 *buffer, isize width, isize height,
                        float maxAmp, u32 color, isize sid) const
{
    assert(isUserThread());
    return muxer.draw(buffer, width, height, maxAmp, color, sid);
}


//
// DMA Debugger
//

DmaDebuggerConfig
VirtualC64::DmaDebuggerAPI::getConfig() const
{
    assert(isUserThread());
    return vic.dmaDebugger.getConfig();
}


//
// Keyboard
//

bool VirtualC64::KeyboardAPI::isPressed(C64Key key) const 
{
    return keyboard.isPressed(key);
}

void VirtualC64::KeyboardAPI::autoType(const string &text)
{
    keyboard.autoType(text);
    c64.markAsDirty();
}

void VirtualC64::KeyboardAPI::abortAutoTyping() 
{
    keyboard.abortAutoTyping();
    c64.markAsDirty();
}


//
// Joystick
//

void VirtualC64::JoystickAPI::trigger(GamePadAction event)
{
    joystick.trigger(event);
    c64.markAsDirty();
}


//
// Datasette
//

DatasetteInfo 
VirtualC64::DatasetteAPI::getInfo() const
{
    return datasette.getInfo();
}

void 
VirtualC64::DatasetteAPI::insertTape(TAPFile &file)
{
    datasette.insertTape(file);
    c64.markAsDirty();
}

void 
VirtualC64::DatasetteAPI::ejectTape()
{
    datasette.ejectTape();
    c64.markAsDirty();
}


//
// Mouse
//

bool VirtualC64::MouseAPI::detectShakeXY(double x, double y)
{
    return mouse.detectShakeXY(x, y);
}

bool VirtualC64::MouseAPI::detectShakeDxDy(double dx, double dy)
{
    return mouse.detectShakeDxDy(dx, dy);
}

void VirtualC64::MouseAPI::setXY(double x, double y)
{
    mouse.setXY(x, y);
    c64.markAsDirty();
}

void VirtualC64::MouseAPI::setDxDy(double dx, double dy)
{
    mouse.setDxDy(dx, dy);
    c64.markAsDirty();
}

void VirtualC64::MouseAPI::trigger(GamePadAction event)
{
    mouse.trigger(event);
    c64.markAsDirty();
}


//
// Recorder
//

const string
VirtualC64::RecorderAPI::getExecPath() const
{
    return FFmpeg::getExecPath();
}

void VirtualC64::RecorderAPI::setExecPath(const string &path)
{
    FFmpeg::setExecPath(path);
}

bool
VirtualC64::RecorderAPI::available() const
{
    return FFmpeg::available();
}

util::Time
VirtualC64::RecorderAPI::getDuration() const
{
    return recorder.getDuration();
}

isize
VirtualC64::RecorderAPI::getFrameRate() const
{
    return recorder.getFrameRate();
}

isize VirtualC64::RecorderAPI::getBitRate() const
{
    return recorder.getBitRate();
}

isize VirtualC64::RecorderAPI::getSampleRate() const
{
    return recorder.getSampleRate();
}

bool
VirtualC64::RecorderAPI::isRecording() const
{
    return recorder.isRecording();
}

void
VirtualC64::RecorderAPI::startRecording(isize x1, isize y1, isize x2, isize y2,
                                  isize bitRate,
                                  isize aspectX, isize aspectY)
{
    recorder.startRecording(x1, y1, x2, y2, bitRate, aspectX, aspectY);
}

void 
VirtualC64::RecorderAPI::stopRecording()
{
    recorder.stopRecording();
}

bool 
VirtualC64::RecorderAPI::exportAs(const string &path)
{
    return recorder.exportAs(path);
}


const char *
VirtualC64::RetroShellAPI::text()
{
    return retroShell.text();
}

isize
VirtualC64::RetroShellAPI::cursorRel()
{
    return retroShell.cursorRel();
}

void
VirtualC64::RetroShellAPI::press(RetroShellKey key, bool shift)
{
    retroShell.press(key, shift); 
}

void 
VirtualC64::RetroShellAPI::press(char c)
{
    retroShell.press(c);
}

void 
VirtualC64::RetroShellAPI::press(const string &s)
{
    retroShell.press(s);
}

void 
VirtualC64::RetroShellAPI::execScript(std::stringstream &ss)
{
    retroShell.execScript(ss);
}

void
VirtualC64::RetroShellAPI::execScript(const std::ifstream &fs)
{
    retroShell.execScript(fs);
}

void
VirtualC64::RetroShellAPI::execScript(const string &contents)
{
    retroShell.execScript(contents);
}

void
VirtualC64::RetroShellAPI::setStream(std::ostream &os)
{
    retroShell.setStream(os);
}


//
// Expansion port
//

CartridgeTraits 
VirtualC64::ExpansionPortAPI::getTraits() const
{
    return expansionport.getTraits();
}

CartridgeInfo
VirtualC64::ExpansionPortAPI::getInfo() const
{
    return expansionport.getInfo();
}

CartridgeRomInfo 
VirtualC64::ExpansionPortAPI::getRomInfo(isize nr) const
{
    return expansionport.getRomInfo(nr);
}

void
VirtualC64::ExpansionPortAPI::attachCartridge(const string &path, bool reset)
{
    expansionport.attachCartridge(path, reset);
    c64.markAsDirty();
}

void 
VirtualC64::ExpansionPortAPI::attachCartridge(const CRTFile &c, bool reset)
{
    expansionport.attachCartridge(c, reset);
    c64.markAsDirty();
}

void 
VirtualC64::ExpansionPortAPI::attachCartridge(Cartridge *c)
{
    expansionport.attachCartridge(c);
    c64.markAsDirty();
}

void 
VirtualC64::ExpansionPortAPI::attachReu(isize capacity)
{
    expansionport.attachReu(capacity);
    c64.markAsDirty();
}

void 
VirtualC64::ExpansionPortAPI::attachGeoRam(isize capacity)
{
    expansionport.attachGeoRam(capacity);
    c64.markAsDirty();
}

void 
VirtualC64::ExpansionPortAPI::attachIsepicCartridge()
{
    expansionport.attachIsepicCartridge();
    c64.markAsDirty();
}

void 
VirtualC64::ExpansionPortAPI::detachCartridge()
{
    expansionport.detachCartridge();
    c64.markAsDirty();
}


//
// Drive
//

const DriveConfig &
VirtualC64::DriveAPI::getConfig() const
{
    return drive.getConfig();
}

DriveInfo
VirtualC64::DriveAPI::getInfo() const
{
    return drive.getInfo();
}

void
VirtualC64::DriveAPI::insertBlankDisk(DOSType fstype, PETName<16> name)
{
    drive.insertNewDisk(fstype, name);
    c64.markAsDirty();
}

void
VirtualC64::DriveAPI::insertD64(const D64File &d64, bool wp)
{
    drive.insertD64(d64, wp);
    c64.markAsDirty();
}

void
VirtualC64::DriveAPI::insertG64(const G64File &g64, bool wp)
{
    drive.insertG64(g64, wp);
    c64.markAsDirty();
}

void
VirtualC64::DriveAPI::insertCollection(AnyCollection &archive, bool wp)
{
    drive.insertCollection(archive, wp);
    c64.markAsDirty();
}

void
VirtualC64::DriveAPI::insertFileSystem(const class FileSystem &device, bool wp)
{
    drive.insertFileSystem(device, wp);
    c64.markAsDirty();
}

void
VirtualC64::DriveAPI::ejectDisk()
{
    drive.ejectDisk();
    c64.markAsDirty();
}
