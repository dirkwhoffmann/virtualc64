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
#include "Socket.h"
#include "MemUtils.h"

namespace vc64 {

Socket::Socket() : socket(INVALID_SOCKET)
{
    debug(SCK_DEBUG, "Socket constructor\n");
}

Socket::Socket(SOCKET id) : socket(id)
{
    debug(SCK_DEBUG, "Wrapping socket %lld\n", (i64)id);
}

Socket::Socket(Socket&& other)
{
    socket = other.socket;
    other.socket = INVALID_SOCKET;
}

Socket& Socket::operator=(Socket&& other)
{
    assert(socket != other.socket);
    
    if (socket != other.socket) {

        close();
        socket = other.socket;
        other.socket = INVALID_SOCKET;
    }
    return *this;
}

Socket::~Socket()
{
    debug(SCK_DEBUG, "Socket destructor\n");
    
    if (socket != INVALID_SOCKET) {
        close();
    }
}

void Socket::create()
{
#ifdef _WIN32
    static struct WSAInit {
        
        WSAInit() {
            
            WSADATA wsaData;
            if (WSAStartup(MAKEWORD(2, 2), &wsaData))
                throw Error(VC64ERROR_SOCK_CANT_CREATE);
        }
        ~WSAInit() {
            
            WSACleanup();
        }
    } wsaInit;
#endif
    
    if (socket == INVALID_SOCKET) {
        
        // Create a new socket
        socket = ::socket(AF_INET, SOCK_STREAM, 0);
        if (socket == INVALID_SOCKET) {
            throw Error(VC64ERROR_SOCK_CANT_CREATE);
        }
        
        // Set options
        int opt = 1;
        auto success = setsockopt(socket,
                                  SOL_SOCKET,
                                  SO_REUSEADDR,
                                  (const char *)&opt,
                                  sizeof(opt));
        if (success < 0) {
            throw Error(VC64ERROR_SOCK_CANT_CREATE);
        }
        
        debug(SCK_DEBUG, "Created new socket %lld\n", (i64)socket);
    }
}

void
Socket::connect(u16 port)
{
    // Create the socket if it is uninitialized
    create();

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = util::bigEndian(port);
    
    if (::connect(socket, (struct sockaddr *)&address, sizeof(address)) < 0) {
        throw Error(VC64ERROR_SOCK_CANT_CONNECT);
    }
}

void
Socket::bind(u16 port)
{
    // Create the socket if it is uninitialized
    create();
    
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = util::bigEndian(port);
    
    if (::bind(socket, (struct sockaddr *)&address, sizeof(address)) < 0) {
        throw Error(VC64ERROR_SOCK_CANT_BIND);
    }
}

void
Socket::listen()
{
    if (::listen(socket, 3) < 0) {
        throw Error(VC64ERROR_SOCK_CANT_LISTEN);
    }
}

Socket
Socket::accept()
{
    struct sockaddr_in address;
    auto addrlen = (socklen_t)sizeof(struct sockaddr_in);
    auto s = ::accept(socket, (struct sockaddr *)&address, &addrlen);

    if (s == INVALID_SOCKET) {
        throw Error(VC64ERROR_SOCK_CANT_ACCEPT);
    }
    
    return Socket(s);
}

std::string
Socket::recv()
{    
    char buffer[BUFFER_SIZE + 1] = {};
    if (auto n = ::recv(socket, buffer, BUFFER_SIZE, 0); n > 0) {
        
        // Convert the buffer to a string
        string result = string(buffer, n);
        return result;
    }
    
    throw Error(VC64ERROR_SOCK_CANT_RECEIVE);
}

void
Socket::send(u8 value)
{
    if (::send(socket, (const char *)&value, 1, 0) < 1) {
        throw Error(VC64ERROR_SOCK_CANT_SEND);
    }
}

void
Socket::send(const string &s)
{
    if (::send(socket, s.c_str(), (int)s.length(), 0) < 0) {
        throw Error(VC64ERROR_SOCK_CANT_SEND);
    }
}

void
Socket::close()
{    
    if (socket != INVALID_SOCKET) {

        debug(SCK_DEBUG, "Closing socket %lld\n", (i64)socket);
#ifdef _WIN32
        closesocket(socket);
#else
        ::close(socket);
#endif
        socket = INVALID_SOCKET;
    }
}

}
