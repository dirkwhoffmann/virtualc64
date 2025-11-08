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
    // Set a console
    current = &debugger;
    
    // Switch the console to let the welcome message appear
    current->exec(QueuedCmd {.cmd = "commander"});
}

void
RetroShell::cacheInfo(RetroShellInfo &result) const
{
    {   SYNCHRONIZED
    
        result.console = current->objid;
        result.cursorRel = current->cursorRel();
    }
}

void
RetroShell::enterConsole(isize nr)
{
    Console *newConsole = nullptr;
    
    switch (nr) {
            
        case 0: newConsole = &commander; break;
        case 1: newConsole = &debugger; break;
            
        default:
            fatalError;
    }
    
    // Assign the new console
    current = newConsole;
    
    // Enter Leave tracking mode
    nr == 1 ? emulator.trackOn(1) : emulator.trackOff(1);
    
    if (current->isEmpty()) {
        
        // Print the welcome message if entered the first time
        current->exec(QueuedCmd {.cmd = "welcome"}); *this << current->getPrompt();

    } else {
        
        // Otherwise, print the summary message
        current->summary();
    }
    
    // Update prompt
    *this << '\r' << current->getPrompt();
    
    // Inform the GUI about the change
    msgQueue.put(Msg::RSH_SWITCH, nr);
}

void
RetroShell::asyncExec(const string &command, bool append)
{
    asyncExec(QueuedCmd { .type = QueuedCmd::Type::USER, .cmd = command });
}

void
RetroShell::asyncExec(const QueuedCmd &command, bool append)
{
    // Feed the command into the command queue
    if (append) {
        commands.push_back(command);
    } else {
        commands.insert(commands.begin(), command);
    }
    
    // Process the command queue in the next update cycle
    emulator.put(Command(Cmd::RSH_EXECUTE));
}

void
RetroShell::asyncExecScript(std::stringstream &ss)
{
    {   SYNCHRONIZED

        std::string line;
        isize nr = 1;

        while (std::getline(ss, line)) {

            QueuedCmd cmd = { .type = QueuedCmd::Type::SCRIPT, .id = nr++, .cmd = line };
            commands.push_back(cmd);
        }
    
        emulator.put(Command(Cmd::RSH_EXECUTE));
    }
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
            
        case FileType::SCRIPT:
            
            s = string((char *)file.getData(), file.getSize());
            asyncExecScript(s);
            break;
            
        default:
            
            throw AppError(Fault::FILE_TYPE_MISMATCH);
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
    {   SYNCHRONIZED

        // Only proceed if there is anything to process
        if (commands.empty()) return;

        try {

            while (!commands.empty()) {

                QueuedCmd cmd = commands.front();
                commands.erase(commands.begin());
                exec(cmd);
            }

        } catch (ScriptInterruption &) {

            msgQueue.put(Msg::RSH_WAIT);

        } catch (...) {

            // Remove all remaining commands
            commands = { };

            msgQueue.put(Msg::RSH_ERROR);
        }

        // Print prompt
        if (current->lastLineIsEmpty()) *this << current->getPrompt();
    }
}

void
RetroShell::exec(QueuedCmd cmd)
{
    auto script = cmd.type == QueuedCmd::Type::SCRIPT;

    // Print the command if it comes from a script
    if (script) *this << cmd.cmd << '\n';

    // Call the interpreter
    current->exec(cmd);
}

RetroShell &
RetroShell::operator<<(char value)
{
    *current << value;
    return *this;
}

RetroShell &
RetroShell::operator<<(const char *value)
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

RetroShell&
RetroShell::operator<<(const vspace &value)
{
    *current << value;
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
RetroShell::press(RSKey key, bool shift)
{
    if (shift) {
        
        switch(key) {
                
            case RSKey::TAB:
                
                if (current->objid == 0) current->input = "debugger";
                if (current->objid == 1) current->input = "commander";
                current->pressReturn(false);
                return;
                
            default:
                break;
        }
    }
    
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
    emulator.put(Command(Cmd::RSH_EXECUTE));
    c64.cancel<SLOT_RSH>();
}

}
