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
            callback(listener,
                     msg.type, msg.data1, msg.data2, msg.data3, msg.data4);
        }
        
        put(MSG_REGISTER);
    }
}

void
MsgQueue::put(MsgType type, isize d1, isize d2, isize d3, isize d4)
{
    {   SYNCHRONIZED

        auto i1 = i32(d1);
        auto i2 = i32(d2);
        auto i3 = i32(d3);
        auto i4 = i32(d4);

        debug(QUEUE_DEBUG,
              "%s [%d:%d:%d:%d]\n", MsgTypeEnum::key(type), i1, i2, i3, i4);

        // Send the message immediately if a lister has been registered
        if (listener) { callback(listener, type, i1, i2, i3, i4); return; }

        // Otherwise, store it in the ring buffer
        Message msg = { type, i1, i2, i3, i4 }; queue.write(msg);
    }
}

}
