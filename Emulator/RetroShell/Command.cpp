// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "Command.h"
#include <algorithm>

namespace vc64 {

std::vector<string> Command::groups;

void
Command::newGroup(const string &description, const string &postfix)
{
    groups.push_back(description.empty() ? "" : description + postfix);
}

void
Command::add(const std::vector<string> &tokens,
             const string &help)
{
    add(tokens, help, nullptr);
}

void
Command::add(const std::vector<string> &tokens,
             const string &help,
             void (RetroShell::*action)(Arguments&, long), long param)
{
    add(tokens, { }, { }, help, action, param);
}

void
Command::add(const std::vector<string> &tokens,
             const std::vector<string> &arguments,
             const string &help,
             void (RetroShell::*action)(Arguments&, long), long param)
{
    add(tokens, arguments, { }, help, action, param);
}

void
Command::add(const std::vector<string> &tokens,
             const std::vector<string> &requiredArgs,
             const std::vector<string> &optionalArgs,
             const string &help,
             void (RetroShell::*action)(Arguments&, long), long param)
{
    assert(!tokens.empty());

    // Traverse the node tree
    Command *cmd = seek(std::vector<string> { tokens.begin(), tokens.end() - 1 });
    assert(cmd != nullptr);

    // Create the instruction
    Command d;
    d.name = tokens.back();
    d.fullName = (cmd->fullName.empty() ? "" : cmd->fullName + " ") + tokens.back();
    d.group = isize(groups.size()) - 1;
    d.requiredArgs = requiredArgs;
    d.optionalArgs = optionalArgs;
    d.help = help;
    d.action = action;
    d.param = param;

    // Register the instruction
    cmd->subCommands.push_back(d);
}

void
Command::hide(const std::vector<string> &tokens)
{
    Command *cmd = seek(std::vector<string> { tokens.begin(), tokens.end() });
    assert(cmd != nullptr);

    cmd->hidden = true;
}

void
Command::remove(const string& token)
{
    for(auto it = std::begin(subCommands); it != std::end(subCommands); ++it) {
        if (it->name == token) { subCommands.erase(it); return; }
    }
}

Command *
Command::seek(const string& token)
{
    for (auto &it : subCommands) {
        if (it.name == token) return &it;
    }
    return nullptr;
}

Command *
Command::seek(const std::vector<string> &tokens)
{
    Command *result = this;

    for (auto &it : tokens) {
        if ((result = result->seek(it)) == nullptr) break;
    }

    return result;
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
            optional + " ";
        }
        if (optional != "") optional = "[ " + optional + "]";

        arguments = required + optional;

    } else {

        // Collect all sub-commands
        isize count = 0;
        for (auto &it : subCommands) {

            if (it.name != "") {

                if (count++) arguments += " | ";
                arguments += it.name;
            }
        }
        if (count > 1) {
            arguments = "{" + arguments + "}";
        }
        if (action && arguments != "") {
            arguments = "[ " + arguments + " ]";
        }
    }

    return fullName + " " + arguments;
}

}
