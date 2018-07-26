/*
 *  Message.cpp
 *  V64
 *
 *  Created by Dirk Hoffmann on 25.12.09.
 *  Copyright 2009 Hochschule Karlsruhe. All rights reserved.
 *
 */

#include "Message.h"

MessageQueue::MessageQueue()
{
    setDescription("MessageQueue");
	r = w = 0;
    listener = NULL;
    callback = NULL;
	pthread_mutex_init(&lock, NULL);
}

MessageQueue::~MessageQueue()
{
	pthread_mutex_destroy(&lock);
}
    
void
MessageQueue::setListener(const void *sender, void(*func)(const void *, int, long)) {

    pthread_mutex_lock(&lock);
    
    listener = sender;
    callback = func;
    
    pthread_mutex_unlock(&lock);
    
    // Process all pending messages
    Message msg;
    while (callback && (msg = getMessage()).type != MSG_NONE) {
        callback(listener, msg.type, msg.data);
    }
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
        r = (r + 1) % queue_size;
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
	w = (w + 1) % queue_size;

	if (w == r) {
        // debug(2, "Queue overflow. Oldest message is lost.\n");
		r = (r + 1) % queue_size;
	}
    
    // Call listener function
    if (callback) {
        callback(listener, type, data);
    }
    
	pthread_mutex_unlock(&lock);
}
