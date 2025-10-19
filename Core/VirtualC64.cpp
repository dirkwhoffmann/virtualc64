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

/* All main API functions are annotated with one of the following keywords:
 *
 *   VC64_PUBLIC:
 *
 *     This macro performs a sanity check in debug builds by assuring
 *     that the function is called from outside the emulator thread.
 *
 *   VC64_PUBLIC_SUSPEND:
 *
 *     The macro additionally ensures that the emulator is in suspended
 *     state before the function body is executed. The emulator can suspend
 *     only after the current frame has been completed. Thus, calling
 *     an API function with this annotation may cause a noticable lag.
 */


struct SuspendResume {
    const API *api;
    SuspendResume(const API *api) : api(api) {
        assert(!api->emu || api->emu->isUserThread());
        api->suspend();
    }
    ~SuspendResume() { api->resume(); }
};

#define VC64_PUBLIC assert(!emu || emu->isUserThread());
#define VC64_PUBLIC_SUSPEND VC64_PUBLIC SuspendResume _sr(this);

void 
API::suspend() const
{
    VC64_PUBLIC
    emu->suspend();
}

void 
API::resume() const
{
    VC64_PUBLIC
    emu->resume();
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

    // Wire all APIs...
    
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
    controlPort1.mouse.emu = emu;
    controlPort1.mouse.mouse = &emu->main.port1.mouse;
    controlPort1.joystick.emu = emu;
    controlPort1.joystick.joystick = &emu->main.port1.joystick;
    controlPort1.paddle.emu = emu;
    controlPort1.paddle.paddle = &emu->main.port1.paddle;

    controlPort2.emu = emu;
    controlPort2.controlPort = &emu->main.port2;
    controlPort2.mouse.emu = emu;
    controlPort2.mouse.mouse = &emu->main.port2.mouse;
    controlPort2.joystick.emu = emu;
    controlPort2.joystick.joystick = &emu->main.port2.joystick;
    controlPort2.paddle.emu = emu;
    controlPort2.paddle.paddle = &emu->main.port2.paddle;

    userPort.emu = emu;
    userPort.userPort = &emu->main.userPort;
    userPort.rs232.emu = emu;
    userPort.rs232.rs232 = &emu->main.userPort.rs232;

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

    remoteManager.emu = emu;
    remoteManager.remoteManager = &emu->main.remoteManager;

    retroShell.emu = emu;
    retroShell.retroShell = &emu->main.retroShell;

    defaults.emu = emu;
    defaults.defaults = &emu->defaults;
}

VirtualC64::~VirtualC64()
{
    VC64_PUBLIC
    halt();
    delete emu;
}

const EmulatorInfo &
VirtualC64::getInfo() const 
{
    VC64_PUBLIC
    return emu->getInfo();
}

const EmulatorInfo &
VirtualC64::getCachedInfo() const
{
    VC64_PUBLIC
    return emu->getCachedInfo();
}

const EmulatorStats &
VirtualC64::getStats() const 
{
    VC64_PUBLIC
    return emu->getStats();
}

bool
VirtualC64::isPoweredOn() const
{
    VC64_PUBLIC
    return emu->isPoweredOn();
}

bool
VirtualC64::isPoweredOff() const
{
    VC64_PUBLIC
    return emu->isPoweredOff();
}

bool
VirtualC64::isPaused() const
{
    VC64_PUBLIC
    return emu->isPaused();
}

bool
VirtualC64::isRunning() const
{
    VC64_PUBLIC
    return emu->isRunning();
}

bool
VirtualC64::isSuspended() const
{
    VC64_PUBLIC
    return emu->isSuspended();
}

bool
VirtualC64::isHalted() const
{
    VC64_PUBLIC
    return emu->isHalted();
}

bool
VirtualC64::isWarping() const
{
    VC64_PUBLIC
    return emu->isWarping();
}

bool
VirtualC64::isTracking() const
{
    VC64_PUBLIC
    return emu->isTracking();
}

