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

class RemoteServer : public SubComponent {

    friend class RemoteManager;

    Descriptions descriptions = {{

        .name           = "RshServer",
        .description    = "Remote Shell Server",
        .shell          = "server rshell"
    }, {
        .name           = "DapServer",
        .description    = "Debug Adaper",
        .shell          = "server dap"
    }, {
        .name           = "GdbServer",
        .description    = "GDB Remote Server",
        .shell          = "server gdb"
    }};

    Options options = {

        Opt::SRV_PORT,
        Opt::SRV_PROTOCOL,
        Opt::SRV_AUTORUN,
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
    ~RemoteServer() { shutDownServer(); }
    void shutDownServer();
    
    RemoteServer& operator= (const RemoteServer& other) {

        CLONE(config)
        
        return *this;
    }


    //
    // Methods from CoreObject
    //
    
protected:

    void _dump(Category category, std::ostream &os) const override;
    
public:

    const Descriptions &getDescriptions() const override { return descriptions; }


    //
    // Methods from CoreComponent
    //
    
protected:
    
    void _powerOff() override;

    template <class T>
    void serialize(T& worker)
    {
        if (isResetter(worker)) return;

        worker

        << config.port
        << config.protocol
        << config.autoRun
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
    // Examining state
    //
    
public:

    bool isOff() const { return state == SrvState::OFF; }
    bool isStarting() const { return state == SrvState::STARTING; }
    bool isListening() const { return state == SrvState::LISTENING; }
    bool isConnected() const { return state == SrvState::CONNECTED; }
    bool isStopping() const { return state == SrvState::STOPPING; }
    bool isErroneous() const { return state == SrvState::INVALID; }

    
    //
    // Starting and stopping the server
    //
    
public:

    // Launch the remote server
    virtual void start() throws;

    // Shuts down the remote server
    virtual void stop() throws;

    // Disconnects the client
    virtual void disconnect() throws = 0;

protected:

    // Switches the internal state
    void switchState(SrvState newState);
    
private:
    
    // Used by the launch daemon to determine if actions should be taken
    virtual bool shouldRun() { return true; }


    //
    // Running the server
    //

protected:

    // The main thread function
    virtual void main() throws = 0;

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
