/*
 * Author: Dirk W. Hoffmann. All rights reserved.
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

#include "VC64Object.h"

VC64Object::VC64Object()
{
    debugLevel = defaultDebugLevel; 
    traceCounter = 0;
    silentTracing = false; 
    description = NULL;
}

VC64Object::~VC64Object()
{
    if (logfile)
        fclose(logfile);
}

unsigned VC64Object::defaultDebugLevel = 1;
FILE *VC64Object::logfile = NULL;
char VC64Object::traceBuffer[512][256];
unsigned VC64Object::tracePtr = 0;

// ---------------------------------------------------------------------------------------------
//                                       Tracing
// ---------------------------------------------------------------------------------------------

void
VC64Object::startTracing(int count) {
    silentTracing = false;
    traceCounter = count;
    for (int i = 0; i < 256; i++)
        strcpy(traceBuffer[i], "--\n");
}

void
VC64Object::startSilentTracing(int count) {
    silentTracing = true;
    traceCounter = count;
    for (int i = 0; i < 256; i++)
        strcpy(traceBuffer[i], "--\n");
}

void
VC64Object::stopTracing() {
    traceCounter = 0;
}

void
VC64Object::backtrace(int count) {
    
    assert(count < 256);
    
    debug("Backtrace:\n");
    unsigned base = 256 + tracePtr - count;
    for (unsigned i = 0; i < count; i++) {
        fprintf(stderr, "%d: %s", (base + i) % 256, traceBuffer[(base + i) % 256]);
    }
}


// ---------------------------------------------------------------------------------------------
//                                      Printing messages
// ---------------------------------------------------------------------------------------------

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
    fprintf(logfile ? logfile : stderr, "%s", buf);
}

void
VC64Object::msg(int level, const char *fmt, ...)
{
    if (level > debugLevel)
        return;
    
    VC64OBJ_PARSE;
    fprintf(logfile ? logfile : stderr, "%s", buf);
}

void
VC64Object::debug(const char *fmt, ...)
{
    VC64OBJ_PARSE;
    if (description)
        fprintf(logfile ? logfile : stderr, "%s: %s", description, buf);
    else
        fprintf(logfile ? logfile : stderr, "%s", buf);
}

void
VC64Object::debug(int level, const char *fmt, ...)
{
    if (level > debugLevel)
        return;
    
    VC64OBJ_PARSE;
    if (description)
        fprintf(logfile ? logfile : stderr, "%s: %s", description, buf);
    else
        fprintf(logfile ? logfile : stderr, "%s", buf);
}

void
VC64Object::warn(const char *fmt, ...)
{
    VC64OBJ_PARSE;
    if (description)
        fprintf(logfile ? logfile : stderr, "%s: WARNING: %s", description, buf);
    else
        fprintf(logfile ? logfile : stderr, "WARNING: %s", buf);
}

void
VC64Object::panic(const char *fmt, ...)
{
    VC64OBJ_PARSE;
    if (description)
        fprintf(logfile ? logfile : stderr, "%s: PANIC: %s", description, buf);
    else
        fprintf(logfile ? logfile : stderr, "PANIC: %s", buf);

    assert(0);
}

void
VC64Object::trace(const char *fmt, ...)
{
    if (traceCounter == 0)
        return;

    if (traceCounter > 0)
        traceCounter--;
    
    VC64OBJ_PARSE;
    if (description)
        sprintf(traceBuffer[tracePtr], "%s: %s", description, buf);
    else
        sprintf(traceBuffer[tracePtr], "%s", buf);

    if (!silentTracing) {
        fprintf(stderr, "%s", traceBuffer[tracePtr]);
    }

    tracePtr = (tracePtr < 255) ? tracePtr + 1 : 0;
}
