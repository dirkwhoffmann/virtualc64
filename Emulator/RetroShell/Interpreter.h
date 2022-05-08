// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "SubComponent.h"
#include "Command.h"
#include "Exception.h"
#include "Error.h"
#include "Parser.h"

enum class Token
{
    about, accuracy, attach, audiate, autofire, autosync, bankmap, brightness,
    bullets, c64, caccesses, checksums, chip, cia, clear, close, config,
    connect, contrast, controlport, counter, cpu, cutout, datasette, defaultbb,
    defaultfs, defaults, delay, device, devices, disconnect, disk, dmadebugger,
    drive, dsksync, easteregg, eject, engine, events, expansion, fastsid,
    filename, filter, flash, frame, gaccesses, gluelogic, graydotbug, hide,
    iaccesses, idle, init, insert, inspect, joystick, keyboard, keyset, left,
    list, load, lock, memory, model, monitor, mouse, newdisk, none, off, on,
    open, paccesses, palette, pan, parcable, pause, poll, power, press,
    raccesses, raminitpattern, registers, regression, release, reset, resid,
    revision, rewind, right, rom, run, saccesses, sampling, saturation, save,
    saveroms, sbcollisions, screenshot, searchpath, set, setup, shakedetector,
    shiftlock, show, sid, slow, slowramdelay, slowrammirror, source, speed,
    sscollisions, state, step, to, tod, timerbbug, type, unmappingtype,
    velocity, vicii, volume, wait
};

struct TooFewArgumentsError : public util::ParseError {
    using ParseError::ParseError;
};

struct TooManyArgumentsError : public util::ParseError {
    using ParseError::ParseError;
};

struct ScriptInterruption: util::Exception {
    using Exception::Exception;
};

class Interpreter: SubComponent
{
    // The registered instruction set
    Command root;
    
    
    //
    // Initializing
    //

public:
    
    Interpreter(C64 &ref) : SubComponent(ref) { registerInstructions(); }

    const char *getDescription() const override { return "Interpreter"; }

private:
    
    // Registers the instruction set
    void registerInstructions();

    void _reset(bool hard) override { }
    
    
    //
    // Serializing
    //

private:

    isize _size() override { return 0; }
    u64 _checksum() override { return 0; }
    isize _load(const u8 *buffer) override {return 0; }
    isize _save(u8 *buffer) override { return 0; }

    
    //
    // Parsing input
    //
    
public:
    
    // Auto-completes a user command
    string autoComplete(const string& userInput);
    
private:
    
    // Splits an input string into an argument list
    Arguments split(const string& userInput);

    // Auto-completes an argument list
    void autoComplete(Arguments &argv);

    
    //
    // Executing commands
    //
    
public:
    
    // Executes a single command
    void exec(const string& userInput, bool verbose = false) throws;
    void exec(const Arguments &argv, bool verbose = false) throws;
            
    // Prints a usage string for a command
    void usage(const Command &command);
    
    // Displays a help text for a (partially typed in) command
    void help(const string &userInput);
    void help(const Arguments &argv);
    void help(const Command &command);

};
