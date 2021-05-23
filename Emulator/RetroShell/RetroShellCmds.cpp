// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "RetroShell.h"
#include "C64.h"
#include "IO.h"
#include "Parser.h"

#include <fstream>
#include <sstream>

//
// Top-level commands
//

template <> void
RetroShell::exec <Token::clear> (Arguments &argv, long param)
{
    clear();
}

template <> void
RetroShell::exec <Token::close> (Arguments &argv, long param)
{
    messageQueue.put(MSG_CLOSE_CONSOLE);
}

template <> void
RetroShell::exec <Token::easteregg> (Arguments& argv, long param)
{
    retroShell << "GREETINGS PROFESSOR HOFFMANN." << '\n' << '\n';
    retroShell << "THE ONLY WINNING MOVE IS NOT TO PLAY." << '\n' << '\n';
    retroShell << "HOW ABOUT A NICE GAME OF CHESS?" << '\n';
}

template <> void
RetroShell::exec <Token::source> (Arguments &argv, long param)
{
    auto stream = std::ifstream(argv.front());
    if (!stream.is_open()) {
        throw VC64Error(ERROR_FILE_NOT_FOUND, argv.front());
    }

    execScript(stream);
}

template <> void
RetroShell::exec <Token::wait> (Arguments &argv, long param)
{
    auto seconds = util::parseNum(argv.front());
    
    Cycle limit = cpu.cycle + seconds * vic.getFrequency();
    c64.retroShell.wakeUp = limit;
    
    throw ScriptInterruption("");
}


//
// Rgression testing
//

template <> void
RetroShell::exec <Token::regression, Token::setup> (Arguments &argv, long param)
{
    auto model = util::parseEnum <C64ModelEnum> (argv.front());

    c64.regressionTester.prepare(c64, model);
    
    // Pause the script to give the C64 some time to boot
    c64.retroShell.wakeUp = cpu.cycle + 3 * vic.getFrequency();
    throw ScriptInterruption("");
}

template <> void
RetroShell::exec <Token::regression, Token::run> (Arguments &argv, long param)
{
    auto path = argv.front();
    if (!util::fileExists(path)) throw VC64Error(ERROR_FILE_NOT_FOUND, path);

    PRGFile *file = AnyFile::make <PRGFile> (path);
    c64.flash(file, 0);
    delete file;
    
    keyboard.autoType("run\n");
}

template <> void
RetroShell::exec <Token::screenshot, Token::set, Token::filename> (Arguments &argv, long param)
{
    c64.regressionTester.dumpTexturePath = argv.front();
}

template <> void
RetroShell::exec <Token::screenshot, Token::set, Token::cutout> (Arguments &argv, long param)
{
    std::vector<string> vec(argv.begin(), argv.end());
    
    isize x1 = util::parseNum(vec[0]);
    isize y1 = util::parseNum(vec[1]);
    isize x2 = util::parseNum(vec[2]);
    isize y2 = util::parseNum(vec[3]);

    c64.regressionTester.x1 = x1;
    c64.regressionTester.y1 = y1;
    c64.regressionTester.x2 = x2;
    c64.regressionTester.y2 = y2;
}
    
template <> void
RetroShell::exec <Token::screenshot, Token::save> (Arguments &argv, long param)
{
    c64.regressionTester.dumpTexture(c64, argv.front());
}

template <> void
RetroShell::exec <Token::screenshot> (Arguments &argv, long param)
{
    std::ofstream file;
    std::vector<string> vec(argv.begin(), argv.end());

    auto path = vec[0];
    auto x1 = util::parseNum(vec[1]);
    auto y1 = util::parseNum(vec[2]);
    auto x2 = util::parseNum(vec[3]);
    auto y2 = util::parseNum(vec[4]);
    
    // Assemble the target file names
    string rawFile = "/tmp/" + path + ".raw";
    string tiffFile = "/tmp/" + path + ".tiff";

    // Open an output stream
    file.open(rawFile.c_str());
    
    // Dump texture
    vic.dumpTexture(file, x1, y1, x2, y2);
    file.close();
    
    // Convert raw data into a TIFF file
    string cmd = "/usr/local/bin/raw2tiff";
    cmd += " -p rgb -b 3";
    cmd += " -w " + std::to_string(x2 - x1);
    cmd += " -l " + std::to_string(y2 - y1);
    cmd += " " + rawFile + " " + tiffFile;
    
    msg("Executing %s\n", cmd.c_str());

    if (system(cmd.c_str()) == -1) {
        warn("Error executing %s\n", cmd.c_str());
    }
    exit(0);
}


