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

class Interpreter: public SubComponent
{
    Descriptions descriptions = {{

        .name           = "Interpreter",
        .description    = "Command Interpreter"
    }};

    enum class Shell { Command, Debug };

    // The currently active shell
    Shell shell = Shell::Command;

    // Commands of the command shell
    Command commandShellRoot;

    // Commands of the debug shell
    Command debugShellRoot;

    
    //
    // Methods
    //

public:
    
    Interpreter(C64 &ref);
    Interpreter& operator= (const Interpreter& other) { return *this; }


    //
    // Methods from Serializable
    //

public:

    template <class T> void serialize(T& worker) { } SERIALIZERS(serialize);


    //
    // Methods from CoreComponent
    //

public:

    const Descriptions &getDescriptions() const override { return descriptions; }

private:

    void _dump(Category category, std::ostream& os) const override { }
    void _initialize() override;

    void initCommons(Command &root);
    void initCommandShell(Command &root);
    void initDebugShell(Command &root);

public:

    static string shellName(const CoreObject &object);


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
    bool isBool(const string &argv);
    bool parseBool(const string  &argv);
    bool parseBool(const string  &argv, bool fallback);
    bool parseBool(const Arguments &argv, long nr, long fallback);

    bool isOnOff(const string &argv);
    bool parseOnOff(const string &argv);
    bool parseOnOff(const string &argv, bool fallback);
    bool parseOnOff(const Arguments &argv, long nr, long fallback);

    long isNum(const string &argv);
    long parseNum(const string &argv);
    long parseNum(const string &argv, long fallback);
    long parseNum(const Arguments &argv, long nr, long fallback);

    u16 parseAddr(const string &argv) { return (u16)parseNum(argv); }
    u16 parseAddr(const string &argv, long fallback) { return (u16)parseNum(argv, fallback); }
    u16 parseAddr(const Arguments &argv, long nr, long fallback) { return (u16)parseNum(argv, nr, fallback); }

    string parseSeq(const string &argv);
    string parseSeq(const string &argv, const string &fallback);

    template <typename T> long parseEnum(const string &argv) {
        return util::parseEnum<T>(argv);
    }
    template <typename T> long parseEnum(const string &argv, long fallback) {
        try { return util::parseEnum<T>(argv); } catch(...) { return fallback; }
    }
    template <typename E, typename T> E parseEnum(const string &argv) {
        return (E)util::parseEnum<T>(argv);
    }
    template <typename E, typename T> long parseEnum(const string &argv, E fallback) {
        try { return (E)util::parseEnum<T>(argv); } catch(...) { return fallback; }
    }


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


    //
    // Wrappers
    //

    void configure(Option opt, i64 value) throws;
    void configure(Option opt, i64 value, isize id) throws;
};

}
