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
#include "Command.h"
#include <algorithm>
#include <utility>

namespace vc64 {

std::vector<string> Command::groups;
std::stack<isize> Command::groupStack;

void
Command::pushGroup(const string &description, const string &postfix)
{
    auto name = description.empty() ? "" : description + postfix;

    groupStack.push(isize(groups.size()));
    groups.push_back(name);
}

void
Command::popGroup()
{
    groupStack.pop();
}

void
Command::add(const std::vector<string> &tokens,
             const string &help,
             std::function<void (Arguments&, long)> func, long param)
{
    add(tokens, { }, { }, { tokens.back(), help }, func, param);
}

void
Command::add(const std::vector<string> &tokens,
             std::pair<const string &, const string &> help,
             std::function<void (Arguments&, long)> func, long param)
{
    add(tokens, { }, { }, help, func, param);
}

void
Command::add(const std::vector<string> &tokens,
             const std::vector<string> &arguments,
             const string &help,
             std::function<void (Arguments&, long)> func, long param)
{
    add(tokens, arguments, { }, { tokens.back(), help }, func, param);
}

void
Command::add(const std::vector<string> &tokens,
             const std::vector<string> &arguments,
             std::pair<const string &, const string &> help,
             std::function<void (Arguments&, long)> func, long param)
{
    add(tokens, arguments, { }, help, func, param);
}

void
Command::add(const std::vector<string> &tokens,
             const std::vector<string> &requiredArgs,
             const std::vector<string> &optionalArgs,
             const string &help,
             std::function<void (Arguments&, long)> func, long param)
{
    add(tokens, requiredArgs, optionalArgs, { tokens.back(), help }, func, param);
}

void
Command::add(const std::vector<string> &tokens,
             const std::vector<string> &requiredArgs,
             const std::vector<string> &optionalArgs,
             std::pair<const string &, const string &> help,
             std::function<void (Arguments&, long)> func, long param)
{
    assert(!tokens.empty());

    // Traverse the node tree
    Command *cmd = seek(std::vector<string> { tokens.begin(), tokens.end() - 1 });
    assert(cmd != nullptr);

    // Create the instruction
    Command d;
    d.name = tokens.back();
    // d.fullName = (cmd->fullName.empty() ? "" : cmd->fullName + " ") + tokens.back();
    d.fullName = (cmd->fullName.empty() ? "" : cmd->fullName + " ") + help.first;
    d.group = groupStack.top();
    d.requiredArgs = requiredArgs;
    d.optionalArgs = optionalArgs;
    d.help = help;
    d.callback = func;
    d.param = param;
    d.hidden = help.second.empty();

    // Register the instruction
    cmd->subCommands.push_back(d);
}

void 
Command::clone(const string &alias,
           const std::vector<string> &tokens,
           long param)
{
    clone(alias, tokens, "", param);
}

void
Command::clone(const string &alias, const std::vector<string> &tokens, const string &help, long param)
{
    assert(!tokens.empty());

    // Find the command to clone
    Command *cmd = seek(std::vector<string> { tokens.begin(), tokens.end() });
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

const Command *
Command::seek(const string& token) const
{
    for (auto &it : subCommands) {
        if (it.name == token) return &it;
    }
    return nullptr;
}

Command *
Command::seek(const string& token)
{
    return const_cast<Command *>(std::as_const(*this).seek(token));
}

const Command *
Command::seek(const std::vector<string> &tokens) const
{
    const Command *result = this;
    
    for (auto &it : tokens) {
        if ((result = result->seek(it)) == nullptr) break;
    }
    
    return result;
}

Command *
Command::seek(const std::vector<string> &tokens)
{
    return const_cast<Command *>(std::as_const(*this).seek(tokens));
}

std::vector<const Command *>
Command::filterPrefix(const string& prefix) const
{
    std::vector<const Command *> result;
    
    for (auto &it : subCommands) {
        
        if (it.hidden) continue;
        if (it.name.substr(0, prefix.size()) == prefix) result.push_back(&it);
    }

    return result;
}

string
Command::autoComplete(const string& token)
{
    string result = token;
    
    auto matches = filterPrefix(token);
    if (!matches.empty()) {
        
        const Command *first = matches.front();
        for (auto i = token.size(); i < first->name.size(); i++) {
            
            for (auto m: matches) {
                if (m->name.size() <= i || m->name[i] != first->name[i]) {
                    return result;
                }
            }
            result += first->name[i];
        }
    }
    return result;
}

string
Command::usage() const
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
