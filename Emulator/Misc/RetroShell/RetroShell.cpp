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
#include "Emulator.h"
#include "Parser.h"
#include <istream>
#include <sstream>

namespace vc64 {

RetroShell::RetroShell(C64& ref) : SubComponent(ref)
{
    subComponents = std::vector<CoreComponent *> {

        &commander,
        &debugger
    };
}

void
RetroShell::_initialize()
{
    enterCommander();
}

void
RetroShell::switchConsole() {

    inCommandShell() ? enterDebugger() : enterCommander();
}

void
RetroShell::enterDebugger()
{
    // Assign the new console
    current = &debugger;

    // Enter tracking mode
    emulator.trackOn(1);

    // Call the delegate of the new console
    current->_enter();
}

void
RetroShell::enterCommander()
{
    // Assign the new console
    current = &commander;

    // Leave tracking mode
    emulator.trackOff(1);

    // Call the delegate of the new console
    current->_enter();
}

void
RetroShell::asyncExec(const string &command, bool append)
{
    // Feed the command into the command queue
    if (append) {
        commands.push_back({ 0, command});
    } else {
        commands.insert(commands.begin(), { 0, command});
    }

    // Process the command queue in the next update cycle
    emulator.put(Cmd(CMD_RSH_EXECUTE));
}

void
RetroShell::asyncExecScript(std::stringstream &ss)
{
    SYNCHRONIZED

    std::string line;
    isize nr = 1;

    while (std::getline(ss, line)) {

        commands.push_back({ nr++, line });
    }

    emulator.put(Cmd(CMD_RSH_EXECUTE));
}

void
RetroShell::asyncExecScript(const std::ifstream &fs)
{
    std::stringstream ss;
    ss << fs.rdbuf();
    asyncExecScript(ss);
}

void
RetroShell::asyncExecScript(const string &contents)
{
    std::stringstream ss;
    ss << contents;
    asyncExecScript(ss);
}

void
RetroShell::asyncExecScript(const MediaFile &file)
{
    string s;

    switch (file.type()) {

        case FILETYPE_SCRIPT:

            s = string((char *)file.getData(), file.getSize());
            asyncExecScript(s);
            break;

        default:

            throw Error(VC64ERROR_FILE_TYPE_MISMATCH);
    }
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
RetroShell::exec()
{
    SYNCHRONIZED

    // Only proceed if there is anything to process
    if (commands.empty()) return;

    std::pair<isize, string> cmd;

    try {

        while (!commands.empty()) {

            cmd = commands.front();
            commands.erase(commands.begin());
            exec(cmd);
        }

    } catch (ScriptInterruption &) {

        msgQueue.put(MSG_RSH_WAIT);

    } catch (...) {

        // Remove all remaining commands
        commands = { };

        msgQueue.put(MSG_RSH_ERROR);
    }

    // Print prompt
    if (current->lastLineIsEmpty()) *this << current->getPrompt();
}

void
RetroShell::exec(QueuedCmd cmd)
{
    auto line = cmd.first;
    auto command = cmd.second;

    try {

        // Print the command if it comes from a script
        if (line) *this << command << '\n';

        // Call the interpreter
        current->exec(command);

    } catch (ScriptInterruption &) {

        // Rethrow the exception
        throw;

    } catch (std::exception &err) {

        // Print error message
        current->describe(err, line, command);

        // Rethrow the exception if the command is not prefixed with 'try'
        if (command.rfind("try", 0)) throw;
    }
}

RetroShell &
RetroShell::operator<<(char value)
{
    *current << value;
    return *this;
}

RetroShell &
RetroShell::operator<<(const string &value)
{
    *current << value;
    return *this;
}

RetroShell &
RetroShell::operator<<(int value)
{
    *current << value;
    return *this;
}

RetroShell &
RetroShell::operator<<(unsigned int value)
{
    *current << value;
    return *this;
}

RetroShell &
RetroShell::operator<<(long value)
{
    *current << value;
    return *this;
}

RetroShell &
RetroShell::operator<<(unsigned long value)
{
    *current << value;
    return *this;
}

RetroShell &
RetroShell::operator<<(long long value)
{
    *current << value;
    return *this;
}

RetroShell &
RetroShell::operator<<(unsigned long long value)
{
    *current << value;
    return *this;
}

RetroShell &
RetroShell::operator<<(std::stringstream &stream)
{
    *current << stream;
    return *this;
}

const char *
RetroShell::text()
{
    return current->text();
}

isize
RetroShell::cursorRel()
{
    return current->cursorRel();
}

void
RetroShell::press(RetroShellKey key, bool shift)
{
    current->press(key, shift);
}

void
RetroShell::press(char c)
{
    current->press(c);
}

void
RetroShell::press(const string &s)
{
    current->press(s);
}

void
RetroShell::setStream(std::ostream &os)
{
    commander.setStream(os);
    debugger.setStream(os);
}

void
RetroShell::serviceEvent()
{
    emulator.put(Cmd(CMD_RSH_EXECUTE));
    c64.cancel<SLOT_RSH>();
}

}
