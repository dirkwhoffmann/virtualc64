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
        throw ConfigFileNotFoundError(argv.front());
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

template <> void
RetroShell::exec <Token::debugcrt> (Arguments &argv, long param)
{
    c64.configure(OPT_DEBUGCART, true);
}

//
// Screenshots (regression testing)
//

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
RetroShell::exec <Token::screenshot, Token::take> (Arguments &argv, long param)
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
    system(cmd.c_str());
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
RetroShell::exec <Token::c64, Token::run, Token::timeout> (Arguments &argv, long param)
{
    auto seconds = util::parseNum(argv.front());
    Cycle cycles = seconds * vic.getFrequency();
    
    c64.configure(OPT_CYCLE_LIMIT, cycles);
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
    c64.reset();
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
    if (!util::fileExists(path)) throw ConfigFileNotFoundError(path);

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
    if (!util::fileExists(path)) throw ConfigFileNotFoundError(path);

    auto &drive = param ? c64.drive9 : c64.drive8;
    drive.insertDisk(path);
}

template <> void
RetroShell::exec <Token::drive, Token::inspect> (Arguments& argv, long param)
{
    auto &drive = param ? c64.drive9 : c64.drive8;
    dump(drive, dump::State);
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
RetroShell::exec <Token::vicii, Token::inspect, Token::state> (Arguments& argv, long param)
{
    dump(c64.vic, dump::State);
}


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
    if (value < 0 || value > 3) throw ConfigArgError("0, 1, 2, or 3");
    dump(c64.sid.getSID(value), dump::State);
}

