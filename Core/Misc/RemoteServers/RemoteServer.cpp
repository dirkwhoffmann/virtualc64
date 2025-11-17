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
#include "RemoteServer.h"
#include "Emulator.h"

namespace vc64 {

void
RemoteServer::_dump(Category category, std::ostream &os) const
{
    using namespace util;

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
    try { stop(); } catch(...) { }
}

void
RemoteServer::_didLoad()
{
    config.enable ? start() : stop();
}

i64
RemoteServer::getOption(Opt option) const
{
    switch (option) {
            
        case Opt::SRV_ENABLE:    return config.enable;
        case Opt::SRV_PORT:      return config.port;
        case Opt::SRV_TRANSPORT:  return (i64)config.transport;
        case Opt::SRV_VERBOSE:   return config.verbose;

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
        case Opt::SRV_TRANSPORT:
        case Opt::SRV_VERBOSE:

            return;

        default:
            throw AppError(Fault::OPT_UNSUPPORTED);
    }
}

void
RemoteServer::setOption(Opt opt, i64 value)
{
    checkOption(opt, value);

    switch (opt) {

        case Opt::SRV_ENABLE:

            config.enable = (bool)value;
            config.enable ? start() : stop();
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
            
            config.transport = (TransportProtocol)value;
            return;

        case Opt::SRV_VERBOSE:
            
            config.verbose = (bool)value;
            return;

        default:
            fatalError;
    }
}

void
RemoteServer::cacheInfo(RemoteServerInfo &result) const
{
    info.state = getState();
}

}
