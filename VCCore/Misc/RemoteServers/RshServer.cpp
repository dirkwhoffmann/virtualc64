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
#include "RshServer.h"
#include "C64.h"
#include "RetroShell.h"
#include "httplib.h"
#include "utl/support.h"

namespace vc64 {

void
RshServer::_initialize()
{
    // The remote server drives its own shell, independent of the GUI's one
    rshShell.console.delegates.push_back(this);
}

void
RshServer::_dump(Category category, std::ostream &os) const
{
    RemoteServer::_dump(category, os);
}

Transport &
RshServer::transport()
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
RshServer::transport() const
{
    return const_cast<RshServer *>(this)->transport();
}

bool
RshServer::isSupported(TransportProtocol protocol) const
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
RshServer::didStart()
{
    if (config.verbose) {

        *this << "Remote server is listening at port " << config.port << "\n";
    }
}

void
RshServer::didConnect()
{
    // Hand the client a fresh shell
    rshShell.newSession();

    if (config.verbose) {

        try {

            *this << "VirtualC64 RetroShell Remote Server ";
            *this << C64::build() << '\n';
            *this << '\n';

            *this << "Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de" << '\n';
            *this << "https://github.com/dirkwhoffmann/virtualc64" << '\n';
            *this << '\n';

            *this << "Type 'help' for help.\n";
            *this << '\n';

            *this << rshShell.prompt();

        } catch (...) { };
    }
}

void
RshServer::didDisconnect()
{
    // Discard the client's session
    rshShell.newSession();
}

void
RshServer::didReceive(const string &payload)
{
    // Remove LF and CR (if present)
    auto trimmed = utl::rtrim(payload, "\n\r");

    rshShell.asyncExec(InputLine {

        .type = InputLine::Source::RSH,
        .input = trimmed
    });
}

void
RshServer::didReceive(const httplib::Request &req, httplib::Response &res)
{
    // Remove LF and CR (if present)
    auto trimmed = utl::rtrim(req.body, "\n\r");

    // HTTP has no persistent session to stream output into, so the command
    // is executed synchronously: a promise is attached to the input line
    // and fulfilled from didExecute, and this function blocks on it.
    auto promise = std::make_shared<std::promise<string>>();
    auto future = promise->get_future();

    rshShell.asyncExec(InputLine {

        .type = InputLine::Source::RSH,
        .input = trimmed,
        .promise = promise
    });

    res.set_content(future.get(), "text/plain");
}

void
RshServer::send(const string &payload)
{
    string mapped;

    for (auto c : payload) {

        switch (c) {

            case '\r':

                mapped += "\33[2K\r";
                break;

            case '\n':

                mapped += "\n";
                break;

            default:

                if (isprint(c)) mapped += c;
                break;
        }
    }

    transport().send(mapped);
}

void
RshServer::didActivate()
{

}

void
RshServer::didDeactivate()
{

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
    // If a promise is attached (HTTP), fulfill it instead of streaming
    if (input.promise) { input.promise->set_value(ss.str()); return; }

    *this << '\n' << ss.str() << '\n';
    *this << rshShell.prompt();
}

void
RshServer::didExecute(const InputLine &input, std::stringstream &ss, std::exception &e)
{
    // If a promise is attached (HTTP), fulfill it instead of streaming
    if (input.promise) { input.promise->set_value(ss.str() + e.what()); return; }

    // Echo the command if it came from somewhere else
    if (!input.isRshCommand()) { *this << input.input << '\n'; }

    *this << '\n' << ss.str() << e.what() << '\n';
    *this << rshShell.prompt();
}

}
