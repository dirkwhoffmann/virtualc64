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

const Transport &
RshServer::transport() const
{
    return const_cast<RshServer *>(this)->transport();
}

bool
RshServer::isSupported(TransportProtocol protocol) const
{
    switch (config.transport) {

        case TransportProtocol::STDIO:  return true;
        case TransportProtocol::TCP:    return true;

        default:
            return false;
    }
}

/*
SrvState
RshServer::getState() const
{
    return transport().getState();
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
RshServer::send(const string &payload)
{
    transport().send(payload);
}
*/

void
RshServer::didSwitch(SrvState from, SrvState to)
{
    if (from != to) msgQueue.put(Msg::SRV_STATE, (i64)to);
}

void
RshServer::didConnect()
{
    try {

        *this << "VirtualC64 RetroShell Remote Server ";
        *this << C64::build() << '\n';
        *this << '\n';

        *this << "Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de" << '\n';
        *this << "https://github.com/dirkwhoffmann/virtualc64" << '\n';
        *this << '\n';

        *this << "Type 'help' for help.\n";
        *this << '\n';

        *this << retroShell.prompt();

    } catch (...) { };
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
    // Echo the command if it came from somewhere else
    if (!input.isRshCommand()) { *this << input.input << '\n'; }
}

void
RshServer::didExecute(const InputLine &input, std::stringstream &ss)
{
    *this << '\n' << ss.str() << '\n';
    *this << retroShell.prompt();
}

void
RshServer::didExecute(const InputLine &input, std::stringstream &ss, std::exception &e)
{
    // Echo the command if it came from somewhere else
    if (!input.isRpcCommand()) { *this << input.input << '\n'; }

    *this << '\n' << ss.str() << e.what() << '\n';
    *this << retroShell.prompt();
}

}
