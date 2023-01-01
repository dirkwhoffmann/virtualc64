// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "Headless.h"
#include "Script.h"

#ifndef _WIN32
#include <getopt.h>
#endif

int main(int argc, char *argv[])
{
    try {

        vc64::Headless().main(argc, argv);

    } catch (vc64::SyntaxError &e) {

        std::cout << "Usage: ";
        std::cout << "VirtualC64Core [-vm] <script>" << std::endl;
        std::cout << std::endl;
        std::cout << "       -v or --verbose   Print executed script lines" << std::endl;
        std::cout << "       -m or --messages  Observe the message queue" << std::endl;
        std::cout << std::endl;

        if (auto what = string(e.what()); !what.empty()) {
            std::cout << what << std::endl;
        }

        return 1;

    } catch (vc64::VC64Error &e) {

        std::cout << "VC64Error: " << std::endl;
        std::cout << e.what() << std::endl;
        return 1;

    } catch (std::exception &e) {

        std::cout << "Error: " << std::endl;
        std::cout << e.what() << std::endl;
        return 1;

    } catch (...) {

        std::cout << "Error" << std::endl;
    }

    return 0;
}

namespace vc64 {

void
Headless::main(int argc, char *argv[])
{
    std::cout << "VirtualC64 Headless v" << c64.version();
    std::cout << " - (C)opyright Dirk W. Hoffmann" << std::endl << std::endl;

    // Parse all command line arguments
    parseArguments(argc, argv);

    // Redirect shell output to the console in verbose mode
    if (keys.find("verbose") != keys.end()) c64.retroShell.setStream(std::cout);

    // Read the input script
    Script script(keys["arg1"]);

    // Register message receiver
    c64.msgQueue.setListener(this, vc64::process);

    // Launch the emulator thread
    c64.launch();

    // Execute the script
    barrier.lock();
    script.execute(c64);

    while (!halt) {

        barrier.lock();
        c64.retroShell.continueScript();
    }
}

#ifdef _WIN32

void
Headless::parseArguments(int argc, char *argv[])
{
    keys["arg1"] = util::makeAbsolutePath("default.ini");
}

#else

void
Headless::parseArguments(int argc, char *argv[])
{
    static struct option long_options[] = {

        { "verbose",    no_argument,    NULL,   'v' },
        { "messages",   no_argument,    NULL,   'm' },
        { NULL,         0,              NULL,    0  }
    };

    // Don't print the default error messages
    opterr = 0;

    // Remember the execution path
    keys["exec"] = util::makeAbsolutePath(argv[0]);

    // Parse all options
    while (1) {

        int arg = getopt_long(argc, argv, ":vm", long_options, NULL);
        if (arg == -1) break;

        switch (arg) {

            case 'v':
                keys["verbose"] = "1";
                break;

            case 'm':
                keys["messages"] = "1";
                break;

            case ':':
                throw SyntaxError("Missing argument for option '" +
                                  string(argv[optind - 1]) + "'");

            default:
                throw SyntaxError("Invalid option '" +
                                  string(argv[optind - 1]) + "'");
        }
    }

    // Parse all remaining arguments
    auto nr = 1;
    while (optind < argc) {
        keys["arg" + std::to_string(nr++)] = util::makeAbsolutePath(argv[optind++]);
    }

    checkArguments();
}

#endif

void
Headless::checkArguments()
{
    // The user needs to specify a single input file
    if (keys.find("arg1") == keys.end()) {
        throw SyntaxError("No script file is given");
    }
    if (keys.find("arg2") != keys.end()) {
        throw SyntaxError("More than one script file is given");
    }

    // The input file must exist
    if (!util::fileExists(keys["arg1"])) {
        throw SyntaxError("File " + keys["arg1"] + " does not exist");
    }
}

void
process(const void *listener, long type, i32 d1, i32 d2, i32 d3, i32 d4)
{
    ((Headless *)listener)->process(type, d1, d2, d3, d4);
}

void
Headless::process(long type, i32 d1, i32 d2, i32 d3, i32 d4)
{
    static bool messages = keys.find("messages") != keys.end();

    if (messages) {

        std::cout << MsgTypeEnum::key(type);
        std::cout << "(" << d1 << ", " << d2 << ", " << d3 << ", " << d4 << ")";
        std::cout << std::endl;
    }

    switch (type) {

        case MSG_SCRIPT_DONE:
        case MSG_SCRIPT_ABORT:
        case MSG_ABORT:

            halt = true;
            [[fallthrough]];

        case MSG_SCRIPT_WAKEUP:

            barrier.unlock();
            break;

        default:
            break;
    }
}

}

