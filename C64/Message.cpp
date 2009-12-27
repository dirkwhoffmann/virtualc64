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
	r = w = 0;
	pthread_mutex_init(&lock, NULL);
}

MessageQueue::~MessageQueue()
{
	pthread_mutex_destroy(&lock);
}

void MessageQueue::printMessage(Message *msg)
{
	switch (msg->id) {
		case MSG_ROM_LOADED:
			fprintf(stderr, "MSG_ROM_LOADED");
			break;
		case MSG_ROM_MISSING:
			fprintf(stderr, "MSG_ROM_MISSING");
			break;
		case MSG_RUN:
			fprintf(stderr, "MSG_RUN");
			break;
		case MSG_HALT:
			fprintf(stderr, "MSG_HALT");
			break;
		case MSG_DRAW:
			fprintf(stderr, "MSG_DRAW");
			break;
		case MSG_CPU:
			fprintf(stderr, "MSG_CPU");
			break;
		case MSG_WARP:
			fprintf(stderr, "MSG_WARP");
			break;
		case MSG_LOG:
			fprintf(stderr, "MSG_LOG");
			break;
		case MSG_VC1541_ATTACHED:
			fprintf(stderr, "MSG_VC1541_ATTACHED");
			break;
		case MSG_VC1541_DISC:
			fprintf(stderr, "MSG_VC_1541_DISC");
			break;
		case MSG_VC1541_LED:
			fprintf(stderr, "MSG_VC1541_LED");
			break;
		case MSG_VC1541_DATA:
			fprintf(stderr, "MSG_VC1541_DATA");
			break;
		case MSG_VC1541_MOTOR:
			fprintf(stderr, "MSG_VC1541_MOTOR");
			break;
		default:
			assert(0);
	}
}

Message *MessageQueue::getMessage() 
{ 
	Message *result;

	pthread_mutex_lock(&lock);	

	// Get data
	if (r == w) {
		// Queue is empty!
		// fprintf(stderr, "MessageQueue::read: Queue is empty!\n");
		result = NULL;
	} else {
		result = (r == w) ? NULL : &queue[r]; 
#if 0		
		if (result->id != MSG_DRAW) {
			fprintf(stderr, "getMessage: "); printMessage(&queue[r]); fprintf(stderr,"\n");
		}
#endif
	}
	
	// Move read pointer to next location
	if (result) r = (r + 1) % QUEUE_SIZE;	
		
	pthread_mutex_unlock(&lock);
	
	return result; 
}

void MessageQueue::putMessage(int id, int i, void *p, const char *c) 
{ 
	// If queue gets filled up, we don't accept any periodic messages any more...
	if (queueGetsFilledUp() && id == MSG_DRAW) {
		return;
	}

	pthread_mutex_lock(&lock);	
		
	// Write data
	queue[w].id = id; 
	queue[w].i = i; 
	queue[w].p = p; 
	if (c != NULL) strncpy(queue[w].c, c, 128); 
#if 0	
	if (id != MSG_DRAW) {		 
		fprintf(stderr, "putMessage: "); printMessage(&queue[w]); fprintf(stderr,"\n");
	}
#endif
	
	// Move write pointer to next location
	w = (w + 1) % QUEUE_SIZE;

	if (w == r) {
		fprintf(stderr, "putMessag: Queue overflow!!! Message is lost!!!\n");
		assert(0);
		r = (r + 1) % QUEUE_SIZE;
	} 
	
	pthread_mutex_unlock(&lock);
}
