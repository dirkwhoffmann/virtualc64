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

namespace vc64 {

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
sidBridge(*this),
audioPort(*this),
videoPort(*this),
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
VirtualC64::set(Option opt, i64 value) throws
{
    Emulator::check(opt, value);
    put(CMD_CONFIG, ConfigCmd { .option = opt, .value = value, .id = -1 });
    main.markAsDirty();
}

void
VirtualC64::set(Option opt, i64 value, long id)
{
    Emulator::check(opt, value, id);
    put(CMD_CONFIG, ConfigCmd { .option = opt, .value = value, .id = id });
    main.markAsDirty();
}

void
VirtualC64::exportConfig(const fs::path &path) const
{
    main.exportConfig(path);
}

void
VirtualC64::exportConfig(std::ostream& stream) const
{
    main.exportConfig(stream);
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
    c64.emulator.put(CMD_INSPECTION_TARGET, target);
}

void
VirtualC64::C64API::removeInspectionTarget()
{
    c64.emulator.put(CMD_INSPECTION_TARGET, INSPECTION_NONE);
}

RomTraits
VirtualC64::C64API::getRomTraits(RomType type) const
{
    return c64.getRomTraits(type);
}

Snapshot *
VirtualC64::C64API::takeSnapshot()
{
    return c64.takeSnapshot();
}

void
VirtualC64::C64API::loadSnapshot(const Snapshot &snapshot)
{
    c64.loadSnapshot(snapshot);
    c64.markAsDirty();
}

void
VirtualC64::C64API::loadRom(const string &path)
{
    c64.loadRom(path);
    c64.markAsDirty();
}

void 
VirtualC64::C64API::loadRom(const RomFile &file)
{
    c64.loadRom(file);
    c64.markAsDirty();
}

void 
VirtualC64::C64API::deleteRom(RomType type)
{
    c64.deleteRom(type);
    c64.markAsDirty();
}

void 
VirtualC64::C64API::saveRom(RomType rom, const string &path)
{
    c64.saveRom(rom, path);
    c64.markAsDirty();
}

void 
VirtualC64::C64API::flash(const AnyFile &file)
{
    c64.flash(file);
    c64.markAsDirty();
}

void 
VirtualC64::C64API::flash(const AnyCollection &file, isize item)
{
    c64.flash(file, item);
    c64.markAsDirty();
}

void 
VirtualC64::C64API::flash(const FileSystem &fs, isize item)
{
    c64.flash(fs, item);
    c64.markAsDirty();
}


//
// CPU
//

const CPUInfo &
VirtualC64::CPUAPI::getInfo() const
{
    assert(isUserThread());
    return cpu.getInfo();
}

const CPUInfo &
VirtualC64::CPUAPI::getCachedInfo() const
{
    assert(isUserThread());
    return cpu.getCachedInfo();
}

isize
VirtualC64::CPUAPI::loggedInstructions() const
{
    assert(isUserThread());
    return cpu.debugger.loggedInstructions();
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
VirtualC64::CPUAPI::disassemble(char *dst, const char *fmt, u16 addr) const
{
    assert(isUserThread());
    return cpu.disassembler.disass(dst, fmt, addr);
}

isize
VirtualC64::CPUAPI::disassembleRecorded(char *dst, const char *fmt, isize nr) const
{
    assert(isUserThread());
    return cpu.debugger.disassRecorded(dst, fmt, nr);
}

Guard *
VirtualC64::CPUAPI::breakpointNr(long nr) const
{
    return cpu.debugger.breakpoints.guardWithNr(nr);
}

Guard *
VirtualC64::CPUAPI::breakpointAt(u32 addr) const
{
    return cpu.debugger.breakpoints.guardAtAddr(addr);
}

Guard *
VirtualC64::CPUAPI::watchpointNr(long nr) const
{
    return cpu.debugger.watchpoints.guardWithNr(nr);
}

Guard *
VirtualC64::CPUAPI::watchpointAt(u32 addr) const
{
    return cpu.debugger.watchpoints.guardAtAddr(addr);
}


//
// Memory
//

const MemConfig &
VirtualC64::MemoryAPI::getConfig() const
{
    assert(isUserThread());
    return mem.getConfig();
}

const MemInfo &
VirtualC64::MemoryAPI::getInfo() const
{
    assert(isUserThread());
    return mem.getInfo();
}

const MemInfo &
VirtualC64::MemoryAPI::getCachedInfo() const
{
    assert(isUserThread());
    return mem.getCachedInfo();
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

void
VirtualC64::MemoryAPI::drawHeatmap(u32 *buffer, isize width, isize height) const
{
    return mem.heatmap.draw(buffer, width, height);
}


//
// CIAs
//

const CIAConfig &
VirtualC64::CIAAPI::getConfig() const
{
    return cia.getConfig();
}

const CIAInfo &
VirtualC64::CIAAPI::getInfo() const
{
    return cia.getInfo();
}

const CIAInfo &
VirtualC64::CIAAPI::getCachedInfo() const
{
    return cia.getCachedInfo();
}

CIAStats
VirtualC64::CIAAPI::getStats() const
{
    return cia.getStats();
}


//
// VICII
//

const VICIITraits &
VirtualC64::VICIIAPI::getTraits() const
{
    return vic.getTraits();
}

const VICIIConfig &
VirtualC64::VICIIAPI::getConfig() const
{
    return vic.getConfig();
}

const VICIIInfo &
VirtualC64::VICIIAPI::getInfo() const
{
    return vic.getInfo();
}

const VICIIInfo &
VirtualC64::VICIIAPI::getCachedInfo() const
{
    return vic.getCachedInfo();
}

SpriteInfo
VirtualC64::VICIIAPI::getSpriteInfo(isize nr) const
{
    return vic.getSpriteInfo(nr);
}

u32
VirtualC64::VICIIAPI::getColor(isize nr) const
{
    return vic.getColor(nr);
}

u32
VirtualC64::VICIIAPI::getColor(isize nr, Palette palette) const
{
    return monitor.getColor(nr, palette);
}


//
// SID
//

SIDInfo
VirtualC64::SIDAPI::getInfo(isize nr) const
{
    assert(nr < 3);

    assert(isUserThread());
    return sidBridge.sid[nr].getInfo();
}

SIDInfo
VirtualC64::SIDAPI::getCachedInfo(isize nr) const
{
    assert(nr < 3);

    assert(isUserThread());
    return sidBridge.sid[nr].getCachedInfo();
}

float
VirtualC64::SIDAPI::draw(u32 *buffer, isize width, isize height,
                         float maxAmp, u32 color, isize sid) const
{
    assert(isUserThread());
    return sidBridge.draw(buffer, width, height, maxAmp, color, sid);
}


//
// Audio port
//

AudioPortStats
VirtualC64::AudioPortAPI::getStats() const
{
    assert(isUserThread());
    return audioPort.getStats();
}

void
VirtualC64::AudioPortAPI::copyMono(float *buffer, isize n)
{
    assert(isUserThread());
    audioPort.copyMono(buffer, n);
}

void
VirtualC64::AudioPortAPI::copyStereo(float *left, float *right, isize n)
{
    assert(isUserThread());
    audioPort.copyStereo(left, right, n);
}

void
VirtualC64::AudioPortAPI::copyInterleaved(float *buffer, isize n)
{
    assert(isUserThread());
    audioPort.copyInterleaved(buffer, n);
}


//
// Video port
//

u32 *
VirtualC64::VideoPortAPI::getTexture() const
{
    return emulator.getTexture();
}

u32 *
VirtualC64::VideoPortAPI::getDmaTexture() const
{
    return emulator.getDmaTexture();
}


//
// DMA Debugger
//

const DmaDebuggerConfig &
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

const JoystickInfo &
VirtualC64::JoystickAPI::getInfo() const
{
    return joystick.getInfo();
}

const JoystickInfo &
VirtualC64::JoystickAPI::getCachedInfo() const
{
    return joystick.getCachedInfo();
}


//
// Paddle
//

const PaddleInfo &
VirtualC64::PaddleAPI::getInfo() const
{
    return paddle.getInfo();
}

const PaddleInfo &
VirtualC64::PaddleAPI::getCachedInfo() const
{
    return paddle.getCachedInfo();
}


//
// Datasette
//

const DatasetteInfo &
VirtualC64::DatasetteAPI::getInfo() const
{
    return datasette.getInfo();
}

const DatasetteInfo &
VirtualC64::DatasetteAPI::getCachedInfo() const
{
    return datasette.getCachedInfo();
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


//
// Recorder
//

const RecorderConfig &
VirtualC64::RecorderAPI::getConfig() const
{
    return recorder.getConfig();
}

const RecorderInfo &
VirtualC64::RecorderAPI::getInfo() const
{
    return recorder.getInfo();
}

const RecorderInfo &
VirtualC64::RecorderAPI::getCachedInfo() const
{
    return recorder.getCachedInfo();
}

const string
VirtualC64::RecorderAPI::getExecPath() const
{
    return FFmpeg::getExecPath();
}

void VirtualC64::RecorderAPI::setExecPath(const string &path)
{
    FFmpeg::setExecPath(path);
}

void
VirtualC64::RecorderAPI::startRecording(isize x1, isize y1, isize x2, isize y2)
{
    recorder.startRecording(x1, y1, x2, y2);
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
VirtualC64::ExpansionPortAPI::getCartridgeTraits() const
{
    return expansionPort.getCartridgeTraits();
}

const CartridgeInfo &
VirtualC64::ExpansionPortAPI::getInfo() const
{
    return expansionPort.getInfo();
}

const CartridgeInfo &
VirtualC64::ExpansionPortAPI::getCachedInfo() const
{
    return expansionPort.getCachedInfo();
}

CartridgeRomInfo
VirtualC64::ExpansionPortAPI::getRomInfo(isize nr) const
{
    return expansionPort.getRomInfo(nr);
}

void
VirtualC64::ExpansionPortAPI::attachCartridge(const string &path, bool reset)
{
    expansionPort.attachCartridge(path, reset);
    c64.markAsDirty();
}

void
VirtualC64::ExpansionPortAPI::attachCartridge(const CRTFile &c, bool reset)
{
    expansionPort.attachCartridge(c, reset);
    c64.markAsDirty();
}

void
VirtualC64::ExpansionPortAPI::attachCartridge(Cartridge *c)
{
    expansionPort.attachCartridge(c);
    c64.markAsDirty();
}

void
VirtualC64::ExpansionPortAPI::attachReu(isize capacity)
{
    expansionPort.attachReu(capacity);
    c64.markAsDirty();
}

void
VirtualC64::ExpansionPortAPI::attachGeoRam(isize capacity)
{
    expansionPort.attachGeoRam(capacity);
    c64.markAsDirty();
}

void
VirtualC64::ExpansionPortAPI::attachIsepicCartridge()
{
    expansionPort.attachIsepicCartridge();
    c64.markAsDirty();
}

void
VirtualC64::ExpansionPortAPI::detachCartridge()
{
    expansionPort.detachCartridge();
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

const DriveInfo &
VirtualC64::DriveAPI::getInfo() const
{
    return drive.getInfo();
}

const DriveInfo &
VirtualC64::DriveAPI::getCachedInfo() const
{
    return drive.getCachedInfo();
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

}
