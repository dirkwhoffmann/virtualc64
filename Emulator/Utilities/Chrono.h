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

#include "Types.h"
#include <ctime>

namespace util {

class Time {
    
public:
    
    i64 ticks = 0;

public:
    
    static Time now();
    static std::tm local(const std::time_t &time);
    
    Time() { };
    Time(i64 value) : ticks(value) { };
    
    i64 asNanoseconds()  const { return ticks; }
    i64 asMicroseconds() const { return ticks / 1000; }
    i64 asMilliseconds() const { return ticks / 1000000; }
    float asSeconds()    const { return ticks / 1000000000.f; }
    
    bool operator==(const Time &rhs) const;
    bool operator!=(const Time &rhs) const;
    bool operator<=(const Time &rhs) const;
    bool operator>=(const Time &rhs) const;
    bool operator<(const Time &rhs) const;
    bool operator>(const Time &rhs) const;
    Time operator+(const Time &rhs) const;
    Time operator-(const Time &rhs) const;
    Time operator*(const int i) const;
    Time& operator+=(const Time &rhs);
    Time& operator-=(const Time &rhs);
    Time& operator*=(const int i);
    Time abs() const;
    Time diff() const;
    
    void sleep();
    void sleepUntil();
};

class Clock {
        
    Time start;
    Time elapsed;
    
    bool paused = false;

    void updateElapsed();

public:
    
    Clock();

    Time getElapsedTime();
    
    Time stop();
    Time go();
    Time restart();
};

class StopWatch {

    string description;
    Clock clock;

public:
    
    StopWatch(const string &description = "");
    ~StopWatch();
};

#define MEASURE_TIME(x) util::StopWatch _watch(x);
}
