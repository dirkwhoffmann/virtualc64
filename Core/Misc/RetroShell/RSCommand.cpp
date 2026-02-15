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
#include "Parser.h"
#include "utl/support/Strings.h"
#include <algorithm>
#include <utility>

namespace vc64 {

string RSCommand::currentGroup;

string
RSArgumentDescriptor::nameStr() const
{
    return name[0];
}

string
RSArgumentDescriptor::helpStr() const
{
    return name.size() > 1 ? name[1] : "" ;
}

string
RSArgumentDescriptor::keyStr() const
 {
    if (key.empty()) {
 
        if (isStdArg())         return "";
        if (isKeyValuePair())   return nameStr();
        if (isFlag())           return "-" + nameStr();
    }
    return key;
 }
 
string
RSArgumentDescriptor::valueStr() const
 {
    if (value.empty()) {

        if (isStdArg())         return "<" + nameStr() + ">";
        if (isKeyValuePair())   return "<arg>";
        if (isFlag())           return "";
    }
    return value;
 }
 
string
RSArgumentDescriptor::keyValueStr() const
 {
    if (key.empty()) {

        if (isStdArg())         return valueStr();
        if (isKeyValuePair())   return keyStr() + "=" + valueStr();
        if (isFlag())           return keyStr();
    }
    return key;
 }
 
string
RSArgumentDescriptor::usageStr() const
 {
    return isHidden() ? "" : isRequired() ? keyValueStr() : "[" + keyValueStr() + "]";
 }
 
