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
#include "RpcServer.h"
#include "Emulator.h"
#include "json.h"
#include "httplib.h"
#include <thread>

namespace vc64 {

using nlohmann::json;

void
RpcServer::_initialize()
{
    retroShell.registerDelegate(*this);
}

void
RpcServer::_dump(Category category, std::ostream &os) const
{
    using namespace utl;

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
    switch (config.transport) {

        case TransportProtocol::STDIO:  return true;
        case TransportProtocol::TCP:    return true;
        case TransportProtocol::HTTP:   return true;

        default:
            fatalError;
    }
}

void
RpcServer::didSwitch(SrvState from, SrvState to)
{
    if (from != to) msgQueue.put(Msg::SRV_STATE, (i64)to);
}

void
RpcServer::didReceive(const string &payload)
{
    if (auto response = process(payload); response) {
        tcp << *response;
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

        // Check input format
        if (!request.contains("method")) {
            throw AppException(RPC::INVALID_REQUEST, "Missing 'method'");
        }
        if (!request.contains("params")) {
            throw AppException(RPC::INVALID_REQUEST, "Missing 'params'");
        }
        if (!request["method"].is_string()) {
            throw AppException(RPC::INVALID_PARAMS, "'method' must be a string");
        }
        if (!request["params"].is_string()) {
            throw AppException(RPC::INVALID_PARAMS, "'params' must be a string");
        }
        if (request["method"] != "retroshell") {
            throw AppException(RPC::INVALID_PARAMS, "method  must be 'retroshell'");
        }

        if (blocking) {
            return execBlocking(request["params"], request.value("id", 0));
        } else {
            return execNonBlocking(request["params"], request.value("id", 0));
        }

    } catch (const json::parse_error &) {

        json response = {

            {"jsonrpc", "2.0"},
            {"error", {{"code", RPC::PARSE_ERROR}, {"message", "Parse error: " + payload}}},
            {"id", nullptr}
        };
        return response.dump();

    } catch (const AppException &e) {

        json response = {

            {"jsonrpc", "2.0"},
            {"error", {{"code", e.data}, {"message", e.what()}}},
            {"id", nullptr}
        };
        return response.dump();
    }
}

optional<string>
RpcServer::execNonBlocking(const string &command, isize id)
{
    // Feed the command into the command queue and return a nullopt
    retroShell.asyncExec(InputLine {

        .id = id,
        .type = InputLine::Source::RPC,
        .input = command
    });

    return {};
}

optional<string>
RpcServer::execBlocking(const string &command, isize id)
{
    // To block the thread, we pass a promise to RetroShell
    auto promise = std::make_shared<std::promise<std::string>>();
    auto future = promise->get_future();

    // Feed the command, with the promise attached, into the command queue
    retroShell.asyncExec(InputLine {

        .id = id,
        .type = InputLine::Source::RPC,
        .input = command,
        .promise = promise
    });

    // Wait until the promise gets fulfilled
    return future.get();
}

void
RpcServer::didExecute(const InputLine& input, std::stringstream &ss)
{
    if (!input.isRpcCommand()) return;

    json response = {

        {"jsonrpc", "2.0"},
        {"result", ss.str()},
        {"id", input.id}
    };

    // If a promise is attached, fulfill it
    if (input.promise) { input.promise->set_value(response.dump()); }

    *this << response.dump();
}

void
RpcServer::didExecute(const InputLine& input, std::stringstream &ss, std::exception &exc)
{
    if (!input.isRpcCommand()) return;

    // By default, signal an internal error
    i64 code = RPC::INTERNAL_ERROR;

    // For parse errors, use a value from the server-defined error range
    if (dynamic_cast<const util::ParseError *>(&exc)) {
        code = RPC::SERVER_ERROR;
    }

    // For application errors, use the fault identifier
    if (const auto *error = dynamic_cast<const AppError *>(&exc)) {
        code = error->data;
    }

    json response = {

        {"jsonrpc", "2.0"},
        {"error", {
            {"code", code},
            {"message", exc.what()}
        }},
        {"id", input.id}
    };

    // If a promise is attached, fulfill it
    if (input.promise) { input.promise->set_value(response.dump()); }

    *this << response.dump();
}

}
