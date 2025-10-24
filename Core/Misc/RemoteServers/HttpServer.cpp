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

#include "config.h"
#include "HttpServer.h"
#include "httplib.h"

namespace vc64 {

void
HttpServer::_dump(Category category, std::ostream &os) const
{
    using namespace util;

    if (category == Category::State) {

        RemoteServer::_dump(category, os);
        // os << tab("...");
        // os << dec(...) << std::endl;

    } else {

        RemoteServer::_dump(category, os);
    }
}

void
HttpServer::disconnect()
{
    debug(SRV_DEBUG, "Disconnecting...\n");
    if (srv) srv->stop();
}

}
