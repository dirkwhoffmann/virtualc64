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
#include <thread>

namespace vc64 {

using nlohmann::json;

void
RpcServer::_dump(Category category, std::ostream &os) const
{
    using namespace util;

    RemoteServer::_dump(category, os);
}

void
RpcServer::didStart()
{
    if (config.verbose) {

        *this << "Remote server is listening at port " << config.port << "\n";
    }
}

string
RpcServer::doReceive()
{
    string payload = connection.recv();

    // Remove LF and CR (if present)
    payload = util::rtrim(payload, "\n\r");

    if (config.verbose) {

        retroShell << "R: " << util::makePrintable(payload) << "\n";
        printf("R: %s\n", util::makePrintable(payload).c_str());
    }

    return payload;
}

void
RpcServer::doSend(const string &payload)
{
    connection.send(payload);

    if (config.verbose) {

        retroShell << "T: " << util::makePrintable(payload) << "\n";
        printf("T: %s\n", util::makePrintable(payload).c_str());
    }
}

void
RpcServer::doProcess(const string &payload)
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

        // Extract required fields
        string method = request["method"];
        string params = request["params"];
        int id = request.value("id", 0);

        // Feed the command into the command queue
        retroShell.asyncExec(InputLine {

            .id = id,
            .type = InputLine::Source::RPC,
            .echo = true,
            .input = params });

    } catch (const json::parse_error &exc) {

        json response = {

            {"jsonrpc", "2.0"},
            {"error", {{"code", RPC::PARSE_ERROR}, {"message", payload}}},
            {"id", nullptr}
        };
        send(response.dump());

    } catch (const AppException &exc) {

        json response = {

            {"jsonrpc", "2.0"},
            {"error", {{"code", exc.data}, {"message", exc.what()}}},
            {"id", nullptr}
        };
        send(response.dump());
    }
}

void
RpcServer::reply(const InputLine& input, std::stringstream &ss)
{
    if (!input.isRpcCommand()) return;

    json response = {

        {"jsonrpc", "2.0"},
        {"result", ss.str()},
        {"id", input.id}
    };

    send(response.dump());
}

void
RpcServer::reply(const InputLine& input, std::stringstream &ss, std::exception &exc)
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

    send(response.dump());
}

}
