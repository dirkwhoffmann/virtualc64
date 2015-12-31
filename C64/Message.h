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

#include "VC64Object.h"

// Queue size
#define QUEUE_SIZE 64

// Message types
enum {
    MSG_ROM_LOADED = 1,
    MSG_ROM_MISSING,
    MSG_ROM_COMPLETE,
    MSG_RUN,
    MSG_HALT,
    MSG_CPU,
    MSG_WARP,
    MSG_ALWAYS_WARP,
    MSG_LOG,
    MSG_VC1541_ATTACHED,
    MSG_VC1541_ATTACHED_SOUND,
    MSG_VC1541_DISK,
    MSG_VC1541_DISK_SOUND,
    MSG_VC1541_LED,
    MSG_VC1541_DATA,
    MSG_VC1541_MOTOR,
    MSG_VC1541_HEAD,
    MSG_VC1541_HEAD_SOUND,
    MSG_CARTRIDGE,
    MSG_VC1530_TAPE,
    MSG_VC1530_PLAY,
    MSG_VC1530_PROGRESS,
    MSG_JOYSTICK_ATTACHED,
    MSG_JOYSTICK_REMOVED,
    MSG_PAL,
    MSG_NTSC
};


typedef struct {
	int id;			// Message ID
	char c[128];	// Text message
	int i;			// Integer value
	void *p;		// Pointer value
} Message;

class MessageQueue : public VC64Object {
	
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
