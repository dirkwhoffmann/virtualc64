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
#include "RshServer.h"
#include "DapServer.h"
#include "PromServer.h"
#include <deque>
#include <mutex>

namespace vc64 {

/* A bounded log keeping the most recently transmitted packets of all remote
 * servers. The log is filled by the transport layers (via
 * RemoteServer::recordTraffic) and read by the GUI (via
 * RemoteManagerAPI::getTraffic). Because packets are recorded on the
 * transports' session threads and read from the GUI thread, all access is
 * mutex-protected.
 */
class TrafficLog {

    // Maximum number of entries to keep
    static constexpr isize capacity = 512;

    // Mutex protecting the log
    mutable std::mutex mutex;

    // The recorded entries
    std::deque<TrafficEntry> entries;

    // Sequence number of the next entry
    isize counter = 0;

public:

    // Appends an entry and returns its sequence number
    isize append(ServerType server, TrafficDirection direction, const string &payload);

    // Returns all entries with a sequence number greater than nr
    std::vector<TrafficEntry> read(isize nr = -1) const;

    // Removes all entries
    void clear();
};

class RemoteManager final : public SubComponent, public Inspectable<RemoteManagerInfo> {

    Descriptions descriptions = {{

        .name           = "RemoteManager",
        .description    = "Remote Manager",
        .shell          = "server"
    }};

    Options options = {

    };

public:
    
    // The remote servers
    RshServer rshServer = RshServer(c64, isize(ServerType::RSH));
    RpcServer rpcServer = RpcServer(c64, isize(ServerType::RPC));
    DapServer dapServer = DapServer(c64, isize(ServerType::DAP));
    PromServer promServer = PromServer(c64, isize(ServerType::PROM));

    // Convenience wrapper
    std::vector <RemoteServer *> servers = { &rshServer, &rpcServer, &dapServer, &promServer };

    
    //
    // Initializing
    //
    
public:
    
    RemoteManager(C64& ref);
    
    RemoteManager& operator= (const RemoteManager& other) {

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
    // Recording traffic
    //

private:

    // Log of the most recently transmitted packets
    TrafficLog trafficLog;

public:

    /* Records a transmitted or received packet and informs the GUI by
     * posting a Msg::SRV_RECEIVE or Msg::SRV_SEND message. The message
     * carries the server type in 'value' and the sequence number of the
     * recorded entry in 'value2'. Empty payloads are ignored.
     */
    void recordTraffic(ServerType server, TrafficDirection direction, const string &payload);


    //
    // Sending packets
    //

public:

    /* Sends a raw payload through the specified server. The caller is
     * responsible for formatting the payload (e.g., assembling a JSON-RPC
     * packet); this function performs no formatting of its own. It is
     * delivered through the server's currently configured transport and
     * silently dropped if no client is connected.
     */
    void send(ServerType server, const string &payload);
};

}
