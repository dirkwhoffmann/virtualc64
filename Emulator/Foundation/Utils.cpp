// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "Utils.h"

#include <ctype.h>

bool
releaseBuild()
{
#ifdef RELEASEBUILD
    return true;
#else
    return false;
#endif
}

struct timeval t;
// long tv_base = ((void)gettimeofday(&t,NULL), t.tv_sec);

void translateToUnicode(const char *petscii, u16 *unichars, u16 base, size_t max)
{
    assert(petscii != NULL);
    assert(unichars != NULL);
    
    unsigned i;
    size_t len = MIN(strlen(petscii), max);
    
    for (i = 0; i < len; i++) {
        unichars[i] = base + (u16)petscii2printable(petscii[i], ' ');
    }
    unichars[i] = 0;
}

size_t
strlen16(const u16 *unichars)
{
    size_t count = 0;
    
    if (unichars)
        while(unichars[count]) count++;
    
    return count;
}

u8
petscii2printable(u8 c, u8 subst)
{
    if (c >= 0x20 /*' '*/ && c <= 0x7E /* ~ */) return c;
    return subst;
}

u8
ascii2pet(u8 asciichar)
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
ascii2petStr(char *str)
{
    assert(str != NULL);
    for (; *str != 0; str++) {
        *str = ascii2pet(*str);
    }
}

void
sprint8d(char *s, u8 value)
{
    for (int i = 2; i >= 0; i--) {
        u8 digit = value % 10;
        s[i] = '0' + digit;
        value /= 10;
    }
    s[3] = 0;
}

void
sprint8x(char *s, u8 value)
{
    for (int i = 1; i >= 0; i--) {
        u8 digit = value % 16;
        s[i] = (digit <= 9) ? ('0' + digit) : ('A' + digit - 10);
        value /= 16;
    }
    s[2] = 0;
}

void
sprint8b(char *s, u8 value)
{
    for (int i = 7; i >= 0; i--) {
        s[i] = (value & 0x01) ? '1' : '0';
        value >>= 1;
    }
    s[8] = 0;
}

void
sprint16d(char *s, u16 value)
{
    for (int i = 4; i >= 0; i--) {
        u8 digit = value % 10;
        s[i] = '0' + digit;
        value /= 10;
    }
    s[5] = 0;
}

void
sprint16x(char *s, u16 value)
{
    for (int i = 3; i >= 0; i--) {
        u8 digit = value % 16;
        s[i] = (digit <= 9) ? ('0' + digit) : ('A' + digit - 10);
        value /= 16;
    }
    s[4] = 0;
}

void
sprint16b(char *s, u16 value)
{
    for (int i = 15; i >= 0; i--) {
        s[i] = (value & 0x01) ? '1' : '0';
        value >>= 1;
    }
    s[16] = 0;
}

char *
extractFilename(const char *path)
{
    assert(path != NULL);
    
    const char *pos = strrchr(path, '/');
    return pos ? strdup(pos + 1) : strdup(path);
}

char *
extractSuffix(const char *path)
{
    assert(path != NULL);
    
    const char *pos = strrchr(path, '.');
    return pos ? strdup(pos + 1) : strdup("");
}

char *
extractFilenameWithoutSuffix(const char *path)
{
    assert(path != NULL);
    
    char *result;
    char *filename = extractFilename(path);
    char *suffix   = extractSuffix(filename);
    
    if (strlen(suffix) == 0)
        result = strdup(filename);
    else
        result = strndup(filename, strlen(filename) - strlen(suffix) - 1);
    
    free(filename);
    free(suffix);
    return result;
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
    
    if (filesize == -1) {
        return false;
    }
    if (min > 0 && filesize < min) {
        return false;
    }
    if (max > 0 && filesize > max) {
        return false;
    }
    return true;
}

bool
matchingFileHeader(const char *path, const u8 *header, size_t length)
{
    assert(path != NULL);
    assert(header != NULL);
    
    bool result = true;
    FILE *file;
    
    if ((file = fopen(path, "r")) == NULL) {
        return false;
    }
    for (size_t i = 0; i < length; i++) {
        int c = fgetc(file);
        if (c != (int)header[i]) {
            result = false;
            break;
        }
    }

    fclose(file);
    return result;
}

bool
matchingBufferHeader(const u8 *buffer, const u8 *header, size_t length)
{
    assert(buffer != NULL);
    assert(header != NULL);

    for (size_t i = 0; i < length; i++) {
        if (header[i] != buffer[i])
        return false;
    }

    return true;
}

u32
fnv_1a_32(u8 *addr, size_t size)
{
    if (addr == NULL || size == 0) return 0;

    u32 hash = fnv_1a_init32();

    for (size_t i = 0; i < size; i++) {
        hash = fnv_1a_it32(hash, (u32)addr[i]);
    }

    return hash;
}

u64
fnv_1a_64(u8 *addr, size_t size)
{
    if (addr == NULL || size == 0) return 0;

    u64 hash = fnv_1a_init64();

    for (size_t i = 0; i < size; i++) {
        hash = fnv_1a_it64(hash, (u64)addr[i]);
    }

    return hash;
}

u32
crc32(const u8 *addr, size_t size)
{
    if (addr == NULL || size == 0) return 0;

    u32 result = 0;

    // Setup lookup table
    u32 table[256];
    for(int i = 0; i < 256; i++) table[i] = crc32forByte(i);

    // Compute CRC-32 checksum
     for(size_t i = 0; i < size; i++)
       result = table[(u8)result ^ addr[i]] ^ result >> 8;

    return result;
}

u32
crc32forByte(u32 r)
{
    for(int j = 0; j < 8; ++j)
        r = (r & 1? 0: (u32)0xEDB88320L) ^ r >> 1;
    return r ^ (u32)0xFF000000L;
}
