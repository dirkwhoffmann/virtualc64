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
    using namespace util;

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

    /*
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

        // Feed the command into the command queue
        retroShell.asyncExec(InputLine {

            .id = request.value("id", 0),
            .type = InputLine::Source::RPC,
            .input = request["params"] });

    } catch (const json::parse_error &) {

        json response = {

            {"jsonrpc", "2.0"},
            {"error", {{"code", RPC::PARSE_ERROR}, {"message", "Parse error: " + payload}}},
            {"id", nullptr}
        };
        tcp.send(response.dump());

    } catch (const AppException &e) {

        json response = {

            {"jsonrpc", "2.0"},
            {"error", {{"code", e.data}, {"message", e.what()}}},
            {"id", nullptr}
        };
        tcp.send(response.dump());
    }
    */
}

void
RpcServer::didReceive(const struct httplib::Request &req, struct httplib::Response &res)
{
    if (auto response = process(req.body); response) {
        res.set_content(*response, "text/plain");
    }
}

optional<string>
RpcServer::process(const string &payload)
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

        if (config.transport == TransportProtocol::HTTP) {

            return execBlocking(request["params"], request.value("id", 0));

        } else {

            execNonBlocking(request["params"], request.value("id", 0));
            return {};
        }

        /*
        // Create shared promise
        auto promise = std::make_shared<std::promise<std::string>>();
        auto future = promise->get_future();

        // Feed the command into the command queue
        retroShell.asyncExec(InputLine {

            .id = request.value("id", 0),
            .type = InputLine::Source::RPC,
            .input = request["params"],
            .promise = promise
        });

        printf("Waiting for the result...\n");
        auto result = future.get();
        printf("Got result %s\n", result.c_str());

        return result;
        */

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

void
RpcServer::execNonBlocking(const string &command, isize id)
{
    // Feed the command into the command queue
    retroShell.asyncExec(InputLine {

        .id = id,
        .type = InputLine::Source::RPC,
        .input = command
    });
}

string
RpcServer::execBlocking(const string &command, isize id)
{
    // To block the thread, we pass a promise to RetroShell
    auto promise = std::make_shared<std::promise<std::string>>();
    auto future = promise->get_future();

    // Feed the command into the command queue
    retroShell.asyncExec(InputLine {

        .id = id,
        .type = InputLine::Source::RPC,
        .input = command,
        .promise = promise
    });

    // Wait for the result
    return future.get();
}

/*
string
RpcServer::respond(const httplib::Request& payload)
{
    try {

        printf("body = %s\n", payload.body.c_str());

        json request = json::parse(payload.body);

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

        // Create shared promise
        auto promise = std::make_shared<std::promise<std::string>>();
        auto future = promise->get_future();

        // Feed the command into the command queue
        retroShell.asyncExec(InputLine {

            .id = request.value("id", 0),
            .type = InputLine::Source::RPC,
            .input = request["params"],
            .promise = promise
        });

        printf("Waiting for the result...\n");
        auto result = future.get();
        printf("Got result %s\n", result.c_str());

        return result;

    } catch (const json::parse_error &) {

        json response = {

            {"jsonrpc", "2.0"},
            {"error", {{"code", RPC::PARSE_ERROR}, {"message", "Parse error: " + payload.body}}},
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
*/

void
RpcServer::didExecute(const InputLine& input, std::stringstream &ss)
{
    if (!input.isRpcCommand()) return;

    json response = {

        {"jsonrpc", "2.0"},
        {"result", ss.str()},
        {"id", input.id}
    };

    // In HTTP mode, return the shell output through the promise
    if (config.transport == TransportProtocol::HTTP) {
        if (input.promise) input.promise->set_value(response.dump());
    } else {
        tcp << response.dump();
    }
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

    // In HTTP mode, return the shell output through the promise
    if (config.transport == TransportProtocol::HTTP) {
        if (input.promise) input.promise->set_value(response.dump());
    } else {
        tcp << response.dump();
    }
}

}
