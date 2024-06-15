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

DefaultsAPI VirtualC64::defaults(&Emulator::defaults);

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

    sid.emu = emu;
    sid.sidBridge = &emu->main.sidBridge;

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

    controlPort1.emu = emu;
    controlPort1.controlPort = &emu->main.port1;
    controlPort1.mouse.mouse = &emu->main.port1.mouse;
    controlPort1.joystick.joystick = &emu->main.port1.joystick;
    controlPort1.paddle.paddle = &emu->main.port1.paddle;

    controlPort2.emu = emu;
    controlPort2.controlPort = &emu->main.port2;
    controlPort2.mouse.mouse = &emu->main.port2.mouse;
    controlPort2.joystick.joystick = &emu->main.port2.joystick;
    controlPort2.paddle.paddle = &emu->main.port2.paddle;

    recorder.emu = emu;
    recorder.recorder = &emu->main.recorder;

    expansionPort.emu = emu;
    expansionPort.expansionPort = &emu->main.expansionport;

    serialPort.emu = emu;
    serialPort.serialPort = &emu->main.iec;

    drive8.emu = emu;
    drive8.drive = &emu->main.drive8;
    drive8.disk.drive = &emu->main.drive8;

    drive9.emu = emu;
    drive9.drive = &emu->main.drive9;
    drive9.disk.drive = &emu->main.drive9;

    retroShell.emu = emu;
    retroShell.retroShell = &emu->main.retroShell;

    defaults.emu = emu;
    defaults.defaults = &emu->defaults;
}

