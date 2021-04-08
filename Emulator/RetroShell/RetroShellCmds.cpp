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
    string filename = argv.front();
    
    std::ifstream stream(filename);
    if (!stream.is_open()) throw ConfigFileReadError(filename);
    
    try {
        exec(stream);
    } catch (util::Exception &e) {
        retroShell << "Error in line " << (isize)e.data << '\n';
        retroShell << e.what() << '\n';
    }
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
    c64.reset();
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
RetroShell::exec <Token::memory, Token::load> (Arguments& argv, long param)
{
    c64.loadRom(argv.front());
}

template <> void
RetroShell::exec <Token::memory, Token::set, Token::raminitpattern> (Arguments& argv, long param)
{
    c64.configure(OPT_RAM_PATTERN, util::parseEnum <RamPatternEnum> (argv.front()));
}

template <> void
RetroShell::exec <Token::memory, Token::inspect> (Arguments& argv, long param)
{
    dump(c64.mem, dump::State);
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

/*
template <> void
RetroShell::exec <Token::vicii, Token::inspect, Token::registers> (Arguments& argv, long param)
{
    dump(c64.vic, dump::Registers);
}
*/

//
// Monitor
//

/*
template <> void
RetroShell::exec <Token::monitor, Token::set, Token::palette> (Arguments& argv, long param)
{
    amiga.configure(OPT_PALETTE, util::parseEnum <PaletteEnum> (argv.front()));
}

template <> void
RetroShell::exec <Token::monitor, Token::set, Token::brightness> (Arguments& argv, long param)
{
    amiga.configure(OPT_BRIGHTNESS, util::parseNum(argv.front()));
}

template <> void
RetroShell::exec <Token::monitor, Token::set, Token::contrast> (Arguments& argv, long param)
{
    amiga.configure(OPT_CONTRAST, util::parseNum(argv.front()));
}

template <> void
RetroShell::exec <Token::monitor, Token::set, Token::saturation> (Arguments& argv, long param)
{
    amiga.configure(OPT_SATURATION, util::parseNum(argv.front()));
}
*/

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
// Paula
//

/*
template <> void
RetroShell::exec <Token::paula, Token::inspect, Token::state> (Arguments& argv, long param)
{
    dump(amiga.paula, Dump::State);
}

template <> void
RetroShell::exec <Token::paula, Token::inspect, Token::registers> (Arguments& argv, long param)
{
    dump(amiga.paula, Dump::Registers);
}

//
// RTC
//

template <> void
RetroShell::exec <Token::rtc, Token::config> (Arguments& argv, long param)
{
    dump(amiga.rtc, Dump::Config);
}

template <> void
RetroShell::exec <Token::rtc, Token::inspect, Token::registers> (Arguments& argv, long param)
{
    dump(amiga.rtc, Dump::Registers);
}

template <> void
RetroShell::exec <Token::rtc, Token::set, Token::revision> (Arguments &argv, long param)
{
    amiga.configure(OPT_RTC_MODEL, util::parseEnum <RTCRevisionEnum> (argv.front()));
}

//
// Control port
//

template <> void
RetroShell::exec <Token::controlport, Token::config> (Arguments& argv, long param)
{
    dump(param == 0 ? amiga.controlPort1 : amiga.controlPort2, Dump::Config);
}

template <> void
RetroShell::exec <Token::controlport, Token::inspect> (Arguments& argv, long param)
{
    dump(param == 0 ? amiga.controlPort1 : amiga.controlPort2, Dump::State);
}

//
// Keyboard
//

template <> void
RetroShell::exec <Token::keyboard, Token::config> (Arguments& argv, long param)
{
    dump(amiga.keyboard, Dump::Config);
}

template <> void
RetroShell::exec <Token::keyboard, Token::set, Token::accuracy> (Arguments &argv, long param)
{
    amiga.configure(OPT_ACCURATE_KEYBOARD, util::parseBool(argv.front()));
}

template <> void
RetroShell::exec <Token::keyboard, Token::inspect> (Arguments& argv, long param)
{
    dump(amiga.keyboard, Dump::State);
}

//
// Mouse
//

template <> void
RetroShell::exec <Token::mouse, Token::config> (Arguments& argv, long param)
{
    dump(amiga.controlPort1.mouse, Dump::Config);
}

template <> void
RetroShell::exec <Token::mouse, Token::set, Token::pullup> (Arguments &argv, long param)
{
    amiga.configure(OPT_PULLUP_RESISTORS, PORT_1, util::parseBool(argv.front()));
    amiga.configure(OPT_PULLUP_RESISTORS, PORT_2, util::parseBool(argv.front()));
}

template <> void
RetroShell::exec <Token::mouse, Token::set, Token::shakedetector> (Arguments &argv, long param)
{
    amiga.configure(OPT_SHAKE_DETECTION, PORT_1, util::parseBool(argv.front()));
    amiga.configure(OPT_SHAKE_DETECTION, PORT_2, util::parseBool(argv.front()));
}

template <> void
RetroShell::exec <Token::mouse, Token::set, Token::velocity> (Arguments &argv, long param)
{
    amiga.configure(OPT_MOUSE_VELOCITY, PORT_1, util::parseNum(argv.front()));
    amiga.configure(OPT_MOUSE_VELOCITY, PORT_2, util::parseNum(argv.front()));
}

template <> void
RetroShell::exec <Token::mouse, Token::inspect> (Arguments& argv, long param)
{
    dump(amiga.keyboard, Dump::State);
}

//
// Serial port
//

template <> void
RetroShell::exec <Token::serial, Token::config> (Arguments& argv, long param)
{
    dump(amiga.serialPort, Dump::Config);
}

template <> void
RetroShell::exec <Token::serial, Token::set, Token::device> (Arguments &argv, long param)
{
    amiga.configure(OPT_SERIAL_DEVICE, util::parseEnum <SerialPortDeviceEnum> (argv.front()));
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
