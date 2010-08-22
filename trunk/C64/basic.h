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
#include <math.h>

// Macros

//! Evaluates to the high byte of x. x is expected to be of type uint16_t.
#define HI_BYTE(x) (uint8_t)((x) >> 8)

//! Evaluates to the low byte of x. x is expected to be of type uint16_t.
#define LO_BYTE(x) (uint8_t)((x) & 0xff)

//! Evaluates to the 16 bit value specified by x and y in little endian order (low, high).
#define LO_HI(x,y) (uint16_t)((y) << 8 | (x))

//! Evaluates to the value of x with bit "nr" set to 1. All other bits remain untouched.
#define SET_BIT(x,nr) ((x) |= (1 << (nr)))

//! Evaluates to the value of x with bit "nr" set to 0. All other bits remain untouched.
#define CLR_BIT(x,nr) ((x) &= ~(1 << (nr)))

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

//! Let the process sleep for msec microseconds
/*! \todo The functions is right now called inside the execution thread of the CPU. The value is slightly too high, because
    falling asleep and waking up also costs some time. As a result, we get a refresh rate slightly lower than the expected 60 Hz.
	We should either take care of this time gap here or in the CPU method. 
	\param millisec Time to sleep in milliseconds
*/
void sleepMicrosec(uint64_t millisec);


#endif

