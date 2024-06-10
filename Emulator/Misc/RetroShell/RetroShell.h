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
#include "Interpreter.h"
#include "TextStorage.h"
#include <sstream>
#include <fstream>

namespace vc64 {

class RetroShell : public SubComponent {

    Descriptions descriptions = {{

        .name           = "RetroShell",
        .description    = "Command Console"
    }};

    friend class Interpreter;
    
    // The command interpreter (parses commands typed into the console window)
    Interpreter interpreter;


    //
    // Text storage
    //

    // The text storage
    TextStorage storage;

    // History buffer storing old input strings and cursor positions
    std::vector<std::pair<string,isize>> history;

    // The currently active input string
    isize ipos = 0;


    //
    // User input
    //

    // Input line
    string input;

    // Command queue (stores all pending commands)
    std::vector<std::pair<isize,string>> commands;

    // Input prompt
    string prompt = "vc64% ";

    // Cursor position
    isize cursor = 0;

    // Indicates if TAB was the most recently pressed key
    bool tabPressed = false;


    //
    // Scripts
    //

    // The currently processed script
    std::stringstream script;

    // The script line counter (first line = 1)
    isize scriptLine = 0;

    
    //
    // Initializing
    //
    
public:
    
    RetroShell(C64& ref);

    RetroShell& operator= (const RetroShell& other) { return *this; }


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
    void _pause() override;


    //
    // Working with the text storage
    //

public:

    // Prints a message
    RetroShell &operator<<(char value);
    RetroShell &operator<<(const string &value);
    RetroShell &operator<<(int value);
    RetroShell &operator<<(unsigned int value);
    RetroShell &operator<<(long value);
    RetroShell &operator<<(unsigned long value);
    RetroShell &operator<<(long long value);
    RetroShell &operator<<(unsigned long long value);
    RetroShell &operator<<(std::stringstream &stream);

    // Returns the prompt
    const string &getPrompt();

    // Updates the prompt according to the current shell mode
    void updatePrompt();

    // Returns the contents of the whole storage as a single C string
    const char *text();

    // Moves the cursor forward to a certain column
    void tab(isize pos);

    // Assigns an additional output stream
    void setStream(std::ostream &os);

private:

    // Marks the text storage as dirty
    void needsDisplay();

    // Clears the console window
    void clear();

    // Prints the welcome message
    void welcome();

    // Prints the help line
    void printHelp();

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


    //
    // Working with the history buffer
    //

public:

    isize historyLength() { return (isize)history.size(); }

    
    //
    // Executing commands
    //

public:

    // Main entry point for executing commands that were typed in by the user
    void execUserCommand(const string &command);

    // Executes all pending commands
    void exec() throws;

    // Executes a single command
    void exec(const string &command, isize line = 0) throws;

    // Executes a shell script
    void execScript(std::stringstream &ss) throws;
    void execScript(const std::ifstream &fs) throws;
    void execScript(const string &contents) throws;
    void execScript(const class MediaFile &script) throws;
    void abortScript();

private:

    // Prints a textual description of an error in the console
    void describe(const std::exception &exception, isize line = 0, const string &cmd = "");

    // Prints help messages for a given command string
    void help(const string &command);


    //
    // Command handlers
    //

public:

    void dump(Dumpable &component, std::vector <Category> categories);
    void dump(Dumpable &component, Category category);

private:

    void _dump(Dumpable &component, Category category);

    
    //
    // Servicing events
    //

public:

    void serviceEvent();
};

}
