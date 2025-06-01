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

#pragma once

#include "VirtualC64.h"
#include "Wakeable.h"
#include <map>

namespace vc64 {

struct SyntaxError : public std::runtime_error {
    using runtime_error::runtime_error;
};

// The message listener
void process(const void *listener, Message msg);

class Headless : Wakeable {

    // Parsed command line arguments
    std::map<string,string> keys;

    // Return code
    int returnCode;

    
    //
    // Launching
    //
    
public:

    // Main entry point
    int main(int argc, char *argv[]);

private:

    // Parses the command line arguments
    void parseArguments(int argc, char *argv[]);

    // Checks all command line arguments for conistency
    void checkArguments() throws;

    // Runs a RetroShell script
    void runScript(const char **script);
    void runScript(const std::filesystem::path &path);
    

    //
    // Running
    //

public:
    
    // Reports size information
    void reportSize();

    // Processes an incoming message
    void process(Message msg);
};

}
