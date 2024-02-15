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

#include "CmdQueueTypes.h"
#include "CoreObject.h"
#include "Synchronizable.h"
#include "RingBuffer.h"
#include <atomic>

namespace vc64 {

class CmdQueue : CoreObject, Synchronizable {

    // Ring buffer storing all pending commands
    util::RingBuffer <Cmd, 128> queue;

    // Indicates if the queue is empty
    std::atomic<bool> empty = true;

    //
    // Constructing
    //



    //
    // Methods from CoreObject
    //

private:

    const char *getDescription() const override { return "CmdQueue"; }
    void _dump(Category category, std::ostream& os) const override { }


    //
    // Managing the queue
    //

public:

    // Sends a command
    void put(const Cmd &cmd);
    void put(CmdType type, KeyCmd payload);

    // Polls a command
    bool poll(Cmd &cmd);
};

}
