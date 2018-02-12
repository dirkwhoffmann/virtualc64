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
MessageQueue::setListener(const void *sender, void(*func)(const void *, int)) {

    pthread_mutex_lock(&lock);
    
    listener = sender;
    callback = func;
    
    pthread_mutex_unlock(&lock);
    
    // Process all pending messages
    VC64Message msg;
    while ((msg = getMessage()) != MSG_NONE) {
        callback(listener, msg);
    }
}

VC64Message
MessageQueue::getMessage()
{ 
	VC64Message result;

	pthread_mutex_lock(&lock);	

	// Read message
	if (r == w) {
		result = MSG_NONE; // Queue is empty
	} else {
        result = queue[r];
        r = (r + 1) % queue_size;
	}
		
	pthread_mutex_unlock(&lock);
	
	return result; 
}

void
MessageQueue::putMessage(VC64Message msg)
{
	pthread_mutex_lock(&lock);
		
	// Write data
    queue[w] = msg;
    
	// Move write pointer to next location
	w = (w + 1) % queue_size;

	if (w == r) {
        // debug(2, "Queue overflow. Oldest message is lost.\n");
		r = (r + 1) % queue_size;
	}
    
    // Call listener function
    if (callback) {
        callback(listener, msg);
    }
    
	pthread_mutex_unlock(&lock);
}
