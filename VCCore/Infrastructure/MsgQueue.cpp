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

        /* Send all pending messages.
         *
         * Only when there is somewhere to send them: passing no callback is
         * how a client detaches, and draining into it would call through a
         * null pointer. The backlog is left in the queue for whoever
         * attaches next.
         */
        if (callback) {

            while (!entries.empty()) {

                Entry &entry = entries.front();

                Message m = entry.message;
                m.str = entry.str.c_str();
                callback(listener, m, entry.exception);

                entries.pop_front();
            }
        }
    }
}

bool
MsgQueue::get(Message &msg, std::exception_ptr &exception)
{
    if (!enabled) return false;

    {   SYNCHRONIZED

        if (entries.empty()) return false;

        Entry &entry = entries.front();

        // Keep the attached string alive until the next call
        current = std::move(entry.str);
        msg = entry.message;
        msg.str = current.c_str();
        exception = entry.exception;

        entries.pop_front();
        return true;
    }
}

void
MsgQueue::put(const Message &msg, const string &str, std::exception_ptr exception)
{
    if (enabled) {

        SYNCHRONIZED

        logmsg(LOG_MSG, "%s [%llx]\n", MsgEnum::key(msg.type), msg.value);

        if (listener) {

            // Send the message immediately if a listener has been registered
            Message m = msg;
            m.str = str.c_str();
            callback(listener, m, exception);

        } else if (isize(entries.size()) < CAPACITY) {

            entries.push_back(Entry { msg, str, exception });

        } else {

            logmsg(LOG_WARN, "Message lost: %s [%llx]\n", MsgEnum::key(msg.type), msg.value);
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
