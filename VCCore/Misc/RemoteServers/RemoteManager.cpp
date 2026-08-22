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
#include "RemoteManager.h"
#include "C64.h"
#include <chrono>

namespace vc64 {

isize
TrafficLog::append(ServerType server, TrafficDirection direction, const string &payload)
{
    std::lock_guard<std::mutex> lock(mutex);

    auto now = std::chrono::system_clock::now().time_since_epoch();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now).count();

    entries.push_back(TrafficEntry {

        .nr         = counter,
        .server     = server,
        .direction  = direction,
        .time       = i64(ms),
        .payload    = payload
    });

    // Drop the oldest entries if the log has grown too large
    while (isize(entries.size()) > capacity) entries.pop_front();

    return counter++;
}

std::vector<TrafficEntry>
TrafficLog::read(isize nr) const
{
    std::lock_guard<std::mutex> lock(mutex);

    std::vector<TrafficEntry> result;

    for (const auto &entry : entries) {
        if (entry.nr > nr) result.push_back(entry);
    }

    return result;
}

void
TrafficLog::clear()
{
    std::lock_guard<std::mutex> lock(mutex);

    entries.clear();
}

RemoteManager::RemoteManager(C64& ref) : SubComponent(ref)
{
    subComponents = std::vector<CoreComponent *> {

        &rshServer,
        &rpcServer,
        &dapServer,
        &promServer,
    };    
}

void
RemoteManager::_dump(Category category, std::ostream &os) const
{
    using namespace utl;

    if (category == Category::State) {

        os << "Remote server status: " << std::endl << std::endl;

        for (auto server : servers) {

            auto name = server->objectName();
            auto port = server->config.port;
            auto transport = server->config.transport;

            os << tab(string(name));

            os << TransportProtocolEnum::key(transport);
            if (transport != TransportProtocol::STDIO) { os << ":" << dec(port); }
            os << " (" << SrvStateEnum::key(server->getState()) << ")" << std::endl;

            /*
             if (server->isOff()) {
             os << "Off" << std::endl;
             } else {
             os << TransportProtocolEnum::key(transport);
             if (transport != TransportProtocol::STDIO) { os << ":" << dec(port);
             os << " (" << SrvStateEnum::key(server->getState()) << ")" << std::endl;
             }
             */
        }
    }
}

void
RemoteManager::cacheInfo(RemoteManagerInfo &result) const
{
    {   SYNCHRONIZED

        info.rshInfo = rshServer.getInfo();
        info.rpcInfo = rpcServer.getInfo();
        info.dapInfo = dapServer.getInfo();
        info.promInfo = promServer.getInfo();
    }
}

isize
RemoteManager::numLaunching() const
{
    isize result = 0;
    for (auto &s : servers) if (s->isStarting()) result++;
    return result;
}

isize
RemoteManager::numListening() const
{
    isize result = 0;
    for (auto &s : servers) if (s->isListening()) result++;
    return result;
}

isize
RemoteManager::numConnected() const
{
    isize result = 0;
    for (auto &s : servers) if (s->isConnected()) result++;
    return result;
}

isize
RemoteManager::numErroneous() const
{
    isize result = 0;
    for (auto &s : servers) if (s->isErroneous()) result++;
    return result;
}

void
RemoteManager::recordTraffic(ServerType server, TrafficDirection direction, const string &payload)
{
    // Ignore empty packets (e.g., HTTP responses without content)
    if (payload.empty()) return;

    // Store the packet in the traffic log
    auto nr = trafficLog.append(server, direction, payload);

    // Inform the GUI
    auto msg = direction == TrafficDirection::SENT ? Msg::SRV_SEND : Msg::SRV_RECEIVE;
    msgQueue.put(msg, i64(server), i64(nr), payload);
}

void
RemoteManager::send(ServerType server, const string &payload)
{
    switch (server) {

        case ServerType::RSH:  rshServer.send(payload);  break;
        case ServerType::RPC:  rpcServer.send(payload);  break;
        case ServerType::DAP:  dapServer.send(payload);  break;
        case ServerType::PROM: promServer.send(payload); break;

        default:
            fatalError;
    }
}

}
