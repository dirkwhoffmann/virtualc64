// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// This FILE is dual-licensed. You are free to choose between:
//
//     - The GNU General Public License v3 (or any later version)
//     - The Mozilla Public License v2
//
// SPDX-License-Identifier: GPL-3.0-or-later OR MPL-2.0
// -----------------------------------------------------------------------------

#pragma once

#include "Error.h"
#include "Dumpable.h"

namespace vc64 {

class CoreObject : public Dumpable {

protected:

    // Set to false to disable all debug messages
    static bool verbose;

    
    //
    // Initializing
    //
    
public:
    
    virtual ~CoreObject() = default;
    
    // Returns the name for this component
    virtual const char *objectName() const = 0;

    // Returns a textual description for this component
    virtual const char *description() const { return ""; }

    // Called by debug() and trace() to produce a detailed debug output
    virtual void prefix() const;
};

/* This file provides multiple macros for printing messages:
 *
 *   - msg    Information message   (Shows up in all builds)
 *   - warn   Warning message       (Shows up in all builds)
 *   - fatal  Error message + Exit  (Shows up in all builds)
 *   - debug  Debug message         (Shows up in debug builds only)
 *   - plain  Plain debug message   (Shows up in debug builds only)
 *   - trace  Detailed debug output (Shows up in debug builds only)
 *
 * Debug messages are prefixed by the component name and a line number. Trace
 * messages are prefixed by a more detailed string description produced by the
 * prefix() function.
 *
 * Debug, plain, and trace messages are accompanied by an optional 'enable'
 * parameter. If 0 is passed in, no output will be generated. In addition,
 * variable 'verbose' is checked which is set to true by default. By setting
 * this variable to false, debug output can be silenced temporarily.
 *
 * Sidenote: In previous releases the printing macros were implemented in form
 * of variadic functions. Although this might seem to be superior at first
 * glance, it is not. Using macros allows modern compilers to verify the format
 * strings against the data types of the provided arguments. This check can't
 * be performed when variadic functions are used.
 */

#define msg(format, ...) \
fprintf(stderr, format __VA_OPT__(,) __VA_ARGS__);

#define warn(format, ...) \
fprintf(stderr, "Warning: " format __VA_OPT__(,) __VA_ARGS__);

#define fatal(format, ...) \
{ fprintf(stderr, "Fatal: " format __VA_OPT__(,) __VA_ARGS__); exit(1); }

#define debug(enable, format, ...) \
if (enable) { if (verbose) { \
fprintf(stderr, "%s:%d " format, objectName(), __LINE__ __VA_OPT__(,) __VA_ARGS__); }}

#define plain(enable, format, ...) \
if (enable) { if (verbose) { \
fprintf(stderr, format __VA_OPT__(,) __VA_ARGS__); }}

#define trace(enable, format, ...) \
if (enable) { if (verbose) { \
prefix(); \
fprintf(stderr, "%s:%d " format, objectName(), __LINE__ __VA_OPT__(,) __VA_ARGS__); }}

#define xfiles(format, ...) \
if (XFILES) { if (verbose) { \
prefix(); \
fprintf(stderr, "XFILES: " format __VA_OPT__(,) __VA_ARGS__); }}

}
