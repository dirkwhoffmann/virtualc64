/*!
 * @file        VC64Object.cpp
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   Dirk W. Hoffmann. All rights reserved.
 */
/*
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
