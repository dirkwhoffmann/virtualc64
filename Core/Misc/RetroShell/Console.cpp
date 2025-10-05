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
#include "Console.h"
#include "Emulator.h"
#include <istream>
#include <sstream>

namespace vc64 {

void
Console::_initialize()
{
    // Register commands
    initCommands(root);
    
    // Initialize the text storage
    clear();
    
    // Initialize the input buffer
    history.push_back( { "", 0 } );
}

Console&
Console::operator<<(char value)
{
    storage << value;
    remoteManager.rshServer << value;
    needsDisplay();
    return *this;
}

Console&
Console::operator<<(const string& value)
{
    storage << value;
    remoteManager.rshServer << value;
    needsDisplay();
    return *this;
}

Console&
Console::operator<<(int value)
{
    *this << std::to_string(value);
    return *this;
}

Console&
Console::operator<<(unsigned int value)
{
    *this << std::to_string(value);
    return *this;
}

Console &
Console::operator<<(long value)
{
    *this << std::to_string(value);
    return *this;
}

Console &
Console::operator<<(unsigned long value)
{
    *this << std::to_string(value);
    return *this;
}

Console &
Console::operator<<(long long value)
{
    *this << std::to_string(value);
    return *this;
}

Console &
Console::operator<<(unsigned long long value)
{
    *this << std::to_string(value);
    return *this;
}

Console &
Console::operator<<(std::stringstream &stream)
{
    string line;
    while(std::getline(stream, line)) {
        *this << line << '\n';
    }
    return *this;
}

const char *
Console::text()
{
    static string all;
    
    // Add the storage contents
    storage.text(all);
    
    // Add the input line
    all += input + " ";
    
    return all.c_str();
}

void
Console::tab(isize pos)
{
    auto count = pos - (isize)storage[storage.size() - 1].size();
    
    if (count > 0) {
        
        std::string fill(count, ' ');
        storage << fill;
        remoteManager.rshServer << fill;
        needsDisplay();
    }
}

void
Console::setStream(std::ostream &os)
{
    storage.ostream = &os;
}

void
Console::needsDisplay()
{
    retroShell.isDirty = true;
}

void
Console::clear()
{
    storage.clear();
    needsDisplay();
}

bool
Console::isEmpty()
{
    return storage.isCleared();
}

bool
Console::lastLineIsEmpty()
{
    return storage.lastLineIsEmpty();
}

/*
 void
 Console::printState()
 {
 std::stringstream ss;
 
 cpu.debugger.dumpLogBuffer(ss, 8);
 ss << "\n";
 c64.dump(Category::Current, ss);
 ss << "\n";
 cpu.disassembler.disassembleRange(ss, cpu.getPC0(), 8);
 ss << "\n";
 
 *this << ss;
 }
 */

void
Console::press(RetroShellKey key, bool shift)
{
    assert_enum(RetroShellKey, key);
    assert(ipos >= 0 && ipos < historyLength());
    assert(cursor >= 0 && cursor <= inputLength());
    
    switch(key) {
            
        case RetroShellKey::UP:
            
            if (ipos > 0) {
                
                // Save the input line if it is currently shown
                if (ipos == historyLength() - 1) history.back() = { input, cursor };
                
                auto &item = history[--ipos];
                input = item.first;
                cursor = item.second;
            }
            break;
            
        case RetroShellKey::DOWN:
            
            if (ipos < historyLength() - 1) {
                
                auto &item = history[++ipos];
                input = item.first;
                cursor = item.second;
            }
            break;
            
        case RetroShellKey::LEFT:
            
            if (cursor > 0) cursor--;
            break;
            
        case RetroShellKey::RIGHT:
            
            if (cursor < (isize)input.size()) cursor++;
            break;
            
        case RetroShellKey::DEL:
            
            if (cursor < inputLength()) {
                input.erase(input.begin() + cursor);
            }
            break;
            
        case RetroShellKey::CUT:
            
            if (cursor < inputLength()) {
                input.erase(input.begin() + cursor, input.end());
            }
            break;
            
        case RetroShellKey::BACKSPACE:
            
            if (cursor > 0) {
                input.erase(input.begin() + --cursor);
            }
            break;
            
        case RetroShellKey::HOME:
            
            cursor = 0;
            break;
            
        case RetroShellKey::END:
            
            cursor = (isize)input.length();
            break;
            
        case RetroShellKey::TAB:
            
            if (tabPressed) {
                
                // TAB was pressed twice
                retroShell.asyncExec("help \"" + input + "\"");
                
            } else {
                
                // Auto-complete the typed in command
                input = autoComplete(input);
                cursor = (isize)input.length();
            }
            break;
            
        case RetroShellKey::RETURN:
            
            pressReturn(shift);
            break;
            
        case RetroShellKey::CR:
            
            input = "";
            cursor = 0;
            break;
    }
    
    tabPressed = key == RetroShellKey::TAB;
    needsDisplay();
    
    assert(ipos >= 0 && ipos < historyLength());
    assert(cursor >= 0 && cursor <= inputLength());
}

void
Console::press(char c)
{
    switch (c) {
            
        case '\n':
            
            press(RetroShellKey::RETURN);
            break;
            
        case '\r':
            
            press(RetroShellKey::CR);
            break;
            
        case '\t':
            
            press(RetroShellKey::TAB);
            break;
            
        default:
            
            if (isprint(c)) {
                
                if (cursor < inputLength()) {
                    input.insert(input.begin() + cursor, c);
                } else {
                    input += c;
                }
                cursor++;
            }
    }
    
    tabPressed = false;
    needsDisplay();
}

void
Console::press(const string &s)
{
    for (auto c : s) press(c);
}

isize
Console::cursorRel()
{
    assert(cursor >= 0 && cursor <= inputLength());
    return cursor - (isize)input.length();
}

void
Console::pressReturn(bool shift)
{
    if (shift) {
        
        // Switch the interpreter
        // retroShell.switchConsole();
        retroShell.asyncExec(".");
        
    } else {
        
        // Add the command to the text storage
        *this << input << '\n';
        
        // Add the command to the history buffer
        history.back() = { input, (isize)input.size() };
        history.push_back( { "", 0 } );
        ipos = (isize)history.size() - 1;
        
        // Feed the command into the command queue
        retroShell.asyncExec(input);
        
        // Clear the input line
        input = "";
        cursor = 0;
    }
}

Arguments
Console::split(const string& userInput)
{
    std::stringstream ss(userInput);
    Arguments result;
    
    string token;
    bool str = false; // String mode
    bool esc = false; // Escape mode
    
    for (usize i = 0; i < userInput.size(); i++) {
        
        char c = userInput[i];
        
        // Abort if a comment begins
        if (c == '#') break;
        
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
Console::autoComplete(const string& userInput)
{
    string result;
    
    // Split input string
    Arguments tokens = split(userInput);
    
    // Complete all tokens
    autoComplete(tokens);
    
    // Recreate the command string
    for (const auto &it : tokens) { result += (result == "" ? "" : " ") + it; }
    
    // Add a space if the command has been fully completed ...
    if (auto cmd = getRoot().seek(tokens); cmd != nullptr && !tokens.empty()) {
        
        // ... and there are additional subcommands or arguments
        if (cmd->subCommands.size() > 0 ||
            cmd->requiredArgs.size() > 0 ||
            cmd->optionalArgs.size()) { result += " "; }
    }
    
    return result;
}

void
Console::autoComplete(Arguments &argv)
{
    RSCommand *current = &getRoot();
    string prefix, token;
    
    for (auto it = argv.begin(); current && it != argv.end(); it++) {
        
        *it = current->autoComplete(*it);
        current = current->seek(*it);
    }
}

bool
Console::isBool(const string &argv)
{
    return util::isBool(argv);
}

bool
Console::isOnOff(const string  &argv)
{
    return util::isOnOff(argv);
}

long
Console::isNum(const string &argv)
{
    return util::isNum(argv);
}

bool
Console::parseBool(const string &argv)
{
    return util::parseBool(argv);
}

bool
Console::parseBool(const string &argv, bool fallback)
{
    try { return parseBool(argv); } catch(...) { return fallback; }
}

bool
Console::parseBool(const Arguments &argv, long nr, long fallback)
{
    return nr < long(argv.size()) ? parseBool(argv[nr]) : fallback;
}

bool
Console::parseOnOff(const string &argv)
{
    return util::parseOnOff(argv);
}

bool
Console::parseOnOff(const string &argv, bool fallback)
{
    try { return parseOnOff(argv); } catch(...) { return fallback; }
}

bool
Console::parseOnOff(const Arguments &argv, long nr, long fallback)
{
    return nr < long(argv.size()) ? parseOnOff(argv[nr]) : fallback;
}

long
Console::parseNum(const string &argv)
{
    return util::parseNum(argv);
}

long
Console::parseNum(const string &argv, long fallback)
{
    try { return parseNum(argv); } catch(...) { return fallback; }
}

long
Console::parseNum(const Arguments &argv, long nr, long fallback)
{
    return nr < long(argv.size()) ? parseNum(argv[nr]) : fallback;
}

string
Console::parseSeq(const string &argv)
{
    return util::parseSeq(argv);
}

string
Console::parseSeq(const string &argv, const string &fallback)
{
    try { return parseSeq(argv); } catch(...) { return fallback; }
}

void
Console::exec(const string& userInput, bool verbose)
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
Console::exec(const Arguments &argv, bool verbose)
{
    // In 'verbose' mode, print the token list
    if (verbose) {
        for (const auto &it : argv) *this << it << ' ';
        *this << '\n';
    }
    
    // Skip empty lines
    if (argv.empty()) return;
    
    // Seek the command in the command tree
    RSCommand *current = &getRoot(), *next;
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
    if (current->param.empty()) {
        current->callback(args, { 0 });
    } else {
        current->callback(args, current->param);
        
    }
}

void
Console::usage(const RSCommand& current)
{
    *this << '\r' << "Usage: " << current.usage() << '\n';
}

void
Console::help(const string& userInput)
{
    // Split the command string
    Arguments tokens = split(userInput);
    
    // Auto complete the token list
    autoComplete(tokens);
    
    // Process the command
    help(tokens);
}

void
Console::help(const Arguments &argv)
{
    RSCommand *current = &getRoot();
    string prefix, token;
    
    for (auto &it : argv) {
        if (current->seek(it) != nullptr) current = current->seek(it);
    }
    
    help(*current);
}

void
Console::help(const RSCommand& current)
{
    auto indent = string("    ");
    
    // Print the usage string
    usage(current);
    
    // Determine tabular positions to align the output
    isize tab = 0;
    for (auto &it : current.subCommands) {
        tab = std::max(tab, (isize)it.fullName.length());
    }
    tab += (isize)indent.size();
    
    isize newlines = 1;
    
    for (auto &it : current.subCommands) {
        
        // Only proceed if the command is visible
        if (it.hidden || it.help.empty() || it.help[0] == "") continue;
        
        // Print the group (if present)
        if (!it.groupName.empty()) {
            
            *this << '\n' << it.groupName << '\n';
            newlines = 1;
        }
        
        // Print newlines
        for (; newlines > 0; newlines--) {
            *this << '\n';
        }
        
        // Print command descriptioon
        *this << indent;
        *this << it.fullName;
        (*this).tab(tab);
        *this << " : ";
        *this << it.help[0];
        *this << '\n';
    }
    
    *this << '\n';
}

void
Console::describe(const std::exception &e, isize line, const string &cmd)
{
    if (line) *this << "Line " << line << ": " << cmd << '\n';
    
    if (auto err = dynamic_cast<const TooFewArgumentsError *>(&e)) {
        
        *this << err->what() << ": Too few arguments";
        *this << '\n';
        return;
    }
    
    if (auto err = dynamic_cast<const TooManyArgumentsError *>(&e)) {
        
        *this << err->what() << ": Too many arguments";
        *this << '\n';
        return;
    }
    
    if (auto err = dynamic_cast<const util::EnumParseError *>(&e)) {
        
        *this << err->token << " is not a valid key" << '\n';
        *this << "Expected: " << err->expected << '\n';
        return;
    }
    
    if (auto err = dynamic_cast<const util::ParseNumError *>(&e)) {
        
        *this << err->token << " is not a number";
        *this << '\n';
        return;
    }
    
    if (auto err = dynamic_cast<const util::ParseBoolError *>(&e)) {
        
        *this << err->token << " must be true or false";
        *this << '\n';
        return;
    }
    
    if (auto err = dynamic_cast<const util::ParseOnOffError *>(&e)) {
        
        *this << "'" << err->token << "' must be on or off";
        *this << '\n';
        return;
    }
    
    if (auto err = dynamic_cast<const util::ParseError *>(&e)) {
        
        *this << err->what() << ": Syntax error";
        *this << '\n';
        return;
    }
    
    if (auto err = dynamic_cast<const AppError *>(&e)) {
        
        *this << err->what();
        *this << '\n';
        return;
    }
}

void
Console::dump(CoreObject &component, Category category)
{
    *this << '\n';
    _dump(component, category);
}

void
Console::dump(CoreObject &component, std::vector <Category> categories)
{
    *this << '\n';
    for(auto &category : categories) _dump(component, category);
}

void
Console::_dump(CoreObject &component, Category category)
{
    std::stringstream ss;
    
    switch (category) {
            
        case Category::Slots:       ss << "Slots:\n\n"; break;
        case Category::Config:      ss << "Configuration:\n\n"; break;
        case Category::Properties:  ss << "Properties:\n\n"; break;
        case Category::Registers:   ss << "Registers:\n\n"; break;
        case Category::State:       ss << "State:\n\n"; break;
        case Category::Stats:       ss << "Statistics:\n\n"; break;
            
        default:
            break;
    }
    
    component.dump(category, ss);
    
    *this << ss << '\n';
}

void
Console::initCommands(RSCommand &root)
{
    //
    // Common commands
    //
    
    RSCommand::currentGroup = "Shell commands";
    
    root.add({
        
        .tokens = { "welcome" },
        .hidden = true,
        .help   = { "Prints the welcome message" },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            welcome();
        }
    });
    
    root.add({
        
        .tokens = { "." },
        .help   = { "Enter or exit the debugger" },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            retroShell.switchConsole();
        }
    });
    
    root.add({
        
        .tokens = { "clear" },
        .help   = { "Clear the console window" },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            clear();
        }
    });
    