VirtualC64::~VirtualC64()
{
    delete emu;
    emu = nullptr;
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
C64API::hardReset()
{
    assert(isUserThread());

    suspend();

    c64->hardReset();
    c64->markAsDirty();

    resume();
}

void
C64API::softReset()
{
    assert(isUserThread());

    suspend();

    c64->hardReset();
    c64->markAsDirty();

    resume();
}

InspectionTarget
C64API::getInspectionTarget() const
{
    return c64->getInspectionTarget();
}

void
C64API::setInspectionTarget(InspectionTarget target)
{
    c64->emulator.put(CMD_INSPECTION_TARGET, target);
}

void
C64API::removeInspectionTarget()
{
    c64->emulator.put(CMD_INSPECTION_TARGET, INSPECTION_NONE);
}

const C64Info &
C64API::getInfo() const
{
    return c64->getInfo();
}

const C64Info &
C64API::getCachedInfo() const
{
    return c64->getCachedInfo();
}

EventSlotInfo
C64API::getSlotInfo(isize nr) const
{
    return c64->getSlotInfo(nr);
}

RomTraits
C64API::getRomTraits(RomType type) const
{
    return c64->getRomTraits(type);
}

MediaFile *
C64API::takeSnapshot()
{
    return c64->takeSnapshot();
}

void
C64API::loadSnapshot(const MediaFile &snapshot)
{
    c64->loadSnapshot(snapshot);
    c64->markAsDirty();
}

void
C64API::loadRom(const fs::path &path)
{
    c64->loadRom(path);
    c64->markAsDirty();
}

void 
C64API::loadRom(const MediaFile &file)
{
    c64->loadRom(file);
    c64->markAsDirty();
}

void 
C64API::deleteRom(RomType type)
{
    c64->deleteRom(type);
    c64->markAsDirty();
}

void 
C64API::saveRom(RomType rom, const std::filesystem::path &path)
{
    c64->saveRom(rom, path);
    c64->markAsDirty();
}

void 
C64API::flash(const MediaFile &file)
{
    c64->flash(file);
    c64->markAsDirty();
}

void 
C64API::flash(const MediaFile &file, isize item)
{
    c64->flash(file, item);
    c64->markAsDirty();
}

void 
C64API::flash(const FileSystem &fs, isize item)
{
    c64->flash(fs, item);
    c64->markAsDirty();
}


//
// CPU
//

const CPUInfo &
CPUAPI::getInfo() const
{
    assert(isUserThread());
    return cpu->getInfo();
}

const CPUInfo &
CPUAPI::getCachedInfo() const
{
    assert(isUserThread());
    return cpu->getCachedInfo();
}

isize
CPUAPI::loggedInstructions() const
{
    assert(isUserThread());
    return cpu->debugger.loggedInstructions();
}

void
CPUAPI::clearLog()
{
    assert(isUserThread());
    return cpu->debugger.clearLog();
}

void
CPUAPI::setNumberFormat(DasmNumberFormat instrFormat, DasmNumberFormat dataFormat)
{
    assert(isUserThread());
    return cpu->disassembler.setNumberFormat(instrFormat, dataFormat);
}

isize
CPUAPI::disassemble(char *dst, const char *fmt, u16 addr) const
{
    assert(isUserThread());
    return cpu->disassembler.disass(dst, fmt, addr);
}

isize
CPUAPI::disassembleRecorded(char *dst, const char *fmt, isize nr) const
{
    assert(isUserThread());
    return cpu->debugger.disassRecorded(dst, fmt, nr);
}

Guard *
CPUAPI::breakpointNr(long nr) const
{
    return cpu->debugger.breakpoints.guardWithNr(nr);
}

Guard *
CPUAPI::breakpointAt(u32 addr) const
{
    return cpu->debugger.breakpoints.guardAtAddr(addr);
}

Guard *
CPUAPI::watchpointNr(long nr) const
{
    return cpu->debugger.watchpoints.guardWithNr(nr);
}

Guard *
CPUAPI::watchpointAt(u32 addr) const
{
    return cpu->debugger.watchpoints.guardAtAddr(addr);
}


//
// Memory
//

const MemConfig &
MemoryAPI::getConfig() const
{
    assert(isUserThread());
    return mem->getConfig();
}

const MemInfo &
MemoryAPI::getInfo() const
{
    assert(isUserThread());
    return mem->getInfo();
}

const MemInfo &
MemoryAPI::getCachedInfo() const
{
    assert(isUserThread());
    return mem->getCachedInfo();
}

string
MemoryAPI::memdump(u16 addr, isize num, bool hex, isize pads, MemoryType src) const
{
    assert(isUserThread());
    return mem->memdump(addr, num, hex, pads, src);
}

string
MemoryAPI::txtdump(u16 addr, isize num, MemoryType src) const
{
    assert(isUserThread());
    return mem->txtdump(addr, num, src);
}

void
MemoryAPI::drawHeatmap(u32 *buffer, isize width, isize height) const
{
    return mem->heatmap.draw(buffer, width, height);
}


//
// CIAs
//

const CIAConfig &
CIAAPI::getConfig() const
{
    return cia->getConfig();
}

const CIAInfo &
CIAAPI::getInfo() const
{
    return cia->getInfo();
}

const CIAInfo &
CIAAPI::getCachedInfo() const
{
    return cia->getCachedInfo();
}

CIAStats
CIAAPI::getStats() const
{
    return cia->getStats();
}


//
// VICII
//

const VICIITraits &
VICIIAPI::getTraits() const
{
    return vicii->getTraits();
}

const VICIIConfig &
VICIIAPI::getConfig() const
{
    return vicii->getConfig();
}

const VICIIInfo &
VICIIAPI::getInfo() const
{
    return vicii->getInfo();
}

const VICIIInfo &
VICIIAPI::getCachedInfo() const
{
    return vicii->getCachedInfo();
}

SpriteInfo
VICIIAPI::getSpriteInfo(isize nr) const
{
    return vicii->getSpriteInfo(nr);
}

u32
VICIIAPI::getColor(isize nr) const
{
    return vicii->getColor(nr);
}

u32
VICIIAPI::getColor(isize nr, Palette palette) const
{
    return vicii->getColor(nr, palette);
}


//
// SID
//

SIDInfo
SIDAPI::getInfo(isize nr) const
{
    assert(nr < 3);

    assert(isUserThread());
    return sidBridge->sid[nr].getInfo();
}

SIDInfo
SIDAPI::getCachedInfo(isize nr) const
{
    assert(nr < 3);

    assert(isUserThread());
    return sidBridge->sid[nr].getCachedInfo();
}

float
SIDAPI::draw(u32 *buffer, isize width, isize height,
                         float maxAmp, u32 color, isize sid) const
{
    assert(isUserThread());
    return sidBridge->draw(buffer, width, height, maxAmp, color, sid);
}


//
// Audio port
//

AudioPortStats
AudioPortAPI::getStats() const
{
    assert(isUserThread());
    return audioPort->getStats();
}

isize
AudioPortAPI::copyMono(float *buffer, isize n)
{
    assert(isUserThread());
    return audioPort->copyMono(buffer, n);
}

isize
AudioPortAPI::copyStereo(float *left, float *right, isize n)
{
    assert(isUserThread());
    return audioPort->copyStereo(left, right, n);
}

isize
AudioPortAPI::copyInterleaved(float *buffer, isize n)
{
    assert(isUserThread());
    return audioPort->copyInterleaved(buffer, n);
}


//
// Video port
//

u32 *
VideoPortAPI::getTexture() const
{
    return emu->getTexture();
}

u32 *
VideoPortAPI::getDmaTexture() const
{
    return emu->getDmaTexture();
}


//
// DMA Debugger
//

const DmaDebuggerConfig &
DmaDebuggerAPI::getConfig() const
{
    assert(isUserThread());
    return dmaDebugger->getConfig();
}


//
// Keyboard
//

bool KeyboardAPI::isPressed(C64Key key) const
{
    return keyboard->isPressed(key);
}

void KeyboardAPI::autoType(const string &text)
{
    keyboard->autoType(text);
    keyboard->markAsDirty();
}

void KeyboardAPI::abortAutoTyping()
{
    keyboard->abortAutoTyping();
    keyboard->markAsDirty();
}


//
// Joystick
//

const JoystickInfo &
JoystickAPI::getInfo() const
{
    return joystick->getInfo();
}

const JoystickInfo &
JoystickAPI::getCachedInfo() const
{
    return joystick->getCachedInfo();
}


//
// Paddle
//

const PaddleInfo &
PaddleAPI::getInfo() const
{
    return paddle->getInfo();
}

const PaddleInfo &
PaddleAPI::getCachedInfo() const
{
    return paddle->getCachedInfo();
}


//
// Datasette
//

const DatasetteInfo &
DatasetteAPI::getInfo() const
{
    return datasette->getInfo();
}

const DatasetteInfo &
DatasetteAPI::getCachedInfo() const
{
    return datasette->getCachedInfo();
}

void
DatasetteAPI::insertTape(MediaFile &file)
{
    datasette->insertTape(file);
    datasette->markAsDirty();
}

void
DatasetteAPI::ejectTape()
{
    datasette->ejectTape();
    datasette->markAsDirty();
}


//
// Mouse
//

bool MouseAPI::detectShakeXY(double x, double y)
{
    return mouse->detectShakeXY(x, y);
}

bool MouseAPI::detectShakeDxDy(double dx, double dy)
{
    return mouse->detectShakeDxDy(dx, dy);
}


//
// Recorder
//

const RecorderConfig &
RecorderAPI::getConfig() const
{
    return recorder->getConfig();
}

const RecorderInfo &
RecorderAPI::getInfo() const
{
    return recorder->getInfo();
}

const RecorderInfo &
RecorderAPI::getCachedInfo() const
{
    return recorder->getCachedInfo();
}

const fs::path
RecorderAPI::getExecPath() const
{
    return FFmpeg::getExecPath();
}

void RecorderAPI::setExecPath(const std::filesystem::path &path)
{
    FFmpeg::setExecPath(path);
}

void
RecorderAPI::startRecording(isize x1, isize y1, isize x2, isize y2)
{
    recorder->startRecording(x1, y1, x2, y2);
}

void
RecorderAPI::stopRecording()
{
    recorder->stopRecording();
}

bool
RecorderAPI::exportAs(const std::filesystem::path &path)
{
    return recorder->exportAs(path);
}


const char *
RetroShellAPI::text()
{
    return retroShell->text();
}

isize
RetroShellAPI::cursorRel()
{
    return retroShell->cursorRel();
}

void
RetroShellAPI::press(RetroShellKey key, bool shift)
{
    retroShell->press(key, shift);
}

void
RetroShellAPI::press(char c)
{
    retroShell->press(c);
}

void
RetroShellAPI::press(const string &s)
{
    retroShell->press(s);
}

void
RetroShellAPI::execScript(std::stringstream &ss)
{
    retroShell->execScript(ss);
}

void
RetroShellAPI::execScript(const std::ifstream &fs)
{
    retroShell->execScript(fs);
}

void
RetroShellAPI::execScript(const string &contents)
{
    retroShell->execScript(contents);
}

void 
RetroShellAPI::execScript(const MediaFile &file)
{
    retroShell->execScript(file);
}

void
RetroShellAPI::setStream(std::ostream &os)
{
    retroShell->setStream(os);
}


//
// Expansion port
//

CartridgeTraits
ExpansionPortAPI::getCartridgeTraits() const
{
    return expansionPort->getCartridgeTraits();
}

const CartridgeInfo &
ExpansionPortAPI::getInfo() const
{
    return expansionPort->getInfo();
}

const CartridgeInfo &
ExpansionPortAPI::getCachedInfo() const
{
    return expansionPort->getCachedInfo();
}

CartridgeRomInfo
ExpansionPortAPI::getRomInfo(isize nr) const
{
    return expansionPort->getRomInfo(nr);
}

void
ExpansionPortAPI::attachCartridge(const std::filesystem::path &path, bool reset)
{
    expansionPort->attachCartridge(path, reset);
    expansionPort->markAsDirty();
}

void
ExpansionPortAPI::attachCartridge(const MediaFile &c, bool reset)
{
    expansionPort->attachCartridge(c, reset);
    expansionPort->markAsDirty();
}

void
ExpansionPortAPI::attachReu(isize capacity)
{
    expansionPort->attachReu(capacity);
    expansionPort->markAsDirty();
}

void
ExpansionPortAPI::attachGeoRam(isize capacity)
{
    expansionPort->attachGeoRam(capacity);
    expansionPort->markAsDirty();
}

void
ExpansionPortAPI::attachIsepicCartridge()
{
    expansionPort->attachIsepicCartridge();
    expansionPort->markAsDirty();
}

void
ExpansionPortAPI::detachCartridge()
{
    expansionPort->detachCartridge();
    expansionPort->markAsDirty();
}


//
// Disk
//

Disk *
DiskAPI::get()
{
    return drive->disk.get();
}


//
// Drive
//

const DriveConfig &
DriveAPI::getConfig() const
{
    return drive->getConfig();
}

const DriveInfo &
DriveAPI::getInfo() const
{
    return drive->getInfo();
}

const DriveInfo &
DriveAPI::getCachedInfo() const
{
    return drive->getCachedInfo();
}

void
DriveAPI::insertBlankDisk(DOSType fstype, string name)
{
    drive->insertNewDisk(fstype, name);
    drive->markAsDirty();
}

void
DriveAPI::insertMedia(MediaFile &file, bool wp)
{
    drive->insertMediaFile(file, wp);
    drive->markAsDirty();
}

void
DriveAPI::insertFileSystem(const class FileSystem &device, bool wp)
{
    drive->insertFileSystem(device, wp);
    drive->markAsDirty();
}

void
DriveAPI::ejectDisk()
{
    drive->ejectDisk();
    drive->markAsDirty();
}


//
// Defaults
//

void
DefaultsAPI::load(const fs::path &path)
{
    defaults->load(path);
}

void
DefaultsAPI::load(std::ifstream &stream)
{
    defaults->load(stream);
}

void
DefaultsAPI::load(std::stringstream &stream)
{
    defaults->load(stream);
}

void
DefaultsAPI::save(const fs::path &path)
{
    defaults->save(path);
}

void
DefaultsAPI::save(std::ofstream &stream)
{
    defaults->save(stream);
}

void
DefaultsAPI::save(std::stringstream &stream)
{
    defaults->save(stream);
}

string
DefaultsAPI::getString(const string &key) const
{
    return defaults->getString(key);
}

i64
DefaultsAPI::getInt(const string &key) const
{
    return defaults->getInt(key);
}

i64
DefaultsAPI::get(Option option) const
{
    return defaults->get(option);
}

i64
DefaultsAPI::get(Option option, isize nr) const
{
    return defaults->get(option, nr);
}

string
DefaultsAPI::getFallbackString(const string &key) const
{
    return defaults->getFallbackString(key);
}

i64
DefaultsAPI::getFallbackInt(const string &key) const
{
    return defaults->getFallbackInt(key);
}

i64
DefaultsAPI::getFallback(Option option) const
{
    return defaults->getFallback(option);
}

i64
DefaultsAPI::getFallback(Option option, isize nr) const
{
    return defaults->getFallback(option, nr);
}

void
DefaultsAPI::setString(const string &key, const string &value)
{
    defaults->setString(key, value);
}

void
DefaultsAPI::set(Option opt, i64 value)
{
    defaults->set(opt, value);
}

void
DefaultsAPI::set(Option opt, i64 value, isize nr)
{
    defaults->set(opt, value, nr);
}

void
DefaultsAPI::set(Option opt, i64 value, std::vector <isize> nrs)
{
    defaults->set(opt, value, nrs);
}

void
DefaultsAPI::setFallback(const string &key, const string &value)
{
    defaults->setFallback(key, value);
}

void
DefaultsAPI::setFallback(Option option, const string &value)
{
    defaults->setFallback(option, value);
}

void
DefaultsAPI::setFallback(Option opt, i64 value)
{
    defaults->setFallback(opt, value);
}

void
DefaultsAPI::setFallback(Option opt, const string &value, isize nr)
{
    defaults->setFallback(opt, value, nr);
}

void
DefaultsAPI::setFallback(Option opt, i64 value, isize nr)
{
    defaults->setFallback(opt, value, nr);
}

void
DefaultsAPI::setFallback(Option opt, const string &value, std::vector <isize> nrs)
{
    defaults->setFallback(opt, value, nrs);
}

void
DefaultsAPI::setFallback(Option opt, i64 value, std::vector <isize> nrs)
{
    defaults->setFallback(opt, value, nrs);
}

void
DefaultsAPI::remove()
{
    defaults->remove();
}

void
DefaultsAPI::remove(const string &key)
{
    defaults->remove(key);
}

void
DefaultsAPI::remove(Option option)
{
    defaults->remove(option);
}

void
DefaultsAPI::remove(Option option, isize nr)
{
    defaults->remove(option, nr);
}

void
DefaultsAPI::remove(Option option, std::vector <isize> nrs)
{
    defaults->remove(option, nrs);
}

}
