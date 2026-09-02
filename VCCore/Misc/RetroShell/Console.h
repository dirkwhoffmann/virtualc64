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
#include "RSCommand.h"
#include "TextStorage.h"
#include "utl/io/Parser.h"

namespace vc64 {

class RetroShell;

class ConsoleDelegate {

public:

    virtual ~ConsoleDelegate() { }

    // Called when the console is entered or left
    virtual void didActivate() { };
    virtual void didDeactivate() { };

    // Provides the output of an executed RetroShell command
    virtual void willExecute(const InputLine &input) = 0;
    virtual void didExecute(const InputLine &input, std::stringstream &ss) = 0;
    virtual void didExecute(const InputLine &input, std::stringstream &ss, std::exception &e) = 0;
};

class HistoryBuffer {

    // History buffer storing old input strings and cursor positions
    std::vector<std::pair<string,isize>> history = { { "", 0 } };

    // The currently active input string
    isize ipos = 0;

public:

    // Returns the current selection
    std::pair<string,isize> current() const { return history[ipos]; }

    // Iterate through the buffer
    void up(string &input, isize &ipos);
    void down(string &input, isize &ipos);

    // Add an entry to the buffer
    void add(const string &input);
};

/* The console is the text-based front end of RetroShell. It manages the text
 * storage, the user input line, and the command tree. Which commands are
 * available is determined by the currently active command set. Switching
 * between the different shells (Commander, Debugger) is implemented by
 * rebuilding the command tree from scratch.
 */
class Console final : public SubComponent, public ConsoleDelegate {

    friend class RetroShell;
    friend class Interpreter;

    Descriptions descriptions = {
        {
            .name           = "Console",
            .description    = "Console",
            .shell          = ""
        },
        {
            .name           = "RshConsole",
            .description    = "RetroShell Server Console",
            .shell          = ""
        },
        {
            .name           = "RpcConsole",
            .description    = "RPC Server Console",
            .shell          = ""
        }
    };

    Options options = {

    };

public:

    // Delegates
    std::vector<ConsoleDelegate *> delegates;

protected:

    // The shell this console belongs to
    RetroShell &shell;

    // The currently active command set
    CommandSet commandSet = CommandSet::Commander;

    // Root node of the command tree
    RSCommand root;

    // Memory pointer for commands accpeting default addresses
    u16 current = 0;


    //
    // Command-set specific state
    //

protected:

    // Commander: Indicates if the welcome message has been printed
    bool welcomed = false;


    //
    // Text storage
    //

protected:

    // The text storage
    TextStorage &storage;

    // History buffer storing old input strings and cursor positions
    static HistoryBuffer historyBuffer;


    //
    // User input
    //

protected:

    // Input line
    string input;

    // Cursor position
    isize cursor {};

    // Indicates how often TAB was pressed in a row
    isize tabPressed {};


    //
    // Initializing
    //

public:

    Console(C64 &c64, RetroShell &shell, isize id, TextStorage &storage)
    : SubComponent(c64, id), shell(shell), storage(storage) { };
    Console& operator= (const Console& other) { return *this; }

    // Returns true for the console of the emulator's main shell
    bool isPrimary() const { return objid == 0; }

    /* Identifies this console as an instruction-tracking client. Each console
     * gets its own source bit so that a remote client leaving the Debugger
     * cannot switch off tracking for another shell. Source 0 is used by the
     * GUI's inspector.
     */
    isize trackSource() const { return objid + 1; }


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

protected:

    void _dump(Category category, std::ostream &os) const override { }
    void _initialize() override;
    void _pause() override;


    //
    // Methods from Configurable
    //

public:

    const Options &getOptions() const override { return options; }


    //
    // Methods from ConsoleDelegate
    //

    void didActivate() override;
    void didDeactivate() override;
    void willExecute(const InputLine &input) override;
    void didExecute(const InputLine &input, std::stringstream &ss) override;
    void didExecute(const InputLine &input, std::stringstream &ss, std::exception &e) override;


    //
    // Switching command sets
    //

public:

    // Returns the currently active command set
    CommandSet getCommandSet() const { return commandSet; }

    // Activates another command set (rebuilds the command tree)
    void setCommandSet(CommandSet cs);


    //
    // Working with the text storage
    //

public:

    // Prints a message
    Console &operator<<(char value);
    Console &operator<<(const string &value);
    Console &operator<<(const char *value);
    Console &operator<<(int value);
    Console &operator<<(unsigned int value);
    Console &operator<<(long value);
    Console &operator<<(unsigned long value);
    Console &operator<<(long long value);
    Console &operator<<(unsigned long long value);
    Console &operator<<(const std::vector<string> &vec);
    Console &operator<<(std::stringstream &stream);
    Console &operator<<(const vspace &value);

