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

#include "BasicTypes.h"
#include <functional>
#include <vector>
#include <stack>

namespace vc64 {

class RetroShell;

typedef std::vector<string> Arguments;

namespace Arg {

static const std::string address    = "<address>";
static const std::string boolean    = "{ true | false }";
static const std::string command    = "<command>";
static const std::string count      = "<count>";
static const std::string dst        = "<destination>";
static const std::string ignores    = "<ignores>";
static const std::string kb         = "<kb>";
static const std::string nr         = "<nr>";
static const std::string onoff      = "{ on | off }";
static const std::string path       = "<path>";
static const std::string process    = "<process>";
static const std::string seconds    = "<seconds>";
static const std::string value      = "<value>";
static const std::string sequence   = "<byte sequence>";
static const std::string src        = "<source>";
static const std::string volume     = "<volume>";
static const std::string string     = "<string>";

};

struct Command {

    // Used during command registration
    static string currentGroup;

    // Group of this command
    string groupName;

    // Name of this command (e.g., "eject")
    string name;

    // Full name of this command (e.g., "df0 eject")
    string fullName;

    // Help description for this command
    std::pair<string, string> help;

    // List of required arguments
    std::vector<string> requiredArgs;

    // List of optional arguments
    std::vector<string> optionalArgs;

    // List of subcommands
    std::vector<Command> subCommands;

    // Command handler
    std::function<void (Arguments&, long)> callback = nullptr;

    // Additional argument passed to the command handler
    long param = 0;

    // Indicates if this command appears in help descriptions
    bool hidden = false;


    //
    // Methods
    //

    // Creates a new node in the command tree
    void add(const std::vector<string> &tokens,
             const string &help,
             std::function<void (Arguments&, long)> func = nullptr, long param = 0);

    void add(const std::vector<string> &tokens,
             std::pair<const string &, const string &> help,
             std::function<void (Arguments&, long)> func = nullptr, long param = 0);

    void add(const std::vector<string> &tokens,
             const std::vector<string> &args,
             const string &help,
             std::function<void (Arguments&, long)> func = nullptr, long param = 0);

    void add(const std::vector<string> &tokens,
             const std::vector<string> &args,
             std::pair<const string &, const string &> help,
             std::function<void (Arguments&, long)> func = nullptr, long param = 0);

    void add(const std::vector<string> &tokens,
             const std::vector<string> &requiredArgs,
             const std::vector<string> &optionalArgs,
             const string &help,
             std::function<void (Arguments&, long)> func = nullptr, long param = 0);

    void add(const std::vector<string> &tokens,
             const std::vector<string> &requiredArgs,
             const std::vector<string> &optionalArgs,
             std::pair<const string &, const string &> help,
             std::function<void (Arguments&, long)> func = nullptr, long param = 0);

    void clone(const string &alias,
               const std::vector<string> &tokens,
               long param = 0);

    void clone(const string &alias,
               const std::vector<string> &tokens,
               const string &help,
               long param = 0);

    // Returns arguments counts
    isize minArgs() const { return isize(requiredArgs.size()); }
    isize optArgs() const { return isize(optionalArgs.size()); }
    isize maxArgs() const { return minArgs() + optArgs(); }

    // Seeks a command object inside the command object tree
    const Command *seek(const string& token) const;
    Command *seek(const string& token);
    const Command *seek(const std::vector<string> &tokens) const;
    Command *seek(const std::vector<string> &tokens);

    // Filters the argument list (used by auto-completion)
    std::vector<const Command *> filterPrefix(const string& prefix) const;

    // Automatically completes a partial token string
    string autoComplete(const string& token);

    // Returns a syntax string for this command
    string usage() const;
};

}
