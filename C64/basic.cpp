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

void
printReadable(const void *data, int length)
{
    int i;
    for(i = 0; i < length; i++) {
        char ch = ((char*)(data))[i];
        if (isascii(ch)) {
            fprintf(stderr, "%02x %c ", ch, ch);
        } else {
            fprintf(stderr, "%02x ? ", ch);
        }
        if (i > 0 && i % 16 == 0) {
            fprintf(stderr, "\n");
        }
    }
}

uint16_t
pet2unicode(uint8_t petchar)
{
    switch (petchar) {
        case 0x05: return 0xF100; case 0x08: return 0xF118; case 0x09: return 0xF119; case 0x0D: return 0x000D;
        case 0x0E: return 0x000E; case 0x11: return 0xF11C; case 0x12: return 0xF11A; case 0x13: return 0xF120;
        case 0x14: return 0x007F; case 0x1C: return 0xF101; case 0x1D: return 0xF11D; case 0x1E: return 0xF102;
        case 0x1F: return 0xF103;
    }

    if (petchar >= 0x20 && petchar <= 0x5B)
        return (uint16_t)petchar;

    switch (petchar) {
        case 0x5C: return 0x00A3; case 0x5D: return 0x005D; case 0x5E: return 0x2191; case 0x5F: return 0x2190;
        case 0x60: return 0x2501; case 0x61: return 0x2660; case 0x62: return 0x2502; case 0x63: return 0x2501;
        case 0x64: return 0xF122; case 0x65: return 0xF123; case 0x66: return 0xF124; case 0x67: return 0xF126;
        case 0x68: return 0xF128; case 0x69: return 0x256E; case 0x6A: return 0x2570; case 0x6B: return 0x256F;
        case 0x6C: return 0xF12A; case 0x6D: return 0x2572; case 0x6E: return 0x2571; case 0x6F: return 0xF12B;
        case 0x70: return 0xF12C; case 0x71: return 0x25CF; case 0x72: return 0xF125; case 0x73: return 0x2665;
        case 0x74: return 0xF127; case 0x75: return 0x256D; case 0x76: return 0x2573; case 0x77: return 0x25CB;
        case 0x78: return 0x2663; case 0x79: return 0xF129; case 0x7A: return 0x2666; case 0x7B: return 0x253C;
        case 0x7C: return 0xF12E; case 0x7D: return 0x2502; case 0x7E: return 0x03C0; case 0x7F: return 0x25E5;
        case 0x81: return 0xF104; case 0x85: return 0xF110; case 0x86: return 0xF112; case 0x87: return 0xF114;
        case 0x88: return 0xF116; case 0x89: return 0xF111; case 0x8A: return 0xF113; case 0x8B: return 0xF115;
        case 0x8C: return 0xF117; case 0x8D: return 0x000A; case 0x8E: return 0x000F; case 0x90: return 0xF105;
        case 0x91: return 0xF11E; case 0x92: return 0xF11B; case 0x93: return 0x000C; case 0x94: return 0xF121;
        case 0x95: return 0xF106; case 0x96: return 0xF107; case 0x97: return 0xF108; case 0x98: return 0xF109;
        case 0x99: return 0xF10A; case 0x9A: return 0xF10B; case 0x9B: return 0xF10C; case 0x9C: return 0xF10D;
        case 0x9D: return 0xF11D; case 0x9E: return 0xF10E; case 0x9F: return 0xF10F; case 0xA0: return 0x00A0;
        case 0xA1: return 0x258C; case 0xA2: return 0x2584; case 0xA3: return 0x2594; case 0xA4: return 0x2581;
        case 0xA5: return 0x258F; case 0xA6: return 0x2592; case 0xA7: return 0x2595; case 0xA8: return 0xF12F;
        case 0xA9: return 0x25E4; case 0xAA: return 0xF130; case 0xAB: return 0x251C; case 0xAC: return 0xF134;
        case 0xAD: return 0x2514; case 0xAE: return 0x2510; case 0xAF: return 0x2582; case 0xB0: return	0x250C;
        case 0xB1: return 0x2534; case 0xB2: return 0x252C; case 0xB3: return 0x2524; case 0xB4: return 0x258E;
        case 0xB5: return 0x258D; case 0xB6: return 0xF131; case 0xB7: return 0xF132; case 0xB8: return 0xF133;
        case 0xB9: return 0x2583; case 0xBA: return 0xF12D; case 0xBB: return 0xF135; case 0xBC: return 0xF136;
        case 0xBD: return 0x2518; case 0xBE: return 0xF137; case 0xBF: return 0xF138; case 0xC0: return 0x2501;
        case 0xC1: return 0x2660; case 0xC2: return 0x2502; case 0xC3: return 0x2501; case 0xC4: return 0xF122;
        case 0xC5: return 0xF123; case 0xC6: return 0xF124; case 0xC7: return 0xF126; case 0xC8: return 0xF128;
        case 0xC9: return 0x256E; case 0xCA: return 0x2570; case 0xCB: return 0x256F; case 0xCC: return 0xF12A;
        case 0xCD: return 0x2572; case 0xCE: return 0x2571; case 0xCF: return 0xF12B; case 0xD0: return 0xF12C;
        case 0xD1: return 0x25CF; case 0xD2: return 0xF125; case 0xD3: return 0x2665; case 0xD4: return 0xF127;
        case 0xD5: return 0x256D; case 0xD6: return 0x2573; case 0xD7: return 0x25CB; case 0xD8: return 0x2663;
        case 0xD9: return 0xF129; case 0xDA: return 0x2666; case 0xDB: return 0x253C; case 0xDC: return 0xF12E;
        case 0xDD: return 0x2502; case 0xDE: return 0x03C0; case 0xDF: return 0x25E5; case 0xE0: return 0x00A0;
        case 0xE1: return 0x258C; case 0xE2: return 0x2584; case 0xE3: return 0x2594; case 0xE4: return 0x2581;
        case 0xE5: return 0x258F; case 0xE6: return 0x2592; case 0xE7: return 0x2595; case 0xE8: return 0xF12F;
        case 0xE9: return 0x25E4; case 0xEA: return 0xF130; case 0xEB: return 0x251C; case 0xEC: return 0xF134;
        case 0xED: return 0x2514; case 0xEE: return 0x2510; case 0xEF: return 0x2582; case 0xF0: return 0x250C;
        case 0xF1: return 0x2534; case 0xF2: return 0x252C; case 0xF3: return 0x2524; case 0xF4: return 0x258E;
        case 0xF5: return 0x258D; case 0xF6: return 0xF131; case 0xF7: return 0xF132; case 0xF8: return 0xF133;
        case 0xF9: return 0x2583; case 0xFA: return 0xF12D; case 0xFB: return 0xF135; case 0xFC: return 0xF136;
        case 0xFD: return 0x2518; case 0xFE: return 0xF137; case 0xFF: return 0x03C0;
    }
    
    return 0x0000;
}