void 
VirtualC64::isReady() const
{
    VC64_PUBLIC
    return emu->isReady();
}

void 
VirtualC64::powerOn() 
{
    VC64_PUBLIC
    emu->put(Cmd::POWER_ON);
}

void 
VirtualC64::powerOff() 
{
    VC64_PUBLIC
    emu->put(Cmd::POWER_OFF);
}

void 
VirtualC64::run() 
{
    VC64_PUBLIC
    
    // Throw an exception if the emulator is not ready to run
    isReady();
    
    emu->put(Cmd::RUN);
}

void 
VirtualC64::pause() 
{
    VC64_PUBLIC

    emu->put(Cmd::PAUSE);
}

void
VirtualC64::hardReset()
{
    VC64_PUBLIC
    emu->put(Cmd::HARD_RESET);
}

void
VirtualC64::softReset()
{
    VC64_PUBLIC
    emu->put(Cmd::SOFT_RESET);
}

void
VirtualC64::halt() 
{
    VC64_PUBLIC
    // Signal the emulator to halt
    emu->put(Cmd::HALT);
    
    // Wait for the thread to terminate
    emu->join();
}

void 
VirtualC64::suspend() 
{
    VC64_PUBLIC
    emu->suspend();
}

void 
VirtualC64::resume() 
{
    VC64_PUBLIC
    emu->resume();
}

void 
VirtualC64::warpOn(isize source)
{
    VC64_PUBLIC
    emu->put(Cmd::WARP_ON, source);
}

void 
VirtualC64::warpOff(isize source)
{
    VC64_PUBLIC
    emu->put(Cmd::WARP_OFF, source);
}

void 
VirtualC64::trackOn(isize source)
{
    VC64_PUBLIC_SUSPEND
    emu->trackOn(source);
}

void 
VirtualC64::trackOff(isize source)
{
    VC64_PUBLIC_SUSPEND
    emu->trackOff(source);
}

void
VirtualC64::stepInto()
{
    VC64_PUBLIC
    emu->stepInto();
}

void
VirtualC64::stepOver()
{
    VC64_PUBLIC
    emu->stepOver();
}

void
VirtualC64::stepCycle()
{
    VC64_PUBLIC
    emu->stepCycle();
}

void
VirtualC64::finishLine()
{
    VC64_PUBLIC
    emu->finishLine();
}

void
VirtualC64::finishFrame()
{
    VC64_PUBLIC
    emu->finishFrame();
}

void
VirtualC64::wakeUp()
{
    VC64_PUBLIC
    emu->wakeUp();
}

void
VirtualC64::launch(const void *listener, Callback *func)
{
    VC64_PUBLIC
    emu->launch(listener, func);
}

bool
VirtualC64::isLaunched() const
{
    VC64_PUBLIC
    return emu->isLaunched();
}

i64
VirtualC64::get(Opt option) const
{
    VC64_PUBLIC
    return emu->get(option);
}

i64
VirtualC64::get(Opt option, long id) const
{
    VC64_PUBLIC
    return emu->get(option, id);
}

void
VirtualC64::set(ConfigScheme model)
{
    VC64_PUBLIC_SUSPEND
    put(Cmd::CONFIG_SCHEME, (i64)model);
    emu->markAsDirty();
}

void
VirtualC64::set(Opt opt, i64 value) throws
{
    VC64_PUBLIC
    emu->check(opt, value);
    put(Cmd::CONFIG_ALL, ConfigCmd { .option = opt, .value = value });
    emu->markAsDirty();
}

void
VirtualC64::set(Opt opt, i64 value, long id)
{
    VC64_PUBLIC
    emu->check(opt, value, { id });
    put(Cmd::CONFIG, ConfigCmd { .option = opt, .value = value, .id = id });
    emu->markAsDirty();
}

void
VirtualC64::exportConfig(const fs::path &path) const
{
    VC64_PUBLIC_SUSPEND
    emu->main.exportConfig(path);
}

void
VirtualC64::exportConfig(std::ostream &stream) const
{
    VC64_PUBLIC_SUSPEND
    emu->main.exportConfig(stream);
}

