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

#include "vcconfig.h"
#include "rvdebug.h"
#include "Console.h"
#include "RSError.h"
#include "Emulator.h"
#include "Option.h"
#include "utl/io/Parser.h"
#include <istream>
#include <sstream>
#include <string>

namespace vc64 {

HistoryBuffer Console::historyBuffer;

void
Console::willExecute(const InputLine &input)
{
    // Echo the command if it came from somewhere else
    if (!input.isUserCommand()) { *this << input.input; } //  << '\n'; }
}

void
Console::didExecute(const InputLine& input, std::stringstream &ss)
{
    if (ss.peek() != EOF) {
        *this << vdelim() << ss.str() << vdelim();
    } else {
        *this << vdelim();
    }
}

void
Console::didExecute(const InputLine& input, std::stringstream &ss, std::exception &exc)
{
    describe(ss, exc, input.id, input.input);

    if (ss.peek() != EOF) {
        *this << vdelim() << ss.str() << vdelim();
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
    initCommands();

    // Initialize the text storage
    clear();

    // Register as delegate to receive command output
    delegates.push_back(this);
}

void
Console::_pause()
{
    if (commandSet == CommandSet::Debugger) debuggerPause();
}

void
Console::didActivate()
{
    if (!welcomed) {

        *this << "RetroShell " << C64::build() << '\n';
        *this << '\n';
        *this << "Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de" << '\n';
        *this << "https://github.com/dirkwhoffmann/virtualc64" << '\n';
        *this << '\n';

        welcomed = true;
    }

    switch (commandSet) {

        case CommandSet::Debugger:

            emulator.trackOn(trackSource());
            break;

        default:
            break;
    }
}

void
Console::didDeactivate()
{
    switch (commandSet) {

        case CommandSet::Debugger:

            emulator.trackOff(trackSource());
            break;

        default:
            break;
    }
}

string
Console::prompt()
{
    switch (commandSet) {

        case CommandSet::Commander:     return commanderPrompt();
        case CommandSet::Debugger:      return debuggerPrompt();
    }
    return "";
}

void
Console::setCommandSet(CommandSet cs)
{
    CommandSetEnum::validate(cs);

    // Leave the old command set
    didDeactivate();

    // Replace the command tree
    commandSet = cs;
    initCommands();

    // Enter the new command set
    didActivate();
}

void
Console::initCommands()
{
    // Wipe out the old command tree
    root = RSCommand();
    RSCommand::currentGroup = "";

    // Register the commands that are available in all command sets
    initCommonCommands(root);

    // Register the commands of the selected command set
    switch (commandSet) {

        case CommandSet::Commander:     initCommanderCommands(root); break;
        case CommandSet::Debugger:      initDebuggerCommands(root); break;
    }
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
    *this << utl::concat(vec);
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
    shell.isDirty = true;
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

string
Console::vdelim() const
{
    return LOG_RSH != LOG_OFF ? "[DEBUG]\n" : "\n";
    // return RSH_DEBUG ? "[DEBUG]\n" : "\n";
}

void
Console::press(RSKey key, bool shift)
{
    assert(RSKeyEnum::isValid(key));
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
                shell.asyncExec("help \"" + input + "\" TAB=" + std::to_string(tabPressed));

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
Console::cursorRel() const
{
    assert(cursor >= 0 && cursor <= (isize)input.length());
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
    shell.asyncExec(input);

    // Clear the input line
    input = "";
    cursor = 0;
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
    string result = utl::concat(tokens);

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
    if (!cmd.callback) { throw RSError(RSError::TOO_FEW_ARGUMENTS, cmd.fullName); }

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
                throw RSError(RSError::MISSING_FLAG, keyStr);
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
                throw RSError(RSError::MISSING_KEY_VALUE, descr.keyValueStr());
            }
            continue;
        }

        // Does the descriptor describe a standard argument?
        if (descr.isStdArg()) {

            if (!std.empty()) {

                map[nameStr] = std.front();
                std.erase(std.begin());

            } else if (descr.isRequired()) {

                throw RSError(RSError::TOO_FEW_ARGUMENTS, cmd.fullName);
            }
            continue;
        }

        fatalError;
    }

    // Print some debug information
    for (auto &it : map) logmsg(LOG_RSH, "arg['%s']='%s'\n", it.first.c_str(), it.second.c_str());

    // Check for invalid or extra arguments
    if (!flags.empty()) { throw RSError(RSError::UNKNOWN_FLAG, flags.front()); }
    if (!keyVal.empty()) { throw RSError(RSError::UNKNOWN_KEY_VALUE, keyVal.front()); }
    if (!std.empty()) { throw RSError(RSError::TOO_MANY_ARGUMENTS, cmd.fullName); }

    return map;
}

bool
Console::isBool(const string &argv) const
{
    return utl::isBool(argv);
}

bool
Console::isOnOff(const string  &argv) const
{
    return utl::isOnOff(argv);
}

long
Console::isNum(const string &argv) const
{
    return utl::isNum(argv);
}

bool
Console::parseBool(const string &argv) const
{
    return utl::parseBool(argv);
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
    return utl::parseOnOff(argv);
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
    return utl::parseNum(argv);
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
    return utl::parseSeq(argv);
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
    if (cmd.isScriptCommand() && cmd.input.empty()) { *this << '\n'; return; }

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
        if (c == &root) throw RSError(RSError::SYNTAX_ERROR, tokens[0]);

        // Parse arguments
        Arguments parsedArgs = parse(*c, args);

        // Call the command handler
        c->callback(ss, parsedArgs, c->payload);

        // Dispatch output
        for (auto &delegate: delegates) delegate->didExecute(cmd, ss);

    } catch (ScriptInterruption &) {

        for (auto &delegate: delegates) delegate->didExecute(cmd, ss);
        throw;

    } catch (std::exception &err) {

        for (auto &delegate: delegates) delegate->didExecute(cmd, ss, err);
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
    *this << vdelim() << ss.str() << vdelim();
}

void
Console::describe(std::ostream &ss, const std::exception &e, isize line, const string &cmd)
{
    if (line) {
        ss << "Line " << line << ": " << cmd << '\n';
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
Console::initCommonCommands(RSCommand &root)
{
    //
    // Common commands
    //

    {   RSCommand::currentGroup = "Shell commands";

        root.add({

            .tokens = { "commander" },
            .chelp  = { "Enter the command console" },

            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                shell.enterCommander();

                os << "RetroShell Commander" << " " << C64::version() << "\n\n";
                os << string(4, ' ') << "Type 'help' or press 'Tab' twice for help.\n";
                os << string(4, ' ') << "Press 'Shift+Tab' to switch consoles.\n";
            }
        });

        root.add({

            .tokens = { "debugger" },
            .chelp  = { "Enter the debug console" },

            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                shell.enterDebugger();
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
                if (!stream.is_open()) throw IOError(IOError::FILE_NOT_FOUND, path);
                shell.asyncExecScript(stream);
            }
        });

        root.add({

            .tokens = { "wait" },
            .chelp  = { "Pause the execution of a command script" },
            .flags  = rs::hidden,
            .args   = { { .name = { "seconds", "Delay" } } },

            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                auto seconds = parseNum(args.at("seconds"));
                shell.scheduleWakeup(C64::sec(seconds));
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

                dump(os, c, Category::Config);
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

string
Console::commanderPrompt()
{
    return "C64% ";
}

void
Console::initCommanderCommands(RSCommand &root)
{
    //
    // Empty command
    //

    root.add({

        .tokens = { "return" },
        .chelp  = { "Print status information" },
        .flags  = rs::hidden,
        .func   = [] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            // os << "\nRetroShell Commander\n";
        }
    });


    //
    // Workspace management
    //

    root.add({

        .tokens = { "workspace" },
        .ghelp  = { "Workspace management" },
        .flags  = releaseBuild ? rs::hidden : 0
    });

    root.add({

        .tokens = { "workspace init" },
        .chelp  = { "First command of a workspace script" },
        .flags  = releaseBuild ? rs::hidden : 0,
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            c64.initWorkspace();
        }
    });

    root.add({

        .tokens = { "workspace activate" },
        .chelp  = { "Last command of a workspace script" },
        .flags  = releaseBuild ? rs::hidden : 0,
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            c64.activateWorkspace();
        }
    });


    //
    // Regression tester
    //

    RSCommand::currentGroup = "Regression testing";

    auto cmd = registerComponent(regressionTester, releaseBuild ? rs::hidden : 0);

    root.add({

        .tokens = { "regression", "setup" },
        .ghelp  = { "Initializes the test environment" },
    });

    for (auto &it : ConfigSchemeEnum::elements()) {

        root.add({

            .tokens = { "regression", "setup", ConfigSchemeEnum::key(it) },
            .chelp  = { ConfigSchemeEnum::help(it) },
            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                regressionTester.prepare(c64, ConfigScheme(values[0]));

                // Pause the script to give the C64 some time to boot
                shell.scheduleWakeup(3 * vic.getFrequency());
                throw ScriptInterruption();

            }, .payload = { isize(it) }
        });
    }

    root.add({

        .tokens = { "regression", "run" },
        .chelp  = { "Launches a regression test" },
        .args   = { { .name = { "path", "Regression test script" } } },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            auto path = host.makeAbsolute(args.at("path"));
            regressionTester.run(path);
        }
    });

