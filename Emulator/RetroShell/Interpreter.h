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

namespace vc64 {

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
    enum class Shell { Command, Debug };

    // The currently active shell
    Shell shell = Shell::Command;

    // Commands of the command shell
    Command commandShellRoot;

    // Commands of the debug shell
    Command debugShellRoot;

    
    //
    // Initializing
    //

public:
    
    Interpreter(C64 &ref);

private:

    void initCommons(Command &root);
    void initCommandShell(Command &root);
    void initDebugShell(Command &root);


    //
    // Methods from AmigaObject
    //

private:

    const char *getDescription() const override { return "Interpreter"; }
    void _dump(Category category, std::ostream& os) const override { }


    //
    // Methods from AmigaComponent
    //

private:

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

    // Parses an argument of a certain type
    bool parseBool(Arguments &argv, isize n = 0) { return util::parseBool(argv[n]); }
    bool parseOnOff(Arguments &argv, isize n = 0) { return util::parseOnOff(argv[n]); }
    long parseNum(Arguments &argv, isize n = 0) { return util::parseNum(argv[n]); }
    template <typename T> long parseEnum(Arguments &argv, isize n = 0) { return util::parseEnum<T>(argv[n]); }


    //
    // Managing the interpreter
    //

public:

    // Returns the root node of the currently active instruction tree
    Command &getRoot();

    // Toggles between the command shell and the debug shell
    void switchInterpreter();

    bool inCommandShell() { return shell == Shell::Command; }
    bool inDebugShell() { return shell == Shell::Debug; }

    
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

}
