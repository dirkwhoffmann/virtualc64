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
#include "Interpreter.h"

class RetroShell : public C64Component {

    // Interpreter for commands typed into the console window
    Interpreter interpreter;
    
    //
    // Text storage
    //
    
    // The text storage
    std::vector<string> storage;
    string all;
    
    // The input history buffer
    std::vector<string> input;

    // Input prompt
    string prompt = "vc64\% ";
    
    // The current cursor position
    isize cpos = 0;

    // The minimum cursor position in this row
    isize cposMin = 0;
    
    // The currently active input string
    isize ipos = 0;

    // Indicates if TAB was the most recently pressed key
    bool tabPressed = false;
    
    bool isDirty = false;
    
    //
    // Script processing
    //
    
    // The name of the currently executed script ("" if none)
    string scriptName;
    
    // The number of the script line to execute next (first line = 1)
    isize scriptLine = 0;
    
    
    //
    // Initializing
    //
    
public:
    
    RetroShell(C64& ref);
        
    const char *getDescription() const override { return "RetroShell"; }

    void _reset() override { }
    
    
    //
    // Serializing
    //

private:

    isize _size() override { return 0; }
    isize _load(const u8 *buffer) override {return 0; }
    isize _save(u8 *buffer) override { return 0; }

    
    //
    // Managing user input
    //

public:
    
    void pressUp();
    void pressDown();
    void pressLeft();
    void pressRight();
    void pressHome();
    void pressEnd();
    void pressTab();
    void pressBackspace();
    void pressDelete();
    void pressReturn();
    void pressKey(char c);


    //
    // Working with the text storage
    //

public:
    
    const char *text();
    
    // Returns a reference to the text storage
    const std::vector<string> &getStorage() { return storage; }
    
    // Returns the cursor position (relative to the line end)
    isize cposAbs() { return cpos; }
    isize cposRel();

    // Prints a message
    RetroShell &operator<<(char value);
    RetroShell &operator<<(const string &value);
    RetroShell &operator<<(int value);
    RetroShell &operator<<(long value);

    // Moves the cursor forward to a certain column
    void tab(isize hpos);

    // Prints the input prompt
    void printPrompt();

private:

    // Returns a reference to the last line in the text storage
    string &lastLine() { return storage.back(); }
    
    // Returns a reference to the last line in the input history buffer
    string &lastInput() { return input.back(); }

    // Clears the console window
    void clear();
    
    // Prints a help line
    void printHelp();
    
    // Shortens the text storage if it grows too large
    void shorten();
    
    // Clears the current line
    void clearLine() { *this << '\r'; }
    
    
    //
    // Executing commands
    //
    
public:
    
    // Executes a user command
    void exec(const string &command) throws;
    
    // Executes a user script
    void execScript(const string &name) throws;
    
    // Continues a previously interrupted script
    void continueScript() throws;
    
    // Prints a textual description of an error in the console
    void describe(const std::exception &exception);
    void describe(const struct VC64Error &error);

    
    //
    // Command handlers
    //
    
public:
        
    template <Token t1>
    void exec(Arguments& argv, long param) throws;
    template <Token t1, Token t2>
    void exec(Arguments& argv, long param) throws;
    template <Token t1, Token t2, Token t3>
    void exec(Arguments& argv, long param) throws;
    template <Token t1, Token t2, Token t3, Token t4>
    void exec(Arguments& argv, long param) throws;

private:
    
    void dump(HardwareComponent &component, dump::Category category);
};