void
VirtualC64::put(const Command &cmd)
{
    VC64_PUBLIC
    emu->put(cmd);
}


//
// C64API
//

u64
C64API::getAutoInspectionMask() const
{
    VC64_PUBLIC
    return c64->getAutoInspectionMask();
}

void
C64API::setAutoInspectionMask(u64 mask)
{
    VC64_PUBLIC_SUSPEND
    c64->setAutoInspectionMask(mask);
}

const C64Info &
C64API::getInfo() const
{
    VC64_PUBLIC
    return c64->getInfo();
}

const C64Info &
C64API::getCachedInfo() const
{
    VC64_PUBLIC
    return c64->getCachedInfo();
}

void
C64API::dump(Category category, std::ostream &os) const
{
    VC64_PUBLIC_SUSPEND
    c64->dump(category, os);
}

bool
C64API::getMsg(Message &msg)
{
    VC64_PUBLIC
    return c64->msgQueue.get(msg);
}

RomTraits
C64API::getRomTraits(RomType type) const
{
    VC64_PUBLIC
    return c64->getRomTraits(type);
}

void
C64API::loadWorkspace(const fs::path &path)
{
    VC64_PUBLIC_SUSPEND
    c64->loadWorkspace(path);
}

void
C64API::saveWorkspace(const fs::path &path) const
{
    VC64_PUBLIC_SUSPEND
    c64->saveWorkspace(path);
}

MediaFile *
C64API::takeSnapshot(Compressor compressor, isize delay, bool repeat)
{
    VC64_PUBLIC_SUSPEND
    return c64->takeSnapshot(compressor, delay, repeat);
}

void
C64API::loadSnapshot(const MediaFile &snapshot)
{
    VC64_PUBLIC_SUSPEND

    emu->markAsDirty();

    try {

        // Restore the saved state
        c64->loadSnapshot(snapshot);

    } catch (AppError &) {

        /* If we reach this point, the emulator has been put into an
         * inconsistent state due to corrupted snapshot data. We cannot
         * continue emulation, because it would likely crash the
         * application. Because we cannot revert to the old state either,
         * we perform a hard reset to eliminate the inconsistency.
         */
        emu->put(Cmd::HARD_RESET);
        throw;
    }
}

void
C64API::loadSnapshot(const fs::path &path)
{
    VC64_PUBLIC_SUSPEND
    c64->loadSnapshot(path);
}

void
C64API::saveSnapshot(const fs::path &path, Compressor compressor) const
{
    VC64_PUBLIC_SUSPEND
    c64->saveSnapshot(path, compressor);

}

void
C64API::loadRom(const fs::path &path)
{
    VC64_PUBLIC_SUSPEND
    c64->loadRom(path);
    emu->markAsDirty();
}

void 
C64API::loadRom(const MediaFile &file)
{
    VC64_PUBLIC_SUSPEND
    c64->loadRom(file);
    emu->markAsDirty();
}

void 
C64API::deleteRom(RomType type)
{
    VC64_PUBLIC_SUSPEND
    c64->deleteRom(type);
    emu->markAsDirty();
}

void 
C64API::deleteRoms()
{
    VC64_PUBLIC_SUSPEND
    c64->deleteRoms();
    emu->markAsDirty();
}

void
C64API::saveRom(RomType rom, const std::filesystem::path &path)
{
    VC64_PUBLIC_SUSPEND
    c64->saveRom(rom, path);
    emu->markAsDirty();
}

void
C64API::installOpenRom(RomType type)
{
    VC64_PUBLIC_SUSPEND
    c64->installOpenRom(type);
    emu->markAsDirty();
}

void
C64API::installOpenRoms()
{
    VC64_PUBLIC_SUSPEND
    c64->installOpenRoms();
    emu->markAsDirty();
}

void
C64API::flash(const MediaFile &file)
{
    VC64_PUBLIC_SUSPEND
    c64->flash(file);
    emu->markAsDirty();
}

