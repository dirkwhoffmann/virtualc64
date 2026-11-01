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
#include "Emulator.h"
#include "json.h"
#include "httplib.h"
#include "utl/support.h"
#include <future>
#include <sstream>

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

    json screenshot = {

        {"name", "screenshot"},
        {"title", "Screenshot"},
        {"description",
            "Returns the current screen contents as a PNG image. The layer "
            "'xray' shows the bus debugger overlay instead of the regular picture."},
        {"inputSchema", {

            {"type", "object"},
            {"properties", {
                {"layer", {
                    {"type", "string"},
                    {"enum", json::array({"emulator", "xray"})},
                    {"description", "Texture to capture (default: emulator)"}
                }},
                {"area", {
                    {"type", "string"},
                    {"enum", json::array({"visible", "full"})},
                    {"description", "'visible' crops the border to the usual screen area, "
                                    "'full' returns the entire texture (default: visible)"}
                }}
            }}
        }},
        {"annotations", {{"readOnlyHint", true}, {"openWorldHint", false}}}
    };

    json readMemory = {

        {"name", "read_memory"},
        {"title", "Read memory"},
        {"description",
            "Reads bytes from the C64 address space without side effects. By "
            "default, memory is shown as the CPU sees it (including the ROMs "
            "and I/O area selected by the processor port). Use source 'ram' to "
            "read the RAM underneath."},
        {"inputSchema", {

            {"type", "object"},
            {"properties", {
                {"address", {
                    {"type", json::array({"integer", "string"})},
                    {"description", "Start address (0...65535), or a string such as '$C000' or '0xC000'"}
                }},
                {"length", {
                    {"type", "integer"},
                    {"minimum", 1},
                    {"maximum", 4096},
                    {"description", "Number of bytes to read (default: 256)"}
                }},
                {"source", {
                    {"type", "string"},
                    {"enum", json::array({"cpu", "ram"})},
                    {"description", "Memory view (default: cpu)"}
                }}
            }},
            {"required", json::array({"address"})}
        }},
        {"annotations", {{"readOnlyHint", true}, {"openWorldHint", false}}}
    };

    json getState = {

        {"name", "get_state"},
        {"title", "Emulator state"},
        {"description",
            "Returns the power state, the video standard, the current frame, "
            "raster line and raster cycle, and the CPU registers."},
        {"inputSchema", {{"type", "object"}, {"properties", json::object()}}},
        {"annotations", {{"readOnlyHint", true}, {"openWorldHint", false}}}
    };

    json writeMemory = {

        {"name", "write_memory"},
        {"title", "Write memory"},
        {"description",
            "Writes bytes into the C64 address space. By default, the write "
            "goes through the CPU's current memory mapping (so I/O registers "
            "can be written, and writes to ROM areas land in the RAM below). "
            "Use target 'ram' to write into RAM unconditionally."},
        {"inputSchema", {

            {"type", "object"},
            {"properties", {
                {"address", {
                    {"type", json::array({"integer", "string"})},
                    {"description", "Start address (0...65535), or a string such as '$C000'"}
                }},
                {"bytes", {
                    {"type", json::array({"array", "string"})},
                    {"items", {{"type", "integer"}, {"minimum", 0}, {"maximum", 255}}},
                    {"description", "Bytes to write, as an array of integers or a hex string such as 'A9 00 8D 20 D0'"}
                }},
                {"target", {
                    {"type", "string"},
                    {"enum", json::array({"cpu", "ram"})},
                    {"description", "Memory view (default: cpu)"}
                }}
            }},
            {"required", json::array({"address", "bytes"})}
        }},
        {"annotations", {{"readOnlyHint", false}, {"destructiveHint", true},
                         {"idempotentHint", true}, {"openWorldHint", false}}}
    };

    json typeText = {

        {"name", "type_text"},
        {"title", "Type text"},
        {"description",
            "Types text on the C64 keyboard. Use '\\n' for the RETURN key. "
            "Typing is asynchronous: the keys are fed to the machine over the "
            "next frames. Example: 'load \"*\",8,1\\n'."},
        {"inputSchema", {

            {"type", "object"},
            {"properties", {
                {"text", {{"type", "string"}, {"description", "Text to type"}}}
            }},
            {"required", json::array({"text"})}
        }},
        {"annotations", {{"readOnlyHint", false}, {"destructiveHint", false}, {"openWorldHint", false}}}
    };

    json insertMedia = {

        {"name", "insert_media"},
        {"title", "Insert media"},
        {"description",
            "Inserts a file into the emulated machine. The kind is derived from "
            "the file extension unless given: D64/G64/T64/P00/PRG are inserted "
            "as a disk into a floppy drive, TAP as a tape into the datasette, "
            "and CRT as a cartridge into the expansion port (which resets the "
            "machine). Kind 'program' flashes a PRG file directly into memory."},
        {"inputSchema", {

            {"type", "object"},
            {"properties", {
                {"path", {{"type", "string"}, {"description", "File path"}}},
                {"kind", {
                    {"type", "string"},
                    {"enum", json::array({"disk", "tape", "cartridge", "program"})},
                    {"description", "Media kind (default: derived from the extension)"}
                }},
                {"drive", {
                    {"type", "integer"},
                    {"enum", json::array({8, 9})},
                    {"description", "Target drive for disks (default: 8)"}
                }}
            }},
            {"required", json::array({"path"})}
        }},
        {"annotations", {{"readOnlyHint", false}, {"destructiveHint", true},
                         {"idempotentHint", false}, {"openWorldHint", false}}}
    };

    json disassemble = {

        {"name", "disassemble"},
        {"title", "Disassemble"},
        {"description",
            "Disassembles 6510 code as the CPU sees it. Starts at the current "
            "program counter unless an address is given. Numbers are hexadecimal, "
            "prefixed with '$'."},
        {"inputSchema", {

            {"type", "object"},
            {"properties", {
                {"address", {
                    {"type", json::array({"integer", "string"})},
                    {"description", "Start address (default: program counter)"}
                }},
                {"count", {
                    {"type", "integer"},
                    {"minimum", 1},
                    {"maximum", 256},
                    {"description", "Number of instructions (default: 16)"}
                }}
            }}
        }},
        {"annotations", {{"readOnlyHint", true}, {"openWorldHint", false}}}
    };

    return json::array({ retroshell, screenshot, readMemory, getState,
                         writeMemory, typeText, insertMedia, disassemble });
}

