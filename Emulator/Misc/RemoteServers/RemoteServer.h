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
    }};

    Options options = {

        OPT_SRV_PORT,
        OPT_SRV_PROTOCOL,
        OPT_SRV_AUTORUN,
        OPT_SRV_VERBOSE
    };

protected:
    
    // Current configuration
    ServerConfig config = {};

    // Sockets
    Socket listener;
    Socket connection;

    // The server thread
    std::thread serverThread;

    // The current server state
    SrvState state = SRV_STATE_OFF;
    
    // The number of sent and received packets
    isize numSent = 0;
    isize numReceived = 0;

    
    //
    // Initializing
    //
    
public:
    
    RemoteServer(C64& ref);
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

    void _dump(Category category, std::ostream& os) const override;
    
public:

    const Descriptions &getDescriptions() const override { return descriptions; }


    //
    // Methods from CoreComponent
    //
    
private:
    
    void _powerOff() override;

    template <class T>
    void serialize(T& worker)
    {
        if (isResetter(worker)) return;

        worker

        << config.port
        << config.protocol
        << config.verbose;

    } SERIALIZERS(serialize);

    void _didLoad() override;


    //
    // Methods from Configurable
    //

public:

    const ServerConfig &getConfig() const { return config; }
    const Options &getOptions() const override { return options; }
    i64 getOption(Option option) const override;
    void checkOption(Option opt, i64 value) override;
    void setOption(Option option, i64 value) override;


    //
    // Examining state
    //
    
public:

    bool isOff() const { return state == SRV_STATE_OFF; }
    bool isStarting() const { return state == SRV_STATE_STARTING; }
    bool isListening() const { return state == SRV_STATE_LISTENING; }
    bool isConnected() const { return state == SRV_STATE_CONNECTED; }
    bool isStopping() const { return state == SRV_STATE_STOPPING; }
    bool isErroneous() const { return state == SRV_STATE_ERROR; }

    
    //
    // Starting and stopping the server
    //
    
public:

    // Launch the remote server
    void start() throws { SUSPENDED _start(); }
    
    // Shuts down the remote server
    void stop() throws { SUSPENDED _stop(); }

    // Disconnects the client
    void disconnect() throws { SUSPENDED _disconnect(); }

protected:

    // Called from disconnect(), start() and stop()
    void _start() throws;
    void _stop() throws;
    void _disconnect() throws;
    
    // Switches the internal state
    void switchState(SrvState newState);
    
private:
    
    // Used by the launch daemon to determine if actions should be taken
    virtual bool shouldRun() { return true; }
        
    
    //
    // Running the server
    //
    
private:
    
    // The main thread function
    void main();

    // Inner loops (called from main)
    void mainLoop() throws;
    void sessionLoop();
    
    
    //
    // Transmitting and processing packets
    //
    
public:
    
    // Receives or packet
    string receive() throws;
    
    // Sends a packet
    void send(const string &payload) throws;
    void send(char payload) throws;
    void send(int payload) throws;
    void send(long payload) throws;
    void send(std::stringstream &payload) throws;
    
    // Operator overloads
    RemoteServer &operator<<(char payload) { send(payload); return *this; }
    RemoteServer &operator<<(const string &payload) { send(payload); return *this; }
    RemoteServer &operator<<(int payload) { send(payload); return *this; }
    RemoteServer &operator<<(long payload) { send(payload); return *this; }
    RemoteServer &operator<<(std::stringstream &payload) { send(payload); return *this; }

    // Processes a package
    void process(const string &payload) throws;
    
private:

    // Reports an error to the GUI
    void handleError(const char *description);
    
    
    //
    // Subclass specific implementations
    //

private:
    
    virtual string doReceive() throws = 0;
    virtual void doSend(const string &payload) throws = 0;
    virtual void doProcess(const string &payload) throws = 0;
    
    
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
