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

#include "Transport.h"

namespace httplib { class Server; struct Request; }

namespace vc64 {

class HttpTransport : public Transport {

    using Transport::Transport;

protected:

    HttpTransport& operator= (const HttpTransport& other) {

        Transport::operator = (other);
        return *this;
    }

    // A simple third-party HTTP server
    httplib::Server *srv = nullptr;


    //
    // Methods from CoreObject
    //

protected:

    // void _dump(Category category, std::ostream &os) const override;


    //
    // Methods from Transport
    //

public:

    virtual void disconnect() override;
};

}
