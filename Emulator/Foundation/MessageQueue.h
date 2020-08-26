// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _MESSAGE_QUEUE_H
#define _MESSAGE_QUEUE_H

#include "C64Object.h"
#include "C64Types.h"
#include <map>

using namespace std;

class MessageQueue : public C64Object {
    
    private:
    
    // Maximum number of queued messages
    const static size_t capacity = 64;
    
    // Message ring buffer
    Message queue[capacity];
    
    // Read and write pointer
    int r = 0;
    int w = 0;
    
    //! @brief Mutex for streamlining parallel read and write accesses
    // TODO: Use synchronized
    pthread_mutex_t lock;
    
    // List of all registered listeners
    map <const void *, Callback *> listeners;
    
    public:
    
    MessageQueue();
    virtual ~MessageQueue();
    
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
    void putMessage(MessageType type, u64 data = 0);
    
    private:
    
    //! @brief    Propagates a single message to all registered listeners.
    void propagateMessage(Message *msg);
};

#endif
