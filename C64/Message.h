/*!
 * @header      Message.h
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   2010 - 2016 Dirk W. Hoffmann
 */
/*              This program is free software; you can redistribute it and/or modify
 *              it under the terms of the GNU General Public License as published by
 *              the Free Software Foundation; either version 2 of the License, or
 *              (at your option) any later version.
 *
 *              This program is distributed in the hope that it will be useful,
 *              but WITHOUT ANY WARRANTY; without even the implied warranty of
 *              MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *              GNU General Public License for more details.
 *
 *              You should have received a copy of the GNU General Public License
 *              along with this program; if not, write to the Free Software
 *              Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _MESSAGE_INC
#define _MESSAGE_INC

#include "VC64Object.h"

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
    
    //! @brief    Maximum number of queued messages
    const static unsigned queue_size = 64;
    
    //! @brief    Message queue ring buffer
	Message queue[queue_size];
	
	//! @brief    The ring buffers read pointer
	int r; 
	
    //! @brief    The ring buffers read pointer
	int w;
		
	//! @brief    Mutex for streamlining parallel read and write accesses
	pthread_mutex_t lock;  
			
	//! @brief    Prints a textual description of the message for debugging
	void printMessage(Message *msg);

public:
	//! @brief    Constructor
	MessageQueue();
	
	//! @brief    Destructor
	~MessageQueue();

	/*! @brief    Returns the next pending message
     *  @return   Returns NULL, if the queue is empty
     */
	Message *getMessage();

	//! @brief   Adds new message to queue
	void putMessage(int id, int i = 0, void *p = NULL, const char *c = NULL);
};

#endif
