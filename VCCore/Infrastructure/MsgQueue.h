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

#include "MsgQueueTypes.h"
#include "CoreObject.h"
#include "utl/abilities/Synchronizable.h"
#include <deque>

namespace vc64 {

class MsgQueue final : CoreObject, utl::Synchronizable {

    /* Maximum number of pending messages. Messages only pile up while no
     * listener is registered (before a client attaches, or if the client
     * polls with get()). A message that arrives at a full queue is dropped.
     */
    static constexpr isize CAPACITY = 65536;

    // A pending message, together with its attached string and the exception it reports (if any)
    struct Entry {

        Message message;
        string str;
        std::exception_ptr exception;
    };

    // All pending messages
    std::deque<Entry> entries;

    // The attached string of the message that has been read last
    string current;

    // The registered listener
    const void *listener = nullptr;

    // The registered callback function
    Callback *callback = nullptr;

    // If disabled, no messages will be stored
    bool enabled = true;

    //
    // Constructing
    //

    //
    // Methods from CoreObject
    //

public:

    const char *objectName() const override { return "MsgQueue"; }


    //
    // Managing the queue
    //

public:

    // Registers a listener together with it's callback function
    void setListener(const void *listener, Callback *func);

    // Disables the message queue
    void disable() { enabled = false; }

    // Reads a message, together with the exception it reports (if any)
    bool get(Message &msg, std::exception_ptr &exception);

    // Sends a message
    void put(const Message &msg, const string &str = "", std::exception_ptr exception = nullptr);
    void put(Msg type, i64 payload = 0, i64 payload2 = 0, const string &str = "");
    void put(Msg type, const string &payload);

    // Sends a message that reports a failure
    void put(Msg type, std::exception_ptr exception);
    void put(Msg type, CpuMsg payload);
    void put(Msg type, ConfigMsg payload, const string &str = "", std::exception_ptr exception = nullptr);
    void put(Msg type, DriveMsg payload);
    void put(Msg type, ScriptMsg payload);
};

}
