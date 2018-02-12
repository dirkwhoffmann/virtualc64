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
	pthread_mutex_init(&lock, NULL);
}

MessageQueue::~MessageQueue()
{
	pthread_mutex_destroy(&lock);
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
	
	pthread_mutex_unlock(&lock);
}
