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
#include <deque>
#include <exception>
#include <functional>

namespace vc64 {

/// Command queue
class CmdQueue final : public CoreObject, Synchronizable {

public:

    /* Maximum number of pending commands. A queue only fills up when nobody
     * carries out commands for a long time (e.g., because a thread forgot to
     * resume a suspended emulator) or when no emulator thread has been
     * launched yet. A command that arrives at a full queue is dropped.
     */
    static constexpr isize CAPACITY = 65536;

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

    /// A pending command together with its attachments
    struct Entry {

        Command cmd;
        string str;
        Completion completion;
    };

    /// All pending commands
    std::deque<Entry> entries;

    /// The attached string of the command that has been polled last
    string current;

    /// Indicates that a command has been dropped since the queue was last below its capacity
    bool overflowing = false;

public:

    /// Indicates if the queue is empty
    std::atomic<bool> empty = true;

    /* Called (on the sender's thread, outside the queue's lock) when a command
     * is dropped. It is called once per overflow, not once per dropped command.
     */
    std::function<void()> onFull;

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

    /* Polls a command together with its completion handler (may be empty).
     * The attached string stays valid until the next call.
     */
    bool poll(Command &cmd, Completion &completion);
};

}
