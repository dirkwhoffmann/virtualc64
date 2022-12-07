// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "Interpreter.h"
#include "RetroShell.h"
#include <sstream>

namespace vc64 {

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
    if (root.seek(tokens) != nullptr) { result += " "; }
    
    return result;
}

void
Interpreter::autoComplete(Arguments &argv)
{
    Command *current = &root;
    string prefix, token;

    for (auto it = argv.begin(); current && it != argv.end(); it++) {
        
        *it = current->autoComplete(*it);
        current = current->seek(*it);
    }
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
    Command *current = &root, *next;
    Arguments args = argv;

    while (!args.empty() && ((next = current->seek(args.front())) != nullptr)) {
        
        current = current->seek(args.front());
        args.erase(args.begin());
    }

    // Error out if no command handler is present
    if (current->action == nullptr && !args.empty()) {
        throw util::ParseError(args.front());
    }
    if (current->action == nullptr && args.empty()) {
        throw TooFewArgumentsError(current->tokens());
    }
    
    // Check the argument count
    if ((isize)args.size() < current->numArgs) throw TooFewArgumentsError(current->tokens());
    if ((isize)args.size() > current->numArgs) throw TooManyArgumentsError(current->tokens());
    
    // Call the command handler
    (retroShell.*(current->action))(args, current->param);
}

void
Interpreter::usage(const Command& current)
{
    retroShell << "Usage: " << current.usage() << '\n' << '\n';
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
    Command *current = &root;
    string prefix, token;
    
    for (auto &it : argv) {
        if (current->seek(it) != nullptr) current = current->seek(it);
    }
    
    help(*current);
}

void
Interpreter::help(const Command& current)
{
    retroShell << '\n';
    
    // Print the usage string
    usage(current);
    
    // Collect all argument types
    auto types = current.types();

    // Determine tabular positions to align the output
    isize tab = 0;
    for (auto &it : current.args) {
        tab = std::max(tab, (isize)it.token.length());
        tab = std::max(tab, 2 + (isize)it.type.length());
    }
    tab += 5;

    for (auto &it : types) {
        
        auto opts = current.filterType(it);
        isize size = (isize)it.length();

        retroShell.tab(tab - size);
        retroShell << "<" << it << "> : ";
        retroShell << (int)opts.size() << (opts.size() == 1 ? " choice" : " choices");
        retroShell << '\n' << '\n';
        
        for (auto &opt : opts) {

            string name = opt->token == "" ? "<>" : opt->token;
            retroShell.tab(tab + 2 - (isize)name.length());
            retroShell << name;
            retroShell << " : ";
            retroShell << opt->info;
            retroShell << '\n';
        }
        retroShell << '\n';
    }
}

}
