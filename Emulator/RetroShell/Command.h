// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "Aliases.h"
#include <vector>
#include <functional>

namespace vc64 {

class RetroShell;

typedef std::vector<string> Arguments;

namespace Arg {

static const std::string address    = "<address>";
static const std::string boolean    = "{ true | false }";
static const std::string command    = "<command>";
static const std::string kb         = "<kb>";
static const std::string onoff      = "{ on | off }";
static const std::string path       = "<path>";
static const std::string process    = "<process>";
static const std::string seconds    = "<seconds>";
static const std::string value      = "<value>";
static const std::string volume     = "<volume>";

};

struct Command {
    
    //Textual descriptions of all command groups
    static std::vector<string> groups;

    // Command group of this command
    isize group;

    // Name of this command (e.g., "eject")
    string name;

    // Full name of this command (e.g., "df0 eject")
    string fullName;

    // List of required arguments
    std::vector<string> requiredArgs;

    // List of optional arguments
    std::vector<string> optionalArgs;

    // Help message for this command
    string help;

    // List of subcommands
    std::vector<Command> subCommands;

    // Command handler
    std::function<void (Arguments&, long)> callback = nullptr;

    // Command handler (DEPRECATED)
    void (RetroShell::*action)(Arguments&, long) = nullptr;

    // Additional argument passed to the command handler
    long param = 0;

    // Indicates if this command appears in help descriptions
    bool hidden = false;

    
    //
    // Methods
    //

    // Creates a new command group
    void newGroup(const string &description, const string &postfix = ":");

    // Creates a new node in the command tree
    void add(const std::vector<string> &tokens,
             const string &help);

    void add(const std::vector<string> &tokens,
             const string &help,
             std::function<void (Arguments&, long)> func, long param = 0);

    void add(const std::vector<string> &tokens,
             const std::vector<string> &args,
             const string &help,
             std::function<void (Arguments&, long)> func, long param = 0);

    void add(const std::vector<string> &tokens,
             const std::vector<string> &requiredArgs,
             const std::vector<string> &optionalArgs,
             const string &help,
             std::function<void (Arguments&, long)> func, long param = 0);

    // DEPRECATED
    void add(const std::vector<string> &tokens,
             const string &help,
             void (RetroShell::*action)(Arguments&, long), long param = 0);

    // DEPRECATED
    void add(const std::vector<string> &tokens,
             const std::vector<string> &args,
             const string &help,
             void (RetroShell::*action)(Arguments&, long), long param = 0);

    // DEPRECATED
    void add(const std::vector<string> &tokens,
             const std::vector<string> &requiredArgs,
             const std::vector<string> &optionalArgs,
             const string &help,
             void (RetroShell::*action)(Arguments&, long), long param = 0);

    // Marks a command as hidden
    void hide(const std::vector<string> &tokens);

    // Removes a registered command
    void remove(const string& token);

    // Returns arguments counts
    isize minArgs() const { return isize(requiredArgs.size()); }
    isize optArgs() const { return isize(optionalArgs.size()); }
    isize maxArgs() const { return minArgs() + optArgs(); }

    // Seeks a command object inside the command object tree
    Command *seek(const string& token);
    Command *seek(const std::vector<string> &tokens);

    // Filters the argument list (used by auto-completion)
    std::vector<const Command *> filterPrefix(const string& prefix) const;

    // Automatically completes a partial token string
    string autoComplete(const string& token);

    // Returns a syntax string for this command
    string usage() const;
};

}
