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

/* This file provides several macros for printing messages:
 *
 *   - msg    Information messages  (Show up in all builds)
 *   - warn   Warning messages      (Show up in all builds)
 *   - debug  Debug messages        (Show up in debug builds, only)
 *   - trace  Detailed debug output (Show up in debug builds, only)
 *
 * Trace messages are prefixed by a string description produced by the prefix()
 * function. By default, prefix() returns the component name. Some classes
 * overwrite this function to output additional debug information.
 *
 * Debug and trace messages are accompanied by an optional 'verbose' parameter.
 * If 0 is passed in, no output will be created.
 *
 * Sidenote: In previous versions the printing macros were implemented in form
 * of variadic functions. Although this might seem to be a superior approach at
 * first glance, it is not. Using macros allows modern compilers to verify the
 * format string placeholders against the data types of the provided arguments.
 * This check can't be performed when using variadic functions are utilized.
 */

#define msg(format, ...) \
fprintf(stderr, format, ##__VA_ARGS__);

#define warn(format, ...) \
fprintf(stderr, "Warning: "); \
fprintf(stderr, format, ##__VA_ARGS__);

#ifndef NDEBUG

#define debug(verbose, format, ...) \
if (verbose) { fprintf(stderr, format, ##__VA_ARGS__); }

#define trace(verbose, format, ...) \
if (verbose) { prefix(); fprintf(stderr, format, ##__VA_ARGS__); }

#else

#define debug(verbose, format, ...)
#define trace(verbose, format, ...)

#endif

#endif
