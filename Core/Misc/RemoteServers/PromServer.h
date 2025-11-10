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

namespace vc64 {

class PromServer final : public HttpServer {

public:

    using HttpServer::HttpServer;

protected:

    PromServer& operator= (const PromServer& other) {

        HttpServer::operator = (other);
        return *this;
    }


    //
    // Methods from CoreObject
    //

protected:

    void _dump(Category category, std::ostream &os) const override;


    //
    // Methods from RemoteServer
    //

public:

    void main() override;


    //
    // Handling requests
    //

    // Generate a response
    string respond(const httplib::Request& request);
};

}
