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
 *
 * Sidenote: Previous version of the emulator implemented the printing macros
 * in form of variadic functions. Although this might seem to be a cleaner
 * approach at first glance, it is not. Using macros allows modern compilers
 * to verify the format string placeholders against the data types of the
 * provided arguments. This check is not performed when using variadic
 * functions.
 */

#ifndef NDEBUG

inline void prefix() { }
inline const char *getDescription() { return ""; }

#define debug(verbose, format, ...) \
if (verbose) { \
fprintf(stderr, format, ##__VA_ARGS__); \
}

// prefix(); \
// fprintf(stderr, "%s: ", getDescription()); prefix();

#else

#define debug(verbose, format, ...)

#endif

#endif
