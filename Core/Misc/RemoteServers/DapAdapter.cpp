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
#include "DapAdapter.h"
#include "DapServer.h"
#include "Emulator.h"
#include "CPU.h"
#include "IOUtils.h"
#include "Memory.h"
#include "MemUtils.h"
#include "MsgQueue.h"
#include "RetroShell.h"

using json = nlohmann::json;

namespace vc64 {

void
DapAdapter::_dump(Category category, std::ostream &os) const
{
    using namespace util;
}

template <> void
DapAdapter::process<dap::Command::BreakpointLocations> (isize seq, const string &packet)
{
    try {
        // Parse the incoming DAP message
        json request = json::parse(packet);

        // Prepare base DAP response
        json response = {

            {"type", "response"},
            {"seq", nextSeq()},
            {"request_seq", seq},
            {"command", "breakpointLocations"},
            {"success", true},
            {"body", {{"breakpoints", json::array()}}}
        };

        // Extract arguments if present
        const auto &args = request["arguments"];
        std::string path;
        int startLine = -1;
        int endLine   = -1;

        if (args.contains("source") && args["source"].contains("path"))
            path = args["source"]["path"].get<std::string>();
        if (args.contains("line"))
            startLine = args["line"].get<int>();
        if (args.contains("endLine"))
            endLine = args["endLine"].get<int>();
        if (endLine < 0)
            endLine = startLine;

        // Create BreakpointLocations object
        BreakpointLocations locations;

        // In the future, you could fill 'locations' based on .sym file info
        // For now, just add placeholder entries for the requested range
        for (int line = startLine; line <= endLine; line++) {

            BreakpointLocation loc;
            loc.line = line;
            loc.column = 1;
            loc.endLine = line;
            loc.endColumn = 80;
            locations.locations.push_back(loc);
        }

        // Serialize into DAP response
        for (const auto &loc : locations.locations) {

            json locJson = {
                {"line", loc.line},
                {"column", loc.column},
                {"endLine", loc.endLine},
                {"endColumn", loc.endColumn}
            };
            response["body"]["breakpoints"].push_back(locJson);
        }

        // Send back
        reply(response.dump());

    } catch (const std::exception &e) {

        // On any parsing or logic error, send a failed response
        json errorResponse = {
            {"type", "response"},
            {"seq", nextSeq()},
            {"request_seq", seq},
            {"command", "breakpointLocations"},
            {"success", false},
            {"message", std::string("Failed to process BreakpointLocations: ") + e.what()}
        };
        
        reply(errorResponse.dump());
    }
}

template <> void
DapAdapter::process<dap::Command::ConfigurationDone> (isize seq, const string &packet)
{
    printf("dap::Command::ConfigurationDone: %ld\n", seq);

    replySuccess(seq, "configurationDone");
}

template <> void
DapAdapter::process<dap::Command::Disconnect> (isize seq, const string &packet)
{
    printf("dap::Command::Disconnect: %ld\n", seq);

    replySuccess(seq, "disconnect");

    json response = {

        {"type", "event"},
        {"event", "terminated"}
    };

    reply(response.dump());

    // remoteManager.dapServer.disconnect();
    // sleep(1);
}

template <> void
DapAdapter::process <dap::Command::Initialize> (isize seq, const string &packet)
{
    printf("dap::Command::Initialize: %ld\n", seq);

    json capabilities = {

        {"supportsConfigurationDoneRequest", true},
        {"supportsFunctionBreakpoints", false},
        {"supportsConditionalBreakpoints", false},
        {"supportsHitConditionalBreakpoints", false},
        {"supportsEvaluateForHovers", false},
        {"exceptionBreakpointFilters", json::array({})},

        {"supportsStepBack", false},
        {"supportsSetVariable", false},
        {"supportsRestartFrame", false},
        {"supportsGotoTargetsRequest", false},
        {"supportsStepInTargetsRequest", false},
        {"supportsCompletionsRequest", false},

        {"supportsModulesRequest", false},
        {"supportsRestartRequest", false},
        {"supportsExceptionOptions", false},
        {"supportsValueFormattingOptions", false},
        {"supportsExceptionInfoRequest", false},

        // Important for launch tests
        {"supportsLaunchRequest", true},
        {"supportsTerminateRequest", true},
        {"supportSuspendDebuggee", false},  // optional, leave as false

        {"supportsDelayedStackTraceLoading", false},
        {"supportsLoadedSourcesRequest", false}, // no source files
        {"supportsLogPoints", false},
        {"supportsTerminateThreadsRequest", false},
        {"supportsSetExpression", false},

        // Features for symbol-level debugging
        {"supportsDataBreakpoints", true},
        {"supportsReadMemoryRequest", true},
        {"supportsWriteMemoryRequest", true},
        {"supportsDisassembleRequest", true},
        {"supportsCancelRequest", true},
        {"supportsBreakpointLocationsRequest", true},
        {"supportsInstructionBreakpoints", true},

        {"supportsClipboardContext", false},
        {"supportsSteppingGranularity", false},
        {"supportsExceptionFilterOptions", false},
        {"supportsSingleThreadExecutionRequests", false},
        {"supportsDataBreakpointBytes", false},
        {"breakpointModes", json::array({})},
        {"supportsANSIStyling", true}
    };

    json response = {

        {"seq", 0},
        {"type", "response"},
        {"request_seq", seq},
        {"success", true},
        {"command", "initialize"},
        {"body", capabilities }
    };
    reply(response.dump());

    json response2 = {

        {"seq", 0},
        {"type", "event"},
        {"event", "initialized"}
    };
    reply(response2.dump());

}

template <> void
DapAdapter::process <dap::Command::Launch> (isize seq, const string &packet)
{
    replySuccess(seq, "launch");
}

template <> void
DapAdapter::process <dap::Command::SetBreakpoints> (isize seq, const string &packet)
{
    try {

        json request = json::parse(packet);

        // Prepare default response
        json response = {
            
            {"type", "response"},
            {"seq", nextSeq()},
            {"request_seq", seq},
            {"command", "setBreakpoints"},
            {"success", true},
            {"body", {{"breakpoints", json::array()}}}
        };

        // Extract breakpoints
        if (request.contains("arguments") && request["arguments"].contains("breakpoints")) {

            const auto &bpArray = request["arguments"]["breakpoints"];

            // Parse into our structured type
            SourceBreakpoints sourceBreakpoints(bpArray);

            // Build DAP response for each breakpoint
            for (const auto &bp : sourceBreakpoints.breakpoints) {
                json bpEntry = {

                    {"verified", false},
                    {"line", bp.line.value_or(-1)},
                    {"message", "Source breakpoints not yet supported"}
                };
                response["body"]["breakpoints"].push_back(bpEntry);
            }
        }

        reply(response.dump());

    } catch (const std::exception &e) {

        json errorResponse = {

            {"type", "response"},
            {"seq", nextSeq()},
            {"request_seq", seq},
            {"command", "setBreakpoints"},
            {"success", false},
            {"message", std::string("Failed to process SetBreakpoints: ") + e.what()}
        };

        reply(errorResponse.dump());
    }
}

template <> void
DapAdapter::process <dap::Command::SetExceptionBreakpoints> (isize seq, const string &packet)
{
    replySuccess(seq, "SetExceptionBreakpoints");
}

void
DapAdapter::process(const string &packet)
{
    json j = json::parse(packet);
    isize s = j.value("seq", 0);
    string t = j.value("type", "");
    string c = j.value("command", "");

    try {

        if (t == "request") {

            if (c == "breakpointLocations") {
                process<dap::Command::BreakpointLocations>(s, packet);
            } else if (c == "configurationDone") {
                process<dap::Command::ConfigurationDone>(s, packet);
            } else if (c == "disconnect") {
                process<dap::Command::Disconnect>(s, packet);
            } else if (c == "initialize") {
                process<dap::Command::Initialize>(s, packet);
            } else if (c == "launch") {
                process<dap::Command::Launch>(s, packet);
            } else if (c == "setBreakpoints") {
                process<dap::Command::SetBreakpoints>(s, packet);
            } else if (c == "setExceptionBreakpoints") {
                process<dap::Command::SetExceptionBreakpoints>(s, packet);
            } else if (t == "response") {

            } else if (t == "event") {

            } else {
                throw AppError(Fault::DAP_UNRECOGNIZED_CMD, packet);
            }
        }

    } catch (...) {

        throw AppError(Fault::DAP_UNRECOGNIZED_CMD, packet);
    }
}

void
DapAdapter::reply(const string &payload)
{
    remoteManager.dapServer.reply(payload);
}

void
DapAdapter::replySuccess(isize seq, const string &command)
{
    json response = {

        {"type", "response"},
        {"seq", nextSeq()},
        {"request_seq", seq},
        {"command", command},
        {"success", true}
    };

    reply(response.dump());
}

string
DapAdapter::readRegister(isize nr)
{
    switch (nr) {

        case 0: return util::hexstr<2>(c64.cpu.reg.a);
        case 1: return util::hexstr<2>(c64.cpu.reg.x);
        case 2: return util::hexstr<2>(c64.cpu.reg.y);
        case 3: return util::hexstr<2>(c64.cpu.reg.sp);
        case 4: return util::hexstr<2>(c64.cpu.getP());
        case 5: return util::hexstr<4>(c64.cpu.getPC0());
        default: return "00";
    }
}

string
DapAdapter::readMemory(isize addr)
{
    auto byte = mem.spypeek((u16)addr);
    return util::hexstr <2> (byte);
}

void
DapAdapter::breakpointReached()
{
    debug(DAP_DEBUG, "breakpointReached()\n");
}

}
