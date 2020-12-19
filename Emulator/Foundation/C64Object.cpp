// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "C64Object.h"

#define VC64OBJ_PARSE \
char buf[256]; \
va_list ap; \
va_start(ap, fmt); \
vsnprintf(buf, sizeof(buf), fmt, ap); \
va_end(ap);

#define VC64PRINTPLAIN(trailer) \
fprintf(stderr, "%s%s", trailer, buf);

#define VC64PRINT(trailer) \
prefix(); \
fprintf(stderr, "%s: %s%s", getDescription(), trailer, buf);

void
C64Object::prefix()
{
    fprintf(stderr, "%s: ", getDescription());
}
