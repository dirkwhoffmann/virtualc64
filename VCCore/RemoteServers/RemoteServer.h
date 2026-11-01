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

#include "RemoteServerTypes.h"
#include "RemoteManagerTypes.h"
#include "SubComponent.h"
// #include "Socket.h"
// #include "Thread.h"
#include "Transport.h"
#include "utl/wrappers.h"
// #include <thread>

namespace vc64 {

/* Every remote server is the transport delegate of its own transport layers.
 * The TransportDelegate base is inherited here (rather than by the concrete
 * subclasses) so that common functionality, such as traffic recording, can
 * be implemented in a single place.
 */
class RemoteServer : public SubComponent, public TransportDelegate {

    friend class RemoteManager;

    Descriptions descriptions = {{

        .name           = "RshServer",
        .description    = "Remote Shell Server",
        .shell          = "server rsh"
    }, {
        .name           = "RpcServer",
        .description    = "JSON RPC Server",
        .shell          = "server rpc"
    }, {
        .name           = "DapServer",
        .description    = "Debug Adapter",
        .shell          = "server dap"
    }, {
        .name           = "PromServer",
        .description    = "Prometheus Server",
        .shell          = "server prom"
    }};

    Options options = {

        // ENABLE comes last so it is executed last when a config script is read,
        // ensuring the server starts with the correct settings.

        Opt::SRV_TRANSPORT,
        Opt::SRV_PORT,
        Opt::SRV_VERBOSE,
        Opt::SRV_ENABLE
    };

public:

    // Result of the latest inspection
    utl::Backed<RemoteServerInfo> info;

protected:
    
    // Current configuration
    ServerConfig config = {};


    //
    // Initializing
    //

public:
    
    RemoteServer(C64& ref, isize id);

    RemoteServer& operator= (const RemoteServer& other) {

        CLONE(config)

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

    void _halt() override { try { stop(); } catch(...) { } };
    const Descriptions &getDescriptions() const override { return descriptions; }

protected:
    
    void _powerOff() override;

    template <class T>
    void serialize(T& worker)
    {
        if (isResetter(worker)) return;

        worker

        << config.enable
        << config.port
        << config.transport
        << config.verbose;

    };
    virtual void operator << (SerChecker &worker) override { serialize(worker); }
    virtual void operator << (SerCounter &worker) override { serialize(worker); }
    virtual void operator << (SerResetter &worker) override { serialize(worker); }
    virtual void operator << (SerReader &worker) override { serialize(worker); }
    virtual void operator << (SerWriter &worker) override { serialize(worker); }
    
    void _didLoad() override;


    //
    // Methods from Configurable
    //

public:

    const ServerConfig &getConfig() const { return config; }
    const Options &getOptions() const override { return options; }
    i64 getOption(Opt option) const override;
    void checkOption(Opt opt, i64 value) override;
    void setOption(Opt option, i64 value) override;

    
    //
    // Analyzing
    //

public:

    RemoteServerInfo cacheInfo() const;


    //
    // Examining the server
    //

private:

    // Returns a reference to the currently selected transport layer
    virtual Transport &transport() = 0;
    virtual const Transport &transport() const = 0;

public:

    virtual bool isSupported(TransportProtocol protocol) const = 0;
    SrvState getState() const { return transport().getState(); }
    bool isOff() const { return getState() == SrvState::OFF; }
    bool isWaiting() const { return getState() == SrvState::WAITING; }
    bool isStarting() const { return getState() == SrvState::STARTING; }
    bool isListening() const { return getState() == SrvState::LISTENING; }
    bool isConnected() const { return getState() == SrvState::CONNECTED; }
    bool isStopping() const { return getState() == SrvState::STOPPING; }
    bool isErroneous() const { return getState() == SrvState::INVALID; }


    //
    // Running the server
    //

public:

    // Indicates if the server is ready to launch
    virtual bool canRun() = 0;

    // Launch the remote server
    void start();

    // Returns the endpoint the transport is started with (HTTP-based servers)
    virtual string endpoint() const { return ""; }

    // Shuts down the remote server
    virtual void stop();

    // Disconnects the client (if any)
    virtual void disconnect() { transport().disconnect(); }

    // Switches into the WAITING state
    void waitForLaunch();


    //
    // Recording traffic
    //

public:

    /* Records a transmitted or received packet in the traffic log of the
     * RemoteManager. This function is called from the transport layer for
     * every packet that passes through. Note: It may be invoked from the
     * transport's session thread.
     */
    void recordTraffic(TrafficDirection direction, const string &payload);


    //
    // Sending packets
    //

public:

    // Sends a packet
    virtual void send(const string &payload) { transport().send(payload); }
    void send(char payload);
    void send(int payload);
    void send(long payload);
    void send(std::stringstream &payload);

    // Operator overloads
    RemoteServer &operator<<(char payload) { send(payload); return *this; }
    RemoteServer &operator<<(const string &payload) { send(payload); return *this; }
    RemoteServer &operator<<(int payload) { send(payload); return *this; }
    RemoteServer &operator<<(long payload) { send(payload); return *this; }
    RemoteServer &operator<<(std::stringstream &payload) { send(payload); return *this; }


    //
    // Methods from TransportDelegate
    //

public:

    void didSwitch(SrvState from, SrvState to) override;
    void didTerminate(const string &error) override;
};

}
