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

void
MessageQueue::printMessage(Message *msg)
{
	switch (msg->id) {
		case MSG_ROM_LOADED:
			debug(2, "MSG_ROM_LOADED");
			break;
		case MSG_ROM_MISSING:
			debug(2, "MSG_ROM_MISSING");
			break;
		case MSG_RUN:
			debug(2, "MSG_RUN");
			break;
		case MSG_HALT:
			debug(2, "MSG_HALT");
			break;
		case MSG_CPU:
			debug(2, "MSG_CPU");
			break;
		case MSG_WARP:
			debug(2, "MSG_WARP");
			break;
		case MSG_LOG:
			debug(2, "MSG_LOG");
			break;
		case MSG_VC1541_ATTACHED:
			debug(2, "MSG_VC1541_ATTACHED");
			break;
		case MSG_VC1541_DISK:
			debug(2, "MSG_VC_1541_DISK");
			break;
		case MSG_VC1541_LED:
			debug(2, "MSG_VC1541_LED");
			break;
		case MSG_VC1541_DATA:
			debug(2, "MSG_VC1541_DATA");
			break;
		case MSG_VC1541_MOTOR:
			debug(2, "MSG_VC1541_MOTOR");
			break;
		case MSG_CARTRIDGE:
			debug(2, "MSG_CARTRIDGE");
			break;
        case MSG_JOYSTICK_ATTACHED:
            debug(2, "MSG_JOYSTICK_ATTACHED");
            break;
        case MSG_JOYSTICK_REMOVED:
            debug(2, "MSG_JOYSTICK_REMOVED");
            break;
        case MSG_PAL:
            debug(2, "MSG_PAL");
            break;
        case MSG_NTSC:
            debug(2, "MSG_NTSC");
            break;
		default:
			assert(0);
	}
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
MessageQueue::putMessage(int id, int i, void *p, const char *c)
{ 
	pthread_mutex_lock(&lock);
		
	// Write data
	queue[w].id = id; 
	queue[w].i = i; 
	queue[w].p = p;
    
	if (c != NULL)
        strncpy(queue[w].c, c, 128);
	 	
	// Move write pointer to next location
	w = (w + 1) % queue_size;

	if (w == r) {
		warn("Queue overflow!!! Message is lost!!!\n");
		r = (r + 1) % queue_size;
	} 
	
	pthread_mutex_unlock(&lock);
}
