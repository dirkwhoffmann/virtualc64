// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "C64Component.h"
#include "Command.h"
#include "Exception.h"
#include "Error.h"
#include "Parser.h"

typedef std::list<string> Arguments;

enum class Token
{
    none,
    
    // Components
    c64,audio, cia, controlport, cpu, drive, keyboard, memory, monitor, mouse,

    // Commands
    about, audiate, autosync, clear, config, connect, disconnect, dsksync,
    easteregg, eject, close, insert, inspect, list, load, lock, on, off, pause,
    power, reset, run, set, source,
    
    // Categories
    checksums, devices, events, registers, state,
    
    // Keys
    accuracy, bankmap, brightness, chip, clxsprspr, clxsprplf, clxplfplf,
    contrast, defaultbb, defaultfs, device, esync, extrom, extstart, fast,
    filter, joystick, keyset, mechanics, model, palette, pan, poll, pullup,
    raminitpattern, revision, rom, sampling, saturation, searchpath,
    shakedetector, slow, slowramdelay, slowrammirror, speed, step, tod, todbug,
    unmappingtype, velocity, volume, wom
};

struct TooFewArgumentsError : public util::ParseError {
    using ParseError::ParseError;
};

struct TooManyArgumentsError : public util::ParseError {
    using ParseError::ParseError;
};

class Interpreter: C64Component
{
    // The registered instruction set
    Command root;
    
    
    //
    // Initializing
    //

public:
    
    Interpreter(C64 &ref);

    const char *getDescription() const override { return "Interpreter"; }

private:
    
    // Registers the instruction set
    void registerInstructions();

    void _reset() override { }
    
    
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
    
    // Splits an input string into an argument list
    Arguments split(const string& userInput);

    // Auto-completes a command. Returns the number of auto-completed tokens
    void autoComplete(Arguments &argv);
    string autoComplete(const string& userInput);

    
    //
    // Executing commands
    //
    
public:
    
    // Executes a single command
    void exec(const string& userInput, bool verbose = false) throws;
    void exec(Arguments &argv, bool verbose = false) throws;
            
    // Prints a usage string for a command
    void usage(Command &command);
    
    // Displays a help text for a (partially typed in) command
    void help(const string &userInput);
    void help(Arguments &argv);
    void help(Command &command);

};