//
// C64
//

template <> void
RetroShell::exec <Token::c64, Token::inspect> (Arguments &argv, long param)
{
    dump(c64, dump::State);
}

template <> void
RetroShell::exec <Token::c64, Token::config> (Arguments &argv, long param)
{
    dump(c64, dump::Config);
}

template <> void
RetroShell::exec <Token::c64, Token::power, Token::on> (Arguments &argv, long param)
{
    c64.powerOn();
}

template <> void
RetroShell::exec <Token::c64, Token::power, Token::off> (Arguments &argv, long param)
{
    c64.powerOff();
}

template <> void
RetroShell::exec <Token::c64, Token::run> (Arguments &argv, long param)
{
    c64.run();
}

template <> void
RetroShell::exec <Token::c64, Token::pause> (Arguments &argv, long param)
{
    c64.pause();
}

template <> void
RetroShell::exec <Token::c64, Token::reset> (Arguments &argv, long param)
{
    c64.hardReset();
}

template <> void
RetroShell::exec <Token::c64, Token::init> (Arguments &argv, long param)
{
    auto model = util::parseEnum <C64ModelEnum> (argv.front());
    c64.initialize(model);
}


//
// Memory
//

template <> void
RetroShell::exec <Token::memory, Token::config> (Arguments& argv, long param)
{
    dump(c64.mem, dump::Config);
}

template <> void
RetroShell::exec <Token::memory, Token::set, Token::raminitpattern> (Arguments& argv, long param)
{
    c64.configure(OPT_RAM_PATTERN, util::parseEnum <RamPatternEnum> (argv.front()));
}

template <> void
RetroShell::exec <Token::memory, Token::load> (Arguments& argv, long param)
{
    c64.loadRom(argv.front());
}

template <> void
RetroShell::exec <Token::memory, Token::flash> (Arguments& argv, long param)
{
    auto path = argv.front();
    if (!util::fileExists(path)) throw VC64Error(ERROR_FILE_NOT_FOUND, path);

    PRGFile *file = AnyFile::make <PRGFile> (argv.front());
    c64.flash(file, 0);
    delete file;
}

template <> void
RetroShell::exec <Token::memory, Token::inspect> (Arguments& argv, long param)
{
    dump(c64.mem, dump::State);
}


//
// Drive
//

template <> void
RetroShell::exec <Token::drive, Token::config> (Arguments& argv, long param)
{
    auto &drive = param ? c64.drive9 : c64.drive8;
    dump(drive, dump::Config);
}

template <> void
RetroShell::exec <Token::drive, Token::connect> (Arguments& argv, long param)
{
    auto id = param ? DRIVE9 : DRIVE8;
    c64.configure(OPT_DRIVE_CONNECT, id, true);
}

template <> void
RetroShell::exec <Token::drive, Token::disconnect> (Arguments& argv, long param)
{
    auto id = param ? DRIVE9 : DRIVE8;
    c64.configure(OPT_DRIVE_CONNECT, id, false);
}

template <> void
RetroShell::exec <Token::drive, Token::eject> (Arguments& argv, long param)
{
    auto &drive = param ? c64.drive9 : c64.drive8;
    drive.ejectDisk();
}

template <> void
RetroShell::exec <Token::drive, Token::insert> (Arguments& argv, long param)
{
    auto path = argv.front();
    if (!util::fileExists(path)) throw VC64Error(ERROR_FILE_NOT_FOUND, path);

    auto &drive = param ? c64.drive9 : c64.drive8;
    drive.insertDisk(path, false);
}

template <> void
RetroShell::exec <Token::drive, Token::insert, Token::newdisk> (Arguments& argv, long param)
{

    auto type = util::parseEnum <DOSTypeEnum> (argv.front());
    auto &drive = param ? c64.drive9 : c64.drive8;
    drive.insertNewDisk(type);
}

