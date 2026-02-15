// -----------------------------------------------------------------------------
// This file is part of utlib - A lightweight utility library
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include "utl/abilities/Synchronizable.h"

namespace utl {

template<typename Msg> class MailboxDelegate {

public:

    virtual ~MailboxDelegate() = default;

    // Called after a message has been successfully written to the queue.
    virtual void messageArrived(const Msg &msg) { };

    // Called when a message could not be written because the queue is full.
    virtual void mailboxFull(const Msg &msg) { };
};

template<typename Msg, isize Capacity> class Mailbox : Synchronizable {

    // Ring buffer storing messages
    utl::RingBuffer<Msg, Capacity> queue;

    // Optional delegate
    MailboxDelegate<Msg> *delegate = nullptr;

public:

    void setDelegate(MailboxDelegate<Msg> *d)
    {
        SYNCHRONIZED
        delegate = d;
    }

    void clearDelegate()
    {
        SYNCHRONIZED
        delegate = nullptr;
    }

    // Add a message to the queue
    void put(const Msg &msg)
    {
        SYNCHRONIZED

        if (!queue.isFull()) {

            queue.write(msg);
            if (delegate) delegate->messageArrived(msg);

        } else {

            if (delegate) delegate->mailboxFull(msg);
        }
    }

    // Retrieve a single message
    bool poll(Msg &msg)
    {
        SYNCHRONIZED

        if (queue.isEmpty()) return false;
        msg = queue.read();
        return true;
    }

    // Retrieve up to `count` messages
    isize poll(isize count, Msg *buffer)
    {
        SYNCHRONIZED

        isize n = std::min(queue.count(), count);
        for (isize i = 0; i < n; i++) {
            buffer[i] = queue.read();
        }
        return n;
    }

    bool empty() const
    {
        SYNCHRONIZED
        return queue.isEmpty();
    }

    isize count() const
    {
        SYNCHRONIZED
        return queue.count();
    }
};

}
