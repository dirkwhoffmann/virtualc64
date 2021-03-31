// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "Macros.h"
#include "C64Types.h"
#include "Buffers.h"
#include "CPUInstructions.h"
#include "TimeDelayed.h"
#include "Volume.h"
#include "envelope.h"

#include <arpa/inet.h>

//
// Basic memory buffer I/O
//

inline u8 read8(u8 *& buf)
{
    u8 result = R8BE(buf);
    buf += 1;
    return result;
}

inline u16 read16(u8 *& buf)
{
    u16 result = R16BE(buf);
    buf += 2;
    return result;
}

inline u32 read32(u8 *& buf)
{
    u32 result = R32BE(buf);
    buf += 4;
    return result;
}

inline u64 read64(u8 *& buf)
{
    u32 hi = read32(buf);
    u32 lo = read32(buf);
    return ((u64)hi << 32) | lo;
}

inline float readFloat(u8 *& buf)
{
    float result;
    *((u32 *)(&result)) = read32(buf);
    return result;
}

inline double readDouble(u8 *& buf)
{
    double result;
    *((u64 *)(&result)) = read64(buf);
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

inline void writeFloat(u8 *& buf, float value)
{
    write32(buf, *((u32 *)(&value)));
}

inline void writeDouble(u8 *& buf, double value)
{
    write64(buf, *((u64 *)(&value)));
}


//
// Counter (determines the state size)
//

#define COUNT(type) \
auto& operator<<(type& v) \
{ \
count += sizeof(type); \
return *this; \
}

#define STRUCT(type) \
auto& operator<<(type& v) \
{ \
v.applyToItems(*this); \
return *this; \
}

#define __ ,

class SerCounter
{
public:

    usize count;

    SerCounter() { count = 0; }

    COUNT(const bool)
    COUNT(const char)
    COUNT(const signed char)
    COUNT(const unsigned char)
    COUNT(const short)
    COUNT(const unsigned short)
    COUNT(const int)
    COUNT(const unsigned int)
    COUNT(const long long)
    COUNT(const unsigned long long)
    COUNT(const float)
    COUNT(const double)

    STRUCT(VICIIRegisters)
    STRUCT(SpriteSR)
    STRUCT(DiskData)
    STRUCT(DiskLength)
    STRUCT(Volume)
    template <class T, int capacity> STRUCT(TimeDelayed<T __ capacity>)

    template <class T, usize N>
    SerCounter& operator<<(T (&v)[N])
    {
        for(usize i = 0; i < N; ++i) {
            *this << v[i];
        }
        return *this;
    }
    
    template <class T>
    SerCounter& operator>>(T &v)
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
#define DESERIALIZE32(type) static_assert(sizeof(type) == 4); DESERIALIZE(type,read32)
#define DESERIALIZE64(type) static_assert(sizeof(type) == 8); DESERIALIZE(type,read64)
#define DESERIALIZEF(type) static_assert(sizeof(type) == 4); DESERIALIZE(type,readFloat)
#define DESERIALIZED(type) static_assert(sizeof(type) == 8); DESERIALIZE(type,readDouble)

class SerReader
{
public:

    u8 *ptr;

    SerReader(u8 *p) : ptr(p)
    {
    }

    DESERIALIZE8(bool)
    DESERIALIZE8(char)
    DESERIALIZE8(signed char)
    DESERIALIZE8(unsigned char)
    DESERIALIZE16(short)
    DESERIALIZE16(unsigned short)
    DESERIALIZE32(int)
    DESERIALIZE32(unsigned int)
    DESERIALIZE64(long long)
    DESERIALIZE64(unsigned long long)
    DESERIALIZEF(float)
    DESERIALIZED(double)
 
    STRUCT(VICIIRegisters)
    STRUCT(SpriteSR)
    STRUCT(DiskData)
    STRUCT(DiskLength)
    STRUCT(Volume)
    template <class T, int capacity> STRUCT(TimeDelayed<T __ capacity>)

    template <class T, usize N>
    SerReader& operator<<(T (&v)[N])
    {
        for(usize i = 0; i < N; ++i) {
            *this << v[i];
        }
        return *this;
    }

    template <class T>
    SerReader& operator>>(T &v)
    {
        v << *this;
        return *this;
    }
    
    template <class T, isize N>
    SerReader& operator>>(T (&v)[N])
    {
        for(isize i = 0; i < N; ++i) {
            v[i] << *this;
        }
        return *this;
    }

    void copy(void *dst, usize n)
    {
        memcpy(dst, (void *)ptr, n);
        ptr += n;
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
#define SERIALIZE32(type) static_assert(sizeof(type) == 4); SERIALIZE(type,write32,u32)
#define SERIALIZE64(type) static_assert(sizeof(type) == 8); SERIALIZE(type,write64,u64)
#define SERIALIZEF(type) static_assert(sizeof(type) == 4); SERIALIZE(type,writeFloat,float)
#define SERIALIZED(type) static_assert(sizeof(type) == 8); SERIALIZE(type,writeDouble,double)

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
    SERIALIZE32(const int)
    SERIALIZE32(const unsigned int)
    SERIALIZE64(const long long)
    SERIALIZE64(const unsigned long long)
    SERIALIZEF(const float)
    SERIALIZED(const double)
 
    STRUCT(VICIIRegisters)
    STRUCT(SpriteSR)
    STRUCT(DiskData)
    STRUCT(DiskLength)
    STRUCT(Volume)
    template <class T, int capacity> STRUCT(TimeDelayed<T __ capacity>)

    template <class T, usize N>
    SerWriter& operator<<(T (&v)[N])
    {
        for(usize i = 0; i < N; ++i) {
            *this << v[i];
        }
        return *this;
    }

    template <class T>
    SerWriter& operator>>(T &v)
    {
        v << *this;
        return *this;
    }
    
    template <class T, isize N>
    SerWriter& operator>>(T (&v)[N])
    {
        for(isize i = 0; i < N; ++i) {
            v[i] << *this;
        }
        return *this;
    }

    void copy(const void *src, usize n)
    {
        memcpy((void *)ptr, src, n);
        ptr += n;
    }
};


//
// Resetter
//

#define RESET(type) \
SerResetter& operator<<(type& v) \
{ \
v = (type)0; \
return *this; \
}

class SerResetter
{
public:

    SerResetter()
    {
    }

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
    
    STRUCT(VICIIRegisters)
    STRUCT(SpriteSR)
    STRUCT(DiskData)
    STRUCT(DiskLength)
    STRUCT(Volume)
    template <class T, int capacity> STRUCT(TimeDelayed<T __ capacity>)

    template <class T, usize N>
    SerResetter& operator<<(T (&v)[N])
    {
        for(usize i = 0; i < N; ++i) {
            *this << v[i];
        }
        return *this;
    }
    
    template <class T>
    SerResetter& operator>>(T &v)
    {
        v << *this;
        return *this;
    }
    
    template <class T, isize N>
    SerResetter& operator>>(T (&v)[N])
    {
        for(isize i = 0; i < N; ++i) {
            v[i] << *this;
        }
        return *this;
    }
};