template <> void
RetroShell::exec <Token::drive, Token::inspect, Token::state> (Arguments& argv, long param)
{
    auto &drive = param ? c64.drive9 : c64.drive8;
    dump(drive, dump::State);
}

template <> void
RetroShell::exec <Token::drive, Token::inspect, Token::disk> (Arguments& argv, long param)
{
    auto &drive = param ? c64.drive9 : c64.drive8;
    dump(drive, dump::Disk);
}


//
// Datasette
//

template <> void
RetroShell::exec <Token::datasette, Token::inspect> (Arguments& argv, long param)
{
    dump(c64.datasette, dump::State);
}

template <> void
RetroShell::exec <Token::datasette, Token::rewind> (Arguments& argv, long param)
{
    datasette.rewind();
}

template <> void
RetroShell::exec <Token::datasette, Token::rewind, Token::to> (Arguments& argv, long param)
{
    auto value = util::parseNum(argv.front());
    datasette.rewind(value);
}


//
// CPU
//

template <> void
RetroShell::exec <Token::cpu, Token::inspect, Token::state> (Arguments& argv, long param)
{
    dump(c64.cpu, dump::State);
}

template <> void
RetroShell::exec <Token::cpu, Token::inspect, Token::registers> (Arguments& argv, long param)
{
    dump(c64.cpu, dump::Registers);
}

//
// CIA
//

template <> void
RetroShell::exec <Token::cia, Token::config> (Arguments &argv, long param)
{
    if (param == 0) {
        dump(c64.cia1, dump::Config);
    } else {
        dump(c64.cia2, dump::Config);
    }
}

template <> void
RetroShell::exec <Token::cia, Token::set, Token::revision> (Arguments &argv, long param)
{
    auto value = util::parseEnum <CIARevisionEnum> (argv.front());
    
    if (param == 0) {
        c64.cia1.configure(OPT_CIA_REVISION, value);
    } else {
        c64.cia2.configure(OPT_CIA_REVISION, value);
    }
}

template <> void
RetroShell::exec <Token::cia, Token::set, Token::timerbbug> (Arguments &argv, long param)
{
    if (param == 0) {
        c64.cia1.configure(OPT_TIMER_B_BUG, util::parseBool(argv.front()));
    } else {
        c64.cia2.configure(OPT_TIMER_B_BUG, util::parseBool(argv.front()));
    }
}

template <> void
RetroShell::exec <Token::cia, Token::inspect, Token::state> (Arguments& argv, long param)
{
    if (param == 0) {
        dump(c64.cia1, dump::State);
    } else {
        dump(c64.cia2, dump::State);
    }
}

template <> void
RetroShell::exec <Token::cia, Token::inspect, Token::registers> (Arguments& argv, long param)
{
    if (param == 0) {
        dump(c64.cia1, dump::Registers);
    } else {
        dump(c64.cia2, dump::Registers);
    }
}

template <> void
RetroShell::exec <Token::cia, Token::inspect, Token::tod> (Arguments& argv, long param)
{
    if (param == 0) {
        dump(c64.cia1.tod, dump::State);
    } else {
        dump(c64.cia2.tod, dump::State);
    }
}

//
// VICII
//

template <> void
RetroShell::exec <Token::vicii, Token::config> (Arguments& argv, long param)
{
    dump(c64.vic, dump::Config);
}

template <> void
RetroShell::exec <Token::vicii, Token::set, Token::revision> (Arguments &argv, long param)
{
    c64.configure(OPT_VIC_REVISION, util::parseEnum <VICIIRevisionEnum> (argv.front()));
}

template <> void
RetroShell::exec <Token::vicii, Token::set, Token::graydotbug> (Arguments &argv, long param)
{
    c64.configure(OPT_GRAY_DOT_BUG, util::parseBool(argv.front()));
}

template <> void
RetroShell::exec <Token::vicii, Token::set, Token::gluelogic> (Arguments &argv, long param)
{
    c64.configure(OPT_GLUE_LOGIC, util::parseBool(argv.front()));
}

