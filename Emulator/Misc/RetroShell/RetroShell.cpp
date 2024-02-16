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

#include "config.h"
#include "RetroShell.h"
#include "VirtualC64.h"
#include "Parser.h"
#include <sstream>

namespace vc64 {

RetroShell::RetroShell(C64& ref) : SubComponent(ref), interpreter(ref)
{

}

void
RetroShell::_initialize()
{
    CoreComponent::_initialize();

    // Initialize the text storage
    clear();

    // Initialize the input buffer
    history.push_back( { "", 0 } );

    // Print the startup message and the input prompt
    welcome();
}

void
RetroShell::_pause()
{
    printState();
}

RetroShell&
RetroShell::operator<<(char value)
{
    storage << value;
    needsDisplay();
    return *this;
}

RetroShell&
RetroShell::operator<<(const string& value)
{
    storage << value;
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

const string &
RetroShell::getPrompt()
{
    return prompt;
}

void
RetroShell::updatePrompt()
{
    if (interpreter.inCommandShell()) {

        prompt = "vc64% ";

    } else {

        std::stringstream ss;

        ss << "(";
        ss << std::right << std::setw(0) << std::dec << isize(c64.scanline);
        ss << ",";
        ss << std::right << std::setw(0) << std::dec << isize(c64.rasterCycle);
        ss << ") $";
        ss << std::right << std::setw(4) << std::hex << isize(cpu.getPC0());
        ss << ": ";

        prompt = ss.str();
    }

    needsDisplay();
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
    msgQueue.put(MSG_CONSOLE_UPDATE);
}

void
RetroShell::clear()
{
    storage.clear();
    needsDisplay();
}

void
RetroShell::welcome()
{
    string name = interpreter.inDebugShell() ? "Debug Shell" : "Retro Shell";

    if (interpreter.inCommandShell()) {

        *this << "VirtualC64 " << name << " ";
        *this << VirtualC64::build() << '\n';
        *this << '\n';
        *this << "Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de" << '\n';
        *this << "Licensed under the GNU General Public License v3" << '\n';
        *this << '\n';
    }

    printHelp();
    *this << '\n';
}

void
RetroShell::printHelp()
{
    string action = interpreter.inDebugShell() ? "exit" : "enter";

    *this << "Type 'help' or press 'TAB' twice for help.\n";
    *this << "Type '.' or press 'SHIFT+RETURN' to " << action << " debug mode.";
    *this << '\n';
}

void
RetroShell::printState()
{
    if (interpreter.inDebugShell()) {

        std::stringstream ss;

        ss << "\n";
        cpu.debugger.dumpLogBuffer(ss, 8);
        ss << "\n";
        c64.dump(Category::Current, ss);
        ss << "\n";
        cpu.disassembler.disassembleRange(ss, cpu.getPC0(), 8);
        ss << "\n";

        *this << ss;

        updatePrompt();

    } else {

        updatePrompt();
    }
}

void
RetroShell::press(RetroShellKey key, bool shift)
{
    assert_enum(RetroShellKey, key);
    assert(ipos >= 0 && ipos < historyLength());
    assert(cursor >= 0 && cursor <= inputLength());

    abortScript();

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

        case RSKEY_CUT:

            if (cursor < inputLength()) {
                input.erase(input.begin() + cursor, input.end());
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

            if (shift) {

                interpreter.switchInterpreter();

            } else {

                SYNCHRONIZED

                *this << '\r' << getPrompt() << input << '\n';
                commands.push_back(input);
                input = "";
                cursor = 0;
                emulator.put(Cmd(CMD_RSH_EXECUTE));
            }
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
    abortScript();

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
RetroShell::exec()
{
    SYNCHRONIZED

    try {

        while (!commands.empty()) {

            auto cmd = commands.front();
            commands.erase(commands.begin());

            exec(cmd);
        }

    } catch (...) { }
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
RetroShell::execScript(std::stringstream &ss)
{
    std::string line;

    while (std::getline(ss, line)) {

        commands.push_back(line);
    }

    emulator.put(Cmd(CMD_RSH_EXECUTE));

    /*
    script.str("");
    script.clear();
    script << ss.rdbuf();
    scriptLine = 1;
    continueScript();
    */
}

void
RetroShell::execScript(const std::ifstream &fs)
{
    std::stringstream ss;
    ss << fs.rdbuf();
    execScript(ss);
}

void
RetroShell::execScript(const string &contents)
{
    std::stringstream ss;
    ss << contents;
    execScript(ss);
}

void 
RetroShell::abortScript()
{
    {   SYNCHRONIZED

        if (!commands.empty()) {

            commands.clear();
            c64.cancel<SLOT_RSH>();
        }
    }
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

        } catch (ScriptInterruption &exc) {

            msgQueue.put(MSG_SCRIPT_PAUSE, ScriptMsg { scriptLine, i16(exc.data) });
            return;

        } catch (std::exception &) {

            *this << "Aborted in line " << scriptLine << '\n';
            msgQueue.put(MSG_SCRIPT_ABORT, ScriptMsg { scriptLine, 0 });
            return;
        }

        scriptLine++;
    }

    msgQueue.put(MSG_SCRIPT_DONE, ScriptMsg { scriptLine, 0 });
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

    if (auto err = dynamic_cast<const util::ParseOnOffError *>(&e)) {

        *this << "'" << err->token << "' must be on or off";
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
RetroShell::dump(CoreObject &component, Category category)
{
    {   SUSPENDED

        *this << '\n';
        _dump(component, category);
    }
}

void
RetroShell::dump(CoreObject &component, std::vector <Category> categories)
{
    {   SUSPENDED

        *this << '\n';
        for(auto &category : categories) _dump(component, category);

    }
}

void
RetroShell::_dump(CoreObject &component, Category category)
{
    std::stringstream ss;

    switch (category) {

        case Category::Slots:       ss << "Slots:\n\n"; break;
        case Category::Config:      ss << "Configuration:\n\n"; break;
        case Category::Properties:  ss << "Properties:\n\n"; break;
        case Category::Registers:   ss << "Registers:\n\n"; break;
        case Category::State:       ss << "State:\n\n"; break;
        case Category::Stats:       ss << "Statistics:\n\n"; break;

        default:
            break;
    }

    component.dump(category, ss);

    *this << ss << '\n';
}

void
RetroShell::serviceEvent()
{
    emulator.put(Cmd(CMD_RSH_EXECUTE));
    c64.cancel<SLOT_RSH>();
}

}
