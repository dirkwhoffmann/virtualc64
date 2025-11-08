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

#pragma once

#include "SubComponent.h"
#include "RemoteManagerTypes.h"
#include "RpcServer.h"
#include "DapServer.h"
#include "GdbServer.h"
#include "RshServer.h"

namespace vc64 {

class RemoteManager : public SubComponent, public Inspectable<RemoteManagerInfo> {

    Descriptions descriptions = {{

        .name           = "RemoteManager",
        .description    = "Remote Manager",
        .shell          = "server"
    }};

    Options options = {

    };

public:
    
    // The remote servers
    RpcServer rpcServer = RpcServer(c64, isize(ServerType::RPC));
    DapServer dapServer = DapServer(c64, isize(ServerType::DAP));
    GdbServer gdbServer = GdbServer(c64, isize(ServerType::GDB));
    RshServer rshServer = RshServer(c64, isize(ServerType::RSH));

    // Convenience wrapper
    std::vector <RemoteServer *> servers = { &rpcServer, &dapServer, &gdbServer, &rshServer };

    
    //
    // Initializing
    //
    
public:
    
    RemoteManager(C64& ref);
    
    RemoteManager& operator= (const RemoteManager& other) {

        CLONE(rshServer)
        CLONE(gdbServer)

        return *this;
    }


    //
    // Methods from CoreObject
    //
    
protected:
    
    void _dump(Category category, std::ostream &os) const override;
    
    
    //
    // Methods from CoreComponent
    //
    
private:
    
    template <class T> void serialize(T& worker) { } SERIALIZERS(serialize);
        
public:

    const Descriptions &getDescriptions() const override { return descriptions; }


    //
    // Methods from Configurable
    //

public:

    const Options &getOptions() const override { return options; }


    //
    // Methods from Inspectable
    //

public:

    void cacheInfo(RemoteManagerInfo &result) const override;


    //
    // Managing connections
    //
    
public:
    
    // Returns the number of servers being in a certain state
    isize numLaunching() const;
    isize numListening() const;
    isize numConnected() const;
    isize numErroneous() const;


    //
    // Servicing events
    //
    
public:
    
    void serviceServerEvent();
};

}
