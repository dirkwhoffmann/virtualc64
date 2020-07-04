// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "MessageQueue.h"

MessageQueue::MessageQueue()
{
    setDescription("MessageQueue");
	pthread_mutex_init(&lock, NULL);
}

MessageQueue::~MessageQueue()
{
	pthread_mutex_destroy(&lock);
}

void
MessageQueue::addListener(const void *listener, Callback *func)
{
    pthread_mutex_lock(&lock);
    listeners.insert(pair <const void *, Callback *> (listener, func));
    pthread_mutex_unlock(&lock);
    
    // Distribute all pending messages
    Message msg;
    while ((msg = getMessage()).type != MSG_NONE) {
        propagateMessage(&msg);
    }
}

void
MessageQueue::removeListener(const void *listener)
{
    pthread_mutex_lock(&lock);
    listeners.erase(listener);
    pthread_mutex_unlock(&lock);
}

Message
MessageQueue::getMessage()
{ 
	Message result;

	pthread_mutex_lock(&lock);	

	// Read message
	if (r == w) {
		result.type = MSG_NONE; // Queue is empty
        result.data = 0;
	} else {
        result = queue[r];
        r = (r + 1) % capacity;
	}
		
	pthread_mutex_unlock(&lock);
	
	return result; 
}

void
MessageQueue::putMessage(MessageType type, u64 data)
{
	pthread_mutex_lock(&lock);
		
	// Write data
    Message msg;
    msg.type = type;
    msg.data = data;
    queue[w] = msg;
    
	// Move write pointer to next location
	w = (w + 1) % capacity;

	if (w == r) {
        debug(MSG_DEBUG, "Queue overflow. Oldest message is lost.\n");
		r = (r + 1) % capacity;
	}
    
    // Serve registered callbacks
    propagateMessage(&msg);

	pthread_mutex_unlock(&lock);
}

void
MessageQueue::propagateMessage(Message *msg)
{
    map <const void *, Callback *> :: iterator i;
    
    for (i = listeners.begin(); i != listeners.end(); i++) {
        i->second(i->first, msg->type, msg->data);
    }
}
