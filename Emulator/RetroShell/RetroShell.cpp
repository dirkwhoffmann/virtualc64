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
#include "Parser.h"
#include <sstream>

namespace vc64 {

RetroShell::RetroShell(C64& ref) : SubComponent(ref), interpreter(ref)
{

}

void
RetroShell::_initialize()
{
    C64Component::_initialize();

    // Initialize the text storage
    clear();

    // Initialize the input buffer
    history.push_back( { "", 0 } );

    // Print the startup message and the input prompt
    storage.welcome();
}

RetroShell&
RetroShell::operator<<(char value)
{
    storage << value;
    // remoteManager.rshServer << value;
    needsDisplay();
    return *this;
}

RetroShell&
RetroShell::operator<<(const string& value)
{
    storage << value;
    // remoteManager.rshServer << value;
    needsDisplay();
    return *this;
}

RetroShell&
RetroShell::operator<<(int value)
{
    *this << std::to_string(value);
    return *this;
}

RetroShell&
RetroShell::operator<<(long value)
{
    *this << std::to_string(value);
    return *this;
}

RetroShell&
RetroShell::operator<<(std::stringstream &stream)
{
    string line;
    while(std::getline(stream, line)) {
        *this << line << '\n';
    }
    return *this;
}

const char *
RetroShell::text()
{
    static string all;

    // Add the storage contents
    storage.text(all);

    // Add the input line
    all += prompt + input + " ";

    return all.c_str();
}

void
RetroShell::tab(isize pos)
{
    auto count = pos - (isize)storage[storage.size() - 1].size();

    if (count > 0) {

        std::string fill(count, ' ');
        storage << fill;
        // remoteManager.rshServer << fill;
        needsDisplay();
    }
}

void
RetroShell::setStream(std::ostream &os)
{
    storage.ostream = &os;
}

void
RetroShell::needsDisplay()
{
    msgQueue.put(MSG_UPDATE_CONSOLE);
}

void
RetroShell::clear()
{
    storage.clear();
    needsDisplay();
}

void
RetroShell::printHelp()
{
    storage.printHelp();
    // remoteManager.rshServer << "Type 'help' for help.\n";
    needsDisplay();
}

void
RetroShell::press(RetroShellKey key)
{
    assert_enum(RetroShellKey, key);
    assert(ipos >= 0 && ipos < historyLength());
    assert(cursor >= 0 && cursor <= inputLength());

    switch(key) {

        case RSKEY_UP:

            if (ipos > 0) {

                // Save the input line if it is currently shown
                if (ipos == historyLength() - 1) history.back() = { input, cursor };

                auto &item = history[--ipos];
                input = item.first;
                cursor = item.second;
            }
            break;

        case RSKEY_DOWN:

            if (ipos < historyLength() - 1) {

                auto &item = history[++ipos];
                input = item.first;
                cursor = item.second;
            }
            break;

        case RSKEY_LEFT:

            if (cursor > 0) cursor--;
            break;

        case RSKEY_RIGHT:

            if (cursor < (isize)input.size()) cursor++;
            break;

        case RSKEY_DEL:

            if (cursor < inputLength()) {
                input.erase(input.begin() + cursor);
            }
            break;

        case RSKEY_BACKSPACE:

            if (cursor > 0) {
                input.erase(input.begin() + --cursor);
            }
            break;

        case RSKEY_HOME:

            cursor = 0;
            break;

        case RSKEY_END:

            cursor = (isize)input.length();
            break;

        case RSKEY_TAB:

            if (tabPressed) {

                // TAB was pressed twice
                help(input);

            } else {

                // Auto-complete the typed in command
                input = interpreter.autoComplete(input);
                cursor = (isize)input.length();
            }
            break;

        case RSKEY_RETURN:

            *this << '\r' << prompt << input << '\n';
            execUserCommand(input);
            input = "";
            cursor = 0;
            // remoteManager.rshServer.send(prompt);
            break;

        case RSKEY_CR:

            input = "";
            cursor = 0;
            break;
    }

    tabPressed = key == RSKEY_TAB;
    needsDisplay();

    assert(ipos >= 0 && ipos < historyLength());
    assert(cursor >= 0 && cursor <= inputLength());
}

void
RetroShell::press(char c)
{
    switch (c) {

        case '\n':

            press(RSKEY_RETURN);
            break;

        case '\r':

            press(RSKEY_CR);
            break;

        case '\t':

            press(RSKEY_TAB);
            break;

        default:

            if (isprint(c)) {

                if (cursor < inputLength()) {
                    input.insert(input.begin() + cursor, c);
                } else {
                    input += c;
                }
                cursor++;
            }
    }

    tabPressed = false;
    needsDisplay();
}

void
RetroShell::press(const string &s)
{
    for (auto c : s) press(c);
}

isize
RetroShell::cursorRel()
{
    assert(cursor >= 0 && cursor <= inputLength());
    return cursor - (isize)input.length();
}

void
RetroShell::execUserCommand(const string &command)
{
    if (!command.empty()) {

        // Add the command to the history buffer
        history.back() = { command, (isize)command.size() };
        history.push_back( { "", 0 } );
        ipos = (isize)history.size() - 1;

        // Execute the command
        try { exec(command); } catch (...) { };

    } else {

        printHelp();
    }
}

void
RetroShell::exec(const string &command)
{
    bool ignoreError = false;

    // Skip comments
    if (command[0] == '#') return;

    try {
        // Check if the command marked with 'try'
        ignoreError = command.rfind("try", 0) == 0;

        // Call the interpreter
        interpreter.exec(command);

    } catch (std::exception &err) {

        // Print error message
        describe(err);

        // Rethrow the exception
        if (!ignoreError) throw;
    }
}

void
RetroShell::execScript(std::ifstream &fs)
{
    script.str("");
    script << fs.rdbuf();
    scriptLine = 1;
    continueScript();
}

void
RetroShell::execScript(const string &contents)
{
    script.str("");
    script << contents;
    scriptLine = 1;
    continueScript();
}

void
RetroShell::continueScript()
{
    string command;
    while(std::getline(script, command)) {

        // Print the command
        *this << command << '\n';

        // Execute the command
        try {
            exec(command);

        } catch (ScriptInterruption &) {

            msgQueue.put(MSG_SCRIPT_PAUSE, scriptLine);
            return;

        } catch (std::exception &) {

            *this << "Aborted in line " << scriptLine << '\n';
            msgQueue.put(MSG_SCRIPT_ABORT, scriptLine);
            return;
        }

        scriptLine++;
    }

    msgQueue.put(MSG_SCRIPT_DONE, scriptLine);
}

void
RetroShell::describe(const std::exception &e)
{
    if (auto err = dynamic_cast<const TooFewArgumentsError *>(&e)) {

        *this << err->what() << ": Too few arguments";
        *this << '\n';
        return;
    }

    if (auto err = dynamic_cast<const TooManyArgumentsError *>(&e)) {

        *this << err->what() << ": Too many arguments";
        *this << '\n';
        return;
    }

    if (auto err = dynamic_cast<const util::EnumParseError *>(&e)) {

        *this << err->token << " is not a valid key" << '\n';
        *this << "Expected: " << err->expected << '\n';
        return;
    }

    if (auto err = dynamic_cast<const util::ParseNumError *>(&e)) {

        *this << err->token << " is not a number";
        *this << '\n';
        return;
    }

    if (auto err = dynamic_cast<const util::ParseBoolError *>(&e)) {

        *this << err->token << " must be true or false";
        *this << '\n';
        return;
    }

    if (auto err = dynamic_cast<const util::ParseError *>(&e)) {

        *this << err->what() << ": Syntax error";
        *this << '\n';
        return;
    }

    if (auto err = dynamic_cast<const VC64Error *>(&e)) {

        *this << err->what();
        *this << '\n';
        return;
    }
}

void
RetroShell::help(const string &command)
{
    interpreter.help(command);
}

void
RetroShell::dump(C64Component &component, Category category)
{
    std::stringstream ss; string line;

    {   SUSPENDED

        component.dump(category, ss);
    }

    while(std::getline(ss, line)) *this << line << '\n';
}

void
RetroShell::vsyncHandler()
{
    if (cpu.clock >= wakeUp) {
        
        msgQueue.put(MSG_SCRIPT_WAKEUP);
        wakeUp = INT64_MAX;
    }
}

}