void 
C64API::flash(const MediaFile &file, isize item)
{
    VC64_PUBLIC_SUSPEND
    c64->flash(file, item);
    emu->markAsDirty();
}

void 
C64API::flash(const FileSystem &fs, isize item)
{
    VC64_PUBLIC_SUSPEND
    c64->flash(fs, item);
    emu->markAsDirty();
}


//
// CPU
//

const CPUInfo &
CPUAPI::getInfo() const
{
    return cpu->getInfo();
}

const CPUInfo &
CPUAPI::getCachedInfo() const
{
    return cpu->getCachedInfo();
}

isize
CPUAPI::loggedInstructions() const
{
    return cpu->debugger.loggedInstructions();
}

void
CPUAPI::clearLog()
{
    return cpu->debugger.clearLog();
}

void
CPUAPI::setNumberFormat(DasmNumberFormat instrFormat, DasmNumberFormat dataFormat)
{
    return cpu->disassembler.setNumberFormat(instrFormat, dataFormat);
}

isize
CPUAPI::disassemble(char *dst, const char *fmt, u16 addr) const
{
    return cpu->disassembler.disass(dst, fmt, addr);
}

isize
CPUAPI::disassembleRecorded(char *dst, const char *fmt, isize nr) const
{
    return cpu->debugger.disassRecorded(dst, fmt, nr);
}

Guard *
CPUAPI::breakpointNr(long nr) const
{
    return cpu->debugger.breakpoints.guardNr(nr);
}

Guard *
CPUAPI::breakpointAt(u32 addr) const
{
    return cpu->debugger.breakpoints.guardAt(addr);
}

Guard *
CPUAPI::watchpointNr(long nr) const
{
    return cpu->debugger.watchpoints.guardNr(nr);
}

Guard *
CPUAPI::watchpointAt(u32 addr) const
{
    return cpu->debugger.watchpoints.guardAt(addr);
}


//
// Memory
//

const MemConfig &
MemoryAPI::getConfig() const
{
    return mem->getConfig();
}

const MemInfo &
MemoryAPI::getInfo() const
{
    return mem->getInfo();
}

const MemInfo &
MemoryAPI::getCachedInfo() const
{
    return mem->getCachedInfo();
}

string
MemoryAPI::memdump(u16 addr, isize num, bool hex, isize pads, MemType src) const
{
    return mem->memdump(addr, num, hex, pads, src);
}

string
MemoryAPI::txtdump(u16 addr, isize num, MemType src) const
{
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

    return sidBridge->sid[nr].getInfo();
}

SIDInfo
SIDAPI::getCachedInfo(isize nr) const
{
    assert(nr < 3);

    return sidBridge->sid[nr].getCachedInfo();
}

float
SIDAPI::draw(u32 *buffer, isize width, isize height,
                         float maxAmp, u32 color, isize sid) const
{
    return sidBridge->draw(buffer, width, height, maxAmp, color, sid);
}


//
// Audio port
//

AudioPortStats
AudioPortAPI::getStats() const
{
    return audioPort->getStats();
}

isize
AudioPortAPI::copyMono(float *buffer, isize n)
{
    return audioPort->copyMono(buffer, n);
}

isize
AudioPortAPI::copyStereo(float *left, float *right, isize n)
{
    return audioPort->copyStereo(left, right, n);
}

isize
AudioPortAPI::copyInterleaved(float *buffer, isize n)
{
    return audioPort->copyInterleaved(buffer, n);
}


//
// Video port
//

void
VideoPortAPI::lockTexture()
{
    VC64_PUBLIC
    emu->lockTexture();
}

void
VideoPortAPI::unlockTexture()
{
    VC64_PUBLIC
    emu->unlockTexture();
}

/*
u32 *
VideoPortAPI::oldGetTexture() const
{
    return emu->oldGetTexture();
}

u32 *
VideoPortAPI::oldGetDmaTexture() const
{
    return emu->oldGetDmaTexture();
}
*/

