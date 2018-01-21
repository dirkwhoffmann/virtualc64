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

Message *
MessageQueue::getMessage()
{ 
	Message *result;

	pthread_mutex_lock(&lock);	

	// Get data
	if (r == w) {
		result = NULL; // Queue is empty!
	} else {
		result = (r == w) ? NULL : &queue[r];
	}
	
	// Move read pointer to next location
	if (result) r = (r + 1) % queue_size;
		
	pthread_mutex_unlock(&lock);
	
	return result; 
}

void
MessageQueue::putMessage(int id, int i)
{
	pthread_mutex_lock(&lock);
		
	// Write data
	queue[w].id = id; 
	queue[w].i = i; 
    
	// Move write pointer to next location
	w = (w + 1) % queue_size;

	if (w == r) {
		warn("Queue overflow!!! Message is lost!!!\n");
		r = (r + 1) % queue_size;
	} 
	
	pthread_mutex_unlock(&lock);
}
