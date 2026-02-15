// -----------------------------------------------------------------------------
// This file is part of utlib - A lightweight utility library
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#include "utl/chrono/Time.h"
#include <chrono>
#include <ctime>
#include <thread>

namespace utl {

#if defined(__MACH__)

//
// MacOS
//

#include <mach/mach_time.h>

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

std::tm
Time::gmtime(const std::time_t &time)
{
    std::tm gmtime {};
    gmtime_r(&time, &gmtime);

    return gmtime;
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

std::tm
Time::gmtime(const std::time_t &time)
{
    std::tm gmtime {};
    gmtime_r(&time, &gmtime);

    return gmtime;
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

std::tm
Time::gmtime(const std::time_t &time)
{
    std::tm gmtime {};
    gmtime_s(&gmtime, &time);

    return gmtime;
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
Time::operator*(const long i) const
{
    return Time(this->ticks * i);
}

Time
Time::operator*(const double d) const
{
    return Time(i64(this->ticks * d));
}

Time
Time::operator/(const long i) const
{
    return Time(this->ticks / i);
}

Time
Time::operator/(const double d) const
{
    return Time(i64(this->ticks / d));
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
Time::operator*=(const long i)
{
    return *this = *this * i;
}

Time&
Time::operator*=(const double d)
{
    return *this = *this * d;
}

Time&
Time::operator/=(const long i)
{
    return *this = *this / i;
}

Time&
Time::operator/=(const double d)
{
    return *this = *this / d;
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

}
