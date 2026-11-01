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
#include "RemoteServer.h"
#include "RemoteManager.h"
#include "Emulator.h"
#include "MsgQueue.h"
#include "RetroShell.h"

namespace vc64 {

RemoteServer::RemoteServer(C64& ref, isize id) : SubComponent(ref, id)
{
    info.bind([this] { return cacheInfo(); } );
}

void
RemoteServer::_dump(Category category, std::ostream &os) const
{
    using namespace utl;

    if (category == Category::Config) {
        
        dumpConfig(os);
    }
    
    if (category == Category::State) {
        
        os << tab("State");
        os << SrvStateEnum::key(getState()) << std::endl;
    }
}

void
RemoteServer::_powerOff()
{
    
}

void
RemoteServer::_didLoad()
{
    // Stop the server (will be restarted by the launch daemon in auto-run mode)
    stop();
}

i64
RemoteServer::getOption(Opt option) const
{
    switch (option) {
            
        case Opt::SRV_ENABLE:       return (i64)config.enable;
        case Opt::SRV_PORT:         return (i64)config.port;
        case Opt::SRV_TRANSPORT:    return (i64)config.transport;
        case Opt::SRV_VERBOSE:      return (i64)config.verbose;

        default:
            fatalError;
    }
}

void
RemoteServer::checkOption(Opt opt, i64 value)
{
    switch (opt) {

        case Opt::SRV_ENABLE:
        case Opt::SRV_PORT:
        case Opt::SRV_VERBOSE:

            return;

        case Opt::SRV_TRANSPORT:

            if (!isSupported(TransportProtocol(value))) {

                auto name = string(TransportProtocolEnum::key(TransportProtocol(value)));
                throw CoreError(CoreError::OPT_UNSUPPORTED, "Unsupported transport: " + name);
            }
            return;

        default:
            throw CoreError(CoreError::OPT_UNSUPPORTED);
    }
}

void
RemoteServer::setOption(Opt opt, i64 value)
{
    checkOption(opt, value);

    switch (opt) {

        case Opt::SRV_ENABLE:

            config.enable = (bool)value;
            return;

        case Opt::SRV_PORT:
            
            if (config.port != (u16)value) {
                
                if (isOff()) {

                    config.port = (u16)value;

                } else {

                    stop();
                    config.port = (u16)value;
                    start();
                }
            }
            return;
            
        case Opt::SRV_TRANSPORT:

            if (config.transport != (TransportProtocol)value) {

                if (isOff()) {

                    config.transport = (TransportProtocol)value;

                } else {

                    stop();
                    config.transport = (TransportProtocol)value;
                    start();
                }
            }
            return;

        case Opt::SRV_VERBOSE:
            
            config.verbose = (bool)value;
            return;

        default:
            fatalError;
    }
}

RemoteServerInfo
RemoteServer::cacheInfo() const
{
    RemoteServerInfo info;

    info.state = getState();

    return info;
}

void
RemoteServer::start()
{
    transport().start(config.port, endpoint());
}

void
RemoteServer::stop()
{
    if (isOff() || isStopping()) return;

    transport().stop();
}

void
RemoteServer::waitForLaunch()
{
    transport().wait();
}

void
RemoteServer::recordTraffic(TrafficDirection direction, const string &payload)
{
    remoteManager.recordTraffic(ServerType(objid), direction, payload);
}

void
RemoteServer::didSwitch(SrvState from, SrvState to)
{
    if (from != to) msgQueue.put(Msg::SRV_STATE, (i64)to);
}

void
RemoteServer::didTerminate(const string &error)
{
    transport().switchState(SrvState::INVALID);
    retroShell << "Server Error: " << error << '\n';
}

void
RemoteServer::send(char payload)
{
    send(string(1, payload));
}

void
RemoteServer::send(int payload)
{
    send(std::to_string(payload));
}

void
RemoteServer::send(long payload)
{
    send(std::to_string(payload));
}

void
RemoteServer::send(std::stringstream &payload)
{
    string line;
    while(std::getline(payload, line)) {
        send(line + "\n");
    }
}

}