    root.add({
        
        .tokens = { "close" },
        .help   = { "Hide the console window" },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            msgQueue.put(Msg::RSH_CLOSE);
        }
    });
    
    root.add({
        
        .tokens = { "help" },
        .extra  = { Arg::command },
        .help   = { "Print usage information" },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            help(argv.empty() ? "" : argv.front());
        }
    });
    
    root.add({
        
        .tokens = { "state" },
        .hidden = true,
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            dump(c64, Category::Trace);
        }
    });
    
    root.add({
        
        .tokens = { "joshua" },
        .hidden = true,
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            *this << "\nGREETINGS PROFESSOR HOFFMANN.\n";
            *this << "THE ONLY WINNING MOVE IS NOT TO PLAY.\n";
            *this << "HOW ABOUT A NICE GAME OF CHESS?\n\n";
        }
    });
    
    root.add({
        
        .tokens = { "source" },
        .args   = { Arg::path },
        .help   = { "Process a command script" },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            auto stream = std::ifstream(argv.front());
            if (!stream.is_open()) throw AppError(Fault::FILE_NOT_FOUND, argv.front());
            retroShell.asyncExecScript(stream);
        }
    });
    
    root.add({
        
        .tokens = { "wait" },
        .hidden = true,
        .args   = { Arg::value, Arg::seconds },
        .help   = { "Pause the execution of a command script" },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            auto seconds = parseNum(argv[0]);
            c64.scheduleRel<SLOT_RSH>(C64::sec(seconds), RSH_WAKEUP);
            throw ScriptInterruption();
        }
    });
    
    root.add({
        
        .tokens = { "shutdown" },
        .help   = { "Terminates the application" },
        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
            
            msgQueue.put(Msg::ABORT, 0);
        }
    });
}

