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
#include "StdioTransport.h"

#include <array>
#include <algorithm>
#include <iostream>
#include <stdexcept>

namespace vc64 {

//
// Platform-independent transport logic
//

void
Stdio::put(const string &str)
{
    // Flush after every packet because stdout is fully buffered when connected to a pipe
    std::cout << str << std::flush;
}

StdioTransport::~StdioTransport()
{
    stop();
}

void
StdioTransport::disconnect()
{
    logmsg(LOG_SRV, "Disconnecting stdio transport...\n");

    switchState(SrvState::STOPPING);

    if (!stdio.terminate()) {
        logmsg(LOG_WARN, "Failed to interrupt the stdio reader\n");
    }
}

void
StdioTransport::main(u16 port, const string &endpoint)
{
    try {

        sessionLoop();

    } catch (std::exception &err) {

        logmsg(LOG_SRV, "Stdio server thread interrupted\n");
        delegate.didTerminate(err.what());
    }

    switchState(SrvState::OFF);
}

void
StdioTransport::sessionLoop()
{
    switchState(SrvState::CONNECTED);

    string line;

    while (!terminating()) {

        stdio >> line;
        deliver(line);
    }
}

void
StdioTransport::send(const string &payload)
{
    if (isConnected()) {

        stdio << payload;
        record(TrafficDirection::SENT, payload);
    }
}

}

//
// Platform-specific stdin primitives
//

#ifdef _WIN32

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

namespace vc64 {

/* A parent process wires up our stdin to an anonymous pipe. Unlike POSIX
 * file descriptors, such a pipe handle does not become signaled when data
 * arrives, so it cannot be waited on with WaitForSingleObject. We therefore
 * poll it with PeekNamedPipe and, while no data is pending, sleep on a wake
 * event. terminate() signals that event to interrupt the wait at once.
 */
static constexpr DWORD pollIntervalMs = 20;

Stdio::Stdio()
{
    // Manual-reset event, initially non-signaled
    wakeEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!wakeEvent) throw std::runtime_error("Failed to create wake event");
}

Stdio::~Stdio()
{
    if (wakeEvent) CloseHandle((HANDLE)wakeEvent);
}

bool
Stdio::terminate()
{
    return SetEvent((HANDLE)wakeEvent) != 0;
}

string
Stdio::get()
{
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    std::array<char, 4096> buffer;

    while (true) {

        // Determine how many bytes can be read without blocking
        DWORD avail = 0;
        if (!PeekNamedPipe(hStdin, nullptr, 0, nullptr, &avail, nullptr)) {

            // A broken or invalid pipe is our equivalent of EOF. The casts
            // keep both operands unsigned (the ERROR_* macros are signed
            // literals) so /W4 does not flag a signed/unsigned mismatch.
            auto err = GetLastError();
            if (err == (DWORD)ERROR_BROKEN_PIPE || err == (DWORD)ERROR_INVALID_HANDLE) {
                throw std::runtime_error("stdin closed");
            }
            throw std::runtime_error("PeekNamedPipe failed");
        }

        if (avail > 0) {

            DWORD nRead = 0;
            DWORD toRead = std::min<DWORD>(avail, (DWORD)buffer.size());

            if (!ReadFile(hStdin, buffer.data(), toRead, &nRead, nullptr)) {
                if (GetLastError() == (DWORD)ERROR_BROKEN_PIPE) throw std::runtime_error("stdin closed");
                throw std::runtime_error("stdin read error");
            }

            // A zero-byte read also indicates EOF
            if (nRead == 0) throw std::runtime_error("stdin closed");

            return string(buffer.data(), nRead);
        }

        // No data pending: wait until the poll interval elapses or terminate()
        // signals the wake event.
        if (WaitForSingleObject((HANDLE)wakeEvent, pollIntervalMs) == WAIT_OBJECT_0) {

            // Drain the signal so a later session starts clean (mirrors the
            // POSIX build reading its self-pipe byte).
            ResetEvent((HANDLE)wakeEvent);
            return "";
        }
    }
}

}

#else

#include <unistd.h>
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

bool
Stdio::terminate() {

    // Wake up get() with a byte on the termination pipe
    while (true) {

        auto n = write(term[1], "x", 1);

        if (n == 1) return true;
        if (n < 0 && errno == EINTR) continue;

        return false;
    }
}

string
Stdio::get()
{
    std::array<char, 4096> buffer;

    fd_set fds;
    int ret;

    do {

        // Setup the descriptor set
        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO, &fds);
        FD_SET(term[0], &fds);

        // Block until stdin or the termination pipe is ready
        ret = select(std::max(STDIN_FILENO, term[0]) + 1, &fds, nullptr, nullptr, nullptr);

    } while (ret < 0 && errno == EINTR);

    // Check for errors
    if (ret < 0) throw std::runtime_error(string("select() failed: ") + strerror(errno));

    // Check if the termination pipe has data
    if (FD_ISSET(term[0], &fds)) {

        // Clear the pipe and exit the loop
        char tmp;
        ssize_t n;
        do { n = read(term[0], &tmp, 1); } while (n < 0 && errno == EINTR);

        // Check for errors
        if (n != 1) throw std::runtime_error("termination pipe read error");

        return "";
    }

    // Check if stdin has data
    if (FD_ISSET(STDIN_FILENO, &fds)) {

        ssize_t n;
        do { n = read(STDIN_FILENO, buffer.data(), buffer.size()); } while (n < 0 && errno == EINTR);

        // Check for errors
        if (n < 0) throw std::runtime_error(string("stdin read error: ") + strerror(errno));

        // Treat EOF as a disconnect
        if (n == 0) throw std::runtime_error("stdin closed");

        return string(buffer.data(), n);
    }

    throw std::runtime_error("get() failed");
}

}

#endif