    root.add({

        .tokens = { "screenshot" },
        .ghelp  = { "Manages screenshots" },
        .flags  = releaseBuild ? rs::hidden : 0
    });

    root.add({

        .tokens = { "screenshot", "set" },
        .ghelp  = { "Configure the screenshot" }
    });

    root.add({

        .tokens = { "screenshot", "set", "filename" },
        .chelp  = { "Assign the screenshot filename" },
        .args   = { { .name = { "path", "File path" } } },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            auto path = host.makeAbsolute(args.at("path"));
            regressionTester.screenshotPath = path;
        }
    });

    root.add({

        .tokens = { "screenshot", "set", "cutout" },
        .chelp  = { "Adjust the texture cutout" },
        .args   = {
            { .name = { "x1", "Left x coordinate" }, .flags = rs::keyval },
            { .name = { "x2", "Right x coordinate" }, .flags = rs::keyval },
            { .name = { "y1", "Lower y coordinate" }, .flags = rs::keyval },
            { .name = { "y2", "Upper y coordinate" }, .flags = rs::keyval }
        },
            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                isize x1 = parseNum(args.at("x1"));
                isize y1 = parseNum(args.at("y1"));
                isize x2 = parseNum(args.at("x2"));
                isize y2 = parseNum(args.at("y2"));

                regressionTester.x1 = x1;
                regressionTester.y1 = y1;
                regressionTester.x2 = x2;
                regressionTester.y2 = y2;
            }
    });

    root.add({

        .tokens = { "screenshot", "save" },
        .chelp  = { "Saves a screenshot and exits the emulator" },
        .args   = { { .name = { "path", "File path" } } },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            auto path = host.makeAbsolute(args.at("path"));
            regressionTester.dumpTexture(c64, path);
        }
    });


    //
    // Components
    //

    RSCommand::currentGroup = "Components";

    //
    // Components (C64)
    //

    cmd = registerComponent(c64);

    root.add({

        .tokens = { cmd, "defaults" },
        .chelp  = { "Displays the user defaults storage" },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            dump(os, emulator, Category::Defaults);
        }
    });

    root.add({

        .tokens = { cmd, "power" },
        .chelp  = { "Switches the C64 on or off" },
        .args   = { { .name = { "onoff", "Power switch state" }, .key = "{ on | off }" } },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            parseOnOff(args.at("onoff")) ? emulator.run() : emulator.powerOff();
        }
    });

    root.add({

        .tokens = { cmd, "reset" },
        .chelp  = { "Performs a hard reset" },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            c64.hardReset();
        }
    });

    root.add({

        .tokens = { cmd, "init" },
        .ghelp  = { "Initializes the C64 with a predefined scheme" },
    });

    for (auto &it : ConfigSchemeEnum::elements()) {

        root.add({

            .tokens = { cmd, "init", ConfigSchemeEnum::key(it) },
            .chelp  = { ConfigSchemeEnum::help(it) },
            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                emulator.set(ConfigScheme(values[0]));
            }, .payload = { isize(it) }
        });
    }


    //
    // Components (Memory)
    //

    cmd = registerComponent(mem);

    root.add({

        .tokens = { cmd, "flash" },
        .chelp  = { "Flashes a file into memory" },
        .args   = { { .name = { "path", "File path" } } },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            auto path = host.makeAbsolute(args.at("path"));
            if (!utl::fileExists(path)) throw IOError(IOError::FILE_NOT_FOUND, path);

            // auto file = PRGFile(path);
            c64.flash(path, 0);
        }
    });

    root.add({

        .tokens = { cmd, "load" },
        .ghelp  = { "Loads memory contents from a file" }
    });

    root.add({

        .tokens = { cmd, "load", "rom" },
        .chelp  = { "Installs a ROM image" },
        .args   = { { .name = { "path", "File path" } } },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            auto path = host.makeAbsolute(args.at("path"));
            c64.loadRom(path);
        }
    });

    root.add({

        .tokens = { cmd, "load", "bin" },
        .chelp  = { "Loads a chunk of memory" },
        .args   = {
            { .name = { "path", "File path" } },
            { .name = { "address", "Target memory address" } },
        },
            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                auto path = host.makeAbsolute(args.at("path"));
                mem.debugger.load(path, parseAddr(args.at("address")));
            }
    });

    root.add({

        .tokens = { cmd, "load", "openroms" },
        .chelp  = { "Installs the MEGA65 OpenROMs" },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {
            c64.installOpenRoms();
        }
    });

    root.add({

        .tokens = { cmd, "save" },
        .chelp  = { "Save memory contents to a file" }
    });

    root.add({

        .tokens = { cmd, "save", "bin" },
        .chelp  = { "Saves a chunk of memory" },
        .args   = {
            { .name = { "path", "File path" } },
            { .name = { "address", "Memory address" } },
            { .name = { "count", "Number of bytes" } },
        },
            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                fs::path path(args.at("path"));
                mem.debugger.save(path, parseAddr(args.at("address")), parseNum(args.at("count")));
            }
    });


    //
    // Components (CPU)
    //

    cmd = registerComponent(cpu);


    //
    // Components (CIA)
    //

    cmd = registerComponent(cia1);
    cmd = registerComponent(cia2);


    //
    // Components (VICII)
    //

    cmd = registerComponent(vic);


    //
    // Components (SID)
    //

    cmd = registerComponent(sid0);
    cmd = registerComponent(sid1);
    cmd = registerComponent(sid2);
    cmd = registerComponent(sid3);


    //
    // Ports
    //

    RSCommand::currentGroup = "Ports";


    //
    // Ports (Power port)
    //

    cmd = registerComponent(powerSupply);


    //
    // Ports (Audio port)
    //

    cmd = registerComponent(audioPort);


    //
    // Ports (Video port)
    //

    cmd = registerComponent(videoPort);


    //
    // Ports (Expansion port)
    //

    cmd = registerComponent(expansionPort);

    /*
     root.add({

     .tokens = { cmd, "attach" },
     .chelp  = { "Attaches a cartridge" }
     });
     */

    root.add({

        .tokens = { cmd, "attach" },
        .ghelp  = { "Attaches a cartridge" },
        .chelp  = { "Attaches a cartridge from a CRT file" },
        .args   = { { .name = { "path", "File path" } } },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            auto path = host.makeAbsolute(args.at("path"));
            expansionPort.attachCartridge(path);
        }
    });

    root.add({

        .tokens = { cmd, "attach", "reu" },
        .chelp  = { "Attaches a REU expansion cartridge" },
        .args   = { { .name = { "KB", "REU size" } } },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            expansionPort.attachReu(parseNum(args.at("KB")));
        }
    });

    root.add({

        .tokens = { cmd, "attach", "georam" },
        .chelp  = { "Attaches a GeoRAM expansion cartridge" },
        .args   = { { .name = { "KB", "REU size" } } },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            expansionPort.attachGeoRam(parseNum(args.at("KB")));
        }
    });

    root.add({

        .tokens = { cmd, "detach" },
        .chelp  = { "Detaches the current cartridge" },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            expansionPort.detachCartridge();
        }
    });


    //
    // Ports (User port)
    //

    cmd = registerComponent(userPort);


    //
    // Peripherals
    //

    RSCommand::currentGroup = "Peripherals";


    //
    // Peripherals (Monitor)
    //

    registerComponent(monitor);


    //
    // Peripherals (Keyboard)
    //

    cmd = registerComponent(keyboard);

    root.add({

        .tokens = { cmd, "press" },
        .chelp  = { "Presses a key" },
        .args   = { { .name = { "keycode", "Numerical code of the C64 key" } } },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            keyboard.press(C64Key(parseNum(args.at("keycode"))));
        }
    });

    root.add({

        .tokens = { cmd, "release" },
        .chelp  = { "Releases a key" },
        .args   = { { .name = { "keycode", "Numerical code of the C64 key" } } },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            keyboard.release(C64Key(parseNum(args.at("keycode"))));
        }
    });

    root.add({

        .tokens = { cmd, "type" },
        .chelp  = { "Types text on the keyboard" }
    });

    root.add({

        .tokens = { cmd, "type", "text" },
        .chelp  = { "Types text on the keyboard" },
        .args   = { { .name = { "text", "Text to type" } } },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            keyboard.autoType(args.at("text"));
        }
    });

    root.add({

        .tokens = { cmd, "type", "load" },
        .chelp  = { "Types \"LOAD\"*\",8,1" },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            keyboard.autoType("load \"*\",8,1\n");
        }
    });

    root.add({

        .tokens = { cmd, "type", "run" },
        .chelp  = { "Types RUN" },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            keyboard.autoType("run\n");
        }
    });


    //
    // Peripherals (Drives)
    //

    root.add({

        .tokens = { "drive[n]" },
        .ghelp  = { "Drive n" },
        .chelp  = { "Commands: drive8, drive9" }
    });

    for (isize i = 0; i < 2; i++) {

        if (i == 0) cmd = registerComponent(drive8, rs::shadowed);
        if (i == 1) cmd = registerComponent(drive9, rs::shadowed);

        root.add({

            .tokens = { cmd, "eject" },
            .chelp  = { "Ejects a floppy disk" },
            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                auto &drive = values.front() ? drive9 : drive8;
                drive.ejectDisk();

            }, .payload = {i}
        });

        root.add({

            .tokens = { cmd, "insert" },
            .chelp  = { "Inserts a floppy disk" },
            .args   = {{ .name = { "path", "File path" } } },
            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                auto path = host.makeAbsolute(args.at("path"));
                if (!utl::fileExists(path))
                    throw IOError(IOError::FILE_NOT_FOUND, path);

                auto &drive = values.front() ? drive9 : drive8;
                drive.insertDisk(path, false);

            }, .payload = {i}
        });

        root.add({

            .tokens = { cmd, "newdisk" },
            .chelp  = { "Inserts a new blank disk" },
            .args   = { { .name = { "dos", "DOS type" } } },
            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                auto &drive = values.front() ? drive9 : drive8;
                auto type = utl::parseEnum <FSFormat, FSFormatEnum> (args.at("dos"));
                drive.insertNewDisk(type, "NEW DISK");

            }, .payload = {i}
        });

        root.add({

            .tokens = { cmd, "protect" },
            .chelp  = { "Enables write protection" },
            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                auto &drive = values.front() ? drive9 : drive8;
                drive.protectDisk();

            }, .payload = {i}
        });

        root.add({

            .tokens = { cmd, "unprotect" },
            .chelp  = { "Disables write protection" },
            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                auto &drive = values.front() ? drive9 : drive8;
                drive.unprotectDisk();

            }, .payload = {i}
        });
    }


    //
    // Peripherals (Datasette)
    //

    cmd = registerComponent(datasette);

    root.add({

        .tokens = { cmd, "insert" },
        .chelp  = { "Inserts a tape from a TAP file" },
        .args   = { { .name = { "path", "File path" } } },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            auto path = host.makeAbsolute(args.at("path"));
            datasette.insertTape(path);
        }
    });

    root.add({

        .tokens = { cmd, "rewind" },
        .chelp  = { "Rewinds the tape" },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            datasette.rewind();
        }
    });

    root.add({

        .tokens = { cmd, "rewind", "to" },
        .chelp  = { "Rewinds the tape to a specific position" },
        .args   = { { .name = { "pos", "Tape position" } } },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            datasette.rewind(parseNum(args.at("pos")));
        }
    });

    root.add({

        .tokens = { cmd, "eject" },
        .chelp  = { "Eject the tape" },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            datasette.ejectTape();
        }
    });


    //
    // Peripherals (Mouse)
    //

    root.add({

        .tokens = { "mouse[n]" },
        .ghelp  = { "Mouse n" },
        .chelp  = { "Commands: mouse1, mouse2" }
    });

    registerComponent(c64.port1.mouse, rs::shadowed);
    registerComponent(c64.port2.mouse, rs::shadowed);


    //
    // Peripherals (Joystick)
    //

    root.add({

        .tokens = { "joystick[n]" },
        .ghelp  = { "Joystick n" },
        .chelp  = { "Commands: joystick1, joystick2" }
    });

    for (isize i = 0; i <= 1; i++) {

        if (i == 0) cmd = registerComponent(port1.joystick, rs::shadowed);
        if (i == 1) cmd = registerComponent(port2.joystick, rs::shadowed);

        root.add({

            .tokens = { cmd, "press" },
            .chelp  = { "Presses a joystick button" },
            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                auto &port = (values[0] == 0) ? c64.port1 : c64.port2;
                port.joystick.trigger(GamePadAction::PRESS_FIRE);

            }, .payload = {i}
        });

        root.add({

            .tokens = { cmd, "unpress" },
            .chelp  = { "Releases a joystick button" },
            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                auto &port = (values[0] == 0) ? c64.port1 : c64.port2;
                port.joystick.trigger(GamePadAction::RELEASE_FIRE);

            }, .payload = {i}
        });

        root.add({

            .tokens = { cmd, "pull" },
            .ghelp  = { "Pulls the joystick" }
        });

        root.add({

            .tokens = { cmd, "pull", "left" },
            .chelp  = { "Pulls the joystick left" },
            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                auto &port = (values.front() == PORT_1) ? c64.port1 : c64.port2;
                port.joystick.trigger(GamePadAction::PULL_LEFT);

            }, .payload = {i}
        });

        root.add({

            .tokens = { cmd, "pull", "right" },
            .chelp  = { "Pulls the joystick right" },
            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                auto &port = (values.front() == PORT_1) ? c64.port1 : c64.port2;
                port.joystick.trigger(GamePadAction::PULL_RIGHT);

            }, .payload = {i}
        });

        root.add({

            .tokens = { cmd, "pull", "up" },
            .chelp  = { "Pulls the joystick up" },
            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                auto &port = (values.front() == PORT_1) ? c64.port1 : c64.port2;
                port.joystick.trigger(GamePadAction::PULL_UP);

            }, .payload = {i}
        });

        root.add({

            .tokens = { cmd, "pull", "down" },
            .chelp  = { "Pulls the joystick down" },
            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                auto &port = (values.front() == PORT_1) ? c64.port1 : c64.port2;
                port.joystick.trigger(GamePadAction::PULL_DOWN);

            }, .payload = {i}
        });

        root.add({

            .tokens = { cmd, "release" },
            .ghelp  = { "Release a joystick axis" }
        });

        root.add({

            .tokens = { cmd, "release", "x" },
            .chelp  = { "Releases the x-axis" },
            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                auto &port = (values.front() == PORT_1) ? c64.port1 : c64.port2;
                port.joystick.trigger(GamePadAction::RELEASE_X);

            }, .payload = {i}
        });

        root.add({

            .tokens = { cmd, "release", "y" },
            .chelp  = { "Releases the y-axis" },
            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                auto &port = (values.front() == PORT_1) ? c64.port1 : c64.port2;
                port.joystick.trigger(GamePadAction::RELEASE_Y);

            }, .payload = {i}
        });
    }


    //
    // Peripherals (Paddles)
    //

    root.add({

        .tokens = { "paddle[n]" },
        .ghelp  = { "Paddle n" },
        .chelp  = { "Commands: paddle1, paddle2" }
    });

    cmd = registerComponent(port1.paddle, rs::shadowed);
    cmd = registerComponent(port2.paddle, rs::shadowed);


    //
    // Peripherals (RS232)
    //

    cmd = registerComponent(userPort.rs232);

    root.add({

        .tokens = { cmd, "send" },
        .chelp  = { "Feeds text into the RS232 adapter" },
        .args   = { { .name = { "text", "Text message" } } },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            userPort.rs232 << args.at("text");
        }
    });


    //
    // Miscellaneous
    //

    RSCommand::currentGroup = "Miscellaneous";

    //
    // Miscellaneous (Host)
    //

    cmd = registerComponent(host);

    root.add({

        .tokens = { cmd, "searchpath" },
        .chelp  = { "Sets the search path for media files" },
        .args   = { { .name = { "path", "File path" } } },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            auto path = fs::path(args.at("path"));
            host.setSearchPath(path);
        }
    });


    //
    // Miscellaneous (Config)
    //

    root.add({

        .tokens = { "config" },
        .ghelp  = { "Virtual machine configuration" },
        .chelp  = { "Displays the current configuration" },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            c64.exportConfig(os);
        }
    });

    root.add({

        .tokens = { "config", "diff" },
        .chelp  = { "Displays the difference to the default configuration" },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            c64.exportDiff(os);
        }
    });


    //
    // Components (DMA Debugger)
    //

    cmd = registerComponent(vic.dmaDebugger);


    //
    // Miscellaneous (Remote server)
    //

    root.add({

        .tokens = { "server" },
        .ghelp  = { "Remote connections" },
        .chelp  = { "Displays a server status summary" },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            dump(os, remoteManager, Category::State);
        }
    });

    cmd = registerComponent(remoteManager.rshServer);
    cmd = registerComponent(remoteManager.rpcServer);
    cmd = registerComponent(remoteManager.dapServer);
    cmd = registerComponent(remoteManager.promServer);
}

