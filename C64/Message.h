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
#include "C64_defs.h"

class MessageQueue : public VC64Object {
	
private:
    
    //! @brief    Maximum number of queued messages
    const static unsigned queue_size = 64;
    
    //! @brief    Message queue ring buffer
	VC64Message queue[queue_size];
	
	//! @brief    The ring buffers read pointer
	int r; 
	
    //! @brief    The ring buffers read pointer
	int w;
		
	//! @brief    Mutex for streamlining parallel read and write accesses
	pthread_mutex_t lock;
    
    //! @brief    Callback function
    /*! @details  If set, the function is called whenever a message is put into the queue
     */
    void(*callback)(const void *, int);

    //! @brief    Registered listener
    /*! @details  This value is passed back into the registered callback
     */
    const void *listener;

public:
	//! @brief    Constructor
	MessageQueue();
	
	//! @brief    Destructor
	~MessageQueue();

    //! @brief    Registers a listener callback function
    void setListener(const void *sender, void(*func)(const void *, int));
    
	/*! @brief    Returns the next pending message
     *  @return   Returns NULL, if the queue is empty
     */
	VC64Message getMessage();

	//! @brief    Writes new message into the message queue
    /*! @detals   If a callback function is set, the functions is invoked.
     */
	// void putMessage(int id, int i = 0);
    void putMessage(VC64Message id);
};

#endif
