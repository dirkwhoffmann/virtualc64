// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "MemUtils.h"
#include "string.h"
#include "IOUtils.h"

namespace util {

bool isZero(const u8 *ptr, isize size)
{
    for (isize i = 0; i < size; i++) {
        if (ptr[i]) return false;
    }
    return true;
}

void replace(u8 *p, isize size, const u8 *sequence, const u8 *substitute)
{
	replace((char *)p, size, (char *)sequence, (char *)substitute);
}

void replace(char *p, isize size, const char *sequence, const char *substitute)
{
	assert(p);
	assert(sequence);
	assert(substitute);
	assert(strlen(sequence) == strlen(substitute));

	auto len = strlen(sequence);
		
	for (isize i = 0; i < size - isize(len); i++) {

		if (strncmp(p + i, sequence, len) == 0) {
			
			memcpy((void *)(p + i), (void *)substitute, len);
			return;
		}
	}
	assert(false);
}

void readAscii(const u8 *buf, isize len, char *result, char pad)
{
	assert(buf);
	assert(result);
	
	for (isize i = 0; i < len; i++) {
		
		result[i] = isprint(int(buf[i])) ? char(buf[i]) : pad;
	}
	result[len] = 0;
}


void hexdump(u8 *p, isize size, isize cols, isize pad)
{
	while (size) {
		
		isize cnt = std::min(size, cols);
		for (isize x = 0; x < cnt; x++) {
			fprintf(stderr, "%02X %s", p[x], ((x + 1) % pad) == 0 ? " " : "");
		}
		
		size -= cnt;
		p += cnt;
		
		fprintf(stderr, "\n");
	}
	fprintf(stderr, "\n");
}

void hexdump(u8 *p, isize size, isize cols)
{
	hexdump(p, size, cols, cols);
}

void hexdumpWords(u8 *p, isize size, isize cols)
{
	hexdump(p, size, cols, 2);
}

void hexdumpLongwords(u8 *p, isize size, isize cols)
{
	hexdump(p, size, cols, 4);
}

}
