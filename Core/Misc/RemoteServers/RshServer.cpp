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
#include "RshServer.h"
#include "C64.h"
#include "RetroShell.h"
#include "StringUtils.h"

namespace vc64 {

Transport &
RshServer::transport()
{
    switch (config.transport) {

        case TransportProtocol::STDIO: return stdio;
        case TransportProtocol::TCP:   return tcp;

        default:
            fatalError;
    }
}

void
RshServer::_initialize()
{
    retroShell.registerDelegate(*this);
}

void
RshServer::_dump(Category category, std::ostream &os) const
{
    using namespace util;
    
    RemoteServer::_dump(category, os);
}

void
RshServer::checkOption(Opt opt, i64 value)
{
    if (opt == Opt::SRV_TRANSPORT) {

        if (value == i64(TransportProtocol::HTTP)) {
            throw AppError(Fault::OPT_UNSUPPORTED, "Unsupported protocol: HTTP");
        }
    }

    RemoteServer::checkOption(opt, value);
}

void
RshServer::didSwitch(SrvState from, SrvState to)
{
    if (from != to) msgQueue.put(Msg::SRV_STATE, (i64)to);
}

void
RshServer::start()
{
    transport().start(config.port);
}

void
RshServer::stop()
{
    transport().stop();
}

void
RshServer::disconnect()
{
    transport().disconnect();
}

void
RshServer::didStart()
{

}

void
RshServer::didStop()
{

}

void
RshServer::didConnect()
{
    auto &out = transport();

    try {

        out << "VirtualC64 RetroShell Remote Server ";
        out << C64::build() << '\n';
        out << '\n';

        out << "Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de" << '\n';
        out << "https://github.com/dirkwhoffmann/virtualc64" << '\n';
        out << '\n';

        out << "Type 'help' for help.\n";
        out << '\n';

        out << retroShell.prompt();

    } catch (...) { };
}

void
RshServer::didDisconnect()
{
    
}

void
RshServer::didReceive(const string &payload)
{
    // Remove LF and CR (if present)
    auto trimmed = util::rtrim(payload, "\n\r");

    debug(1, "didReceive: %s\n", trimmed.c_str());

    retroShell.asyncExec(InputLine {

        .type = InputLine::Source::RSH,
        .input = trimmed
    });
}

void
RshServer::willExecute(const InputLine &input)
{
    auto &out = transport();

    // Echo the command if it came from somewhere else
    if (!input.isRshCommand()) { out << input.input << '\n'; }
}

void
RshServer::didExecute(const InputLine &input, std::stringstream &ss)
{
    auto &out = transport();

    out << '\n' << ss.str() << '\n';
    out << retroShell.prompt();
}

void
RshServer::didExecute(const InputLine &input, std::stringstream &ss, std::exception &e)
{
    auto &out = transport();

    // Echo the command if it came from somewhere else
    if (!input.isRpcCommand()) { out << input.input << '\n'; }

    out << '\n' << ss.str() << e.what() << '\n';
    out << retroShell.prompt();
}

}
