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

#include "RetroShellTypes.h"
#include "SubComponent.h"
#include "Command.h"
#include "Parser.h"
#include "TextStorage.h"

namespace vc64 {

typedef std::pair<isize, string> QueuedCmd;

struct TooFewArgumentsError : public util::ParseError {
    using ParseError::ParseError;
};

struct TooManyArgumentsError : public util::ParseError {
    using ParseError::ParseError;
};

struct ScriptInterruption: util::Exception {
    using Exception::Exception;
};

class Console : public SubComponent
{
    friend class RetroShell;
    friend class RshServer;
    friend class Interpreter;

    Descriptions descriptions = {
        {
            .name           = "CmdConsole",
            .description    = "Command shell",
            .shell          = ""
        },
        {
            .name           = "DbgConsole",
            .description    = "Debug shell",
            .shell          = ""
        }
    };

    Options options = {

    };

protected:

    // Root node of the command tree
    Command root;


    //
    // Text storage
    //

protected:

    // The text storage
    TextStorage storage;

    // History buffer storing old input strings and cursor positions
    std::vector<std::pair<string,isize>> history;

    // The currently active input string
    isize ipos = 0;


    //
    // User input
    //

protected:

    // Input line
    string input;

    // Cursor position
    isize cursor = 0;

    // Indicates if TAB was the most recently pressed key
    bool tabPressed = false;


    //
    // Initializing
    //

public:

    using SubComponent::SubComponent;
    Console& operator= (const Console& other) { return *this; }

protected:

    virtual void initCommands(Command &root);
    const char *registerComponent(CoreComponent &c);
    const char *registerComponent(CoreComponent &c, Command &root);


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


    //
    // Methods from Configurable
    //

public:

    const Options &getOptions() const override { return options; }


    //
    // Delegates
    //

    // Called from RetroShell when the console is entered
    virtual void _enter() { };


    //
    // Working with the text storage
    //

public:
    
    // Prints a message
    Console &operator<<(char value);
    Console &operator<<(const string &value);
    Console &operator<<(int value);
    Console &operator<<(unsigned int value);
    Console &operator<<(long value);
    Console &operator<<(unsigned long value);
    Console &operator<<(long long value);
    Console &operator<<(unsigned long long value);
    Console &operator<<(std::stringstream &stream);

    // Returns the prompt
    virtual string getPrompt() = 0;

    // Returns the contents of the whole storage as a single C string
    const char *text();

    // Moves the cursor forward to a certain column
    void tab(isize pos);

    // Assigns an additional output stream
    void setStream(std::ostream &os);

    // Marks the text storage as dirty
    void needsDisplay();

protected:

    // Clears the console window
    void clear();

    // Returns true if the console is cleared
    bool isEmpty();

    // Returns true if the last line contains no text
    bool lastLineIsEmpty();

    // Prints the welcome message
    virtual void welcome() = 0;

    // Prints the help line
    virtual void printHelp() = 0;

    // Prints a state summary (used by the debug shell)
    void printState();


    //
    // Managing user input
    //

public:

    // Returns the size of the current user-input string
    isize inputLength() { return (isize)input.length(); }

    // Presses a key or a series of keys
    void press(RetroShellKey key, bool shift = false);
    void press(char c);
    void press(const string &s);

    // Returns the cursor position relative to the line end
    isize cursorRel();

protected:

    virtual void pressReturn(bool shift);


    //
    // Working with the history buffer
    //

public:

    isize historyLength() { return (isize)history.size(); }


    //
    // Parsing input
    //

public:

    // Auto-completes a user command
    string autoComplete(const string& userInput);
    
protected:

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

    // Returns the root node of the instruction tree
    Command &getRoot() { return root; }

protected:

    // Executes a single command
    void exec(const string& userInput, bool verbose = false) throws;
    void exec(const Arguments &argv, bool verbose = false) throws;

    // Prints a usage string for a command
    void usage(const Command &command);

    // Displays a help text for a (partially typed in) command
    void help(const string &userInput);
    void help(const Arguments &argv);
    void help(const Command &command);

    // Prints a textual description of an error in the console
    void describe(const std::exception &exception, isize line = 0, const string &cmd = "");


    //
    // Command handlers
    //

public:

    void dump(CoreObject &component, std::vector <Category> categories);
    void dump(CoreObject &component, Category category);

protected:

    void _dump(CoreObject &component, Category category);
};

class CommandConsole : public Console
{
    using Console::Console;

    virtual void initCommands(Command &root) override;
    void _enter() override;
    void _pause() override;
    string getPrompt() override;
    void welcome() override;
    void printHelp() override;
    void pressReturn(bool shift) override;
};

class DebugConsole : public Console
{
    using Console::Console;

    virtual void initCommands(Command &root) override;
    void _enter() override;
    void _pause() override;
    string getPrompt() override;
    void welcome() override;
    void printHelp() override;
    void pressReturn(bool shift) override;
};

}