template <> void
RetroShell::exec <Token::vicii, Token::set, Token::sscollisions> (Arguments &argv, long param)
{
    c64.configure(OPT_SS_COLLISIONS, util::parseBool(argv.front()));
}

template <> void
RetroShell::exec <Token::vicii, Token::set, Token::sbcollisions> (Arguments &argv, long param)
{
    c64.configure(OPT_SB_COLLISIONS, util::parseBool(argv.front()));
}

template <> void
RetroShell::exec <Token::vicii, Token::inspect, Token::registers> (Arguments& argv, long param)
{
    dump(c64.vic, dump::Registers);
}

template <> void
RetroShell::exec <Token::vicii, Token::inspect, Token::state> (Arguments& argv, long param)
{
    dump(c64.vic, dump::State);
}


//
// DMA Debugger
//

template <> void
RetroShell::exec <Token::dmadebugger, Token::config> (Arguments& argv, long param)
{
    dump(c64.vic.dmaDebugger, dump::Config);
}

template <> void
RetroShell::exec <Token::dmadebugger, Token::open> (Arguments& argv, long param)
{
    c64.configure(OPT_DMA_DEBUG_ENABLE, true);
}

template <> void
RetroShell::exec <Token::dmadebugger, Token::close> (Arguments& argv, long param)
{
    c64.configure(OPT_DMA_DEBUG_ENABLE, false);
}

template <> void
RetroShell::exec <Token::dmadebugger, Token::show, Token::raccesses> (Arguments& argv, long param)
{
    c64.configure(OPT_DMA_DEBUG_ENABLE, 0, true);
}

template <> void
RetroShell::exec <Token::dmadebugger, Token::show, Token::iaccesses> (Arguments& argv, long param)
{
    c64.configure(OPT_DMA_DEBUG_ENABLE, 1, true);
}

template <> void
RetroShell::exec <Token::dmadebugger, Token::show, Token::caccesses> (Arguments& argv, long param)
{
    c64.configure(OPT_DMA_DEBUG_ENABLE, 2, true);
}

template <> void
RetroShell::exec <Token::dmadebugger, Token::show, Token::gaccesses> (Arguments& argv, long param)
{
    c64.configure(OPT_DMA_DEBUG_ENABLE, 3, true);
}

template <> void
RetroShell::exec <Token::dmadebugger, Token::show, Token::paccesses> (Arguments& argv, long param)
{
    c64.configure(OPT_DMA_DEBUG_ENABLE, 4, true);
}

template <> void
RetroShell::exec <Token::dmadebugger, Token::show, Token::saccesses> (Arguments& argv, long param)
{
    c64.configure(OPT_DMA_DEBUG_ENABLE, 5, true);
}

template <> void
RetroShell::exec <Token::dmadebugger, Token::hide, Token::raccesses> (Arguments& argv, long param)
{
    c64.configure(OPT_DMA_DEBUG_ENABLE, 0, false);
}

template <> void
RetroShell::exec <Token::dmadebugger, Token::hide, Token::iaccesses> (Arguments& argv, long param)
{
    c64.configure(OPT_DMA_DEBUG_ENABLE, 1, false);
}

template <> void
RetroShell::exec <Token::dmadebugger, Token::hide, Token::caccesses> (Arguments& argv, long param)
{
    c64.configure(OPT_DMA_DEBUG_ENABLE, 2, false);
}

template <> void
RetroShell::exec <Token::dmadebugger, Token::hide, Token::gaccesses> (Arguments& argv, long param)
{
    c64.configure(OPT_DMA_DEBUG_ENABLE, 3, false);
}

template <> void
RetroShell::exec <Token::dmadebugger, Token::hide, Token::paccesses> (Arguments& argv, long param)
{
    c64.configure(OPT_DMA_DEBUG_ENABLE, 4, false);
}

template <> void
RetroShell::exec <Token::dmadebugger, Token::hide, Token::saccesses> (Arguments& argv, long param)
{
    c64.configure(OPT_DMA_DEBUG_ENABLE, 5, false);
}

/*
template <> void
RetroShell::exec <Token::dmadebugger, Token::inspect> (Arguments& argv, long param)
{
    dump(c64.vic.dmaDebugger, dump::State);
}
*/


//
// Monitor
//

