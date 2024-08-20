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

#include "CoreObject.h"

#ifdef _WIN32

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <WinSock2.h>
#include <ws2tcpip.h>

#else

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

namespace vc64 { typedef int SOCKET; }
#define INVALID_SOCKET -1

#endif

namespace vc64 {

class Socket : public CoreObject {

    SOCKET socket;

public:
    
    // Size of the communication buffer
    static constexpr isize BUFFER_SIZE = 512;
    
    
    //
    // Initializing
    //
    
public:

    Socket();
    Socket(SOCKET id);
    Socket(const Socket& other) = delete;
    Socket& operator=(const Socket& other) = delete;
    Socket(Socket&& other);
    Socket& operator=(Socket&& other);
    ~Socket();

    void create();

    
    //
    // Methods from CoreObject
    //
    
private:
    
    const char *objectName() const override { return "Socket"; }
    void _dump(Category category, std::ostream& os) const override { };

    
    //
    // Establishing and terminating a connection
    //
    
public:
    
    void connect(u16 port) throws;
    void bind(u16 port) throws;
    void listen();
    Socket accept();
    void close();

    
    //
    // Transfering data
    //
    
public:
    
    string recv();
    void send(u8 value);
    void send(char c) { send((u8)c); }
    void send(const string &s);
};

}
