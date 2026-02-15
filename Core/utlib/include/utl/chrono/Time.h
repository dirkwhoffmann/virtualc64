// -----------------------------------------------------------------------------
// This file is part of utlib - A lightweight utility library
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include "utl/common.h"

namespace utl {

class Time {

public:

    i64 ticks = 0;

public:

    static Time now();
    static Time nanoseconds(i64 value) { return Time(value); }
    static Time microseconds(i64 value) { return Time(value * 1000); }
    static Time milliseconds(i64 value)  { return Time(value * 1000000); }
    static Time seconds(i64 value) { return Time(value * 1000000000); }
    static Time seconds(double value) { return Time(i64(value * 1000000000.f)); }
    static std::tm local(const std::time_t &time);
    static std::tm gmtime(const std::time_t &time);

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
    Time operator*(const long i) const;
    Time operator*(const double d) const;
    Time operator/(const long i) const;
    Time operator/(const double d) const;
    Time& operator+=(const Time &rhs);
    Time& operator-=(const Time &rhs);
    Time& operator*=(const long i);
    Time& operator*=(const double d);
    Time& operator/=(const long i);
    Time& operator/=(const double d);
    Time abs() const;
    Time diff() const;

    void sleep();
    void sleepUntil();
};

}