void
Console::debuggerPause()
{
    *this << '\n';
    exec(InputLine {.input = "state"});
    *this << prompt();
}

string
Console::debuggerPrompt()
{
    std::stringstream ss;

    ss << "(";
    ss << std::right << std::setw(0) << std::dec << isize(c64.scanline);
    ss << ",";
    ss << std::right << std::setw(0) << std::dec << isize(c64.rasterCycle);
    ss << ") $";
    ss << std::right << std::setw(4) << std::hex << isize(cpu.getPC0());
    ss << ": ";

    return ss.str();
}

void
Console::initDebuggerCommands(RSCommand &root)
{
    //
    // Empty command
    //

    root.add({

        .tokens = { "return" },
        .chelp  = { "Print status information" },
        .flags  = rs::hidden,
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            if (emulator.isPaused()) {
                emulator.stepInto();
            } else {
                os << std::endl;
                c64.dump(Category::Current, os);
            }
        }
    });


    //
    // Program execution
    //

    RSCommand::currentGroup = "Program execution";

    root.add({ .tokens = { "p[ause]" }, .ghelp  = { "Pause emulation" }, .chelp  = { "p or pause" } });

    root.add({

        .tokens = { "pause" },
        .chelp  = { "Pause emulation" },
        .flags  = rs::shadowed,
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            if (emulator.isRunning()) emulator.put(Cmd::PAUSE);
        }
    });

    root.clone({ "pause" }, "p");

    root.add({ .tokens = { "g[oto]" }, .ghelp  = { "Goto address" }, .chelp  = { "g or goto" } });
    root.add({

        .tokens = { "goto" },
        .chelp  = { "Goto address" },
        .flags  = rs::shadowed,
        .args   = { { .name = { "address", "Memory address" }, .flags = rs::opt } },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            args.contains("address") ? cpu.jump(parseAddr(args.at("address"))) : emulator.run();
        }
    });
    root.clone({ "goto" }, "g");

    root.add({ .tokens = { "s[tep]" }, .ghelp  = { "Step into the next instruction" }, .chelp  = { "s or step" } });
    root.add({

        .tokens = { "step" },
        .chelp  = { "Step into the next instruction" },
        .flags  = rs::shadowed,
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            emulator.stepInto();
        }
    });
    root.clone({ "step" }, "s");

    root.add({ .tokens = { "n[next]" }, .ghelp  = { "Step over the next instruction" }, .chelp  = { "n or next" } });
    root.add({

        .tokens = { "next" },
        .chelp  = { "Step over the next instruction" },
        .flags  = rs::shadowed,
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            emulator.stepOver();
        }
    });
    root.clone({ "next" }, "n");

    root.add({

        .tokens = { "eol" },
        .chelp  = { "Complete the current line" },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            emulator.finishLine();
        }
    });

    root.add({

        .tokens = { "eof" },
        .chelp  = { "Complete the current frame" },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            emulator.finishFrame();
        }
    });


    //
    // Breakpoints
    //

    root.add({

        .tokens = { "break" },
        .ghelp  = { "Manage CPU breakpoints" },
        .chelp  = { "List all breakpoints" },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            dump(os, cpu, Category::Breakpoints);
        }
    });

    root.add({

        .tokens = { "break", "at" },
        .chelp  = { "Set a breakpoint" },
        .args   = {
            { .name = { "address", "Memory address" } },
            { .name = { "ignores", "Ignore count" }, .flags = rs::opt }
        },
            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                auto addr = parseAddr(args.at("address"));
                cpu.setBreakpoint(addr, parseNum(args, "ignores", 0));
            }
    });

    root.add({

        .tokens = { "break", "delete" },
        .chelp  = { "Delete breakpoints" },
        .args   = { { .name = { "nr", "Breakpoint number" } } },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            cpu.deleteBreakpoint(parseNum(args.at("nr")));
        }
    });

    root.add({

        .tokens = { "break", "toggle" },
        .chelp  = { "Enable or disable breakpoints" },
        .args   = { { .name = { "nr", "Breakpoint number" } } },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            cpu.toggleBreakpoint(parseNum(args.at("nr")));
        }
    });


    //
    // Watchpoints
    //

    root.add({

        .tokens = { "watch" },
        .ghelp  = { "Manage CPU watchpoints" },
        .chelp  = { "Lists all watchpoints" },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            dump(os, cpu, Category::Watchpoints);
        }
    });

    root.add({

        .tokens = { "watch", "at" },
        .chelp  = { "Set a watchpoint at the specified address" },
        .args   = {
            { .name = { "address", "Memory address" } },
            { .name = { "ignores", "Ignore count" }, .flags = rs::opt }
        },
            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                auto addr = parseAddr(args, "address");
                cpu.setWatchpoint(addr, parseNum(args, "ignores", 0));
            }
    });

    root.add({

        .tokens = { "watch", "delete" },
        .chelp  = { "Delete a watchpoint" },
        .args   = { { .name = { "nr", "Watchpoint number" } } },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            cpu.deleteWatchpoint(parseNum(args, "nr"));
        }
    });

    root.add({

        .tokens = { "watch", "toggle" },
        .chelp  = { "Enable or disable a watchpoint" },
        .args   = { { .name = { "nr", "Watchpoint number" } } },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            cpu.toggleWatchpoint(parseNum(args, "nr"));
        }
    });


    //
    // Monitoring
    //

    RSCommand::currentGroup = "Monitoring";

    root.add({

        .tokens = { "d" },
        .chelp  = { "Disassemble instructions" },
        .args   = { { .name = { "address", "Memory address" }, .flags = rs::opt } },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            cpu.disassembler.disassembleRange(os, parseAddr(args, "address", cpu.getPC0()), 16);
        }
    });

    root.add({

        .tokens = { "a" },
        .chelp  = { "Dump memory in ASCII" },
        .args   = { { .name = { "address", "Memory address" }, .flags = rs::opt } },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            if (args.contains("address")) { current = parseAddr(args, "address"); }

            current += (u16)mem.debugger.ascDump(os, current, 16);
        }
    });

    root.add({

        .tokens = { "m" },
        .chelp  = { "Dump memory" },
        .args   = { { .name = { "address", "Memory address" }, .flags = rs::opt } },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            if (args.contains("address")) { current = parseAddr(args, "address"); }

            current += (u16)mem.debugger.memDump(os, current, 16);
        }
    });

    root.add({

        .tokens = { "w" },
        .chelp  = { "Write into memory" },
        .args   = {
            { .name = { "value", "Payload" } },
            { .name = { "target", "Memory address" }, .flags = rs::opt } },

            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                auto addr = current;

                if (args.contains("target")) {
                    addr = parseAddr(args, "target");
                }

                mem.debugger.write(addr, u8(parseNum(args.at("value"))));
                current = addr + 1;
            }
    });

    root.add({

        .tokens = { "c" },
        .chelp  = { "Copy a chunk of memory" },
        .args   = {
            { .name = { "src", "Source address" }, .flags = rs::keyval },
            { .name = { "dest", "Destination address" }, .flags = rs::keyval },
            { .name = { "count", "Number of bytes" }, .flags = rs::keyval } },

            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                auto src = parseNum(args.at("src"));
                auto dst = parseNum(args.at("dest"));
                auto cnt = parseNum(args.at("count"));

                if (src < dst) {

                    for (isize i = cnt - 1; i >= 0; i--)
                        mem.poke(u16(dst + i), mem.spypeek(u16(src + i)));

                } else {

                    for (isize i = 0; i <= cnt - 1; i++)
                        mem.poke(u16(dst + i), mem.spypeek(u16(src + i)));
                }
            }
    });

    root.add({

        .tokens = { "f" },
        .chelp  = { "Find a sequence in memory" },
        .args   = {
            { .name = { "sequence", "Search string" } },
            { .name = { "address", "Start address" }, .flags = rs::opt } },

            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                auto pattern = parseSeq(args.at("sequence"));
                auto addr = u16(parseNum(args, "address", current));
                auto found = mem.debugger.memSearch(pattern, addr);

                if (found >= 0) {

                    mem.debugger.memDump(os, u16(found), 1);
                    current = u16(found);

                } else {

                    os << "Not found";
                }
            }
    });

    root.add({

        .tokens = { "e" },
        .chelp  = { "Erase memory" },
        .args   = {
            { .name = { "address", "Start address" } },
            { .name = { "count", "Number of bytes to erase" } },
            { .name = { "value", "Replacement value" }, .flags = rs::opt } },

            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                auto addr = parseAddr(args.at("address"));
                auto count = parseNum(args, "count");
                auto val = u8(parseNum(args, "value", 0));

                mem.debugger.write(addr, val, count);
            }
    });

    root.add({

        .tokens = { "r" },
        .ghelp  = { "Show registers" }
    });

    root.add({

        .tokens = { "r", "cia1" },
        .chelp  = { "Complex Interface Adapter 1" },

        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            dump(os, cia1, Category::Registers);
        }
    });

    root.add({

        .tokens = { "r", "cia2" },
        .chelp  = { "Complex Interface Adapter 2" },

        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            dump(os, cia2, Category::Registers);
        }
    });

    root.add({

        .tokens = { "r", "vicii" },
        .chelp  = { "Video Interface Controller" },

        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            dump(os, vic, Category::Registers);
        }
    });

    root.add({

        .tokens = { "r", "sid" },
        .chelp  = { "Sound Interface Device" },

        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            dump(os, sid0, Category::Registers);
        }
    });


    //
    // Components
    //

    root.add({

        .tokens = { "?" },
        .ghelp  = { "Inspect a component" }
    });

    RSCommand::currentGroup = "Components";

    root.add({

        .tokens = { "?", "c64" },
        .chelp  = { "Inspects the internal state" },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            dump(os, c64, Category::State);
        }
    });

    root.add({

        .tokens = { "?", "memory" },
        .ghelp  = { "Memory" },
        .chelp  = { "Inspects the internal state" },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            dump(os, mem, Category::State);
        }
    });

    root.add({

        .tokens = { "?", "cpu" },
        .ghelp  = { "Central Processing Unit" },
        .chelp  = { "Inspect the internal state" },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            dump(os, cpu, Category::State );
        }
    });

    for (isize i = 0; i < 2; i++) {

        string cia = (i == 0) ? "cia1" : "cia2";
        root.add({

            .tokens = { "?", cia },
            .ghelp  = { "Complex Interface Adapter" },
            .chelp  = { "Inspect the internal state" },
            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                if (values[0] == 0) {
                    dump(os, cia1, Category::State );
                } else {
                    dump(os, cia1, Category::State );
                }
            }, .payload = {i}
        });

        root.add({

            .tokens = { "?", cia, "tod" },
            .chelp  = { "Display the state of the time-of-day clock" },
            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                if (values[0] == 0) {
                    dump(os, cia1.tod, Category::State );
                } else {
                    dump(os, cia2.tod, Category::State );
                }
            }, .payload = {i}
        });
    }

    root.add({

        .tokens = { "?", "vic" },
        .ghelp  = { "Video Interface Controller" },
        .chelp  = { "Inspect the internal state" },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            dump(os, vic, Category::State );
        }
    });

    root.add({

        .tokens = { "?", "sid" },
        .ghelp  = { "Sound Interface Device" },
        .chelp  = { "Inspect the internal state" },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            dump(os, sid0, Category::State );
        }
    });

    root.add({

        .tokens = { "?", "sidbridge" },
        .ghelp  = { "SID bridge" },
        .chelp  = { "Inspect the internal state" },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            dump(os, sidBridge, Category::State );
        }
    });

    root.add({

        .tokens = { "?", "expansion" },
        .ghelp  = { "Expansion port" },
        .chelp  = { "Inspect the internal state" },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            dump(os, expansionPort, Category::State );
        }
    });

    root.add({

        .tokens = { "?", "thread" },
        .ghelp  = { "Emulator thread" },
        .chelp  = { "Display information about the thread state" },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            dump(os, emulator, Category::State);
        }
    });

    root.add({

        .tokens = { "?", "thread", "runahead" },
        .chelp  = { "Run-ahead instance" },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            dump(os, emulator, Category::RunAhead);
        }
    });


    //
    // Peripherals
    //

    RSCommand::currentGroup = "Peripherals";

    root.add({

        .tokens = { "?", "keyboard" },
        .chelp  = { "Keyboard" },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            dump(os, keyboard, Category::RunAhead);
        }
    });

    root.add({

        .tokens = { "?", "port" },
        .chelp  = { "Control port" }
    });

    for (isize i = 1; i <= 2; i++) {

        string nr = (i == 1) ? "1" : "2";

        root.add({

            .tokens = { "?", "port", nr },
            .ghelp  = { "Control port " + nr },
            .chelp  = { "Inspect the internal state" },
            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                if (values[0] == 1) dump(os, port1, Category::State);
                if (values[0] == 2) dump(os, port2, Category::State);
            }, .payload = {i}
        });
    }

    root.add({

        .tokens = { "?", "mouse" },
        .ghelp  = { "Mouse" }
    });

    for (isize i = 1; i <= 2; i++) {

        string nr = (i == 1) ? "1" : "2";

        root.add({

            .tokens = { "?", "mouse", nr },
            .ghelp  = { "Mouse in port " + nr },
            .chelp  = { "Inspect the internal state" },
            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                if (values[0] == 1) dump(os, port1.mouse, Category::State );
                if (values[0] == 2) dump(os, port2.mouse, Category::State );
            }, .payload = {i}
        });
    }

    root.add({

        .tokens = { "?", "joystick" },
        .chelp  = { "Joystick" }
    });

    for (isize i = 1; i <= 2; i++) {

        string nr = (i == 1) ? "1" : "2";

        root.add({

            .tokens = { "?", "joystick", nr },
            .ghelp  = { "Joystick in port " + nr },
            .chelp  = { "Inspect the internal state" },
            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                if (values[0] == 1) dump(os, port1.joystick, Category::State);
                if (values[0] == 2) dump(os, port2.joystick, Category::State);
            }, .payload = {i}
        });
    }

    root.add({

        .tokens = { "?", "drive[n]" },
        .ghelp  = { "Floppy drive" },
        .chelp  = { "? drive8, ? drive9" }
    });

    for (isize i = 0; i < 2; i++) {

        string df = "drive" + std::to_string(i + 8);

        root.add({

            .tokens = { "?", df },
            .ghelp  = { "Floppy drive n" },
            .chelp  = { "Inspect the internal state" },
            .flags  = rs::shadowed,
            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                auto *df = drive[values[0]];
                dump(os, *df, Category::State);

            }, .payload = {i}
        });

        root.add({

            .tokens = { "?", df, "bankmap" },
            .chelp  = { "Displays the memory layout" },
            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                auto *df = drive[values[0]];
                dump(os, *df, Category::BankMap);

            }, .payload = {i}
        });

        root.add({

            .tokens = { "?", df, "disk" },
            .chelp  = { "Inspect the inserted disk" },
            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                auto *df = drive[values[0]];
                dump(os, *df, Category::Disk);

            }, .payload = {i}
        });

        root.add({

            .tokens = { "?", df, "layout" },
            .chelp  = { "Displays the disk layout" },
            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                auto *df = drive[values[0]];
                dump(os, *df, Category::Layout);

            }, .payload = {i}
        });
    }

    root.add({

        .tokens = { "?", "serial" },
        .chelp  = { "Serial Port" },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            dump(os, serialPort, Category::State);
        }
    });

    root.add({

        .tokens = { "?", "datasette" },
        .chelp  = { "Datasette" },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            dump(os, datasette, Category::State);
        }
    });

    root.add({

        .tokens = { "?", "audio" },
        .chelp  = { "Audio Port" },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            dump(os, audioPort, Category::State);
        }
    });

    root.add({

        .tokens = { "?", "host" },
        .chelp  = { "Host computer" },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            dump(os, host, Category::State);
        }
    });


    //
    // Miscellaneous
    //

    RSCommand::currentGroup = "Miscellaneous";

    root.add({

        .tokens = { "import" },
        .ghelp  = { "Import debug data" }
    });

    root.add({

        .tokens = { "import", "symbols" },
        .chelp  = { "Loads a CC65 linker debug file" },
        .args   = { { .name = { "path", "Debug file" } } },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            auto path = host.makeAbsolute(args.at("path"));
            cpu.symbolTable.loadCS65File(path);

            isize numFiles = cpu.symbolTable.files.size();
            isize numLines = cpu.symbolTable.lines.size();
            isize numSegments = cpu.symbolTable.segments.size();
            isize numSpans = cpu.symbolTable.spans.size();
            isize numSymbols = cpu.symbolTable.symbols.size();
            isize numTotal = numFiles + numFiles + numSegments + numSpans + numSymbols;

            os << "Read " << numTotal << " symbols\n\n";
            os << "     Files: " << numFiles << "\n";
            os << "     Lines: " << numLines << "\n";
            os << "  Segments: " << numSegments << "\n";
            os << "     Spans: " << numSpans << "\n";
            os << "   Symbols: " << numSymbols << "\n\n";
        }
    });

    root.add({

        .tokens = { "checksums" },
        .chelp  = { "Displays checksum of various components" },
        .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            dump(os, c64, Category::Checksums);
        }
    });

