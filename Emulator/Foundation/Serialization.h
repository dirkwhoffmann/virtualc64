// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _SERIALIZATION_H
#define _SERIALIZATION_H

//
// Basic memory buffer I/O
//

inline u8 read8(u8 *& buffer)
{
    u8 result = *buffer;
    buffer += 1;
    return result;
}

inline u16 read16(u8 *& buffer)
{
    u16 result = ntohs(*((u16 *)buffer));
    buffer += 2;
    return result;
}

inline u32 read32(u8 *& buffer)
{
    u32 result = ntohl(*((u32 *)buffer));
    buffer += 4;
    return result;
}

inline u64 read64(u8 *& buffer)
{
    u32 hi = read32(buffer);
    u32 lo = read32(buffer);
    return ((u64)hi << 32) | lo;
}

inline double readDouble(u8 *& buffer)
{
    double result;
    *((u64 *)(&result)) = read64(buffer);
    return result;
}
 
inline void write8(u8 *& buffer, u8 value)
{
    *buffer = value;
    buffer += 1;
}

inline void write16(u8 *& buffer, u16 value)
{
    *((u16 *)buffer) = htons(value);
    buffer += 2;
}

inline void write32(u8 *& buffer, u32 value)
{
    *((u32 *)buffer) = htonl(value);
    buffer += 4;
}

inline void write64(u8 *& buffer, u64 value)
{
    write32(buffer, (u32)(value >> 32));
    write32(buffer, (u32)(value));
}

inline void writeDouble(u8 *& buffer, double value)
{
    write64(buffer, *((u64 *)(&value)));
}

//
// Counter (determines the state size)
//

#define COUNT(type) \
auto& operator&(type& v) \
{ \
count += sizeof(type); \
return *this; \
}

#define STRUCT(type) \
auto& operator&(type& v) \
{ \
v.applyToItems(*this); \
return *this; \
}

#define __ ,

class SerCounter
{
public:

    size_t count;

    SerCounter() { count = 0; }

    COUNT(const bool)
    COUNT(const char)
    COUNT(const signed char)
    COUNT(const unsigned char)
    COUNT(const short)
    COUNT(const unsigned short)
    COUNT(const int)
    COUNT(const unsigned int)
    COUNT(const long)
    COUNT(const unsigned long)
    COUNT(const long long)
    COUNT(const unsigned long long)
    COUNT(const double)

    // COUNT(const AgnusRevision)
    // COUNT(const DeniseRevision)

    // STRUCT(Beam)
    // STRUCT(DDF<true>)

    template <class T, size_t N>
    SerCounter& operator&(T (&v)[N])
    {
        for(size_t i = 0; i < N; ++i) {
            *this & v[i];
        }
        return *this;
    }
};


//
// Reader (Deserializer)
//

#define DESERIALIZE(type,function) \
SerReader& operator&(type& v) \
{ \
v = (type)function(ptr); \
return *this; \
}

#define DESERIALIZE8(type)  static_assert(sizeof(type) == 1); DESERIALIZE(type,read8)
#define DESERIALIZE16(type) static_assert(sizeof(type) == 2); DESERIALIZE(type,read16)
#define DESERIALIZE32(type) static_assert(sizeof(type) == 4); DESERIALIZE(type,read32)
#define DESERIALIZE64(type) static_assert(sizeof(type) == 8); DESERIALIZE(type,read64)
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
    DESERIALIZE64(long)
    DESERIALIZE64(unsigned long)
    DESERIALIZE64(long long)
    DESERIALIZE64(unsigned long long)
    DESERIALIZED(double)
 
    // DESERIALIZE64(AgnusRevision)
    // DESERIALIZE64(DeniseRevision)

    // STRUCT(Beam)
    // STRUCT(DDF<true>)

    template <class T, size_t N>
    SerReader& operator&(T (&v)[N])
    {
        for(size_t i = 0; i < N; ++i) {
            *this & v[i];
        }
        return *this;
    }

    void copy(void *dst, size_t n)
    {
        memcpy(dst, (void *)ptr, n);
        ptr += n;
    }
};


//
// Writer (Serializer)
//

#define SERIALIZE(type,function,cast) \
SerWriter& operator&(type& v) \
{ \
function(ptr, (cast)v); \
return *this; \
}

#define SERIALIZE8(type)  static_assert(sizeof(type) == 1); SERIALIZE(type,write8,u8)
#define SERIALIZE16(type) static_assert(sizeof(type) == 2); SERIALIZE(type,write16,u16)
#define SERIALIZE32(type) static_assert(sizeof(type) == 4); SERIALIZE(type,write32,u32)
#define SERIALIZE64(type) static_assert(sizeof(type) == 8); SERIALIZE(type,write64,u64)
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
    SERIALIZE64(const long)
    SERIALIZE64(const unsigned long)
    SERIALIZE64(const long long)
    SERIALIZE64(const unsigned long long)
    SERIALIZED(const double)
 
    // SERIALIZE64(const AgnusRevision)
    // SERIALIZE64(const DeniseRevision)

    // STRUCT(Beam)
    // STRUCT(DDF<true>)

    template <class T, size_t N>
    SerWriter& operator&(T (&v)[N])
    {
        for(size_t i = 0; i < N; ++i) {
            *this & v[i];
        }
        return *this;
    }

    void copy(const void *src, size_t n)
    {
        memcpy((void *)ptr, src, n);
        ptr += n;
    }

};


//
// Resetter
//

#define RESET(type) \
SerResetter& operator&(type& v) \
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
    RESET(double)

    // RESET(DriveState)
    // RESET(RTCModel)

    // STRUCT(Beam)
    // STRUCT(DDF<true>)

    template <class T, size_t N>
    SerResetter& operator&(T (&v)[N])
    {
        for(size_t i = 0; i < N; ++i) {
            *this & v[i];
        }
        return *this;
    }
};

#endif
