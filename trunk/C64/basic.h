/*
 * (C) 2006 Dirk W. Hoffmann. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

// Last review: 24.7.06

#ifndef _BASIC_INC
#define _BASIC_INC

//! Indicate that we run in debug mode
#define DEBUG



// General Includes
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <limits.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <sched.h>
#include <assert.h>

// #include "VirtualComponent.h"

// Macros

//! Evaluates to the high byte of x. x is expected to be of type uint16_t.
#define HI_BYTE(x) (uint8_t)((x) >> 8)

//! Evaluates to the low byte of x. x is expected to be of type uint16_t.
#define LO_BYTE(x) (uint8_t)((x) & 255)

//! Evaluates to the 16 bit value specified by x and y in little endian order (low, high).
#define LO_HI(x,y) (uint16_t)((y) << 8 | (x))

//! Evaluates to the value of x with bit "nr" set to 1. All other bits remain untouched.
#define SET_BIT(x,nr) ((x) |= (1 << (nr)))

//! Evaluates to the value of x with bit "nr" set to 0. All other bits remain untouched.
#define CLEAR_BIT(x,nr) ((x) &= 255 - (1 << (nr)))

//! Clip the value of x if it is outside the specified limits
template <class T>
inline T clip(T x, T min, T max) { return (x < min) ? min : (x > max ? max : x); }

// Conversion functions

//! Convert a BCD number to a binary value
inline uint8_t BCDToBinary(uint8_t value) { return 10 * (value & 0xF0) + (value & 0x0F); }

//! Convert a binary value to a BCD number
inline uint8_t BinaryToBCD(uint8_t value) { return (value / 10) << 4 + (value % 10); }

//! Check file suffix
/*! The function is used for determining the type of a file.
	\param filename Path and name of the file to investigate
	\param suffix Expected suffix
*/
bool checkFileSuffix(const char *filename, const char *suffix);

//! Check file size
/*! The function is used for validating the size of a file.
	\param filename Path and name of the file to investigate
	\param min Expected minimum size (-1 if no lower bound exists)
	\param max Expected maximum size (-1 if no upper bound exists)	
*/
bool checkFileSize(const char *filename, int min, int max);

//! Check magic bytes of a file.
/*! The function is used for determining the type of a file.
	\param filename Path and name of the file to investigate
	\param header Expected byte sequence, terminated by EOF
	\return Returns true iff magic bytes match
	\see Memory::isBasicRom Memory::isKernelRom Memory::isCharRom
*/
bool 
checkFileHeader(const char *filename, int *header);

//! Returns the bit pattern of a 8-Bit value as string
/*! The functions is used by several debugging functions to produce human readable output.
	Don't free the strings as they are cached in a static array. 
*/
const char *getBitPattern(uint8_t value);

//! Write up to four bytes to file
/*! \param file handle of the destination file
	\param value 64 Bit value containing the data to written. The lower bytes are written first.
	\param count Number of bytes to be written (range: 1 to 4)
*/
void write(FILE *file, uint64_t value, int count);

//! Read up to four bytes from a file
/*! \param file handle of the source file
	\param count Number of bytes to be read (range: 1 to 4)
	\return Data bytes, stored in a single 64 bit value. The lower bytes are written first. 
*/
uint64_t read(FILE *file, int count);

//! Write 8 bit value to a file
static void write8(FILE *file, uint8_t value) { write(file, (uint64_t)value, sizeof(uint8_t)); }
//! Write 16 bit value to a file
static void write16(FILE *file, uint16_t value) { write(file, (uint64_t)value, sizeof(uint16_t)); }
//! Write 32 bit value to a file
static void write32(FILE *file, uint32_t value) { write(file, (uint64_t)value, sizeof(uint32_t)); }
//! Write 64 bit value to a file
static void write64(FILE *file, uint64_t value) { write(file, (uint64_t)value, sizeof(uint64_t)); }

//! Read 8 bit value from a file
static uint8_t read8(FILE *file) { return (uint8_t)read(file, sizeof(uint8_t)); }
//! Read 16 bit value from a file
static uint16_t read16(FILE *file) { return (uint16_t)read(file, sizeof(uint16_t)); }
//! Read 32 bit value from a file
static uint32_t read32(FILE *file) { return (uint32_t)read(file, sizeof(uint32_t)); }
//! Read 64 bit value from a file
static uint64_t read64(FILE *file) { return (uint64_t)read(file, sizeof(uint64_t)); }


// Timing

//! Application launch time in seconds
/*! The value is read by function \a msec for computing the elapsed number of microseconds. */
extern long tv_base;

//! Return the number of elapsed milliseconds since program launch
uint64_t msec();
//! Read real-time clock (1/10th seconds)
uint8_t localTimeSecFrac();
//! Read real-time clock (seconds)
uint8_t localTimeSec();
//! Read real-time clock (minutes)
uint8_t localTimeMin();
//! Read real-time clock (hours)
uint8_t localTimeHour();

//! Convert an ASCII character to a PET character
uint8_t ascii2pet(char c);

//! Let the process sleep for msec microseconds
/*! \todo The functions is right now called inside the execution thread of the CPU. The value is slightly too high, because
    falling asleep and waking up also costs some time. As a result, we get a refresh rate slightly lower than the expected 60 Hz.
	We should either take care of this time gap here or in the CPU method. 
	\param millisec Time to sleep in milliseconds
*/
void sleepMicrosec(uint64_t millisec);

// Debugging

//! Write debugging message
/*! Syntax is similar to the printf function. The function only takes effect in debug mode.  */
//void debug(char *fmt, ...);
//! Write debugging message (warning style)
/*! Syntax is similar to the standard printf function. The function only takes effect in debug mode. */
void warn(char *fmt, ...);
//! Terminate application with an error message */
/*! Syntax is similar to the printf function. The function only takes effect in debug mode. */
void fail(char *fmt, ...);

#endif

