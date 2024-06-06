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
#include "Emulator.h"

namespace vc64 {

const Defaults &
VirtualC64::defaults = Emulator::defaults;

void 
API::suspend()
{
    emu->suspend();
}

void 
API::resume()
{
    emu->resume();
}

bool 
API::isUserThread() const
{
    return !emu->isEmulatorThread();
}

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

VirtualC64::VirtualC64() {

    emu = new Emulator();

    c64.emu = emu;
    c64.c64 = &emu->main;

    mem.emu = emu;
    mem.mem = &emu->main.mem;

    cpu.emu = emu;
    cpu.cpu = &emu->main.cpu;

    cia1.emu = emu;
    cia1.cia = &emu->main.cia1;

    cia2.emu = emu;
    cia2.cia = &emu->main.cia2;

    vicii.emu = emu;
    vicii.vicii = &emu->main.vic;

    sidBridge.emu = emu;
    sidBridge.sidBridge = &emu->main.sidBridge;

    audioPort.emu = emu;
    audioPort.audioPort = &emu->main.audioPort;

    videoPort.emu = emu;
    videoPort.videoPort = &emu->main.videoPort;

    dmaDebugger.emu = emu;
    dmaDebugger.dmaDebugger = &emu->main.vic.dmaDebugger;

    keyboard.emu =emu;
    keyboard.keyboard = &emu->main.keyboard;

    datasette.emu = emu;
    datasette.datasette = &emu->main.datasette;

    port1.emu = emu;
    port1.controlPort = &emu->main.port1;
    port1.mouse.mouse = &emu->main.port1.mouse;
    port1.joystick.joystick = &emu->main.port1.joystick;
    port1.paddle.paddle = &emu->main.port1.paddle;

    port2.emu = emu;
    port2.controlPort = &emu->main.port2;
    port2.mouse.mouse = &emu->main.port2.mouse;
    port2.joystick.joystick = &emu->main.port2.joystick;
    port2.paddle.paddle = &emu->main.port2.paddle;

    recorder.emu = emu;
    recorder.recorder = &emu->main.recorder;

    expansionport.emu = emu;
    expansionport.expansionPort = &emu->main.expansionport;

    iec.emu = emu;
    iec.serialPort = &emu->main.iec;

    drive8.emu = emu;
    drive8.drive = &emu->main.drive8;
    drive8.disk.drive = &emu->main.drive8;

    drive9.emu = emu;
    drive9.drive = &emu->main.drive9;
    drive9.disk.drive = &emu->main.drive9;

    retroShell.emu = emu;
    retroShell.retroShell = &emu->main.retroShell;
}

VirtualC64::~VirtualC64()
{
    emu->halt();
    delete emu;
}

const EmulatorConfig &
VirtualC64::getConfig() const 
{
    return emu->getConfig();
}

const EmulatorInfo &
VirtualC64::getInfo() const 
{
    return emu->getInfo();
}

const EmulatorInfo &
VirtualC64::getCachedInfo() const
{
    return emu->getCachedInfo();
}

const EmulatorStats &
VirtualC64::getStats() const 
{
    return emu->getStats();
}

bool
VirtualC64::isPoweredOn() 
{
    return emu->isPoweredOn();
}

bool
VirtualC64::isPoweredOff()
{
    return emu->isPoweredOff();
}

bool
VirtualC64::isPaused()
{
    return emu->isPaused();
}

bool
VirtualC64::isRunning() 
{
    return emu->isRunning();
}

bool
VirtualC64::isSuspended()
{
    return emu->isSuspended();
}

bool
VirtualC64::isHalted()
{
    return emu->isHalted();
}

bool
VirtualC64::isWarping() 
{
    return emu->isWarping();
}

bool
VirtualC64::isTracking() 
{
    return emu->isTracking();
}

void 
VirtualC64::isReady() 
{
    return emu->isReady();
}

void 
VirtualC64::powerOn() 
{
    emu->Thread::powerOn();
}

void 
VirtualC64::powerOff() 
{
    emu->Thread::powerOff();
}

void 
VirtualC64::run() 
{
    emu->run();
}

void 
VirtualC64::pause() 
{
    emu->pause();
}

void 
VirtualC64::halt() 
{
    emu->halt();
}

void 
VirtualC64::suspend() 
{
    emu->suspend();
}

void 
VirtualC64::resume() 
{
    emu->resume();
}

void 
VirtualC64::warpOn(isize source)
{
    emu->warpOn(source);
}

void 
VirtualC64::warpOff(isize source)
{
    emu->warpOff(source);
}

void 
VirtualC64::trackOn(isize source)
{
    emu->trackOn(source);
}

void 
VirtualC64::trackOff(isize source)
{
    emu->trackOff(source);
}

void
VirtualC64::stepInto()
{
    assert(isUserThread());
    emu->stepInto();
}

void
VirtualC64::stepOver()
{
    assert(isUserThread());
    emu->stepOver();
}

void 
VirtualC64::wakeUp()
{
    emu->wakeUp();
}

void
VirtualC64::launch(const void *listener, Callback *func)
{
    assert(isUserThread());
    emu->launch(listener, func);
}

i64
VirtualC64::get(Option option) const
{
    assert(isUserThread());
    return emu->get(option);
}

i64
VirtualC64::get(Option option, long id) const
{
    assert(isUserThread());
    return emu->get(option, id);
}

void
VirtualC64::set(C64Model model)
{
    assert(isUserThread());
    emu->set(model);
    emu->main.markAsDirty();
}

void
VirtualC64::set(Option opt, i64 value) throws
{
    assert(isUserThread());

    emu->check(opt, value);
    put(CMD_CONFIG, ConfigCmd { .option = opt, .value = value, .id = -1 });
    emu->main.markAsDirty();
}

void
VirtualC64::set(Option opt, i64 value, long id)
{
    assert(isUserThread());

    emu->check(opt, value, id);
    put(CMD_CONFIG, ConfigCmd { .option = opt, .value = value, .id = id });
    emu->main.markAsDirty();
}

void
VirtualC64::exportConfig(const fs::path &path) const
{
    assert(isUserThread());
    emu->main.exportConfig(path);
}

void
VirtualC64::exportConfig(std::ostream& stream) const
{
    assert(isUserThread());
    emu->main.exportConfig(stream);
}

void
VirtualC64::put(const Cmd &cmd)
{
    assert(isUserThread());
    emu->put(cmd);
}


//
// C64
//

void
VirtualC64::C64API::hardReset()
{
    assert(isUserThread());

    suspend();

    c64->hardReset();
    c64->markAsDirty();

    resume();
}

void
VirtualC64::C64API::softReset()
{
    assert(isUserThread());

    suspend();

    c64->hardReset();
    c64->markAsDirty();

    resume();
}

InspectionTarget
VirtualC64::C64API::getInspectionTarget() const
{
    return c64->getInspectionTarget();
}

void
VirtualC64::C64API::setInspectionTarget(InspectionTarget target)
{
    c64->emulator.put(CMD_INSPECTION_TARGET, target);
}

void
VirtualC64::C64API::removeInspectionTarget()
{
    c64->emulator.put(CMD_INSPECTION_TARGET, INSPECTION_NONE);
}

const C64Info &
VirtualC64::C64API::getInfo() const
{
    return c64->getInfo();
}

const C64Info &
VirtualC64::C64API::getCachedInfo() const
{
    return c64->getCachedInfo();
}

EventSlotInfo
VirtualC64::C64API::getSlotInfo(isize nr) const
{
    return c64->getSlotInfo(nr);
}

RomTraits
VirtualC64::C64API::getRomTraits(RomType type) const
{
    return c64->getRomTraits(type);
}

MediaFile *
VirtualC64::C64API::takeSnapshot()
{
    return c64->takeSnapshot();
}

void
VirtualC64::C64API::loadSnapshot(const MediaFile &snapshot)
{
    c64->loadSnapshot(snapshot);
    c64->markAsDirty();
}

void
VirtualC64::C64API::loadRom(const string &path)
{
    c64->loadRom(path);
    c64->markAsDirty();
}

void 
VirtualC64::C64API::loadRom(const RomFile &file)
{
    c64->loadRom(file);
    c64->markAsDirty();
}

void 
VirtualC64::C64API::deleteRom(RomType type)
{
    c64->deleteRom(type);
    c64->markAsDirty();
}

void 
VirtualC64::C64API::saveRom(RomType rom, const string &path)
{
    c64->saveRom(rom, path);
    c64->markAsDirty();
}

void 
VirtualC64::C64API::flash(const AnyFile &file)
{
    c64->flash(file);
    c64->markAsDirty();
}

void 
VirtualC64::C64API::flash(const AnyCollection &file, isize item)
{
    c64->flash(file, item);
    c64->markAsDirty();
}

void 
VirtualC64::C64API::flash(const FileSystem &fs, isize item)
{
    c64->flash(fs, item);
    c64->markAsDirty();
}


//
// CPU
//

const CPUInfo &
VirtualC64::CPUAPI::getInfo() const
{
    assert(isUserThread());
    return cpu->getInfo();
}

const CPUInfo &
VirtualC64::CPUAPI::getCachedInfo() const
{
    assert(isUserThread());
    return cpu->getCachedInfo();
}

isize
VirtualC64::CPUAPI::loggedInstructions() const
{
    assert(isUserThread());
    return cpu->debugger.loggedInstructions();
}

void
VirtualC64::CPUAPI::clearLog()
{
    assert(isUserThread());
    return cpu->debugger.clearLog();
}

void
VirtualC64::CPUAPI::setNumberFormat(DasmNumberFormat instrFormat, DasmNumberFormat dataFormat)
{
    assert(isUserThread());
    return cpu->disassembler.setNumberFormat(instrFormat, dataFormat);
}

isize
VirtualC64::CPUAPI::disassemble(char *dst, const char *fmt, u16 addr) const
{
    assert(isUserThread());
    return cpu->disassembler.disass(dst, fmt, addr);
}

isize
VirtualC64::CPUAPI::disassembleRecorded(char *dst, const char *fmt, isize nr) const
{
    assert(isUserThread());
    return cpu->debugger.disassRecorded(dst, fmt, nr);
}

Guard *
VirtualC64::CPUAPI::breakpointNr(long nr) const
{
    return cpu->debugger.breakpoints.guardWithNr(nr);
}

Guard *
VirtualC64::CPUAPI::breakpointAt(u32 addr) const
{
    return cpu->debugger.breakpoints.guardAtAddr(addr);
}

Guard *
VirtualC64::CPUAPI::watchpointNr(long nr) const
{
    return cpu->debugger.watchpoints.guardWithNr(nr);
}

Guard *
VirtualC64::CPUAPI::watchpointAt(u32 addr) const
{
    return cpu->debugger.watchpoints.guardAtAddr(addr);
}


//
// Memory
//

const MemConfig &
VirtualC64::MemoryAPI::getConfig() const
{
    assert(isUserThread());
    return mem->getConfig();
}

const MemInfo &
VirtualC64::MemoryAPI::getInfo() const
{
    assert(isUserThread());
    return mem->getInfo();
}

const MemInfo &
VirtualC64::MemoryAPI::getCachedInfo() const
{
    assert(isUserThread());
    return mem->getCachedInfo();
}

string
VirtualC64::MemoryAPI::memdump(u16 addr, isize num, bool hex, isize pads, MemoryType src) const
{
    assert(isUserThread());
    return mem->memdump(addr, num, hex, pads, src);
}

string
VirtualC64::MemoryAPI::txtdump(u16 addr, isize num, MemoryType src) const
{
    assert(isUserThread());
    return mem->txtdump(addr, num, src);
}

void
VirtualC64::MemoryAPI::drawHeatmap(u32 *buffer, isize width, isize height) const
{
    return mem->heatmap.draw(buffer, width, height);
}


//
// CIAs
//

const CIAConfig &
VirtualC64::CIAAPI::getConfig() const
{
    return cia->getConfig();
}

const CIAInfo &
VirtualC64::CIAAPI::getInfo() const
{
    return cia->getInfo();
}

const CIAInfo &
VirtualC64::CIAAPI::getCachedInfo() const
{
    return cia->getCachedInfo();
}

CIAStats
VirtualC64::CIAAPI::getStats() const
{
    return cia->getStats();
}


//
// VICII
//

const VICIITraits &
VirtualC64::VICIIAPI::getTraits() const
{
    return vicii->getTraits();
}

const VICIIConfig &
VirtualC64::VICIIAPI::getConfig() const
{
    return vicii->getConfig();
}

const VICIIInfo &
VirtualC64::VICIIAPI::getInfo() const
{
    return vicii->getInfo();
}

const VICIIInfo &
VirtualC64::VICIIAPI::getCachedInfo() const
{
    return vicii->getCachedInfo();
}

SpriteInfo
VirtualC64::VICIIAPI::getSpriteInfo(isize nr) const
{
    return vicii->getSpriteInfo(nr);
}

u32
VirtualC64::VICIIAPI::getColor(isize nr) const
{
    return vicii->getColor(nr);
}

u32
VirtualC64::VICIIAPI::getColor(isize nr, Palette palette) const
{
    return vicii->getColor(nr, palette);
}


//
// SID
//

SIDInfo
VirtualC64::SIDAPI::getInfo(isize nr) const
{
    assert(nr < 3);

    assert(isUserThread());
    return sidBridge->sid[nr].getInfo();
}

SIDInfo
VirtualC64::SIDAPI::getCachedInfo(isize nr) const
{
    assert(nr < 3);

    assert(isUserThread());
    return sidBridge->sid[nr].getCachedInfo();
}

float
VirtualC64::SIDAPI::draw(u32 *buffer, isize width, isize height,
                         float maxAmp, u32 color, isize sid) const
{
    assert(isUserThread());
    return sidBridge->draw(buffer, width, height, maxAmp, color, sid);
}


//
// Audio port
//

AudioPortStats
VirtualC64::AudioPortAPI::getStats() const
{
    assert(isUserThread());
    return audioPort->getStats();
}

void
VirtualC64::AudioPortAPI::copyMono(float *buffer, isize n)
{
    assert(isUserThread());
    audioPort->copyMono(buffer, n);
}

void
VirtualC64::AudioPortAPI::copyStereo(float *left, float *right, isize n)
{
    assert(isUserThread());
    audioPort->copyStereo(left, right, n);
}

void
VirtualC64::AudioPortAPI::copyInterleaved(float *buffer, isize n)
{
    assert(isUserThread());
    audioPort->copyInterleaved(buffer, n);
}


//
// Video port
//

u32 *
VirtualC64::VideoPortAPI::getTexture() const
{
    return emu->getTexture();
}

u32 *
VirtualC64::VideoPortAPI::getDmaTexture() const
{
    return emu->getDmaTexture();
}


//
// DMA Debugger
//

const DmaDebuggerConfig &
VirtualC64::DmaDebuggerAPI::getConfig() const
{
    assert(isUserThread());
    return dmaDebugger->getConfig();
}


//
// Keyboard
//

bool VirtualC64::KeyboardAPI::isPressed(C64Key key) const
{
    return keyboard->isPressed(key);
}

void VirtualC64::KeyboardAPI::autoType(const string &text)
{
    keyboard->autoType(text);
    keyboard->markAsDirty();
}

void VirtualC64::KeyboardAPI::abortAutoTyping()
{
    keyboard->abortAutoTyping();
    keyboard->markAsDirty();
}


//
// Joystick
//

const JoystickInfo &
VirtualC64::JoystickAPI::getInfo() const
{
    return joystick->getInfo();
}

const JoystickInfo &
VirtualC64::JoystickAPI::getCachedInfo() const
{
    return joystick->getCachedInfo();
}


//
// Paddle
//

const PaddleInfo &
VirtualC64::PaddleAPI::getInfo() const
{
    return paddle->getInfo();
}

const PaddleInfo &
VirtualC64::PaddleAPI::getCachedInfo() const
{
    return paddle->getCachedInfo();
}


//
// Datasette
//

const DatasetteInfo &
VirtualC64::DatasetteAPI::getInfo() const
{
    return datasette->getInfo();
}

const DatasetteInfo &
VirtualC64::DatasetteAPI::getCachedInfo() const
{
    return datasette->getCachedInfo();
}

void
VirtualC64::DatasetteAPI::insertTape(TAPFile &file)
{
    datasette->insertTape(file);
    datasette->markAsDirty();
}

void
VirtualC64::DatasetteAPI::ejectTape()
{
    datasette->ejectTape();
    datasette->markAsDirty();
}


//
// Mouse
//

bool VirtualC64::MouseAPI::detectShakeXY(double x, double y)
{
    return mouse->detectShakeXY(x, y);
}

bool VirtualC64::MouseAPI::detectShakeDxDy(double dx, double dy)
{
    return mouse->detectShakeDxDy(dx, dy);
}


//
// Recorder
//

const RecorderConfig &
VirtualC64::RecorderAPI::getConfig() const
{
    return recorder->getConfig();
}

const RecorderInfo &
VirtualC64::RecorderAPI::getInfo() const
{
    return recorder->getInfo();
}

const RecorderInfo &
VirtualC64::RecorderAPI::getCachedInfo() const
{
    return recorder->getCachedInfo();
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
    recorder->startRecording(x1, y1, x2, y2);
}

void
VirtualC64::RecorderAPI::stopRecording()
{
    recorder->stopRecording();
}

bool
VirtualC64::RecorderAPI::exportAs(const string &path)
{
    return recorder->exportAs(path);
}


const char *
VirtualC64::RetroShellAPI::text()
{
    return retroShell->text();
}

isize
VirtualC64::RetroShellAPI::cursorRel()
{
    return retroShell->cursorRel();
}

void
VirtualC64::RetroShellAPI::press(RetroShellKey key, bool shift)
{
    retroShell->press(key, shift);
}

void
VirtualC64::RetroShellAPI::press(char c)
{
    retroShell->press(c);
}

void
VirtualC64::RetroShellAPI::press(const string &s)
{
    retroShell->press(s);
}

void
VirtualC64::RetroShellAPI::execScript(std::stringstream &ss)
{
    retroShell->execScript(ss);
}

void
VirtualC64::RetroShellAPI::execScript(const std::ifstream &fs)
{
    retroShell->execScript(fs);
}

void
VirtualC64::RetroShellAPI::execScript(const string &contents)
{
    retroShell->execScript(contents);
}

void
VirtualC64::RetroShellAPI::setStream(std::ostream &os)
{
    retroShell->setStream(os);
}


//
// Expansion port
//

CartridgeTraits
VirtualC64::ExpansionPortAPI::getCartridgeTraits() const
{
    return expansionPort->getCartridgeTraits();
}

const CartridgeInfo &
VirtualC64::ExpansionPortAPI::getInfo() const
{
    return expansionPort->getInfo();
}

const CartridgeInfo &
VirtualC64::ExpansionPortAPI::getCachedInfo() const
{
    return expansionPort->getCachedInfo();
}

CartridgeRomInfo
VirtualC64::ExpansionPortAPI::getRomInfo(isize nr) const
{
    return expansionPort->getRomInfo(nr);
}

void
VirtualC64::ExpansionPortAPI::attachCartridge(const string &path, bool reset)
{
    expansionPort->attachCartridge(path, reset);
    expansionPort->markAsDirty();
}

void
VirtualC64::ExpansionPortAPI::attachCartridge(const CRTFile &c, bool reset)
{
    expansionPort->attachCartridge(c, reset);
    expansionPort->markAsDirty();
}

/*
void
VirtualC64::ExpansionPortAPI::attachCartridge(Cartridge *c)
{
    expansionPort->attachCartridge(c);
    expansionPort->markAsDirty();
}
*/

void
VirtualC64::ExpansionPortAPI::attachReu(isize capacity)
{
    expansionPort->attachReu(capacity);
    expansionPort->markAsDirty();
}

void
VirtualC64::ExpansionPortAPI::attachGeoRam(isize capacity)
{
    expansionPort->attachGeoRam(capacity);
    expansionPort->markAsDirty();
}

void
VirtualC64::ExpansionPortAPI::attachIsepicCartridge()
{
    expansionPort->attachIsepicCartridge();
    expansionPort->markAsDirty();
}

void
VirtualC64::ExpansionPortAPI::detachCartridge()
{
    expansionPort->detachCartridge();
    expansionPort->markAsDirty();
}


//
// Disk
//

Disk *
VirtualC64::DiskAPI::get()
{
    return drive->disk.get();
}


//
// Drive
//

const DriveConfig &
VirtualC64::DriveAPI::getConfig() const
{
    return drive->getConfig();
}

const DriveInfo &
VirtualC64::DriveAPI::getInfo() const
{
    return drive->getInfo();
}

const DriveInfo &
VirtualC64::DriveAPI::getCachedInfo() const
{
    return drive->getCachedInfo();
}

void
VirtualC64::DriveAPI::insertBlankDisk(DOSType fstype, PETName<16> name)
{
    drive->insertNewDisk(fstype, name);
    drive->markAsDirty();
}

void
VirtualC64::DriveAPI::insertD64(const D64File &d64, bool wp)
{
    drive->insertD64(d64, wp);
    drive->markAsDirty();
}

void
VirtualC64::DriveAPI::insertG64(const G64File &g64, bool wp)
{
    drive->insertG64(g64, wp);
    drive->markAsDirty();
}

void
VirtualC64::DriveAPI::insertCollection(AnyCollection &archive, bool wp)
{
    drive->insertCollection(archive, wp);
    drive->markAsDirty();
}

void
VirtualC64::DriveAPI::insertFileSystem(const class FileSystem &device, bool wp)
{
    drive->insertFileSystem(device, wp);
    drive->markAsDirty();
}

void
VirtualC64::DriveAPI::ejectDisk()
{
    drive->ejectDisk();
    drive->markAsDirty();
}

}
