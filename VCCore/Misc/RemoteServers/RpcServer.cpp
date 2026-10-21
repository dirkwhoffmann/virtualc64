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
#include "RpcServer.h"
#include "Emulator.h"
#include "json.h"
#include "httplib.h"
#include "utl/support.h"
#include <thread>

namespace vc64 {

using nlohmann::json;

void
RpcServer::_initialize()
{
    rpcShell.console.delegates.push_back(this);
}

void
RpcServer::_dump(Category category, std::ostream &os) const
{
    RemoteServer::_dump(category, os);
}

Transport &
RpcServer::transport()
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
RpcServer::transport() const
{
    return const_cast<RpcServer *>(this)->transport();
}

bool
RpcServer::isSupported(TransportProtocol protocol) const
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
RpcServer::didStart()
{
    if (config.verbose) {

        *this << "Remote server is listening at port " << config.port << "\n";
    }
}

void
RpcServer::send(const string &payload)
{
    transport().send(payload);

    if (config.verbose) {

        retroShell << "T: " << utl::makePrintable(payload) << "\n";
        printf("T: %s\n", utl::makePrintable(payload).c_str());
    }
}

void
RpcServer::didReceive(const string &payload)
{
    // Remove LF and CR (if present)
    auto trimmed = utl::rtrim(payload, "\n\r");

    // Ignore blank packets (e.g., stray newlines on the stdio transport)
    if (trimmed.find_first_not_of(" \t") == string::npos) return;

    if (config.verbose) {

        retroShell << "R: " << utl::makePrintable(trimmed) << "\n";
        printf("R: %s\n", utl::makePrintable(trimmed).c_str());
    }

    if (auto response = process(trimmed, false); response) {

        // The newline allows clients to process the stream line by line
        send(*response + "\n");
    }
}

void
RpcServer::didReceive(const struct httplib::Request &req, struct httplib::Response &res)
{
    if (auto response = process(req.body, true); response) {
        res.set_content(*response, "text/plain");
    }
}

optional<string>
RpcServer::process(const string &payload, bool blocking)
{
    try {

        json request = json::parse(payload);

        // Check the envelope
        if (!request.contains("method")) {
            throw utl::Error(RPC::INVALID_REQUEST, "Missing 'method'");
        }
        if (!request["method"].is_string()) {
            throw utl::Error(RPC::INVALID_PARAMS, "'method' must be a string");
        }
        if (request["method"] == "retroshell") {

            if (!request.contains("params")) {
                throw utl::Error(RPC::INVALID_REQUEST, "Missing 'params'");
            }
            if (!request["params"].is_string()) {
                throw utl::Error(RPC::INVALID_PARAMS, "'params' must be a string");
            }

            auto id = request.value("id", 0);

            if (blocking) {
                return execBlocking(request["params"], id);
            } else {
                return execNonBlocking(request["params"], id);
            }
        }

        // Any other method is not handled by the core
        return { };

    } catch (const json::parse_error &) {

        json response = {

            {"jsonrpc", "2.0"},
            {"error", {{"code", RPC::PARSE_ERROR}, {"message", "Parse error: " + payload}}},
            {"id", nullptr}
        };
        return response.dump();

    } catch (const utl::Error &e) {

        json response = {

            {"jsonrpc", "2.0"},
            {"error", {{"code", e.payload}, {"message", e.what()}}},
            {"id", nullptr}
        };
        return response.dump();
    }
}

optional<string>
RpcServer::execNonBlocking(const string &command, isize id)
{
    // Feed the command into the command queue and return a nullopt
    rpcShell.asyncExec(InputLine {

        .id = id,
        .type = InputLine::Source::RPC,
        .input = command
    });

    return {};
}

optional<string>
RpcServer::execBlocking(const string &command, isize id)
{
    // To block the thread, we attach a completion handler fulfilling a promise
    auto promise = std::make_shared<std::promise<std::string>>();
    auto future = promise->get_future();

    // Feed the command, with the handler attached, into the command queue
    rpcShell.asyncExec(InputLine {

        .id = id,
        .type = InputLine::Source::RPC,
        .input = command,
        .completion = [this, promise, id](const string &output, std::exception_ptr error) {

            promise->set_value(error ? makeError(id, error) : makeResult(id, output));
        }
    });

    // Wait until the promise gets fulfilled
    return future.get();
}

string
RpcServer::makeResult(isize id, const string &output)
{
    json response = {

        {"jsonrpc", "2.0"},
        {"result", output},
        {"id", id}
    };

    return response.dump();
}

string
RpcServer::makeError(isize id, std::exception_ptr error)
{
    // By default, signal an internal error
    i64 code = RPC::INTERNAL_ERROR;
    string message = "Unknown error";

    try { std::rethrow_exception(error); } catch (std::exception &exc) {

        message = exc.what();

        // For parse errors, use a value from the server-defined error range
        if (dynamic_cast<const utl::ParseError *>(&exc)) {
            code = RPC::SERVER_ERROR;
        }

        // For application errors, use the fault identifier
        if (const auto *e = dynamic_cast<const CoreError *>(&exc)) {
            code = i64(e->fault());
        }

    } catch (...) { }

    json response = {

        {"jsonrpc", "2.0"},
        {"error", {
            {"code", code},
            {"message", message}
        }},
        {"id", id}
    };

    return response.dump();
}

void
RpcServer::didExecute(const InputLine& input, std::stringstream &ss)
{
    if (!input.isRpcCommand()) return;

    send(makeResult(input.id, ss.str()) + "\n");
}

void
RpcServer::didExecute(const InputLine& input, std::stringstream &ss, std::exception &exc)
{
    if (!input.isRpcCommand()) return;

    // Called from within the catch block of the failed command
    send(makeError(input.id, std::current_exception()) + "\n");
}

}
