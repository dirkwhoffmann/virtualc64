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

struct timeval t;
long tv_base = (gettimeofday(&t,NULL), t.tv_sec);

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

uint64_t 
msec()
{
	struct timeval t;
	gettimeofday(&t,NULL);	
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
