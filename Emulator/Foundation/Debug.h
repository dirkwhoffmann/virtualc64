// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _DEBUG_H
#define _DEBUG_H

/* This file provides several macros for printing messages
 *
 *   - msg     Debug messages   (Shows up in all builds)
 *   - warn    Warning messages (Shows up in all builds)
 *   - panic   Error messages   (Shows up in all builds, terminates execution)
 *   - debug   Debug messages   (Shows up in debug builds, only)
 *
 * Debug messages are prefixed by the string description produced by the
 * prefix() function which returns an empty string by default. Classes may
 * implement this function to provide a detailed string.
 */

/*
#define debug(verbose, format, ...) \
if (verbose) { fprintf (stderr, format __VA_OPT__(,) __VA_ARGS__); }
*/

#ifndef NDEBUG

#define debug(verbose, format, ...) \
if (verbose) { fprintf (stderr, format, ##__VA_ARGS__); } \

#else

#define debug(verbose, format, ...)

#endif

#endif