// Encodes binary data as base64
string
base64(const std::vector<u8> &data)
{
    static const char *tab = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    string out;
    out.reserve((data.size() + 2) / 3 * 4);

    for (size_t i = 0; i < data.size(); i += 3) {

        u32 n = u32(data[i]) << 16;
        if (i + 1 < data.size()) n |= u32(data[i + 1]) << 8;
        if (i + 2 < data.size()) n |= u32(data[i + 2]);

        out += tab[(n >> 18) & 63];
        out += tab[(n >> 12) & 63];
        out += i + 1 < data.size() ? tab[(n >> 6) & 63] : '=';
        out += i + 2 < data.size() ? tab[n & 63] : '=';
    }
    return out;
}

// Parses an address given as an integer or as a string ('$C000', '0xC000', '49152')
long
parseAddress(const json &value)
{
    if (value.is_number_integer()) return value.get<long>();

    if (value.is_string()) {

        auto s = value.get<string>();
        int base = 10;
        if (!s.empty() && s[0] == '$') { s.erase(0, 1); base = 16; }
        else if (s.size() > 2 && s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) { s.erase(0, 2); base = 16; }

        size_t used = 0;
        try {
            auto result = std::stol(s, &used, base);
            if (used == s.size()) return result;
        } catch (...) { }
    }
    throw std::invalid_argument("Invalid address");
}

// Suspends the emulator thread for the lifetime of the object
struct Suspender {

    const Emulator &emulator;
    Suspender(const Emulator &e) : emulator(e) { emulator.suspend(); }
    ~Suspender() { emulator.resume(); }
};

