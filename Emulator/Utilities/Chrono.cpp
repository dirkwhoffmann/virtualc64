// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "Chrono.h"
#include <chrono>
#include <thread>

#ifdef __MACH__
#include <mach/mach_time.h>
#endif


namespace util {

#if defined(__MACH__)

//
// MacOS
//

static struct mach_timebase_info timebaseInfo()
{
    struct mach_timebase_info tb;
    mach_timebase_info(&tb);
    return tb;
}

Time
Time::now()
{
    static struct mach_timebase_info tb = timebaseInfo();
    return (i64)mach_absolute_time() * tb.numer / tb.denom;
}

std::tm
Time::local(const std::time_t &time)
{
    std::tm local {};
    localtime_r(&time, &local);
    
    return local;
}
    
void
Time::sleep()
{
    static struct mach_timebase_info tb = timebaseInfo();
    if (ticks > 0) {
        mach_wait_until(now().asNanoseconds() + (ticks * tb.denom / tb.numer));
    }
}

void
Time::sleepUntil()
{
    static struct mach_timebase_info tb = timebaseInfo();
    mach_wait_until(ticks * tb.denom / tb.numer);
}

#elif defined(__unix__)

//
// Unix
//

Time
Time::now()
{
    struct timespec ts;
    (void)clock_gettime(CLOCK_MONOTONIC, &ts);
    return (i64)ts.tv_sec * 1000000000 + ts.tv_nsec;
}

std::tm
Time::local(const std::time_t &time)
{
    std::tm local {};
    localtime_r(&time, &local);
    
    return local;
}

void
Time::sleep()
{
    struct timespec req, rem;
    
    if (ticks > 0) {
        req.tv_sec = 0;
        req.tv_nsec = ticks;
        nanosleep(&req, &rem);
    }
}

void
Time::sleepUntil()
{
    (*this - now()).sleep();
}

#else
    
//
// Generic
//

Time
Time::now()
{
    const auto now = std::chrono::steady_clock::now();
    static auto start = now;
    return std::chrono::nanoseconds(now - start).count();
}

std::tm
Time::local(const std::time_t &time)
{
    std::tm local {};
    localtime_s(&local, &time);
    
    return local;
}

void
Time::sleep()
{
    if (ticks > 0)
        std::this_thread::sleep_for(std::chrono::nanoseconds(ticks));
}

void
Time::sleepUntil()
{
    (*this - now()).sleep();
}

#endif


//
// All platforms
//

bool
Time::operator==(const Time &rhs) const
{
    return this->asNanoseconds() == rhs.asNanoseconds();
}

bool
Time::operator!=(const Time &rhs) const
{
    return this->asNanoseconds() == rhs.asNanoseconds();
}

bool
Time::operator<=(const Time &rhs) const
{
    return this->asNanoseconds() <= rhs.asNanoseconds();
}

bool
Time::operator>=(const Time &rhs) const
{
    return this->asNanoseconds() >= rhs.asNanoseconds();
}

bool
Time::operator<(const Time &rhs) const
{
    return this->asNanoseconds() < rhs.asNanoseconds();
}

bool
Time::operator>(const Time &rhs) const
{
    return this->asNanoseconds() > rhs.asNanoseconds();
}

Time
Time::operator+(const Time &rhs) const
{
    return Time(this->ticks + rhs.ticks);
}

Time
Time::operator-(const Time &rhs) const
{
    return Time(this->ticks - rhs.ticks);
}

Time
Time::operator*(const int i) const
{
    return Time(i * this->ticks);
}

Time&
Time::operator+=(const Time &rhs)
{
    return *this = *this + rhs;
}

Time&
Time::operator-=(const Time &rhs)
{
    return *this = *this - rhs;
}

Time&
Time::operator*=(const int i)
{
    return *this = *this * i;
}

Time
Time::abs() const
{
    return Time(ticks >= 0 ? ticks : -ticks);
}

Time
Time::diff() const {
    return *this - now();
}

Clock::Clock()
{
    start = Time::now();
}

void
Clock::updateElapsed()
{
    auto now = Time::now();
    if (!paused) elapsed += now - start;
    start = now;
}

Time
Clock::getElapsedTime()
{
    updateElapsed();
    return elapsed;
}

Time
Clock::stop()
{
    updateElapsed();
    paused = true;
    return elapsed;
}

Time
Clock::go()
{
    updateElapsed();
    paused = false;
    return elapsed;
}

Time
Clock::restart()
{
    updateElapsed();
    auto result = elapsed;

    start = Time::now();
    elapsed = 0;
    paused = false;
    
    return result;
}

StopWatch::StopWatch(const string &description) : description(description)
{
    clock.restart();
}

StopWatch::~StopWatch()
{
    auto elapsed = clock.stop();
    fprintf(stderr, "%s: %f sec\n", description.c_str(), elapsed.asSeconds());
}

}
