// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "Debug.h"
#include "VC64Error.h"
#include "C64Config.h"
#include "C64Constants.h"
#include "C64Types.h"

#include <limits.h>
#include <math.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string>
#include <set>
#include <sstream>
#include <fstream>

using std::string;

// Returns true if this executable is a release build
bool releaseBuild();

//
// Handling low level data objects
//

// Returns a byte from a u16 value
#define LO_BYTE(x) (u8)((x) & 0xFF)
#define HI_BYTE(x) (u8)((x) >> 8)

// Returns the low word or the high word of a 32 bit value
#define LO_WORD(x) (u16)((x) & 0xFFFF)
#define HI_WORD(x) (u16)((x) >> 16)

// Builds a larger integer in little endian byte format
#define LO_HI(x,y) (u16)((y) << 8 | (x))
#define LO_LO_HI(x,y,z) (u32)((z) << 16 | (y) << 8 | (x))
#define LO_LO_HI_HI(x,y,z,w) (u32)((w) << 24 | (z) << 16 | (y) << 8 | (x))

// Builds a larger integer in big endian byte format
#define HI_LO(x,y) (u16)((x) << 8 | (y))
#define HI_HI_LO(x,y,z) (u32)((x) << 16 | (y) << 8 | (z))
#define HI_HI_LO_LO(x,y,z,w) (u32)((x) << 24 | (y) << 16 | (z) << 8 | (w))

// Returns a certain byte of a larger integer
#define BYTE0(x) LO_BYTE(x)
#define BYTE1(x) LO_BYTE((x) >> 8)
#define BYTE2(x) LO_BYTE((x) >> 16)
#define BYTE3(x) LO_BYTE((x) >> 24)

// Returns a non-zero value if the n-th bit is set in x
#define GET_BIT(x,nr) ((x) & (1 << (nr)))

// Changes a single bit
#define SET_BIT(x,nr) ((x) |= (1 << (nr)))
#define CLR_BIT(x,nr) ((x) &= ~(1 << (nr)))
#define TOGGLE_BIT(x,nr) ((x) ^= (1 << (nr)))

// Sets a single bit to 0 (value == 0) or 1 (value != 0)
#define WRITE_BIT(x,nr,value) ((value) ? SET_BIT(x, nr) : CLR_BIT(x, nr))

// Replaces bits, bytes, and words
#define REPLACE_BIT(x,nr,v) ((v) ? SET_BIT(x, nr) : CLR_BIT(x, nr))
#define REPLACE_LO(x,y) (((x) & ~0x00FF) | (y))
#define REPLACE_HI(x,y) (((x) & ~0xFF00) | ((y) << 8))
#define REPLACE_LO_WORD(x,y) (((x) & ~0xFFFF) | (y))
#define REPLACE_HI_WORD(x,y) (((x) & ~0xFFFF0000) | ((y) << 16))

// Checks for a rising or a falling edge
#define RISING_EDGE(x,y) (!(x) && (y))
#define RISING_EDGE_BIT(x,y,n) (!((x) & (1 << (n))) && ((y) & (1 << (n))))
#define FALLING_EDGE(x,y) ((x) && !(y))
#define FALLING_EDGE_BIT(x,y,n) (((x) & (1 << (n))) && !((y) & (1 << (n))))

// Checks is a number is even or odd
#define IS_EVEN(x) (!IS_ODD(x))
#define IS_ODD(x) ((x) & 1)


//
// Accessing memory
//

// Reads a value in big-endian format
#define R8BE(a)  (*(u8 *)(a))
#define R16BE(a) HI_LO(*(u8 *)(a), *(u8 *)((a)+1))
#define R32BE(a) HI_HI_LO_LO(*(u8 *)(a), *(u8 *)((a)+1), *(u8 *)((a)+2), *(u8 *)((a)+3))

#define R8BE_ALIGNED(a)  (*(u8 *)(a))
#define R16BE_ALIGNED(a) (htons(*(u16 *)(a)))
#define R32BE_ALIGNED(a) (htonl(*(u32 *)(a)))

// Writes a value in big-endian format
#define W8BE(a,v)  { *(u8 *)(a) = (v); }
#define W16BE(a,v) { *(u8 *)(a) = HI_BYTE(v); *(u8 *)((a)+1) = LO_BYTE(v); }
#define W32BE(a,v) { W16BE(a,HI_WORD(v)); W16BE((a)+2,LO_WORD(v)); }

#define W8BE_ALIGNED(a,v)  { *(u8 *)(a) = (u8)(v); }
#define W16BE_ALIGNED(a,v) { *(u16 *)(a) = ntohs((u16)v); }
#define W32BE_ALIGNED(a,v) { *(u32 *)(a) = ntohl((u32)v); }


//
// Pretty printing
//

// Writes an integer into a string in decimal format
void sprint8d(char *s, u8 value);
void sprint16d(char *s, u16 value);

// Writes an integer into a string in hexadecimal format
void sprint8x(char *s, u8 value);
void sprint16x(char *s, u16 value);

// Writes an integer into a string in binary format
void sprint8b(char *s, u8 value);
void sprint16b(char *s, u16 value);

// Prints a hex dump of a buffer to the console (for debugging)
void hexdump(u8 *p, usize size, usize cols, usize pad);
void hexdump(u8 *p, usize size, usize cols = 32);
void hexdumpWords(u8 *p, usize size, usize cols = 32);
void hexdumpLongwords(u8 *p, usize size, usize cols = 32);


//
// Accessing memory
//

// Checks if a certain memory area is all zero
bool isZero(const u8 *ptr, usize size); 


//
// Handling files
//

/* Extracts a certain component from a path. Every function returns a newly
 * created string which needs to be deleted manually.
 */
string extractFileName(const string &s);
string extractSuffix(const string &s);
string stripSuffix(const string &s); 

// Extracts the suffix from a filename
std::string suffix(const std::string &name);

// Returns the size of a file in bytes
long getSizeOfFile(const char *filename);

// Checks if a path points to a directory
bool isDirectory(const std::string &path);
bool isDirectory(const char *path);

// Returns the number of files in a directory
usize numDirectoryItems(const std::string &path);
usize numDirectoryItems(const char *path);

// Checks the header signature (magic bytes) of a file or buffer
bool matchingStreamHeader(std::istream &stream, const u8 *header, usize length);

// Loads a file from disk
bool loadFile(const std::string &path, u8 **bufptr, long *lenptr);
bool loadFile(const std::string &path, const std::string &name, u8 **bufptr, long *lenptr);


//
// Handling streams
//

usize streamLength(std::istream &stream);


//
// Computing checksums
//

// Returns the FNV-1a seed value
inline u32 fnv_1a_init32() { return 0x811c9dc5; }
inline u64 fnv_1a_init64() { return 0xcbf29ce484222325; }

// Performs a single iteration of the FNV-1a hash algorithm
inline u32 fnv_1a_it32(u32 prev, u32 value) { return (prev ^ value) * 0x1000193; }
inline u64 fnv_1a_it64(u64 prev, u64 value) { return (prev ^ value) * 0x100000001b3; }

// Computes a FNV-1a checksum for a given buffer
u32 fnv_1a_32(u8 *addr, usize size);
u64 fnv_1a_64(u8 *addr, usize size);

// Computes a CRC-32 checksum for a given buffer
u32 crc32(const u8 *addr, usize size);
u32 crc32forByte(u32 r);
