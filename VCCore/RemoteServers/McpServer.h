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

#include "RemoteServer.h"
#include "StdioTransport.h"
#include "TcpTransport.h"
#include "HttpTransport.h"
#include "json_fwd.h"
#include <atomic>
#include <mutex>

namespace vc64 {

namespace MCP {

// JSON-RPC error codes
const long PARSE_ERROR           = -32700;
const long INVALID_REQUEST       = -32600;
const long METHOD_NOT_FOUND      = -32601;
const long INVALID_PARAMS        = -32602;
const long INTERNAL_ERROR        = -32603;

// Error codes reserved by the MCP specification
const long HEADER_MISMATCH       = -32020;
const long UNSUPPORTED_VERSION   = -32022;

}

/* A Model Context Protocol (MCP) server. It speaks the stateless protocol
 * revision 2026-07-28 and answers the handshake of the earlier revisions
 * (initialize, ping) for clients that haven't been updated yet.
 *
 * Like the RPC server, the MCP server can be run on top of all transports:
 *
 *   STDIO: Newline-delimited JSON-RPC over stdin / stdout. Use this transport
 *          if the emulator (e.g., the headless app) is launched by the client.
 *   HTTP:  Streamable HTTP. All messages are POSTed to the endpoint "/mcp".
 *   TCP:   Newline-delimited JSON-RPC over a socket (as recommended by the
 *          specification for custom byte-stream transports).
 */
class McpServer final : public RemoteServer {

    StdioTransport stdio = StdioTransport(*this);
    TcpTransport tcp = TcpTransport(*this);
    HttpTransport http = HttpTransport(*this);

    // Receive buffer for the byte-stream transports (STDIO, TCP)
    string rxBuffer;
    std::mutex rxMutex;

    // Counter used to generate RetroShell command ids
    std::atomic<isize> commandCounter { 0 };

public:

    using RemoteServer::RemoteServer;

    McpServer& operator= (const McpServer& other) {

        RemoteServer::operator=(other);
        return *this;
    }


    //
    // Methods from CoreObject
    //

protected:

    void _dump(Category category, std::ostream &os) const override;


    //
    // Methods from RemoteServer
    //

    Transport &transport() override;
    const Transport &transport() const override;
    bool isSupported(TransportProtocol protocol) const override;
    bool canRun() override { return true; }
    string endpoint() const override { return "/mcp"; }

public:

    void send(const string &payload) override;


    //
    // Methods from TransportDelegate
    //

private:

    void didStart() override;
    void didDisconnect() override;
    void didReceive(const string &payload) override;
    void didReceive(const struct httplib::Request &req, struct httplib::Response &res) override;


    //
    // Handling requests
    //

private:

    // Response to a single message (an empty body denotes a notification)
    struct Reply { int status = 200; string body; };

    // Processes a single JSON-RPC message
    Reply process(const string &message, const struct httplib::Request *req);

    // Executes a RetroShell command and waits for the result
    string execBlocking(const string &command);


    //
    // Tools
    //

    /* Each tool receives the 'arguments' object of the call and returns the
     * result of the tool call (content, structuredContent). A tool signals
     * failure by throwing an exception.
     */
    nlohmann::json callTool(const string &name, const nlohmann::json &args);
    nlohmann::json toolRetroShell(const nlohmann::json &args);
    nlohmann::json toolScreenshot(const nlohmann::json &args);
    nlohmann::json toolReadMemory(const nlohmann::json &args);
    nlohmann::json toolGetState(const nlohmann::json &args);
    nlohmann::json toolWriteMemory(const nlohmann::json &args);
    nlohmann::json toolTypeText(const nlohmann::json &args);
    nlohmann::json toolInsertMedia(const nlohmann::json &args);
    nlohmann::json toolDisassemble(const nlohmann::json &args);
};

}
