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

#include "Macros.h"
#include "MemUtils.h"
#include "Buffer.h"
#include <vector>

namespace vc64 {

class Serializable {

public:

    virtual ~Serializable() = default;

    // Serializers (to be implemented by the subclass)
    virtual void operator << (class SerCounter &worker) = 0;
    virtual void operator << (class SerChecker &worker) = 0;
    virtual void operator << (class SerResetter &worker) = 0;
    virtual void operator << (class SerReader &worker) = 0;
    virtual void operator << (class SerWriter &worker) = 0;

    // Zeroes out all items
    void hardReset();
    void softReset();

    // Calculates the snapshot size
    isize size();

    // Computes a checksum
    u64 checksum();

    // Recreates the component from a buffer
    isize load(const u8 *buffer);

    // Serializes the component into a buffer
    isize save(u8 *buffer);
};


//
// Basic memory buffer I/O
//

inline u8 read8(const u8 *& buf)
{
    u8 result = R8BE(buf);
    buf += 1;
    return result;
}

inline u16 read16(const u8 *& buf)
{
    u16 result = R16BE(buf);
    buf += 2;
    return result;
}

inline u32 read32(const u8 *& buf)
{
    u32 result = R32BE(buf);
    buf += 4;
    return result;
}

inline u64 read64(const u8 *& buf)
{
    u32 hi = read32(buf);
    u32 lo = read32(buf);
    return ((u64)hi << 32) | lo;
}

inline double readDouble(const u8 *& buf)
{
    double result = 0;
    for (isize i = 0; i < 8; i++) ((u8 *)&result)[i] = read8(buf);
    return result;
}

inline string readString(const u8 *& buf)
{
    u8 len = read8(buf);
    string result = string((const char *)buf, len);
    buf += len;
    return result;
}

inline void write8(u8 *& buf, u8 value)
{
    W8BE(buf, value);
    buf += 1;
}

inline void write16(u8 *& buf, u16 value)
{
    W16BE(buf, value);
    buf += 2;
}

inline void write32(u8 *& buf, u32 value)
{
    W32BE(buf, value);
    buf += 4;
}

inline void write64(u8 *& buf, u64 value)
{
    write32(buf, (u32)(value >> 32));
    write32(buf, (u32)(value));
}

inline void writeDouble(u8 *& buf, double value)
{
    for (isize i = 0; i < 8; i++) write8(buf, ((u8 *)&value)[i]);
}

inline void writeString(u8 *& buf, string value)
{
    auto len = value.length();
    write8(buf, u8(len));
    value.copy((char *)buf, len);
    buf += len;
}


//
// Counter (determines the state size)
//

#define COUNT(type,size) \
auto& operator<<(type& v) \
{ \
count += size; \
return *this; \
}

#define COUNT8(type) static_assert(sizeof(type) == 1); COUNT(type,1)
#define COUNT16(type) static_assert(sizeof(type) == 2); COUNT(type,2)
#define COUNT64(type) static_assert(sizeof(type) <= 8); COUNT(type,8)
#define COUNTD(type) static_assert(sizeof(type) <= 8); COUNT(type,8)

class SerCounter
{
public:

    isize count;

    SerCounter() { count = 0; }

    COUNT8(const bool)
    COUNT8(const char)
    COUNT8(const signed char)
    COUNT8(const unsigned char)
    COUNT16(const short)
    COUNT16(const unsigned short)
    COUNT64(const int)
    COUNT64(const unsigned int)
    COUNT64(const long)
    COUNT64(const unsigned long)
    COUNT64(const long long)
    COUNT64(const unsigned long long)
    COUNTD(const float)
    COUNTD(const double)
       
    template <class T>
    auto& operator<<(util::Allocator<T> &a)
    {
        count += 8 + a.size;
        return *this;
    }
    
    auto& operator<<(string &v)
    {
        auto len = v.length();
        count += 1 + isize(len);
        return *this;
    }

    template <class T>
    auto& operator<<(std::vector<T> &v)
    {
        auto len = v.size();
        for(usize i = 0; i < len; i++) *this << v[i];
        count += 8;
        return *this;
    }

    template <class T, isize N>
    SerCounter& operator<<(T (&v)[N])
    {
        for(isize i = 0; i < N; ++i) {
            *this << v[i];
        }
        return *this;
    }

    template <class E, class = std::enable_if_t<std::is_enum<E>{}>>
    SerCounter& operator<<(E &v)
    {
        count += sizeof(u64);
        return *this;
    }

    template <std::derived_from<Serializable> T>
    SerCounter& operator<<(T &v)
    {
        v << *this;
        return *this;
    }
};


//
// Checksum generator
//

#define CHECK(type) \
auto& operator<<(type& v) \
{ \
hash = util::fnvIt64(hash, (u64)v); \
return *this; \
}

class SerChecker
{
public:

    u64 hash;

