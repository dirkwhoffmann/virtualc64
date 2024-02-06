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
#include "Emulator.h"
#include <sstream>

namespace vc64 {

Interpreter::Interpreter(C64 &ref) : SubComponent(ref)
{
    initCommandShell(commandShellRoot);
    initDebugShell(debugShellRoot);

    // Hide some commands
    commandShellRoot.hide({"regression"});
    commandShellRoot.hide({"screenshot"});
    commandShellRoot.hide({"joshua"});
    commandShellRoot.hide({"wait"});
    debugShellRoot.hide({"joshua"});
    debugShellRoot.hide({"wait"});
}

Arguments
Interpreter::split(const string& userInput)
{
    isize userInputSize = (isize)userInput.size();
    std::stringstream ss(userInput);
    Arguments result;

    string token;
    bool str = false; // String mode
    bool esc = false; // Escape mode
    
    for (isize i = 0; i < userInputSize; i++) {

        char c = userInput[i];
        
        // Check for escape mode
        if (c == '\\') { esc = true; continue; }

        // Switch between string mode and non-string mode if '"' is detected
        if (c == '"' && !esc) { str = !str; continue; }
        
        // Check for special characters in escape mode
        if (esc && c == 'n') c = '\n';
        
        // Process character
        if (c != ' ' || str) {
            token += c;
        } else {
            if (!token.empty()) result.push_back(token);
            token = "";
        }
        esc = false;
    }
    if (!token.empty()) result.push_back(token);
    
    return result;
}

string
Interpreter::autoComplete(const string& userInput)
{
    string result;
    
    // Split input string
    Arguments tokens = split(userInput);
    
    // Complete all tokens
    autoComplete(tokens);

    // Recreate the command string
    for (const auto &it : tokens) { result += (result == "" ? "" : " ") + it; }

    // Add a space if the command has been fully completed
    if (!tokens.empty() && getRoot().seek(tokens)) result += " ";

    return result;
}

void
Interpreter::autoComplete(Arguments &argv)
{
    Command *current = &getRoot();
    string prefix, token;

    for (auto it = argv.begin(); current && it != argv.end(); it++) {
        
        *it = current->autoComplete(*it);
        current = current->seek(*it);
    }
}

Command &
Interpreter::getRoot()
{
    switch (shell) {

        case Shell::Command: return commandShellRoot;
        case Shell::Debug: return debugShellRoot;

        default:
            fatalError;
    }
}

void
Interpreter::switchInterpreter()
{
    if (inCommandShell()) {

        shell = Shell::Debug;
        c64.emulator.trackOn(1);
        msgQueue.put(MSG_CONSOLE_DEBUGGER, true);

    } else {

        shell = Shell::Command;
        c64.emulator.trackOff(1);
        msgQueue.put(MSG_CONSOLE_DEBUGGER, false);
    }

    retroShell.updatePrompt();
}

void
Interpreter::exec(const string& userInput, bool verbose)
{
    // Split the command string
    Arguments tokens = split(userInput);

    // Skip empty lines
    if (tokens.empty()) return;

    // Remove the 'try' keyword
    if (tokens.front() == "try") tokens.erase(tokens.begin());

    // Auto complete the token list
    autoComplete(tokens);

    // Process the command
    exec(tokens, verbose);
}

void
Interpreter::exec(const Arguments &argv, bool verbose)
{
    // In 'verbose' mode, print the token list
    if (verbose) {
        for (const auto &it : argv) retroShell << it << ' ';
        retroShell << '\n';
    }

    // Skip empty lines
    if (argv.empty()) return;

    // Seek the command in the command tree
    Command *current = &getRoot(), *next;
    Arguments args = argv;

    while (!args.empty() && ((next = current->seek(args.front())) != nullptr)) {

        current = current->seek(args.front());
        args.erase(args.begin());
    }
    if ((next = current->seek(""))) current = next;

    // Error out if no command handler is present
    if (!current->callback && !args.empty()) {
        throw util::ParseError(args.front());
    }
    if (!current->callback && args.empty()) {
        throw TooFewArgumentsError(current->fullName);
    }

    // Check the argument count
    if ((isize)args.size() < current->minArgs()) throw TooFewArgumentsError(current->fullName);
    if ((isize)args.size() > current->maxArgs()) throw TooManyArgumentsError(current->fullName);

    // Call the command handler
    current->callback(args, current->param);
}

void
Interpreter::usage(const Command& current)
{
    retroShell << "Usage: " << current.usage() << '\n';
}

void
Interpreter::help(const string& userInput)
{    
    // Split the command string
    Arguments tokens = split(userInput);

    // Auto complete the token list
    autoComplete(tokens);

    // Process the command
    help(tokens);
}

void
Interpreter::help(const Arguments &argv)
{
    Command *current = &getRoot();
    string prefix, token;

    for (auto &it : argv) {
        if (current->seek(it) != nullptr) current = current->seek(it);
    }

    help(*current);
}

void
Interpreter::help(const Command& current)
{
    auto indent = string("    ");

    // Print the usage string
    usage(current);

    // Determine tabular positions to align the output
    isize tab = 0;
    for (auto &it : current.subCommands) {
        tab = std::max(tab, (isize)it.fullName.length());
    }
    tab += isize(indent.size());

    isize group = -1;

    for (auto &it : current.subCommands) {

        // Only proceed if the command is visible
        if (it.hidden) continue;

        // Print group description (when a new group begins)
        if (group != it.group) {

            group = it.group;
            retroShell << '\n';

            if (!Command::groups[group].empty()) {
                retroShell << Command::groups[group] << '\n' << '\n';
            }
        }

        // Print command descriptioon
        retroShell << indent;
        retroShell << it.fullName;
        retroShell.tab(tab);
        retroShell << " : ";
        retroShell << it.help;
        retroShell << '\n';
    }

    retroShell << '\n';
}

}