#ifndef NDEBUG

    /* Logging and debug flags can only be changed in debug builds. In
     * release builds they are compile-time constants, so the commands
     * below are not registered at all.
     *
     * VirtualC64 and rvlib maintain their own, independent flag tables.
     * Both are listed and made settable here, addressed by table index.
     */

    static const std::vector<const std::vector<utl::FlagInfo> *> logTables = {
        &vc64::logFlags, &retro::vault::logFlags
    };
    static const std::vector<const std::vector<utl::FlagInfo> *> debugTables = {
        &vc64::debugFlags, &retro::vault::debugFlags
    };

    root.add({

        .tokens = { "log" },
        .ghelp  = { "Logging flags" },
        .chelp  = { "Display all logging flags" },

        .func   = [] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            for (const auto *table : logTables) {
                for (const auto &flag : *table) {

                    os << utl::tab(flag.name);
                    os << utl::dec(flag.get()) << std::endl;
                }
            }
        }
    });

    root.add({

        .tokens = { "log", "set" },
        .ghelp  = { "Change a logging flag" }
    });

    for (isize t = 0; t < isize(logTables.size()); t++) {

        for (isize i = 0; i < isize(logTables[t]->size()); i++) {

            const auto &flag = (*logTables[t])[i];

            root.add({

                .tokens = { "log", "set", flag.name },
                .chelp  = { flag.help },
                .args   = {
                    { .name = { "level", "Severity level" } }
                },
                .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                    auto value = parseNum(args, "level");

                    if (value < 0 || value > 6)
                        throw CoreError(CoreError::OPT_INV_ARG, "0 (off) ... 6 (trace)");

                    (*logTables[values[0]])[values[1]].set(value);

                }, .payload = { t, i }
            });
        }
    }

    root.add({

        .tokens = { "debug" },
        .ghelp  = { "Debug flags" },
        .chelp  = { "Display all debug flags" },

        .func   = [] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

            for (const auto *table : debugTables) {
                for (const auto &flag : *table) {

                    os << utl::tab(flag.name);
                    if (flag.boolean) {
                        os << utl::bol(flag.get() != 0) << std::endl;
                    } else {
                        os << utl::dec(flag.get()) << std::endl;
                    }
                }
            }
        }
    });

    root.add({

        .tokens = { "debug", "set" },
        .ghelp  = { "Change a debug flag" }
    });

    for (isize t = 0; t < isize(debugTables.size()); t++) {

        for (isize i = 0; i < isize(debugTables[t]->size()); i++) {

            const auto &flag = (*debugTables[t])[i];

            if (flag.boolean) {

                root.add({

                    .tokens = { "debug", "set", flag.name },
                    .ghelp  = { flag.help }
                });

                // Register a setter for both boolean values
                for (const auto &[key, value] : std::vector<std::pair<string,isize>>
                     { { "false", 0 }, { "true", 1 } }) {

                    root.add({

                        .tokens = { "debug", "set", flag.name, key },
                        .chelp  = { value ? "Enable the flag" : "Disable the flag" },

                        .func   = [] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                            (*debugTables[values[0]])[values[1]].set(values[2]);

                        }, .payload = { t, i, value }
                    });
                }

            } else {

                // The flag holds a parameter value. Register a single setter
                root.add({

                    .tokens = { "debug", "set", flag.name },
                    .chelp  = { flag.help },
                    .args   = {
                        { .name = { "value", "Parameter value" } }
                    },
                    .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                        (*debugTables[values[0]])[values[1]].set(parseNum(args, "value"));

                    }, .payload = { t, i }
                });
            }
        }
    }

#endif

    root.add({

        .tokens = {"%"},
        .chelp  = { "Convert a value into different formats" },
        .args   = {
            { .name = { "value", "Payload" } }
        },
            .func   = [this] (std::ostream &os, const Arguments &args, const std::vector<isize> &values) {

                auto value = args.at("value");

                if (isNum(value)) {
                    mem.debugger.convertNumeric(os, (u32)parseNum(value));
                } else {
                    mem.debugger.convertNumeric(os, value);
                }
            }
    });
}

}
