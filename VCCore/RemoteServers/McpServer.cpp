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
#include "McpServer.h"
#include "C64.h"
#include "json.h"
#include "httplib.h"
#include "utl/support.h"
#include <future>

namespace vc64 {

using nlohmann::json;

namespace {

// The protocol revision implemented by this server
const char *currentVersion = "2026-07-28";

// Revisions that are accepted from clients still using the initialize handshake
const char *legacyVersions[] = { "2025-11-25", "2025-06-18", "2025-03-26", "2024-11-05" };

// Longest time a tool call may wait for the emulator
const auto toolTimeout = std::chrono::seconds(30);

// Error raised while processing a request. It is turned into a JSON-RPC error.
struct McpError {

    long code;
    string message;
    int status = 200;
    json data = nullptr;
};

json
makeError(const json &id, const McpError &e)
{
    json error = { {"code", e.code}, {"message", e.message} };
    if (!e.data.is_null()) error["data"] = e.data;

    return { {"jsonrpc", "2.0"}, {"id", id}, {"error", error} };
}

json
makeResult(const json &id, json result)
{
    return { {"jsonrpc", "2.0"}, {"id", id}, {"result", std::move(result)} };
}

// Looks up the protocol version a request announces in its _meta block
optional<string>
announcedVersion(const json &params)
{
    if (!params.is_object() || !params.contains("_meta")) return { };

    const auto &meta = params["_meta"];
    if (!meta.is_object()) return { };

    for (auto it = meta.begin(); it != meta.end(); ++it) {

        const auto &key = it.key();
        if (key.size() >= 15 && key.compare(key.size() - 15, 15, "protocolVersion") == 0) {
            if (it.value().is_string()) return it.value().get<string>();
        }
    }
    return { };
}

bool
isLocalOrigin(const string &origin)
{
    for (auto prefix : { "http://localhost", "https://localhost",
                         "http://127.0.0.1", "https://127.0.0.1",
                         "http://[::1]", "https://[::1]" }) {

        auto len = strlen(prefix);
        if (origin.compare(0, len, prefix) != 0) continue;

        // The host must end here or be followed by a port
        if (origin.size() == len || origin[len] == ':') return true;
    }
    return false;
}

// Decodes a header value of the form =?base64?...?= (other values pass through)
string
decodeHeader(const string &value)
{
    static const string prefix = "=?base64?", suffix = "?=";

    if (value.size() < prefix.size() + suffix.size() ||
        value.compare(0, prefix.size(), prefix) != 0 ||
        value.compare(value.size() - suffix.size(), suffix.size(), suffix) != 0) return value;

    auto b64 = value.substr(prefix.size(), value.size() - prefix.size() - suffix.size());

    string out;
    u32 acc = 0;
    int bits = 0;
    for (char c : b64) {

        int v;
        if      (c >= 'A' && c <= 'Z') v = c - 'A';
        else if (c >= 'a' && c <= 'z') v = c - 'a' + 26;
        else if (c >= '0' && c <= '9') v = c - '0' + 52;
        else if (c == '+') v = 62;
        else if (c == '/') v = 63;
        else continue;

        acc = (acc << 6) | u32(v);
        bits += 6;
        if (bits >= 8) { bits -= 8; out += char((acc >> bits) & 0xFF); }
    }
    return out;
}

json
toolDefinitions()
{
    json retroshell = {

        {"name", "retroshell"},
        {"title", "RetroShell"},
        {"description",
            "Executes a RetroShell command in the emulator and returns its output. "
            "RetroShell is the emulator's command line interface. It controls the "
            "emulator (e.g., 'c64 power on', 'c64 pause'), configures components, "
            "mounts media ('df8 insert <path>'), manages snapshots, and offers a "
            "debugger. Use 'help' to list the available commands. "
            "Use 'help <command>' to learn more about a specific one."},
        {"inputSchema", {

            {"type", "object"},
            {"properties", {
                {"command", {
                    {"type", "string"},
                    {"description", "The RetroShell command to execute"}
                }}
            }},
            {"required", json::array({"command"})}
        }}
    };

    return json::array({ retroshell });
}

}

void
McpServer::_dump(Category category, std::ostream &os) const
{
    RemoteServer::_dump(category, os);
}

Transport &
McpServer::transport()
{
    switch (config.transport) {

        case TransportProtocol::STDIO: return stdio;
        case TransportProtocol::TCP:   return tcp;
        case TransportProtocol::HTTP:  return http;

        default:
            fatalError;
    }
}

const Transport &
McpServer::transport() const
{
    return const_cast<McpServer *>(this)->transport();
}

bool
McpServer::isSupported(TransportProtocol protocol) const
{
    switch (protocol) {

        case TransportProtocol::STDIO:  return true;
        case TransportProtocol::TCP:    return true;
        case TransportProtocol::HTTP:   return true;

        default:
            return false;
    }
}

void
McpServer::didStart()
{
    if (config.verbose) {

        *this << "Remote server is listening at port " << config.port << "\n";
    }
}

void
McpServer::didDisconnect()
{
    std::lock_guard<std::mutex> lock(rxMutex);
    rxBuffer.clear();
}

void
McpServer::send(const string &payload)
{
    transport().send(payload);

    if (config.verbose) {

        printf("T: %s\n", utl::makePrintable(payload).c_str());
    }
}

void
McpServer::didReceive(const string &payload)
{
    std::vector<string> lines;

    if (config.transport == TransportProtocol::STDIO) {

        // The stdio transport already hands out complete lines
        lines.push_back(payload);

    } else {

        // A TCP read may return a fraction of a message or several messages
        std::lock_guard<std::mutex> lock(rxMutex);

        rxBuffer += payload;

        for (size_t pos; (pos = rxBuffer.find('\n')) != string::npos; ) {

            lines.push_back(rxBuffer.substr(0, pos));
            rxBuffer.erase(0, pos + 1);
        }
    }

    for (auto &line : lines) {

        auto trimmed = utl::rtrim(line, "\r");
        if (trimmed.find_first_not_of(" \t") == string::npos) continue;

        if (config.verbose) printf("R: %s\n", utl::makePrintable(trimmed).c_str());

        auto reply = process(trimmed, nullptr);
        if (!reply.body.empty()) send(reply.body + "\n");
    }
}

void
McpServer::didReceive(const struct httplib::Request &req, struct httplib::Response &res)
{
    // This server never initiates messages, so there is no stream to offer
    if (req.method != "POST") {

        res.status = 405;
        res.set_header("Allow", "POST");
        return;
    }

    if (config.verbose) printf("R: %s\n", utl::makePrintable(req.body).c_str());

    auto reply = process(req.body, &req);

    res.status = reply.status;
    if (!reply.body.empty()) res.set_content(reply.body, "application/json");
}

McpServer::Reply
McpServer::process(const string &message, const struct httplib::Request *req)
{
    json id = nullptr;

    try {

        // DNS rebinding protection (HTTP transport only)
        if (req && req->has_header("Origin") && !isLocalOrigin(req->get_header_value("Origin"))) {
            throw McpError { MCP::INVALID_REQUEST, "Forbidden origin", 403 };
        }

        json request;
        try { request = json::parse(message); }
        catch (const json::parse_error &) {
            throw McpError { MCP::PARSE_ERROR, "Parse error", 400 };
        }

        if (!request.is_object() || !request.contains("method") || !request["method"].is_string()) {
            throw McpError { MCP::INVALID_REQUEST, "Invalid request", 400 };
        }

        auto method = request["method"].get<string>();
        auto params = request.value("params", json::object());
        if (!params.is_object()) params = json::object();

        // Notifications and responses are acknowledged without a body
        if (!request.contains("id")) return { 202, "" };
        id = request["id"];

        // The HTTP headers must agree with the message body
        if (req) {

            if (req->has_header("Mcp-Method") && req->get_header_value("Mcp-Method") != method) {
                throw McpError { MCP::HEADER_MISMATCH, "Mcp-Method header does not match the request", 400 };
            }
            if (method == "tools/call" && req->has_header("Mcp-Name")) {

                auto name = params.value("name", string());
                if (decodeHeader(req->get_header_value("Mcp-Name")) != name) {
                    throw McpError { MCP::HEADER_MISMATCH, "Mcp-Name header does not match the request", 400 };
                }
            }
        }

        // Requests of the stateless protocol announce their version
        auto version = announcedVersion(params);
        if (version && *version != currentVersion) {

            throw McpError {

                MCP::UNSUPPORTED_VERSION,
                "Unsupported protocol version: " + *version,
                400,
                { {"supported", json::array({ currentVersion })} }
            };
        }
        bool modern = version.has_value();

        // Legacy handshake (clients that haven't adopted the stateless protocol)
        if (method == "initialize") {

            auto requested = params.value("protocolVersion", string());
            auto negotiated = string(currentVersion);

            for (auto v : legacyVersions) if (requested == v) negotiated = v;

            return { 200, makeResult(id, {

                {"protocolVersion", negotiated},
                {"capabilities", {{"tools", json::object()}}},
                {"serverInfo", {{"name", "VirtualC64"}, {"version", C64::version()}}}

            }).dump() };
        }
        if (method == "ping") {
            return { 200, makeResult(id, json::object()).dump() };
        }

        json result;

        if (method == "server/discover") {

            result = {

                {"supportedVersions", json::array({ currentVersion })},
                {"capabilities", {{"tools", json::object()}}},
                {"serverInfo", {{"name", "VirtualC64"}, {"version", C64::version()}}},
                {"instructions", "VirtualC64 is a Commodore 64 emulator. "
                    "Use the 'retroshell' tool to control it."}
            };

        } else if (method == "tools/list") {

            result = { {"tools", toolDefinitions()}, {"ttlMs", 3600000}, {"cacheScope", "public"} };

        } else if (method == "tools/call") {

            if (params.value("name", string()) != "retroshell") {
                throw McpError { MCP::INVALID_PARAMS, "Unknown tool", 200 };
            }
            auto args = params.value("arguments", json::object());
            if (!args.is_object() || !args.contains("command") || !args["command"].is_string()) {
                throw McpError { MCP::INVALID_PARAMS, "Missing string argument 'command'", 200 };
            }

            // Failing commands are reported as tool errors, not as protocol errors
            result = { {"isError", false} };
            try {
                auto output = execBlocking(args["command"].get<string>());
                result["content"] = json::array({ {{"type", "text"}, {"text", output}} });
            } catch (const std::exception &e) {
                result["isError"] = true;
                result["content"] = json::array({ {{"type", "text"}, {"text", e.what()}} });
            }

        } else {

            throw McpError { MCP::METHOD_NOT_FOUND, "Method not found: " + method, 200 };
        }

        if (modern) result["resultType"] = "complete";
        return { 200, makeResult(id, result).dump() };

    } catch (const McpError &e) {

        return { e.status, makeError(id, e).dump() };

    } catch (const std::exception &e) {

        return { 500, makeError(id, McpError { MCP::INTERNAL_ERROR, e.what(), 500 }).dump() };
    }
}

string
McpServer::execBlocking(const string &command)
{
    struct Outcome { string output; std::exception_ptr error; };

    // The completion handler fulfills a promise the calling thread waits for
    auto promise = std::make_shared<std::promise<Outcome>>();
    auto future = promise->get_future();

    // Commands are tagged as MCP so the RPC server doesn't report them as well
    rpcShell.asyncExec(InputLine {

        .id = ++commandCounter,
        .type = InputLine::Source::MCP,
        .input = command,
        .completion = [promise](const string &output, std::exception_ptr error) {

            promise->set_value({ output, error });
        }
    });

    if (future.wait_for(toolTimeout) != std::future_status::ready) {
        throw std::runtime_error("Timeout. Is the emulator running?");
    }

    auto outcome = future.get();
    if (outcome.error) std::rethrow_exception(outcome.error);

    return outcome.output;
}

}
