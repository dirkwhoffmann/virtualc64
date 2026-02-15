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

#if 0

#include "config.h"
#include "RpcHttpServer.h"
#include "Emulator.h"
#include "httplib.h"
#include "json.h"
#include <thread>

namespace vc64 {

using nlohmann::json;

void
RpcHttpServer::_initialize()
{
    retroShell.registerDelegate(*this);
}

void
RpcHttpServer::_dump(Category category, std::ostream &os) const
{
    using namespace utl;

    HttpServer::_dump(category, os);
}

void
RpcHttpServer::checkOption(Opt opt, i64 value)
{
    if (opt == Opt::SRV_TRANSPORT && value == i64(TransportProtocol::TCP)) {
        throw AppError(Fault::OPT_UNSUPPORTED, "This server requires a HTTP connection.");
    }

    RemoteServer::checkOption(opt, value);
}

string
RpcHttpServer::respond(const httplib::Request& payload)
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

void
RpcHttpServer::main()
{
    try {

        // Create the HTTP server
        if (!srv) srv = new httplib::Server();

        // Setup the endpoint
        srv->Post("/jsonrpc", [this](const httplib::Request& req, httplib::Response& res) {

            switchState(SrvState::CONNECTED);
            res.set_content(respond(req), "text/plain");
        });

        // Start the server to listen on localhost
        loginfo(SRV_DEBUG, "Starting RPC HTTP data provider\n");
        srv->listen("localhost", (int)config.port);

    } catch (std::exception &err) {

        loginfo(SRV_DEBUG, "Server thread interrupted\n");
        handleError(err.what());
    }
}

void
RpcHttpServer::willExecute(const InputLine &input)
{

}

void
RpcHttpServer::didExecute(const InputLine& input, std::stringstream &ss)
{
    if (!input.isRpcCommand()) return;

    json response = {

        {"jsonrpc", "2.0"},
        {"result", ss.str()},
        {"id", input.id}
    };

    printf("Response: %s\n", response.dump().c_str());

    // Return the shell output through the promise
    if (input.promise) input.promise->set_value(response.dump());
}

void
RpcHttpServer::didExecute(const InputLine& input, std::stringstream &ss, std::exception &exc)
{
    if (!input.isRpcCommand()) return;

    // By default, signal an internal error
    i64 code = -32603;

    // For parse errors, use a value from the server-defined error range
    if (dynamic_cast<const util::ParseError *>(&exc)) {
        code = -32000;
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

    printf("Error response: %s\n", response.dump().c_str());

    // Return the shell output through the promise
    if (input.promise) input.promise->set_value(response.dump());

}

}

#endif
