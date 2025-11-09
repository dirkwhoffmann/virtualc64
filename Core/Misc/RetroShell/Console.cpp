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
#include "Parser.h"
#include "Option.h"
#include <istream>
#include <sstream>
#include <string>

namespace vc64 {

HistoryBuffer Console::historyBuffer;

void
Console::willExecute(const InputLine &input)
{
    // Echo the command if it came from somewhere else
    if (!input.isUserCommand()) { *this << input.input << '\n'; }
}

void
Console::didExecute(const InputLine& input, std::stringstream &ss)
{
    if (ss.peek() != EOF) {
        *this << vdelim << ss.str() << vdelim;
    }
}

void
Console::didExecute(const InputLine& input, std::stringstream &ss, std::exception &exc)
{
    describe(ss, exc, input.id, input.input);

    if (ss.peek() != EOF) {
        *this << vdelim << ss.str() << vdelim;
    }
}

void
HistoryBuffer::up(string &input, isize &cursor)
{
    if (ipos > 0) {
        
        // Save the input line if it is currently shown
        if (ipos == isize(history.size() - 1)) history.back() = { input, cursor };
        
        auto &item = history[--ipos];
        input = item.first;
        cursor = item.second;
    }
}

void
HistoryBuffer::down(string &input, isize &cursor)
{
    if (ipos < isize(history.size() - 1)) {
        
        auto &item = history[++ipos];
        input = item.first;
        cursor = item.second;
    }
}

void
HistoryBuffer::add(const string &input)
{
    history.back() = { input, (isize)input.size() };
    history.push_back( { "", 0 } );
    ipos = (isize)history.size() - 1;
}

void
Console::_initialize()
{
    // Register commands
    initCommands(root);
    
    // Initialize the text storage
    clear();

    // Register as delegate to receive command output
    delegates.push_back(this);
}

Console&
Console::operator<<(char value)
{
    storage << value;
    needsDisplay();
    return *this;
}

Console&
Console::operator<<(const string& value)
{
    storage << value;
    needsDisplay();
    return *this;
}

Console&
Console::operator<<(const char *value)
{
    *this << string(value);
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
Console::operator<<(const std::vector<string> &vec)
{
    *this << util::concat(vec);
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

Console&
Console::operator<<(const vspace &value)
{
    auto blanks = storage.trailingEmptyLines();
    while (blanks++ <= value.lines) {
        *this << '\n';
    }
    return *this;
}

void
Console::welcome()
{
    *this << "RetroShell " << C64::build() << '\n';
    *this << '\n';
    *this << "Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de" << '\n';
    *this << "https://github.com/dirkwhoffmann/virtualc64" << '\n';
    *this << '\n';

    printHelp(0);
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

void
Console::printHelp(isize tab)
{
    *this << vspace{1};
    *this << "RetroShell " << description() << " " << C64::version() << "\n\n";
    *this << string(tab + 4, ' ') << "Type 'help' or press 'Tab' twice for help.\n";
    *this << string(tab + 4, ' ') << "Press 'Shift+Tab' to switch consoles.";
    *this << vspace{1};
}

void
Console::press(RSKey key, bool shift)
{
    assert_enum(RSKey, key);
    // assert(ipos >= 0 && ipos < historyLength());
    assert(cursor >= 0 && cursor <= inputLength());
    
    switch(key) {
            
        case RSKey::UP:
            
            historyBuffer.up(input, cursor);
            break;
            
        case RSKey::DOWN:
            
            historyBuffer.down(input, cursor);
            break;
            
        case RSKey::LEFT:
            
            if (cursor > 0) cursor--;
            break;
            
        case RSKey::RIGHT:
            
            if (cursor < (isize)input.size()) cursor++;
            break;

        case RSKey::PAGE_UP:
        case RSKey::PAGE_DOWN:

            break;

        case RSKey::DEL:
            
            if (cursor < inputLength()) {
                input.erase(input.begin() + cursor);
            }
            break;
            
        case RSKey::CUT:
            
            if (cursor < inputLength()) {
                input.erase(input.begin() + cursor, input.end());
            }
            break;
            
        case RSKey::BACKSPACE:
            
            if (cursor > 0) {
                input.erase(input.begin() + --cursor);
            }
            break;
            
        case RSKey::HOME:
            
            cursor = 0;
            break;
            
        case RSKey::END:
            
            cursor = (isize)input.length();
            break;
            
        case RSKey::TAB:
            
            if (tabPressed++) {
                
                // TAB was pressed multiple times in a row
                *this << input << '\n';
                retroShell.asyncExec("help \"" + input + "\" TAB=" + std::to_string(tabPressed));
                
            } else {
                
                // Auto-complete the typed in command
                input = autoComplete(input);
                cursor = (isize)input.length();
            }
            break;
            
        case RSKey::RETURN:
            
            // Remember the command
            historyBuffer.add(input);
            
            pressReturn(shift);
            break;
            
        case RSKey::CR:
            
            input = "";
            cursor = 0;
            break;
    }
    
    if (key != RSKey::TAB) tabPressed = 0;
    needsDisplay();
    
    assert(cursor >= 0 && cursor <= inputLength());
}

void
Console::press(char c)
{
    switch (c) {
            
        case '\n':
            
            press(RSKey::RETURN);
            break;
            
        case '\r':
            
            press(RSKey::CR);
            break;
            
        case '\t':
            
            press(RSKey::TAB);
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
    
    tabPressed = c == '\t';
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
    if (!input.empty()) {

        // Add the command to the text storage
        *this << input << '\n';
    }

    // Feed the command into the command queue
    retroShell.asyncExec(input);

    // Clear the input line
    input = "";
    cursor = 0;

    /*
    if (input.empty()) {

        retroShell.asyncExec("ping");

    } else {
        
        // Add the command to the text storage
        *this << input << '\n';
                
        // Feed the command into the command queue
        retroShell.asyncExec(input);
        
        // Clear the input line
        input = "";
        cursor = 0;
    }
    */
}

Tokens
Console::split(const string& userInput)
{
    std::stringstream ss(userInput);
    Tokens result;
    
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

std::pair<RSCommand *, std::vector<string>>
Console::seekCommand(const string &argv)
{
    return seekCommand(split(argv));
}

std::pair<RSCommand *, std::vector<string>>
Console::seekCommand(const std::vector<string> &argv)
{
    std::vector<string> args = argv;
    RSCommand *cmd = nullptr;
    
    for (auto *it = &root; !args.empty() && (it = it->seek(args.front())); ) {
        
        args.erase(args.begin());
        cmd = it;
    }
    return { cmd ? cmd : &root, args };
}

string
Console::autoComplete(const string& userInput)
{
    // Split the input string
    Tokens tokens = split(userInput);
    
    // Complete the last token
    autoComplete(tokens);
    
    // Recreate the command string
    string result = util::concat(tokens);
    
    // Add a space if the command has been fully completed ...
    if (auto cmd = getRoot().seek(tokens); cmd && !tokens.empty()) {
        
        // ... and there are additional subcommands or arguments
        if (!cmd->subcommands.empty() || !cmd->args.empty()) { result += " "; }
    }
    
    return result;
}

void
Console::autoComplete(Tokens &argv)
{
    RSCommand *current = &getRoot();
    string prefix, token;
    
    for (auto it = argv.begin(); current && it != argv.end(); it++) {
        
        current->autoComplete(*it);
        current = current->seek(*it);
    }
}

std::map<string,string>
Console::parse(const RSCommand &cmd, const Tokens &args)
{
    std::map<string,string> map;
    std::vector<string> flags;
    std::vector<string> keyVal;
    std::vector<string> std;
    
    // Check if a command handler is present
    if (!cmd.callback)  { throw TooFewArgumentsError(cmd.fullName); }
    
    // Sort input tokens by type
    for (usize i = 0; i < args.size(); i++) {
        
        auto token = args[i];
        map[std::to_string(i)] = token;
        
        if (token[0] == '-') {
            for (usize j = 1; j < token.size(); j++) flags.push_back(string("-") + token[j]);
        } else if (token.find('=') != std::string::npos) {
            keyVal.push_back(token);
        } else {
            std.push_back(token);
    }
}
    
    // Iterate over all argument descriptors
    for (auto &descr : cmd.args) {
        
        auto keyStr = descr.keyStr();
        auto nameStr = descr.nameStr();
        
        // Does the descriptor describe a flag?
        if (descr.isFlag()) {
            
            bool found = false;
            for (auto it = flags.begin(); it != flags.end(); it++) {

                if (keyStr == *it) {
                    
                    map[nameStr] = "true";
                    flags.erase(it);
                    found = true;
                    break;
                }
            }
            if (!found && descr.isRequired()){
                throw util::ParseError("Missing flag " + keyStr);
            }
            continue;
        }
        
        // Does the descriptor describe a key-value pair?
        if (descr.isKeyValuePair()) {
            
            bool found = false;
            for (auto it = keyVal.begin(); it != keyVal.end(); it++) {
                
                auto pos = it->find('=');
                auto key = it->substr(0, pos);
                auto val = it->substr(pos + 1);
                
                if (keyStr == key) {
                    
                    map[nameStr] = val;
                    keyVal.erase(it);
                    found = true;
                    break;
                }
            }
            if (!found && descr.isRequired()) {
                throw util::ParseError("Missing key-value pair " + descr.keyValueStr());
            }
            continue;
        }
        
        // Does the descriptor describe a standard argument?
        if (descr.isStdArg()) {
            
            if (!std.empty()) {
                
                map[nameStr] = std.front();
                std.erase(std.begin());
                
            } else if (descr.isRequired()) {
                
                throw TooFewArgumentsError(cmd.fullName);
            }
            continue;
        }
        
        fatalError;
    }
    
    // Print some debug information
    for (auto &it : map) debug(RSH_DEBUG, "arg['%s']='%s'\n", it.first.c_str(), it.second.c_str());
    
    // Check for invalid or extra arguments
    if (!flags.empty()) { throw UnknownFlagError(flags.front()); }
    if (!keyVal.empty()) { throw UnknownKeyValueError(keyVal.front()); }
    if (!std.empty()) { throw TooManyArgumentsError(cmd.fullName); }
    
    return map;
}

bool
Console::isBool(const string &argv) const
{
    return util::isBool(argv);
}

bool
Console::isOnOff(const string  &argv) const
{
    return util::isOnOff(argv);
}

long
Console::isNum(const string &argv) const
{
    return util::isNum(argv);
}

bool
Console::parseBool(const string &argv) const
{
    return util::parseBool(argv);
}

bool
Console::parseBool(const string &argv, bool fallback) const
{
    try { return parseBool(argv); } catch(...) { return fallback; }
}

bool
Console::parseBool(const Arguments &argv, const string &key) const
{
    assert(argv.contains(key));
    return parseBool(argv.at(key));
}

bool
Console::parseBool(const Arguments &argv, const string &key, long fallback) const
{
    return argv.contains(key) ? parseBool(argv.at(key)) : fallback;
}

bool
Console::parseOnOff(const string &argv) const
{
    return util::parseOnOff(argv);
}

bool
Console::parseOnOff(const string &argv, bool fallback) const
{
    try { return parseOnOff(argv); } catch(...) { return fallback; }
}

bool
Console::parseOnOff(const Arguments &argv, const string &key, long fallback) const
{
    return argv.contains(key) ? parseBool(argv.at(key)) : fallback;
}

bool
Console::parseOnOff(const Arguments &argv, const string &key) const
{
    assert(argv.contains(key));
    return parseBool(argv.at(key));
}

long
Console::parseNum(const string &argv) const
{
    return util::parseNum(argv);
}

long
Console::parseNum(const string &argv, long fallback) const
{
    try { return parseNum(argv); } catch(...) { return fallback; }
}

long
Console::parseNum(const Arguments &argv, const string &key) const
{
    assert(argv.contains(key));
    return parseNum(argv.at(key));
}

long
Console::parseNum(const Arguments &argv, const string &token, long fallback) const
{
    return argv.contains(token) ? parseNum(argv.at(token)) : fallback;
}

u16
Console::parseAddr(const string &argv) const
{
    if (auto resolved = cpu.symbolTable.symbols.seek(argv); resolved) {
        return resolved->val;
    }
    return (u16)parseNum(argv);
}

u16
Console::parseAddr(const string &argv, long fallback) const
{
    if (auto resolved = cpu.symbolTable.symbols.seek(argv); resolved) {
        return resolved->val;
    }
    return (u16)parseNum(argv, fallback);
}

u16
Console::parseAddr(const Arguments &argv, const string &key) const
{
    assert(argv.contains(key));
    return parseAddr(argv.at(key));
}

u16
Console::parseAddr(const Arguments &argv, const string &key, long fallback) const
{
    return argv.contains(key) ? parseAddr(argv.at(key)) : (u16)fallback;
}

string
Console::parseSeq(const string &argv) const
{
    return util::parseSeq(argv);
}

string
Console::parseSeq(const string &argv, const string &fallback) const
{
    try { return parseSeq(argv); } catch(...) { return fallback; }
}

void
Console::exec(const InputLine& cmd)
{
    std::stringstream ss;

    // Skip empty script lines
    if (cmd.isScriptCommand() && cmd.input.empty()) return;

    // Inform the delegates
    for (auto &delegate: delegates) delegate->willExecute(cmd);

    try {

        // Split the command string
        Tokens tokens = split(cmd.input);

        // Remove the 'try' keyword
        if (!tokens.empty() && tokens.front() == "try") tokens.erase(tokens.begin());

        // Reroute empty commands to the hidden "return" command
        if (tokens.empty()) tokens = { "return" };

        // Find the command in the command tree
        auto [c, args] = seekCommand(tokens);

        // Only proceed if a command has been found
        if (c == &root) throw util::ParseError(tokens[0]);

        // Parse arguments
        Arguments parsedArgs = parse(*c, args);

        // Call the command handler
        c->callback(ss, parsedArgs, c->payload);

        // Dispatch output
        for (auto &delegate: delegates) delegate->didExecute(cmd, ss);

    } catch (std::exception &err) {

        // Dispatch error message
        for (auto &delegate: delegates) delegate->didExecute(cmd, ss, err);

        // Rethrow exception
        throw;
    }
}

void
Console::cmdUsage(const RSCommand& current, const string &prefix)
{
    *this << '\r' << prefix << current.cmdUsage() << '\n';
}

void
Console::argUsage(const RSCommand& current, const string &prefix)
{
    *this << '\r' << prefix << current.argUsage() << '\n';
}

void
Console::help(std::ostream &os, const string& userInput, isize tabs)
{
    if (auto [cmd, args] = seekCommand(userInput); cmd) {
        cmd->printHelp(os);
    }
}

void
Console::describe(const std::exception &exc, isize line, const string &cmd)
{
    std::stringstream ss;
    describe(ss, exc, line, cmd);
    *this << vdelim << ss.str() << vdelim;
}

void
Console::describe(std::ostream &ss, const std::exception &e, isize line, const string &cmd)
{
    if (line) {
        ss << "Line " << line << ": " << cmd << '\n';
    }
    // ss << "Error: ";
    
    if (auto err = dynamic_cast<const TooFewArgumentsError *>(&e)) {
        
        ss << err->what() << ": Too few arguments.";
        ss << '\n';
        return;
    }
    if (auto err = dynamic_cast<const TooManyArgumentsError *>(&e)) {
        
        ss << err->what() << ": Too many arguments.";
        ss << '\n';
        return;
    }
    if (auto err = dynamic_cast<const UnknownFlagError *>(&e)) {
    
        ss << err->what() << " is not a valid flag.";
        ss << '\n';
        return;
    }
    if (auto err = dynamic_cast<const UnknownKeyValueError *>(&e)) {
        
        ss << err->what() << " is not a valid key-value pair.";
        ss << '\n';
        return;
    }
    if (auto err = dynamic_cast<const util::EnumParseError *>(&e)) {
    
        ss << err->token << " is not a valid key." << '\n';
        ss << "Expected: " << err->expected << '\n';
        return;
    }
    if (auto err = dynamic_cast<const util::ParseNumError *>(&e)) {
        
        ss << err->token << " is not a number.";
        ss << '\n';
        return;
    }
    if (auto err = dynamic_cast<const util::ParseBoolError *>(&e)) {
        
        ss << err->token << " must be true or false.";
        ss << '\n';
        return;
    }
    if (auto err = dynamic_cast<const util::ParseOnOffError *>(&e)) {
        
        ss << "'" << err->token << "' must be on or off.";
        ss << '\n';
        return;
    }
    if (auto err = dynamic_cast<const util::ParseError *>(&e)) {
        
        if (auto what = string(err->what()); !what.empty()) {
            ss << err->what() << ": ";
        }
        ss << "Syntax error\n";
        return;
    }
    if (auto err = dynamic_cast<const AppError *>(&e)) {
        
        ss << err->what();
        ss << '\n';
        return;
    }
    
    ss << e.what();
}

void
Console::dump(std::ostream &os, CoreObject &component, Category category)
{
    _dump(os, component, category);
}

void
Console::dump(std::ostream &os, CoreObject &component, std::vector <Category> categories)
{
    for (usize i = 0; i < categories.size(); i++) {
        
        if (i) os << std::endl;
        _dump(os, component, categories[i]);
    }
}

void
Console::_dump(std::ostream &os, CoreObject &component, Category category)
{
    switch (category) {
            
        case Category::Slots:       os << "Slots:\n\n"; break;
        case Category::Config:      os << "Configuration:\n\n"; break;
        case Category::Properties:  os << "Properties:\n\n"; break;
        case Category::Registers:   os << "Registers:\n\n"; break;
        case Category::State:       os << "State:\n\n"; break;
        case Category::Stats:       os << "Statistics:\n\n"; break;
            
        default:
            break;
    }
    
    component.dump(category, os);
}

void
Console::initCommands(RSCommand &root)
{
    //
    // Common commands
    //
    
    {   RSCommand::currentGroup = "Shell commands";
    
    root.add({
        
        .tokens = { "welcome" },
            .chelp  = { "Prints the welcome message" },
            .flags  = rs::hidden,
            
            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {
                
            welcome();
            }
        });
        
        root.add({
            
            .tokens = { "ping" },
            .chelp  = { "Print a status message" },
            .flags  = rs::hidden,
            
            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {
                
                ping(os);
        }
    });
    
    root.add({
        
        .tokens = { "commander" },
            .chelp  = { "Enter the command console" },

            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                retroShell.enterCommander();

                os << "RetroShell Commander" << " " << C64::version() << "\n\n";
                os << string(4, ' ') << "Type 'help' or press 'Tab' twice for help.\n";
                os << string(4, ' ') << "Press 'Shift+Tab' to switch consoles.\n";
        }
    });

        root.add({

            .tokens = { "debugger" },
            .chelp  = { "Enter the debug console" },

            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                retroShell.enterDebugger();
                c64.dump(Category::Current, os);
            }
        });


        root.add({

            .tokens = { "clear" },
            .chelp  = { "Clear the console window" },

            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                clear();
        }
    });
    
    root.add({
        
        .tokens = { "close" },
            .chelp  = { "Hide the console window" },
            
            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {
                
            msgQueue.put(Msg::RSH_CLOSE);
        }
    });
    
    root.add({
        
        .tokens = { "help" },
            .chelp  = { "Print usage information" },
            .args   = {
                { .name = { "command", "Command name" }, .flags = rs::opt },
                { .name = { "TAB", "" }, .flags = rs::keyval | rs::hidden }
            },
                .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {
            
                    help(os, args.contains("command") ? args.at("command") : "", parseNum(args, "TAB", 0));
        }
    });
    
    root.add({
        
        .tokens = { "state" },
            .chelp  = { "Prints information about the current emulator state" },
            .flags  = rs::hidden,
            
            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {
                
                dump(os, c64, Category::Trace);
        }
    });
    
    root.add({
        
        .tokens = { "joshua" },
            .chelp  = { "Easter egg" },
            .flags  = rs::hidden,
            
            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {
                
                os << "\nGREETINGS PROFESSOR HOFFMANN.\n";
                os << "THE ONLY WINNING MOVE IS NOT TO PLAY.\n";
                os << "HOW ABOUT A NICE GAME OF CHESS?\n\n";
                
                msgQueue.put(Msg::EASTER_EGG);
        }
    });
    
    root.add({
        
        .tokens = { "source" },
            .chelp  = { "Process a command script" },
            .args   = { { .name = { "path", "Script file" } } },
            
            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {
                
                auto path = host.makeAbsolute(args.at("path"));
                auto stream = std::ifstream(path);
                if (!stream.is_open()) throw AppError(Fault::FILE_NOT_FOUND, path);
            retroShell.asyncExecScript(stream);
        }
    });
    
    root.add({
        
        .tokens = { "wait" },
            .chelp  = { "Pause the execution of a command script" },
            .flags  = rs::hidden,
            .args   = { { .name = { "seconds", "Delay" } } },
            
            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {
                
                auto seconds = parseNum(args.at("seconds"));
                c64.scheduleRel<SLOT_RSH>(C64::sec(seconds), RSH_WAKEUP);
                throw ScriptInterruption();
        }
    });
    
    root.add({
        
        .tokens = { "shutdown" },
            .chelp   = { "Terminates the application" },
            
            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {
            
            msgQueue.put(Msg::ABORT, 0);
        }
    });
    }
}

const char *
Console::registerComponent(CoreComponent &c, usize flags)
{
    return registerComponent(c, root, flags);
}

const char *
Console::registerComponent(CoreComponent &c, RSCommand &root, usize flags)
{
    // Get the shell name and the options for this component
    auto cmd = c.shellName();
    auto descr = c.description();
    auto &options = c.getOptions();
    
    // In case this component has no options, register a stub
    if (options.empty()) {
    
        root.add({
        
            .tokens = { cmd },
            .ghelp  = { descr }
        });
        
    } else {
        
        // Register a command for querying the current configuration
        root.add({
            
            .tokens = { cmd },
            .ghelp  = descr,
            .chelp  = { "Display the current configuration" },
            .flags  = flags,

            .func   = [this, &c] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {
                
                retroShell.commander.dump(os, c, Category::Config);
            }
        });
        
        // Register a setter for every option
        root.add({
            
            .tokens = { cmd, "set" },
            .ghelp  = { "Configure the component" }
        });
        
        for (auto &opt : options) {
            
            // Get the key value pairs
            auto pairs = OptionParser::pairs(opt);
            
            if (pairs.empty()) {
                
                // The argument is not an enum. Register a single setter
                root.add({
                    
                    .tokens = { cmd, "set", OptEnum::key(opt) },
                    .chelp  = { OptEnum::help(opt) },
                    .args   = {
                        { .name = { "value", OptionParser::argList(opt) } }
                    },
                        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {
                        
                            emulator.set(Opt(values[0]), args.at("value"), { values[1] });
                            msgQueue.put(Msg::CONFIG);
                        
                        }, .payload = { isize(opt), c.objid }
                });
                
            } else {
                
                // Register a setter for every enum
                root.add({
                    
                    .tokens = { cmd, "set", OptEnum::key(opt) },
                    .chelp  = { OptEnum::help(opt) }
                });
                
                for (const auto& [first, second] : pairs) {
                    
                    auto help = OptionParser::help(opt, second);
                    root.add({
                        
                        .tokens = { cmd, "set", OptEnum::key(opt), first },
                        .chelp  = { help.empty() ? "Set to " + first : help },
                        
                        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {
                            
                            emulator.set(Opt(values[0]), values[1], { values[2] });
                            msgQueue.put(Msg::CONFIG);
                            
                        },  .payload = { isize(opt), isize(second), c.objid }
                    });
                }
            }
        }
    }
    
    return cmd;
}

}
