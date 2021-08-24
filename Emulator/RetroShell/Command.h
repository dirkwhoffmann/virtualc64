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

class RetroShell;

typedef std::vector<string> Arguments;

struct Command {
    
    // Pointer to the parent command
    Command *parent = nullptr;
    
    // The token string (e.g., "agnus" or "set")
    string token;
    
    // A string describing the token type (e.g., "component or "command")
    string type;
    
    // The help string for this command
    string info;
    
    // The sub commands of this command
    std::vector<Command> args;
    
    // Command handler
    void (RetroShell::*action)(Arguments&, long) = nullptr;
    
    // Number of additional arguments expected by the command handler
    isize numArgs = 0;
    
    // An additional paramter passed to the command handler
    long param = 0;
    
    // Indicates if this command appears in the help descriptions
    bool hidden = false;
        
    // Creates a new node in the command tree
    void add(const std::vector<string> tokens,
             const string &type,
             const string &help,
             void (RetroShell::*action)(Arguments&, long) = nullptr,
             isize numArgs = 0, long param = 0);
    
    // Removes a registered command
    void remove(const string& token);
    
    // Seeks a command object inside the command object tree
    Command *seek(const string& token);
    Command *seek(const std::vector<string> &tokens);
    
    // Collects the type descriptions in the args vector
    std::vector<string> types() const;
    
    // Filters the argument list
    std::vector<const Command *> filterType(const string& type) const;
    std::vector<const Command *> filterPrefix(const string& prefix) const;

    // Automatically completes a partial token string
    string autoComplete(const string& token);
    
    // Returns the full command string for this command
    string tokens() const;
    
    // Returns a syntax string for this command
    string usage() const;
};