template <> void
RetroShell::exec <Token::sid, Token::inspect, Token::registers> (Arguments& argv, long param)
{
    auto value = util::parseNum(argv.front());
    if (value < 0 || value > 3) throw ConfigArgError("0, 1, 2, or 3");
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


//
// Serial port
//

/*
template <> void
RetroShell::exec <Token::serial, Token::config> (Arguments& argv, long param)
{
    dump(amiga.serialPort, Dump::Config);
}

template <> void
RetroShell::exec <Token::serial, Token::set, Token::device> (Arguments &argv, long param)
{
    auto value = util::parseEnum <SerialPortDeviceEnum> (argv.front());
    amiga.configure(OPT_SERIAL_DEVICE, value);
}

template <> void
RetroShell::exec <Token::serial, Token::inspect> (Arguments& argv, long param)
{
    dump(amiga.serialPort, Dump::State);
}

//
// Disk controller
//

template <> void
RetroShell::exec <Token::dc, Token::config> (Arguments& argv, long param)
{
    dump(amiga.paula.diskController, Dump::Config);
}

template <> void
RetroShell::exec <Token::dc, Token::inspect> (Arguments& argv, long param)
{
    dump(amiga.paula.diskController, Dump::Registers);
}

template <> void
RetroShell::exec <Token::dc, Token::speed> (Arguments& argv, long param)
{
    amiga.configure(OPT_DRIVE_SPEED, util::parseNum(argv.front()));
}

template <> void
RetroShell::exec <Token::dc, Token::dsksync, Token::autosync> (Arguments& argv, long param)
{
    amiga.configure(OPT_AUTO_DSKSYNC, util::parseBool(argv.front()));
}

template <> void
RetroShell::exec <Token::dc, Token::dsksync, Token::lock> (Arguments& argv, long param)
{
    amiga.configure(OPT_LOCK_DSKSYNC, util::parseBool(argv.front()));
}

//
// Df0, Df1, Df2, Df3
//

template <> void
RetroShell::exec <Token::dfn, Token::config> (Arguments& argv, long param)
{
    dump(*amiga.df[param], Dump::Config);
}

template <> void
RetroShell::exec <Token::dfn, Token::audiate, Token::insert> (Arguments& argv, long param)
{
    long num = util::parseNum(argv.front());

    if (param >= 0 && param <= 3) {
        amiga.configure(OPT_INSERT_VOLUME, param, num);
    } else {
        amiga.configure(OPT_INSERT_VOLUME, num);
    }
}

template <> void
RetroShell::exec <Token::dfn, Token::audiate, Token::eject> (Arguments& argv, long param)
{
    long num = util::parseNum(argv.front());
    
    if (param >= 0 && param <= 3) {
        amiga.configure(OPT_EJECT_VOLUME, param, num);
    } else {
        amiga.configure(OPT_EJECT_VOLUME, num);
    }
}

template <> void
RetroShell::exec <Token::dfn, Token::audiate, Token::step> (Arguments& argv, long param)
{
    long num = util::parseNum(argv.front());
    
    if (param >= 0 && param <= 3) {
        amiga.configure(OPT_STEP_VOLUME, param, num);
    } else {
        amiga.configure(OPT_STEP_VOLUME, num);
    }
}

template <> void
RetroShell::exec <Token::dfn, Token::audiate, Token::poll> (Arguments& argv, long param)
{
    long num = util::parseNum(argv.front());
    
    if (param >= 0 && param <= 3) {
        amiga.configure(OPT_POLL_VOLUME, param, num);
    } else {
        amiga.configure(OPT_POLL_VOLUME, num);
    }
}

template <> void
RetroShell::exec <Token::dfn, Token::eject> (Arguments& argv, long param)
{
    amiga.df[param]->ejectDisk();
}

template <> void
RetroShell::exec <Token::dfn, Token::connect> (Arguments& argv, long param)
{
    amiga.configure(OPT_DRIVE_CONNECT, param, true);
}

template <> void
RetroShell::exec <Token::dfn, Token::disconnect> (Arguments& argv, long param)
{
    amiga.configure(OPT_DRIVE_CONNECT, param, false);
}

template <> void
RetroShell::exec <Token::dfn, Token::insert> (Arguments& argv, long param)
{
    auto path = argv.front();
    if (!util::fileExists(path)) throw ConfigFileNotFoundError(path);
    
    amiga.paula.diskController.insertDisk(path, param);
}

template <> void
RetroShell::exec <Token::dfn, Token::set, Token::model> (Arguments& argv, long param)
{
    long num = util::parseEnum <DriveTypeEnum> (argv.front());
    
    if (param >= 0 && param <= 3) {
        amiga.configure(OPT_DRIVE_TYPE, param, num);
    } else {
        amiga.configure(OPT_DRIVE_TYPE, num);
    }
}

template <> void
RetroShell::exec <Token::dfn, Token::set, Token::pan> (Arguments& argv, long param)
{
    long num = util::parseNum(argv.front());
    
    if (param >= 0 && param <= 3) {
        amiga.configure(OPT_DRIVE_PAN, param, num);
    } else {
        amiga.configure(OPT_DRIVE_PAN, num);
    }
}

template <> void
RetroShell::exec <Token::dfn, Token::set, Token::mechanics> (Arguments& argv, long param)
{
    long num = util::parseBool(argv.front());
    
    if (param >= 0 && param <= 3) {
        amiga.configure(OPT_EMULATE_MECHANICS, param, num);
    } else {
        amiga.configure(OPT_EMULATE_MECHANICS, num);
    }
}

template <> void
RetroShell::exec <Token::dfn, Token::set, Token::searchpath> (Arguments& argv, long param)
{
    string path = argv.front();
    
    if (param >= 0 && param <= 3) {
        amiga.paula.diskController.setSearchPath(path, param);
    } else {
        amiga.paula.diskController.setSearchPath(path);
    }
}

template <> void
RetroShell::exec <Token::dfn, Token::set, Token::defaultbb> (Arguments& argv, long param)
{
    long num = util::parseEnum <BootBlockIdEnum> (argv.front());
    
    if (param >= 0 && param <= 3) {
        amiga.configure(OPT_DEFAULT_BOOTBLOCK, param, num);
    } else {
        amiga.configure(OPT_DEFAULT_BOOTBLOCK, num);
    }
}

template <> void
RetroShell::exec <Token::dfn, Token::set, Token::defaultfs> (Arguments& argv, long param)
{
    long num = util::parseEnum <FSVolumeTypeEnum> (argv.front());
    
    if (param >= 0 && param <= 3) {
        amiga.configure(OPT_DEFAULT_FILESYSTEM, param, num);
    } else {
        amiga.configure(OPT_DEFAULT_FILESYSTEM, num);
    }
}

template <> void
RetroShell::exec <Token::dfn, Token::inspect> (Arguments& argv, long param)
{
    dump(*amiga.df[param], Dump::State);
}

*/
