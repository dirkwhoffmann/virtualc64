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

#include "vcconfig.h"
#include "RetroShell.h"
#include "RSError.h"
#include "Emulator.h"
#include <istream>
#include <sstream>

namespace vc64 {

RetroShell::RetroShell(C64& ref, isize id) : SubComponent(ref, id)
{
    subComponents = std::vector<CoreComponent *> {

        &console
    };

    // The main shell boots into the Commander. The remote shells stay idle
    // until a client connects.
    if (isPrimary()) commands = { InputLine {.input = "commander"} };
}

void
RetroShell::scheduleWakeup(Cycle delay)
{
    switch (objid) {

        case 0:     c64.scheduleRel<SLOT_RSH0>(delay, RSH_WAKEUP); break;
        case 1:     c64.scheduleRel<SLOT_RSH1>(delay, RSH_WAKEUP); break;
        case 2:     c64.scheduleRel<SLOT_RSH2>(delay, RSH_WAKEUP); break;

        default:
            fatalError;
    }
}

void
RetroShell::cancelWakeup()
{
    switch (objid) {

        case 0:     c64.cancel<SLOT_RSH0>(); break;
        case 1:     c64.cancel<SLOT_RSH1>(); break;
        case 2:     c64.cancel<SLOT_RSH2>(); break;

        default:
            fatalError;
    }
}

bool
RetroShell::waiting() const
{
    switch (objid) {

        case 0:     return c64.hasEvent<SLOT_RSH0>(RSH_WAKEUP);
        case 1:     return c64.hasEvent<SLOT_RSH1>(RSH_WAKEUP);
        case 2:     return c64.hasEvent<SLOT_RSH2>(RSH_WAKEUP);

        default:
            fatalError;
    }
}

void
RetroShell::newSession()
{
    /* Called from the server threads. The lock keeps us from rebuilding the
     * command tree while the emulator thread is walking it.
     */
    {   SYNCHRONIZED

        commands = { };
        cancelWakeup();
        console.clear();
        enterCommander();
    }
}

void
RetroShell::_initialize()
{
    exec();
}

void
RetroShell::cacheInfo(RetroShellInfo &result) const
{
    {   SYNCHRONIZED

        result.console = isize(console.getCommandSet());
        result.cursorRel = console.cursorRel();
    }
}

void
RetroShell::enterConsole(CommandSet cs)
{
    // Replace the command tree
    console.setCommandSet(cs);

    // Inform the GUI about the change
    msgQueue.put(Msg::RSH_SWITCH, objid, isize(cs));
}

void
RetroShell::asyncExec(const string &command, bool append)
{
    asyncExec(InputLine { .type = InputLine::Source::USER, .input = command });
}

void
RetroShell::asyncExec(const InputLine &command, bool append)
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

            commands.push_back(InputLine {

                .id    = nr++,
                .type  = InputLine::Source::SCRIPT,
                .input = line
            });
        }

        emulator.put(Command(Cmd::RSH_EXECUTE));
    }
}

void
RetroShell::asyncExecScript(const fs::path &path)
{
    std::ifstream fs(host.makeAbsolute(path));
    if (!fs) throw IOError(IOError::FILE_NOT_FOUND, path);

    asyncExecScript(fs);
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
RetroShell::abortScript()
{
    {   SYNCHRONIZED

        if (!commands.empty()) {

            commands.clear();

            // Drops the pending wake-up along with the commands
            cancelWakeup();
        }
    }
}

void
RetroShell::exec()
{
    {   SYNCHRONIZED

        // Only proceed if there is anything to process
        if (commands.empty()) return;

        // Stay put while a 'wait' command is pending
        if (waiting()) return;

        try {

            while (!commands.empty()) {

                InputLine cmd = commands.front();
                commands.erase(commands.begin());
                exec(cmd);
            }

        } catch (ScriptInterruption &) {

            // The queue stays suspended until the scheduled RSH_WAKEUP
            // arrives; the command that threw is what scheduled it.
            msgQueue.put(Msg::RSH_WAIT, objid);

        } catch (...) {

            // Remove all remaining commands
            commands = { };

            msgQueue.put(Msg::RSH_ERROR, objid);
        }

        // Print prompt
        if (console.lastLineIsEmpty()) *this << console.prompt();
    }
}

void
RetroShell::exec(const InputLine &cmd)
{
    try {

        // Call the interpreter
        console.exec(cmd);

    } catch (ScriptInterruption &) {

        // Rethrow the exception
        throw;

    } catch (std::exception &) {

        // Rethrow the exception if the command is not prefixed with 'try'
        if (cmd.input.rfind("try", 0)) throw;
    }
}

RetroShell &
RetroShell::operator<<(char value)
{
    console << value;
    return *this;
}

RetroShell &
RetroShell::operator<<(const char *value)
{
    console << value;
    return *this;
}

RetroShell &
RetroShell::operator<<(const string &value)
{
    console << value;
    return *this;
}

RetroShell &
RetroShell::operator<<(int value)
{
    console << value;
    return *this;
}

RetroShell &
RetroShell::operator<<(unsigned int value)
{
    console << value;
    return *this;
}

RetroShell &
RetroShell::operator<<(long value)
{
    console << value;
    return *this;
}

RetroShell &
RetroShell::operator<<(unsigned long value)
{
    console << value;
    return *this;
}

RetroShell &
RetroShell::operator<<(long long value)
{
    console << value;
    return *this;
}

RetroShell &
RetroShell::operator<<(unsigned long long value)
{
    console << value;
    return *this;
}

RetroShell &
RetroShell::operator<<(std::stringstream &stream)
{
    console << stream;
    return *this;
}

RetroShell&
RetroShell::operator<<(const vspace &value)
{
    console << value;
    return *this;
}

const char *
RetroShell::text()
{
    return console.text();
}

isize
RetroShell::cursorRel()
{
    return console.cursorRel();
}

void
RetroShell::press(RSKey key, bool shift)
{
    if (shift) {

        switch(key) {

            case RSKey::TAB:

                // Cycle through the available command sets
                switch (console.getCommandSet()) {

                    case CommandSet::Commander:     console.input = "debugger"; break;
                    case CommandSet::Debugger:      console.input = "commander"; break;
                }
                console.pressReturn(false);
                return;

            default:
                break;
        }
    }

    console.press(key, shift);
}

void
RetroShell::press(char c)
{
    console.press(c);
}

void
RetroShell::press(const string &s)
{
    console.press(s);
}

void
RetroShell::setStream(std::ostream &os)
{
    console.setStream(os);
}

void
RetroShell::serviceEvent()
{
    // Clear the wake-up first: it is what waiting() reads, so the queue must
    // no longer look suspended by the time the queued command is processed.
    cancelWakeup();

    emulator.put(Command(Cmd::RSH_EXECUTE));
}

}