template <> void
RetroShell::exec <Token::monitor, Token::set, Token::palette> (Arguments& argv, long param)
{
    c64.configure(OPT_PALETTE, util::parseEnum <PaletteEnum> (argv.front()));
}

template <> void
RetroShell::exec <Token::monitor, Token::set, Token::brightness> (Arguments& argv, long param)
{
    c64.configure(OPT_BRIGHTNESS, util::parseNum(argv.front()));
}

template <> void
RetroShell::exec <Token::monitor, Token::set, Token::contrast> (Arguments& argv, long param)
{
    c64.configure(OPT_CONTRAST, util::parseNum(argv.front()));
}

template <> void
RetroShell::exec <Token::monitor, Token::set, Token::saturation> (Arguments& argv, long param)
{
    c64.configure(OPT_SATURATION, util::parseNum(argv.front()));
}


//
// SID
//

template <> void
RetroShell::exec <Token::sid, Token::config> (Arguments& argv, long param)
{
    dump(c64.sid, dump::Config);
}

template <> void
RetroShell::exec <Token::sid, Token::set, Token::engine> (Arguments &argv, long param)
{
    auto value = util::parseEnum <SIDEngineEnum> (argv.front());
    c64.configure(OPT_SID_ENGINE, value);
}

template <> void
RetroShell::exec <Token::sid, Token::set, Token::revision> (Arguments &argv, long param)
{
    auto value = util::parseEnum <SIDRevisionEnum> (argv.front());
    c64.configure(OPT_SID_REVISION, value);
}

template <> void
RetroShell::exec <Token::sid, Token::set, Token::sampling> (Arguments& argv, long param)
{
    auto value = util::parseEnum <SamplingMethodEnum> (argv.front());
    c64.configure(OPT_SID_SAMPLING, value);
}

template <> void
RetroShell::exec <Token::sid, Token::set, Token::filter> (Arguments& argv, long param)
{
    auto value = util::parseBool(argv.front());
    c64.configure(OPT_SID_FILTER, value);
}

template <> void
RetroShell::exec <Token::sid, Token::set, Token::volume> (Arguments& argv, long param)
{
    auto value = util::parseNum(argv.front());
    
    switch (param) {
            
        case 0: c64.configure(OPT_AUDVOL, 0, value); break;
        case 1: c64.configure(OPT_AUDVOL, 1, value); break;
        case 2: c64.configure(OPT_AUDVOL, 2, value); break;
        case 3: c64.configure(OPT_AUDVOL, 3, value); break;
        case 4: c64.configure(OPT_AUDVOLL, value); break;
        case 5: c64.configure(OPT_AUDVOLR, value); break;
            
        default:
            assert(false);
    }
}

template <> void
RetroShell::exec <Token::sid, Token::set, Token::pan> (Arguments& argv, long param)
{
    auto value = util::parseNum(argv.front());
    c64.configure(OPT_AUDPAN, param, value);
}

template <> void
RetroShell::exec <Token::sid, Token::inspect, Token::sid> (Arguments& argv, long param)
{
    dump(c64.sid, dump::State);
}

template <> void
RetroShell::exec <Token::sid, Token::inspect, Token::state> (Arguments& argv, long param)
{
    auto value = util::parseNum(argv.front());
    if (value < 0 || value > 3) throw VC64Error(ERROR_OPT_INV_ARG, "0, 1, 2, or 3");
    dump(c64.sid.getSID(value), dump::State);
}

template <> void
RetroShell::exec <Token::sid, Token::inspect, Token::registers> (Arguments& argv, long param)
{
    auto value = util::parseNum(argv.front());
    if (value < 0 || value > 3) throw VC64Error(ERROR_OPT_INV_ARG, "0, 1, 2, or 3");
    dump(c64.sid.getSID(value), dump::Registers);
}


//
// Control port
//

template <> void
RetroShell::exec <Token::controlport, Token::inspect> (Arguments& argv, long param)
{
    dump(param == 0 ? c64.port1 : c64.port2, dump::State);
}


//
// Expansion port
//

template <> void
RetroShell::exec <Token::expansion, Token::inspect> (Arguments& argv, long param)
{
    dump(expansionport, dump::State);
}

