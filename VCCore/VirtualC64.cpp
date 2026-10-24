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

#include "vcconfig.h"
#include "VirtualC64.h"
#include "Emulator.h"
#include "SVMFile.h"

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
VirtualC64::snapshotVersion()
{
    return C64::snapshotVersion();
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

    xray.emu = emu;
    xray.xray = &emu->main.vic.xray;

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
    return emu->info.current();
}

const EmulatorInfo &
VirtualC64::getCachedInfo() const
{
    VC64_PUBLIC
    return emu->info.backed();
}

const EmulatorMetrics &
VirtualC64::getMetrics() const
{
    VC64_PUBLIC
    return emu->metrics.current();
}

const EmulatorMetrics &
VirtualC64::getCachedMetrics() const
{
    VC64_PUBLIC
    return emu->metrics.backed();
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
    VC64_PUBLIC
    emu->put(Cmd::TRACK_ON, source);
}

void 
VirtualC64::trackOff(isize source)
{
    VC64_PUBLIC
    emu->put(Cmd::TRACK_OFF, source);
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

void
VirtualC64::removeListener()
{
    VC64_PUBLIC
    emu->main.msgQueue.setListener(nullptr, nullptr);
}

void
VirtualC64::setListener(const void *listener, Callback *func)
{
    VC64_PUBLIC
    emu->main.msgQueue.setListener(listener, func);
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
VirtualC64::setAsync(ConfigScheme model)
{
    VC64_PUBLIC_SUSPEND
    putAsync(Cmd::CONFIG_SCHEME, (i64)model);
    emu->markAsDirty();
}

void
VirtualC64::setAsync(Opt opt, i64 value)
{
    VC64_PUBLIC
    putAsync(Cmd::CONFIG_ALL, ConfigCmd { .option = opt, .value = value });
    emu->markAsDirty();
}

void
VirtualC64::set(ConfigScheme model)
{
    VC64_PUBLIC
    emu->sendAndWait(Command(Cmd::CONFIG_SCHEME, (i64)model));
    emu->markAsDirty();
}

void
VirtualC64::set(Opt opt, i64 value)
{
    VC64_PUBLIC
    emu->sendAndWait(Command(Cmd::CONFIG_ALL, ConfigCmd { .option = opt, .value = value }));
    emu->markAsDirty();
}

void
VirtualC64::set(Opt opt, i64 value, long id)
{
    VC64_PUBLIC
    emu->sendAndWait(Command(Cmd::CONFIG, ConfigCmd { .option = opt, .value = value, .id = id }));
    emu->markAsDirty();
}

void
VirtualC64::check(Opt opt, i64 value)
{
    VC64_PUBLIC
    emu->check(opt, value);
}

void
VirtualC64::check(Opt opt, i64 value, long id)
{
    VC64_PUBLIC
    emu->check(opt, value, { id });
}

void
VirtualC64::setAsync(Opt opt, i64 value, long id)
{
    VC64_PUBLIC
    putAsync(Cmd::CONFIG, ConfigCmd { .option = opt, .value = value, .id = id });
    emu->markAsDirty();
}

void
VirtualC64::sync()
{
    VC64_PUBLIC

    // A command that does nothing: once it has been carried out, so has everything before it
    emu->sendAndWait(Command(Cmd::NONE));
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
VirtualC64::put(const Command &cmd, const std::string &str)
{
    VC64_PUBLIC
    emu->sendAndWait(cmd, str);
}

void
VirtualC64::putAsync(const Command &cmd, const std::string &str)
{
    VC64_PUBLIC
    emu->put(cmd, str);
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
    return c64->info.current();
}

const C64Info &
C64API::getCachedInfo() const
{
    VC64_PUBLIC
    return c64->info.backed();
}

void
C64API::dump(Category category, std::ostream &os) const
{
    VC64_PUBLIC_SUSPEND
    c64->dump(category, os);
}

bool
C64API::getMsg(Message &msg, std::exception_ptr &exception)
{
    VC64_PUBLIC
    return c64->msgQueue.get(msg, exception);
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

void
C64API::saveWorkspaceAsync(const fs::path &path) const
{
    VC64_PUBLIC
    emu->put(Cmd::SAVE_WORKSPACE, path.string());
}

void
C64API::loadWorkspace(retro::vault::SVMFile &svm)
{
    VC64_PUBLIC_SUSPEND
    c64->loadWorkspace(svm);
}

void
C64API::saveWorkspace(retro::vault::SVMFile &svm) const
{
    VC64_PUBLIC_SUSPEND
    c64->saveWorkspace(svm);
}

void
C64API::saveWorkspaceAsync(retro::vault::SVMFile &svm) const
{
    VC64_PUBLIC
    emu->put(Command(Cmd::SAVE_WORKSPACE, i64(0), i64(&svm)));
}

std::unique_ptr<Snapshot>
C64API::takeSnapshot(Compressor compressor, isize delay, bool repeat)
{
    VC64_PUBLIC_SUSPEND
    return c64->takeSnapshotNew(compressor, delay, repeat);
}


void
C64API::loadSnapshot(const Snapshot &snapshot)
{
    VC64_PUBLIC_SUSPEND

    emu->markAsDirty();

    try {

        // Restore the saved state
        c64->loadSnapshot(snapshot);

    } catch (std::exception &) {

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
C64API::saveSnapshotAsync(const fs::path &path, Compressor compressor) const
{
    VC64_PUBLIC
    emu->put(Command(Cmd::SAVE_SNAPSHOT, i64(compressor)), path.string());
}

void
C64API::saveSnapshot(retro::vault::SVMFile &svm, Compressor compressor) const
{
    VC64_PUBLIC_SUSPEND
    c64->saveSnapshot(svm, compressor);
}

void
C64API::saveSnapshotAsync(retro::vault::SVMFile &svm, Compressor compressor) const
{
    VC64_PUBLIC
    emu->put(Command(Cmd::SAVE_SNAPSHOT, i64(compressor), i64(&svm)));
}

void
C64API::hibernate(const fs::path &path, bool snapshot, bool workspace) const
{
    VC64_PUBLIC_SUSPEND
    c64->hibernate(path, snapshot, workspace);
}

void
C64API::hibernateAsync(const fs::path &path, bool snapshot, bool workspace) const
{
    VC64_PUBLIC
    emu->put(Command(Cmd::HIBERNATE, i64(snapshot) | i64(workspace) << 1), path.string());
}

void
C64API::hibernate(retro::vault::SVMFile &svm, bool snapshot, bool workspace) const
{
    VC64_PUBLIC_SUSPEND
    c64->hibernate(svm, snapshot, workspace);
}

void
C64API::hibernateAsync(retro::vault::SVMFile &svm, bool snapshot, bool workspace) const
{
    VC64_PUBLIC
    emu->put(Command(Cmd::HIBERNATE, i64(snapshot) | i64(workspace) << 1, i64(&svm)));
}

void
C64API::loadRom(const fs::path &path)
{
    VC64_PUBLIC_SUSPEND
    c64->loadRom(path);
    emu->markAsDirty();
}

void
C64API::loadRom(const std::filesystem::path &path, RomType rom)
{
    VC64_PUBLIC_SUSPEND
    c64->loadRom(path, rom);
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
C64API::saveRom(const std::filesystem::path &path, RomType rom)
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
C64API::flash(const fs::path &path, isize item)
{
    VC64_PUBLIC_SUSPEND
    c64->flash(path, item);
    emu->markAsDirty();
}


//
// CPU
//

const CPUInfo &
CPUAPI::getInfo() const
{
    return cpu->info.current();
}

const CPUInfo &
CPUAPI::getCachedInfo() const
{
    return cpu->info.backed();
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

RecordedInstruction
CPUAPI::logEntry(isize nr) const
{
    return cpu->debugger.logEntryAbs(nr);
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
    return mem->info.current();
}

const MemInfo &
MemoryAPI::getCachedInfo() const
{
    return mem->info.backed();
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
    return cia->info.current();
}

const CIAInfo &
CIAAPI::getCachedInfo() const
{
    return cia->info.backed();
}

const CIAMetrics &
CIAAPI::getMetrics() const
{
    return cia->metrics.current();
}

const CIAMetrics &
CIAAPI::getCachedMetrics() const
{
    return cia->metrics.backed();
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
    return vicii->info.current();
}

const VICIIInfo &
VICIIAPI::getCachedInfo() const
{
    return vicii->info.backed();
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
    assert(nr <= 3);

    return sidBridge->sid[nr].info.current();
}

SIDInfo
SIDAPI::getCachedInfo(isize nr) const
{
    assert(nr <= 3);

    return sidBridge->sid[nr].info.backed();
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

const AudioPortMetrics &
AudioPortAPI::getStats() const
{
    return audioPort->metrics.current();
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
VideoPortAPI::oldGetXrayTexture() const
{
    return emu->oldGetXrayTexture();
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
VideoPortAPI::getXrayTexture() const
{
    VC64_PUBLIC
    return (u32 *)emu->getXrayTexture().pixels.ptr;
}

const u32 *
VideoPortAPI::getXrayTexture(isize *nr, isize *width, isize *height) const
{
    VC64_PUBLIC
    auto &texture = emu->getXrayTexture();

    *nr = isize(texture.nr);
    *width = isize(Texture::width);
    *height = isize(Texture::height);

    return (u32 *)texture.pixels.ptr;
}

void
VideoPortAPI::saveTexture(const fs::path &path) const
{
    VC64_PUBLIC
    emu->lockTexture();
    try { videoPort->saveTexture(path); }
    catch (...) { emu->unlockTexture(); throw; }
    emu->unlockTexture();
}

void
VideoPortAPI::saveTexture(const fs::path &path, isize x1, isize x2, isize y1, isize y2) const
{
    VC64_PUBLIC
    emu->lockTexture();
    try { videoPort->saveTexture(path, x1, x2, y1, y2); }
    catch (...) { emu->unlockTexture(); throw; }
    emu->unlockTexture();
}

void
VideoPortAPI::saveXrayTexture(const fs::path &path) const
{
    VC64_PUBLIC
    emu->lockTexture();
    try { videoPort->saveXrayTexture(path); }
    catch (...) { emu->unlockTexture(); throw; }
    emu->unlockTexture();
}

void
VideoPortAPI::saveXrayTexture(const fs::path &path, isize x1, isize x2, isize y1, isize y2) const
{
    VC64_PUBLIC
    emu->lockTexture();
    try { videoPort->saveXrayTexture(path, x1, x2, y1, y2); }
    catch (...) { emu->unlockTexture(); throw; }
    emu->unlockTexture();
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

const XRayConfig &
XRayAPI::getConfig() const
{
    return xray->getConfig();
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

bool
KeyboardAPI::isLocked(C64Key key) const
{
    return keyboard->isLocked(key);
}

void
KeyboardAPI::lock(C64Key key)
{
    keyboard->lock(key);
}

void
KeyboardAPI::unlock(C64Key key)
{
    keyboard->unlock(key);
}

void
KeyboardAPI::unlockAll()
{
    keyboard->unlockAll();
}

void
KeyboardAPI::autoType(const string &text)
{
    keyboard->autoType(text);
    emu->markAsDirty();
}

void
KeyboardAPI::abortAutoTyping()
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
    return joystick->info.current();
}

const JoystickInfo &
JoystickAPI::getCachedInfo() const
{
    VC64_PUBLIC
    return joystick->info.backed();
}

void
JoystickAPI::trigger(GamePadAction event)
{
    VC64_PUBLIC
    emu->put(Cmd::JOY_EVENT, GamePadCmd { .port = joystick->objid, .action = event });
}

void
JoystickAPI::trigger(bool state[5])
{
    VC64_PUBLIC
    joystick->trigger(state);
}


//
// Paddle
//

const PaddleInfo &
PaddleAPI::getInfo() const
{
    VC64_PUBLIC
    return paddle->info.current();
}

const PaddleInfo &
PaddleAPI::getCachedInfo() const
{
    VC64_PUBLIC
    return paddle->info.backed();
}


//
// Datasette
//

const DatasetteInfo &
DatasetteAPI::getInfo() const
{
    VC64_PUBLIC
    return datasette->info.current();
}

const DatasetteInfo &
DatasetteAPI::getCachedInfo() const
{
    VC64_PUBLIC
    return datasette->info.backed();
}

void
DatasetteAPI::insertTape(const fs::path &path)
{
    VC64_PUBLIC
    datasette->insertTape(path);
    emu->markAsDirty();
}

void
DatasetteAPI::ejectTape()
{
    VC64_PUBLIC
    datasette->ejectTape();
    emu->markAsDirty();
}


void
DatasetteAPI::exportTape(const fs::path &path) const
{
    datasette->exportTAP(path);
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
    return remoteManager->info.current();
}

const RemoteManagerInfo &
RemoteManagerAPI::getCachedInfo() const
{
    return remoteManager->info.backed();
}

void
RemoteManagerAPI::send(ServerType server, const string &payload)
{
    VC64_PUBLIC
    remoteManager->send(server, payload);
}


//
// RetroShell
//

const RetroShellInfo &
RetroShellAPI::getInfo() const
{
    VC64_PUBLIC
    return retroShell->info.current();
}

const RetroShellInfo &
RetroShellAPI::getCachedInfo() const
{
    VC64_PUBLIC
    return retroShell->info.backed();
}

const char *
RetroShellAPI::text()
{
    VC64_PUBLIC_SUSPEND
    return retroShell->text();
}

void
RetroShellAPI::press(RSKey key, bool shift)
{
    VC64_PUBLIC
    retroShell->press(key, shift);
}

void
RetroShellAPI::press(char c)
{
    VC64_PUBLIC
    retroShell->press(c);
}

void
RetroShellAPI::press(const string &s)
{
    VC64_PUBLIC
    retroShell->press(s);
}

void
RetroShellAPI::execScript(const fs::path &path)
{
    VC64_PUBLIC_SUSPEND
    retroShell->asyncExecScript(path);
}

void
RetroShellAPI::execScript(std::stringstream &ss)
{
    VC64_PUBLIC
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

void
ExpansionPortAPI::exportCRT(const fs::path &path) const
{
    expansionPort->exportCRT(path);
}


//
// Disk
//

FloppyDisk *
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
    return drive->info.current();
}

const DriveInfo &
DriveAPI::getCachedInfo() const
{
    return drive->info.backed();
}

void
DriveAPI::insertBlankDisk(FSFormat fstype, string name)
{
    VC64_PUBLIC_SUSPEND
    drive->insertNewDisk(fstype, name);
    emu->markAsDirty();
}

void
DriveAPI::insert(const fs::path &path, bool wp)
{
    VC64_PUBLIC_SUSPEND
    drive->insertDisk(path, wp);
    emu->markAsDirty();
}

void
DriveAPI::ejectDisk()
{
    VC64_PUBLIC_SUSPEND
    drive->ejectDisk();
    emu->markAsDirty();
}

void
DriveAPI::save(const std::filesystem::path& path)
{
    VC64_PUBLIC_SUSPEND
    drive->writeToFile(path);
}

void
DriveAPI::saveFiles(const std::filesystem::path& path)
{
    VC64_PUBLIC_SUSPEND
    drive->saveFiles(path);
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
