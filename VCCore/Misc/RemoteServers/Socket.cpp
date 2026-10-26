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
#include "Socket.h"
#include "utl/support/Bits.h"

namespace vc64 {

Socket::Socket() : socket(INVALID_SOCKET)
{
    logmsg(LOG_SCK, "Socket constructor\n");
}

// Keeps a write to a closed connection from raising SIGPIPE, which would end the process
static void
suppressSigPipe(SOCKET s)
{
#ifdef SO_NOSIGPIPE
    int on = 1;
    setsockopt(s, SOL_SOCKET, SO_NOSIGPIPE, (const char *)&on, sizeof(on));
#endif
}

Socket::Socket(SOCKET id) : socket(id)
{
    logmsg(LOG_SCK, "Wrapping socket %lld\n", (i64)id);
    if (id != INVALID_SOCKET) suppressSigPipe(id);
}

Socket::Socket(Socket&& other)
{
    socket = other.socket.exchange(INVALID_SOCKET);
}

Socket& Socket::operator=(Socket&& other)
{
    if (this != &other) {

        close();
        socket = other.socket.exchange(INVALID_SOCKET);
    }
    return *this;
}

Socket::~Socket()
{
    logmsg(LOG_SCK, "Socket destructor\n");
    
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
                throw ServerError(ServerError::SOCK_CANT_CREATE);
        }
        ~WSAInit() {
            
            WSACleanup();
        }
    } wsaInit;
#endif
    
    if (socket == INVALID_SOCKET) {
        
        // Create a new socket
        socket = ::socket(AF_INET, SOCK_STREAM, 0);
        if (socket == INVALID_SOCKET)
            throw ServerError(ServerError::SOCK_CANT_CREATE);
        
        // Set options
        int opt = 1;
        auto success = setsockopt(socket,
                                  SOL_SOCKET,
                                  SO_REUSEADDR,
                                  (const char *)&opt,
                                  sizeof(opt));
        if (success < 0)
            throw ServerError(ServerError::SOCK_CANT_CREATE);
        
        suppressSigPipe(socket);

        logmsg(LOG_SCK, "Created new socket %lld\n", (i64)socket);
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
    address.sin_port = utl::bigEndian(port);
    
    if (::connect(socket, (struct sockaddr *)&address, sizeof(address)) < 0)
        throw ServerError(ServerError::SOCK_CANT_CONNECT);
}

void
Socket::bind(u16 port)
{
    // Create the socket if it is uninitialized
    create();
    
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = utl::bigEndian(port);
    
    if (::bind(socket, (struct sockaddr *)&address, sizeof(address)) < 0)
        throw ServerError(ServerError::SOCK_CANT_BIND);
}

void
Socket::listen()
{
    if (::listen(socket, 3) < 0)
        throw ServerError(ServerError::SOCK_CANT_LISTEN);
}

bool
Socket::waitForConnection(isize milliseconds)
{
    if (socket == INVALID_SOCKET)
        throw ServerError(ServerError::SOCK_CANT_ACCEPT);

    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(socket, &fds);

    struct timeval tv;
    tv.tv_sec  = (decltype(tv.tv_sec))(milliseconds / 1000);
    tv.tv_usec = (decltype(tv.tv_usec))((milliseconds % 1000) * 1000);

    // The first argument is ignored on Windows
    auto ret = ::select((int)socket + 1, &fds, nullptr, nullptr, &tv);

    if (ret < 0) throw ServerError(ServerError::SOCK_CANT_ACCEPT);

    return ret > 0;
}

Socket
Socket::accept()
{
    struct sockaddr_in address;
    auto addrlen = (socklen_t)sizeof(struct sockaddr_in);
    auto s = ::accept(socket, (struct sockaddr *)&address, &addrlen);

    if (s == INVALID_SOCKET)
        throw ServerError(ServerError::SOCK_CANT_ACCEPT);
    
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
    
    throw ServerError(ServerError::SOCK_CANT_RECEIVE);
}

void
Socket::send(u8 value)
{
    send(string(1, char(value)));
}

void
Socket::send(const string &s)
{
#ifdef MSG_NOSIGNAL
    const int flags = MSG_NOSIGNAL;
#else
    const int flags = 0;
#endif

    /* Writing and closing exclude each other, so a descriptor that has been
     * closed (and possibly handed out again) is never written to. A packet is
     * written as a whole before the next sender gets a turn.
     */
    std::lock_guard<std::mutex> guard(sendMutex);

    auto fd = socket.load();
    if (fd == INVALID_SOCKET) throw ServerError(ServerError::SOCK_CANT_SEND);

    // A write may be partial
    const char *ptr = s.c_str();
    auto left = isize(s.length());

    while (left > 0) {

        auto n = ::send(fd, ptr, (int)left, flags);
        if (n <= 0) throw ServerError(ServerError::SOCK_CANT_SEND);

        ptr += n;
        left -= isize(n);
    }
}

void
Socket::close()
{
    auto fd = socket.load();
    if (fd == INVALID_SOCKET) return;

    /* Wake up threads that are blocked in recv() or send(). The latter would
     * otherwise keep us from getting the send mutex below.
     */
#ifdef _WIN32
    ::shutdown(fd, SD_BOTH);
#else
    ::shutdown(fd, SHUT_RDWR);
#endif

    std::lock_guard<std::mutex> guard(sendMutex);

    // Whoever gets the descriptor out of the atomic closes it, and only once
    fd = socket.exchange(INVALID_SOCKET);
    if (fd == INVALID_SOCKET) return;

    logmsg(LOG_SCK, "Closing socket %lld\n", (i64)fd);
#ifdef _WIN32
    closesocket(fd);
#else
    ::close(fd);
#endif
}

}