template <> void
RetroShell::exec <Token::expansion, Token::attach> (Arguments& argv, long param)
{
    auto path = argv.front();
    if (!util::fileExists(path)) throw VC64Error(ERROR_FILE_NOT_FOUND, path);

    expansionport.attachCartridge(path);
}


//
// Keyboard
//

template <> void
RetroShell::exec <Token::keyboard, Token::inspect> (Arguments& argv, long param)
{
    dump(c64.keyboard, dump::State);
}

template <> void
RetroShell::exec <Token::keyboard, Token::type> (Arguments& argv, long param)
{
    keyboard.autoType(argv.front());
}

template <> void
RetroShell::exec <Token::keyboard, Token::type, Token::load> (Arguments& argv, long param)
{
    keyboard.autoType("load \"*\",8,1\n");
}

template <> void
RetroShell::exec <Token::keyboard, Token::type, Token::run> (Arguments& argv, long param)
{
    keyboard.autoType("run\n");
}

template <> void
RetroShell::exec <Token::keyboard, Token::press> (Arguments& argv, long param)
{
    isize key = util::parseNum(argv.front());

    keyboard.press(C64Key(key));
}

template <> void
RetroShell::exec <Token::keyboard, Token::press, Token::shiftlock> (Arguments& argv, long param)
{
    keyboard.pressShiftLock();
}

template <> void
RetroShell::exec <Token::keyboard, Token::release> (Arguments& argv, long param)
{
    isize key = util::parseNum(argv.front());

    keyboard.release(C64Key(key));
}

template <> void
RetroShell::exec <Token::keyboard, Token::release, Token::shiftlock> (Arguments& argv, long param)
{
    keyboard.releaseShiftLock();
}


//
// Joystick
//

template <> void
RetroShell::exec <Token::joystick, Token::config> (Arguments& argv, long param)
{
    dump(c64.port1.joystick, dump::Config);
    *this << '\n';
    dump(c64.port2.joystick, dump::Config);
}

template <> void
RetroShell::exec <Token::joystick, Token::inspect> (Arguments& argv, long param)
{
    dump(c64.port1.joystick, dump::State);
    *this << '\n';
    dump(c64.port2.joystick, dump::State);
}

template <> void
RetroShell::exec <Token::joystick, Token::set, Token::autofire> (Arguments& argv, long param)
{
    auto value = util::parseBool(argv.front());
    c64.configure(OPT_AUTOFIRE, value);
}

template <> void
RetroShell::exec <Token::joystick, Token::set, Token::bullets> (Arguments& argv, long param)
{
    auto value = util::parseNum(argv.front());
    c64.configure(OPT_AUTOFIRE_BULLETS, value);
}

template <> void
RetroShell::exec <Token::joystick, Token::set, Token::delay> (Arguments& argv, long param)
{
    auto value = util::parseNum(argv.front());
    c64.configure(OPT_AUTOFIRE_DELAY, value);
}


//
// Mouse
//

template <> void
RetroShell::exec <Token::mouse, Token::config> (Arguments& argv, long param)
{
    dump(c64.port1.mouse, dump::Config);
    *this << '\n';
    dump(c64.port2.mouse, dump::Config);
}

template <> void
RetroShell::exec <Token::mouse, Token::inspect> (Arguments& argv, long param)
{
    dump(c64.port1.mouse, dump::State);
    *this << '\n';
    dump(c64.port2.mouse, dump::State);
}

template <> void
RetroShell::exec <Token::mouse, Token::set, Token::model> (Arguments &argv, long param)
{
    auto value = util::parseEnum <MouseModelEnum> (argv.front());
    c64.configure(OPT_MOUSE_MODEL, value);
}

template <> void
RetroShell::exec <Token::mouse, Token::set, Token::velocity> (Arguments &argv, long param)
{
    auto value = util::parseNum(argv.front());
    c64.configure(OPT_MOUSE_VELOCITY, value);
}

template <> void
RetroShell::exec <Token::mouse, Token::set, Token::shakedetector> (Arguments &argv, long param)
{
    auto value = util::parseBool(argv.front());
    c64.configure(OPT_SHAKE_DETECTION, value);
}
