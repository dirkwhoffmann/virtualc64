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
    none,
    
    // Components
    c64, cia, controlport, cpu, datasette, drive, expansion, fastsid, keyboard,
    memory, monitor, mouse, parcable, resid, sid, vicii,

    // Commands
    about, attach, audiate, autosync, clear, close, config, connect, disconnect,
    dmadebugger, dsksync, easteregg, eject, flash, hide, init, insert, inspect,
    list, load, lock, off, on, open, pause, power, press, regression, release,
    reset, rewind, run, save, screenshot, set, setup, show, source, type, wait,
    
    // Categories
    checksums, devices, events, registers, state, disk,
    
    // Keys
    accuracy, autofire, bankmap, brightness, bullets, caccesses, chip,
    contrast, counter, cutout, defaultbb, defaultfs, delay, device, engine,
    filename, filter, frame, gaccesses, gluelogic, graydotbug, iaccesses, idle,
    joystick, keyset, left, model, newdisk, paccesses, palette, pan, poll,
    raccesses, raminitpattern, revision, right, rom, saccesses, sampling,
    saturation, sbcollisions, searchpath, shakedetector, shiftlock, slow,
    slowramdelay, slowrammirror, speed, sscollisions, step, to, tod, timerbbug,
    unmappingtype, velocity, volume
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