const char *
Console::registerComponent(CoreComponent &c)
{
    return registerComponent(c, root);
}

const char *
Console::registerComponent(CoreComponent &c, RSCommand &root)
{
    // Get the shell name for this component
    auto cmd = c.shellName();
    assert(cmd != nullptr);
    
    // Register a command with the proper name
    if (c.shellHelp().empty()) {
        root.add( { .tokens = { cmd }, .help = { c.description() } } );
    } else {
        root.add( {.tokens = { cmd }, .help = c.shellHelp() } );
    }
    
    // In case this component has options...
    if (auto &options = c.getOptions(); !options.empty()) {
        
        // Register a command for querying the current configuration
        root.add({
            
            .tokens = { cmd, ""},
            .help   = { "Display the current configuration" },
            .func   = [this, &c] (Arguments& argv, const std::vector<isize> &values) {
                
                retroShell.commander.dump(c, Category::Config);
            }
        });
        
        // Register a setter for every option
        root.add({
            
            .tokens = { cmd, "set" },
            .help   = { "Configure the component" }
        });
        
        for (auto &opt : options) {
            
            // Get the key value pairs
            auto pairs = OptionParser::pairs(opt);
            
            if (pairs.empty()) {
                
                // The argument is not an enum. Register a single setter
                root.add({
                    
                    .tokens = { cmd, "set", OptEnum::key(opt) },
                    .args   = { OptionParser::argList(opt) },
                    .help   = { OptEnum::help(opt) },
                    .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
                        
                        emulator.set(Opt(values[0]), argv[0], { values[1] });
                        // msgQueue.put(Msg::CONFIG);
                        
                    }, .values = { isize(opt), c.objid }
                });
                
            } else {
                
                // Register a setter for every enum
                root.add({
                    
                    .tokens = { cmd, "set", OptEnum::key(opt) },
                    .args   = { OptionParser::argList(opt) },
                    .help   = { OptEnum::help(opt) }
                });
                
                for (const auto& [first, second] : pairs) {
                    
                    auto help = OptionParser::help(opt, second);
                    root.add({
                        
                        .tokens = { cmd, "set", OptEnum::key(opt), first },
                        .help   = { help.empty() ? "Set to " + first : help },
                        .func   = [this] (Arguments& argv, const std::vector<isize> &values) {
                            
                            emulator.set(Opt(values[0]), values[1], { values[2] });
                            // msgQueue.put(Msg::CONFIG);
                            
                        },  .values = { isize(opt), isize(second), c.objid }
                    });
                }
            }
        }
        
        /*
         // ...register a command for querying the current configuration
         root.add({cmd, ""},
         "Display the current configuration",
         [this, &c](Arguments& argv, const std::vector<isize> &values) {
         
         retroShell.commander.dump(c, Category::Config);
         });
         
         // ...register a setter for all config options
         root.add({cmd, "set"}, "Configure the component");
         for (auto &opt : options) {
         
         root.add({cmd, "set", OptEnum::key(opt)},
         {OptionParser::argList(opt)},
         OptEnum::help(opt),
         [this](Arguments& argv, const std::vector<isize> &values) {
         
         emulator.set(Option(HI_WORD(values.front())), argv[0], { LO_WORD(values.front()) });
         
         }, HI_W_LO_W(opt, c.objid));
         }
         */
    }
    
    return cmd;
}

}
