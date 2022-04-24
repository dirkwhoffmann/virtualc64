// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "MsgQueue.h"

void
MsgQueue::setListener(const void *listener, Callback *callback)
{
    {   SYNCHRONIZED

        this->listener = listener;
        this->callback = callback;

        // Send all pending messages
        while (!queue.isEmpty()) {
            Message &msg = queue.read();
            callback(listener, msg.type, msg.data1, msg.data2);
        }
        put(MSG_REGISTER);
    }
}

void
MsgQueue::put(MsgType type, isize data1, isize data2)
{
    {   SYNCHRONIZED

        auto payload1 = u32(data1);
        auto payload2 = u32(data2);

        debug(QUEUE_DEBUG,
              "%s [%u:%u]\n", MsgTypeEnum::key(type), payload1, payload2);

        // Send the message immediately if a lister has been registered
        if (listener) { callback(listener, type, payload1, payload2); return; }

        // Otherwise, store it in the ring buffer
        Message msg = { type, payload1, payload2 }; queue.write(msg);
    }
}

void
MsgQueue::put(MsgType type, u16 val1, u16 val2, u16 val3, u16 val4)
{
    put(type, HI_W_LO_W(val1,val2), HI_W_LO_W(val3,val4));
}