 void
RSCommand::add(const RSCommandDescriptor &descriptor)
 {
    assert(!descriptor.tokens.empty());
    assert(!descriptor.chelp.empty() || !descriptor.ghelp.empty());

    // Only register enabled commands
    if (descriptor.flags & rs::disabled) return;
 
 // Cleanse the token list (convert { "aaa bbb" } into { "aaa", "bbb" }
    auto tokens = utl::split(descriptor.tokens, ' ');
 
    // The last entry in the token list is the command name
    auto name = tokens.back();

    // Traversing the command tree
    RSCommand *node = seek(std::vector<string> { tokens.begin(), tokens.end() - 1 });
    assert(node != nullptr);
 
 // Create the instruction
    RSCommand cmd;
    cmd.groupName = currentGroup;
    cmd.name = name;
    cmd.fullName = utl::concat({ node->fullName, name }, " ");
    cmd.flags = descriptor.flags;
    cmd.ghelp = !descriptor.ghelp.empty() ? descriptor.ghelp : descriptor.chelp;
    cmd.chelp = !descriptor.chelp.empty() ? descriptor.chelp : "???";
    cmd.args = descriptor.args;
    cmd.callback = descriptor.func;
    cmd.payload = descriptor.payload;
 
    // Remove all disabled arguments
    cmd.args.erase(std::remove_if(cmd.args.begin(), cmd.args.end(), [](const RSArgumentDescriptor& arg) {
        return arg.flags & rs::disabled; }), cmd.args.end());
 
    // Reset the group
    if (cmd.isVisible()) currentGroup = "";
 
    // Register the instruction at the proper location
    node->subcommands.push_back(cmd);
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
    add(RSCommandDescriptor {

        .tokens = newTokens,
        .ghelp  = cmd->ghelp,
        .chelp  = cmd->chelp,
        .flags  = rs::hidden,
        .args   = cmd->args,
        .func   = cmd->callback,
        .payload = values
    });
 }

const RSCommand *
RSCommand::seek(const string& token) const
{
    for (auto &it : subcommands) {
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
    auto uprefix = utl::uppercased(prefix);
    
    for (auto &it : subcommands) {
        
        if (it.isHidden()) continue;
        auto substr = it.name.substr(0, prefix.size());
        if (utl::uppercased(substr) == uprefix) result.push_back(&it);
    }
    
    return result;
}

isize
RSCommand::autoComplete(string &token)
{
    std::vector<string> tokens;
    
    auto matches = filterPrefix(token);
    for (auto &it : matches) { tokens.push_back(it->name); }
            
    if (!tokens.empty()) token = utl::commonPrefix(tokens);
    return (isize)matches.size();
            }

void
RSCommand::printHelp(std::ostream &os)
{
    string prefix;

    if (subcommands.empty()) {

        // Describe the current command
        prefix = "Usage: ";
        os << prefix + argUsage() << std::endl;
        printArgumentHelp(os, isize(prefix.size()));

    } else {

        // Describe all subcommands
        prefix = "Commands: ";
        os << prefix + cmdUsage() << std::endl;
        printSubcmdHelp(os, isize(prefix.size()));

        if (callback && !args.empty()) {

            // Describe the current command
            prefix = string(prefix.size(), ' ') + "Usage: ";
            os << std::endl << prefix + argUsage() << std::endl;
            printArgumentHelp(os, isize(prefix.size()), false);
        }
    }
}

void
RSCommand::printArgumentHelp(std::ostream &os, isize indent, bool verbose)
{
    auto skip = [](const RSArgumentDescriptor &it) { return it.isHidden() || it.helpStr().empty(); };
    
    // Gather all arguments with a help description
    std::vector<RSArgumentDescriptor *> hargs;
    for (auto &it : args) { if (!skip(it)) hargs.push_back(&it); }
        
    // Determine the tabular position to align the output
    isize tab = 0;
    for (auto &it : hargs) { tab = std::max(tab, (isize)it->keyValueStr().length()); }
        
    // Print command description
    if (verbose) os << std::endl << string(indent, ' ') << chelp << std::endl;
            
    if (!hargs.empty()) {
            
        os << std::endl;

        // Print argument descriptions
        for (auto &it : hargs) {

            os << string(indent, ' ') << std::left << std::setw(int(tab)) << it->keyValueStr() << " : ";
            os << it->helpStr() << std::endl;
        }
    }
    // os << std::endl;
}
        
void
RSCommand::printSubcmdHelp(std::ostream &os, isize indent, bool verbose)
{
    if (subcommands.empty()) return;
        
    // Collect all commands that appear in the help description
    std::vector<const RSCommand *> cmds;
    if (callback) cmds.push_back(this);
    for (auto &it : subcommands) { if (it.isVisible()) cmds.push_back(&it); }
        
    // Determine alignment parameters to get a properly formatted output
    isize newlines = 1, tab = 0;
    for (auto &it : cmds) {
        tab = std::max(tab, (isize)it->fullName.length());
    }
            
    for (auto &it : cmds) {
            
        // For top-level commands, print the command group (if present)
        if (!it->groupName.empty() && name.empty()) {
                
            // *this << '\n' << it->groupName << '\n';
            os << std::endl << it->groupName << std::endl;
            newlines = 1;
            }

        // Print newlines
        for (; newlines > 0; newlines--) os << std::endl;

        // Print command description
        os << string(indent, ' ') << std::left << std::setw(int(tab)) << it->fullName << " : ";
        os << (it == this ? it->chelp : it->ghelp) << std::endl;
        }
    // os << std::endl;
        }

string
RSCommand::cmdUsage() const
{
    std::vector<string> items;

    for (auto &it : subcommands) {
        if (it.isVisible()) items.push_back(it.name);
        }
    auto combined = utl::concat(items, " | ", callback ? "[ " : "{ ", callback ? " ]" : " }");
    return  utl::concat({ fullName, combined });
    }
    
string
RSCommand::argUsage() const
{
    // Create a common usage string for all flags
    string flags = "";

    for (auto &it : args) {
        if (it.isFlag()) flags += it.nameStr()[0];
    }
    if (!flags.empty()) flags = "[-" + flags + "]";

    // Create a usage string for all other arguments
    std::vector<string> items;
    
    for (auto &it : args) {
        if (!it.isFlag()) items.push_back(it.usageStr());
    }
    string other = utl::concat(items);

    return utl::concat({ fullName, flags, other });
}

}
