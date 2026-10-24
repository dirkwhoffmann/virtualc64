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
#include "utl/storage/RingBuffer.h"
#include <atomic>
#include <exception>
#include <functional>

namespace vc64 {

/// Command queue
class CmdQueue final : CoreObject, utl::Synchronizable {

    static constexpr isize CAPACITY = 256;

    /// Ring buffer storing all pending commands
    utl::RingBuffer <Command, CAPACITY> queue;

    /// Attached string objects
    string attachments[CAPACITY];

public:

    /* Completion handler of a command.
     *
     * Invoked on the emulator thread once the command has been carried out.
     * The argument is null on success and holds the exception otherwise. If
     * the command never makes it into the queue (the queue is full), the
     * handler is called with an exception right away, on the sender's thread.
     *
     * Handlers must be short and must not block, call wait(), or touch UI
     * objects. Capture shared ownership (e.g., a shared_ptr), not references:
     * the handler runs later and on another thread.
     */
    using Completion = std::function<void(std::exception_ptr)>;

private:

    /// Completion handlers (parallel to the ring buffer)
    Completion completions[CAPACITY];

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
    void put(const Command &cmd, const string &str = "", Completion completion = nullptr);

    // Polls a command together with its completion handler (may be empty)
    bool poll(Command &cmd, Completion &completion);
};

}
