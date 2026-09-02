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
#include "Console.h"
#include "TextStorage.h"
#include <sstream>
#include <fstream>
#include <functional>

/* RetroShell is a text-based command shell capable of controlling the emulator.
 * The shell consists of a single console whose command tree is replaced when
 * the user switches between the following command sets:
 *
 * 1. Commmander:
 *
 *    This command set is the default and offers various command for
 *    configuring the emulator and performing actions such as ejecting a disk.
 *
 * 2. Debugger:
 *
 *    This command set offers multiple debug command similar to the ones found in
 *    debug monitor. E.g., it is possible to inspect the registers of various
 *    components or generating a memory dump.
 */

namespace vc64 {

class RetroShell final : public SubComponent, public Inspectable<RetroShellInfo> {

    Descriptions descriptions = {
        {
            .name           = "RetroShell",
            .description    = "Retro Shell",
            .shell          = ""
        },
        {
            .name           = "RshShell",
            .description    = "RetroShell Server Shell",
            .shell          = ""
        },
        {
            .name           = "RpcShell",
            .description    = "RPC Server Shell",
            .shell          = ""
        }
    };

    Options options = {

    };

    TextStorage storage;

public:

    // The console
    Console console = Console(c64, *this, objid, storage);

    // Indicates if the console has new contents
    bool isDirty = false;

private:

    // Command queue (stores all pending commands)
    std::vector<InputLine> commands;

public:

    CommandSet commandSet() const { return console.getCommandSet(); }

    bool inCommandShell() const { return commandSet() == CommandSet::Commander; }
    bool inDebugShell() const { return commandSet() == CommandSet::Debugger; }


    //
    // Initializing
    //

public:

    RetroShell(C64& ref, isize id);
    RetroShell& operator= (const RetroShell& other) { return *this; }

    // Returns true for the emulator's main shell
    bool isPrimary() const { return objid == 0; }

    // Starts a fresh session (wipes the console, returns to the Commander)
    void newSession();


    //
    // Scheduling wake-up events
    //

public:

    /* Each shell owns one of the SLOT_RSH<n> event slots. The functions below
     * map the shell's object id onto the matching slot.
     */
    void scheduleWakeup(Cycle delay);
    void cancelWakeup();

    /* Indicates that the command queue is suspended. The 'wait' command
     * schedules a RSH_WAKEUP event and throws a ScriptInterruption; the
     * scheduled event is what "being suspended" means, so it is also what
     * this asks. Without the check, any other source of a Cmd::RSH_EXECUTE --
     * a nested 'source', a second --exec argument, an RPC request, a command
     * typed into the shell -- would drain the pending commands right away and
     * run the rest of the script through the wait.
     */
    bool waiting() const;


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

    void _dump(Category category, std::ostream &os) const override { }
    void _initialize() override;


    //
    // Methods from Inspectable
    //

private:

    void cacheInfo(RetroShellInfo &result) const override;


    //
    // Methods from Configurable
    //

public:

    const Options &getOptions() const override { return options; }


    //
    // Managing the command set
    //

public:

    void enterConsole(CommandSet cs);
    void enterCommander() { enterConsole(CommandSet::Commander); }
    void enterDebugger() { enterConsole(CommandSet::Debugger); }


    //
    // Executing commands
    //

public:

    // Adds a command to the list of pending commands
    void asyncExec(const string &command, bool append = true);
    void asyncExec(const InputLine &command, bool append = true);

    // Adds the commands of a shell script to the list of pending commands
    void asyncExecScript(const fs::path &path);
    void asyncExecScript(const std::ifstream &fs);
    void asyncExecScript(std::stringstream &ss);
    void asyncExecScript(const string &contents);

    // Aborts the execution of a script
    void abortScript();

    // Executes all pending commands
    void exec();

private:

    // Executes a single pending command
    void exec(const InputLine &cmd);


    //
    // Bridge functions
    //

public:

    RetroShell &operator<<(char value);
    RetroShell &operator<<(const char *value);
    RetroShell &operator<<(const string &value);
    RetroShell &operator<<(int value);
    RetroShell &operator<<(unsigned int value);
    RetroShell &operator<<(long value);
    RetroShell &operator<<(unsigned long value);
    RetroShell &operator<<(long long value);
    RetroShell &operator<<(unsigned long long value);
    RetroShell &operator<<(std::stringstream &stream);
    RetroShell &operator<<(const vspace &value);

    string prompt() { return console.prompt(); }
    const char *text();
    isize cursorRel();
    void press(RSKey key, bool shift = false);
    void press(char c);
    void press(const string &s);
    void setStream(std::ostream &os);

    void serviceEvent();
};

}
