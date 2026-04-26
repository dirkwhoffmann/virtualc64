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
#include <stack>

namespace vc64 {

class RetroShell;

// Represents an uninterpreted sequence of tokens (e.g. "type -h lines=100")
typedef std::vector<string> Tokens;

// Parsed arguments (e.g. { {"h", "true"}, {"lines", "100"} })
typedef std::map<string,string> Arguments;

// Command and argument flags
namespace rs {

static const usize opt              = 1LL << 0;
static const usize keyval           = 1LL << 1;
static const usize flag             = 1LL << 2 | opt;
static const usize disabled         = 1LL << 3;
static const usize hidden           = 1LL << 4 | opt;
static const usize shadowed         = 1LL << 5;
static const usize acdir            = 1LL << 6;
static const usize acfile           = 1LL << 7;
static const usize ac               = acdir | acfile;

}

struct RSArgumentDescriptor {

    std::vector<string> name;
    string key;
    string value;
    usize flags;
    
    bool isFlag() const { return (flags & rs::flag) == rs::flag; }
    bool isKeyValuePair() const { return (flags & rs::keyval) == rs::keyval; }
    bool isStdArg() const { return !isFlag() && !isKeyValuePair(); }
    bool isHidden() const { return (flags & rs::hidden) == rs::hidden; }
    bool isOptional() const { return (flags & rs::opt) == rs::opt; }
    bool isRequired() const { return !isOptional(); }
    
    string nameStr() const;
    string helpStr() const;
    string keyStr() const;
    string valueStr() const;
    string keyValueStr() const;
    string usageStr() const;
};

struct RSCommandDescriptor {
    
    // Tokens the command is composed of
    const std::vector<string> &tokens = {};
    
    // General description of this command and all subcommands
    string ghelp = {};
    
    // Specific description of this command
    string chelp = {};
    
    // Command flags
    usize flags = {};
    
    // Argument descriptions of this command
    const std::vector<RSArgumentDescriptor> &args = {};
    
    // The command callback
    std::function<void (std::ostream&, const Arguments&, const std::vector<isize>&)> func = nullptr;
    
    // Addition values passed to the command callback as last argument
    const std::vector<isize> &payload = {};
};

struct RSCommand {
    
    // Used during command registration
    static string currentGroup;
    
    // Group of this command
    string groupName;
    
    // Name of this command (e.g., "eject")
    string name;
    
    // Full name of this command (e.g., "df0 eject")
    string fullName;
    
    // Command flags
    usize flags;
    
    // General description of this command and all subcommands
    string ghelp;
    
    // Specific description of this command
    string chelp;
    
    // Argument descriptions of this command
    std::vector<RSArgumentDescriptor> args;
    
    // Command handler
    std::function<void (std::ostream&, const Arguments&, const std::vector<isize>&)> callback = nullptr;
    
    // Addition values passed to the command callback as last argument
    std::vector<isize> payload;
    
    // List of subcommands
    std::vector<RSCommand> subcommands;
    
    
    //
    // Querying properties
    //
    
    bool isHidden() const { return (flags & rs::hidden) == rs::hidden; }
    bool isShadowed() const { return (flags & rs::shadowed) == rs::shadowed; }
    bool isVisible() const { return !isHidden() && !isShadowed(); }
    
    
    //
    // Working with the command tree
    //
    
    // Creates a new node in the command tree
    void add(const RSCommandDescriptor &descriptor);
    
    // Registers an alias name for an existing command
    void clone(const std::vector<string> &tokens,
               const string &alias,
               const std::vector<isize> &values = { });
    
    // Seeks a command object inside the command object tree
    const RSCommand *seek(const string& token) const;
    const RSCommand *seek(const std::vector<string> &tokens) const;
    const RSCommand &operator/(const string& token) const { return *seek(token); }
    
    RSCommand *seek(const string& token);
    RSCommand *seek(const std::vector<string> &tokens);
    RSCommand &operator/(const string& token) { return *seek(token); }
     
     
    //
    // Auto-completing user input
    //
     
public:
     
    // Auto-completes a partial token string (returns the number of matches)
    isize autoComplete(string &token);
     
private:
     
    // Filters the argument list (used by auto-completion)
    std::vector<const RSCommand *> filterPrefix(const string& prefix) const;
     
    
    //
    // Generating help messages
    //
    
public:
    
    // Returns a syntax description for subcommands or arguments
    string cmdUsage() const;
    string argUsage() const;
    
    // Displays a help text for a (partially typed in) command
    void printHelp(std::ostream &os);
    
private:
    
    void printArgumentHelp(std::ostream &os, isize indent, bool verbose = true);
    void printSubcmdHelp(std::ostream &os, isize indent, bool verbose = true);
};

}
