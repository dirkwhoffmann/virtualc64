/*!
 * @header      MessageQueue.h
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   Dirk W. Hoffmann, all rights reserved.
 */
/*
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

#ifndef _MESSAGE_QUEUE_INC
#define _MESSAGE_QUEUE_INC

#include "VC64Object.h"
#include "C64_types.h"
#include <map>

using namespace std;

class MessageQueue : public VC64Object {
    
    private:
    
    //! @brief    Maximum number of queued messages
    const static size_t capacity = 64;
    
    //! @brief    Message ring buffer
    Message queue[capacity];
    
    //! @brief    The ring buffers read pointer
    int r = 0;
    
    //! @brief    The ring buffers write pointer
    int w = 0;
    
    //! @brief    Mutex for streamlining parallel read and write accesses
    pthread_mutex_t lock;
    
    //! @brief    A list of all registered listeners
    map <const void *, Callback *> listeners;
    
    public:
    
    //! @brief    Constructor
    MessageQueue();
    
    //! @brief    Destructor
    ~MessageQueue();
    
    //! @brief    Registers a listener together with it's callback function
    void addListener(const void *listener, Callback *func);
    
    //! @brief    Removes a listener
    void removeListener(const void *listener);
    
    /*! @brief    Returns the next pending message
     *  @return   NULL, if the queue is empty
     */
    Message getMessage();
    
    /*! @brief    Writes new message into the message queue
     *  @details  Furthermore, the message is propaged to all registered
     *            listeners.
     *  @see      propagateMessage
     */
    void putMessage(MessageType type, uint64_t data = 0);
    
    private:
    
    //! @brief    Propagates a single message to all registered listeners.
    void propagateMessage(Message *msg);
};

#endif