const u32 *
VideoPortAPI::getTexture() const
{
    VC64_PUBLIC
    return (u32 *)emu->getTexture().pixels.ptr;
}

const u32 *
VideoPortAPI::getTexture(isize *nr, isize *width, isize *height) const
{
    VC64_PUBLIC
    auto &texture = emu->getTexture();

    *nr = isize(texture.nr);
    *width = isize(Texture::width);
    *height = isize(Texture::height);

    return (u32 *)texture.pixels.ptr;
}

const u32 *
VideoPortAPI::getDmaTexture() const
{
    VC64_PUBLIC
    return (u32 *)emu->getDmaTexture().pixels.ptr;
}

const u32 *
VideoPortAPI::getDmaTexture(isize *nr, isize *width, isize *height) const
{
    VC64_PUBLIC
    auto &texture = emu->getDmaTexture();

    *nr = isize(texture.nr);
    *width = isize(Texture::width);
    *height = isize(Texture::height);

    return (u32 *)texture.pixels.ptr;
}

void
VideoPortAPI::findInnerArea(isize &x1, isize &x2, isize &y1, isize &y2) const
{
    VC64_PUBLIC_SUSPEND
    videoPort->findInnerArea(x1, x2, y1, y2);
}

void
VideoPortAPI::findInnerAreaNormalized(double &x1, double &x2, double &y1, double &y2) const
{
    VC64_PUBLIC_SUSPEND
    videoPort->findInnerAreaNormalized(x1, x2, y1, y2);
}


//
// DMA Debugger
//

const DmaDebuggerConfig &
DmaDebuggerAPI::getConfig() const
{
    return dmaDebugger->getConfig();
}


//
// Keyboard
//

bool 
KeyboardAPI::isPressed(C64Key key) const
{
    return keyboard->isPressed(key);
}

void
KeyboardAPI::press(C64Key key, double delay, double duration)
{
    if (delay == 0.0) {

        keyboard->press(key);
        emu->markAsDirty();

    } else {
        
        emu->put(Command(Cmd::KEY_PRESS, KeyCmd { .keycode = (u8)key.nr, .delay = delay }));
    }
    if (duration != 0.0) {
        
        emu->put(Command(Cmd::KEY_RELEASE, KeyCmd { .keycode = (u8)key.nr, .delay = delay + duration }));
    }
}

void 
KeyboardAPI::toggle(C64Key key, double delay, double duration)
{
    if (delay == 0.0) {
        
        keyboard->toggle(key);
        emu->markAsDirty();
        
    } else {

        emu->put(Command(Cmd::KEY_TOGGLE, KeyCmd { .keycode = (u8)key.nr, .delay = delay }));
    }
    if (duration != 0.0) {
        
        emu->put(Command(Cmd::KEY_TOGGLE, KeyCmd { .keycode = (u8)key.nr, .delay = delay + duration }));
    }

}

void
KeyboardAPI::release(C64Key key, double delay)
{
    if (delay > 0.0) {

        emu->put(Command(Cmd::KEY_RELEASE, KeyCmd { .keycode = (u8)key.nr, .delay = delay }));
        return;
    }
    keyboard->release(key);
    emu->markAsDirty();
}

void 
KeyboardAPI::releaseAll(double delay)
{
    if (delay > 0.0) {

        emu->put(Command(Cmd::KEY_RELEASE_ALL, KeyCmd { .delay = delay }));
        return;
    }
    keyboard->releaseAll();
    emu->markAsDirty();
}

void KeyboardAPI::autoType(const string &text)
{
    keyboard->autoType(text);
    emu->markAsDirty();
}

void KeyboardAPI::abortAutoTyping()
{
    keyboard->abortAutoTyping();
    emu->markAsDirty();
}


//
// Joystick
//

const JoystickInfo &
JoystickAPI::getInfo() const
{
    VC64_PUBLIC
    return joystick->getInfo();
}

const JoystickInfo &
JoystickAPI::getCachedInfo() const
{
    VC64_PUBLIC
    return joystick->getCachedInfo();
}

