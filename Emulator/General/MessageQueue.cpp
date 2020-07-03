/*!
 * @file        MessageQueue.cpp
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   Dirk W. Hoffmann. All rights reserved.
 */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

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
MessageQueue::putMessage(MessageType type, uint64_t data)
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
        // debug(2, "Queue overflow. Oldest message is lost.\n");
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
