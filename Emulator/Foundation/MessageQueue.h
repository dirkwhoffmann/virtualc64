// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _MESSAGE_QUEUE_INC
#define _MESSAGE_QUEUE_INC

#include "C64Object.h"
#include "C64Types.h"
#include <map>

using namespace std;

class MessageQueue : public C64Object {
    
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
    void putMessage(MessageType type, u64 data = 0);
    
    private:
    
    //! @brief    Propagates a single message to all registered listeners.
    void propagateMessage(Message *msg);
};

#endif