    // Returns the prompt
    string prompt();

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

private:

    // Additional output inserted before and after command execution
    string vdelim() const; //  = "";


    //
    // Managing user input
    //

public:

    // Returns the size of the current user-input string
    isize inputLength() { return (isize)input.length(); }

    // Presses a key or a series of keys
    void press(RSKey key, bool shift = false);
    void press(char c);
    void press(const string &s);

    // Returns the cursor position relative to the line end
    isize cursorRel() const;

protected:

    void pressReturn(bool shift);


    //
    // Parsing input
    //

public:

    // Auto-completes a user command
    string autoComplete(const string& userInput);

protected:

    // Splits an input string into an argument list
    Tokens split(const string& userInput);

    // Auto-completes an argument list
    void autoComplete(Tokens &argv);

    // Strips off the command tokens and returns a pointer to the command
    std::pair<RSCommand *, std::vector<string>> seekCommand(const string &argv);
    std::pair<RSCommand *, std::vector<string>> seekCommand(const std::vector<string> &argv);

    // Parses an argument list
    std::map<string,string> parse(const RSCommand &cmd, const Tokens &args);

    // Checks or parses an argument of a certain type
    bool isBool(const string &argv) const;
    bool parseBool(const string  &argv) const;
    bool parseBool(const string  &argv, bool fallback) const;
    bool parseBool(const Arguments &argv, const string &key) const;
    bool parseBool(const Arguments &argv, const string &key, long fallback) const;

    bool isOnOff(const string &argv) const;
    bool parseOnOff(const string &argv) const;
    bool parseOnOff(const string &argv, bool fallback) const;
    bool parseOnOff(const Arguments &argv, const string &key) const;
    bool parseOnOff(const Arguments &argv, const string &key, long fallback) const;

    long isNum(const string &argv) const;
    long parseNum(const string &argv) const;
    long parseNum(const string &argv, long fallback) const;
    long parseNum(const Arguments &argv, const string &key) const;
    long parseNum(const Arguments &argv, const string &key, long fallback) const;

    u16 parseAddr(const string &argv) const;
    u16 parseAddr(const string &argv, long fallback) const;
    u16 parseAddr(const Arguments &argv, const string &key) const;
    u16 parseAddr(const Arguments &argv, const string &key, long fallback) const;

    string parseSeq(const string &argv) const;
    string parseSeq(const string &argv, const string &fallback) const;

    template <typename T> long parseEnum(const string &argv) {
        return utl::parseEnum<T>(argv);
    }
    template <typename T> long parseEnum(const string &argv, long fallback) {
        try { return utl::parseEnum<T>(argv); } catch(...) { return fallback; }
    }
    template <typename E, typename T> E parseEnum(const string &argv) {
        return (E)utl::parseEnum<T>(argv);
    }
    template <typename E, typename T> long parseEnum(const string &argv, E fallback) {
        try { return (E)utl::parseEnum<T>(argv); } catch(...) { return fallback; }
    }


    //
    // Managing the command tree
    //

public:

    // Returns the root node of the instruction tree
    RSCommand &getRoot() { return root; }

protected:

    // Rebuilds the command tree for the currently active command set
    void initCommands();

    // Registers the commands of a certain command set
    void initCommonCommands(RSCommand &root);
    void initCommanderCommands(RSCommand &root);
    void initDebuggerCommands(RSCommand &root);

    const char *registerComponent(CoreComponent &c, usize flags = 0);
    const char *registerComponent(CoreComponent &c, RSCommand &root, usize flags = 0);


    //
    // Executing commands
    //

protected:

    // Executes a single command
    void exec(const InputLine& cmd);

    // Prints a usage string for a command
    void cmdUsage(const RSCommand &cmd, const string &prefix);
    void argUsage(const RSCommand &cmd, const string &prefix);

    // Displays a help text for a (partially typed in) command
    void help(std::ostream &os, const string &cmd, isize tabs);

    // Creates a textual description of an error
    void describe(const std::exception &exc, isize line = 0, const string &cmd = "");
    void describe(std::ostream &os, const std::exception &exc, isize line = 0, const string &cmd = "");


    //
    // Command handlers
    //

public:

    void dump(std::ostream &os, CoreObject &component, std::vector <Category> categories);
    void dump(std::ostream &os, CoreObject &component, Category category);

protected:

    void _dump(std::ostream &os, CoreObject &component, Category category);


    //
    // Command set: Commander
    //

protected:

    string commanderPrompt();


    //
    // Command set: Debugger
    //

protected:

    string debuggerPrompt();
    void debuggerPause();
};

}