// Wraps text into the result of a tool call
json
textResult(const string &text)
{
    return { {"content", json::array({ {{"type", "text"}, {"text", text}} })} };
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

            auto name = params.value("name", string());
            auto args = params.value("arguments", json::object());
            if (!args.is_object()) args = json::object();

            bool known = false;
            for (auto &tool : toolDefinitions()) if (tool["name"] == name) known = true;
            if (!known) throw McpError { MCP::INVALID_PARAMS, "Unknown tool: " + name, 200 };

            // Failing tools are reported as tool errors, not as protocol errors
            try {
                result = callTool(name, args);
                result["isError"] = false;
            } catch (const std::exception &e) {
                result = textResult(e.what());
                result["isError"] = true;
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

json
McpServer::callTool(const string &name, const json &args)
{
    if (name == "retroshell")   return toolRetroShell(args);
    if (name == "screenshot")   return toolScreenshot(args);
    if (name == "read_memory")  return toolReadMemory(args);
    if (name == "get_state")    return toolGetState(args);
    if (name == "write_memory") return toolWriteMemory(args);
    if (name == "type_text")    return toolTypeText(args);
    if (name == "insert_media") return toolInsertMedia(args);
    if (name == "disassemble")  return toolDisassemble(args);

    throw std::invalid_argument("Unknown tool: " + name);
}

json
McpServer::toolRetroShell(const json &args)
{
    if (!args.contains("command") || !args["command"].is_string()) {
        throw std::invalid_argument("Missing string argument 'command'");
    }
    return textResult(execBlocking(args["command"].get<string>()));
}

json
McpServer::toolScreenshot(const json &args)
{
    auto layer = args.value("layer", string("emulator"));
    auto area = args.value("area", string("visible"));

    if (layer != "emulator" && layer != "xray") throw std::invalid_argument("Invalid layer");
    if (area != "visible" && area != "full") throw std::invalid_argument("Invalid area");

    std::vector<u8> png;
    {
        Suspender _(emulator);
        emulator.lockTexture();

        try {
            if (area == "full") {

                png = layer == "xray" ? videoPort.encodeXrayTexture() : videoPort.encodeTexture();

            } else {

                // Same cutout as used for snapshot previews
                auto y2 = vic.pal() ? 299 : 249;
                png = layer == "xray" ? videoPort.encodeXrayTexture(104, 487, 16, y2)
                                      : videoPort.encodeTexture(104, 487, 16, y2);
            }

        } catch (...) { emulator.unlockTexture(); throw; }
        emulator.unlockTexture();
    }

    return { {"content", json::array({

        { {"type", "image"}, {"data", base64(png)}, {"mimeType", "image/png"} }

    })} };
}

json
McpServer::toolReadMemory(const json &args)
{
    if (!args.contains("address")) throw std::invalid_argument("Missing argument 'address'");

    auto address = parseAddress(args["address"]);
    auto length = args.value("length", 256L);
    auto source = args.value("source", string("cpu"));

    if (address < 0 || address > 0xFFFF) throw std::invalid_argument("Address out of range");
    if (length < 1 || length > 4096) throw std::invalid_argument("Length must be in 1...4096");
    if (address + length > 0x10000) throw std::invalid_argument("Range exceeds the address space");
    if (source != "cpu" && source != "ram") throw std::invalid_argument("Invalid source");

    json bytes = json::array();
    {
        Suspender _(emulator);

        for (long i = 0; i < length; i++) {

            auto addr = u16(address + i);
            bytes.push_back(source == "ram" ? mem.spypeek(addr, MemType::RAM) : mem.spypeek(addr));
        }
    }

    // Render a hex dump with 16 bytes per line
    std::stringstream dump;
    for (long i = 0; i < length; i += 16) {

        char line[16];
        snprintf(line, sizeof(line), "$%04lX:", address + i);
        dump << line;

        string ascii;
        for (long j = i; j < std::min(i + 16, length); j++) {

            u8 b = bytes[size_t(j)];
            snprintf(line, sizeof(line), " %02X", b);
            dump << line;
            ascii += (b >= 32 && b < 127) ? char(b) : '.';
        }
        dump << "  " << ascii << "\n";
    }

    auto result = textResult(dump.str());
    result["structuredContent"] = {

        {"address", address},
        {"length", length},
        {"source", source},
        {"bytes", bytes}
    };
    return result;
}

json
McpServer::toolGetState(const json &)
{
    json state;
    {
        Suspender _(emulator);

        auto ci = cpu.cacheInfo();

        auto power = c64.isPoweredOff() ? "off" : c64.isPaused() ? "paused" : "running";
        auto flags = string("") + (ci.sr & 0x80 ? 'N' : '-') + (ci.sr & 0x40 ? 'V' : '-')
                   + '-' + (ci.sr & 0x10 ? 'B' : '-') + (ci.sr & 0x08 ? 'D' : '-')
                   + (ci.sr & 0x04 ? 'I' : '-') + (ci.sr & 0x02 ? 'Z' : '-') + (ci.sr & 0x01 ? 'C' : '-');

        state = {

            {"power", power},
            {"video", vic.pal() ? "PAL" : "NTSC"},
            {"frame", c64.frame},
            {"rasterLine", c64.scanline},
            {"rasterCycle", c64.rasterCycle},
            {"cpu", {
                {"pc", ci.pc},
                {"pc0", ci.pc0},
                {"sp", ci.sp},
                {"a", ci.a},
                {"x", ci.x},
                {"y", ci.y},
                {"sr", ci.sr},
                {"flags", flags},
                {"jammed", ci.jammed},
                {"cycle", ci.cycle}
            }}
        };
    }

    char line[256];
    snprintf(line, sizeof(line),
             "%s, %s, frame %llu, raster line %d cycle %d\n"
             "PC=$%04X SP=$%02X A=$%02X X=$%02X Y=$%02X SR=$%02X (%s)%s\n",
             state["power"].get<string>().c_str(), state["video"].get<string>().c_str(),
             (unsigned long long)c64.frame, int(c64.scanline), int(c64.rasterCycle),
             state["cpu"]["pc"].get<int>(), state["cpu"]["sp"].get<int>(),
             state["cpu"]["a"].get<int>(), state["cpu"]["x"].get<int>(), state["cpu"]["y"].get<int>(),
             state["cpu"]["sr"].get<int>(), state["cpu"]["flags"].get<string>().c_str(),
             state["cpu"]["jammed"].get<bool>() ? " JAMMED" : "");

    auto result = textResult(line);
    result["structuredContent"] = state;
    return result;
}

json
McpServer::toolWriteMemory(const json &args)
{
    if (!args.contains("address")) throw std::invalid_argument("Missing argument 'address'");
    if (!args.contains("bytes")) throw std::invalid_argument("Missing argument 'bytes'");

    auto address = parseAddress(args["address"]);
    auto target = args.value("target", string("cpu"));

    // The payload is either an array of integers or a string of hex digits
    std::vector<u8> data;
    if (args["bytes"].is_array()) {

        for (auto &b : args["bytes"]) {
            if (!b.is_number_integer() || b.get<long>() < 0 || b.get<long>() > 255)
                throw std::invalid_argument("Bytes must be integers in 0...255");
            data.push_back(u8(b.get<long>()));
        }

    } else if (args["bytes"].is_string()) {

        string digits;
        for (char c : args["bytes"].get<string>()) {
            if (isxdigit((unsigned char)c)) digits += c;
            else if (!isspace((unsigned char)c) && c != ',' && c != '$')
                throw std::invalid_argument("Invalid hex string");
        }
        if (digits.size() % 2) throw std::invalid_argument("Odd number of hex digits");
        for (size_t i = 0; i < digits.size(); i += 2) data.push_back(u8(std::stoi(digits.substr(i, 2), nullptr, 16)));

    } else {
        throw std::invalid_argument("'bytes' must be an array or a hex string");
    }

    if (data.empty()) throw std::invalid_argument("Nothing to write");
    if (data.size() > 4096) throw std::invalid_argument("At most 4096 bytes can be written at once");
    if (address < 0 || address + long(data.size()) > 0x10000) throw std::invalid_argument("Range exceeds the address space");
    if (target != "cpu" && target != "ram") throw std::invalid_argument("Invalid target");

    // Write on the emulator thread
    emulator.call([&] {

        for (size_t i = 0; i < data.size(); i++) {

            auto addr = u16(address + long(i));
            if (target == "ram") mem.poke(addr, data[i], MemType::RAM); else mem.poke(addr, data[i]);
        }
    });

    char line[64];
    snprintf(line, sizeof(line), "Wrote %zu byte(s) at $%04lX", data.size(), address);
    return textResult(line);
}

json
McpServer::toolTypeText(const json &args)
{
    if (!args.contains("text") || !args["text"].is_string()) {
        throw std::invalid_argument("Missing string argument 'text'");
    }
    auto text = args["text"].get<string>();
    if (text.empty()) throw std::invalid_argument("Nothing to type");

    emulator.put(Cmd::KEY_AUTOTYPE, text);

    return textResult("Typing " + std::to_string(text.size()) + " character(s)");
}

json
McpServer::toolInsertMedia(const json &args)
{
    if (!args.contains("path") || !args["path"].is_string()) {
        throw std::invalid_argument("Missing string argument 'path'");
    }

    auto path = host.makeAbsolute(args["path"].get<string>());
    if (!utl::fileExists(path)) throw std::invalid_argument("File not found: " + path.string());

    // Determine the media kind
    auto ext = path.extension().string();
    for (auto &c : ext) c = char(tolower((unsigned char)c));

    auto kind = args.value("kind", string());
    if (kind.empty()) {

        if (ext == ".d64" || ext == ".g64" || ext == ".t64" || ext == ".p00" || ext == ".prg") kind = "disk";
        else if (ext == ".tap") kind = "tape";
        else if (ext == ".crt") kind = "cartridge";
        else throw std::invalid_argument("Can't derive the media kind from extension '" + ext + "'");
    }

    auto drive = args.value("drive", 8L);
    if (drive != 8 && drive != 9) throw std::invalid_argument("Drive must be 8 or 9");

    if (kind == "disk") {

        emulator.call([&] { (drive == 9 ? drive9 : drive8).insertDisk(path, false); });
        return textResult("Inserted " + path.filename().string() + " into drive " + std::to_string(drive));
    }
    if (kind == "tape") {

        emulator.call([&] { datasette.insertTape(path); });
        return textResult("Inserted tape " + path.filename().string());
    }
    if (kind == "cartridge") {

        emulator.call([&] { expansionPort.attachCartridge(path); });
        return textResult("Attached cartridge " + path.filename().string());
    }
    if (kind == "program") {

        emulator.call([&] { c64.flash(path, 0); });
        return textResult("Flashed " + path.filename().string() + " into memory");
    }
    throw std::invalid_argument("Invalid kind");
}

json
McpServer::toolDisassemble(const json &args)
{
    auto count = args.value("count", 16L);
    if (count < 1 || count > 256) throw std::invalid_argument("Count must be in 1...256");

    json instructions = json::array();
    std::stringstream text;
    {
        Suspender _(emulator);

        long address = args.contains("address") ? parseAddress(args["address"]) : long(cpu.getPC0());
        if (address < 0 || address > 0xFFFF) throw std::invalid_argument("Address out of range");

        auto addr = u16(address);
        for (long i = 0; i < count; i++) {

            char str[256];
            auto length = cpu.disassembler.disass(str, "%i", addr, DasmStyle::hex());

            string mnemonic = str;
            mnemonic.erase(mnemonic.find_last_not_of(' ') + 1);

            json bytes = json::array();
            char hex[16];
            string dump;
            for (isize j = 0; j < length; j++) {
                auto b = cpu.readDasm(u16(addr + j));
                bytes.push_back(b);
                snprintf(hex, sizeof(hex), "%s%02X", j ? " " : "", b);
                dump += hex;
            }

            instructions.push_back({ {"address", addr}, {"bytes", bytes}, {"text", mnemonic} });

            snprintf(hex, sizeof(hex), "$%04X", addr);
            char col[32];
            snprintf(col, sizeof(col), "%-9s", dump.c_str());
            text << hex << "  " << col << " " << mnemonic << "\n";

            addr = u16(addr + length);
        }
    }

    auto result = textResult(text.str());
    result["structuredContent"] = { {"instructions", instructions} };
    return result;
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
