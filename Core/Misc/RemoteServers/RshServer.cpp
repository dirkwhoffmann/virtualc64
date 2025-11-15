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

void
RshServer::checkOption(Opt opt, i64 value)
{
    if (opt == Opt::SRV_TRANSPORT && value == i64(TransportProtocol::HTTP)) {
        throw AppError(Fault::OPT_UNSUPPORTED, "This server requires a raw TCP connection.");
    }

    RemoteServer::checkOption(opt, value);
}

void
RshServer::switchState(SrvState newState)
{
    tcp.switchState(newState);
    
    // Inform the GUI
    msgQueue.put(Msg::SRV_STATE, (i64)newState);
}

void
RshServer::start()
{
    tcp.start(config.port);
}

void
RshServer::stop()
{
    tcp.stop();
}

void
RshServer::disconnect()
{
    tcp.disconnect();
}

void
RshServer::main()
{
    tcp.main(config.port);
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
    if (config.verbose) {
        
        try {

            tcp << "VirtualC64 RetroShell Remote Server ";
            tcp << C64::build() << '\n';
            tcp << '\n';

            tcp << "Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de" << '\n';
            tcp << "https://github.com/dirkwhoffmann/virtualc64" << '\n';
            tcp << '\n';

            tcp << "Type 'help' for help.\n";
            tcp << '\n';

            tcp << retroShell.prompt();

        } catch (...) { };
    }
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

/*
string
RshServer::doReceive()
{
    string payload = connection.recv();
    
    // Remove LF and CR (if present)
    payload = util::rtrim(payload, "\n\r");

    // Ask the client to delete the input (will be replicated by RetroShell)
    // connection.send("\033[A\33[2K\r");

    return payload;
}
*/

/*
void
RshServer::doSend(const string &payload)
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
    
    connection.send(mapped);
}
*/

/*
void
RshServer::doProcess(const string &payload)
{
    retroShell.asyncExec(InputLine {

        .type = InputLine::Source::RSH,
        .input = payload
    });
}
*/

void
RshServer::willExecute(const InputLine &input)
{
    // Echo the command if it came from somewhere else
    if (!input.isRshCommand()) {tcp << input.input << '\n'; }
}

void
RshServer::didExecute(const InputLine &input, std::stringstream &ss)
{
    tcp << '\n' << ss.str() << '\n';
    tcp << retroShell.prompt();
}

void
RshServer::didExecute(const InputLine &input, std::stringstream &ss, std::exception &e)
{
    // Echo the command if it came from somewhere else
    if (!input.isRpcCommand()) { tcp << input.input << '\n'; }

    tcp << '\n' << ss.str() << e.what() << '\n';
    tcp << retroShell.prompt();
}

}
