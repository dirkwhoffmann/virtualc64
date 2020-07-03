// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "VC64Object.h"

bool
VC64Object::tracingEnabled()
{
    if (traceCounter == 0)
        return false;
    
    if (traceCounter > 0)
        traceCounter--;
    
    return true;
}

#define VC64OBJ_PARSE \
    char buf[256]; \
    va_list ap; \
    va_start(ap, fmt); \
    vsnprintf(buf, sizeof(buf), fmt, ap); \
    va_end(ap); 

void
VC64Object::msg(const char *fmt, ...)
{
    VC64OBJ_PARSE;
    fprintf(stderr, "%s", buf);
}

void
VC64Object::msg(int level, const char *fmt, ...)
{
    if (level > debugLevel)
        return;
    
    VC64OBJ_PARSE;
    fprintf(stderr, "%s", buf);
}

void
VC64Object::debug(const char *fmt, ...)
{
    VC64OBJ_PARSE;
    if (description)
        fprintf(stderr, "%s: %s", description, buf);
    else
        fprintf(stderr, "%s", buf);
}

void
VC64Object::debug(int level, const char *fmt, ...)
{
    if (level > debugLevel)
        return;
    
    VC64OBJ_PARSE;
    if (description)
        fprintf(stderr, "%s: %s", description, buf);
    else
        fprintf(stderr, "%s", buf);
}

void
VC64Object::warn(const char *fmt, ...)
{
    VC64OBJ_PARSE;
    if (description)
        fprintf(stderr, "%s: WARNING: %s", description, buf);
    else
        fprintf(stderr, "WARNING: %s", buf);
}

void
VC64Object::panic(const char *fmt, ...)
{
    VC64OBJ_PARSE;
    if (description)
        fprintf(stderr, "%s: PANIC: %s", description, buf);
    else
        fprintf(stderr, "PANIC: %s", buf);

    assert(0);
}
