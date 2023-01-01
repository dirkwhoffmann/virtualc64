// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "C64.h"
#include <map>

using std::map;
using std::vector;

namespace vc64 {

struct SyntaxError : public std::runtime_error {
    using runtime_error::runtime_error;
};

void process(const void *listener, long type, i32, i32, i32, i32);

class Headless {

    // Parsed command line arguments
    map<string,string> keys;

    // The emulator instance
    C64 c64;

    // Barrier for syncing script execution
    util::Mutex barrier;

    // Exit flag
    bool halt = false;


    //
    // Launching
    //

public:

    // Main entry point
    void main(int argc, char *argv[]);

private:

    // Parses the command line arguments
    void parseArguments(int argc, char *argv[]);

    // Checks all command line arguments for conistency
    void checkArguments() throws;


    //
    // Running
    //

public:

    // Processes an incoming message
    void process(long type, i32, i32, i32, i32);
};

}

