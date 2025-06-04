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

#include "VirtualC64Config.h"
#include "RemoteServer.h"
#include "Emulator.h"

namespace vc64 {

RemoteServer::RemoteServer(C64 &ref) : SubComponent(ref)
{

}

void
RemoteServer::shutDownServer()
{
    debug(SRV_DEBUG, "Shutting down\n");
    try { stop(); } catch(...) { }
}

void
RemoteServer::_dump(Category category, std::ostream &os) const
{
    using namespace util;

    if (category == Category::Config) {
        
        dumpConfig(os);
    }
    
    if (category == Category::State) {
        
        os << tab("State");
        os << SrvStateEnum::key(state) << std::endl;
        os << tab("Received packets");
        os << dec(numReceived) << std::endl;
        os << tab("Transmitted packets");
        os << dec(numSent) << std::endl;
    }
}

void
RemoteServer::_powerOff()
{
    shutDownServer();
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
            
        case Opt::SRV_PORT:     return (i64)config.port;
        case Opt::SRV_PROTOCOL: return (i64)config.protocol;
        case Opt::SRV_AUTORUN:  return (i64)config.autoRun;
        case Opt::SRV_VERBOSE:  return (i64)config.verbose;

        default:
            fatalError;
    }
}

void
RemoteServer::checkOption(Opt opt, i64 value)
{
    switch (opt) {

        case Opt::SRV_PORT:
        case Opt::SRV_PROTOCOL:
        case Opt::SRV_AUTORUN:
        case Opt::SRV_VERBOSE:

            return;

        default:
            throw AppError(Fault::OPT_UNSUPPORTED);
    }
}

void
RemoteServer::setOption(Opt option, i64 value)
{
    switch (option) {

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
            
        case Opt::SRV_PROTOCOL:
            
            config.protocol = (ServerProtocol)value;
            return;
            
        case Opt::SRV_AUTORUN:
            
            config.autoRun = (bool)value;
            return;

        case Opt::SRV_VERBOSE:
            
            config.verbose = (bool)value;
            return;

        default:
            fatalError;
    }
}

void
RemoteServer::_start()
{
    if (isOff()) {
        
        debug(SRV_DEBUG, "Starting server...\n");
        switchState(SrvState::STARTING);
        
        // Make sure we continue with a terminated server thread
        if (serverThread.joinable()) serverThread.join();
        
        // Spawn a new thread
        serverThread = std::thread(&RemoteServer::main, this);
    }
}

void
RemoteServer::_stop()
{
    if (!isOff()) {
        
        debug(SRV_DEBUG, "Stopping server...\n");
        switchState(SrvState::STOPPING);
        
        // Interrupt the server thread
        _disconnect();
        
        // Wait until the server thread has terminated
        if (serverThread.joinable()) serverThread.join();
        
        switchState(SrvState::OFF);
    }
}

void
RemoteServer::_disconnect()
{
    debug(SRV_DEBUG, "Disconnecting...\n");
    
    // Trigger an exception inside the server thread
    connection.close();
    listener.close();
}

void
RemoteServer::switchState(SrvState newState)
{
    auto oldState = state;
    
    if (oldState != newState) {
        
        debug(SRV_DEBUG, "Switching state: %s -> %s\n",
              SrvStateEnum::key(state), SrvStateEnum::key(newState));
        
        // Switch state
        state = newState;
        
        // Call the delegation method
        didSwitch(oldState, newState);
        
        // Inform the GUI
        msgQueue.put(Msg::SRV_STATE, (i64)newState);
    }
}

string
RemoteServer::receive()
{
    string packet;
    
    if (isConnected()) {
        
        packet = doReceive();
        msgQueue.put(Msg::SRV_RECEIVE, ++numReceived);
    }
    
    return packet;
}

void
RemoteServer::send(const string &packet)
{
    if (isConnected()) {
        
        doSend(packet);
        msgQueue.put(Msg::SRV_SEND, ++numSent);
    }
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

void
RemoteServer::process(const string &payload)
{
    doProcess(payload);
}

void
RemoteServer::main()
{    
    try {
        
        mainLoop();
        
    } catch (std::exception &err) {

        debug(SRV_DEBUG, "Server thread interrupted\n");
        handleError(err.what());
    }
}

void
RemoteServer::mainLoop()
{
    switchState(SrvState::LISTENING);

    while (isListening()) {
        
        try {
            
            try {
                
                // Try to be a client by connecting to an existing server
                connection.connect(config.port);
                debug(SRV_DEBUG, "Acting as a client\n");
                
            } catch (...) {
                
                // If there is no existing server, be the server
                debug(SRV_DEBUG, "Acting as a server\n");
                
                // Create a port listener
                listener.bind(config.port);
                listener.listen();
                
                // Wait for a client to connect
                connection = listener.accept();
            }
            
            // Handle the session
            sessionLoop();
            
            // Close the port listener
            listener.close();
            
        } catch (std::exception &err) {
            
            debug(SRV_DEBUG, "Main loop interrupted\n");

            // Handle error if we haven't been interrupted purposely
            if (!isStopping()) handleError(err.what());
        }
    }
    
    switchState(SrvState::OFF);
}

void
RemoteServer::sessionLoop()
{
    switchState(SrvState::CONNECTED);
    
    numReceived = 0;
    numSent = 0;

    try {

        // Receive and process packets
        while (1) { process(receive()); }
        
    } catch (std::exception &err) {

        debug(SRV_DEBUG, "Session loop interrupted\n");

        // Handle error if we haven't been interrupted purposely
        if (!isStopping()) {
            
            handleError(err.what());
            switchState(SrvState::LISTENING);
        }
    }

    numReceived = 0;
    numSent = 0;

    connection.close();
}

void
RemoteServer::handleError(const char *description)
{
    switchState(SrvState::INVALID);
    retroShell << "Server Error: " << string(description) << '\n';
}

void
RemoteServer::didSwitch(SrvState from, SrvState to)
{
    if (from == SrvState::STARTING && to == SrvState::LISTENING) {
        didStart();
    }
    if (to == SrvState::OFF) {
        didStop();
    }
    if (to == SrvState::CONNECTED) {
        didConnect();
    }
    if (from == SrvState::CONNECTED) {
        didDisconnect();
    }
}

}
