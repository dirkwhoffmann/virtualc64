// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "MsgQueue.h"
#include "C64Types.h"

void
MsgQueue::addListener(const void *listener, Callback *func)
{
    synchronized {
        listeners.push_back(std::pair <const void *, Callback *> (listener, func));
    }
    
    // Distribute all pending messages
    Message msg;
    while ((msg = get()).type != MSG_NONE) {
        propagate(msg);
    }

    put(MSG_REGISTER);
}

void
MsgQueue::removeListener(const void *listener)
{
    put(MSG_UNREGISTER);
    
    synchronized {
        
        for (auto it = listeners.begin(); it != listeners.end(); it++) {
            if (it->first == listener) { listeners.erase(it);  break; }
        }
    }
}

Message
MsgQueue::get()
{
    Message result;
    
    synchronized {
        
        if (queue.isEmpty()) {
            result = { MSG_NONE, 0 };
        } else {
            result = queue.read();
        }
    }
    
    return result;
}
 
void
MsgQueue::put(MsgType type, i64 data)
{
    synchronized {
                        
        debug (QUEUE_DEBUG, "%s [%lld]\n", MSGTypeEnum::key(type), data);
        
        // Delete the oldest message if the queue overflows
        if (queue.isFull()) (void)queue.read();
    
        // Write data
        Message msg = { type, data };
        queue.write(msg);
        
        // Serve registered callbacks
        propagate(msg);
    }
}

void
MsgQueue::dump()
{
    for (int i = queue.begin(); i != queue.end(); i = queue.next(i)) {
        msg("%02d", i); dump(queue.elements[i]);
    }
}

void
MsgQueue::dump(const Message &msg)
{
    msg("%s [%ld]\n", MSGTypeEnum::key(msg.type), msg.data);
}

void
MsgQueue::propagate(const Message &msg) const
{
    for (auto i = listeners.begin(); i != listeners.end(); i++) {
        i->second(i->first, msg.type, msg.data);
    }
}
