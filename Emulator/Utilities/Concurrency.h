// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include <pthread.h>
#include <future>

namespace util {

class Mutex
{
    pthread_mutex_t mutex;

public:
    
    Mutex();
    ~Mutex();
    
    int lock();
    int unlock();
};

class ReentrantMutex
{
    pthread_mutex_t mutex;

public:
    
    ReentrantMutex();
    ~ReentrantMutex();
    
    int lock();
    int unlock();
};

class AutoMutex
{
    ReentrantMutex &mutex;

public:

    bool active = true;

    AutoMutex(ReentrantMutex &ref) : mutex(ref) { mutex.lock(); }
    ~AutoMutex() { mutex.unlock(); }
};

class Wakeable
{
#ifdef USE_CONDITION_VARIABLE
    
    std::mutex condMutex;
    std::condition_variable cond;
    bool condFlag = false;
    
#else
    
    std::promise<int> promise;
    std::future<int> future = promise.get_future();

#endif
    
public:

    void waitForWakeUp();
    void wakeUp();
};

}
