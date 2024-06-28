// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// This FILE is dual-licensed. You are free to choose between:
//
//     - The GNU General Public License v3 (or any later version)
//     - The Mozilla Public License v2
//
// SPDX-License-Identifier: GPL-3.0-or-later OR MPL-2.0
// -----------------------------------------------------------------------------

#pragma once

#include "MsgQueueTypes.h"
#include "CoreObject.h"
#include "Synchronizable.h"
#include "RingBuffer.h"

namespace vc64 {

class MsgQueue final : CoreObject, Synchronizable {

    // Ring buffer storing all pending messages
    util::RingBuffer <Message, 128> queue;

    // The registered listener
    const void *listener = nullptr;
    
    // The registered callback function
    Callback *callback = nullptr;

    // If disabled, no messages will be stored
    bool enabled = true;

    
    //
    // Methods
    //
    
private:

    const char *objectName() const override { return "MsgQueue"; }


    //
    // Managing the queue
    //
    
public:
    
    // Registers a listener together with it's callback function
    void setListener(const void *listener, Callback *func);

    // Disables the message queue
    void disable() { enabled = false; }

    // Sends a message
    void put(const Message &msg);
    void put(MsgType type, i64 payload = 0);
    void put(MsgType type, CpuMsg payload);
    void put(MsgType type, DriveMsg payload);
    void put(MsgType type, ScriptMsg payload);

    // Reads a message
    bool get(Message &msg);
};

}
