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
#include "Error.h"
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

using std::string;

// Returns true if this executable is a release build
bool releaseBuild();

//
// Handling low level data objects
//

// Returns a byte from a u16 value
#define HI_BYTE(x) (u8)((x) >> 8)
#define LO_BYTE(x) (u8)((x) & 0xFF)

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


//
// Converting low level data objects
//

// Returns the number of characters in a null terminated unichar array
size_t strlen16(const u16 *unichars);

/* Converts a PETSCII character to a printable character. Replaces all
 * unprintable characters by subst.
 */
u8 petscii2printable(u8 c, u8 subst);

/* Converts an ASCII character to a PETSCII character. This function translates
 * into the unshifted PET character set. I.e., lower case characters are
 * converted to uppercase characters. Returns ' ' for ASCII characters with no
 * PETSCII representation.
 */
u8 ascii2pet(u8 asciichar);

/* Converts an ASCII string into a PETSCII string. Applies function ascii2pet
 * to all characters of a string.
 */
void ascii2petStr(char *str);

// Writes an integer into a string in decimal format
void sprint8d(char *s, u8 value);
void sprint16d(char *s, u16 value);

// Writes an integer into a string in hexadecimal format
void sprint8x(char *s, u8 value);
void sprint16x(char *s, u16 value);

// Writes an integer into a string in binary format
void sprint8b(char *s, u8 value);
void sprint16b(char *s, u16 value);


//
// Pretty printing
//

// Prints a hex dump of a buffer to the console (for debugging)
void hexdump(u8 *p, size_t size, size_t cols, size_t pad);
void hexdump(u8 *p, size_t size, size_t cols = 32);
void hexdumpWords(u8 *p, size_t size, size_t cols = 32);
void hexdumpLongwords(u8 *p, size_t size, size_t cols = 32);


//
// Accessing memory
//

// Checks if a certain memory area is all zero
bool isZero(const u8 *ptr, size_t size); 


//
// Handling file
//

/* Extracts a certain component from a path. Every function returns a newly
 * created string which needs to be deleted manually.
 */
char *extractFilename(const char *path);
char *extractSuffix(const char *path);
char *extractFilenameWithoutSuffix(const char *path);

// Checks the suffix of a file name
bool checkFileSuffix(const char *filename, const char *suffix);

// Returns the size of a file in bytes
long getSizeOfFile(const char *filename);

// Checks if a path points to a directory
bool isDirectory(const char *path);

// Returns the number of files in a directory
long numDirectoryItems(const char *path);

// Checks the size of a file
bool checkFileSize(const char *filename, long min = -1, long max = -1);

// Checks the header signature (magic bytes) of a file or buffer
bool matchingFileHeader(const char *path, const u8 *header, size_t length);
bool matchingBufferHeader(const u8 *buffer, const u8 *header, size_t length);

// Loads a file from disk
bool loadFile(const char *path, u8 **buffer, long *size);
bool loadFile(const char *path, const char *name, u8 **buffer, long *size);


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
u32 fnv_1a_32(u8 *addr, size_t size);
u64 fnv_1a_64(u8 *addr, size_t size);

// Computes a CRC-32 checksum for a given buffer
u32 crc32(const u8 *addr, size_t size);
u32 crc32forByte(u32 r);