char
toASCII(char c)
{
    uint8_t u = (uint8_t)c;
    
    u &= 0x7F;
    
    if (u >= 0x20 && u <= 0x7A) {
        return (char)u;
    } else {
        return '.';
    }
}

uint8_t
pet2ascii(uint8_t petchar)
{
    if (petchar == 0x00)
        return 0x00;

    uint16_t unicodechar = pet2unicode(petchar);
    return (unicodechar & 0xFF00) ? '.' : (uint8_t)unicodechar;
}

void
pet2ascii(char *petstring)
{
assert(petstring != NULL);

for (; *petstring != 0; petstring++)
*petstring = pet2ascii(*petstring);
}

uint8_t
ascii2pet(uint8_t asciichar)
{
    if (asciichar == 0x00)
        return 0x00;
    
    asciichar = toupper(asciichar);
    
    if (asciichar >= 0x20 && asciichar <= 0x5D) {
        return asciichar;
    } else {
        return ' ';
    }
}

void
ascii2pet(char *asciistring)
{
    assert(asciistring != NULL);
    
    for (; *asciistring != 0; asciistring++)
        *asciistring = ascii2pet(*asciistring);
}

void
binary8_to_string(uint8_t value, char *s)
{
	unsigned i;
	for (i = 0; i < 8; i++) {
		s[7-i] = (value & (1 << i)) ? '1' : '0';
	}
	s[i] = 0;
}

void
binary32_to_string(uint32_t value, char *s)
{
    unsigned i;
    for (i = 0; i < 32; i++) {
        s[31-i] = (value & (1 << i)) ? '1' : '0';
    }
    s[i] = 0;
}

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

long
getSizeOfFile(const char *filename)
{
    struct stat fileProperties;
    
    if (filename == NULL)
        return -1;
    
    if (stat(filename, &fileProperties) != 0)
        return -1;
    
    return fileProperties.st_size;
}

bool
checkFileSize(const char *filename, long min, long max)
{
    long filesize = getSizeOfFile(filename);
    
    if (filesize == -1)
        return false;
    
	if (min > 0 && filesize < min)
		return false;

	if (max > 0 && filesize > max)
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

//! Returns elepased time since application start in microseconds
uint64_t 
usec()
{
	struct timeval t;
	gettimeofday(&t,NULL);	
	return (uint64_t)1000000*(uint64_t)(t.tv_sec - tv_base) + (uint64_t)t.tv_usec;
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
sleepMicrosec(unsigned usec)
{		
	if (usec > 0 && usec < 1000000) {
		usleep(usec);
	}
}

int64_t
sleepUntil(uint64_t kernelTargetTime, uint64_t kernelEarlyWakeup)
{
    uint64_t now = mach_absolute_time();
    int64_t jitter;
    
    if (now > kernelTargetTime)
        return 0;
    
    // Sleep
    // printf("Sleeping for %d\n", kernelTargetTime - now);
    mach_wait_until(kernelTargetTime - kernelEarlyWakeup);
    
    // Count some sheep to increase precision
    unsigned sheep = 0;
    do {
        jitter = mach_absolute_time() - kernelTargetTime;
        sheep++;
    } while (jitter < 0);
    
    return jitter;
}
