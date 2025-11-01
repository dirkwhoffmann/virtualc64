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
#include "DapServer.h"
#include "DapServerCmds.h"
#include "Emulator.h"
#include "CPU.h"
#include "Memory.h"
#include "MsgQueue.h"
#include "json.h"

using json = nlohmann::json;

namespace vc64 {

using dc = dap::Command;

template <> void
DapServer::process<dap::Command::BreakpointLocations> (isize seq, const string &packet)
{
    printf("dap::Command::BreakpointLocations: %ld\n", seq);

    replySuccess(seq, "breakpointLocations");
}

template <> void
DapServer::process<dap::Command::ConfigurationDone> (isize seq, const string &packet)
{
    printf("dap::Command::ConfigurationDone: %ld\n", seq);

    replySuccess(seq, "configurationDone");
}

template <> void
DapServer::process <dap::Command::Initialize> (isize seq, const string &packet)
{
    printf("dap::Command::Initialize: %ld\n", seq);

    json capabilities = {

        {"supportsConfigurationDoneRequest", true},
        {"supportsFunctionBreakpoints", false},
        {"supportsConditionalBreakpoints", false},
        {"supportsHitConditionalBreakpoints", false},
        {"supportsEvaluateForHovers", false},
        {"exceptionBreakpointFilters", json::array({})}, // TODO
        {"supportsStepBack", false},
        {"supportsSetVariable", false},
        {"supportsRestartFrame", false},
        {"supportsGotoTargetsRequest", false},
        {"supportsStepInTargetsRequest", false},
        {"supportsCompletionsRequest", false},
        {"completionTriggerCharacters", json::array({})}, // TODO
        {"supportsModulesRequest", false},
        {"additionalModuleColumns", json::array({})}, // TODO
        {"supportedChecksumAlgorithms", json::array({})}, // TODO
        {"supportsRestartRequest", false},
        {"supportsExceptionOptions", false},
        {"supportsValueFormattingOptions", false},
        {"supportsExceptionInfoRequest", false},
        {"supportTerminateDebuggee", false},
        {"supportSuspendDebuggee", false},
        {"supportsDelayedStackTraceLoading", false},
        {"supportsLoadedSourcesRequest", false},
        {"supportsLogPoints", false},
        {"supportsTerminateThreadsRequest", false},
        {"supportsSetExpression", false},
        {"supportsTerminateRequest", false},
        {"supportsDataBreakpoints", true},
        {"supportsReadMemoryRequest", true},
        {"supportsWriteMemoryRequest", true},
        {"supportsDisassembleRequest", true},
        {"supportsCancelRequest", true},
        {"supportsBreakpointLocationsRequest", true},
        {"supportsClipboardContext", false},
        {"supportsSteppingGranularity", false},
        {"supportsInstructionBreakpoints", false},
        {"supportsExceptionFilterOptions", false},
        {"supportsSingleThreadExecutionRequests", false},
        {"supportsDataBreakpointBytes", false},
        {"breakpointModes", json::array({})}, // TODO
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
DapServer::process <dap::Command::Launch> (isize seq, const string &packet)
{
    replySuccess(seq, "launch");
}

template <> void
DapServer::process <dap::Command::SetBreakpoints> (isize seq, const string &packet)
{
    replySuccess(seq, "setBreakpoints");
}

/*
    switch (cmd) {

        case dap::Command::initialize: {

            //
            // initialize
            //

            json capabilities = {

                {"supportsConfigurationDoneRequest", true},
                {"supportsFunctionBreakpoints", false},
                {"supportsConditionalBreakpoints", false},
                {"supportsHitConditionalBreakpoints", false},
                {"supportsEvaluateForHovers", false},
                {"exceptionBreakpointFilters", json::array({})}, // TODO
                {"supportsStepBack", false},
                {"supportsSetVariable", false},
                {"supportsRestartFrame", false},
                {"supportsGotoTargetsRequest", false},
                {"supportsStepInTargetsRequest", false},
                {"supportsCompletionsRequest", false},
                {"completionTriggerCharacters", json::array({})}, // TODO
                {"supportsModulesRequest", false},
                {"additionalModuleColumns", json::array({})}, // TODO
                {"supportedChecksumAlgorithms", json::array({})}, // TODO
                {"supportsRestartRequest", false},
                {"supportsExceptionOptions", false},
                {"supportsValueFormattingOptions", false},
                {"supportsExceptionInfoRequest", false},
                {"supportTerminateDebuggee", false},
                {"supportSuspendDebuggee", false},
                {"supportsDelayedStackTraceLoading", false},
                {"supportsLoadedSourcesRequest", false},
                {"supportsLogPoints", false},
                {"supportsTerminateThreadsRequest", false},
                {"supportsSetExpression", false},
                {"supportsTerminateRequest", false},
                {"supportsDataBreakpoints", true},
                {"supportsReadMemoryRequest", true},
                {"supportsWriteMemoryRequest", true},
                {"supportsDisassembleRequest", true},
                {"supportsCancelRequest", true},
                {"supportsBreakpointLocationsRequest", true},
                {"supportsClipboardContext", false},
                {"supportsSteppingGranularity", false},
                {"supportsInstructionBreakpoints", false},
                {"supportsExceptionFilterOptions", false},
                {"supportsSingleThreadExecutionRequests", false},
                {"supportsDataBreakpointBytes", false},
                {"breakpointModes", json::array({})}, // TODO
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
            break;
        }

        case dap::Command::launch: {

            //
            // launch
            //

            replySuccess(seq, "launch");
            break;
        }

        case dap::Command::setBreakpoints: {

            //
            // setBreakpoints
            //

            replySuccess(seq, "setBreakpoints");
            break;
        }

        default:
            break;
    }
}
*/

}
