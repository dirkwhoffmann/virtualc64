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
#include "SubComponent.h"
#include "Socket.h"
#include "Thread.h"
#include <thread>

namespace vc64 {

class RemoteServer : public SubComponent, public Inspectable<RemoteServerInfo> {

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
        .description    = "Debug Adaper",
        .shell          = "server dap"
    }, {
        .name           = "PromServer",
        .description    = "Prometheus Server",
        .shell          = "server prom"
    }};

    Options options = {

        Opt::SRV_ENABLE,
        Opt::SRV_PORT,
        Opt::SRV_TRANSPORT,
        Opt::SRV_VERBOSE
    };

protected:
    
    // Current configuration
    ServerConfig config = {};

    // The server thread
    std::thread serverThread;

    // The current server state
    SrvState state = SrvState::OFF;
    

    //
    // Initializing
    //
    
public:
    
    using SubComponent::SubComponent;
    ~RemoteServer();

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

public:

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
    // Methods from Inspectable
    //

public:

    void cacheInfo(RemoteServerInfo &result) const override;


    //
    // Examining state
    //
    
public:

    virtual SrvState getState() const { return state; }
    virtual bool isOff() const { return state == SrvState::OFF; }
    virtual bool isWaiting() const { return state == SrvState::WAITING; }
    virtual bool isStarting() const { return state == SrvState::STARTING; }
    virtual bool isListening() const { return state == SrvState::LISTENING; }
    virtual bool isConnected() const { return state == SrvState::CONNECTED; }
    virtual bool isStopping() const { return state == SrvState::STOPPING; }
    virtual bool isErroneous() const { return state == SrvState::INVALID; }

    
    //
    // Starting and stopping the server
    //
    
private: // public:

    // Launch the remote server
    virtual void start() = 0;

    // Shuts down the remote server
    virtual void stop() = 0;

    // Disconnects the client
    virtual void disconnect() throws = 0;

protected:

    // Switches the internal state
    virtual void switchState(SrvState newState);

private:
    
    // Indicates if the server is ready to launch
    virtual bool canRun() { return true; }


    //
    // Running the server
    //

protected:

    // The main thread function
    virtual void main() throws { };

    // Reports an error to the GUI
    void handleError(const char *description);


    //
    // Delegation methods
    //

    void didSwitch(SrvState from, SrvState to);
    virtual void didStart() { };
    virtual void didStop() { };
    virtual void didConnect() { };
    virtual void didDisconnect() { };
};

}
