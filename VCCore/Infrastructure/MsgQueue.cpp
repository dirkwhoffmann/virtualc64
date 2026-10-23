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

#include "vcconfig.h"
#include "MsgQueue.h"

namespace vc64 {

void
MsgQueue::setListener(const void *listener, Callback *callback)
{
    {   SYNCHRONIZED

        this->listener = listener;
        this->callback = callback;

        // Send all pending messages
        if (callback) {

            while (!queue.isEmpty()) {

                Entry &entry = queue.read();
                callback(listener, entry.message, entry.exception);
                entry.exception = nullptr;
            }
        }
    }
}

bool
MsgQueue::get(Message &msg, std::exception_ptr &exception)
{
    if (!enabled) return false;

    {   SYNCHRONIZED

        if (queue.isEmpty()) return false;

        Entry &entry = queue.read();
        msg = entry.message;
        exception = entry.exception;
        entry.exception = nullptr;
        return true;
    }
}

void
MsgQueue::put(const Message &msg, const string &str, std::exception_ptr exception)
{
    if (enabled) {

        SYNCHRONIZED

        logmsg(LOG_MSG, "%s [%llx]\n", MsgEnum::key(msg.type), msg.value);

        // Delete the oldest element if the queue is full
        if (queue.isFull()) {

            if (!listener) {

                auto &lost = queue.read();
                logmsg(LOG_WARN, "Message lost: %s [%llx]\n", MsgEnum::key(lost.message.type), lost.message.value);
                lost.exception = nullptr;
            }
        }

        // Add message to the ringbuffer
        auto w = queue.w;
        queue.write(Entry { msg, exception });
        attachments[w] = str;
        queue.elements[w].message.str = attachments[w].c_str();

        // Send the message immediately if a lister has been registered
        if (listener) {

            callback(listener, queue.elements[w].message, queue.elements[w].exception);
            (void) queue.read();
            queue.elements[w].exception = nullptr;
        }
    }
}

void
MsgQueue::put(Msg type, i64 payload, i64 payload2, const string &str)
{
    put( Message { .type = type, .value = payload, .value2 = payload2 }, str);
}

void
MsgQueue::put(Msg type, const string &payload)
{
    put( Message { .type = type }, payload);
}

void
MsgQueue::put(Msg type, std::exception_ptr exception)
{
    put( Message { .type = type }, "", exception );
}

void
MsgQueue::put(Msg type, CpuMsg payload)
{
    put( Message { .type = type, .cpu = payload } );
}

void
MsgQueue::put(Msg type, ConfigMsg payload, const string &str, std::exception_ptr exception)
{
    put( Message { .type = type, .config = payload }, str, exception);
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
