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

#include "SocketServer.h"
#include "Console.h"

namespace vc64 {

class RshServer final : public SocketServer, public ConsoleDelegate {

public:
    
    using SocketServer::SocketServer;

    RshServer& operator= (const RshServer& other) {

        SocketServer::operator = (other);
        return *this;
    }


    //
    // Methods from CoreObject
    //
    
private:

    void _initialize() override;
    void _dump(Category category, std::ostream &os) const override;

    //
    // Methods from RemoteServer
    //

    virtual bool canRun() override { return true; }


    //
    // Methods from SocketServer
    //

    string doReceive() throws override;
    void doProcess(const string &packet) throws override;
    void doSend(const string &packet)throws  override;
    void didStart() override;
    void didConnect() override;


    //
    // Methods from ConsoleDelegate
    //

    void didActivate() override;
    void didDeactivate() override;
    void willExecute(const InputLine &input) override;
    void didExecute(const InputLine &input, std::stringstream &ss) override;
    void didExecute(const InputLine &input, std::stringstream &ss, std::exception &e) override;
};

}
