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
#include "MsgQueue.h"

namespace vc64 {

void
MsgQueue::setListener(const void *listener, Callback *callback)
{
    {   SYNCHRONIZED

        this->listener = listener;
        this->callback = callback;

        // Send all pending messages
        while (!queue.isEmpty()) {

            Message &msg = queue.read();
            callback(listener, msg);
        }
    }
}

bool
MsgQueue::get(Message &msg)
{
    if (!enabled) return false;

    {   SYNCHRONIZED

        if (queue.isEmpty()) return false;

        msg = queue.read();
        return true;
    }
}

void
MsgQueue::put(const Message &msg)
{
    if (enabled) {

        SYNCHRONIZED

        debug(MSG_DEBUG, "%s [%llx]\n", MsgEnum::key(msg.type), msg.value);

        if (listener) {

            // Send the message immediately if a lister has been registered
            callback(listener, msg);

        } else {

            // Otherwise, store it in the ring buffer
            if (!queue.isFull()) {
                queue.write(msg);
            } else {
                warn("Message lost: %s [%llx]\n", MsgEnum::key(msg.type), msg.value);
            }
        }
    }
}

void
MsgQueue::put(Msg type, i64 payload, i64 payload2)
{
    put( Message { .type = type, .value = payload, .value2 = payload2 } );
}

void
MsgQueue::put(Msg type, CpuMsg payload)
{
    put( Message { .type = type, .cpu = payload } );
}

void
MsgQueue::put(Msg type, DriveMsg payload)
{
    put( Message { .type = type, .drive = payload } );
}

void
MsgQueue::put(Msg type, ScriptMsg payload)
{
    put( Message { .type = type, .script = payload } );
}

}