    SerChecker() { hash = util::fnvInit64(); }

    CHECK(const bool)
    CHECK(const char)
    CHECK(const signed char)
    CHECK(const unsigned char)
    CHECK(const short)
    CHECK(const unsigned short)
    CHECK(const int)
    CHECK(const unsigned int)
    CHECK(const long)
    CHECK(const unsigned long)
    CHECK(const long long)
    CHECK(const unsigned long long)
    CHECK(const float)
    CHECK(const double)
       
    template <class T>
    auto& operator<<(util::Allocator<T> &a)
    {
        hash = util::fnvIt64(hash, a.fnv64());
        return *this;
    }
        
    auto& operator<<(string &v)
    {
        auto len = v.length();
        for (usize i = 0; i < len; i++) {
            hash = util::fnvIt64(hash, v[i]);
        }
        return *this;
    }

    template <class T>
    auto& operator<<(std::vector<T> &v)
    {
        isize len = isize(v.size());
        for (isize i = 0; i < len; i++) {
            *this << v[i];
        }
        return *this;
    }

    template <class T, isize N>
    SerChecker& operator<<(T (&v)[N])
    {
        for(isize i = 0; i < N; ++i) {
            *this << v[i];
        }
        return *this;
    }

    template <class E, class = std::enable_if_t<std::is_enum<E>{}>>
    SerChecker& operator<<(E &v)
    {
        hash = util::fnvIt64(hash, v);
        return *this;
    }

    template <std::derived_from<Serializable> T>
    SerChecker& operator<<(T &v)
    {
        v << *this;
        return *this;
    }
};


//
// Reader (Deserializer)
//

#define DESERIALIZE(type,function) \
SerReader& operator<<(type& v) \
{ \
v = (type)function(ptr); \
return *this; \
}

#define DESERIALIZE8(type)  static_assert(sizeof(type) == 1); DESERIALIZE(type,read8)
#define DESERIALIZE16(type) static_assert(sizeof(type) == 2); DESERIALIZE(type,read16)
#define DESERIALIZE64(type) static_assert(sizeof(type) <= 8); DESERIALIZE(type,read64)
#define DESERIALIZED(type) static_assert(sizeof(type) <= 8); DESERIALIZE(type,readDouble)

class SerReader
{
public:

    const u8 *ptr;

    SerReader(const u8 *p) : ptr(p)
    {
    }

    DESERIALIZE8(bool)
    DESERIALIZE8(char)
    DESERIALIZE8(signed char)
    DESERIALIZE8(unsigned char)
    DESERIALIZE16(short)
    DESERIALIZE16(unsigned short)
    DESERIALIZE64(int)
    DESERIALIZE64(unsigned int)
    DESERIALIZE64(long)
    DESERIALIZE64(unsigned long)
    DESERIALIZE64(long long)
    DESERIALIZE64(unsigned long long)
    DESERIALIZED(float)
    DESERIALIZED(double)

    template <class T>
    auto& operator<<(util::Allocator<T> &a)
    {
        i64 len;
        *this << len;
        a.init(ptr, isize(len));
        ptr += len;
        return *this;
    }

    auto& operator<<(string &v)
    {
        v = readString(ptr);
        return *this;
    }

    template <class T>
    auto& operator<<(std::vector<T> &v)
    {
        i64 len;
        *this << len;
        v.clear();
        v.reserve(len);
        for (isize i = 0; i < len; i++) {
            v.push_back(T());
            *this << v.back();
        }
        return *this;
    }

    template <class T, isize N>
    SerReader& operator<<(T (&v)[N])
    {
        for(isize i = 0; i < N; ++i) {
            *this << v[i];
        }
        return *this;
    }

    void copy(void *dst, isize n)
    {
        std::memcpy(dst, (void *)ptr, n);
        ptr += n;
    }

    template <class E, class = std::enable_if_t<std::is_enum<E>{}>>
    SerReader& operator<<(E &v)
    {
        v = (E)read64(ptr);
        return *this;
    }

    template <std::derived_from<Serializable> T>
    SerReader& operator<<(T &v)
    {
        v << *this;
        return *this;
    }
};


//
// Writer (Serializer)
//

#define SERIALIZE(type,function,cast) \
SerWriter& operator<<(type& v) \
{ \
function(ptr, (cast)v); \
return *this; \
}

#define SERIALIZE8(type)  static_assert(sizeof(type) == 1); SERIALIZE(type,write8,u8)
#define SERIALIZE16(type) static_assert(sizeof(type) == 2); SERIALIZE(type,write16,u16)
#define SERIALIZE64(type) static_assert(sizeof(type) <= 8); SERIALIZE(type,write64,u64)
#define SERIALIZED(type) static_assert(sizeof(type) <= 8); SERIALIZE(type,writeDouble,double)

class SerWriter
{
public:

    u8 *ptr;

    SerWriter(u8 *p) : ptr(p)
    {
    }

