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
#include "StdioTransport.h"
#include <iostream>

#ifdef _WIN32

// stdio transport is not supported on Windows
namespace vc64 {
Stdio::Stdio() { }
Stdio::~Stdio() { }
void Stdio::terminate() { }
string Stdio::get() { return ""; }
void Stdio::put(const string &) { }
void StdioTransport::disconnect() { }
void StdioTransport::main(u16, const string &) { }
void StdioTransport::sessionLoop() { }
void StdioTransport::send(const string &) { }
}
#else

#include <unistd.h>
#include <stdexcept>
#include <errno.h>
#include <string.h>

namespace vc64 {

Stdio::Stdio() {

    // Create a pipe for signaling termination
    if (pipe(term) < 0) {
        throw std::runtime_error("Failed to create termination pipe");
    }
}

Stdio::~Stdio() {

    close(term[0]);
    close(term[1]);
}

void
Stdio::terminate() {

    write(term[1], "x", 1);
}

string
Stdio::get()
{
    std::array<char, 4096> buffer;

    // Setup the descriptor set
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    FD_SET(term[0], &fds);

    // Block until stdin or the termination pipe is ready
    int ret = select(std::max(STDIN_FILENO, term[0]) + 1, &fds, nullptr, nullptr, nullptr);

    // Check for errors
    if (ret < 0) throw std::runtime_error("select() failed");

    // Check if the termination pipe has data
    if (FD_ISSET(term[0], &fds)) {

        // Clear the pipe and exit the loop
        char tmp;
        read(term[0], &tmp, 1);
        return "";
    }

    // Check if stdin has data
    if (FD_ISSET(STDIN_FILENO, &fds)) {

        auto n = read(STDIN_FILENO, buffer.data(), buffer.size());

        // Check for errors
        if (n < 0) throw std::runtime_error("stdin read error");

        return string(buffer.data(), n);
    }

    throw std::runtime_error("get() failed");
}

void
Stdio::put(const string &str)
{
    std::cout << str;
}

void
StdioTransport::disconnect()
{
    loginfo(SRV_DEBUG, "Disconnecting stdio transport...\n");

    switchState(SrvState::STOPPING);
    stdio.terminate();
}

void
StdioTransport::main(u16 port, const string &endpoint)
{
    try {

        sessionLoop();

    } catch (std::exception &err) {

        loginfo(SRV_DEBUG, "Stdio server thread interrupted\n");
        delegate.didTerminate(err.what());
    }

    switchState(SrvState::OFF);
}

void
StdioTransport::sessionLoop()
{
    switchState(SrvState::CONNECTED);

    string line;

    while (!isStopping()) {

        stdio >> line;
        delegate.didReceive(line);
    }
}

void
StdioTransport::send(const string &payload)
{
    if (isConnected()) { stdio << payload; }
}

}

#endif
