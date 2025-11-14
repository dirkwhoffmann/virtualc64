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

#include "HttpServer.h"
#include "Console.h"

namespace vc64 {

class RpcHttpServer final : public HttpServer, public ConsoleDelegate {

    using HttpServer::HttpServer;

protected:

    RpcHttpServer& operator= (const RpcHttpServer& other) {

        HttpServer::operator = (other);
        return *this;
    }


    //
    // Methods from CoreObject
    //

protected:

    void _initialize() override;
    void _dump(Category category, std::ostream &os) const override;


    //
    // Methods from Configurable
    //

private:

    void checkOption(Opt opt, i64 value) override;
    // void setOption(Opt option, i64 value) override;
    

    //
    // Methods from RemoteServer
    //

public:

    void main() override;


    //
    // Methods from ConsoleDelegate
    //

public:

    void willExecute(const InputLine &input) override;
    void didExecute(const InputLine &input, std::stringstream &ss) override;
    void didExecute(const InputLine &input, std::stringstream &ss, std::exception &e) override;


    //
    // Handling requests
    //

private:

    // Generate a response
    string respond(const httplib::Request& request);
};

}
