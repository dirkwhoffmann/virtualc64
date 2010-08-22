/*
 * (C) 2010 Dirk W. Hoffmann. All rights reserved.
 *
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

#ifndef _MESSAGE_INC
#define _MESSAGE_INC

#include "basic.h"

// Queue size
#define QUEUE_SIZE 32

// Message types
#define MSG_ROM_LOADED 1
#define MSG_ROM_MISSING 2
#define MSG_RUN 3
#define MSG_HALT 4
//#define MSG_DRAW 5
#define MSG_CPU 6
#define MSG_WARP 7
#define MSG_LOG 8
#define MSG_VC1541_ATTACHED 9
#define MSG_VC1541_DISC 10
#define MSG_VC1541_LED 11
#define MSG_VC1541_DATA 12
#define MSG_VC1541_MOTOR 13
#define MSG_CARTRIDGE 14

typedef struct {
	int id;			// Message ID
	char c[128];	// Text message
	int i;			// Integer value
	void *p;		// Pointer value
} Message;

class MessageQueue {
	
private:
	Message queue[QUEUE_SIZE];
	
	// Read pointer
	int r; 
	
	// Write pointer
	int w;
		
	// Mutex for protecting the message queue
	pthread_mutex_t lock;  
			
	// Print message contents to stderr
	void printMessage(Message *msg);

public:
	//! Constructor
	MessageQueue();
	
	//! Destructor
	~MessageQueue();

	// Returns number of messages in queue
	int numMessagesInQueue() { return (w >= r) ? w - r : (w + QUEUE_SIZE) - r; };

	// Returns true, iff message queue is full
	bool queueIsFull() { return numMessagesInQueue() == QUEUE_SIZE - 1; }

	// Returns true, iff message queue gets filled up
	bool queueGetsFilledUp() { return numMessagesInQueue() > (QUEUE_SIZE / 2); }

	// Get next message from queue
	Message *getMessage();

	// Put message into queue
	void putMessage(int id, int i = 0, void *p = NULL, const char *c = NULL);
};

#endif