    SERIALIZE8(const bool)
    SERIALIZE8(const char)
    SERIALIZE8(const signed char)
    SERIALIZE8(const unsigned char)
    SERIALIZE16(const short)
    SERIALIZE16(const unsigned short)
    SERIALIZE64(const int)
    SERIALIZE64(const unsigned int)
    SERIALIZE64(const long)
    SERIALIZE64(const unsigned long)
    SERIALIZE64(const long long)
    SERIALIZE64(const unsigned long long)
    SERIALIZED(const float)
    SERIALIZED(const double)

    template <class T>
    auto& operator<<(util::Allocator<T> &a)
    {
        *this << i64(a.size);
        a.copy(ptr);
        ptr += a.size;
        return *this;
    }

    auto& operator<<(const string &v)
    {
        writeString(ptr, v);
        return *this;
    }

    template <class T>
    auto& operator<<(std::vector<T> &v)
    {
        auto len = v.size();
        *this << i64(len);
        for (usize i = 0; i < len; i++) {
            *this << v[i];
        }
        return *this;
    }

    template <class T, isize N>
    SerWriter& operator<<(T (&v)[N])
    {
        for(isize i = 0; i < N; ++i) {
            *this << v[i];
        }
        return *this;
    }

    template <class E, class = std::enable_if_t<std::is_enum<E>{}>>
    SerWriter& operator<<(E &v)
    {
        write64(ptr, (long)v);
        return *this;
    }

    template <std::derived_from<Serializable> T>
    SerWriter& operator<<(T &v)
    {
        v << *this;
        return *this;
    }

    void copy(const void *src, isize n)
    {
        std::memcpy((void *)ptr, src, n);
        ptr += n;
    }
};


//
// Resetter
//

#define RESET(type) \
SerResetter & operator<<(type& v) \
{ \
v = (type)0; \
return *this; \
}

class SerResetter
{
    bool hard;

public:

    SerResetter(bool hard) : hard(hard) { };

    bool isHard() { return hard; }
    bool isSoft() { return !isHard(); }

    RESET(bool)
    RESET(char)
    RESET(signed char)
    RESET(unsigned char)
    RESET(short)
    RESET(unsigned short)
    RESET(int)
    RESET(unsigned int)
    RESET(long)
    RESET(unsigned long)
    RESET(long long)
    RESET(unsigned long long)
    RESET(float)
    RESET(double)

    template <class T>
    auto& operator<<(util::Allocator<T> &a)
    {
        a.clear();
        return *this;
    }

    auto& operator<<(string &v)
    {
        v = "";
        return *this;
    }
    
    template <class T>
    auto& operator<<(std::vector<T> &v)
    {
        v.clear();
        return *this;
    }

    template <class T, isize N>
    SerResetter & operator<<(T (&v)[N])
    {
        for(isize i = 0; i < N; ++i) {
            *this << v[i];
        }
        return *this;
    }

    template <class E, class = std::enable_if_t<std::is_enum<E>{}>>
    SerResetter& operator<<(E &v)
    {
        v = (E)0;
        return *this;
    }

    template <std::derived_from<Serializable> T>
    SerResetter & operator<<(T &v)
    {
        v << *this;
        return *this;
    }
};

template <class T> inline bool isChecker(T &worker) { return false; }
template <> inline bool isChecker(SerChecker &worker) { return true; }
template <class T> inline bool isResetter(T &worker) { return false; }
template <> inline bool isResetter(SerResetter &worker) { return true; }
template <class T> inline bool isSoftResetter(T &worker) { return false; }
template <> inline bool isSoftResetter(SerResetter &worker) { return worker.isSoft(); }
template <class T> inline bool isHardResetter(T &worker) { return false; }
template <> inline bool isHardResetter(SerResetter &worker) { return worker.isHard(); }

}

#define SERIALIZERS(fn) \
void operator << (SerChecker &worker) override { fn(worker); } \
void operator << (SerCounter &worker) override { fn(worker); } \
void operator << (SerResetter &worker) override { fn(worker); } \
void operator << (SerReader &worker) override { fn(worker); } \
void operator << (SerWriter &worker) override { fn(worker); }

#define CARTRIDGE_SERIALIZERS(fn) \
void operator << (SerChecker &worker) override { Cartridge::operator<<(worker); fn(worker); } \
void operator << (SerCounter &worker) override { Cartridge::operator<<(worker); fn(worker); } \
void operator << (SerResetter &worker) override { Cartridge::operator<<(worker); fn(worker); } \
void operator << (SerReader &worker) override { Cartridge::operator<<(worker); fn(worker); } \
void operator << (SerWriter &worker) override { Cartridge::operator<<(worker); fn(worker); }

#define CLONE(x) x = other.x;
#define CLONE_ARRAY(x) std::copy(std::begin(other.x), std::end(other.x), std::begin(x));
