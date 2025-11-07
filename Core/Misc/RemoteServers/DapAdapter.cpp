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
    printf("dap::Command::BreakpointLocations: %ld\n", seq);

    replySuccess(seq, "breakpointLocations");
}

template <> void
DapAdapter::process<dap::Command::ConfigurationDone> (isize seq, const string &packet)
{
    printf("dap::Command::ConfigurationDone: %ld\n", seq);

    replySuccess(seq, "configurationDone");
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
        {"supportTerminateDebuggee", false},
        {"supportSuspendDebuggee", false},
        {"supportsDelayedStackTraceLoading", false},
        {"supportsLoadedSourcesRequest", false}, // no source files
        {"supportsLogPoints", false},
        {"supportsTerminateThreadsRequest", false},
        {"supportsSetExpression", false},
        {"supportsTerminateRequest", false},

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
    replySuccess(seq, "setBreakpoints");
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

            if (c == "breakpointLocations") { process<dap::Command::BreakpointLocations>(s, packet); }
            if (c == "configurationDone") { process<dap::Command::ConfigurationDone>(s, packet); }
            if (c == "initialize") { process<dap::Command::Initialize>(s, packet); }
            if (c == "launch") { process<dap::Command::Launch>(s, packet); }
            if (c == "setBreakpoints") { process<dap::Command::SetBreakpoints>(s, packet); }

        } else if (t == "response") {

        } else if (t == "event") {

        } else {

            throw AppError(Fault::DAP_UNRECOGNIZED_CMD, packet);
        }

    } catch (...) {

        throw AppError(Fault::DAP_UNRECOGNIZED_CMD, packet);
    }
}

void
DapAdapter::reply(const string &payload)
{
    std::stringstream ss;
    ss << "Content-Length: " << payload.size() << "\r\n\r\n" << payload;
    printf("T: %s\n", util::makePrintable(payload).c_str());
    remoteManager.dapServer.send(ss.str());
}

void
DapAdapter::replySuccess(isize seq, const string &command)
{
    json response = {

        {"type", "response"},
        {"seq", 0},
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
