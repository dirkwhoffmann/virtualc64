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
#include "RSCommand.h"
#include "StringUtils.h"
#include <algorithm>
#include <utility>

namespace vc64 {

string RSCommand::currentGroup;

void
RSCommand::add(const RetroShellCmdDescriptor &descriptor)
{
    assert(!descriptor.tokens.empty());
    
    // Cleanse the token list (convert { "aaa bbb" } into { "aaa", "bbb" }
    auto tokens = util::split(descriptor.tokens, ' ');
    
    // The last entry in the token list is the command name
    auto name = tokens.back();
    
    // Determine how the token is displayed in help messages
    auto helpName = descriptor.help.size() > 1 ? descriptor.help[1] : name;
    
    // Traversing the command tree
    RSCommand *node = seek(std::vector<string> { tokens.begin(), tokens.end() - 1 });
    assert(node != nullptr);
    
    // Create the instruction
    RSCommand cmd;
    cmd.name = name;
    cmd.fullName = (node->fullName.empty() ? "" : node->fullName + " ") + helpName;
    cmd.helpName = helpName;
    cmd.groupName = currentGroup;
    cmd.requiredArgs = descriptor.args;
    cmd.optionalArgs = descriptor.extra;
    cmd.help = descriptor.help;
    cmd.callback = descriptor.func;
    cmd.param = descriptor.values;
    cmd.hidden = descriptor.hidden; //  || descriptor.help.empty();
    
    if (!cmd.hidden) currentGroup = "";
    
    // Register the instruction at the proper location
    node->subCommands.push_back(cmd);
}

void
RSCommand::clone(const std::vector<string> &tokens,
                 const string &alias,
                 const std::vector<isize> &values)
{
    assert(!tokens.empty());
    
    // Find the command to clone
    RSCommand *cmd = seek(std::vector<string> { tokens.begin(), tokens.end() });
    assert(cmd != nullptr);
    
    // Assemble the new token list
    auto newTokens = std::vector<string> { tokens.begin(), tokens.end() - 1 };
    newTokens.push_back(alias);
    
    // Create the instruction
    add(RetroShellCmdDescriptor {
        
        .tokens = newTokens,
        .hidden = true,
        .args   = cmd->requiredArgs,
        .extra  = cmd->optionalArgs,
        .func   = cmd->callback,
        .values = values
    });
}

/*
 void
 RetroShellCmd::add(const std::vector<string> &tokens,
 const string &help,
 RetroShellCallback func, long param)
 {
 add(tokens, { }, { }, { tokens.back(), help }, func, param);
 }
 
 void
 RetroShellCmd::add(const std::vector<string> &tokens,
 std::pair<const string &, const string &> help,
 RetroShellCallback func, long param)
 {
 add(tokens, { }, { }, help, func, param);
 }
 
 void
 RetroShellCmd::add(const std::vector<string> &tokens,
 const std::vector<string> &arguments,
 const string &help,
 RetroShellCallback func, long param)
 {
 add(tokens, arguments, { }, { tokens.back(), help }, func, param);
 }
 
 void
 RetroShellCmd::add(const std::vector<string> &tokens,
 const std::vector<string> &arguments,
 std::pair<const string &, const string &> help,
 RetroShellCallback func, long param)
 {
 add(tokens, arguments, { }, help, func, param);
 }
 
 void
 RetroShellCmd::add(const std::vector<string> &tokens,
 const std::vector<string> &requiredArgs,
 const std::vector<string> &optionalArgs,
 const string &help,
 RetroShellCallback func, long param)
 {
 add(tokens, requiredArgs, optionalArgs, { tokens.back(), help }, func, param);
 }
 
 void
 RetroShellCmd::add(const std::vector<string> &rawtokens,
 const std::vector<string> &requiredArgs,
 const std::vector<string> &optionalArgs,
 std::pair<const string &, const string &> help,
 RetroShellCallback func, long param)
 {
 assert(!rawtokens.empty());
 
 // Cleanse the token list (convert { "aaa bbb" } into { "aaa", "bbb" }
 auto tokens = util::split(rawtokens, ' ');
 
 // Traverse the node tree
 RetroShellCmd *cmd = seek(std::vector<string> { tokens.begin(), tokens.end() - 1 });
 assert(cmd != nullptr);
 
 // Create the instruction
 RetroShellCmd d;
 d.name = tokens.back();
 d.fullName = (cmd->fullName.empty() ? "" : cmd->fullName + " ") + help.first;
 d.groupName = currentGroup;
 d.requiredArgs = requiredArgs;
 d.optionalArgs = optionalArgs;
 d.help = { help.first };
 d.callback = func;
 d.param = { param };
 d.hidden = help.second.empty();
 
 if (!d.hidden) currentGroup = "";
 
 // Register the instruction
 cmd->subCommands.push_back(d);
 }
 
 void 
 RetroShellCmd::clone(const string &alias,
 const std::vector<string> &tokens,
 long param)
 {
 clone(alias, tokens, "", param);
 }
 
 void
 RetroShellCmd::clone(const string &alias, const std::vector<string> &tokens, const string &help, long param)
 {
 assert(!tokens.empty());
 
 // Find the command to clone
 RetroShellCmd *cmd = seek(std::vector<string> { tokens.begin(), tokens.end() });
 assert(cmd != nullptr);
 
 // Assemble the new token list
 auto newTokens = std::vector<string> { tokens.begin(), tokens.end() - 1 };
 newTokens.push_back(alias);
 
 // Create the instruction
 add(newTokens, 
 cmd->requiredArgs,
 cmd->optionalArgs,
 help,
 cmd->callback,
 param);
 }
 */

const RSCommand *
RSCommand::seek(const string& token) const
{
    for (auto &it : subCommands) {
        if (it.name == token) return &it;
    }
    return nullptr;
}

RSCommand *
RSCommand::seek(const string& token)
{
    return const_cast<RSCommand *>(std::as_const(*this).seek(token));
}

const RSCommand *
RSCommand::seek(const std::vector<string> &tokens) const
{
    const RSCommand *result = this;
    
    for (auto &it : tokens) {
        if ((result = result->seek(it)) == nullptr) break;
    }
    
    return result;
}

RSCommand *
RSCommand::seek(const std::vector<string> &tokens)
{
    return const_cast<RSCommand *>(std::as_const(*this).seek(tokens));
}

std::vector<const RSCommand *>
RSCommand::filterPrefix(const string& prefix) const
{
    std::vector<const RSCommand *> result;
    auto uprefix = util::uppercased(prefix);
    
    for (auto &it : subCommands) {
        
        if (it.hidden) continue;
        auto substr = it.name.substr(0, prefix.size());
        if (util::uppercased(substr) == uprefix) result.push_back(&it);
    }
    
    return result;
}

string
RSCommand::autoComplete(const string& token)
{
    string result;
    
    auto matches = filterPrefix(token);
    if (!matches.empty()) {
        
        const RSCommand *first = matches.front();
        for (usize i = 0;; i++) {
            
            for (auto m: matches) {
                if (m->name.size() <= i || m->name[i] != first->name[i]) {
                    return result;
                }
            }
            result += first->name[i];
        }
    }
    
    return result.size() >= token.size() ? result : token;
}

string
RSCommand::usage() const
{
    string arguments;
    
    if (subCommands.empty()) {
        
        string required;
        string optional;
        
        for (isize i = 0; i < minArgs(); i++) {
            
            required += requiredArgs[i];
            required += " ";
        }
        for (isize i = 0; i < optArgs(); i++) {
            
            optional += optionalArgs[i];
            optional += " ";
        }
        if (optional != "") optional = "[ " + optional + "]";
        
        arguments = required + optional;
        
    } else {
        
        // Collect all sub-commands
        isize count = 0;
        for (auto &it : subCommands) {
            
            if (it.hidden) continue;
            
            if (it.name != "") {
                
                if (count++) arguments += " | ";
                arguments += it.name;
            }
        }
        if (count > 1) {
            arguments = "{" + arguments + "}";
        }
        if (seek("") && arguments != "") {
            arguments = "[ " + arguments + " ]";
        }
    }
    
    return fullName + " " + arguments;
}

}