void
JoystickAPI::trigger(GamePadAction event)
{
    VC64_PUBLIC
    emu->put(Cmd::JOY_EVENT, GamePadCmd { .port = joystick->objid, .action = event });
}


//
// Paddle
//

const PaddleInfo &
PaddleAPI::getInfo() const
{
    VC64_PUBLIC
    return paddle->getInfo();
}

const PaddleInfo &
PaddleAPI::getCachedInfo() const
{
    VC64_PUBLIC
    return paddle->getCachedInfo();
}


//
// Datasette
//

const DatasetteInfo &
DatasetteAPI::getInfo() const
{
    VC64_PUBLIC
    return datasette->getInfo();
}

const DatasetteInfo &
DatasetteAPI::getCachedInfo() const
{
    VC64_PUBLIC
    return datasette->getCachedInfo();
}

void
DatasetteAPI::insertTape(MediaFile &file)
{
    VC64_PUBLIC
    datasette->insertTape(file);
    emu->markAsDirty();
}

void
DatasetteAPI::ejectTape()
{
    VC64_PUBLIC
    datasette->ejectTape();
    emu->markAsDirty();
}


//
// RS232
//

void
RS232API::operator<<(char c)
{
    VC64_PUBLIC
    *rs232 << c;
}

void 
RS232API::operator<<(const string &s)
{
    VC64_PUBLIC
    *rs232 << s;
}

std::u16string
RS232API::readIncoming()
{
    VC64_PUBLIC
    return rs232->readIncoming();
}

std::u16string 
RS232API::readOutgoing()
{
    VC64_PUBLIC
    return rs232->readOutgoing();
}

int
RS232API::readIncomingPrintableByte()
{
    VC64_PUBLIC
    return rs232->readIncomingPrintableByte();
}

int
RS232API::readOutgoingPrintableByte()
{
    VC64_PUBLIC
    return rs232->readOutgoingPrintableByte();
}


//
// Mouse
//

const MouseConfig &
MouseAPI::getConfig() const
{
    VC64_PUBLIC
    return mouse->getConfig();
}

void
MouseAPI::setXY(double x, double y)
{
    VC64_PUBLIC
    emu->put(Command(Cmd::MOUSE_MOVE_ABS, CoordCmd { .port = mouse->objid, .x = x, .y = y }));
}

void
MouseAPI::setDxDy(double dx, double dy)
{
    VC64_PUBLIC
    emu->put(Command(Cmd::MOUSE_MOVE_REL, CoordCmd { .port = mouse->objid, .x = dx, .y = dy }));
}

void
MouseAPI::trigger(GamePadAction action)
{
    VC64_PUBLIC
    emu->put(Command(Cmd::MOUSE_BUTTON, GamePadCmd { .port = mouse->objid, .action = action }));
}


bool
MouseAPI::detectShakeXY(double x, double y)
{
    VC64_PUBLIC
    return mouse->detectShakeXY(x, y);
}

bool
MouseAPI::detectShakeDxDy(double dx, double dy)
{
    VC64_PUBLIC
    return mouse->detectShakeDxDy(dx, dy);
}


//
// RemoteManager
//

const RemoteManagerInfo &
RemoteManagerAPI::getInfo() const
{
    return remoteManager->getInfo();
}

const RemoteManagerInfo &
RemoteManagerAPI::getCachedInfo() const
{
    return remoteManager->getCachedInfo();
}


//
// RetroShell
//

const RetroShellInfo &
RetroShellAPI::getInfo() const
{
    VC64_PUBLIC
    return retroShell->getInfo();
}

const RetroShellInfo &
RetroShellAPI::getCachedInfo() const
{
    VC64_PUBLIC
    return retroShell->getCachedInfo();
}

const char *
RetroShellAPI::text()
{
    VC64_PUBLIC_SUSPEND
    return retroShell->text();
}

/*
isize
RetroShellAPI::cursorRel()
{
    return retroShell->cursorRel();
}
*/

