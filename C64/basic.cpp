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

#include "basic.h"

struct 
timeval t;

static char
bitPattern[256][9];

long 
tv_base = (gettimeofday(&t,NULL), t.tv_sec);

bool
checkFileSuffix(const char *filename, const char *suffix)
{
	assert(filename != NULL);
	assert(suffix != NULL);
	
	if (strlen(suffix) > strlen(filename))
		return false;
	
	filename += (strlen(filename) - strlen(suffix));
	if (strcmp(filename, suffix) == 0)
		return true;
	else
		return false;
}

bool
checkFileSize(const char *filename, int min, int max)
{
	struct stat fileProperties;
	
	if (filename == NULL) {
		return false;
	}
	
    if (stat(filename, &fileProperties) != 0)
		return false;

	if (min > 0 && fileProperties.st_size < min)
		return false;

	if (max > 0 && fileProperties.st_size > max)
		return false;

	return true;
}

bool 
checkFileHeader(const char *filename, int *header)
{
	int i, c;
	bool result = true;
	FILE *file;

	assert(filename != NULL);
	assert(header != NULL);
	
	if ((file = fopen(filename, "r")) == NULL)
		return false; 

	for (i = 0; header[i] != EOF; i++) {
		c = fgetc(file);
		if (c != header[i]) {
			result = false;
			break;
		}
	}
	
	fclose(file);
	return result;
}

#if 0
// DEPRECATED: Use checkFileHeader(const char *filename, int *header) instead
bool 
checkFileHeader(const char *filename, int size, uint8_t magic1, uint8_t magic2, uint8_t magic3)
{
	struct stat fileProperties;
	uint byte1, byte2, byte3;
	
	if (filename == NULL) {
		return false;
	}
	
	// Check file size and existence
    if (stat(filename, &fileProperties) != 0) {
		// Could not open file...
		return false;
	} else {
		if (size > 0 && fileProperties.st_size != size) {
			// File size does not match
			return false;
		}
	}
	
	// Check magic numbers
	bool result = true;
	FILE *file  = fopen(filename, "r");
	if (file == NULL)
		return false; 
	
	byte1 = fgetc(file);
	byte2 = fgetc(file);
	byte3 = fgetc(file);
	
	if (magic1 != byte1 || magic2 != byte2 || magic3 != byte3) {
		result = false;
	} else {
		result = true;
	}
	
	fclose(file);
	return result;
}
#endif

const char *
getBitPattern(uint8_t value) 
{
	static bool uninitialized = true;
	
	if (uninitialized) {
		uninitialized = false;
		for (int i = 0; i < 256; i++) {
			for (int j = 0; j < 8; j++) {
				bitPattern[i][j] = (i & (128 >> j)) ? '1' : '0';
			}
		bitPattern[i][8] = 0x00;
		}
	}
	
	return bitPattern[value];
}


void 
write(FILE *file, uint64_t value, int count)
{
	assert(file != NULL);
	assert(count > 0 && count <= 8);

	for (int i=0; i < count; i++) {
		fputc((int)(value & 0xff), file); 
		value >>= 8;
	}
}

uint64_t 
read(FILE *file, int count)
{
	uint64_t result = 0;
	
	assert(file != NULL);
	assert(count > 0 && count <= 8);
	
	for (int i=0; i < count; i++) {
		result += (uint64_t)fgetc(file) << (i*8);		
	}
	
	return result;
}

uint64_t 
msec()
{
	struct timeval t;
	
	gettimeofday(&t,NULL);
//	printf("(%d) %d seconds, %d microseconds since 1970\n", tv_base, t.tv_sec, t.tv_usec);
	
	return (uint64_t)1000000*(uint64_t)(t.tv_sec - tv_base) + (uint64_t)t.tv_usec;
}

uint8_t 
localTimeSecFrac()
{
	uint64_t millisec = msec();
	millisec = millisec / 100; // 1/10th seconds
	millisec = millisec % 10;
	return millisec;
}

uint8_t 
localTimeSec()
{
	time_t t = time(NULL);
	struct tm *loctime = localtime(&t);
	return (uint8_t)loctime->tm_sec;
}

uint8_t 
localTimeMinute()
{
	time_t t = time(NULL);
	struct tm *loctime = localtime(&t);
	return (uint8_t)loctime->tm_min;
}

uint8_t 
localTimeHour()
{
	time_t t = time(NULL);
	struct tm *loctime = localtime(&t);
	return (uint8_t)loctime->tm_hour;
}

	
void 
sleepMicrosec(uint64_t microsec)
{		
	if (microsec > 0 && microsec < 1000000) {
		usleep((long)microsec);
	}
}

#if 0
uint8_t 
ascii2pet(char c)
{
	if ((c >= 'A') && (c <= 'Z') || (c >= 'a') && (c <= 'z'))
		c = c ^ 0x20;
	else if ((c >= 0xc1) && (c <= 0xda))
		c = c ^ 0x80;
	return c;
}

#endif

void 
debug(char *fmt, ...) {
#ifdef DEBUG
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap); fflush(stderr);
	va_end(ap);
#endif
}

void
warn(char *fmt, ...) {
#ifdef DEBUG
	va_list ap;
	va_start(ap, fmt);
	printf("WARNING: ");
	vfprintf(stderr, fmt, ap); fflush(stderr);
	va_end(ap);
#endif
}

void
fail(char *fmt, ...) {
#ifdef DEBUG
	va_list ap;
	va_start(ap, fmt);
	printf("FATAL ERROR: ");
	vfprintf(stderr, fmt, ap); fflush(stderr);
	va_end(ap);
	exit(1);
#endif
}


