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
/// @file

#pragma once

#include "CmdQueueTypes.h"
#include "CoreObject.h"
#include "utl/abilities/Synchronizable.h"
#include "RingBuffer.h"
#include "Utilities/RingBuffer.h" // DEPRECATED
#include <atomic>

namespace vc64 {

/// Command queue
class CmdQueue final : CoreObject, utl::Synchronizable {

    /// Ring buffer storing all pending commands
    util::RingBuffer <Command, 256> queue;

public:
    
    /// Indicates if the queue is empty
    std::atomic<bool> empty = true;

    //
    // Methods
    //

private:

    const char *objectName() const override { return "CmdQueue"; }


    //
    // Managing the queue
    //

public:

    // Sends a command
    void put(const Command &cmd);

    // Polls a command
    bool poll(Command &cmd);
};

}