void
RetroShellAPI::press(RSKey key, bool shift)
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
    retroShell->asyncExecScript(ss);
}

void
RetroShellAPI::execScript(const std::ifstream &fs)
{
    retroShell->asyncExecScript(fs);
}

void
RetroShellAPI::execScript(const string &contents)
{
    retroShell->asyncExecScript(contents);
}

void 
RetroShellAPI::execScript(const MediaFile &file)
{
    retroShell->asyncExecScript(file);
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
    emu->markAsDirty();
}

void
ExpansionPortAPI::attachCartridge(const MediaFile &c, bool reset)
{
    expansionPort->attachCartridge(c, reset);
    emu->markAsDirty();
}

void
ExpansionPortAPI::attachReu(isize capacity)
{
    expansionPort->attachReu(capacity);
    emu->markAsDirty();
}

void
ExpansionPortAPI::attachGeoRam(isize capacity)
{
    expansionPort->attachGeoRam(capacity);
    emu->markAsDirty();
}

void
ExpansionPortAPI::attachIsepicCartridge()
{
    expansionPort->attachIsepicCartridge();
    emu->markAsDirty();
}

void
ExpansionPortAPI::detachCartridge()
{
    expansionPort->detachCartridge();
    emu->markAsDirty();
}

MediaFile *
ExpansionPortAPI::exportCRT() const
{
    return expansionPort->exportCRT();
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
    emu->markAsDirty();
}

void
DriveAPI::insertMedia(MediaFile &file, bool wp)
{
    drive->insertMediaFile(file, wp);
    emu->markAsDirty();
}

void
DriveAPI::insertFileSystem(const class FileSystem &device, bool wp)
{
    drive->insertFileSystem(device, wp);
    emu->markAsDirty();
}

void
DriveAPI::ejectDisk()
{
    drive->ejectDisk();
    emu->markAsDirty();
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
DefaultsAPI::getRaw(const string &key) const
{
    return defaults->getRaw(key);
}

i64
DefaultsAPI::get(const string &key) const
{
    return defaults->get(key);
}

i64
DefaultsAPI::get(Opt option, isize nr) const
{
    return defaults->get(option, nr);
}

string
DefaultsAPI::getFallbackRaw(const string &key) const
{
    return defaults->getFallbackRaw(key);
}

i64
DefaultsAPI::getFallback(const string &key) const
{
    return defaults->getFallback(key);
}

i64
DefaultsAPI::getFallback(Opt option, isize nr) const
{
    return defaults->getFallback(option, nr);
}

void
DefaultsAPI::set(const string &key, const string &value)
{
    defaults->set(key, value);
}

void
DefaultsAPI::set(Opt opt, const string &value)
{
    defaults->set(opt, value);
}

void
DefaultsAPI::set(Opt opt, const string &value, std::vector<isize> objids)
{
    defaults->set(opt, value, objids);
}

void
DefaultsAPI::set(Opt opt, i64 value)
{
    defaults->set(opt, value);
}

void
DefaultsAPI::set(Opt opt, i64 value, std::vector<isize> objids)
{
    defaults->set(opt, value, objids);
}

void
DefaultsAPI::setFallback(const string &key, const string &value)
{
    defaults->setFallback(key, value);
}

void
DefaultsAPI::setFallback(Opt opt, const string &value)
{
    defaults->setFallback(opt, value);
}

void
DefaultsAPI::setFallback(Opt opt, const string &value, std::vector<isize> objids)
{
    defaults->setFallback(opt, value, objids);
}

void
DefaultsAPI::setFallback(Opt opt, i64 value)
{
    defaults->setFallback(opt, value);
}

void
DefaultsAPI::setFallback(Opt opt, i64 value, std::vector<isize> objids)
{
    defaults->setFallback(opt, value, objids);
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
DefaultsAPI::remove(Opt option)
{
    defaults->remove(option);
}

void
DefaultsAPI::remove(Opt option, std::vector <isize> objids)
{
    defaults->remove(option, objids);
}

}
